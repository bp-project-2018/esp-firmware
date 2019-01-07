#ifndef Datagram_h
#define Datagram_h

#include "Arduino.h"

#define DATAGRAM_ADDRESS_LENGTH 256
#define AES_BLOCK_SIZE 16

class Datagram {
  public:
    enum Datagram_Type {
        MESSAGE,
        COMMAND,
    };

    enum Payload_Encoding {
        BINARY,
        JSON,
        UTF8,
    };

  public:
    bool decode(byte* data);
    void encode(byte** result_out, int* result_length_out);

    void generate_random_iv();

  public:
    Datagram_Type type;
    int version;
    Payload_Encoding encoding;

    char address[DATAGRAM_ADDRESS_LENGTH];

    byte fixed[16];
    int fixedLength;
    byte* payload;
    int payloadLength;

    byte iv[AES_BLOCK_SIZE];
};

#endif
