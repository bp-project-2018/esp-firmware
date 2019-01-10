#include <Crypto.h>

#include "Datagram.h"

const char* const TIME_SERVER_PASSPHRASE = "Ein Idiot in Uniform ist immer noch ein Idiot.";

const byte AES_KEY[AES_BLOCK_SIZE] = {0xe8, 0xc4, 0xe4, 0x83, 0x0a, 0x9c, 0xfc, 0x1b, 0x05, 0xbe, 0xd9, 0xa2, 0xf5, 0x59, 0x08, 0x10};
const char* const HMAC_PASSPHRASE = "Unfortunately, there's a radio connected to my brain.";

bool decode_time(byte* datagram, int datagram_length, int32_t* timestamp_out) {
  if (datagram_length != 2+4+SHA256_SIZE /* length, payload, mac */) {
    Serial.println("decode_time: Datagram has wrong length.");
    return false;
  }

  { // Validate provided length.
    int high = int(datagram[0]);
    int low = int(datagram[1]);
    int length = (high << 8) + low;
    if (length != 4) {
      Serial.println("decode_time: Length is wrong.");
      return false;
    }
  }

  { // Validate MAC.
    byte expected[SHA256_SIZE];
    SHA256HMAC hmac((const byte*) TIME_SERVER_PASSPHRASE, strlen(TIME_SERVER_PASSPHRASE));
    hmac.doUpdate(datagram + 2, 4);
    hmac.doFinal(expected);
    if (memcmp(datagram+2+4, expected, SHA256_SIZE) != 0) {
      Serial.println("decode_time: MAC is wrong.");
      return false;
    }
  }

  int32_t timestamp = 0;
  timestamp += int32_t(datagram[2]) << 24;
  timestamp += int32_t(datagram[3]) << 16;
  timestamp += int32_t(datagram[4]) <<  8;
  timestamp += int32_t(datagram[5]) <<  0;

  *timestamp_out = timestamp;
  return true;
}

bool Datagram::decode(byte* data) {
  return false;
  // isValid = false;
  // if(payload[0] == '\0') return false; //unexpected termination

  // //datagram type
  // if(payload[0] == 'M') type = TYPE_MSG;
  // else if(payload[0] == 'C') type = TYPE_CMD;
  // else return false;
  // payload++;

  // if(payload[0] == '\0') return false; //unexpected termination

  // if(payload[0] != 0) return false; //higher version which we do not support yet
  // payload++;

  // if(payload[0] == '\0') return false; //unexpected termination
}

void Datagram::encode(byte** result_out, int* result_length_out) {
  int addressLength = strlen(address);
  int headerLength = (3 + 1 + addressLength); // 3 bytes for the format, 1 byte for the length of the address and the length of the address itself.
  int aesContentLength = headerLength + fixedLength + payloadLength;
  int paddingLength = AES_BLOCK_SIZE - aesContentLength % AES_BLOCK_SIZE;
  int aesLength = aesContentLength + paddingLength;
  int hmacLength = AES_BLOCK_SIZE + 2 + aesLength; // iv + size + content
  int length = headerLength + 2 + hmacLength + SHA256_SIZE; // header + size + content + mac

  byte* result = (byte*) malloc(length);
  if (!result) {
    *result_out = nullptr;
    *result_length_out = 0;
    return;
  }

  byte* pos = result;
  byte* hmacStart = nullptr;
  byte* aesStart = nullptr;

  { // Header.
    switch (type) {
      case Datagram_Type::MESSAGE:
        *pos++ = 'M';
        break;
      case Datagram_Type::COMMAND:
        *pos++ = 'C';
        break;
      default:
        free(result);
        *result_out = nullptr;
        *result_length_out = 0;
        return;
    }

    // Version.
    *pos++ = '0';

    switch (encoding) {
      case Payload_Encoding::BINARY:
        *pos++ = 'B';
        break;
      case Payload_Encoding::JSON:
        *pos++ = 'J';
        break;
      case Payload_Encoding::UTF8:
        *pos++ = 'U';
        break;
      default:
        free(result);
        *result_out = nullptr;
        *result_length_out = 0;
        return;
    }

    *pos++ = addressLength;
    memcpy(pos, address, addressLength);
    pos += addressLength;
  }

  {
    // HMAC length.
    *pos++ = byte(hmacLength >> 8);
    *pos++ = byte(hmacLength >> 0);

    hmacStart = pos;

    // IV.
    memcpy(pos, iv, AES_BLOCK_SIZE);
    pos += AES_BLOCK_SIZE;

    // AES length.
    *pos++ = byte(aesLength >> 8);
    *pos++ = byte(aesLength >> 0);

    aesStart = pos;

    // Header copy.
    memcpy(pos, result, headerLength);
    pos += headerLength;

    // Fixed payload.
    memcpy(pos, fixed, fixedLength);
    pos += fixedLength;

    // Variable payload.
    memcpy(pos, payload, payloadLength);
    pos += payloadLength;

    // Padding.
    for (int i = 0; i < paddingLength; i++) {
      *pos++ = byte(paddingLength);
    }
  }

  { // Encrypt AES part.
    AES aesEncryptor(AES_KEY, iv, AES::AES_MODE_128, AES::CIPHER_ENCRYPT);
    aesEncryptor.processNoPad(aesStart, aesStart, aesLength);
  }

  { // Calculate HMAC.
    SHA256HMAC hmac((const byte*) HMAC_PASSPHRASE, strlen(HMAC_PASSPHRASE));
    hmac.doUpdate(hmacStart, hmacLength);
    hmac.doFinal(pos);
  }

  *result_out = result;
  *result_length_out = length;
  return;
}

void Datagram::generate_random_iv() {
  RNG::fill(iv, AES_BLOCK_SIZE);
}
