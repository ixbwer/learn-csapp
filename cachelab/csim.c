#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <getopt.h>
#include "cachelab.h"
#include "time.h"
#include "sys/time.h"
/**
 * |*** tag ***| *** set_index ***| *** offset *** |
 *                      s bit           b bit
 * 
 * 
 * 
 */


int S, s, B, b, E;

int miss_count,
    hit_count,
    eviction_count;

typedef struct block_info
{
    int set_index;
    int valid_bit;
    int timestamp;
}BLOCK_INFO;
BLOCK_INFO** _cache;

void print_usage(void)
{
    printf(""
    "-h: Optional help flag that prints usage info\n"
    "-v: Optional verbose flag that displays trace info\n"
    "-s <s>: Number of set index bits (S = 2^s is the number of sets)\n"
    "-E <E>: Associativity (number of lines per set)\n"
    "-b <b>: Number of block bits (B = 2^b is the block size)\n"
    "-t <tracefile>: Name of the valgrind trace to replay\n");
}


/**
 * @brief 
 *          
 * 
 * @param addr 
 * @param size 
 */
int update_cache(int addr, int size)
{
    int addr_setindex = (addr >> b) & ((1 << b) - 1);
    struct timespec time;
    clock_gettime(CLOCK_REALTIME, &time);
    long int timestamp = time.tv_sec * 1000000000 + time.tv_nsec;
    for( int i = 0; i < S; i ++)
    {
        int j;
        for(j = 0 ; j < E ; j ++)
        {
            if(_cache[i][j].valid_bit == 1)
            {
                //timestamp cal
                if(_cache[i][j].set_index == addr_setindex) //hit
                {
                    printf("hit\n");
                    return 0;
                }
            }
            else{   //blank block, miss, udpate current block with timestamp and set_index
                _cache[i][j].timestamp = timestamp;
                _cache[i][j].valid_bit = 1;
                _cache[i][j].set_index = addr_setindex;
            }
        }
        if(j == E) // need eviction, update timestamp and set_index at the former least recent user block
        {
            
        }
    }
}


void parse_file(FILE* fd)
{
    char command;
    int address, size;
    while(1)
    {
        fscanf(fd, "%c %d/,%d", &command, &address, &size);
    }
    switch(command)
    {
        case 'L':
        {
            update_cache(address, size);
        }
        case 'S':
        {
            update_cache(address, size);
        }
        case 'M':
        {
            update_cache(address, size);
            hit_count ++;
        }

    }
}


void init_cache()
{
    _cache = malloc(sizeof(BLOCK_INFO) * S * E);//in total, S set, E ways per set
    if(!_cache)
    {
        printf("cache malloc failed");
        exit(1);
    }
    for( int i = 0; i < S; i ++)
    {
        for(int j = 0; j < E ; j ++)
        {
            _cache[i][j].set_index = -1;
            _cache[i][i].valid_bit = 0;
            _cache[i][j].timestamp = -1;
        }
    }
}




int main(int argc, char* argv[])
{
    int ch;
    FILE* fd;
    while ((ch = getopt(argc, argv, "hv:s:E:b:")) != -1)
    {
        switch(ch)
        {
        case 'h':
        {
            print_usage();
            break;
        }
        case 'v':
        {
            //display_trace();
            break;
        }
        case 's':
        {
            s = atoi(optarg);
            break;
        }
        case 'E':
        {   
            s = atoi(optarg);
            break;
        }
        case 'b':
        {
            s = atoi(optarg);
            break;
        }
        case 't':
        {
            fd = fopen(optarg,"rb");
        }
        }
    }
    S = 1 << s; //S = 2^s
    B = 1 << b; //B = 2^s
    init_cache();
    parse_file(fd); 
    printSummary(0, 0, 0);
    return 0;
}
