#include <getopt.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/time.h>
#include <unistd.h>

#include "cachelab.h"
#include "string.h"

#if __STDC_VERSION__ >= 199901L
#define _XOPEN_SOURCE 600
#else
#define _XOPEN_SOURCE 500
#endif /* __STDC_VERSION__ */
/**
 * |*** tag ***| *** set_index ***| *** offset *** |
 *                      s bit           b bit
 *
 *
 *
 */

#define CACHE_HIT 1 << 0
#define CACHE_MISS 1 << 1
#define CACHE_EVICTION 1 << 2

int S, s, B, b, E, v;

int miss_count, hit_count, eviction_count;

typedef struct block_info {
  int tag;
  int valid_bit;
  int timestamp;
} BLOCK_INFO;
BLOCK_INFO **_cache;

void print_usage(void) {
  printf(""
         "-h: Optional help flag that prints usage info\n"
         "-v: Optional verbose flag that displays trace info\n"
         "-s <s>: Number of set index bits (S = 2^s is the number of sets)\n"
         "-E <E>: Associativity (number of lines per set)\n"
         "-b <b>: Number of block bits (B = 2^b is the block size)\n"
         "-t <tracefile>: Name of the valgrind trace to replay\n");
}

void update_timestamp() {
  for (int i = 0; i < S; i++) {
    for (int j = 0; j < E; j++) {
      _cache[i][j].timestamp++;
    }
  }
}

int update_cache(int addr, int size) {
  int addr_setindex = (addr >> b) & ((1 << s) - 1);
  int lru_y = 0, max_timestamp = 0;
  int tag_addr = addr >> (b + s);
  int i = 0;
  for (i = 0; i < E; i++) {
    printf("set index %d line %d ", addr_setindex, i);
    if (_cache[addr_setindex][i].valid_bit == 1) { //有数据
      if (max_timestamp < _cache[addr_setindex][i].timestamp) {
        max_timestamp = _cache[addr_setindex][i].timestamp;
        lru_y = i;
      }
      if (_cache[addr_setindex][i].tag == tag_addr) { // hit
        printf("hit\n");
        _cache[addr_setindex][i].timestamp = 0;
        hit_count++;
        return CACHE_HIT;
      } else { //有数据但是未hit
        if (i ==
            (E - 1)) { //如果有数据且是最后一个，并且tag不符合，那么eviction
          printf("eviction");
          _cache[addr_setindex][lru_y].timestamp = 0;
          _cache[addr_setindex][lru_y].valid_bit = 1;
          _cache[addr_setindex][lru_y].tag = tag_addr;
          miss_count++;
          eviction_count++;
          return CACHE_EVICTION;
        }
      }
    } else { //有空行
      _cache[addr_setindex][i].timestamp = 0;
      _cache[addr_setindex][i].valid_bit = 1;
      _cache[addr_setindex][i].tag = tag_addr;
      printf(" miss and available %d\n", i);
      miss_count++;
      return CACHE_MISS;
    }
  }
  return 0;
}

void parse_file(FILE *fd) {
  char command;
  int address, size, ret;
  if (fd == NULL) {
    printf("open file error\n");
    exit(1);
  }
  while (1) {
    ret = fscanf(fd, " %c %x,%d", &command, &address, &size);
    if (ret == -1) {
      printf("match error\n");
      break;
    } else {
      printf("%c %x,%d (ret %d) ", command, address, size, ret);
    }
    switch (command) {
    case 'L': {
      printf(" op: L ");
      update_cache(address, size);
      break;
    }
    case 'S': {
      printf(" op: S ");
      if (CACHE_MISS == update_cache(address, size)) {
        hit_count++;
      }
      break;
    }
    case 'M': {
      printf(" op: M ");
      update_cache(address, size);
      hit_count++;
      break;
    }
    default:
      break;
    }
  }
}

void init_cache() {
  _cache = (BLOCK_INFO **)malloc(sizeof(BLOCK_INFO *) * S);
  for (int i = 0; i < S; i++) {
    _cache[i] = (BLOCK_INFO *)malloc(sizeof(BLOCK_INFO) * E);
    for (int j = 0; j < E; j++) {
      _cache[i][j].tag = 0;
      _cache[i][i].valid_bit = 0;
      _cache[i][j].timestamp = 0;
    }
  }
  printf("init done\n");
}

int main(int argc, char *argv[]) {
  int ch;
  FILE *fd;
  while ((ch = getopt(argc, argv, "hvs:E:b:t:")) != -1) {
    switch (ch) {
    case 'h': {
      print_usage();
      break;
    }
    case 'v': {
      v = 1;
      break;
    }
    case 's': {
      s = atoi(optarg);
      break;
    }
    case 'E': {
      E = atoi(optarg);
      break;
    }
    case 'b': {
      b = atoi(optarg);
      break;
    }
    case 't': {
      fd = fopen(optarg, "rb");
      // memcpy(file_name, optarg, 20);
    }
    default: {
      print_usage();
    }
    }
  }
  S = 1 << s; // S = 2^s
  B = 1 << b; // B = 2^s
  printf("S = %d s = %d E = %d  B = %d b = %d t = ", S, s, E, B, b);
  init_cache();
  parse_file(fd);
  printSummary(hit_count, miss_count, eviction_count);
  return 0;
}
