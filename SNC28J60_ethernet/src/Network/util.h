/*
 * utils.h
 *
 *  Created on: 10 дек. 2024 г.
 *      Author: VirtWin10
 */

#ifndef NETWORK_UTIL_H_
#define NETWORK_UTIL_H_

inline unsigned char Low(const unsigned short value){
 return (value&0xFF);
}

inline unsigned char High(const unsigned short value){
 return ((value>>8)&0xFF);
}

inline void LowPut(unsigned short *value, const unsigned char ch){
 ((unsigned char*)value)[0] = ch;
}

inline void HighPut(unsigned short *value, const unsigned char ch){
 ((unsigned char*)value)[1] = ch;
}

inline unsigned short CharsToShort(const unsigned char *value){
 unsigned short numericValue;
 ((unsigned char*)&numericValue)[1] = value[0];
 ((unsigned char*)&numericValue)[0] = value[1];
 return numericValue;
}

inline void CharsPutShort(unsigned char *chars, const unsigned short value){
 chars[0] = ((unsigned char*)&value)[1];
 chars[1] = ((unsigned char*)&value)[0];
}

inline unsigned long CharsToLong(const unsigned char *value){
 unsigned long numericValue;
 ((unsigned char*)&numericValue)[3] = value[0];
 ((unsigned char*)&numericValue)[2] = value[1];
 ((unsigned char*)&numericValue)[1] = value[2];
 ((unsigned char*)&numericValue)[0] = value[3];
 return numericValue;
}

unsigned char ParseLong(unsigned long *number, const unsigned char *ch, const unsigned short length);

unsigned char CharsCmp(const unsigned char *ch1, const unsigned short length1, const unsigned char *ch2, const unsigned short length2);
void CharsCat(unsigned char *destination, unsigned short *destinationLength, const unsigned char *source, const unsigned short sourceLength);

#endif /* NETWORK_UTIL_H_ */
