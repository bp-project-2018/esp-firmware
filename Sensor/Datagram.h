#ifndef Datagram_h
#define Datagram_h

#include "Arduino.h"

#define DATAGRAM_MAX_ADDRESS_SIZE 255

#define DATAGRAM_IV_SIZE 16
#define DATAGRAM_KEY_SIZE 16
#define DATAGRAM_NONCE_SIZE 8

void generate_random_bytes(byte* array, int array_length);

// Assembles the time request and stores the result in buffer returning the actual length of the request.
// Returns 0 if the buffer is too small.
int assemble_time_request(byte* result, int result_length, const char* address, const byte* nonce, const char* passphrase);

// Assembles the datagram and stores the result in buffer returning the actual length of the datagram.
// Returns 0 if the buffer is too small.
int assemble_datagram(byte* result, int result_length, const char* address, const byte* iv, int64_t timestamp, const byte* data, int data_length, const byte* key, const char* passphrase);

bool extract_address(const byte* message, int message_length, char* address_out);
bool disassemble_time_response(const byte* message, int message_length, const char* address, const char* passphrase, int64_t* timestamp_out, byte* nonce_out);
bool disassemble_datagram(byte* datagram, int datagram_length, const char* address, const byte* key, const char* passphrase, int64_t* timestamp_out, byte** data_out, int* data_length_out);

#endif
