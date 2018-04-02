#include "includes.h"

#undef  PACKAGE_NAME
#define PACKAGE_NAME "ssh-wrapper"

extern int xmkfifo(const char *pathname, mode_t mode);

static int init_opt __P((int, char **));
static void usage __P((int));

static char *arg_host = NULL;		// hostitel
static char *arg_id   = NULL;		// id spojeni pro wrapper
static char *arg_user = NULL;		// uzivatel
static pid_t child_pid = 0;
static char *pid_file = NULL, *fifo_r = NULL, *fifo_w = NULL;	// nazvy souboru rour apod.
static int ptsm = 0;			// deskriptor pseudoterminalu


int main(int argc, char **argv) {
int ffdr, ffdw;

// pri obdrzeni nasledujicich signalu vsechno ukonci
signal(SIGKILL, all_close);
signal(SIGINT,  all_close);
signal(SIGTERM, all_close);
signal(SIGCHLD, all_close);

// nacti parametry z prikazove radky
if (init_opt(argc, argv) == -1) usage(EXIT_FAILURE);	// vypis navod k pouziti, pokud je neco spatne

// otevri pseudoterminal
ptsm = openpts();
tcflush(ptsm, TCIOFLUSH);	// flushne data
// pustime na pozadi
daemon(0, 0);

// samotne ssh
child_pid = xfork();		// vytvorime proces
if(child_pid == 0) {		// potomek
	int fd;
	char *user_host;
	
	// hostitel
	user_host = XMALLOC(char, strlen(arg_host)+strlen(arg_user)+2);	// alokace pameti pro hostitele
	sprintf(user_host, "%s@%s", arg_user, arg_host);		// a zapsani do promenne
	
	// zavru
	if ((close(0)) == -1) PERROR("close(0)");	// STDIN
	if ((close(1)) == -1) PERROR("close(1)");	// STDOUT
	if ((close(2)) == -1) PERROR("close(2)");	// STDERR

	// otevru pseudoterminal
	setsid();	// otevru program v novem sezeni
	if ((fd = open(ptsname(ptsm), O_RDWR)) == -1) PERROR("open ptsm");	// otevri pseudoterminal pro cteni a zapis; fd=0

	// duplikuji 2x STDIN - vznikne tak vice zaznamu na STDIN
	if ((dup(0)) == -1) PERROR("dup(0) - 1");
	if ((dup(0)) == -1) PERROR("dup(0) - 2");

	/*
	spustim externi shell
	nezalezi na hostitelskem klici
	ani na jeho IP adrese

	proces nahradi aktualni proces
	*/
	execlp(
	"ssh", "ssh",
	"-o", "StrictHostKeyChecking=no",
	"-o", "CheckHostIP=no",
	user_host, NULL
	);
	
	// pokud nenahradil, tak je neco spatne - chyba!
	PERROR("execlp");
}
// sem se dostane jen rodic
int len;
const char *form_pid = RUN_DIR"/"PACKAGE_NAME".pid.%s";	// v RUN_DIR adresari vytvor soubor s PID procesu
pid_t pid;
FILE *fp;

// vytvori soubor pro PID procesu
pid = getpid();				// zjisti PID procesu
len = strlen(form_pid)+strlen(arg_id);
pid_file = XMALLOC(char, len);
sprintf(pid_file, form_pid, arg_id);	// pojmenuj soubor

// otevri pro zapis a zapis tam cislo procesu
if ((fp = fopen(pid_file, "w")) == NULL) {
	PERROR("open pid_file");
}
fprintf(fp, "%i", pid);
fclose(fp);

// roura fifo
const char *form_fifo_r = RUN_DIR"/"PACKAGE_NAME".fifo.r.%s";	// pro cteni
const char *form_fifo_w = RUN_DIR"/"PACKAGE_NAME".fifo.w.%s";	// pro zapis


// alokace pameti pro nazvy rour a zapis nazvu souboru do nich
len = strlen(form_fifo_r)+strlen(arg_id);
fifo_r = XMALLOC(char, len);
fifo_w = XMALLOC(char, len);
sprintf(fifo_r, form_fifo_r, arg_id);
sprintf(fifo_w, form_fifo_w, arg_id);

// vytvorim pojmenovane vstupni a vystupni roury pro cteni a zapis vsem
xmkfifo(fifo_r, 0666);
xmkfifo(fifo_w, 0666);

// otevru roury
if ((ffdw = open(fifo_r, O_WRONLY)) == -1) PERROR("open fifo_r");	// vstupni cast roury - musi byt otevrena cteci cast roury!
if ((ffdr = open(fifo_w, O_RDONLY)) == -1) PERROR("open fifo_w");	// vystupni cast roury - je otevrena driv, nez zapisova cast v PHP skriptu

// hlavni smycka
while(1) {
	int retval;
	fd_set rfds;		// mnozina deskriptoru souboru
	struct timeval tv;
	char buf[BUFSIZ];

	FD_ZERO(&rfds);		// vymazat mnozinu deskriptoru
	FD_SET(ptsm, &rfds);	// pridat deskriptor terminalu
	FD_SET(ffdr, &rfds);	// pridat vystupni cast roury
	tv.tv_sec = 300;	// kontrola deskriptoru po 300 sekundach
	tv.tv_usec = 0;
	retval = select(ffdr+1, &rfds, NULL, NULL, &tv);	// kontrola, zda se nezmenily deskriptory cteni
	
	if(retval == -1) {			// chyba v select 
		perror("select");
		all_close(EXIT_FAILURE);
		break;
	} else if (retval == 0) {		// timeout
		all_close(EXIT_FAILURE);
		break;
	}
	
	if(FD_ISSET(ptsm, &rfds)) {		// zmenil se deskriptor pseudoterminalu
		int n;
		n = read(ptsm, buf, sizeof(buf));	// prectu, co je v deskriptoru
		if (n <= 0) continue;			// nic? tak pokracovat
		write(ffdw, buf, n);			// z terminalu poslu na vstupni stranu roury
	}

	if (FD_ISSET(ffdr, &rfds)) {		// zmenil se deskriptor vystupni casti roury
		int n;
		n = read(ffdr, buf, sizeof(buf));	// prectu, co je na vystupni casti roury
		if (n <= 0) continue;			// nic? tak pokracovat
		write(ptsm, buf, n);			// z vystupni casti roury poslat na terminal
	}
}

// uspesne ukonceni - nemelo by sem dojit, jen tak pro jistotu
all_close(EXIT_SUCCESS);
return 0;
}

