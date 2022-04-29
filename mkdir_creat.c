#include "functions.h"

void create(MINODE *pmip, char *basename)
{
    int ino = 0, pino = 0, x = 1, blk = 0;

    char dirname[1024], path[1024];

    tokenize(basename);

    if (name[1])
    {
        strcpy(path, name[0]);
        for (; x < (n - 1); ++x)
        {
            strcat(path, "/");
            strcat(path, name[x]);
        }
        strcpy(dirname, name[x]);
    }

    else
    {
        pino = 2;
        strcpy(dirname, name[0]);
        strcpy(path, "/");
    }

    // printf("path: %s, dirname: %s", path, dirname);

    if (pino == 0)
        pino = getino(path);

    // printf("pino: %d\n", pino);
    pmip = iget(dev, pino);

    int result = search(pmip, basename);

    if (S_ISDIR(pmip->INODE.i_mode) && result == 0)
    {
        ino = ialloc(dev);
        blk = balloc(dev);

        MINODE *mip = iget(dev, ino);
        INODE *ip = &mip->INODE;
        ip->i_mode = 0x81A4;      // 040755: DIR type and permissions
        ip->i_uid = running->uid; // owner uid
        ip->i_gid = running->gid; // group Id
        ip->i_size = 0;           // size in bytes
        ip->i_links_count = 1;    // links count=2 because of . and ..
        ip->i_atime = ip->i_ctime = ip->i_mtime = time(0L);
        ip->i_blocks = 2;     // LINUX: Blocks count in 512-byte chunks
        ip->i_block[0] = blk; // new DIR has one data block

        for (int x = 1; x < 15; ++x)
        {
            ip->i_block[x] = 0;
        }

        mip->dirty = 1; // mark minode dirty

        char buf[1024];
        bzero(buf, 1024); // optional: clear buf[ ] to 0
        DIR *dp = (DIR *)buf;
        // make . entry
        dp->inode = ino;
        dp->rec_len = 12;
        dp->name_len = 1;
        dp->name[0] = '.';
        // make .. entry: pino=parent DIR ino, blk=allocated block
        dp = (char *)dp + 12;
        dp->inode = pino;
        dp->rec_len = 1024 - 12; // rec_len spans block
        dp->name_len = 2;
        dp->name[0] = dp->name[1] = '.';
        put_block(dev, blk, buf); // write to blk on diks */

        enter_child(pmip, ino, dirname);

        iput(mip);
        // (4).3
    }

    return;
}

void enter_child(MINODE *pmip, int ino, char *basename)
{
    char buffer[1024], *cp;
    DIR *dp;
    int ideal_length = 0;
    int x = 0;

    for (x = 0; x < 12; ++x)
    {
        if (pmip->INODE.i_block[x] == 0)
        {
            break; // break if block is empty
        }

        get_block(pmip->dev, pmip->INODE.i_block[x], buffer); // get the direct block

        dp = (DIR *)buffer;
        cp = buffer;

        while (cp + dp->rec_len < buffer + BLKSIZE)
        {
            cp += dp->rec_len;
            // printf("dp->rec_len: %d", dp->rec_len);
            dp = (DIR *)cp; // THIS IS SORTING THROUGH THE DIRECTORY 
        }

        ideal_length = 4 * ((8 + dp->name_len + 3) / 4); // a multiple of 4
        int need_length = 4 * ((8 + strlen(basename) + 3) / 4);

        int remain = dp->rec_len - ideal_length; // get remaining length to assign as rec_len of new entry!

        if (remain >= need_length)
        {
            // do this enter the new entry as the LAST entry
            ideal_length = 4 * ((8 + dp->name_len + 3) / 4);

            dp->rec_len = ideal_length; // trim previous entry rec_len to its ideal_length
            dp = (DIR *)((char *)dp + ideal_length);

            dp->inode = ino;
            strcpy(dp->name, basename);
            dp->rec_len = remain;
            dp->name_len = strlen(basename);

            // printf("\n%d, %d, %s, %d", dp->inode, dp->rec_len, dp->name, dp->name_len);

            put_block(pmip->dev, pmip->INODE.i_block[x], buffer);
            return;
        }

        else
        {
            int blk = balloc(pmip->dev);

            dp->rec_len = BLKSIZE;
            dp->inode = ino;
            strcpy(dp->name, basename);
            dp->rec_len = remain;
            dp->name_len = strlen(basename);
            put_block(pmip->dev, blk, buffer);
        }
    }
}

