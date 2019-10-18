/* #include <stdio.h> */
/* #include <stdlib.h> */
/* #include <string.h> */
/* #include <assert.h> */
/* #include <unistd.h> */

/* #include "mm.h" */
/* #include "memlib.h" */

/* #define ALIGNMENT 8 */

/* #define ALIGN(size) (((size) + (ALIGNMENT-1)) & ~0x7) */

/* #define SIZE_T_SIZE (ALIGN(sizeof(size_t))) */

/* #define BLK_HDR_SIZE ALIGN(sizeof(blockHdr)) */

/* typedef struct header blockHdr; */

/* struct header { */
/*   size_t size; */
/*   blockHdr *next_p; */
/*   blockHdr *prior_p; */
/* }; */

/* void *find_fit(size_t size); */
/* void print_heap(); */

/* int mm_init(void) */
/* { */
/*   blockHdr *p = mem_sbrk(BLK_HDR_SIZE); */
/*   p->size = BLK_HDR_SIZE; */
/*   p->next_p = p; */
/*   p->prior_p = p; */
/*   return 0; */
/* } */

/* void print_heap() { */
/*   blockHdr *bp = mem_heap_lo(); */
/*   while (bp < (blockHdr *)mem_heap_hi()) { */
/*     printf("%s block at %p, size %d\n", */
/* 	   (bp->size&1)?"allocated":"free", */
/* 	   bp, */
/* 	   (int)(bp->size & ~1)); */
/*     bp = (blockHdr *)((char *)bp + (bp->size & ~1)); */
/*   } */
/* } */

/* void *mm_malloc(size_t size) */
/* { */
/*   int newsize = ALIGN(BLK_HDR_SIZE + size); */
/*   blockHdr *bp = find_fit(newsize); */
/*   if (bp == NULL) { */
/*     bp = mem_sbrk(newsize); */
/*     if ((long)bp == -1) */
/*       return NULL; */
/*     else */
/*       bp->size = newsize | 1; */
/*   } else { */
/*     bp->size |= 1; */
/*     bp->prior_p->next_p = bp->next_p; */
/*     bp->next_p->prior_p = bp->prior_p; */
/*   } */
/*   return (char *)bp + BLK_HDR_SIZE; */
/* } */

/* void split(void *ptr, size_t size) */
/* { */
/*   blockHdr *bp = (blockHdr *)ptr; */
/*   if (bp->size > (size + BLK_HDR_SIZE)) */
/*     { */
/*       blockHdr *new_bp = (blockHdr *)((char *)bp + size); */
/*       size_t newsize = (bp->size & ~1) - size; */

/*       new_bp->size = newsize; */
/*       new_bp->next_p = bp->next_p; */
/*       bp->next_p->prior_p = new_bp; */
/*       new_bp->prior_p = bp; */
/*       bp->next_p = new_bp; */
/*     } */
/* } */

/* void *find_fit(size_t size) */
/* { */
/*   blockHdr *p; */
/*   for (p = ((blockHdr *)mem_heap_lo())->next_p; */
/*        p != mem_heap_lo() && p->size < size; */
/*        p = p->next_p); */

/*   if (p != mem_heap_lo()) */
/*   { */
/*     split(p, size); */
/*     return p; */
/*   } */
/*   else */
/*   { */
/*     return NULL; */
/*   } */
/* } */

/* void coalesce(void *ptr) */
/* { */
/*   blockHdr *bp = (blockHdr *)ptr; */
/*   blockHdr *prev, *next = (blockHdr *)((char *)bp + (bp->size & ~1)); */

/*   if (next > (blockHdr *)mem_heap_hi()) */
/*   { */
/*     next = (blockHdr *)mem_heap_lo(); */
/*     next->size |= 1; */
/*   } */
  
/*   for (prev = ((blockHdr *)mem_heap_lo())->next_p; */
/*        prev != mem_heap_lo() && (bp != (blockHdr *)((char *)prev + prev->size)); */
/*        prev = prev->next_p); */
  
/*   if (prev != mem_heap_lo() && (prev->size & 1) == 0) */
/*     { */
/*       if ((next->size & 1) == 0) //Case 3 */
/* 	{ */
/* 	  bp->prior_p = prev->prior_p; */
/* 	  prev->prior_p->next_p = bp; */

