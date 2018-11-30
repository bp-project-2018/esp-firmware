#include "Datagram.h"

Datagram::Datagram() {
  
}

bool Datagram::decode(char* payload) {
  isValid = false;
  if(payload[0] == '\0') return false; //unexpected termination

  //datagram type
  if(payload[0] == 'M') type = TYPE_MSG;
  else if(payload[0] == 'C') type = TYPE_CMD;
  else return false;
  payload++;

  if(payload[0] == '\0') return false; //unexpected termination

  if(payload[0] != 0) return false; //higher version which we do not support yet
  payload++;

  if(payload[0] == '\0') return false; //unexpected termination

  
}

void Datagram::encode(Datagram_type_t type) {
  
}
