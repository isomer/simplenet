#ifndef DEFS_H
#define DEFS_H 1

/* Some platforms are missing some definitions, define them here. */
#ifndef SOCK_DCCP
#define SOCK_DCCP 6
#endif

#ifndef IPPROTO_DCCP
#define IPPROTO_DCCP 33
#endif

#ifndef IPPROTO_MPTCP
#define IPPROTO_MPTCP 252
#endif


#endif
