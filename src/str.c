#include "str.h"
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

void freeString(String *ptr) {
  if (ptr) {
    if (ptr->str)
      free(ptr->str);
    free(ptr);
  }
}
