//
//  main.c
//  redblacktree.c
//
#define COUNT 10

#include <stdlib.h>
#include <stdio.h>

typedef struct node {
    struct node* parent;
    struct node* leftChild;
    struct node* rightChild;
    int isBlack;
    int data;
} node;

node* root = NULL;

node* insert(node* trav,node* temp)
{
    if (trav == NULL){
        return temp;
    }
        
    if (temp->data < trav->data)
    {
        trav->leftChild = insert(trav->leftChild, temp);
        trav->leftChild->parent = trav;
    }
    else if (temp->data > trav->data)
    {
        trav->rightChild = insert(trav->rightChild, temp);
        trav->rightChild->parent = trav;
    }
    return trav;
}

void leftRotate(node* temp){
    if ((temp == NULL) || (temp->rightChild == NULL ) ) {
        printf("no node or right child\n");
        exit(1);
    }
    node* rightOfTemp = temp->rightChild;
    temp->rightChild = rightOfTemp->leftChild;
    if (rightOfTemp->rightChild != NULL) {
        rightOfTemp->rightChild->parent = temp;
    }
    rightOfTemp->parent=temp->parent;
    if (temp->parent == NULL) {
        root = rightOfTemp;
    }
    else if (temp->parent->rightChild == temp){
        temp->parent->rightChild=rightOfTemp;
    }
    else {
        temp->parent->leftChild=rightOfTemp;
    }
    rightOfTemp->leftChild=temp;
    temp->parent=rightOfTemp;
}

void rightRotate(node* temp){
    if ((temp == NULL) || (temp->leftChild == NULL ) ) {
        printf("no node or left child\n");
        exit(1);
    }
    node* leftOfTemp = temp->leftChild;
    temp->leftChild = leftOfTemp->rightChild;
    if (leftOfTemp->leftChild != NULL) {
        leftOfTemp->leftChild->parent = temp;
    }
    leftOfTemp->parent=temp->parent;
    if (temp->parent == NULL) {
        root = leftOfTemp;
    }
    else if (temp->parent->leftChild == temp){
        temp->parent->leftChild=leftOfTemp;
    }
    else {
        temp->parent->rightChild=leftOfTemp;
    }
    leftOfTemp->rightChild=temp;
    temp->parent=leftOfTemp;
}

/*
void inoredr(node* temp){
    if (temp == NULL) {
        return;
    }
    inoredr(temp->leftChild);
    printf("%d\n", temp->data);
    inoredr(temp->rightChild);
}
*/

node* getUncle(node* parent){
    node* grandfather = parent->parent;
    if (grandfather->leftChild==parent) {
        return grandfather->rightChild;
    }
    else if (grandfather->rightChild==parent) {
        return grandfather->leftChild;
    }
    else {
        printf("uncle doesnt exist");
        exit(1);
    }
}

void fixAfterInsert(node* insertedNode)
{
    if (insertedNode->parent == NULL) {
        insertedNode->isBlack=1;
        return;
    }
    if (insertedNode->parent->isBlack==1) {
        return;
    }
    //
    // tohle nevim jestli je treba
    //
    node* grandafther = insertedNode->parent->parent;
    if (grandafther == NULL) {
        insertedNode->parent->isBlack=1;
    }
    //
    //
    node* uncle = getUncle(insertedNode->parent);
    node* parent = insertedNode->parent;
    if (uncle != NULL && uncle->isBlack == 0) {
        uncle->isBlack = 1;
        parent->isBlack = 1;
        uncle->parent->isBlack = 0;
        fixAfterInsert(uncle->parent);
    }
    else if (parent == grandafther->leftChild) {
        if (insertedNode == parent->rightChild) {
            leftRotate(parent);
            parent = insertedNode;
        }
        rightRotate(grandafther);
        parent->isBlack=1;
        grandafther->isBlack=0;
    }
    else {
        if (insertedNode == parent->leftChild) {
            rightRotate(parent);
            parent = insertedNode;
        }
        leftRotate(grandafther);
        parent->isBlack=1;
        grandafther->isBlack=0;
    }
}

void deleteNode(node* deleteNode){
    
}

void print2DUtil(node* root, int space)
{
    if (root == NULL)
        return;
 
    space += COUNT;
 
    print2DUtil(root->rightChild, space);
 
    printf("\n");
    for (int i = COUNT; i < space; i++)
        printf(" ");
    printf("%d - %d\n", root->data, root->isBlack);

    print2DUtil(root->leftChild, space);
}

int main() {
    
    int a[6] = { 3, 1, 18, 6, 2, 0};
    int n = sizeof(a)/sizeof(int);
     
    for (int i = 0; i < n; i++) {
     
        node* temp
            = (node*)malloc(sizeof(struct node));
        temp->rightChild = NULL;
        temp->leftChild = NULL;
        temp->parent = NULL;
        temp->data = a[i];
        temp->isBlack = 0;

        root = insert(root, temp);
        
        fixAfterInsert(temp);
        
        printf("%d - root data",root->data);
        print2DUtil(root, 0);
        printf("\n");
    }
    printf("%d - root data",root->data);
    print2DUtil(root,0);
    
    return 0;
}
