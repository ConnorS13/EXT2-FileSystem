#include "functions.h"

int tst_bit(char *buf, int bit)
{
    return buf[bit / 8] & (1 << (bit % 8));
}

int set_bit(char *buf, int bit)
{
    buf[bit / 8] |= (1 << (bit % 8));
}

int decFreeInodes(int de)
{
    // dec free inodes count in SUPER and GD
    char buf[1024];
    get_block(de, 1, buf);
    sp = (SUPER *)buf;

    sp->s_free_inodes_count--; // THE ISSUE IS RIGHT HERE!

    put_block(de, 1, buf);

    get_block(de, 2, buf);
    gp = (GD *)buf;
    gp->bg_free_inodes_count--;
    put_block(de, 2, buf);
}

int decFreeBlocks(int de)
{
    // dec free inodes count in SUPER and GD
    char buf[1024];
    get_block(de, 1, buf);
    sp = (SUPER *)buf;
    sp->s_free_blocks_count--;
    put_block(de, 1, buf);

    get_block(de, 2, buf);
    gp = (GD *)buf;
    gp->bg_free_blocks_count--;
    put_block(de, 2, buf);
}

int ialloc(int de) // allocate an inode number from inode_bitmap
{
    int i;
    char buf[BLKSIZE];

    // read inode_bitmap block
    get_block(de, imap, buf);

    for (i = 0; i < ninodes; i++)
    { // use ninodes from SUPER block
        if (tst_bit(buf, i) == 0)
        {
            set_bit(buf, i);
            put_block(de, imap, buf);

            decFreeInodes(de);

            printf("allocated ino = %d\n", i + 1); // bits count from 0; ino from 1
            return i + 1;
        }
    }
    return 0;
}

int balloc(int de)
{
    int i;
    char buf[BLKSIZE];

    // read inode_bitmap block
    get_block(de, bmap, buf);

    for (i = 0; i < nblocks; i++)
    { // use ninodes from SUPER block
        if (tst_bit(buf, i) == 0)
        {
            set_bit(buf, i);
            put_block(de, bmap, buf);

            decFreeBlocks(de);

            printf("allocated block = %d\n", i + 1); // bits count from 0; ino from 1
            return i + 1;
        }
    }
    return 0;
}

int clr_bit(char *buf, int bit) // clear bit in char buf[BLKSIZE]
{
    buf[bit / 8] &= ~(1 << (bit % 8));
}

int incFreeInodes(int dev)
{
    char buf[BLKSIZE];
    // inc free inodes count in SUPER and GD
    get_block(dev, 1, buf);
    sp = (SUPER *)buf;
    sp->s_free_inodes_count++;
    put_block(dev, 1, buf);
    get_block(dev, 2, buf);
    gp = (GD *)buf;
    gp->bg_free_inodes_count++;
    put_block(dev, 2, buf);
}

int incFreeBlocks(int dev)
{
    char buf[BLKSIZE];
    // inc free inodes count in SUPER and GD
    get_block(dev, 1, buf);
    sp = (SUPER *)buf;
    sp->s_free_blocks_count++;
    put_block(dev, 1, buf);
    get_block(dev, 2, buf);
    gp = (GD *)buf;
    gp->bg_free_blocks_count++;
    put_block(dev, 2, buf);
}

int idalloc(int dev, int ino)
{
    int i;
    char buf[BLKSIZE];

    if (ino > ninodes)
    {
        printf("inumber %d out of range\n", ino);
        return -1;
    }

    // get inode bitmap block
    get_block(dev, imap, buf);
    clr_bit(buf, ino - 1);

    // write buf back
    put_block(dev, imap, buf);

    // update free inode count in SUPER and GD
    incFreeInodes(dev);
}

int bdalloc(int dev, int ino)
{
    int i;
    char buf[BLKSIZE];

    if (ino > nblocks)
    {
        printf("inumber %d out of range\n", ino);
        return -1;
    }

    // get inode bitmap block
    get_block(dev, iblk, buf);
    clr_bit(buf, ino - 1);

    // write buf back
    put_block(dev, iblk, buf);

    // update free inode count in SUPER and GD
    incFreeBlocks(dev);
}