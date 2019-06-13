#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <sys/types.h>
#include <unistd.h>

#define MAX_TERM_LEN 7

typedef unsigned char uchar;
typedef struct _hashmap_t {
  uchar *key;
  int count;
  struct _hashmap_t *next;
} hashmap_t;

const size_t mapsize = (1 << 18);

// BKDR Hash Function
unsigned int BKDRHash(uchar *str)
{
  unsigned int seed = 131; // 31 131 1313 13131 131313 etc..
  unsigned int hash = 0;

  while (*str) {
    hash = hash * seed + (*str++);
  }

  return (hash & 0x7FFFFFFF);
}

void hashmap_put(hashmap_t **map, uchar *key)
{
  unsigned int index;
  hashmap_t *new;

  new = (hashmap_t *)calloc(sizeof(hashmap_t), 1);
  new->key = (uchar *)strdup(key);

  index = (BKDRHash(key) & (mapsize - 1));
  if (map[index]) {
    new->next = map[index];
    map[index] = new;
  } else {
    map[index] = new;
  }
}

hashmap_t* hashmap_get(hashmap_t **map, uchar *key)
{
  unsigned int index;
  hashmap_t *ptr;

  index = BKDRHash(key) & (mapsize - 1);
  if ((ptr = map[index])) {
    while (ptr) {
      if (strcmp((char *)key, (char *)ptr->key) == 0) {
        return ptr;
      }
      ptr = ptr->next;
    }
  }

  return NULL;
}

uchar *getword(uchar *ptr, uchar *endptr, uchar *word)
{
  uchar *qtr;

  if (isspace(*ptr)) {
    while (isspace(*ptr)) ++ptr;
  }

  qtr = word;
  if (ptr < endptr) {
    if (*ptr < 128) {
      while (*ptr && *ptr < 128 && !isspace(*ptr)) {
        *qtr++ = *ptr++;
        if (qtr - word >= 63) break;
      }
    } else {
      *qtr++ = *ptr++;
      *qtr++ = *ptr++;
      *qtr++ = *ptr++;
    }
  }
  *qtr++ = '\0';

  return ptr;
}

uchar *ngram(uchar *ptr, uchar *endptr, uchar *term, int n)
{
  uchar word[64];
  int i;

  term[0] = '\0';
  for (i = 0; i < n; i++) {
    ptr = getword(ptr, endptr, word);
    if (ptr >= endptr) break;
    strcat(term, word);
  }

  return ptr;
}

int cmp(const void *a, const void *b)
{
  hashmap_t **x = (hashmap_t **)a;
  hashmap_t **y = (hashmap_t **)b;

  return (*y)->count - (*x)->count;
}

int main(int argc, char *argv[])
{
  uchar *ptr, *endptr;
  uchar *nextptr;
  uchar word[64];
  uchar text[512];
  uchar linebuf[1024];
  uchar *textbuf;
  size_t textsize;
  size_t numbytes;
  size_t numterms;
  hashmap_t **terms, **termsarray;
  hashmap_t *term;
  FILE *fp;
  int i, j;

  terms = (hashmap_t **)calloc(sizeof(hashmap_t *), mapsize);

  fp = fopen(argv[1], "r");
  if (!fp) {
    exit(1);
  }

  numterms = 0;
  while (fgets(linebuf, 1024, fp)) {
    linebuf[strlen(linebuf) - 1] = '\0';
    hashmap_put(terms, linebuf);
    ++numterms;
  }

  textsize = lseek(0, 0, SEEK_END);
  if (textsize <= 0) {
    return 0;
  }

  textbuf = (uchar *)malloc(sizeof(uchar) * (textsize + 1));

  lseek(0, 0, SEEK_SET);
  if ((numbytes = read(0, textbuf, textsize)) > 0) {
    textbuf[numbytes] = '\0';
  }

  ptr = textbuf;
  endptr = ptr + strlen(textbuf);
  while (ptr < endptr) {
    for (i = MAX_TERM_LEN; i >= 1; i--) {
      nextptr = ngram(ptr, endptr, text, i);
      if ((term = hashmap_get(terms, text)) != NULL) {
        term->count += 1;
        break;
      }
    }
    ptr = nextptr;
  }

  termsarray = (hashmap_t **)malloc(sizeof(hashmap_t *) * numterms);
  for (i = 0, j = 0; i < mapsize, j < numterms; i++) {
    term = terms[i];
    while (term) {
      termsarray[j++] = term;
      term = term->next;
    }
  }

  qsort(termsarray, numterms, sizeof(hashmap_t *), cmp);

  for (i = 0; i < numterms; i++) {
    if (termsarray[i]->count > 1) {
      printf("%s %d\n", termsarray[i]->key, termsarray[i]->count);
    } else {
      break;
    }
  }

  fclose(fp);

  free(textbuf);
  textbuf = NULL;

  for (i = 0; i < numterms; i++) {
    free(termsarray[i]->key);
    termsarray[i]->key = NULL;
    free(termsarray[i]);
    termsarray[i] = NULL;
  }
  free(termsarray);
  termsarray = NULL;
  free(terms);
  terms = NULL;

  return 0;
}
