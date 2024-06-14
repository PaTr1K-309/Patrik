#include <errno.h>
#include <getopt.h>
#include <stdbool.h>
#include <stdio.h>

#include "CatFlags.h"

typedef struct {
  bool NumberNonBlank;     //-b
  bool markEndl;           // -e
  bool numberAll;          // -n
  bool squeeze;            // -s
  bool tab;                //-t
  bool printNonPrintable;  //-v
} Flags;
//функция считывания флагов
Flags CatReadFlags(int argc, char *argv[]) {
  struct option longOptions[] = {{"number-nonblank", 0, NULL, 'b'},
                                 {"number", 0, NULL, 'n'},
                                 {"squeeze-blank", 0, NULL, 's'},
                                 {NULL, 0, NULL, 0}};
  int currentFlag = getopt_long(argc, argv, "bevEnstT", longOptions, NULL);
  Flags flags = {false, false, false, false, false, false};
  for (; currentFlag != -1;
       currentFlag = getopt_long(argc, argv, "bevEnstT", longOptions, NULL)) {
    switch (currentFlag) {
      case 'b':
        flags.NumberNonBlank = true;
        break;
      case 'e':
        flags.markEndl = true;
        flags.printNonPrintable = true;
        break;
      case 'v':
        flags.printNonPrintable = true;
        break;
      case 'E':
        flags.markEndl = true;
        break;
      case 'n':
        flags.numberAll = true;
        break;
      case 's':
        flags.squeeze = true;
        break;
      case 't':
        flags.tab = true;
        flags.printNonPrintable = true;
        break;
      case 'T':
        flags.tab = true;
        break;
    }
  }
  return flags;
}

void CatFile(FILE *file, Flags flags, const char *table[static 256],
             int *lineno, int *FileCount) {
  int c = 0;  // хранить текушии символы
  int last;
  bool squeeze = false;  // для обработки других аргументов
  last = '\n';
  int StringCount = 0;
  char buffer[256];
  if ((flags.markEndl && flags.tab && flags.printNonPrintable &&
       flags.squeeze) ||
      (flags.squeeze || flags.tab || flags.markEndl ||
       flags.printNonPrintable)) {
    while (fread(&c, 1, 1, file) > 0) {
      if (last == '\n') {
        if (flags.squeeze && c == '\n') {
          if (squeeze) continue;
          squeeze = true;
        } else {
          squeeze = false;
        }
      }
      if (!*table[c])
        printf("%c", '\0');
      else
        printf("%s", table[c]);
      last = c;
    }
  }
  if (flags.numberAll || flags.NumberNonBlank || flags.markEndl ||
      flags.printNonPrintable || flags.squeeze || flags.tab) {
    while (fgets(buffer, sizeof(buffer), file) != NULL) {
      StringCount++;
      if ((flags.NumberNonBlank)) {
        if ((buffer[0] != '\n')) {
          printf("%6i\t", ++*lineno);
          printf("%s", buffer);
        } else {
          printf("%s", buffer);
        }
      } else if (flags.numberAll) {
        if (*FileCount > 0) {
          printf("%6i\t", ++*lineno);
          printf("%s", buffer);
        }
      }
    }
  } else {
    while (fgets(buffer, sizeof(buffer), file) != NULL) {
      printf("%s", buffer);
    }
  }
}

void Cat(int argc, char *argv[], Flags flags, const char *table[static 256]) {
  int lineno = 0;
  int FileCout = 0;
  for (char **filename = &argv[1], **end = &argv[argc]; filename != end;
       ++filename) {
    if (**filename == '-') continue;
    FILE *file = fopen(*filename, "rb");
    if (errno) {
      fprintf(stderr, "%s", argv[0]);
      perror(*filename);
      continue;
    }
    FileCout++;
    CatFile(file, flags, table, &lineno, &FileCout);
    fclose(file);
  }
}

int main(int argc, char *argv[]) {
  Flags flags = CatReadFlags(argc, argv);
  const char *table[256];
  CatSetTable(table);
  if (flags.markEndl) {
    CatSetEndel(table);
  }
  if (flags.tab) {
    CatSetTab(table);
  }
  if (flags.printNonPrintable) {
    CarSetNonPrintable(table);
  }
  Cat(argc, argv, flags, table);
}
