
#include "cachelab.h"
#include "stdlib.h"
#include "stdio.h"
#include "getopt.h"
#include "math.h"

int counter = 0; // Global LRU counter

typedef struct{
  int h;
  int v;
} options;

typedef struct{
  int hits;
  int misses;
  int evictions;
  int s;
  int s2;
  int e;
  int b;
  int b2;
} cacheSettings;
 
typedef struct{
  int valid;
  unsigned long long tag;
  int lru;
} slot;

typedef struct{
  slot* slot;
} set;

typedef struct{
  set* set;
} cache;
  
cache makeCache(cacheSettings settings)
{
  cache Cache;
  set Set;
  slot Slot;
  int s2 = settings.s2;
  int e = settings.e;
  
  Cache.set = (set*) malloc(sizeof(set) * s2);

  for (int i = 0; i < s2; i++)
    {
      Set.slot = (slot*) malloc(sizeof(slot) * e);
      Cache.set[i] = Set;
      for (int j = 0; j < e; j++)
	{
	  Slot.valid = 0;
	  Slot.tag = 0;
	  Slot.lru = 0;
	  Set.slot[j] = Slot;
	}
    }
  return Cache;
}

int HitOrMiss(set Set, cacheSettings settings, unsigned long long tag) // check if hit or miss
{
  for (int i = 0; i < settings.e; i++)
    {
      if (Set.slot[i].valid == 1)
	{
	  if (Set.slot[i].tag == tag)
	    {
	      counter += 1;
	      Set.slot[i].lru = counter;
	      return 1;
	    }
	}
    }
  return 0;
}

int findLRU(set Set, cacheSettings settings) // Find the lru of a set
{
  int slotIndex = 0;
  int lru = Set.slot[0].lru;
  for (int i = 1; i < settings.e; i++)
    {
      if (Set.slot[i].lru < lru)
        {
          slotIndex = i;
          lru = Set.slot[i].lru;
        }
    }
  return slotIndex;
}


set Evict(set Set, cacheSettings settings, unsigned long long tag) // evict a slot
{
  int slotIndex = findLRU(Set, settings);
  printf("tag = %llx\n", tag);
  Set.slot[slotIndex].tag = tag;
  printf("newtag = %llx\n", Set.slot[slotIndex].tag);
  counter += 1;
  Set.slot[slotIndex].lru = counter;
  return Set;
}

int findEmpty(set Set, cacheSettings settings, unsigned long long tag) // If there is an empty slot in the set, put the new tag in there and return 1
{
  for (int i = 0; i < settings.e; i++)
    {
      if (Set.slot[i].valid == 0)
	{
	  Set.slot[i].valid = 1;
	  Set.slot[i].tag = tag;
	  counter += 1;
	  Set.slot[i].lru = counter;
	  return 1;
	}
    }
  return 0;
}

cacheSettings runCache(cache Cache, cacheSettings settings, unsigned long long address)
{
  int tagSize = 64 - (settings.s + settings.b);
  unsigned long long tag = address >> (settings.s + settings.b);
  unsigned long long temp = address << tagSize;
  unsigned long long index = temp >> (tagSize + settings.b);
  set Set = Cache.set[index];
  
  if (HitOrMiss(Set, settings, tag) == 0)
    {
      settings.misses += 1;
      if (findEmpty(Set, settings, tag) == 1)
	{
	  return settings;
	}
      else
	{
	  int slotIndex = findLRU(Set, settings);
	  Set.slot[slotIndex].tag = tag;
	  counter += 1;
	  Set.slot[slotIndex].lru = counter;
	  settings.evictions += 1;
	  return settings;
	}
    }
  else
    {
      settings.hits += 1;
      return settings;
    }
  return settings;
}
int main(int argc, char **argv)
{
  cacheSettings settings;
  char *traceFile;
  char input;
  while ((input = getopt(argc, argv, "vh:s:E:b:t:")) != -1)
    {
      switch(input)
	{
	case 's':
	  settings.s = atoi(optarg);
	  settings.s2 = pow(2.0, settings.s);
	  break;
	case 'E':
	  settings.e = atoi(optarg);
	  break;
	case 'b':
	  settings.b = atoi(optarg);
	  settings.b2 = pow(2.0, settings.b);
	  break;
	case 't':
	  traceFile = optarg;
	  break;
	default:
	  break;
	}
    }
  settings.hits = 0;
  settings.misses = 0;
  settings.evictions = 0;
  cache Cache = makeCache(settings);
  FILE *openFile;
  char operation;
  unsigned long long int address;
  int size;
  openFile = fopen(traceFile, "r");
  if (openFile != NULL)
    {
      while(fscanf(openFile, " %c %llx,%d", &operation, &address, &size) == 3)
	{
	  switch(operation)
	    {
	    case 'I':
	      break;
	    case 'L':
	      settings = runCache(Cache, settings, address);
	      break;
	    case 'S':
              settings = runCache(Cache, settings, address);
              break;
	    case 'M':
              settings = runCache(Cache, settings, address);
	      settings = runCache(Cache, settings, address);
              break;
	    default:
	      break;
	    }
	}
    }
  printSummary(settings.hits, settings.misses, settings.evictions);
  fclose(openFile);
  return 0;
}
