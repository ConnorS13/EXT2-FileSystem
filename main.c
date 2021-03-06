/****************************************************************************
 *                   KCW: mount root file system                             *
 *****************************************************************************/
// C LIBS:
#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <ext2fs/ext2_fs.h>
#include <string.h>
#include <libgen.h>
#include <sys/stat.h>
#include <time.h>

// USR LIBS:
#include "globals.h"
#include "type.h"
#include "functions.h"

// forward declarations of funcs in main.c
void tokenize1(char *lin);
int init();
int quit();
int mount_root();

char *disk = "diskimage"; // diskimage

int main(int argc, char *argv[])
{
  int ino, nfd = 0;
  char buf[BLKSIZE];
  MINODE *mip;

  printf("checking EXT2 FS ....");
  if ((fd = open(disk, O_RDWR)) < 0)
  {
    printf("open %s failed\n", disk);
    exit(1);
  }

  dev = fd; // global dev same as this fd
  // iblk = gp->bg_inode_table;

  /********** read super block  ****************/
  get_block(dev, 1, buf);
  sp = (SUPER *)buf;

  /* verify it's an ext2 file system ***********/
  if (sp->s_magic != 0xEF53)
  {
    printf("magic = %x is not an ext2 filesystem\n", sp->s_magic);
    exit(1);
  }
  // change iblk variable!!!!!!!!!!!!!
  printf("EXT2 FS OK\n");
  ninodes = sp->s_inodes_count;
  nblocks = sp->s_blocks_count;

  get_block(dev, 2, buf);
  gp = (GD *)buf;

  bmap = gp->bg_block_bitmap;
  imap = gp->bg_inode_bitmap;
  iblk = gp->bg_inode_table;
  printf("bmp=%d imap=%d inode_start = %d\n", bmap, imap, iblk);

  init();
  mount_root();
  printf("root refCount = %d\n", root->refCount);

  printf("creating P0 as running process\n");
  running = &proc[0];
  running->cwd = iget(dev, 2);
  printf("root refCount = %d\n", root->refCount);

  // WRTIE code here to create P1 as a USER process

  bzero(buf, 1024);

  while (1)
  {
    printf("input command : [ls|cd|pwd|mkdir|creat|rmdir|link|symlink|readlink|unlink|stat|chmod|utime|open|close|read|cat|write|cp|quit] ");
    fgets(line, 128, stdin);
    line[strlen(line) - 1] = 0;

    tokenize1(line);

    if (line[0] == 0)
      continue;
    pathname[0] = 0;

    sscanf(line, "%s %s", cmd, pathname);
    printf("cmd=%s pathname=%s\n", cmd, pathname);

    if (strcmp(cmd, "ls") == 0)
      ls();
    else if (strcmp(cmd, "cd") == 0)
    {
      if (strcmp(line, "cd ") == 0 || strcmp(line, "cd") == 0)
      {
        running->cwd = root;
      }
      else
      {
        cd();
      }
    }
    else if (strcmp(cmd, "pwd") == 0)
      pwd(running->cwd);
    else if (strcmp(cmd, "pfd") == 0)
      pfd();
    else if (strcmp(cmd, "mkdir") == 0)
      kmkdir(mip, pathname);
    else if (strcmp(cmd, "creat") == 0)
      create(mip, pathname);
    else if (strcmp(cmd, "rmdir") == 0)
      rmdirFunc();
    else if (strcmp(cmd, "link") == 0)
      ____link(name[2]);
    else if (strcmp(cmd, "symlink") == 0)
      __symlink(name[2]);
    else if (strcmp(cmd, "unlink") == 0)
      __unlink();
    else if (strcmp(cmd, "readlink") == 0)
      __readlink(name[2]);
    else if (strcmp(cmd, "chmod") == 0)
      __chmod(name[2]);
    else if (strcmp(cmd, "utime") == 0)
      __utime();
    else if (strcmp(cmd, "stat") == 0)
      __stat();
    else if (strcmp(cmd, "open") == 0)
      __open(name[1], atoi(name[2]));
    else if (strcmp(cmd, "close") == 0)
      close_file(atoi(name[1]));
    else if (strcmp(cmd, "read") == 0)
      myread(atoi(name[1]), buf, atoi(name[2]));
    else if (strcmp(cmd, "cat") == 0)
      cat_file();
    else if (strcmp(cmd, "write") == 0)
      mywrite(atoi(name[1]), name[2], strlen(name[2]));
    else if (strcmp(cmd, "cp") == 0)
      mycp(name[2]);
    else if (strcmp(cmd, "lseek") == 0)
      __lseek(atoi(name[0]), atoi(name[1]));
    else if (strcmp(cmd, "quit") == 0)
      quit();
  }
}

int quit()
{
  int i;
  MINODE *mip;
  for (i = 0; i < NMINODE; i++)
  {
    mip = &minode[i];
    if (mip->refCount > 0)
      iput(mip);
  }
  exit(0);
}

void tokenize1(char *lin)
{
  int i;
  char *s;
  printf("tokenize %s\n", lin);

  strcpy(gpath, lin); // tokens are in global gpath[ ]
  n = 0;

  s = strtok(gpath, " ");
  while (s)
  {
    name[n] = s;
    n++;
    s = strtok(0, " ");
  }
  name[n] = 0;

  for (i = 0; i < n; i++)
    printf("%s  ", name[i]);
  printf("\n");
}

int init()
{
  int i, j;
  MINODE *mip;
  PROC *p;

  printf("init()\n");

  for (i = 0; i < NMINODE; i++)
  {
    mip = &minode[i];
    mip->dev = mip->ino = 0;
    mip->refCount = 0;
    mip->mounted = 0;
    mip->mptr = 0;
  }
  for (i = 0; i < NPROC; i++)
  {
    p = &proc[i];
    p->pid = i;
    p->uid = p->gid = 0;
    p->cwd = 0;

    for (int x = 0; x < 2; ++x)
      p->ofd[x] = 0;

  }

  for (i = 0; i < 64; i++)
  {
    oft[i].refcount = 0; 
  }
}

// load root INODE and set root pointer to it
int mount_root()
{
  printf("mount_root()\n");
  root = iget(dev, 2);
}