char *get_basename(char *pathname)
{
    int i;
    char *s;
    printf("tokenize %s\n", pathname);

    strcpy(gpath, pathname); // tokens are in global gpath[ ]
    n = 0;

    s = strtok(gpath, "/");
    while (s)
    {
        name[n] = s;
        n++;
        s = strtok(0, "/");
    }
    name[n] = 0;

    for (i = 0; i < n; i++)
        printf("%s  ", name[i]);
        
    printf("\n");
}

void kmkdir(MINODE *pmip, char *basename)
{
    int ino = 0, pino = 0, x = 1, blk = 0;

    char dirname[1024], path[1024];

    tokenize(basename);

    if (name[1])
    {
        strcpy(path, name[0]);
        for (; x < (n - 1); ++x)
        {
            strcat(path, "/");
            strcat(path, name[x]);
        }
        strcpy(dirname, name[x]);
    }

    else
    {
        pino = 2;
        strcpy(dirname, name[0]);
        strcpy(path, "/");
    }

    // printf("path: %s, dirname: %s", path, dirname);

    if (pino == 0)
        pino = getino(path); // redundant code, all code above is for parsing the string!

    // printf("pino: %d\n", pino);
    pmip = iget(dev, pino); // get parent MINODE 

    int result = search(pmip, basename); // this is checking to make sure dir/file is not already made

    if (S_ISDIR(pmip->INODE.i_mode) && result == 0) // making sure it is a directory
    {
        ino = ialloc(dev);

        blk = balloc(dev);

        MINODE *mip = iget(dev, ino);
        INODE *ip = &mip->INODE;

        /*      *ip = (INODE){                  // for some reason this syntax works and the other doesnt.
                            .i_mode = 0x41ED, // 0x41ED,
                            .i_uid = running->uid,
                            .i_gid = running->gid,
                            .i_size = BLKSIZE,
                            .i_links_count = 2,
                            .i_atime = time(0L),
                            .i_ctime = time(0L),
                            .i_mtime = time(0L),
                            .i_blocks = 2,
                            .i_block = {0}};
      */
        ip->i_block[0] = blk;
        ip->i_mode = 0x41ED;      // 040755: DIR type and permissions
        ip->i_uid = running->uid; // owner uid
        ip->i_gid = running->gid; // group Id
        ip->i_size = 1024;        // size in bytes
        ip->i_links_count = 2;    // links count=2 because of . and ..
        ip->i_atime = ip->i_ctime = ip->i_mtime = time(0L);
        ip->i_blocks = 2;     // LINUX: Blocks count in 512-byte chunks
        ip->i_block[0] = blk; // new DIR has one data block
        mip->dirty = 1;       // mark minode dirty */

        for (int x = 1; x < 15; ++x)
        {
            ip->i_block[x] = 0;
        }

        iput(mip);

        char buf[1024];
        bzero(buf, 1024); // optional: clear buf[ ] to 0
        DIR *dp = (DIR *)buf;
        // make . entry
        dp->inode = ino;
        dp->rec_len = 12;
        dp->name_len = 1;
        dp->name[0] = '.';
        // make .. entry: pino=parent DIR ino, blk=allocated block
        dp = (DIR *)(((char *)dp) + 12);
        dp->inode = pmip->ino;
        dp->rec_len = BLKSIZE - 12;
        dp->name_len = 2;
        dp->name[0] = dp->name[1] = '.';
        put_block(dev, blk, buf);

        // printf("BLOCK: %d\n\n", blk)

        enter_child(pmip, ino, dirname);

        // print_block(mip, blk);

        pmip->INODE.i_links_count++;
        pmip->dirty = 1;
        iput(pmip);
    }

    else
    {
        printf("This action is not supported");
    }

    // printf("HERE!");

    return;
}