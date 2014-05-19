/*
CS220 Spring 2014 Lab 4: Cache Lab
Part A: Simulating a Cache
Completed by Christopher Zhang (czhang44@binghamton.edu)
*/

#include "cachelab.h"
#include <getopt.h>
#include <stdlib.h>
#include <unistd.h>
#include <stdio.h>
#include <assert.h>
#include <math.h>
#include <limits.h>
#include <string.h>
#include <errno.h>

/* Type: Memory address*/
typedef unsigned long long int mem_addr_t;

/* Globals set by command line args */
int verbosity = 0; /* print trace if set */
int s = 0; /* set index bits */
int b = 0; /* block offset bits */
int E = 0; /* associativity */
char* trace_file = NULL;

/* Counters for hits, misses, and evictions. */
unsigned hit_count = 0;
unsigned miss_count = 0;
unsigned eviction_count = 0;

/*
The cache is a collection of 2^s sets. 
It will be set to newly allocated memory once s, b, and E are determined. 
Here's an example:

Cache
    [set 0][set 1][set 2][set 3][set 4]

    Each element contains pointers to arrays in turn.

In [set 0]

    [line 0][line 1][line 2] 

    Each element here is a pointer to a Line struct.
*/

// Masking will be used on the original memory address, followed by right-shifts
struct Line {
    // A valid bit, either 0 or 1
    char valid;
    // Tag bits 
    mem_addr_t tag; // Size determined by t = m - (s + b)
    // In theory, the cache would store the block as well but for the purpose
    // of the simulation in recording hits, misses, and evictions, it's 
    // actually unnecessary since it's the tag, set, and valid bits that matter.
    // A counter that increases as other lines in the same set are used
    unsigned LRUcounter;
};

// Line * - a pointer to a Line, an element inside a Set
// Line ** - a pointer to a Set, an element inside the Cache
// Line *** - a pointer to the Cache itself
struct Line *** cache;


/* 
 * buildCache - Allocate memory, write 0's for valid and tag and LRU
 * There are S sets
 * Every set has E ways
 * Every way has:
 ***a block of B bytes of data
 ***valid bit
 ***tag bits
 ***LRU counter to help with replacement for set assoc/assoc caches
 * You get the values for s, e, b etc.. from the command line options specified
     by the user
 */

void buildCache() {
    // Allocate an array of size 2^s comprised of sets (pointers to Lines)
    cache = (struct Line ***) malloc(pow(2, s) * sizeof(struct Line **));
    // For each of these pointers, allocate E lines
    for (int setIndex = 0; setIndex < pow(2, s); ++setIndex) {
        cache[setIndex] = (struct Line **) malloc(E * sizeof(struct Line *));
        for (int lineIndex = 0; lineIndex < E; ++lineIndex) {
            // For each of these lines, allocate a Line struct
            cache[setIndex][lineIndex] = 
                (struct Line *) malloc(sizeof(struct Line));
            // Initialize the valid bit, tag bits, and the LRU counter
            (cache[setIndex][lineIndex])->valid = 0;
            (cache[setIndex][lineIndex])->tag = 0;
            (cache[setIndex][lineIndex])->LRUcounter = 0;
        }
    }
}

/*
 * playTrace - replays the given trace file against the cache 
 */
