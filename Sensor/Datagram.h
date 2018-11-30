#ifndef Datagram_h
#define Datagram_h

#include "Arduino.h"

#define DATAGRAM_ADDRESS_LENGTH 256

class Datagram {
  public:
    typedef enum {
        TYPE_MSG,
        TYPE_CMD
    } Datagram_type_t;
    
    Datagram();
    bool decode(char* payload);
    void encode(Datagram_type_t type);
    Datagram_type_t type;

    char sourceAddress[DATAGRAM_ADDRESS_LENGTH];
    char targetAddress[DATAGRAM_ADDRESS_LENGTH];
    
    bool isValid;
  private:
    char* _payload;
};

#endif
