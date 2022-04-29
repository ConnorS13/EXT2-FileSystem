#include "functions.h"

void mywrite(int fd, char *buf, int nbytes)
{
    int count = 0, lbk = 0, start = 0, blk = 0, intbuf[256];

    // printf("DO THIS: %s", buf);

    char wbuf[BLKSIZE], *cq = buf;

    MINODE *mip = oft[fd].minodePtr;

    while (nbytes)
    {
        lbk = oft[fd].offset / BLKSIZE;
        start = oft[fd].offset % BLKSIZE;

        if (lbk < 12)
        {
            if (mip->INODE.i_block[lbk] == 0)
            {
                mip->INODE.i_block[lbk] = balloc(mip->dev);
            }

            blk = mip->INODE.i_block[lbk];
        }

        else if (lbk >= 12 && lbk < (256 + 12))
        {
            // UNFINISHED
            if (mip->INODE.i_block[12] == 0)
            {
                mip->INODE.i_block[12] = balloc(mip->dev);
                get_block(mip->dev, mip->INODE.i_block[12], intbuf);// zero out block on disk this statement is correct {}
                bzero(intbuf, BLKSIZE);
                put_block(mip->dev, mip->INODE.i_block[12], intbuf);
            }

            get_block(dev, mip->INODE.i_block[12], intbuf);
            blk = intbuf[lbk - 12];

            int intbuf2[256];

            if (blk == 0)
            {
                intbuf[lbk - 12] = balloc(mip->dev);
                get_block(mip->dev, intbuf[lbk - 12], intbuf2);
                bzero(intbuf2, BLKSIZE);
                put_block(mip->dev, intbuf[lbk - 12], intbuf2);
                // DO THIS!! done?
                blk = intbuf[lbk - 12];
            }

            put_block(mip->dev, mip->INODE.i_block[12], intbuf);
        }

        else
        {
            int ibuf[256];

            if (mip->INODE.i_block[13] == 0)
            {
                mip->INODE.i_block[13] = balloc(mip->dev);
                get_block(mip->dev, mip->INODE.i_block[13], (char*)(ibuf)); // zero out block on disk?
                bzero((char*)(ibuf), BLKSIZE);
                put_block(mip->dev, mip->INODE.i_block[13], (char*)(ibuf));
            }

            get_block(dev, mip->INODE.i_block[13], (char*)(ibuf)); // 13
            int lbkset = (lbk - 268) / 256;
            int lbkoffset = (lbk - 268) % 256;
            // get_block(dev, ibuf[lbkset], (char*)(ibuf));
            // blk = ibuf[lbkset];

            int intbuf2[256];

            if (ibuf[lbkset] == 0)
            {
                ibuf[lbkset] = balloc(mip->dev);
                get_block(mip->dev, ibuf[lbkset], intbuf2);
                bzero(intbuf2, BLKSIZE);
                put_block(mip->dev, ibuf[lbkset], intbuf2);
                blk = ibuf[lbkset];
            }

            get_block(mip->dev, ibuf[lbkset], intbuf2);

            int intbuf3[256];

            if (intbuf2[lbkoffset] == 0)
            {
                intbuf2[lbkoffset] = balloc(mip->dev);
                get_block(mip->dev, intbuf2[lbkoffset], intbuf3);
                bzero(intbuf3, BLKSIZE);
                put_block(mip->dev, intbuf2[lbkoffset], intbuf3);
            }

            blk = intbuf2[lbkoffset];
            put_block(mip->dev, ibuf[lbkset], intbuf2);
            put_block(mip->dev, mip->INODE.i_block[13], ibuf);
        }

        get_block(mip->dev, blk, wbuf); // read disk block into wbuf[ ]
        char *cp = wbuf + start;        // cp points at startByte in wbuf[]
        int remain = BLKSIZE - start;   // number of BYTEs remain in this block

        /* while (remain > 0)
        {                 
            *cp++ = *cq++; 
            nbytes--; // decrement the number of bytes remaining. 
            remain--;                          
            oft[fd].offset++;                   
            if (oft[fd].offset > mip->INODE.i_size) 
                mip->INODE.i_size++;               
            if (nbytes <= 0)
                break;
        } */

        if (nbytes > remain)
        {
            memcpy(cp, cq, remain);
            oft[fd].offset += remain;
            if (oft[fd].offset > mip->INODE.i_size) 
                mip->INODE.i_size += remain; 
            nbytes -= remain;
            count += remain;
        }

        else
        {
            memcpy(cp, cq, nbytes);
            oft[fd].offset += nbytes;
            if (oft[fd].offset > mip->INODE.i_size) 
                mip->INODE.i_size += nbytes; 
            count += nbytes;
            nbytes = 0;
        }


        // printf("wbuf: %s", wbuf);

        put_block(mip->dev, blk, wbuf); // write wbuf[ ] to disk
        mip->dirty = 1;
        iput(mip);
    }
}

void mycp(char *dest)
{
    int gd = 0;
    MINODE *mip;

    int fd = __open(pathname, 0), x = 0;
    char buf[BLKSIZE];

    int check = getino(dest);

    strcpy(pathname, dest);

    if (check == 0)
    {
        create(mip, dest);
        gd = __open(dest, 1);
    }

    // strcpy(pathname, dest);

    if (gd == 0)
        gd = __open(dest, 1);

    // printf("FD: %d, pathname: %s", fd, pathname);
    // return;

    while (x = myread(fd, buf, BLKSIZE))
    {
        buf[x] = 0;
        // printf("buf: its working --> %s, gd is: %d", buf, gd);
        // char* cp = buf;
        // strcpy(buf, "hello");
        mywrite(gd, buf, x);
    }

    close_file(fd);
    close_file(gd);
}