#ifndef Message_h
#define Message_h

#include "Arduino.h"

class Message {
  public:
    Message();
  private:
    char* _payload;
};

#endif
