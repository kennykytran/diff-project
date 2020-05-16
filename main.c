#include "diff.h"



int main(int argc, const char * argv[]) {
  
  init_options_files(--argc, ++argv);

  if(showsidebyside && showleftcolumn) diff_y(argc,argv,1);
  else if(showsidebyside && suppresscommon) diff_y(argc, argv,2);
  else if(showsidebyside) diff_y(argc, argv, 0);
  else if(showcontext) diff_context(argc,argv);
  else if(showunified) diff_unified(argc,argv);
  else diff_normal(argc,argv);
 
  return 0;
}



void version(void) {
  printf("\n\n\ndiff (CSUF diffutils) 1.0.0\n");
  printf("Copyright (C) 2014 CSUF\n");
  printf("This program comes with NO WARRANTY, to the extent permitted by law.\n");
  printf("You may redistribute copies of this program\n");
  printf("under the terms of the GNU General Public License.\n");
  printf("For more information about these matters, see the file named COPYING.\n");
  printf("Written by Kenny Tran\n\n");
}


void loadfiles(const char* filename1, const char* filename2) {
  memset(buf, 0, sizeof(buf));
  memset(strings1, 0, sizeof(strings1));
  memset(strings2, 0, sizeof(strings2));
  
  FILE *fin1 = openfile(filename1, "r");
  FILE *fin2 = openfile(filename2, "r");
  
  while (!feof(fin1) && fgets(buf, BUFLEN, fin1) != NULL) { strings1[count1++] = strdup(buf); }  fclose(fin1);
  while (!feof(fin2) && fgets(buf, BUFLEN, fin2) != NULL) { strings2[count2++] = strdup(buf); }  fclose(fin2);
  
  if (count1 != count2) { different = 1;  return; }
  for (int i = 0, j = 0; i < count1 && j < count2;  ++i, ++j) {
    if (strcmp(strings1[i], strings2[j]) != 0) { different = 1;  return; }
  }
}

void print_option(const char* name, int value) { printf("%17s: %s\n", name, yesorno(value)); }

void diff_output_conflict_error(void) {
  fprintf(stderr, "diff: conflicting output style options\n");
  fprintf(stderr, "diff: Try `diff --help' for more information.)\n");
  exit(CONFLICTING_OUTPUT_OPTIONS);
}

void setoption(const char* arg, const char* s, const char* t, int* value) {
  if ((strcmp(arg, s) == 0) || ((t != NULL && strcmp(arg, t) == 0))) {
    *value = 1;
  }
}

void showoptions(const char* file1, const char* file2) {
  printf("diff options...\n");
  print_option("diffnormal", diffnormal);
  print_option("show_version", showversion);
  print_option("show_brief", showbrief);
  print_option("ignorecase", ignorecase);
  print_option("report_identical", report_identical);
  print_option("show_sidebyside", showsidebyside);
  print_option("show_leftcolumn", showleftcolumn);
  print_option("suppresscommon", suppresscommon);
  print_option("showcontext", showcontext);
  print_option("show_unified", showunified);
  
  printf("file1: %s,  file2: %s\n\n\n", file1, file2);
  
  printline();
}


void init_options_files(int argc, const char* argv[]) {
  int cnt = 0;
  //const char* files[2] = { NULL, NULL };
  int size = argc;
  
  while (argc-- > 0) {
    const char* arg = *argv;
    setoption(arg, "-v",       "--version",                  &showversion);
    setoption(arg, "-q",       "--brief",                    &showbrief);
    setoption(arg, "-i",       "--ignore-case",              &ignorecase);
    setoption(arg, "-s",       "--report-identical-files",   &report_identical);
    setoption(arg, "--normal", NULL,                         &diffnormal);
    setoption(arg, "-y",       "--side-by-side",             &showsidebyside);
    setoption(arg, "--left-column", NULL,                    &showleftcolumn);
    setoption(arg, "--suppress-common-lines", NULL,          &suppresscommon);
    setoption(arg, "-c",       "--context",                  &showcontext);
    setoption(arg, "-u",       "showunified",                &showunified);
    if (arg[0] != '-') {
      if (cnt == 2) {
        fprintf(stderr, "apologies, this version of diff only handles two files\n");
        fprintf(stderr, "Usage: ./diff [options] file1 file2\n");
        exit(TOOMANYFILES_ERROR);
      } 
      else { files[cnt++] = arg; }
    }
    ++argv;   // DEBUG only;  move increment up to top of switch at release
  }

  if (!showcontext && !showunified && !showsidebyside && !showleftcolumn) {
    diffnormal = 1;
  }

  if (showversion) { version();  exit(0); }
  
  if (((showsidebyside || showleftcolumn) && (diffnormal || showcontext || showunified)) ||
      (showcontext && showunified) || (diffnormal && (showcontext || showunified))) {

    diff_output_conflict_error();
  }
  
//  showoptions(files[0], files[1]);
  loadfiles(files[0], files[1]);

  if (report_identical && !different) { printf("The files are identical.\n\n");   exit(0); }

  if (showbrief && different) { printf("The files are different.\n\n");   exit(0); }
  else if(showbrief) exit(0);
}


