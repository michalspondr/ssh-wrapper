#ifndef SSH_WRAPPER_H
#define SSH_WRAPPER_H

#define RUN_DIR LOCALSTATEDIR"/run"

extern void all_close __P((int));

#define EXIT(retval) all_close(retval)

#endif /* ifndef SSH_WRAPPER_H */
