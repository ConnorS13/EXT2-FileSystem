#include "functions.h"

void __utime()
{
    int oino = getino(pathname);
    MINODE *omip = iget(dev, oino);

    INODE *ip = &omip->INODE;

    ip->i_atime = ip->i_ctime = ip->i_mtime = time(0L);

    iput(omip);

    return;
}

void __stat()
{
    char ftime[1000];
    struct stat myst;

    int oino = getino(pathname);
    MINODE *omip = iget(dev, oino);

    myst.st_dev = dev;
    myst.st_ino = oino;
    myst.st_mode = omip->INODE.i_mode;
    myst.st_nlink = omip->INODE.i_links_count;
    myst.st_uid = omip->INODE.i_uid;
    myst.st_gid = omip->INODE.i_gid;
    myst.st_size = omip->INODE.i_size;
    myst.st_mtime = omip->INODE.i_mtime;

    printf("dev: %d, ino: %d, links: %d, uid: %d, gid: %d, size: %d, time: ", myst.st_dev, myst.st_ino, myst.st_nlink, myst.st_uid, myst.st_gid, myst.st_size);
    strcpy(ftime, ctime(&myst.st_mtime)); // print time in calendar form
    ftime[strlen(ftime) - 1] = 0;              // kill \n at end
    printf("%s \n", ftime);

    iput(omip);

    return;
}

void __chmod(char *mode)
{
    int oino = getino(mode);
    MINODE *omip = iget(dev, oino);

    int new = atoi(pathname);

    omip->INODE.i_mode |= new;

    omip->dirty = 1;

    iput(omip);
}