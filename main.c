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
    int isNilNode;
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
    if ((temp == NULL) || (temp->rightChild == NULL )) {
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

node* getSibling(node* root){
    node* parent = root->parent;
    if (parent->leftChild == root) {
        return parent->rightChild;
    }
    else{
        return parent->leftChild;
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

void switchParentChild(node* parent, node* newChild, node* oldChild){
    if (parent == NULL) {
        root = newChild;
    }
    else if (parent->leftChild == oldChild){
        parent->leftChild = newChild;
    }
    else if (parent->rightChild == oldChild){
        parent->rightChild = newChild;
    }
    
    if (newChild != NULL) {
        newChild->parent = parent;
    }
}

node* deleteZeroOneChild(node* deleteNode){
    if (deleteNode->rightChild != NULL) {
        switchParentChild(deleteNode->parent, deleteNode->rightChild, deleteNode);
        return deleteNode->rightChild;
    }
    else if (deleteNode->leftChild != NULL){
        switchParentChild(deleteNode->parent, deleteNode->leftChild, deleteNode);
        return deleteNode->leftChild;
    }
    else {
        if (deleteNode->isBlack == 1) {
            node* nilNode = (node*)malloc(sizeof(struct node));
            nilNode->isBlack=1;
            nilNode->isNilNode=1;
            
            switchParentChild(deleteNode->parent, nilNode, deleteNode);
            return nilNode;
            
        }
        else {
            node* nullNode = NULL;
            switchParentChild(deleteNode->parent, nullNode, deleteNode);
            return nullNode;
        }
    }
}

node* findMinInSubtree(node* root){
    while (root->leftChild != NULL) {
        root = root->leftChild;
    }
    return root;
}

void fixRedSiblingAfterDeletion(node* broken, node* brokenSibling){
    brokenSibling->isBlack = 1;
    broken->parent->isBlack = 0;
    
    if (broken->parent->rightChild == broken) {
        leftRotate(broken->parent);
    }
    else {
        rightRotate(broken->parent);
    }
}

void blackSiblingWithOneOrMoreRedChild(node* broken, node* brokenSibling){
    if ((broken->parent->leftChild == broken) && (brokenSibling->rightChild->isBlack == 1)) {
        brokenSibling->leftChild->isBlack = 1;
        brokenSibling->isBlack = 0;
        rightRotate(brokenSibling);
        brokenSibling = broken->parent->rightChild;
    }
    else if ((broken->parent->leftChild != broken) && (brokenSibling->leftChild->isBlack == 1)){
        brokenSibling->rightChild->isBlack = 1;
        brokenSibling->isBlack = 0;
        leftRotate(brokenSibling);
        brokenSibling = broken->parent->rightChild;
    }
    
    brokenSibling->isBlack = broken->parent->isBlack;
    broken->parent->isBlack = 1;
    
    if (broken->parent->leftChild == broken) {
        brokenSibling->rightChild->isBlack = 1;
        leftRotate(broken->parent);
    }
    else {
        brokenSibling->leftChild->isBlack = 1;
        rightRotate(broken->parent);
    }
}

void fixAfterDeletion(node* maybeBrokenNode){
    if (maybeBrokenNode == root) {
        maybeBrokenNode->isBlack = 1;
        return;
    }
    
    node* sibling = getSibling(maybeBrokenNode);
    
    if (sibling->isBlack == 0) {
        fixRedSiblingAfterDeletion(maybeBrokenNode, sibling);
        sibling = getSibling(maybeBrokenNode); //nový bratránek po rotaci
    }
    
    if ((sibling->leftChild->isBlack == 1) && (sibling->rightChild->isBlack == 1)) {
        sibling->isBlack = 0;
        
        if (maybeBrokenNode->parent->isBlack == 0) {
            maybeBrokenNode->parent->isBlack = 1;
        }
        else {
            fixAfterDeletion(maybeBrokenNode->parent);
        }
    }
    else {
        blackSiblingWithOneOrMoreRedChild(maybeBrokenNode, sibling);
    }
}

void deleteNode(node* deleteNode){
    if (deleteNode == NULL)
        return;
    
    int deletedNodeColour;
    node* maybeBrokenNode;
    
    if (deleteNode->leftChild == NULL || deleteNode->rightChild == NULL) {
        deletedNodeColour = deleteNode->isBlack;
        maybeBrokenNode = deleteZeroOneChild(deleteNode);
    }
    //printf("%d - deleted",deletedNodeColour);
    else {
        node* minInSubtree = findMinInSubtree(deleteNode);
        deleteNode->data = minInSubtree->data;
        maybeBrokenNode = deleteZeroOneChild(minInSubtree);
        deletedNodeColour = minInSubtree->isBlack;
    }
    
    if (deletedNodeColour == 1) {
        fixAfterDeletion(maybeBrokenNode);
        
        if (maybeBrokenNode->isNilNode == 1) {
            switchParentChild(maybeBrokenNode->parent, NULL, maybeBrokenNode);
        }
    }
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
    printf("\n");
    printf("\n");
    
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
        temp->isNilNode = 0;

        root = insert(root, temp);
        
        fixAfterInsert(temp);
        
        printf("%d - root data",root->data);
        print2DUtil(root, 0);
        printf("\n");
    }
    printf("%d - root data",root->data);
    print2DUtil(root,0);
    
    deleteNode(root);
    
    print2DUtil(root,0);
    
    return 0;
}
