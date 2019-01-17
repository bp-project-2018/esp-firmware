#include <Crypto.h>

#include "Datagram.h"

#define DATAGRAM_ADDRESS_LENGTH_SIZE 1
#define DATAGRAM_TIMESTAMP_SIZE 8
#define DATAGRAM_MAC_SIZE SHA256_SIZE

#define AES_BLOCK_SIZE 16

void generate_random_bytes(byte* array, int array_length) {
	RNG::fill(array, array_length);
}

byte* write_timestamp(byte* buffer, int64_t timestamp) {
	*buffer++ = byte(timestamp >> 56);
	*buffer++ = byte(timestamp >> 48);
	*buffer++ = byte(timestamp >> 40);
	*buffer++ = byte(timestamp >> 32);
	*buffer++ = byte(timestamp >> 24);
	*buffer++ = byte(timestamp >> 16);
	*buffer++ = byte(timestamp >>  8);
	*buffer++ = byte(timestamp >>  0);
	return buffer;
}

byte* write_mac(byte* buffer, byte* result, const char* passphrase) {
	SHA256HMAC hmac((const byte*) passphrase, strlen(passphrase));
	hmac.doUpdate(result, buffer-result);
	hmac.doFinal(buffer);
	buffer += DATAGRAM_MAC_SIZE;
	return buffer;
}

int assemble_time_request(byte* result, int result_length, const char* address, const byte* nonce, const char* passphrase) {
	const int address_length = strlen(address);
	const int total_length = DATAGRAM_ADDRESS_LENGTH_SIZE + address_length + DATAGRAM_NONCE_SIZE + DATAGRAM_MAC_SIZE;

	if (total_length > result_length) return 0;

	byte* buffer = result;

	*buffer++ = byte(address_length);

	memcpy(buffer, address, address_length);
	buffer += address_length;

	memcpy(buffer, nonce, DATAGRAM_NONCE_SIZE);
	buffer += DATAGRAM_NONCE_SIZE;

	buffer = write_mac(buffer, result, passphrase);

	return buffer - result;
}

int assemble_datagram(byte* result, int result_length, const char* address, const byte* iv, int64_t timestamp, const byte* data, int data_length, const byte* key, const char* passphrase) {
	const int address_length = strlen(address);
	const int aes_content_length = DATAGRAM_TIMESTAMP_SIZE + data_length;
	const int padding_length = AES_BLOCK_SIZE - aes_content_length % AES_BLOCK_SIZE;
	const int aes_length = aes_content_length + padding_length;
	const int total_length = DATAGRAM_ADDRESS_LENGTH_SIZE + address_length + DATAGRAM_IV_SIZE + aes_length + DATAGRAM_MAC_SIZE;

	if (total_length > result_length) return 0;

	byte* buffer = result;

	*buffer++ = address_length;

	memcpy(buffer, address, address_length);
	buffer += address_length;

	memcpy(buffer, iv, DATAGRAM_IV_SIZE);
	buffer += DATAGRAM_IV_SIZE;

	byte* aes_start = buffer;

	buffer = write_timestamp(buffer, timestamp);

	memcpy(buffer, data, data_length);
	buffer += data_length;

	// Add PKCS#7 padding.
	for (int i = 0; i < padding_length; i++) {
		*buffer++ = byte(padding_length);
	}

	{ // Encrypt buffer.
		AES encryptor(key, iv, AES::AES_MODE_128, AES::CIPHER_ENCRYPT);
		encryptor.processNoPad(aes_start, aes_start, aes_length);
	}

	buffer = write_mac(buffer, result, passphrase);

	return buffer - result;
}

bool extract_address(const byte* message, int message_length, char* address_out) {
	if (message_length == 0) return false;
	byte length = message[0];
	if (message_length < DATAGRAM_ADDRESS_LENGTH_SIZE + length) return false;
	memcpy(address_out, message+DATAGRAM_ADDRESS_LENGTH_SIZE, length);
	address_out[length] = '\0';
	return true;
}

bool check_mac(const byte* message, int message_length, const char* passphrase) {
	byte expected[DATAGRAM_MAC_SIZE];
	if (message_length < DATAGRAM_MAC_SIZE) return false;
	SHA256HMAC hmac((const byte*) passphrase, strlen(passphrase));
	hmac.doUpdate(message, message_length-DATAGRAM_MAC_SIZE);
	hmac.doFinal(expected);
	return memcmp(message+message_length-DATAGRAM_MAC_SIZE, expected, DATAGRAM_MAC_SIZE) == 0;
}

int64_t decode_timestamp(const byte* data) {
	int64_t timestamp = 0;
	timestamp += int64_t(data[0]) << 56;
	timestamp += int64_t(data[1]) << 48;
	timestamp += int64_t(data[2]) << 40;
	timestamp += int64_t(data[3]) << 32;
	timestamp += int64_t(data[4]) << 24;
	timestamp += int64_t(data[5]) << 16;
	timestamp += int64_t(data[6]) <<  8;
	timestamp += int64_t(data[7]) <<  0;
	return timestamp;
}

bool disassemble_time_response(const byte* message, int message_length, const char* address, const char* passphrase, int64_t* timestamp_out, byte* nonce_out) {
	if (!check_mac(message, message_length, passphrase)) return false;
	if (message_length != DATAGRAM_ADDRESS_LENGTH_SIZE + strlen(address) + DATAGRAM_TIMESTAMP_SIZE + DATAGRAM_NONCE_SIZE + DATAGRAM_MAC_SIZE) return false;

	message += DATAGRAM_ADDRESS_LENGTH_SIZE + strlen(address);
	*timestamp_out = decode_timestamp(message);
	message += DATAGRAM_TIMESTAMP_SIZE;
	memcpy(nonce_out, message, DATAGRAM_NONCE_SIZE);
	message += DATAGRAM_NONCE_SIZE;
	return true;
}

bool disassemble_datagram(byte* datagram, int datagram_length, const char* address, const byte* key, const char* passphrase, int64_t* timestamp_out, byte* data_out, int* data_length_out) {
	if (!check_mac(datagram, datagram_length, passphrase)) return false;

	byte* iv_start  = datagram + DATAGRAM_ADDRESS_LENGTH_SIZE + strlen(address);
	byte* aes_start = iv_start + DATAGRAM_IV_SIZE;
	byte* aes_end   = datagram + datagram_length - DATAGRAM_MAC_SIZE;

	const int aes_length = aes_end - aes_start;
	if (aes_length < AES_BLOCK_SIZE || (aes_length % AES_BLOCK_SIZE) != 0) return false;

	{ // Decrypt data.
		AES decryptor(key, iv_start, AES::AES_MODE_128, AES::CIPHER_DECRYPT);
		decryptor.processNoPad(aes_start, aes_start, aes_length);
	}

	const int padding = aes_start[aes_length-1];
	if (padding > AES_BLOCK_SIZE) return false;
	for (int i = 0; i < padding; i++) {
		if (aes_start[aes_length-i-1] != padding) {
			return false;
		}
	}

	const int data_length = aes_length - padding - DATAGRAM_TIMESTAMP_SIZE;
	if (data_length < 0) return false;

	*timestamp_out = decode_timestamp(aes_start);
	memcpy(data_out, aes_start+DATAGRAM_TIMESTAMP_SIZE, data_length);
	*data_length_out = data_length;

	return true;
}