void playTrace(char* trace_fn) {
    char buf[1000];
    mem_addr_t addr=0;
    unsigned int len=0;
    FILE* trace_fp = fopen(trace_fn, "r");
    if(!trace_fp) {
        fprintf(stderr, "%s: %s\n", trace_fn, strerror(errno));
        exit(1);
    }
    // Mask for the block bits
    // Necessary only to help make the tag mask. See earlier explanation why
    // explicitly having block data is unnecessary in this simulation
    mem_addr_t blockMask = pow(2,b) - 1;
    // Mask for the set index bits 
    mem_addr_t setMask = pow(2,s) - 1;
    setMask <<= (b);
    // Mask for the tag bits
    mem_addr_t tagMask = ~(blockMask | setMask);
    // Read the file lines
    while( fgets(buf, 1000, trace_fp) != NULL) { //get a line
        if(buf[1]=='S' || buf[1]=='L' || buf[1]=='M') {
            sscanf(buf+3, "%llx,%u", &addr, &len);
            if(verbosity)
                printf("%c %llx,%u\n", buf[1], addr, len);
    	    // In here you have to write code to simulate how your address 
            // accesses the cache based on the operation
            // Interpret the address as t tag bits, s set index bits, and b 
            // block bits
            // Apply the set index mask to get the set index
            mem_addr_t setIndex = (setMask & addr) >> b;
            mem_addr_t tag = (tagMask & addr) >> (s + b);
//          printf("set index = %llx\n", setIndex);
//          printf("tag = %llx\n", tag);
            int numIterations = 1;
            switch(buf[1]) {
            // There are no empty lines remaining (and thus an eviction)
            // if there are no more invalid lines
            // For the simulation count, the counts of hisses/misses/evictions
            // does not differ if Stores are considered as Loads and Moves
            // as twice of either.
            case 'M':
                numIterations = 2;
            case 'S':
            case 'L': {
                for (int times = 0; times < numIterations; ++times) {
                    // Attempt to load data from the cache
                    // If it exists, there's a hit; if not, a miss (and if lines
                    // are all used, an eviction)
                    // Note:
                    // In the event of a MISS, the need for an eviction depends 
                    // on if there are any invalid lines that could be filled
                    // Assume set is full, when an invalid line is found, set 
                    // the flag to false
                    char setIsFull = 1;
                    int emptyLineIndex = 0; // Will hold line index that's empty
                    // HIT: Check which valid lines have matching tags
                    char hasHit = 0;
                    for (int lineIndex = 0; lineIndex < E; ++lineIndex) {
                        // Check if the line's valid bit is set...
                        if ((cache[setIndex][lineIndex])->valid) {
                            // Check if the tag matches...
                            if ((cache[setIndex][lineIndex])->tag == tag) {
                                // Hit! Increment the counter
//                              printf("HIT!\n");
                                ++hit_count;
                                // Increase the LRU counter of all the lines and
                                // reset the current line later (easier than 
                                // picking all the lines but the current line 
                                // to increment)
                                for (int i = 0; i < E; ++i)
                                    if ((cache[setIndex][i])->valid)
                                        ++((cache[setIndex][i])->LRUcounter);
                                // The line was just accessed, reset its LRU 
                                // counter
                                (cache[setIndex][lineIndex])->LRUcounter = 0;
                                // There was just a hit so stop searching
                                hasHit = 1;
                                break;
                            }
                        }
                        else {
                            emptyLineIndex = lineIndex;
                            setIsFull = 0;
                        }
                    }
                    // MISS: Given a miss, check if the set has room for load
                    if (!hasHit) {
//                  printf("MISS!\n");
                        ++miss_count;
                        if (!setIsFull) {
                            // Not all the lines were filled
                            (cache[setIndex][emptyLineIndex])->valid = 1;
                            (cache[setIndex][emptyLineIndex])->tag = tag;
                            // Increment the LRU counter of all the lines
                            for (int i = 0; i < E; ++i)
                                if ((cache[setIndex][i])->valid)
                                    ++((cache[setIndex][i])->LRUcounter);
                            // The line was just accessed, reset its LRU counter
                            (cache[setIndex][emptyLineIndex])->LRUcounter = 0;
                        }
                        else {
                            // The set was full, a line will be evicted
                            ++eviction_count;
//                          printf("EVICTION!\n");
                            // Find the line with the highest LRU counter
                            int highestLRULineIndex = 0;
                            int highestLRUcount = 0;
                            for (int i = 0; i < E; ++i) {
                                if ((cache[setIndex][i])->LRUcounter >
                                     highestLRUcount) {
                                    highestLRUcount = 
                                        (cache[setIndex][i])->LRUcounter;
                                    highestLRULineIndex = i;
                                }
                            }
                            // Evict that line by overwriting it's tag bit
                            (cache[setIndex][highestLRULineIndex])->tag = tag;
                            // Increment the LRU counter of all the lines
                            for (int i = 0; i < E; ++i) {
                                ++((cache[setIndex][i])->LRUcounter);
                            }
                            // The line was just accessed, reset its LRU counter
                            (cache[setIndex][highestLRULineIndex])->LRUcounter =
                                0;
                        }
                    } 
                }
                break;
            }
            default:
                break;
            }
        }
    }
    fclose(trace_fp);
}

/* 
 * freeCache - free allocated memory
 */
void freeCache() {
    // For every set
    for (int setIndex = 0; setIndex < pow(2, s); ++setIndex) {     
        // For every line
        for (int lineIndex = 0; lineIndex < E; ++lineIndex) {
            // Free the line struct
            free(cache[setIndex][lineIndex]);
        }
        // Free the set
        free(cache[setIndex]);
    }
    // Free the cache itself
    free(cache);
}

/*
 * printUsage - Print usage info
 */
void printUsage(char* argv[]) {
    printf("Usage: %s [-hv] -s <num> -E <num> -b <num> -t <file>\n", argv[0]);
    printf("Options:\n");
    printf("  -h         Print this help message.\n");
    printf("  -v         Optional verbose flag.\n");
    printf("  -s <num>   Number of set index bits.\n");
    printf("  -E <num>   Number of lines per set.\n");
    printf("  -b <num>   Number of block offset bits.\n");
    printf("  -t <file>  Trace file.\n");
    printf("\nExamples:\n");
    printf("  linux>  %s -s 4 -E 1 -b 4 -t traces/yi.trace\n", argv[0]);
    printf("  linux>  %s -v -s 8 -E 2 -b 4 -t traces/yi.trace\n", argv[0]);
    exit(0);
}

/*
 * main - Main routine 
 */
int main(int argc, char* argv[]) {
    char c;
    while((c=getopt(argc,argv,"s:E:b:t:vh")) != -1) {
        switch(c) {
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
            trace_file = optarg;
            break;
        case 'v':
            verbosity = 1;
            break;
        case 'h':
            printUsage(argv);
            exit(0);
        default:
            printUsage(argv);
            exit(1);
        }
    }
    /* Make sure that all required command line args were specified */
    if (s == 0 || E == 0 || b == 0 || trace_file == NULL) {
        printf("%s: Missing required command line argument\n", argv[0]);
        printUsage(argv);
        exit(1);
    }
    /* Initialize cache */
    buildCache();
    /* Test it with the trace*/
    playTrace(trace_file);
    /* Free allocated memory */
    freeCache();
    /* Output the hit and miss statistics for the autograder */
    printSummary(hit_count, miss_count, eviction_count);
    return 0;
}
