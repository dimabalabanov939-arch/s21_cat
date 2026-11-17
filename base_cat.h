#ifndef BASE_CAT_H
#define BASE_CAT_H
#define _POSIX_C_SOURCE 200809L
#include "getopt.h"
#include "stdint.h"
#include "stdio.h"
#include "stdlib.h"
#include "string.h"
typedef struct {
  int number_nonblank;   //-b
  int number_all;        //-n
  int show_tabs;         //(-t && -v) || -T
  int show_ends;         //(-e && -v) || -E
  int show_nonprinting;  // -v
  int squeeze_blank;     //-s
} options;
void init_long_options_for_cat(struct option* long_opt);
options init_opt();
int read_and_transformations(char* filename, options opt, int* n);
char* lines_transformations(char* line, options opt, int* n, int* print,
                            ssize_t* len);
char* number(char* line, int* n, ssize_t* len);
char* number_nonblank(char* line, int* n, ssize_t* len, options opt);
char* show_tabs(char* line, ssize_t* len, options opt);
char* show_ends(char* line, ssize_t* len, options opt);
char* show_nonprinting(char* line, ssize_t* len);
int squeeze_blank(const char* line, int* print, options opt);
char* numbered_line(char* line, int n, ssize_t* len);
int is_empty_line(const char* line, options opt);
void output(char* line, ssize_t len);
char* show_nonprinting_one(char* line, ssize_t* len, unsigned char sym,
                           options opt);
void clean_line(char** cur, char* next, char* original);

#endif
