#ifndef BASE_CAT_H
#define BASE_CAT_H
#include "stdio.h"
#include "stdlib.h"
#include "getopt.h"
#include "string.h"
#include "stdint.h"
typedef struct {
    int number_nonblank; //-b
    int number_all; //-n
    int show_tabs;//(-t && -v) || -T
    int show_ends; //(-e && -v) || -E
    int show_nonprinting;// -v
    int squeeze_blank; //-s
} options;
void init_long_options_for_cat(struct option *long_opt);
options init_opt();
void read_and_transformations(char* filename, options opt, int *n);
char* lines_transformations(char* line, options opt, int *n);
char* number(char* line, int *n); char* number_nonblank(char* line, int *n); char* show_tabs(char* line); char* show_ends(char* line); char* show_nonprinting(char* line); char* squeeze_blank(char* line);
char* numbered_line(char* line, int n);
int is_empty_line(char* line);
void output(char *line);

#endif
