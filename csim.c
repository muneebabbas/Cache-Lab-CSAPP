#include "cachelab.h"
#include <stdio.h>
#include <stdlib.h>
#include <getopt.h>

//---------------------------------------------------------------
// This is a single block or cache line of 2^b bytes
//---------------------------------------------------------------
typedef struct{
	unsigned long long tag;
	unsigned int age;
	unsigned short int valid;
}block;

//---------------------------------------------------------------
// This struct contains one set with k blocks (lines) in a k way
// set associative cache
//---------------------------------------------------------------
typedef struct{
	block* lines;
	unsigned int E;
}set;

//---------------------------------------------------------------
// This struct contains the complete cache.
// sets in initialized to 2^s sets
//---------------------------------------------------------------

typedef struct{
	set* sets;
	unsigned int no_sets;
	short int verboseFlag;
	int b;
	int E;
	int s;
	int hits;
	int misses;
	int evictions;
}cache;

//---------------------------------------------------------------
// Function declarations

void init_cache(cache*, int, int, int, int);
void init_line(block*);
void incrementAge(block* cacheLines, int E);
void access(cache* myCache, unsigned long long address);

//---------------------------------------------------------------



int main(int argc, char** argv)
{
	unsigned int b = 0;
	unsigned int s = 1;
	unsigned int E = 1;
	unsigned short int vFlag = 0;
	int c;
	char* trace;
	
	while ((c = getopt (argc, argv, "vb:s:E:t:")) != -1)
		switch (c)
	{
		case 'v':
		vFlag = 1;
		break;

		case 'b':
		b = atoi(optarg);
		break;

		case 's':
		s = atoi(optarg);
		break;

		case 'E':
		E = atoi(optarg);
		break;

		case 't':
		trace = optarg;
		break;

		default:
		printf("Invalid argument\n");
		return 0;
	}

	int num_sets = 1 << s;
	cache myCache;
	myCache.verboseFlag = vFlag;
	init_cache(&myCache, num_sets, E, b, s);

	// for(unsigned long long i = 0; i < 16; ++i)
	// 	access(&myCache, i);
	// access (&myCache, 15);

	FILE* fp = fopen(trace, "r");
	if (fp == NULL)
	{
		printf("No such file\n");
		return 1;
	}

	unsigned long long address;
	char type;
	int length;
	int retVal;
	while(1)
	{ 
		retVal = fscanf(fp," %c %llx,%d", &type, &address, &length);
		if (retVal != 3)
			break;

		switch(type)
		{
			case 'I':
			break;

			case 'S':
			access(&myCache, address);
			break;

			case 'L':
			access(&myCache, address);
			break;

			case 'M':
			access(&myCache, address);
			access(&myCache, address);
			break;
			
			default:
			break;
		}
		
	}
	
	printSummary(myCache.hits, myCache.misses, myCache.evictions);
	return 0;
}

/*
	Allocate memory for 2^s sets. Now for each set allocate memory for E 
	lines as each set contains E lines.
*/

void init_cache(cache* myCache, int num_sets, int E, int b, int s)
{
	myCache->hits = 0;
	myCache->misses = 0;
	myCache->evictions = 0;
	myCache->b = b;
	myCache->E = E;
	myCache->s = s;
	myCache->sets = malloc(num_sets * sizeof(*myCache->sets));
	myCache->no_sets = num_sets;
	for (int i = 0; i < num_sets; ++i)
	{
		myCache->sets[i].E = E;
		myCache->sets[i].lines = malloc(E * sizeof(*myCache->sets[i].lines));
		for(int j = 0; j < E; ++j)
		{
			init_line(&myCache->sets[i].lines[j]);
		}
	}
}

void init_line(block* line)
{
	line->tag = 0;
	line->age = 0;
	line->valid = 0;
}

void access(cache* myCache, unsigned long long address)
{
	unsigned int set_index;
	unsigned long long tag;
	// unsigned long long addr = address;
	
	// shift adress right b bits
	
	address = address >> myCache->b;
	set_index = (unsigned int) address & ((1<<myCache->s) - 1);
	tag = address >> myCache->s;
	// get the relevant cache lines
	block* cacheLines = myCache->sets[set_index].lines;

	// traverse the set and see if tag matches with any line
	// also keep track of line with max age 
	int space = -1;
	unsigned int max = 0;
	incrementAge(cacheLines, myCache->E);
	for (int i = 0; i < myCache->E; ++i)
	{
		if(cacheLines[i].valid)
		{
			if (cacheLines[i].tag == tag){
				++myCache->hits;
				cacheLines[i].age = 0;
				return;
			}
			else{
				if (cacheLines[i].age > cacheLines[max].age)
					max = i;
			}
		}
		else
			space = i;
	}

	++myCache->misses;
	if (space >= 0)
	{
		cacheLines[space].valid = 1;
		cacheLines[space].tag = tag;
		cacheLines[space].age = 0;
	}
	else
	{
		cacheLines[max].valid = 1;
		// printf("tag--> %u address--> %llu\n", (unsigned int) tag, addr);	
		++myCache->evictions;
		cacheLines[max].tag = tag;
		cacheLines[max].age = 0;
	}
}

void incrementAge(block* cacheLines, int E)
{
	for(int i = 0; i < E; ++i)
	{
		if (cacheLines[i].valid)
			cacheLines[i].age++;
	}
}








