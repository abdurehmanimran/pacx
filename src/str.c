#include "str.h"

#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

void allocString(String **str, unsigned int cap) {
  *str = (String *)malloc(sizeof(**str));
  if (*str == NULL) {
    printf("Error: Failed to allocate memory for String struct !!\n");
    exit(1);
  }

  (*str)->size = 0;
  (*str)->capacity = cap;

  (*str)->str = (char *)malloc(sizeof(char) * cap);
  memset((*str)->str, 0, sizeof(char) * (*str)->capacity);

  if ((*str)->str == NULL) {
    printf("Error: Failed to allocate memory for string !!\n");
    exit(1);
  }
}

String *createString(char *str) {
  String *stringStruct;
  allocString(&stringStruct, strlen(str) + 1);

  stringStruct->size = strlen(str);
  stringStruct->capacity = strlen(str) + 1;
  memset(stringStruct->str, 0, sizeof(char) * stringStruct->size);
  strcpy(stringStruct->str, str);

  return stringStruct;
}

void expandStringCap(String **str) {
  (*str)->capacity += EXPAND_CAP;
  char *tempStr = (char *)realloc((*str)->str, sizeof(char) * (*str)->capacity);

  if (tempStr != NULL) {
    (*str)->str = tempStr;
  } else {
    printf("Error: Failed to extend memory for string !!\n");
    exit(1);
  }
}

void stringAppend(String **dest, char *srcStr) {
  while ((*dest)->size + strlen(srcStr) >= (*dest)->capacity)
    expandStringCap(dest);

  (*dest)->size += strlen(srcStr);
  strcat((*dest)->str, srcStr);
}

void stringCat(String **dest, String *src) {
  while ((*dest)->size + src->size >= (*dest)->capacity)
    expandStringCap(dest);

  int index = (*dest)->size;
  (*dest)->size += src->size;

  for (unsigned int i = 0; i < src->size; i++) {
    (*dest)->str[index++] = src->str[i];
  }
  (*dest)->str[index] = 0;
}

void replaceInString(String **str, char *find, char *replace) {
  unsigned int findlen = strlen(find);
  assert(findlen != 0);
  unsigned int replacelen = strlen(replace);
  int offset = replacelen - findlen;

  for (unsigned int i = 0; i < (*str)->size; i++) {
    if (strstr((*str)->str, find) != (*str)->str + i)
      continue; // Not found go to the next char

    // Adding 1 to account for the NULL char
    while (offset + (*str)->size + 1 > (*str)->capacity)
      expandStringCap(str);

    // Slide to the right starting from the end
    if (offset > 0) {
      for (unsigned int k = (*str)->size - 1; k >= findlen + i; k--)
        (*str)->str[k + offset] = (*str)->str[k];
    } else { // Slide to the left starting from the left side
      for (unsigned int k = findlen + i; k < (*str)->size; k++)
        (*str)->str[k + offset] = (*str)->str[k];
    }

    // Calculate the new size and place the null character accordingly
    (*str)->size += offset;
    (*str)->str[(*str)->size] = '\0';

    // Replace
    for (unsigned int k = 0; k < replacelen; k++) {
      (*str)->str[k + i] = replace[k];
    }
  }
}

String *getOutput(FILE *stream) {
  String *output;
  allocString(&output, 2128);

  char buffer[1025];
  memset(buffer, 0, sizeof(buffer));

  long unsigned int bytes =
      fread(buffer, sizeof(char), sizeof(buffer) - 1, stream);

  if (bytes == 0) {
    freeString(output);
    return NULL;
  }

  while (bytes > 0) {
    stringAppend(&output, buffer);

    memset(buffer, 0, sizeof(buffer));
    bytes = fread(buffer, sizeof(char), sizeof(buffer) - 1, stream);
  }

  return output;
}

char **strToArray(const char *str) {
  char *dupStr = strdup(str);
  char **arr = NULL; // Will be allocated after size calculation
  int end = strlen(dupStr) - 1;
  while (dupStr[end] == ' ') {
    dupStr[end] = 0;
    end--;
  }

  char *i = strstr(dupStr, " ");
  int spaceCount = 0;

  while (i) {
    spaceCount++;
    i++; // Move from space to next char
    i = strstr(i, " ");
  }

  // spaceCount + 2 -> one for NULL and one for the last word after space
  arr = malloc(sizeof(char *) * (spaceCount + 2));

  char *tok, *tokPtr;
  tok = strtok_r(dupStr, " ", &tokPtr);
  int index = 0;

  while (tok != NULL) {
    arr[index++] = strdup(tok);
    tok = strtok_r(NULL, " ", &tokPtr);
  }

  arr[index] = NULL;

  if (dupStr)
    free(dupStr);

  return arr;
}

void freeString(String *ptr) {
  if (ptr) {
    if (ptr->str)
      free(ptr->str);
    free(ptr);
  }
}
