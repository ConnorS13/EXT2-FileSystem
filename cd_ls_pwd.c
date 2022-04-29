#include "functions.h"



/************* cd_ls_pwd.c file **************/
int cd()
{
  // printf("cd: under construction READ textbook!!!!\n");

  int ino = getino(pathname); // return error if ino=0
  printf("ino = %d\n", ino);
  MINODE *mip;
  mip = iget(dev, ino);

  // printf("is: %p\n", mip);

  // running->cwd = mip;

  // printf("is: %p\n", running->cwd);

  if (S_ISDIR(mip->INODE.i_mode)) // this function S_ISDIR checks if
  {
    // printf("HEY~!!!!!!!\n");
    iput(running->cwd); // release old cwd
    running->cwd = mip; // change cwd to mip
  }

  else
  {
    printf("This is not a directory!\n");
  }

  // READ Chapter 11.7.3 HOW TO chdir
}

int ls_file(MINODE *mip, char *name)
{
  // printf("ls_file: to be done: READ textbook!!!!\n");
  // READ Chapter 11.7.3 HOW TO ls

  char *t1 = "xwrxwrxwr-------";
   char *t2 = "----------------";
   struct stat fstat, *sp;
   int r, q;
   char ftime[64];

  if ((mip->INODE.i_mode & 0xF000) == 0x8000) // if (S_ISREG())
      printf("%c", '-');
   if ((mip->INODE.i_mode & 0xF000) == 0x4000) // if (S_ISDIR())
      printf("%c", 'd');
   if ((mip->INODE.i_mode & 0xF000) == 0xA000) // if (S_ISLNK())
      printf("%c", 'l');
   for (q = 8; q >= 0; q--)
   {
      if (mip->INODE.i_mode & (1 << q)) // print r|w|x
         printf("%c", t1[q]);
      else
         printf("%c", t2[q]); // or print -
   }

   printf("%4d ", mip->INODE.i_links_count); // link count
   printf("%4d ", mip->INODE.i_gid);         // gid
   printf("%4d ", mip->INODE.i_uid);         // uid
   printf("%8d ", mip->INODE.i_size);        // file size
   // print time
   strcpy(ftime, ctime(&mip->INODE.i_mtime)); // print time in calendar form
   ftime[strlen(ftime) - 1] = 0;              // kill \n at end
   printf("%s ", ftime);
   printf("\t%s", name);

  if ((mip->INODE.i_mode & 0xF000) == 0xA000)
  {
    /* char linkname[100] = "";
    MINODE* lmip = iget(dev, mip->ino);
    findmyname(root, lmip->ino, linkname);
    // use readlink() to read linkname
    if (strcmp(linkname, name) != 0)
      printf(" -> %s", linkname); // print linked name */

      int lengthSize = mip->INODE.i_size;

      char buf[BLKSIZE] = "";

      memcpy(buf, mip->INODE.i_block, lengthSize);

      if (strcmp(buf, name) != 0)
        printf(" -> %s", buf);
    
  }

  else if ((mip->INODE.i_mode & 0xF000) == 0xA000 && check == 0)
    ++check;

  printf("\n");
}

int ls_dir(MINODE *mip)
{
  // printf("ls_dir: list CWD's file names; YOU FINISH IT as ls -l\n");
  check = 0;
  char buf[BLKSIZE], temp[256];
  DIR *dp;
  char *cp;

  get_block(dev, mip->INODE.i_block[0], buf);
  dp = (DIR *)buf;
  cp = buf;
  
  while (cp < buf + BLKSIZE){
     strncpy(temp, dp->name, dp->name_len);
     temp[dp->name_len] = 0;
	
     // printf("%s  ", temp);

     mip = iget(dev, dp->inode);
     // getchar();
     ls_file(mip, temp);

     cp += dp->rec_len;
     dp = (DIR *)cp;
  }
  printf("\n");
}

int ls()
{
  // printf("ls: list CWD only! YOU FINISH IT for ls pathname\n");

  if (pathname[0] != 0)
  {
      int ino = getino(pathname); // return error if ino=0
      // printf("ino = %d\n", ino);
      MINODE *mip;
      mip = iget(dev, ino);
      ls_dir(mip);
  }

  else
    ls_dir(running->cwd);
}

void pwd(MINODE *wd)
{
 // printf("pwd: READ HOW TO pwd in textbook!!!!\n");
  if (wd == root){
    printf("/\n");
    return;
  }

  else
  {
      rpwd(wd);
      printf("\n");
  }
}

int rpwd(MINODE *wd)
{
  if (wd == root)
  {
    return;
  }

  char myname[64];

  MINODE *pip;

  int inoValue;

  int parent_ino = findino(wd, &inoValue);
  // printf("parent ino: %d %s\n", parent_ino, myname);
  pip = iget(dev, parent_ino);
  findmyname(pip, inoValue, myname);
  rpwd(pip);
  printf("/%s", myname); 
}