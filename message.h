#ifndef __MESSAGE_H__
#define __MESSAGE_H__

#include <stdint.h>
#ifdef ERR_VERBOSE
#define print_error(namespace, msg) fprintf(stderr, "[%s] %s\n", namespace, msg)

#define vprint_error(namespace, fmt, ...)                                      \
  fprintf(stderr, "[%s] " fmt "\n", namespace, ##__VA_ARGS__)
#else
#define print_error(namespace, msg)                                            \
  do {                                                                         \
  } while (0)
#define vprint_error(namespace, fmt, ...)                                      \
  do {                                                                         \
  } while (0)
#endif

#ifdef LOG_VERBOSE
#define print_log(namespace, msg) fprintf(stdout, "[%s] %s\n", namespace, msg)

#define vprint_log(namespace, fmt, ...)                                        \
  fprintf(stdout, "[%s] " fmt "\n", namespace, ##__VA_ARGS__)
#else
#define print_log(namespace, msg)                                              \
  do {                                                                         \
  } while (0)
#define vprint_log(namespace, fmt, ...)                                        \
  do {                                                                         \
  } while (0)
#endif

typedef uint64_t Message;
typedef uint8_t MessageType;

#define MESSAGE_SHIFT 56

#define CHECK 0x00

#define START_FLY 0x02
#define ENSEMBLE_COMPLETE 0x03
#define FLY 0x04
#define CHANGE_TARGET 0x01

/*
 * 64 bits to send
 * [_ _ _ _ _ ... _ _ _]
 *
 * First 8 bits
 * message type
 *
 * 00000000: check connection
 *
 * ----------------
 *
 * 00000010: start fly
 *
 * ----------------
 *
 * 00000011: ensemble complete
 *
 * ----------------
 *
 * 00000001: change target
 * bits 32-16: new x
 * bits 15-0: new y
 *
 * */

Message build_message(MessageType message_type);
MessageType get_message_type(Message message);

#endif // !__MESSAGE_H__
