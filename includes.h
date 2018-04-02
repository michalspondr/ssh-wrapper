#ifndef INCLUDES_H
#define INCLUDES_H

#define _XOPEN_SOURCE

#include "defines.h"	// defaultni cesta k souborum
#include <inttypes.h>
#include <memory.h>
#include <stdint.h>
#include <stdlib.h>
#include <strings.h>
#include <string.h>
#include <sys/select.h>
#include <sys/socket.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>
#include <sys/time.h>
#include <time.h>
#include <stdarg.h>
#include <ansidecl.h>
#include <ctype.h>
#include <errno.h>
#include <fcntl.h>
#include <pty.h>
#include <signal.h>
#include <stdio.h>
#include <sys/ioctl.h>
#include <termios.h>

#ifndef __P
	#if __STDC__
		#define __P(x) x
	#else
		#define __P(x) ()
	#endif
#endif

#if __STDC__
typedef void *voidptr;
#else
typedef char *voidptr;
#endif

#include "fifo.h"
#include "fork.h"
#include "malloc.h"
//#include "pipe.h"
#include "pts.h"
#include "ssh-wrapper.h"
#include "str.h"

#ifndef PERROR
	#define PERROR(s) perror(s),EXIT(EXIT_FAILURE)
#endif
#endif

