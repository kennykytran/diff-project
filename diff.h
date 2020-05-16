#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <time.h>

#define ARGC_ERROR 1
#define TOOMANYFILES_ERROR 2
#define CONFLICTING_OUTPUT_OPTIONS 3

#define MAXSTRINGS 1024
#define MAXPARAS 4096

#define HASHLEN 200

#define BUFF_SIZE 1000

#include "para.h"
#include "util.h"

void version(void);
int diffLC(int argc, const char * argv[]);
void loadfiles(const char* filename1, const char* filename2);
void print_option(const char* name, int value);
void diff_output_conflict_error(void);
void setoption(const char* arg, const char* s, const char* t, int* value);
void showoptions(const char* file1, const char* file2);
void init_options_files(int argc, const char* argv[]);
char *safegets(char *buffer, int length, FILE *file);
int comparelines(int argc, const char *argv[]);
int diff_y(int argc, const char * argv[], int difftype);
int diffLC(int argc, const char * argv[]);
int diffSL(int argc, const char * argv[]);
int diff_normal(int argc, const char * argv[]);
int diff_context(int argc, const char * argv[]);
int diff_unified(int argc, const char * argv[]);

const char* files[2] = { NULL, NULL };

char buf[BUFLEN];
char *strings1[MAXSTRINGS], *strings2[MAXSTRINGS];
int showversion = 0, showbrief = 0, ignorecase = 0, report_identical = 0, showsidebyside = 0;
int showleftcolumn = 0, showunified = 0, showcontext = 0, suppresscommon = 0, diffnormal = 0;

int count1 = 0, count2 = 0, different = 0;