#include <getopt.h>
#include <stdlib.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>

typedef struct {
  int valid;
  unsigned long long tag;
  int lruCounter;
} Line;

void print_usage(char* argv[]);
void print_summary(int hits, int misses, int evictions);
