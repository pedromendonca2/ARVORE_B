#include "arvoreB.h"
#include <stdio.h>
#include <stdlib.h>

diskSearch(int key, BT *root, int order)
{
    if(root == NULL)
        return NULL;

    int i = 0;
    while(i < getNumKeys(root) && key > getKeysValues(root, i))
        i++;

    if(i < getNumKeys(root) && key == getKeysValues(root, i))
        return root;

    if(getIsLeaf(root))
        return NULL;

    return diskSearch(key, disk_read(getKidsValues(root, i), order, fp), order);
}

{
    FILE *fp = fopen("arvoreB.bin", "rb");
    if(fp == NULL){
        perror("Error opening file");
        exit(1);
    }

    BT *root = disk_read(0, order, fp);
    fclose(fp);

    return root;
}

BT *disk_read(int diskID, int order, FILE *fp)
{
    int offset = calculate_offset(diskID, order);
    fseek(fp, offset, SEEK_SET);      // set the file pointer there

    int numKeys, isLeaf, pos_in_disk;

    fread(&numKeys, sizeof(int), 1, fp);    // read the information from the file
    fread(&isLeaf, sizeof(int), 1, fp);
    fread(&pos_in_disk, sizeof(int), 1, fp);

    BT *read_node = createBtBT(numKeys, isLeaf, order);

    tElement *keys = (tElement*) calloc (sizeof(tElement*) * order-1);
    fread(keys, sizeof(tElement), order-1, fp);

    int *kids = (int *) malloc (sizeof(int) * order);
    fread(kids, sizeof(int), order, fp);
    setPosInDisk(read_node, pos_in_disk);

    for(int i=0; i < order-1; i++)
        setKeysValues(read_node, keys[i], i);
    for(int i=0; i < order; i++)
        setKidsValues(read_node, kids[i], i);

    return read_node;
}

void disk_write(BT *node, int order, FILE *fp)
{
    int offset = calculate_offset(getPosInDisk(node), order);
    fseek(fp, offset, SEEK_SET);                                 // set the file pointer there

    int numKeys = getNumKeys(node);
    int isLeaf = getIsLeaf(node);
    int posInDisk = getPosInDisk(node);
    

    fwrite(&numKeys, sizeof(getNumKeys(node)), 1, fp);          // write the information to the file
    fwrite(&isLeaf, sizeof(getIsLeaf(node)), 1, fp);
    fwrite(&posInDisk, sizeof(getPosInDisk(node)), 1, fp);
    fwrite(getKeys(node), sizeof(getKeys(node)), order-1, fp);
    fwrite(getKids(node), sizeof(getKids(node)), order, fp);

}