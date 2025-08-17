#ifndef __CONSTANTS_H__
#define __CONSTANTS_H__

typedef struct {
  int x; // targets
  int w; // % to kill drone
  int q; // % to lose communication
  int z; // seconds to assume lost
} Constants;

void parse_constans(char file[], Constants *Constants);

#endif
