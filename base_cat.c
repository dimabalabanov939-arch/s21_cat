#include "base_cat.h"
options init_opt() {
    options opt;
    opt.number_all = 0;
    opt.number_nonblank = 0;
    opt.show_ends = 0;
    opt.show_nonprinting = 0;
    opt.show_tabs = 0;
    opt.squeeze_blank = 0;
    return opt;
}
int is_empty_line(char* line) {
    int is_empty = 0;
    if (strcmp(line, "\n") == 0 || strcmp(line, "\r\n") == 0 || strcmp(line, "") == 0 || strcmp(line, "\r") == 0) is_empty = 1;
    return is_empty;
}
void init_long_options_for_cat(struct option *long_opt) {
    long_opt[0].name = "number"; long_opt[0].val = 'n';
    long_opt[1].name = "number-nonblank"; long_opt[1].val = 'b';
    long_opt[2].name = "show-tabs"; long_opt[2].val = 'T';
    long_opt[3].name= "show-ends"; long_opt[3].val = 'E';
    long_opt[4].name = "show-nonprinting"; long_opt[4].val = 'v';
    long_opt[5].name = "squeeze-blank";long_opt[5].val = 's';
    long_opt[6].name = "show-all"; long_opt[6].val = 'A';
    long_opt[7].name = NULL; long_opt[7].val = 0;
    for (int i = 0; i < 8; i ++) {
        long_opt[i].has_arg = no_argument;
        long_opt[i].flag = NULL;
    }
}
void read_and_transformations(char* filename, options opt, int *n) {
    int er1 = 0, er2 = 0; char line[256];
    FILE *f = fopen(filename, "r");
    if (f == NULL) {
         fprintf(stderr, "s21_cat: %s: No such file or directory", filename);
         er1 = 1;
    }
    if(!er1) {
        while (fgets(line, sizeof(line), f) != NULL) {
            if(!er2) {
                char* new_line = lines_transformations(line, opt, n);
                if(new_line != NULL) {
                    output(new_line);
                    if (new_line != line) free(new_line);
                } else {
                    er2 = 1;
                }
            }
        }
        if (fclose(f) != 0) {
            perror("fclose");
        }
    }
    return;
}
char* lines_transformations(char* line, options opt, int *n) {
    if(opt.squeeze_blank) squeeze_blank(line);
    if(opt.number_all) line = number(line, n);
    if(opt.number_nonblank) line = number_nonblank(line, n);
    if(opt.show_ends) show_ends(line);
    if(opt.show_tabs) show_tabs(line);
    if(opt.show_nonprinting) show_nonprinting(line);
    return line;
}
char* numbered_line(char* line, int n) {
    char prefix[32]; int er1 = 0; int er2 = 0; int er3 =0; int er4 = 0; int res; char* new_line = NULL;
    res = snprintf(prefix, 32, "%6d\t", n);
    if(res < 0) er1 = 1; else if (res >= 32) er2 = 1;
    if (!er1 && !er2) {
        size_t plen = strlen(prefix);
        size_t llen = strlen(line);
        if(llen > SIZE_MAX - plen - 1) er3 = 1; else {
            size_t total = plen + llen + 1;
            new_line = malloc(total);
            if(new_line == NULL) er4 = 1; else {
                memcpy(new_line, prefix, plen);
                memcpy(new_line + plen, line, llen);
                new_line[total - 1] = '\0';
            }
        }
    }
    return new_line;
}
char* number(char* line, int *n) {
    line = numbered_line(line, *n);
    (*n) ++;
    return line;
}
char* number_nonblank(char* line, int *n){
    if(!is_empty_line(line)) {
        line = numbered_line(line, *n);
        (*n) ++;
    }
    return line;
}
char* show_tabs(char* line){
    return line;
}
char* show_ends(char* line) {
    return line;
}
char* show_nonprinting(char* line) {
    return line;
}
char* squeeze_blank(char* line){
    return line;
}
void output(char *line) {
    printf("%s", line);
}
