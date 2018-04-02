#include "includes.h"

/*
xmalloc - alokuje pamet, nemeni jeji obsah
*/
voidptr xmalloc(size_t size) {
	voidptr p;

	p = (voidptr)malloc(size);
	if (p == NULL) PERROR("xmalloc");
	return p;
}

