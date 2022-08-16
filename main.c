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

node* insert(node* root,node* temp)
{
    //provedem BST insert
    if (root == NULL){
        return temp;
    }
        
    if (temp->data < root->data)
    {
        root->leftChild = insert(root->leftChild, temp);
        root->leftChild->parent = root;
    }
    else if (temp->data > root->data)
    {
        root->rightChild = insert(root->rightChild, temp);
        root->rightChild->parent = root;
    }
    return root;
}

void switchParentChild(node* parent, node* newChild, node* oldChild){
    // změní syna rodiče
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

void rightRotate(node* temp){
    if ((temp == NULL) || (temp->leftChild == NULL ) ) {
        printf("no node or left child\n");
        exit(1);
    }
    //
    //    temp ->   N                       L
    //           /     \                  /   \
    //          L       R       ->      LL      N
    //         / \     / \             /  \    /  \
    //        LL  LR                          LR   R
    //
    node* parent = temp->parent;
    node* left = temp->leftChild;
    
    temp->leftChild = left->rightChild;
    //
    //      N               N
    //     /        ->     /
    //    L               LR
    //
    if (left->rightChild != NULL) {
        left->rightChild->parent = temp;
    }
    
    left->rightChild = temp;
    //
    //      L               L
    //       \      ->       \
    //        LR              N
    //
    temp->parent = left;
    
    // vymění starého syna N za nového syna L
    switchParentChild(parent, left, temp);
}

void leftRotate(node* temp){
    if ((temp == NULL) || (temp->rightChild == NULL )) {
        printf("no node or right child\n");
        exit(1);
    }
    node* parent = temp->parent;
    node* right = temp->rightChild;
    
    temp->rightChild = right->leftChild;
    if (right->leftChild != NULL) {
        right->leftChild->parent = temp;
    }
    
    right->leftChild = temp;
    temp->parent = right;
    
    switchParentChild(parent, right, temp);
}


node* getUncle(node* parent){
    // funkce pro nalezení strýce = bratr našeho rodiče
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
    // nalezne bratra
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
    // nalezen root, konec rekurze, root musí být černý
    if (insertedNode->parent == NULL) {
        insertedNode->isBlack=1;
        return;
    }
    // rodič je černý, nemusíme nic měnit
    if (insertedNode->parent->isBlack==1) {
        return;
    }

    node* grandafther = insertedNode->parent->parent;
    // neexistuje prarodič -> rodič je root -> musí být černý
    if (grandafther == NULL) {
        insertedNode->parent->isBlack=1;
        return;
    }

    node* uncle = getUncle(insertedNode->parent);
    node* parent = insertedNode->parent;
    // strýc je červený -> přebarvíme otce, strýce a prarodiče
    if (uncle != NULL && uncle->isBlack == 0) {
        uncle->isBlack = 1;
        parent->isBlack = 1;
        grandafther->isBlack = 0;
        fixAfterInsert(grandafther);  // rekurze pro případ, že je prarodič je root ( nyní je červený )
    }
    // rodič je vnitřní potomek prarodiče ( je to levý syn )
    else if (parent == grandafther->leftChild) {
        //insertedNode je od prarodiče ->left->right
        if (insertedNode == parent->rightChild) {
            leftRotate(parent);
            // rodič je nový root rotovaného podstromu
            parent = insertedNode;
        }
        //insertedNode je od prarodiče ->left->left
        rightRotate(grandafther);
        // přebarvíme rodiče a prarodiče
        parent->isBlack=1;
        grandafther->isBlack=0;
    }
    // rodič je vnější potomek prarodiče ( je to pravý syn )
    else {
        // stejné jako blok výš, pouze jsou rotace prohozené
        if (insertedNode == parent->leftChild) {
            rightRotate(parent);
            parent = insertedNode;
        }
        leftRotate(grandafther);
        parent->isBlack=1;
        grandafther->isBlack=0;
    }
}

node* deleteZeroOneChild(node* deleteNode){
    // existuje pouze pravý syn, nahradíme node za parvého syna
    if (deleteNode->rightChild != NULL) {
        switchParentChild(deleteNode->parent, deleteNode->rightChild, deleteNode);
        return deleteNode->rightChild;
    }
    // existuje pouze levý syn, nahradíme node za levého syna
    else if (deleteNode->leftChild != NULL){
        switchParentChild(deleteNode->parent, deleteNode->leftChild, deleteNode);
        return deleteNode->leftChild;
    }
    // není syn
    else {
        // node na smazání je černý
        if (deleteNode->isBlack == 1) {
            // nahradíme smazaný node za nilNode pro potřeby pozdější opravy
            node* nilNode = (node*)malloc(sizeof(struct node));
            nilNode->isBlack=1;
            nilNode->isNilNode=1;
            
            switchParentChild(deleteNode->parent, nilNode, deleteNode);
            return nilNode;
            
        }
        else {
            //node je červený -> smažeme ho
            node* nullNode = NULL;
            switchParentChild(deleteNode->parent, nullNode, deleteNode);
            return nullNode;
        }
    }
}

node* findMinInSubtree(node* root){
    // nalezne minimum v podstromě jehož root je (node* root)
    while (root->leftChild != NULL) {
        root = root->leftChild;
    }
    return root;
}

void fixRedSiblingAfterDeletion(node* broken, node* brokenSibling){
    // sourozenec je červený -> přebarvíme ho na černo a rodiče na červeno
    // tím porušíme vlastnost, že všechny cesty z rootu k listům mají stejný počet černých nodů
    // proto rotujem
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
    // sourozenec je levý syn ( s alespon jednim červeným potomkem ) rodiče a synovec je vnější node a je černý
    // přebarvíme sourozence a jeho děti a provedem rotaci sourozence
    if ((broken->parent->leftChild == broken) && (brokenSibling->rightChild->isBlack == 1)) {
        brokenSibling->leftChild->isBlack = 1;
        brokenSibling->isBlack = 0;
        rightRotate(brokenSibling);
        brokenSibling = broken->parent->rightChild;
    }
    // sourozenec je pravý syn ( s alespon jednim červeným potomkem ) a synovec je vnitřní node a je černý
    else if ((broken->parent->leftChild != broken) && (brokenSibling->leftChild->isBlack == 1)){
        brokenSibling->rightChild->isBlack = 1;
        brokenSibling->isBlack = 0;
        leftRotate(brokenSibling);
        brokenSibling = broken->parent->rightChild;
    }
    
    // černý sourozenec a vnejší synovec je červený
    // přebarvíme sourozence + rodiče + potomka sourozence, rotujeme okolo rodiče
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
    // base case pro rekurzi / node* z inputu je root
    if (maybeBrokenNode == root) {
        maybeBrokenNode->isBlack = 1;
        return;
    }
    
    // naleznem sourozence
    node* sibling = getSibling(maybeBrokenNode);
    
    //sourozenec je červený
    if (sibling->isBlack == 0) {
        fixRedSiblingAfterDeletion(maybeBrokenNode, sibling);
        sibling = getSibling(maybeBrokenNode); //nový bratránek po rotaci
    }
    
    // černý sourozenec má dva černé potomky
    if ((sibling->leftChild->isBlack == 1) && (sibling->rightChild->isBlack == 1)) {
        // přebarvíme na červenou
        sibling->isBlack = 0;
        
        // node má černého sourozence s dvěma červenými potomky a má červeného rodiče
        if (maybeBrokenNode->parent->isBlack == 0) {
            // přebarvíme na černou
            maybeBrokenNode->parent->isBlack = 1;
        }
        
        // node má sourozence s dvěma červenými potomky a má černého rodiče
        else {
            fixAfterDeletion(maybeBrokenNode->parent);
        }
    }
    // node má sourozence s dvěma červenými potomky a má černého rodiče
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
        // mažeme node pouze s jedním potomkem
        deletedNodeColour = deleteNode->isBlack;
        maybeBrokenNode = deleteZeroOneChild(deleteNode);
    }
    // mažem node s dvěma potomky
    else {
        //naleznem node s nejmenší hodnotou, ten smažem, uložíme si jeho hodnotu a barvu
        node* minInSubtree = findMinInSubtree(deleteNode->rightChild);
        deleteNode->data = minInSubtree->data;
        maybeBrokenNode = deleteZeroOneChild(minInSubtree);
        deletedNodeColour = minInSubtree->isBlack;
    }
    
    // smazali jsme černý node, porušili jsme výšku, musí opravit
    if (deletedNodeColour == 1) {
        fixAfterDeletion(maybeBrokenNode);
        
        if (maybeBrokenNode->isNilNode == 1) {
            switchParentChild(maybeBrokenNode->parent, NULL, maybeBrokenNode);
        }
    }
}

void print2DTree(node* root, int space)
{
    if (root == NULL)
        return;
 
    space += COUNT;
 
    print2DTree(root->rightChild, space);
 
    printf("\n");
    for (int i = COUNT; i < space; i++)
        printf(" ");
    printf("%d - %d\n", root->data, root->isBlack);

    print2DTree(root->leftChild, space);
    printf("\n");
    printf("\n");
    
}

int main() {
    // po spuštění postupně bude postupně ukazovat jak zrovna vypadá RB strom po insertu
    // strom se tiskne jakoby byl položen na levém boku
    // kazdy node bude mit dve hodnoty - node->data - node->isBlack
    // nakonec smazem root
    int a[12] = { 3, 1, 18, 6, 2, 0, 4, 9, 5, 11, 14, 21};
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
        print2DTree(root, 0);
        printf("\n");
    }

    printf("%d - root data",root->data);
    print2DTree(root,0);
    
    
    deleteNode(root);
    printf("\n");
    printf("\n");
    print2DTree(root,0);

    
    return 0;
}
