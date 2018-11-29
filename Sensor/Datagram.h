#ifndef Datagram_h
#define Datagram_h

#include "Arduino.h"
#include "libraries/arduino-crypto/Crypto.h"

class Datagram {
  public:
    Datagram();
    void decode();
    char* type;
    bool isValid;
  private:
    char* _payload;
};

#endif
