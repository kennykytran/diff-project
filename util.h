//
//  util.h
//  diff
//
//  Created by William McCarthy on 5/9/19.
//  Copyright © 2019 William McCarthy. All rights reserved.
//

#ifndef util_h
#define util_h

#include <stdio.h>
#define BUFLEN 256

char* yesorno(int condition);
FILE* openfile(const char* filename, const char* openflags);

void printleft(const char* left);
void printright(const char* right);
void printboth(const char* left_right);

void printline(void);

void printsameleft(const char* left);

void printrightnormal(const char* right);
void printleftnormal(const char* left);

void printrightcontext(const char* right);
void printleftcontext(const char* left);

void printsame(const char* left_right);

#endif /* util_h */
