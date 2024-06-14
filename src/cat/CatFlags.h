#ifndef CAT_CATFLAGS_H
#define CAT_CATFLAGS_H

void CatSetTable(const char *table[static 256]);
void CatSetEndel(const char *table[static 256]);
void CatSetTab(const char *table[static 256]);
void CarSetNonPrintable(const char *table[static 256]);

#endif
