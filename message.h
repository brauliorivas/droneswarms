#ifndef __ERROR_H__
#define __ERROR_H__

#ifdef ERR_VERBOSE
    #define print_error(namespace, msg) \
        fprintf(stderr, "[%s] %s\n", namespace, msg)
#else
    #define print_error(namespace, msg) \
        do {} while (0)
#endif


#ifdef LOG_VERBOSE
    #define print_log(namespace, msg) \
        fprintf(stdout, "[%s] %s\n", namespace, msg)
#else
    #define print_log(namespace, msg) \
        do {} while (0)
#endif

#endif // !__ERROR_H__
