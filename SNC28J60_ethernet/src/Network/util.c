#ifndef UTIL
#define UTIL

#include <string.h>
#include "util.h"





unsigned char ParseLong(unsigned long *number, const unsigned char *ch, const unsigned short length){
 if(length == 0){
  return 0;
 }
 unsigned short i;
 unsigned long decimalIndex = 1;
 *number = 0;
 for(i = length - 1;; i--){
  if(ch[i] < '0' || ch[i] > '9'){
   return 0;
  }
  *number += (ch[i] - '0') * decimalIndex;
  decimalIndex *= 10;
  if(i == 0){
   break;
  }
 }
 return 1;
}

unsigned char CharsCmp(const unsigned char *ch1, const unsigned short length1, const unsigned char *ch2, const unsigned short length2){
 if(length1 != length2){
  return 0;
 }
 return memcmp(ch1, ch2, length1) == 0;
}
void CharsCat(unsigned char *destination, unsigned short *destinationLength, const unsigned char *source, const unsigned short sourceLength){
 memcpy(destination + *destinationLength, source, sourceLength);
 *destinationLength += sourceLength;
}
#endif
