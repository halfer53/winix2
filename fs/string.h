/**
 * A rather limited version of <string.h>
 *
 * Revision History:
 *  2016-09-19		Paul Monigatti			Original
 *  2016-11-20		Bruce Tan			Modified
 **/
#ifndef _STRING_H_
#define _STRING_H_

//void *memcpy(void *s1, const void *s2, register int n);
int strcmp(const char *s1, const char *s2);
//int strlen(const char *s);
char *strcpy(char *dest,const char *src);
char *strcat(char *dest, const char *src);
int Substring(char* buffer,char* original,int start_index,int length);


#endif