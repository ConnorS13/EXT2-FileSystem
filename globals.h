#ifndef GLOBALS
#define GLOBALS
#include <ext2fs/ext2_fs.h>
#include "type.h"

MINODE minode[NMINODE];
MINODE *root;
PROC   proc[NPROC], *running;

char gpath[128]; // global for tokenized components
char *name[64], *newName[64];  // assume at most 64 components in pathname
int   n, m, nfd;         // number of component strings

int fd, dev, check;
int nblocks, ninodes, bmap, imap, iblk;
char line[128], cmd[32], pathname[128];

OFT oft[64];

#endif