/* 	  next->next_p->prior_p = bp; */
/* 	  bp->next_p = next->next_p; */
/* 	  bp->size = prev->size + bp->size + next->size; */
/* 	} */
/*       else //Case 1 */
/* 	{ */
/* 	  bp->prior_p = prev->prior_p; */
/* 	  prev->prior_p->next_p = bp; */

/* 	  bp->size = prev->size + bp->size; */
/* 	} */
/*     } */
/*   else if ((next->size & 1) == 0) //Case 2 */
/*     { */
/*       bp->next_p = next->next_p; */
/*       next->next_p->prior_p = bp; */
/*       bp->size = bp->size + next->size; */
/*     } */
/* } */


/* void mm_free(void *ptr) */
/* { */
/*   blockHdr *bp = ptr - BLK_HDR_SIZE, *p = mem_heap_lo(); */
  
/*   bp->size &= ~1; */
/*   bp->next_p = p->next_p; */
/*   bp->prior_p = p; */
/*   p->next_p->prior_p = bp; */
/*   p->next_p = bp; */

/*   coalesce(bp); */
/* } */

/* void *mm_realloc(void *ptr, size_t size) */
/* { */
/*   blockHdr *bp = ptr-BLK_HDR_SIZE; */
/*   void *newptr = mm_malloc(size); */
/*   if (newptr == NULL) */
/*     return NULL; */
/*   int copySize = bp->size-BLK_HDR_SIZE; */
/*   if (size < copySize) */
/*     copySize = size; */
/*   memcpy(newptr, ptr, copySize); */
/*   mm_free(ptr); */
/*   return newptr; */
/* }   */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <unistd.h>

#include "mm.h"
#include "memlib.h"

#define ALIGNMENT 8

#define ALIGN(size) (((size) + (ALIGNMENT-1)) & ~0x7)

#define SIZE_T_SIZE (ALIGN(sizeof(size_t)))

#define BLK_HDR_SIZE ALIGN(sizeof(blockHdr))

#define BLK_FTR_SIZE ALIGN(sizeof(blockFtr))

#define BLK_SIZE ALIGN(BLK_HDR_SIZE + BLK_FTR_SIZE)

typedef struct header blockHdr;

struct header {
  size_t size;
};

typedef struct footer blockFtr;

struct footer {
  size_t size;
};

typedef struct free flist;

struct free {
  flist *next;
  blockHdr *hp;
  int size;
};

void *find_fit(size_t size);
void print_heap();

void *mem_heap_hic()
{
  return mem_heap_hi() + 1;
}

int mm_init(void)
{
  blockHdr *hp = mem_sbrk(BLK_SIZE);
  hp->size = BLK_SIZE;
  hp->size |= 1;
  blockFtr *fp = (blockFtr *)mem_heap_hic();
  fp->size = hp->size;
  
  flist *free_list = (flist *)hp;
  free_list->next = free_list;
  free_list->hp = hp;
  free_list->size = 0;
  return 0;
}

void print_heap() {
  blockHdr *bp = (blockHdr *)(mem_heap_lo() + BLK_SIZE);
  while (bp < (blockHdr *)mem_heap_hic() && bp->size > 0) {
    printf("%s block at %p, size %d\n",
	   (bp->size&1)?"allocated":"free",
	   bp,
	   (int)(bp->size & ~1));
    bp = (blockHdr *)((char *)bp + (bp->size & ~1));
  }
}

void *mm_malloc(size_t size)
{
  int newsize = ALIGN(BLK_SIZE + size);
  blockHdr *bp = find_fit(newsize);
  blockFtr *fp;
  if (bp == NULL)
  {
    bp = mem_sbrk(newsize);
    if ((long)bp == -1)
    {
      return NULL;
    }
    else
    {
      bp->size = newsize | 1;
    }
    fp = (blockFtr *)((char *)mem_heap_hic() - BLK_FTR_SIZE);
    if ((long)fp == -1)
    {
      printf("error\n");
      return NULL;
    }
    else
    {
      fp->size = bp->size;
    }
  }
  else
  {
    fp = (blockFtr *)((char *)bp + (bp->size & ~1) - BLK_FTR_SIZE);
    bp->size |= 1;
    fp->size = bp->size;
  }
  return (char *)bp + BLK_HDR_SIZE;
}