/*
uzavreni vsech souboru
*/
void all_close(int retval) {
	if (ptsm) close(ptsm);				// zavrit pseudoterminal
	if (child_pid) kill(child_pid, SIGKILL);	// zabit potomka
	if (pid_file != NULL) unlink(pid_file);		// smazat soubor s nazvem potomka
	if (fifo_r != NULL) unlink(fifo_r);		// smazat cteci rouru
	if (fifo_w != NULL) unlink(fifo_w);		// smazat zapisovou rouru
	exit(retval);					// ukoncit program
}

/*
nacte prislusne parametry z prikazove radky
*/
static int init_opt(int argc, char **argv) {
	int c, hflag, iflag, uflag;

	hflag = iflag = uflag = 0;
	while ((c = getopt(argc, argv, "h:i:u:")) > 0) {
		switch ((char)c) {
			case 'h':
				hflag = 1;
				arg_host = xstrdup(optarg);
				break;
			case 'i':
				iflag = 1;
				arg_id = xstrdup(optarg);
				break;
			case 'u':
				uflag = 1;
				arg_user = xstrdup(optarg);
				break;
			default:
				return -1;
		}
	}
	
	if (!hflag || !iflag || !uflag) return -1;
	
	argc -= optind;
	argv += optind;

	if (argc) return -1;
	return 0;
}

/*
navod k pouziti
*/
static void usage(int retval) {
	fprintf(stderr, "usage: "PACKAGE_NAME" -h host -u user -i id\n");
	EXIT(retval);
}

