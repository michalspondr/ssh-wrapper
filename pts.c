#include "includes.h"

extern int getpt(void);

/* otevre pseudoterminal
obsahuje i kontrolu
*/
int openpts() {
	int fd;
	void (*sigcld)__P((int));

	fd = getpt();	// otevre pseudoterminal
	if (fd < 0) PERROR("getpt");	// nepovedlo se otevrit
	sigcld = signal(SIGCHLD, SIG_DFL);
	if(grantpt(fd) < 0) PERROR("grantpt");	// zmeni vlastnika pseudoterminalu
	signal(SIGCHLD, sigcld);
	if(unlockpt(fd) < 0) PERROR("unlockpt");// odemce pseudoterminal
	return fd;
}

