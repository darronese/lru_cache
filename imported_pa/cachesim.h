#ifndef CACHESIM_H
#define CACHESIM_H
#include <stdio.h>
#include <stdlib.h>
#include <getopt.h>
#include <unistd.h>
#include <stdio.h>
#include <math.h>
#include <errno.h>

typedef struct {
  int valid;
  unsigned long long tag;
  int lruCounter;
} Line;

typedef struct {
  Line* lines;
} Set;

typedef struct {
  Set* sets;
  int s, E, b;
  int hits, misses, evictions;
  unsigned tick;
  // CAN ONLY BE 0 OR 1
  int verbose;
} Cache;

void print_usage(char* argv[]);
void print_summary(int hits, int misses, int evictions);

// my functions
Cache* createCache(int setBits, int numLines, int blockBits, int verbose);
void deleteCache(Cache* cache);
void run(Cache* c, FILE* traceFile); 
#endif
