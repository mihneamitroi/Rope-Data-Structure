/* Copyright 2021 <Bulica Andrei & Mitroi Mihnea-Cristian> */
#include "./rope.h"
#include "./utils.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <assert.h>

#define EMPTY ""

/* Implementare functie strdup */
char *my_strdup(const char *src) {
    if (src == NULL)
        return "";
    unsigned int size = strlen(src) + 1;
	char *dst = calloc(size, 1);
    DIE(dst == NULL, "Malloc failed!");
    memcpy(dst, src, size);
    return dst;
}

/* Functia aloca memorie pentru nod si initializeaza campurile acestuia cu
    0 sau NULL. */
RopeNode* makeRopeNode(const char* str) {
    // TODO(Bulica Andrei)
    RopeNode* rope_node;

    rope_node = calloc(1, sizeof(*rope_node));
    DIE(rope_node == NULL, "rope_node calloc");

    rope_node->left = NULL;
    rope_node->right = NULL;

    rope_node->str = str;
    rope_node->weight = 0;

    return rope_node;
}

/* Functia aloca memorie pentru arbore si pune ca radacina nodul primit ca
    parametru. */
RopeTree* makeRopeTree(RopeNode* root) {
    // TODO(Bulica Andrei)
    RopeTree* rope_tree = calloc(1, sizeof(*rope_tree));
	DIE(!rope_tree, "rope_tree calloc");
	rope_tree->root = root;
	return rope_tree;
}

/* Functia printeaza toate stringurile stocate in subarborii nodului pasat ca
    parametru. */
void printRopeNode(RopeNode* rn) {
    if (!rn)
        return;

    if (!(rn->left) && !(rn->right)) {
        printf("%s", rn->str);
        return;
    }

    printRopeNode(rn->left);
    printRopeNode(rn->right);
}

/* Functia printeaza toate stringurile in arborele pasat ca parametru. */
void printRopeTree(RopeTree* rt) {
    if (rt && rt->root) {
        printRopeNode(rt->root);
        printf("%s", "\n");
    }
}

void debugRopeNode(RopeNode* rn, int indent) {
    if (!rn)
        return;

    for (int i = 0; i < indent; ++i)
        printf("%s", " ");

    if (!strcmp(rn->str, EMPTY))
        printf("# %d\n", rn->weight);
    else
        printf("%s %d\n", rn->str, rn->weight);

    debugRopeNode(rn->left, indent+2);
    debugRopeNode(rn->right, indent+2);
}


/* Functia returneaza valoarea greutatii totale stocate in arbore. */
int getTotalWeight(RopeNode* rt) {
    if (!rt)
        return 0;

    if (rt->left == NULL && rt->right == NULL)
        return strlen(rt->str);

    return rt->weight + getTotalWeight(rt->right);
}

/* Functia cauta nodul in care se afla char-ul de pe pozitia idx. */
RopeNode* getNode(RopeNode* node, int *idx) {
	if (node->weight <= *idx && node->right != NULL) {
        *idx = *idx - node->weight;
        return getNode(node->right, idx);
    }

    if (node->left != NULL) {
        return getNode(node->left, idx);
    }

    return node;
}

/* Functia salveaza in vectorul v toate nodurile prin care se trece pentru a
    ajunge din nodul pasat ca parametru la frunza ce contine char-ul de pe
    pozitia idx. */
RopeNode* getParents(RopeNode* node, RopeNode **v, int *idx, int *cnt) {
	v[*cnt] = node;
    *cnt = *cnt + 1;
    if (node->weight <= *idx && node->right != NULL) {
        *idx = *idx - node->weight;
        return getParents(node->right, v, idx, cnt);
    }

    if (node->left != NULL) {
        return getParents(node->left, v, idx, cnt);
    }
    return node;
}


/* Functia creeaza un nou arbore a carui radacina are ca fiu stanga primul
arbore pasat ca parametru si ca fiu dreapta cel de-al doilea arbore. */
RopeTree* concat(RopeTree* rt1, RopeTree* rt2) {
    // TODO(Mitroi Mihnea-Cristian)
    RopeNode *new_node = makeRopeNode(my_strdup(EMPTY));
    new_node->left = rt1->root;
    new_node->right = rt2->root;
    new_node->weight = getTotalWeight(rt1->root);
    RopeTree *new_tree = makeRopeTree(new_node);
    return new_tree;
}

/* Functia returneaza char-ul aflat pe pozitia idx raportat la nodul pasat ca
    parametru. */
char getChar(RopeNode* node, int idx) {
    if (node->weight <= idx && node->right != NULL) {
        return getChar(node->right, idx - node->weight);
    }

    if (node->left != NULL) {
        return getChar(node->left, idx);
    }
    return node->str[idx];
}

/* Functia returneaza char-ul aflat pe pozitia idx raportat la radacina
    arborelui pasat ca parametru. */
char indexRope(RopeTree* rt, int idx) {
    // TODO(Mitroi Mihnea-Cristian)
    return getChar(rt->root, idx);
}

/* Functia returneaza string-ul aflat intre pozitiile start si end din arbore.
    */
char* search(RopeTree* rt, int start, int end) {
    // TODO(Bulica Andrei)
    char *sequence = calloc((end - start + 1), 1);
    int i;
    for (i = start; i < end; i++)
        sequence[i - start] = indexRope(rt, i);

    return sequence;
}

