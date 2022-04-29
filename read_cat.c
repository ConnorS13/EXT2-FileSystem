#include "functions.h"

int myread(int fd, char *buf, int nbytes)
{
    int count = 0, offset = 0, lbk = 0, startByte = 0, remain = 0, blk = 0;
    char *cq = buf, *readBuf = buf; // , readBuf[BLKSIZE];

    offset = oft[fd].offset;

    MINODE *mip = oft[fd].minodePtr;

    int avil = mip->INODE.i_size - offset; // this is size of file - offset from the oft

    // printf("avil: %d, ino: %d", mip->ino);

    while (nbytes && avil)
    {
        lbk = oft[fd].offset / BLKSIZE;
        startByte = oft[fd].offset % BLKSIZE;

        if (lbk < 12)
        {
            blk = mip->INODE.i_block[lbk];
        }

        else if (lbk >= 12 && lbk < (256 + 12))
        {
            int ibuf[256];
            // indirect blocks

            get_block(dev, mip->INODE.i_block[12], (char *)ibuf);

            // printf("current block: %d \n", ibuf[lbk - 12]);

            blk = ibuf[lbk - 12];
        }

        else
        {
            int ibuf[256];
            get_block(dev, mip->INODE.i_block[13], (char *)(ibuf));
            int lbkset = (lbk - 268) / 256;
            int lbkoffset = (lbk - 268) % 256;
            get_block(dev, ibuf[lbkset], (char *)(ibuf));
            blk = ibuf[lbkoffset];
        }

        get_block(mip->dev, blk, readBuf);
        char *cp = readBuf + startByte;
        remain = BLKSIZE - startByte; // number of bytes remain in readbuf[]

        // printf("remain: %d\n", remain);

        /* while (remain > 0)
        {
            *cq++ = *cp++;  // copy byte from readbuf[] into buf[]
            oft[fd].offset++; // oftp->offset++; // advance offset
            count++;        // inc count as number of bytes read
            avil--, nbytes--, remain--;

            if (nbytes <= 0 || avil <= 0)
                break;
        } */

        if (avil > remain)
        {
            memcpy(buf, cp, remain);
            oft[fd].offset += remain;
            nbytes -= remain;
            avil -= remain;
            count += remain;
        }

        else
        {
            memcpy(cq, cp, avil);
            oft[fd].offset += avil;
            nbytes -= avil;
            count += avil;
            avil = 0;
        }

        // printf("%s!!!", buf);
    }

    // printf("buf: %s \ncount = %d", buf, count);

    return count;
}

void cat_file()
{
    char myBuf[BLKSIZE], dummy = 0;
    int n = 0;

    int fileD = __open(pathname, 0);

    if (fileD > 10)
    {
        printf("Not a file!\n");
        return;
    }

    printf("fileD: %d, path: %s\n", fileD, pathname);

    while (n = myread(fileD, myBuf, BLKSIZE))
    {
        myBuf[n] = 0;        // as a null terminated string
        printf("%s", myBuf); //   <=== THIS works but not good
        // return; // spit out chars from mybuf[] but handle \n properly;
        if (n < 1024)
            break;
    }

    return;
}