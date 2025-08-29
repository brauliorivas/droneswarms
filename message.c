#include "message.h"

Message build_message(MessageType message_type) {
  Message m;
  m = message_type;
  m = m << MESSAGE_SHIFT;
  return m;
}

MessageType get_message_type(Message message) {
  MessageType type;

  type = message >> MESSAGE_SHIFT;

  return type;
}
