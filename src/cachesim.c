// header library
#include "cachesim.h"

#define ADDRESS_LENGTH 64  // 64-bit memory addressing

/* 
 * this function provides a standard way for your cache
 * simulator to display its final statistics (i.e., hit and miss)
 */ 
void print_summary(int hits, int misses, int evictions)
{
  printf("hits:%d misses:%d evictions:%d\n", hits, misses, evictions);
}

/*
 * print usage info
 */
void print_usage(char* argv[])
{
  printf("Usage: %s [-hv] -s <num> -E <num> -b <num> -t <file>\n", argv[0]);
  printf("Options:\n");
  printf("  -h         Print this help message.\n");
  printf("  -v         Optional verbose flag.\n");
  printf("  -s <num>   Number of set index bits.\n");
  printf("  -E <num>   Number of lines per set.\n");
  printf("  -b <num>   Number of block offset bits.\n");
  printf("  -t <file>  Trace file.\n");
  printf("\nExamples:\n");
  printf("  linux>  %s -s 4 -E 1 -b 4 -t traces/trace01.dat\n", argv[0]);
  printf("  linux>  %s -v -s 8 -E 2 -b 4 -t traces/trace01.dat\n", argv[0]);
  exit(0);
}

// my functions
Cache* createCache(int setBits, int numLines, int blockBits, int verbose) {
  // calloc already intializes our properties
  Cache* myCache = calloc(1, sizeof(*myCache));
  if (!myCache) return NULL;
  myCache->s = setBits;
  myCache->E = numLines;
  myCache->b = blockBits;
  myCache->verbose = verbose;
  /* debug statement */
  printf("CREATED CACHE WITH %d s, %d E, %d b, with verbose set to ", setBits, numLines, blockBits);
  if (verbose) {
    printf("VERBOSE IS ENABLED\n");
  } else {
    printf("VERBOSE IS DISABLED\n");
  }
  /* CREATING SPACE FOR SETS */
  int myS = 1 << setBits;
  Set *newSet = calloc(myS, sizeof(Set));
  if (!newSet) return NULL;
  // iterate through every item in set
  for (int i = 0; i < myS; ++i) {
    newSet[i].lines = calloc(numLines, sizeof(Line)); 
    if (!newSet[i].lines) {
      // delete cache if successfully allocated previously
      for (int j = 0; j < i; ++j) {
        free(newSet[j].lines);
      }
      free(newSet);
      free(myCache);
      return NULL;
    }
  }
  // only set it to new set if not null
  myCache->sets = newSet;
  return myCache;
}

void deleteCache(Cache* cache) {
  // base:
  if (!cache) return;
  int myS = 1 << cache->s;
  // free each line first
  for (int i = 0; i < myS; ++i) {
    free(cache->sets[i].lines);
  }
  free(cache->sets);
  free(cache);
}

// helper function to simulate a single access at a cache
void accessCache(Cache* c, unsigned long long addy) {
  c->tick++;
  // calculate the index (FROM NOTES)
  size_t setIndex = (size_t)((addy >> c->b) & ((1ULL << c->s) - 1ULL));
  unsigned long long tag = addy >> (c->s + c->b);
  Set* set = &c->sets[setIndex];
  Line *lines = set->lines;

  // check if hit
  for (int i = 0; i < c->E; ++i) {
    // check for validity and matching tags
    if (lines[i].valid && lines[i].tag == tag) {
      c->hits++;
      lines[i].lruCounter = c->tick;
      if (c->verbose) printf(" hit");
      return;
    }
  }
  // MISSED
  c->misses++;
  if (c->verbose) printf(" miss");
  // look for the empty line and bring it into the cache
  for (int i = 0; i < c->E; ++i) {
    if (!lines[i].valid) {
      lines[i].valid = 1;
      lines[i].tag = tag;
      lines[i].lruCounter = c->tick;
      return;
    }
  }
  // if missed AND all the E lines are "full"
  // EVICT
  int victim = 0;
  int oldest = lines[0].lruCounter;
  for (int i = 1; i < c->E; ++i) {
    if (lines[i].lruCounter < oldest) {
      oldest = lines[i].lruCounter;
      victim = i;
    }
  }
  lines[victim].tag = tag;
  lines[victim].lruCounter = c->tick;
  c->evictions++;
  if (c->verbose) printf(" evicted");
}

void run(Cache* c, FILE* traceFile) {
  // open file and parse the contents
  char buf[100];
  while (fgets(buf, sizeof(buf), traceFile)) {
    char op;
    unsigned long long addy;
    int size;
    // all 3 has to be there
    if (sscanf(buf, " %c %llx,%d", &op, &addy, &size) != 3) {
      continue;
    }
    // skip instruction
    if (op == 'I') continue;

    switch (op) {
      case 'L':
      case 'S':
        accessCache(c, addy);
        if (c->verbose) printf("\n");
        break;
      case 'M':
        // load
        accessCache(c, addy);
        // store
        accessCache(c, addy);
        if (c->verbose) printf("\n");
        break;
    }
  }
  fclose(traceFile);
} 


/* Main */
int main(int argc, char* argv[]) {
  // complete your simulator
  // output cache hit and miss statistics
  // print_summary(hit_count, miss_count, eviction_count);
  // assignment done. life is good!
  int c;
  // parameters we store
  int s = -1, E = -1, b = -1, verbose = 0;
  char* fileName = NULL;
  while( (c=getopt(argc,argv,"s:E:b:t:vh")) != -1){
    switch(c){
      case 'h':
        print_usage(argv);
        exit(0);
      case 's':
        s = atoi(optarg);
        break;
      case 'E':
        E = atoi(optarg);
        break;
      case 'b':
        b = atoi(optarg);
        break;
      case 't':
        fileName = optarg;
        break;
      case 'v':
        verbose = 1;
        break;
      default:
        print_usage(argv);
        exit(1);
    }
  }

  if (verbose) {
    printf("Verbose mode activated\n");
  }
  // validate args
  if (s < 0 || E < 0 || b < 0 || fileName == NULL) {
    printf("INCORRECT PARAMETERS!");
    return -1;
  }
  FILE* traceFile = fopen(fileName, "r");
  if (!traceFile) {
    printf("Error opening file!");
    return 1;
  }
  Cache *cache = createCache(s, E, b, verbose);
  if (!cache) {
    printf("Error creating cache!");
    return 1;
  }
  // run once then delete
  run(cache, traceFile);
  print_summary(cache->hits, cache->misses, cache->evictions);
  deleteCache(cache);
  return 0;
}
