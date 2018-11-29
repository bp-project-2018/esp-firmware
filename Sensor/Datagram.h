#ifndef Datagram_h
#define Datagram_h

#include "Arduino.h"
#include "libraries/arduino-crypto/Crypto.h"

class Datagram {
  public:
    typedef enum {
        MSG = 77,
        CMD = 67
    } Datagram_type_t;
    
    Datagram();
    bool decode(char* payload);
    void encode(char* type);
    Datagram_type_t type;
    bool isValid;
  private:
    char* _payload;
};

#endif
