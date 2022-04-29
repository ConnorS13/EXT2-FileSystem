#include "functions.h"

void __symlink(char *nameValue)
{
    char child[2024], parent[2014];
    int x = 1;

    // SPLIT PATHNAME
    strcat(pathname, " ");
    strcat(pathname, nameValue);

    split_name(pathname);
    printf("name0: %s name1: %s\n", name[0], name[1]);

    char size[100];

    strcpy(size, name[0]);

    char ginoPass[1024];
    strcpy(ginoPass, name[1]);

    int pino = 0;
    int oino = getino(name[0]);
    MINODE *omip = iget(dev, oino);

    tokenize(ginoPass);

    if (name[1])
    {
        strcpy(parent, name[0]);

        for (; x < (n - 1); ++x)
        {
            strcat(parent, "/");
            strcat(parent, name[x]);
        }

        strcpy(child, name[x]);
    }

    else
    {
        pino = 2;
        strcpy(child, name[0]);
        strcpy(parent, "/");
    }

    if (!omip)
    {
        printf("ERROR! Not a file.\n");
        return;
    }

    if (getino(ginoPass) != 0)
    {
        printf("This file exists already\n");
        return;
    }

    MINODE *pmip;

    if (pino == 0)
        pino = getino(parent);

    pmip = iget(dev, pino);

    enter_child(pmip, oino, child);

    // CREAT ABOVE, FINISH STEP 3/4 (STEP 3!) BELOW!!!!

    char buffer[1024];

    // findmyname(pmip, pino, buffer);

    // omip->INODE.i_mode = 0xA1FF;

    omip->INODE.i_mode = 0xA1FF;

    omip->INODE.i_links_count++;   

    // int newblk = balloc(dev);

    // omip->INODE.i_block[0] = newblk;

    strcpy(omip->INODE.i_block, size);//  CHANGED THIS 

    // put_block(dev, omip->INODE.i_block[0], size);

    omip->INODE.i_size = strlen(size);

    omip->dirty = 1; // inc INODE’s links_count by 1
    iput(omip);

    pmip->dirty = 1; // for write back by iput(omip)
    iput(pmip);
}

int __readlink(char *buffer)
{  
    int pino = 0;
    pino = getino(pathname);

    MINODE *pmip = iget(dev, pino);

    strcpy(buffer, pmip->INODE.i_block[0]);

    printf("FILE SIZE: %d\n", pmip->INODE.i_size);

    return pmip->INODE.i_size;
}