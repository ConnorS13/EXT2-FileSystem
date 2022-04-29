#include "functions.h"

char *basename(char *base)
{
    char *s, *myName = name[1], *x;

    strcpy(gpath, pathname);
    s = strtok(gpath, "/");

    while (s)
    {
        x = s;
        s = strtok(0, "/");
    }

    return x;
}

char *dirname(char *base)
{
    char *s, *myName = name[1], *x;

    strcpy(gpath, pathname);
    s = strtok(gpath, "/");

    strcpy(x, s);

    while (s)
    {
        strcat(x, "/");
        strcat(x, s);
        s = strtok(0, "/");
    }

    return x;
}

void split_name(char *pathname)
{
    int i;
    char *s;
    printf("tokenize %s\n", pathname);

    strcpy(gpath, pathname); // tokens are in global gpath[ ]
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

void ____link(char *nameValue)
{
    char child[2024], parent[2014];
    int x = 1;

    // SPLIT PATHNAME
    strcat(pathname, " ");
    strcat(pathname, nameValue);

    split_name(pathname);
    // printf("name0: %s name1: %s\n", name[0], name[1]);

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
    }

    printf("path: %s, dirname: %s", parent, child); // ALL CODE ABOVE THIS LINE IS FOR TOKENIZING

    if (S_ISDIR(omip->INODE.i_mode) || !omip)
    {
        printf("ERROR! Not a file.\n"); // error checking to see if it is a file
        return;
    }

    if (getino(ginoPass) != 0) // ensure files does not exist NEWFILE
    {
        printf("This file exists already\n");
        return;
    }

    MINODE *pmip;

    if (pino == 0)
    {
        pino = getino(parent); // previously pino could have been root already
    }

    pmip = iget(dev, pino); // iget the pmip

    enter_child(pmip, oino, child); // call enter child passing the pino!!!

    omip->INODE.i_mode = 0100755; // mode assigning
    omip->INODE.i_links_count++; // inc INODE’s links_count by 1
    omip->dirty = 1;
    iput(omip);
    iput(pmip);

    return;
}

void __unlink()
{
    int x = 1, pino = 0;
    char path[100], dirname[100], child[100], parent[100];
    int ino = getino(pathname);
    MINODE *mip = iget(dev, ino);

    if ((S_ISDIR(mip->INODE.i_mode)))
    {
        printf("This is a directory!");
        return;
    } // make sure it is not a directory!


    tokenize(pathname);

    if (name[1])
    {
        strcpy(parent, name[0]);

        for (; x < (n - 1); ++x)
        {
            strcat(parent, "/");
            strcat(parent, name[x]); // this section is sorting the strings out!
        }

        strcpy(child, name[x]);
    }

    else
    {
        pino = 2;
        strcpy(child, name[0]);
        strcpy(parent, "/");
    }

    printf("path: %s, dirname: %s", parent, dirname);

    if (pino == 0)
        pino = getino(parent); // get ino of the parent if it is not the root

    MINODE *pmip = iget(dev, pino); // iget the parent minode

    rm_child(pmip, child); // call remove child 
    pmip->dirty = 1;
    iput(pmip);

    mip->INODE.i_links_count--;

    if (mip->INODE.i_links_count > 0)
        mip->dirty = 1;

    else
    {
        bdalloc(mip->dev, mip->INODE.i_block[0]);
        idalloc(mip->dev, mip->ino); // deallocate the ino/blk
    }

    iput(mip);

    return;
}