void split(void *ptr, size_t size)
{
  blockHdr *bp = (blockHdr *)ptr;

  if (bp->size > (size + BLK_SIZE + BLK_SIZE))
  {
    blockHdr *new_hp = (blockHdr *)((char *)bp + size);
    size_t newsize = (bp->size & ~1) - size;
    blockFtr *new_fp = (blockFtr *)((char *)new_hp + newsize - BLK_FTR_SIZE);
    blockFtr *old_fp = (blockFtr *)((char *)new_hp - BLK_FTR_SIZE);

    bp->size = size;
    old_fp->size = bp->size;

    new_hp->size = newsize;
    new_fp->size = new_hp->size;
  }
}

void *find_fit(size_t size)
{
  blockHdr *p = (blockHdr *)mem_heap_lo();
  for (p += 2;
       p < (blockHdr *)mem_heap_hic() && !(p->size >= size && (p->size & 1) == 0);
       p = (blockHdr *)((char *)p + (p->size & ~1)));
    
  if (p < (blockHdr *)mem_heap_hic())
  {
    split(p, size);
    return p;
  }
  else
  {
    return NULL;
  }
}

void coalesce(void *ptr)
{
  blockHdr *bp = (blockHdr *)ptr;
  blockFtr *prev_fp = (blockFtr *)((char *)bp - BLK_FTR_SIZE);
  blockHdr *next_hp = (blockHdr *)((char *)bp + (bp->size & ~1));
  
  if (prev_fp <= (blockFtr *)(mem_heap_lo() + BLK_SIZE))
  {
    prev_fp = (blockFtr *)(mem_heap_lo() + BLK_HDR_SIZE);
    prev_fp->size |= 1;
  }

  if (next_hp >= (blockHdr *)mem_heap_hi())
  {
    next_hp = (blockHdr *)mem_heap_lo();
    next_hp->size |= 1;
  }
  
  if ((prev_fp->size & 1) == 0)
  {
    if ((next_hp->size & 1) == 0) //Case 3
    {
      blockHdr *new_hp = (blockHdr *)((char *)prev_fp -
      				       (prev_fp->size & ~1) +
      				      BLK_HDR_SIZE);
      
      blockFtr *new_fp = (blockFtr *)((char *)next_hp +
      				      (next_hp->size & ~1) -
      				      BLK_FTR_SIZE);

      new_hp->size = (new_hp->size & ~1) + (bp->size & ~1) + (next_hp->size & ~1);
      new_fp->size = new_hp->size;
    }
    else //Case 1
    {
      blockHdr *new_hp = (blockHdr *)((char *)prev_fp -
				       (prev_fp->size & ~1) +
				       BLK_HDR_SIZE);

      blockFtr *new_fp = (blockFtr *)((char *)bp + (bp->size & ~1) - BLK_FTR_SIZE);

      new_hp->size = (new_hp->size & ~1) + (bp->size & ~1);
      new_fp->size = new_hp->size;
    }
  }
  else if ((next_hp->size & 1) == 0) //Case 2
  {
    blockFtr *new_fp = (blockFtr *)((char *)next_hp + (next_hp->size & ~1) - BLK_FTR_SIZE);

    bp->size = (bp->size & ~1) + (next_hp->size & ~1);
    new_fp->size = bp->size;
  }
}

void mm_free(void *ptr)
{
  blockHdr *bp = ptr - BLK_HDR_SIZE;
  blockFtr *fp = (blockFtr *)((char *)bp + (bp->size & ~1) - BLK_FTR_SIZE);
  
  bp->size &= ~1;
  fp->size &= ~1;
  coalesce(bp);
}

void *mm_realloc(void *ptr, size_t size)
{
  blockHdr *bp = ptr - BLK_HDR_SIZE;  
  void *newptr = mm_malloc(size);

  if (newptr == NULL)
    return NULL;
  
  int copySize = bp->size - BLK_SIZE;

  if (size < copySize)
    copySize = size;
  memcpy(newptr, ptr, copySize);
  mm_free(ptr);
  return newptr;
}
