#include "includes.h"

/*
vytvori pojmenovanou rouru
*/
int xmkfifo(const char *pathname, mode_t mode) {
	int retval;
	
	// vytvori pojmenovanou rouru FIFO s danymi pravy
	retval = mkfifo(pathname, mode);
	if(retval == -1) PERROR("mkfifo");
	return retval;
}

