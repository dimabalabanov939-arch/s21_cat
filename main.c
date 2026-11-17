#include "base_cat.h"

int main(int argc, char* argv[]) {
  options opt = init_opt();
  static struct option long_opt[8];
  init_long_options_for_cat(long_opt);
  int option;
  while ((option = getopt_long(argc, argv, "benstvETA", long_opt, NULL)) !=
         -1) {
    switch (option) {
      case 'b':
        opt.number_nonblank = 1;
        break;
      case 'n':
        opt.number_all = 1;
        break;
      case 'T':
        opt.show_tabs = 1;
        break;
      case 't':
        opt.show_tabs = 1;
        opt.show_nonprinting = 1;
        break;
      case 'E':
        opt.show_ends = 1;
        break;
      case 'e':
        opt.show_ends = 1;
        opt.show_nonprinting = 1;
        break;
      case 's':
        opt.squeeze_blank = 1;
        break;
      case 'v':
        opt.show_nonprinting = 1;
        break;
      case 'A':
        opt.show_nonprinting = 1;
        opt.show_ends = 1;
        opt.show_tabs = 1;
        break;
      case '?':
        printf("s21_cat: invalid option -- '%d'", option);
        break;
    }
  }
  if (opt.number_nonblank) opt.number_all = 0;
  int n = 1;
  int is_error = 0;
  for (int i = optind; i < argc; i++) {
    char* filename = argv[i];
    is_error += read_and_transformations(filename, opt, &n);
  }
  int er = 0;
  if (is_error > 0) er = 1;
  return er;
}
