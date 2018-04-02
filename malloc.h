#ifndef MALLOC_H
#define MALLOC_H

extern voidptr xmalloc  __P((size_t));

#define XMALLOC(type, size)       ((type *)xmalloc(sizeof(type)*(size)))
#endif /* ifndef MALLOC_H */
