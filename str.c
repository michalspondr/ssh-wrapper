#include "includes.h"

extern char *strdup(const char *s);

/*
strdup s osetrenim chyb
*/

char *xstrdup(const char *s) {
	char *p;
	
	p = strdup(s);
	if (p == NULL) PERROR("xstrdup");
	return p;
}