/* Functia copiaza tot subarborele aferent nodului pasat ca parametru si
returneaza o un nod copie ce are aceiasi subarbori. */
RopeNode* copyTree(RopeNode* node) {
    if (!node)
        return NULL;

    RopeNode* copy_tree_root = makeRopeNode(my_strdup(node->str));
    copy_tree_root->weight = node->weight;

    copy_tree_root->left = copyTree(node->left);
    copy_tree_root->right = copyTree(node->right);
    return copy_tree_root;
}

/* Functia elibereaza toata memoria stocata in subarborii nodului si apoi
    in nodul pasat. */
void freeTree(RopeNode* node) {
    if (node == NULL)
        return;

    free((void*)node->str);
    freeTree(node->left);
    freeTree(node->right);
    free(node);
}

/* Se cauta nodul unde se va realiza ruperea. Se verifica daca nodul trebuie
    impartit in doua. Se stocheaza o lista cu nodurile ce alcatuiesc drumul de
    la root la nodul respectiv. Se cauta de la nod la root prima legatura de 
    dreapta si apoi se verifica din nod in nod daca nodul fiu este nodul left
    al tatalui(caz in care se elimina legatura din drepta)
*/
SplitPair split(RopeTree* rt, int idx) {
    // TODO(Bulica Andrei)
    SplitPair ropes;
    int index = idx, i, size_string = 0, cnt = 0;
    RopeNode *v[100]={NULL};
    RopeNode *node;
    RopeTree *tree1 = NULL, *tree2 = NULL, *tree3, *tree4;
    tree3 = makeRopeTree(copyTree(rt->root));
    if (idx == 0) {
        node = makeRopeNode(my_strdup(EMPTY));
        ropes.left = node;
        ropes.right = tree3->root;
        free(tree3);
        return ropes;
    }
    node = getNode(tree3->root, &index);
    size_string = strlen(node->str);

    if (index != 0) {
        RopeNode *node_left, *node_right;
        char *s1 = calloc(index + 1, 1), *s2 = calloc(size_string -
                                                      index + 1, 1);
        for (i = 0; i < index; i++)
            s1[i] = node->str[i];

        for (i = 0; i < size_string - index; i++)
            s2[i] = node->str[index + i];

        node_left = makeRopeNode(s1);
        node_right = makeRopeNode(s2);

        free((void*)node->str);
        node->str = NULL;
        node->left = node_left;
        node->right = node_right;
        node->weight = index;
    }

    node = getParents(tree3->root, v, &idx, &cnt);
    cnt -= 2;
    node = v[cnt]->right;

    while (v[cnt + 1] != v[cnt]->right) {
        cnt--;
    }

    node = v[cnt]->right;
    tree1 = makeRopeTree(node);
    v[cnt]->right = NULL;

    for (i = cnt - 1; i >= 0; i--) {
        node = v[i]->right;
        if (v[i + 1] == v[i]->left) {
            tree2 = makeRopeTree(node);
            tree4 = concat(tree1, tree2);
            free(tree2);
            free(tree1);
            v[i]->right = NULL;
            tree1 = tree4;
        }
    }
    ropes.left = tree3->root;
    ropes.right = tree1->root;
    free(tree3);
    free(tree1);
    return ropes;
}

/* Se actualizeaza grutatiile nodurilor din arbore dupa efectuarea unui
    insert sau delete. */
void setWeightTree(RopeNode *rt)
{
    if (!rt || (!rt->left && !rt->right))
        return;
    setWeightTree(rt->left);
    setWeightTree(rt->right);
    rt->weight = getTotalWeight(rt->left);
}


/* Functia insereaza stringul pasat la pozitia idx in arbore. Arborele este 
    rupt la pozitia idx, cele doua perechi rezultate sunt facute fiecare
    arbore si este realizat un nou arbore si dintr-un nod ce contine stringul
    de adaugat. Apoi prima pereche, cea din stanga, este concatenata cu 
    arborele de inserat, iar apoi se concateneaza rezultatul cu cea de-a doua 
    pereche, astfel ca arborele si nodul ce contin stringul de inserat se afla
    practic intre cele doua perechi separate la idx. */
RopeTree* insert(RopeTree* rt, int idx, const char* str) {
    // TODO(Mitroi Mihnea-Cristian)
    RopeTree *first_tree, *result_tree;
    SplitPair new_pair;
    new_pair = split(rt, idx);
    RopeNode *node = makeRopeNode(str);
    RopeTree *between_tree = makeRopeTree(node);
    RopeTree *first_pair = makeRopeTree(new_pair.left);
    RopeTree *second_pair = makeRopeTree(new_pair.right);
    first_tree = concat(first_pair, between_tree);
    result_tree = concat(first_tree, second_pair);
    free(first_tree);
    free(between_tree);
    free(first_pair);
    free(second_pair);
    setWeightTree(result_tree->root);
    return result_tree;
}

/* Functia rupe vechiul arbore in doua puncte, la id-ul aflat la inceputul
    stringului si la cel de la final. Prima si ultima pereche de noduri sunt
    facute arbori si sunt apoi concatenati, portiunea din mijloc fiind
    eliminata. */
RopeTree* delete(RopeTree* rt, int start, int len) {
    // TODO(Mitroi Mihnea-Cristian)
    SplitPair first_pair, last_pair;
    int sum = start + len;
    if (sum > getTotalWeight(rt->root))
        sum = getTotalWeight(rt->root);
    first_pair = split(rt, start);
    last_pair = split(rt, sum);
    RopeTree *first_tree = makeRopeTree(first_pair.left);
    RopeTree *last_tree = makeRopeTree(last_pair.right);
    RopeTree *result_tree = concat(first_tree, last_tree);
    setWeightTree(result_tree->root);
    return result_tree;
}
