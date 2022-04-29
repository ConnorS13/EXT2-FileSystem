#ifndef FUNCTIONS
#define FUNCTIONS

// C Libs:
#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <ext2fs/ext2_fs.h>
#include <string.h>
#include <libgen.h>
#include <sys/stat.h>
#include <time.h>
#include <sys/types.h>
#include <unistd.h>


// USR
#include "type.h"
#include "globals.h"


/* util.c */
int get_block(int dev, int blk, char *buf);

int put_block(int dev, int blk, char *buf);

int tokenize(char *pathname);

MINODE *iget(int dev, int ino);

void iput(MINODE *mip);

int getino(char *pathname);

int findmyname(MINODE *parent, u32 myino, char myname[]);

int findino(MINODE *mip, u32 *myino);

/* cd_ls_pwd.c */
int cd();

int ls_file(MINODE *mip, char *name);

int ls_dir(MINODE *mip);

int ls();

void pwd(MINODE *wd);

/* alloc.c */

void create(MINODE *pmip, char *basename);

void kmkdir(MINODE *pmip, char *basename);

char* get_basename(char *path);

void enter_child(MINODE *pmip, int ino, char *basename);

int balloc(int dev);

int ialloc(int dev);

int decFreeBlocks(int dev);

int decFreeInodes(int dev);

int set_bit(char *buf, int bit);

int tst_bit(char *buf, int bit);

char *slimName(char *path);

int incFreeBlocks(int dev);

int incFreeInodes(int dev);

void rm_child(MINODE *mip, char *pName);

void rmdirFunc();

int check_empty(MINODE *mip);

int idalloc(int dev, int ino);

int bdalloc(int dev, int ino);

/* link.c */

char *basename(char *base);

void split_name(char *pathname);

char *dirname(char *base);

void ____link(char *nameValue);

void __symlink();

int __readlink(char *buffer);

void __unlink();

/* misc.c */

void __utime();

void __chmod(char *nameVal);

void __stat();

/* open.c */

int __open(char *filename, int flags);

void __truncate(MINODE *mip, int index);

void close_file(int fileD);

void pfd();

int __lseek(int fd, int position);

/* read.c */

int myread(int fd, char *buf, int nbytes);

/* write.c */

void mycp(char *dest);

void mywrite(int fd, char buf[], int nbytes);

#endif