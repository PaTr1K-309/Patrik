#ifndef S21_GREP_H
#define S21_GREP_H

#define _GNU_SOURCE
#include <getopt.h>
#include <regex.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define BUFFER_SIZE 1024
#define FLAGS_NAME "e:f:ivcnhosl"

typedef struct {
  int e, f, i, v, c, l, n, h, s, o;
  int CompteurFlag;
  int CompteurFile;
  char *filename;
  char *pattern;
} FlagsPattern;

void line_n(char *lineptr, int count_printed_line);
void AddPatterns(char **patterns, char *str);
void AddFilePatterns(char **patterns, char *str);
void FlagParser(FlagsPattern *arg, char opt, char **patterns);
void flag_c(FlagsPattern arg, FILE *file, regex_t *regex);
void flag_v(FlagsPattern arg, FILE *file, regex_t *regex);
void flag_l(FlagsPattern arg, FILE *file, regex_t *regex);
void flag_o(FlagsPattern arg, FILE *file, regex_t *regex);
void without_flag(FlagsPattern arg, FILE *file, regex_t *regex);
void output(FlagsPattern arg, FILE *file, regex_t *regex);

#endif