#include <stdlib.h>
#include <string.h>
#include "hashtable.h"

/* Daniel J. Bernstein's "times 33" string hash function, from comp.lang.C;
   See https://groups.google.com/forum/#!topic/comp.lang.c/lSKWXiuNOAk */
unsigned long hash(char *str) {
  unsigned long hash = 5381;
  int c;

  while ((c = *str++))
    hash = ((hash << 5) + hash) + c; /* hash * 33 + c */

  return hash;
}

hashtable_t *make_hashtable(unsigned long size) {
  hashtable_t *ht = malloc(sizeof(hashtable_t));
  ht->size = size;
  ht->buckets = calloc(sizeof(bucket_t *), size);
  return ht;
}

void ht_put(hashtable_t *ht, char *key, void *val) {
  /* FIXME: the current implementation doesn't update existing entries */
  unsigned int idx = hash(key) % ht->size;

  bucket_t *a = ht->buckets[idx];
  while(a)
  {
    if (strcmp(a->key, key) == 0)
    {
      a->val = val;
      return;
    }
    a = a->next;
  }
  
  bucket_t *b = malloc(sizeof(bucket_t));
  b->key = key;
  b->val = val;
  b->next = ht->buckets[idx];
  ht->buckets[idx] = b;
}

void *ht_get(hashtable_t *ht, char *key) {
  unsigned int idx = hash(key) % ht->size;
  bucket_t *b = ht->buckets[idx];
  while (b) {
    if (strcmp(b->key, key) == 0) {
      return b->val;
    }
    b = b->next;
  }
  return NULL;
}

void ht_iter(hashtable_t *ht, int (*f)(char *, void *)) {
  bucket_t *b;
  unsigned long i;
  for (i=0; i<ht->size; i++) {
    b = ht->buckets[i];
    while (b) {
      if (!f(b->key, b->val)) {
        return; // abort iteration
      }
      b = b->next;
    }
  }
}

void free_hashtable(hashtable_t *ht) {
  int i;
  bucket_t *b, *temp;
  for (i = 0; i < ht->size; i++)
  {
    b = ht->buckets[i];
    while (b)
    {
      temp = b;
      b = b->next;
      free(temp->key);
      free(temp->val);
      free(temp);
    }
  }
  free(ht->buckets);
  free(ht); // FIXME: must free all substructures!
}

/* TODO */
void  ht_del(hashtable_t *ht, char *key) {
  unsigned int i = hash(key)% ht->size;
  bucket_t *b, *temp;
  b = ht->buckets[i];
  if (strcmp(b->key, key) == 0)
  {
    ht->buckets[i] = b->next;
    free(b->key);
    free(b->val);
    free(b);
    return;
  }
  temp = b->next;
  while(temp)
  {
    if (strcmp(temp->key, key) == 0)
    {
      b->next = temp->next;
      free(temp->key);
      free(temp->val);
      free(temp);
      return;
    }
    b = b->next;
    temp = b->next;
  }
}

void  ht_rehash(hashtable_t *ht, unsigned long newsize) {
  // make temp hashtable to store the values in original hashtable
  hashtable_t *temp_ht = malloc(sizeof(hashtable_t));
  temp_ht->size = ht->size;
  temp_ht->buckets = ht->buckets;
  
  // reset original hashtable
  ht->size = newsize;
  ht->buckets = calloc(sizeof(bucket_t),newsize);

  // Make temp bucket pointer to store the values in temp hashtable to new hashtable with new key
  bucket_t *temp_bucket, *temp2_bucket;
  unsigned long i;
  for(i = 0; i < temp_ht->size; i++)
  {
    temp_bucket = temp_ht->buckets[i];
    while(temp_bucket)
    {
      ht_put(ht, temp_bucket->key, temp_bucket->val);
      temp2_bucket = temp_bucket;
      temp_bucket = temp_bucket->next;
      free(temp2_bucket);
    }
  }

  // Free temp hashtable
  free(temp_ht->buckets);
  free(temp_ht);
}
