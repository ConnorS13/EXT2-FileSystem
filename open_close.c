#include "functions.h"

int __open(char *filename, int flags)
{
    int fdd = 0, index = 0, in = 0;

    MINODE *mip;

    int ino = getino(pathname);

    if (ino == 0)
    {
        create(mip, pathname);
        ino = getino(pathname);
    } // creat file with new ino

    mip = iget(dev, ino);

    if (((mip->INODE.i_mode & 0xF000) == 0x8000)) // check for regular file
        printf("");

    else
        return; // return if it is not!

    for (in = 0; in < 64; ++in)
    {
        if (oft[in].refcount == 0) // find the first empty one in the table
            break;
    }

    OFT *entry = &oft[in]; // define oft entry


    entry->mode = flags;
    entry->minodePtr = mip; // set permissions and then assign minodeptr
    entry->refcount = 1;


    if (flags == 0 || flags == 2)
        entry->offset = 0;

    else if (flags == 1)
    {
        __truncate(mip, in); // call truncate to clear all of the memory inside of the file!
        entry->offset = 0;
    }

    else if (flags == 3)
        entry->offset = mip->INODE.i_size; // if mode is append then the offset is the size of the file!

    else
    {
        printf("INCORRECT MODE\n"); // error checking for wrong mode!
        return;
    }

    if (flags == 0)
    {
        mip->INODE.i_atime = time(0L); // read change access time!
    }

    else
    {
        mip->INODE.i_atime = mip->INODE.i_mtime = time(0L); // change access and modify time
    }

    for (index = 0; proc[0].ofd[index] != NULL && index < NFD; ++index); // find the first entry in the proc struct array that is zero

    if (index == 10) // this is the max amnt of the array
        return;

    else
        proc[0].ofd[index] = entry;

    fd = index;

    ++nfd;

    pfd();

    // printf("offset: %d\n", proc[0].ofd[index]->offset);

    return index;
}

void __truncate(MINODE *mip, int index)
{
    int iblkBuf[256];

    // FINISH!!! STEP 1
    for (int i = 0; i < 12; ++i)
        bdalloc(dev, mip->INODE.i_block[i]);

    get_block(dev, mip->INODE.i_block[12], iblkBuf);

    for (int i = 0; i < 256; ++i)
    {
        if (iblkBuf[i])
        {
            bdalloc(dev, iblkBuf[i]);
        }
    }

    if (mip->INODE.i_block[13])
    {
        get_block(dev, mip->INODE.i_block[13], iblkBuf);
        int doubleIbuf[256];

        for (int i = 0; i < 256; ++i)
        {
            get_block(dev, iblkBuf[i], doubleIbuf);
            for (int j = 0; j < 256; ++j)
            {
                bdalloc(dev, doubleIbuf[j]);
            }
        }
    }

    mip->INODE.i_atime = mip->INODE.i_ctime = mip->INODE.i_mtime = time(0L);
    mip->INODE.i_size = 0;
    mip->dirty = 1;
}

void close_file(int fileD)
{
    // CHECK FD DO STEP ONE!!!!

    if (fd > nfd)
        return;

    OFT *oftp;

    // printf("fd: %d\n", fileD);

    if (running->ofd[fileD] != 0)
    {
        oftp = running->ofd[fileD];
        running->ofd[fileD] = 0;
        oftp->refcount--;

        if (oftp->refcount > 0)
            return;

        printf("fd %d closed\n", fileD);

        --nfd;

        MINODE *mip = oftp->minodePtr;
        iput(mip);
    }

    return;
}

void pfd()
{
    for (int x = 0; x < nfd; ++x)
    {
        if (running->ofd[x]->refcount == 0)
        {
            break;
        }

        printf("fd: %d, mode: %d, offset: %d, [%d, %d]\n", x, running->ofd[x]->mode, running->ofd[x]->offset, running->ofd[x]->minodePtr->ino, running->ofd[x]->minodePtr->dev);
    }
} // OPEN MODE 1 ONLY WORKS 1 TIME!!

int __lseek(int fd, int position)
{
    OFT *oftp;
    MINODE *mip;

    if (running->ofd[fd] != 0) // make sure this entry is not null
    {
        oftp = running->ofd[fd];
        mip = oftp->minodePtr;
    }

    if (position > mip->INODE.i_size || position < 1) // lseek error checking before changing oftp offset
    {
        return;
    }

    int originalPosition = oftp->offset;
    oftp->offset = position;

    return originalPosition;
}