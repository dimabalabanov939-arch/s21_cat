#include "base_cat.h"

#include <stdlib.h>
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
void clean_line(char** cur, char* next, char* original) {
  if (next == NULL) {
    if (*cur != original) {
      *cur = NULL;
    }
  }
  if (next != *cur) {
    if (*cur != original) {
      free(*cur);
    }
    *cur = next;
  }
}
char* show_nonprinting_one(char* line, ssize_t* len, unsigned char sym,
                           options opt) {
  char* new_line = NULL;
  if (*len >= 0 && (size_t)*len <= (SIZE_MAX - 1) / 3) {
    size_t cap = (size_t)*len * 3 + 1;
    new_line = malloc(cap);
    if (new_line != NULL) {
      size_t out = 0;
      for (size_t i = 0; i < (size_t)*len; i++) {
        unsigned char uc = (unsigned char)line[i];
        if (sym == '\n' && uc == '\r' && (unsigned char)line[i + 1] == sym &&
            opt.show_nonprinting != 1) {
          new_line[out++] = '^';
          new_line[out++] = 'M';
          new_line[out++] = '$';
          new_line[out++] = '\n';
          i++;
        } else if (uc == sym) {
          if (sym == '\n') {
            new_line[out++] = '$';
            new_line[out++] = '\n';
          } else {
            new_line[out++] = '^';
            new_line[out++] = 'I';
          }
        } else
          new_line[out++] = uc;
      }
      new_line[out] = '\0';
      *len = (ssize_t)out;
    }
  }
  return new_line;
}
int is_empty_line(const char* line, options opt) {
  int is_empty = 0;
  if (line[0] == '\n' || (line[0] == '\r' && line[1] == '\n') ||
      (opt.show_ends == 1 && line[0] == '$' && line[1] == '\n'))
    is_empty = 1;
  return is_empty;
}
void init_long_options_for_cat(struct option* long_opt) {
  long_opt[0].name = "number";
  long_opt[0].val = 'n';
  long_opt[1].name = "number-nonblank";
  long_opt[1].val = 'b';
  long_opt[2].name = "show-tabs";
  long_opt[2].val = 'T';
  long_opt[3].name = "show-ends";
  long_opt[3].val = 'E';
  long_opt[4].name = "show-nonprinting";
  long_opt[4].val = 'v';
  long_opt[5].name = "squeeze-blank";
  long_opt[5].val = 's';
  long_opt[6].name = "show-all";
  long_opt[6].val = 'A';
  long_opt[7].name = NULL;
  long_opt[7].val = 0;
  for (int i = 0; i < 8; i++) {
    long_opt[i].has_arg = no_argument;
    long_opt[i].flag = NULL;
  }
}
int read_and_transformations(char* filename, options opt, int* n) {
  int er1 = 0, er2 = 0;
  char* line = NULL;
  int count_empty = 0;
  FILE* f = fopen(filename, "r");
  if (f == NULL) {
    fprintf(stderr, "s21_cat: %s: No such file or directory\n", filename);
    er1 = 1;
  }
  if (!er1) {
    ssize_t len;
    size_t cap = 0;
    while ((len = getline(&line, &cap, f)) != -1) {
      if (!er2) {
        char* new_line =
            lines_transformations(line, opt, n, &count_empty, &len);
        if (new_line != NULL) {
          if (count_empty <= 1) output(new_line, len);
          if (new_line != line) free(new_line);
        } else {
          er2 = 1;
        }
      }
    }
    free(line);
    if (fclose(f) != 0) {
      perror("fclose");
    }
  }
  return er1 + er2;
}
char* lines_transformations(char* line, options opt, int* n, int* count_empty,
                            ssize_t* len) {
  int skip = 0;
  char* original = line;
  char* curr = line;
  char* next;
  if (opt.squeeze_blank) skip = squeeze_blank(curr, count_empty, opt);
  if (!skip) {
    if (opt.show_ends) {
      next = show_ends(curr, len, opt);
      clean_line(&curr, next, original);
    }
    if (opt.show_tabs) {
      next = show_tabs(curr, len, opt);
      clean_line(&curr, next, original);
    }
    if (opt.number_all) {
      next = number(curr, n, len);
      clean_line(&curr, next, original);
    }
    if (opt.number_nonblank) {
      next = number_nonblank(curr, n, len, opt);
      clean_line(&curr, next, original);
    }
    if (opt.show_nonprinting) {
      next = show_nonprinting(curr, len);
      clean_line(&curr, next, original);
    }
  }
  return curr;
}
char* numbered_line(char* line, int n, ssize_t* len) {
  char prefix[32];
  int er1 = 0;
  int er2 = 0;
  int res;
  char* new_line = NULL;
  res = snprintf(prefix, sizeof(prefix), "%6d\t", n);
  if (res < 0)
    er1 = 1;
  else if (res >= (int)sizeof(prefix))
    er2 = 1;
  if (!er1 && !er2) {
    size_t plen = strlen(prefix);
    size_t llen = (size_t)*len;
    if (llen <= SIZE_MAX - plen - 1) {
      size_t total = plen + llen + 1;
      new_line = malloc(total);
      if (new_line != NULL) {
        memcpy(new_line, prefix, plen);
        memcpy(new_line + plen, line, llen);
        new_line[total - 1] = '\0';
        *len = (ssize_t)(plen + llen);
      }
    }
  }
  return new_line;
}
char* number(char* line, int* n, ssize_t* len) {
  line = numbered_line(line, *n, len);
  (*n)++;
  return line;
}
char* number_nonblank(char* line, int* n, ssize_t* len, options opt) {
  if (!is_empty_line(line, opt)) {
    line = numbered_line(line, *n, len);
    (*n)++;
  }
  return line;
}
char* show_tabs(char* line, ssize_t* len, options opt) {
  line = show_nonprinting_one(line, len, (unsigned char)'\t', opt);
  return line;
}
char* show_ends(char* line, ssize_t* len, options opt) {
  line = show_nonprinting_one(line, len, (unsigned char)'\n', opt);
  return line;
}
char* show_nonprinting(char* line, ssize_t* len) {
  char* new_line = NULL;
  if (*len >= 0 && (size_t)*len <= (SIZE_MAX - 1) / 4) {
    size_t cap = (size_t)*len * 4 + 1;
    new_line = malloc(cap);
    if (new_line != NULL) {
      size_t out = 0;
      for (size_t i = 0; i < (size_t)*len; i++) {
        unsigned char uc = (unsigned char)line[i];
        if (uc == '\n' || uc == '\t') {
          new_line[out++] = uc;
        } else if (uc < '\x20') {
          new_line[out++] = '^';
          new_line[out++] = (int)uc + 64;
        } else if (uc == '\x7F') {
          new_line[out++] = '^';
          new_line[out++] = '?';
        } else if (uc >= (unsigned char)'\x80') {
          new_line[out++] = 'M';
          new_line[out++] = '-';
          unsigned char m = uc & '\x7F';
          if (m == '\x7F') {
            new_line[out++] = '^';
            new_line[out++] = '?';
          } else if (m < '\x20') {
            new_line[out++] = '^';
            new_line[out++] = (int)m + 64;
          } else
            new_line[out++] = m;
        } else
          new_line[out++] = uc;
      }
      new_line[out] = '\0';
      *len = (ssize_t)out;
    }
  }
  return new_line;
}
int squeeze_blank(const char* line, int* count_empty, options opt) {
  int is_empty = is_empty_line(line, opt);
  int skip = 0;
  if (is_empty)
    (*count_empty)++;
  else
    *count_empty = 0;
  if (*count_empty > 1) skip = 1;
  return skip;
}
void output(char* line, ssize_t len) { fwrite(line, 1, len, stdout); }
