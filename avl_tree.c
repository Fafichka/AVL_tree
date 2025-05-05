#include <stdio.h>
#include <stdlib.h>

typedef struct node {
    int key;
    int height; //высота поддерева
    struct node *left;
    struct node *right;
} avl_node;

int max(int a, int b) {
    return (a > b) ? a : b;
}

int get_height(avl_node *node) { //высота узла
    if (node == NULL) return 0;
    return node->height;
}

void change_height(avl_node *tree) { //изменение высоты
    if (tree == NULL) return;
    tree->height = 1 + max(get_height(tree->left), get_height(tree->right));
}

int difference(avl_node *tree) { //разница между правым и левым поддеревьями
    if (tree == NULL) return 0;
    return get_height(tree->right) - get_height(tree->left);
}

// Функции балансировки
void balance_right (avl_node ** tree){ //поворот вправо для исправления LL случая
    avl_node * pointer = (*tree)->left;
    (*tree)->left = pointer->right;
    pointer->right = (*tree);
    (*tree) = pointer; 
    change_height((*tree)->right); //корректировка данных о высоте нового и старого корня
    change_height(*tree);
}

void balance_left (avl_node ** tree){ //поворот влево для исправления RR случая
    avl_node * pointer = (*tree)->right;
    (*tree)->right = pointer->left;
    pointer->left = (*tree);
    (*tree) = pointer; 
    change_height((*tree)->left); //корректировка данных о высоте нового и старого корня
    change_height(*tree);
}

void balance_lr (avl_node ** tree){ //комбинация поворотов для исправления LR случая
    balance_left(&(*tree)->left); 
    balance_right(tree);
}

void balance_rl (avl_node ** tree){ //комбинация поворотов для исправления RL случая
    balance_right(&(*tree)->right);
    balance_left(tree);
}

void balance(avl_node **tree){ //общая рекурсивная балансировка
    if (!(*tree)) return;
    balance(&(*tree)->right);
    balance(&(*tree)->left);
    change_height(*tree);
    if (difference(*tree) == -2){ //если слева длиннее
        if (difference((*tree)->left)<=0){ 
            balance_right(tree);
        }
        else{
            balance_lr(tree);
        }
    }
    else if (difference(*tree) == 2){ //если справа длиннее
        if (difference((*tree)->right)>=0){
            balance_left(tree);
        }
        else{
            balance_rl(tree);
        }
    }
}

avl_node* avl_add(avl_node* tree, int key) { //добавление элемента
    if (tree == NULL) { //если это первый элемент
        avl_node* new_node = (avl_node*)malloc(sizeof(avl_node));
        new_node->key = key;
        new_node->height = 1;
        new_node->left = NULL;
        new_node->right = NULL;
        return new_node;
    }
    
    if (key == tree->key) { //если элемент повторяется
        printf("Элемент %d уже существует в дереве\n", key);
        return tree;
    }
    
    if (key < tree->key) {
        tree->left = avl_add(tree->left, key);
    } else {
        tree->right = avl_add(tree->right, key);
    }
    
    change_height(tree);
    balance(&tree);
    
    return tree;
}

avl_node* find_min(avl_node* tree) {
    while (tree->left != NULL) {
        tree = tree->left;
    }
    return tree;
}

avl_node* avl_delete(avl_node* tree, int key) {
    if (tree == NULL) {
        printf("Элемент %d не найден в дереве\n", key);
        return NULL;
    }
    
    if (key < tree->key) {
        tree->left = avl_delete(tree->left, key);
    } else if (key > tree->key) {
        tree->right = avl_delete(tree->right, key);
    } else {
        if ((tree->left == NULL) || (tree->right == NULL)) {
            avl_node* temp = tree->left ? tree->left : tree->right;
            
            if (temp == NULL) {
                temp = tree;
                tree = NULL;
            } else {
                *tree = *temp;
            }
            
            free(temp);
        } else {
            avl_node* temp = find_min(tree->right);
            tree->key = temp->key;
            tree->right = avl_delete(tree->right, temp->key);
        }
    }
    
    if (tree == NULL) {
        return tree;
    }
    
    change_height(tree);
    balance(&tree);
    
    return tree;
}

avl_node* poisk(avl_node* tree, int key) { //поиск элемента
    if (tree == NULL) return NULL;
    
    if (key < tree->key) {
        return poisk(tree->left, key);
    } else if (key > tree->key) {
        return poisk(tree->right, key);
    } else {
        return tree;
    }
}

void print_tree(avl_node *root, int space) { //вывод дерева
    if (root == NULL) return;
    
    space += 5; //отступ для печати
    
    print_tree(root->right, space);
    
    printf("\n");
    for (int i = 5; i < space; i++) {
        printf(" ");
    }
    printf("%d(%d)\n", root->key, root->height);
    
    print_tree(root->left, space);
}


int main() {
    avl_node *root = NULL;

    //добавление элементов
    printf("Добавляем элементы: 10, 20, 30, 40, 50, 25\n");
    root = avl_add(root, 10);
    root = avl_add(root, 20);
    root = avl_add(root, 30);
    root = avl_add(root, 40);
    root = avl_add(root, 50);
    root = avl_add(root, 25);
    
    printf("\nДерево после добавления элементов\n");
    print_tree(root, 0);
    printf("\n");

    //попытка добавления существующего элемента
    printf("\nПопытаемся добавить уже существующий элемент\n");
    root = avl_add(root, 30);

    //поиск элементов
    printf("\nИщем элемент\n");
    int keys[] = {20, 35, 25};
    for (int i = 0; i < 3; i++) {
        avl_node *found = poisk(root, keys[i]);
        if (found) {
            printf("Элемент %d найден (height: %d)\n", found->key, found->height);
        } else {
            printf("Элемент %d не найден\n", keys[i]);
        }
    }

    //удаление элементов
    printf("\nУдалим элементы: 30, 10\n");
    root = avl_delete(root, 30);
    root = avl_delete(root, 10);
    
    printf("\n Дерево после удаления элементов:\n");
    print_tree(root, 0);
    printf("\n");

    //попытка удаления несуществующего элемента
    printf("\nПопытаемся удалить несуществующий элемент: 100:\n");
    root = avl_delete(root, 100);

    //добавление новых элементов после удалений
    printf("\nДобавим новые элементы: 15, 5, 35\n");
    root = avl_add(root, 15);
    root = avl_add(root, 5);
    root = avl_add(root, 35);
    
    printf("\nFinal tree structure:\n");
    print_tree(root, 0);
    printf("\n");

    return 0;
}