int diff_normal(int argc, const char * argv[]) {//NORMAL DIFF
  para* p = para_first(strings1, count1);
  para* q = para_first(strings2, count2);
  int foundmatch = 0;

  para* qlast = q;
  while (p != NULL) {
    qlast = q;
    foundmatch = 0;
    while (q != NULL && (foundmatch = para_equal(p, q)) == 0) {
      q = para_next(q);
    }
    q = qlast;

    if (foundmatch) {
      while ((foundmatch = para_equal(p, q)) == 0) {
        para_print_normal(q, printrightnormal, 1);
        q = para_next(q);
        qlast = q;
      }
      p = para_next(p);
      q = para_next(q);
    } else {
      para_print_normal(p, printleftnormal, 0);
      p = para_next(p);
    }
  }
  while (q != NULL) {
    para_print_normal(q, printrightnormal, 1);
    q = para_next(q);
  }
  return 0;
}


int diff_y(int argc, const char * argv[], int difftype) {//SIDE BY SIDE
  para* p = para_first(strings1, count1);
  para* q = para_first(strings2, count2);
  int foundmatch = 0;

  para* qlast = q;
  while (p != NULL) {
    qlast = q;
    foundmatch = 0;
    while (q != NULL && (foundmatch = para_equal(p, q)) == 0) {
      q = para_next(q);
    }
    q = qlast;

    if (foundmatch) {
      while ((foundmatch = para_equal(p, q)) == 0) {
        para_print(q, printright);
        q = para_next(q);
        qlast = q;
      }
      if(difftype == 0) para_print(q, printboth);//SIDE BY SIDE
      else if(difftype == 1) para_print(q, printsameleft);//LEFT COLUMN
      //if difftype == 2 do nothing //SUPPRESS COMMON LINES
      p = para_next(p);
      q = para_next(q);
    } else {
      para_print(p, printleft);
      p = para_next(p);
    }
  }
  while (q != NULL) {
    para_print(q, printright);
    q = para_next(q);
  }
  return 0;
}


int diff_context(int argc, const char * argv[]) {//-C COMMAND
  struct stat filestat;
  stat(files[0],&filestat);
  printf("*** %s %s", files[0], ctime(&filestat.st_mtime));
  stat(files[1],&filestat);
  printf("--- %s %s", files[1], ctime(&filestat.st_mtime));
  
  para* p = para_first(strings1, count1);
  para* q = para_first(strings2, count2);
  int foundmatch = 0;

  para* qlast = q;
  while (p != NULL) {
    qlast = q;
    foundmatch = 0;
    while (q != NULL && (foundmatch = para_equal(p, q)) == 0) {
      q = para_next(q);
    }
    q = qlast;

    if (foundmatch) {
      while ((foundmatch = para_equal(p, q)) == 0) {
        para_print_context(q, printrightcontext, 1);
        q = para_next(q);
        qlast = q;
      }
      p = para_next(p);
      q = para_next(q);
    } else {
      para_print_context(p, printleftcontext, 0);
      p = para_next(p);
    }
  }
  while (q != NULL) {
    para_print_context(q, printrightcontext, 1);
    q = para_next(q);
  }
  return 0;
}


int diff_unified(int argc, const char * argv[]) {//-U COMMAND
  struct stat filestat;
  stat(files[0],&filestat);
  printf("--- %s %s", files[0], ctime(&filestat.st_mtime));
  stat(files[1],&filestat);
  printf("+++ %s %s", files[1], ctime(&filestat.st_mtime));
  
  para* p = para_first(strings1, count1);
  para* q = para_first(strings2, count2);
  int foundmatch = 0;

  para* qlast = q;
  while (p != NULL) {
    qlast = q;
    foundmatch = 0;
    while (q != NULL && (foundmatch = para_equal(p, q)) == 0) {
      q = para_next(q);
    }
    q = qlast;

    if (foundmatch) {
      while ((foundmatch = para_equal(p, q)) == 0) {
        para_print_normal(q, printrightcontext, 1);
        q = para_next(q);
        qlast = q;
      }
        para_print_normal(q, printsame, 2);
      p = para_next(p);
      q = para_next(q);
    } else {
      para_print_normal(p, printleftcontext, 0);
      p = para_next(p);
    }
  }
  while (q != NULL) {
    para_print_normal(q, printrightcontext, 1);
    q = para_next(q);
  }
  return 0;
}