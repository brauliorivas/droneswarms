#ifndef __MESSAGE_H__
#define __MESSAGE_H__

#ifdef ERR_VERBOSE
#define print_error(namespace, msg) fprintf(stderr, "[%s] %s\n", namespace, msg)

#define vprint_error(namespace, fmt, ...)                                      \
  fprintf(stderr, "[%s] " fmt "\n", namespace, ##__VA_ARGS__)
#else
#define print_error(namespace, msg)                                            \
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
#endif

#endif // !__MESSAGE_H__
