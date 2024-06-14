#include "s21_grep.h"

int main(int argc, char *argv[]) {
  if (argc < 3) {
    fprintf(stderr, "Использование: grep <pattern> <file>\n");
    return 1;
  }

  FlagsPattern arg = {0};
  char *patterns = NULL;
  char opt;
  while ((opt = getopt(argc, argv, FLAGS_NAME)) != -1) {
    FlagParser(&arg, opt, &patterns);
  }

  if (patterns == NULL) {
    AddPatterns(&patterns, argv[optind++]);
  }

  regex_t regex;
  if (arg.i ? regcomp(&regex, patterns, REG_ICASE | REG_EXTENDED)
            : regcomp(&regex, patterns, REG_EXTENDED)) {
    fprintf(stderr, "fail regcomp");
    exit(1);
  }

  arg.CompteurFile = argc - optind;
  for (; optind < argc; optind++) {
    arg.filename = argv[optind];
    FILE *file = fopen(arg.filename, "r");
    if (file == NULL) {
      if (!arg.s) fprintf(stderr, "Ошибка открытия файла\n");
      continue;
    }

    output(arg, file, &regex);
    fclose(file);
  }
  regfree(&regex);
  free(patterns);
  return 0;
}

void FlagParser(FlagsPattern *arg, char opt, char **patterns) {
  switch (opt) {
    case 'e':
      arg->e = 1;
      AddPatterns(patterns, optarg);
      break;
    case 'f':
      arg->f = 1;
      AddFilePatterns(patterns, optarg);
      break;
    case 'i':
      arg->i |= REG_ICASE;
      break;
    case 'v':
      arg->v = 1;
      arg->CompteurFlag++;
      break;
    case 'c':
      arg->c = 1;
      arg->CompteurFlag++;
      break;
    case 'l':
      arg->l = 1;
      arg->CompteurFlag++;
      break;
    case 'n':
      arg->n = 1;
      break;
    case 'h':
      arg->h = 1;
      break;
    case 's':
      arg->s = 1;
      break;
    case 'o':
      arg->o = 1;
      arg->CompteurFlag++;
      break;
    default:
      exit(1);
  }
}

void line_n(char *lineptr, int count_printed_line) {
  char last_char_str = 'q';
  char *new_line = lineptr;
  while (*new_line != '\0') {
    last_char_str = *new_line;
    new_line++;
  }
  if (last_char_str != '\n' && count_printed_line > 0) {
    printf("\n");
  }
}

void AddPatterns(char **patterns, char *str) {
  if (*patterns == NULL) {
    *patterns = calloc(1, (strlen(str) + 1));
    strcat(*patterns, str);
  } else {
    if (strlen(*patterns) > 0) {
      *patterns = realloc(
          *patterns, (sizeof(char) * (strlen(*patterns) + strlen(str) + 3)));
      strcat(*patterns, "|");
      strcat(*patterns, str);
    }
  }
}

void AddFilePatterns(char **patterns, char *str) {
  char *line = NULL;
  size_t len = 0;
  FILE *file = fopen(str, "r");
  if (file == NULL) {
    fprintf(stderr, "Ошибка открытия файла\n");
    exit(1);
  }
  while (getline(&line, &len, file) != -1) {
    size_t newline_pos = strcspn(line, "\n");
    line[newline_pos] = '\0';
    AddPatterns(patterns, line);
  }
  free(line);
  fclose(file);
}

void output(FlagsPattern arg, FILE *file, regex_t *regex) {
  if (arg.l)
    flag_l(arg, file, regex);
  else if (arg.v && !arg.o && !arg.c)
    flag_v(arg, file, regex);
  else if (arg.o && !arg.v && !arg.c)
    flag_o(arg, file, regex);
  else {
    if (arg.c) flag_c(arg, file, regex);
    if ((arg.CompteurFlag == 0 || arg.i || arg.h || arg.n) &&
        (!arg.o && !arg.v))
      without_flag(arg, file, regex);
  }
}

