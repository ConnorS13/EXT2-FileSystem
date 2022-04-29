#include "functions.h"

int check_empty(MINODE *mip)
{
    char *cp, sbuf[1024];
    DIR *dp;
    int check = 0;

    if (mip->INODE.i_links_count > 2)
    {
        return -1;
    }

    else
    {
        get_block(dev, mip->INODE.i_block[0], sbuf);
        dp = (DIR *)sbuf;
        cp = sbuf;

        while (cp < (sbuf + 1024))
        {
            mip = iget(dev, dp->inode);
            cp += dp->rec_len;
            dp = (DIR *)cp;
            check++;
        }

        if (check > 2)
        {
            return -1;
        }
    }

    return 0;
}

void rm_child(MINODE *mip, char *pName)
{
    int size = 0, addlength = 0;
    char buf[BLKSIZE], temp[256], buffer[BLKSIZE];
    DIR *dp, *prev, *dpFull, *prev2, *prev3, *dpFinal;
    char *jp, *cp, *fp;

    get_block(dev, mip->INODE.i_block[0], buf);
    dp = (DIR *)buf;
    cp = buf;
    jp = cp;
    dpFull = dp;

    fp = cp;
    dpFinal = dp;

    while (jp < buf + BLKSIZE)
    {
        prev2 = dpFull;
        jp += dpFull->rec_len;
        dpFull = (DIR *)jp;
    }

    while (cp < buf + BLKSIZE)
    {
        strncpy(temp, dp->name, dp->name_len);
        temp[dp->name_len] = 0;

        // MAIN FUNCTION IS HYPOTHETICALLY DONE, FINISH THIS HERE!

        if (strcmp(temp, pName) == 0)
        {
            // size += dp->rec_len;
            break;
        }

        prev = dp;
        size += dp->rec_len;
        cp += dp->rec_len;
        dp = (DIR *)cp;
    }

    // printf("%p -- %p", dp, prev2);

    // FIRST AND ONLY ENTRY CASE
    if (dp->rec_len == BLKSIZE)
    {
        bdalloc(dev, mip->INODE.i_block[0]);
        mip->INODE.i_size -= 1024; // simply dealloc block and put size == 0
    }

    // LAST ENTRY IN THE BLOCK
    else if (dp == prev2)
    {
        prev->rec_len += dp->rec_len; // if it is the last entry in the block then length of the deleted is added to the previous. 
    }

    else // if not last entry and multiple others exist
    {
        int rec_len = 0;

        rec_len = dp->rec_len;

        cp += dp->rec_len;
        dpFull = (DIR *)cp;
        // memcpy(dp, prev2, BLKSIZE - size);

        memcpy(dp, dpFull, BLKSIZE - size);

        while (fp < (buf + BLKSIZE - size))
        {
            prev3 = dpFinal;
            fp += dpFinal->rec_len;
            dpFinal = (DIR *)fp;
        }

        // printf("PREV3: %s", prev3->name);
        prev3->rec_len += rec_len;
    }

    put_block(dev, mip->INODE.i_block[0], buf);

    return;
}

void rmdirFunc()
{
    MINODE *mip, *pmip;
    int ino = getino(pathname), pino = 0;
    mip = iget(dev, ino);

    if (!(S_ISDIR(mip->INODE.i_mode)) && mip->refCount != 1)
    {
        printf("This is a file\n"); // ensure that we are removing a directory
        return;
    }

    if (check_empty(mip) == -1) // make sure that the given directory is empty 
    {
        return;
    }

    int inoValue;

    // printf("%d mip\n", mip->ino);

    pino = findino(mip, &inoValue); // find the ino of the parent

    pmip = iget(mip->dev, pino); // get the minode ptr of the previous minode

    char myname[1024];

    findmyname(pmip, ino, myname); // find the name of the directory we are removing

    rm_child(pmip, myname); // rm_child! ^

    // printf("name: %s pino: %d\n",myname, pino);

    pmip->INODE.i_links_count = 1;
    pmip->dirty = 1;

    iput(pmip);

    bdalloc(mip->dev, mip->INODE.i_block[0]);
    idalloc(mip->dev, mip->ino);
    iput(mip);
}