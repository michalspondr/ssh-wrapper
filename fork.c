#include "includes.h"

/* rozvetvi proces
obsahuje i kontrolu
*/
pid_t xfork() {
	pid_t pid;

	// rozdel proces
	pid = fork();
	if(pid == -1) PERROR("xfork");

	return pid;
}