void flag_l(FlagsPattern arg, FILE *file, regex_t *regex) {
  char *line = NULL;
  size_t len = 0;
  regmatch_t match;
  int con = 0, flag = 0;

  while (getline(&line, &len, file) != -1 && flag != 1) {
    if (arg.v ? !regexec(regex, line, 1, &match, 0) == 0
              : regexec(regex, line, 1, &match, 0) == 0) {
      ++con;
      if (con == 1) {
        printf("%s\n", arg.filename);
        flag = 1;
      }
    }
  }
  free(line);
}

void flag_v(FlagsPattern arg, FILE *file, regex_t *regex) {
  char *line = NULL;
  size_t len = 0;
  int count_string = 0;
  int number_string = 0;

  while (getline(&line, &len, file) != -1) {
    ++number_string;
    if (regexec(regex, line, 0, 0, 0) == 1) {
      if (arg.CompteurFile == 1 || arg.h) {
        if (regexec(regex, line, 0, 0, 0)) {
          ++count_string;
          arg.n ? printf("%d:%s", number_string, line) : printf("%s", line);
        }
      } else {
        arg.n ? printf("%s:%d:%s", arg.filename, number_string, line)
              : printf("%s:%s", arg.filename, line);
        ++count_string;
      }
      line_n(line, count_string);
    }
  }
  free(line);
}

void flag_o(FlagsPattern arg, FILE *file, regex_t *regex) {
  char *lineptr_o = NULL;
  size_t len_o = 0;
  int count_str = 1;
  while (getline(&lineptr_o, &len_o, file) != -1) {
    regmatch_t pmatch[1];
    regoff_t len;
    char *line = lineptr_o;
    while (!regexec(regex, line, 1, pmatch, 0)) {
      len = pmatch[0].rm_eo - pmatch[0].rm_so;
      if (arg.CompteurFile == 1 || arg.h) {
        arg.n ? printf("%d:%.*s\n", count_str, (int)len, line + pmatch[0].rm_so)
              : printf("%.*s\n", (int)len, line + pmatch[0].rm_so);
      } else {
        arg.n ? printf("%s:%d:%.*s\n", arg.filename, count_str, (int)len,
                       line + pmatch[0].rm_so)
              : printf("%s:%.*s\n", arg.filename, (int)len,
                       line + pmatch[0].rm_so);
      }
      line += pmatch[0].rm_eo;
    }
    count_str++;
  }
  free(lineptr_o);
}

void flag_c(FlagsPattern arg, FILE *file, regex_t *regex) {
  char *line = NULL;
  size_t len = 0;
  int count_string = 0;

  while (getline(&line, &len, file) != -1) {
    size_t newline_pos = strcspn(line, "\n");
    line[newline_pos] = '\0';
    if (arg.v ? regexec(regex, line, 0, 0, 0)
              : regexec(regex, line, 0, 0, 0) == 0) {
      count_string++;
    }
  }
  if (arg.CompteurFile == 1 || arg.h) {
    printf("%d\n", count_string);
  } else
    printf("%s:%d\n", arg.filename, count_string);
  free(line);
}

void without_flag(FlagsPattern arg, FILE *file, regex_t *regex) {
  char *line = NULL;
  size_t len = 0;
  regmatch_t match;
  int number_string = 0;
  int count_string = 0;

  while (getline(&line, &len, file) != -1) {
    ++number_string;
    if (regexec(regex, line, 1, &match, 0) == 0) {
      ++count_string;
      if (arg.CompteurFile == 1 || arg.h) {
        arg.n ? printf("%d:%s", number_string, line) : printf("%s", line);
      } else {
        arg.n ? printf("%s:%d:%s", arg.filename, number_string, line)
              : printf("%s:%s", arg.filename, line);
      }
      line_n(line, count_string);
    }
  }
  free(line);
}
