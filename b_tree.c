#include <stdio.h>
#include <stdlib.h>
#include <time.h>

typedef int bool;
#define FALSE 0
#define TRUE 1
#define MIN_DEGREE 9 // B tree의 최소 degree

typedef struct Btree_node Btree_node;
typedef struct Btree Btree;
typedef struct Btree_return Btree_return;

// B tree를 구성하는 노드 구조체
struct Btree_node {
	int key_num;
	int key[2 * MIN_DEGREE - 1];
	Btree_node* child[2 * MIN_DEGREE];
	bool is_leaf;
};

// B tree 구조체
typedef struct Btree {
	Btree_node* root;
};

Btree_node* malloc_Btree_node();
void create_Btree(Btree* tree);
void insert_Btree(Btree* tree, int key);
void split_child_Btree(Btree_node* parent, int index);
void insert_nonfull_Btree(Btree_node* node, int key);


// 새로운 B tree 노드를 동적메모리 할당으로 생성하는 함수
Btree_node* malloc_Btree_node() {
	Btree_node* new = (Btree_node*)malloc(sizeof(Btree_node));
	if (new == NULL) {
		printf("malloc failed\n");
		exit(1);
	}
}

// 새로운 B tree를 생성하는 함수
void create_Btree(Btree* tree) {
	Btree_node* root = malloc_Btree_node();
	root->key_num = 0;
	root->is_leaf = TRUE;

	tree->root = root;
}
// B tree의 root가 full이면 분할하고, key를 삽입하는 함수
void insert_Btree(Btree* tree, int key) {
	Btree_node* r = tree->root;

	if (r->key_num == 2 * MIN_DEGREE - 1) { // 루트가 full이면 
		// 루트 노드를 분할한 후에 값을 삽입
		Btree_node* new_root = malloc_Btree_node();
		tree->root = new_root;
		new_root->is_leaf = FALSE;
		new_root->key_num = 0;
		new_root->child[0] = r;

		split_child_Btree(new_root, 0);
		insert_nonfull_Btree(new_root, key);
	}
	else { // 루트가 full이 아니면
		insert_nonfull_Btree(r, key);
	}
}

// full인 노드를 둘로 분할하는 함수
void split_child_Btree(Btree_node* parent, int index) {
	Btree_node* new = malloc_Btree_node(); // 새로운 형제 노드
	Btree_node* target = parent->child[index]; // 분할되는 노드
	new->is_leaf = target->is_leaf;
	new->key_num = MIN_DEGREE - 1;
	int i;
	
	// 키를 새로운 노드로 이동시킴
	for (i = 0; i < MIN_DEGREE - 1; i++) {
		new->key[i] = target->key[MIN_DEGREE + i];
	}

	// 분할될 노드가 leaf가 아니면 새로운 노드로 자식들을 이동시킴
	if (target->is_leaf == FALSE) {
		for (i = 0; i < MIN_DEGREE; i++) {
			new->child[i] = target->child[MIN_DEGREE + i];
		}
	}
	target->key_num = MIN_DEGREE - 1;
	
	// 부모 노드에 새로운 자식 노드를 추가
	for (i = parent->key_num; i >= index + 1; i--) {
		parent->child[i + 1] = parent->child[i];
	}
	parent->child[index + 1] = new;

	// 부모노드에 분할된 자식 노드의 중간 키를 추가
	for (i = parent->key_num - 1; i >= index; i--) {
		parent->key[i + 1] = parent->key[i];
	}
	parent->key[index] = target->key[MIN_DEGREE - 1];
	parent->key_num++;
}

// leaf에 노드를 추가하거나 leaf를 찾기 위해서 재귀를 수행하는 함수
void insert_nonfull_Btree(Btree_node* node, int key) {
	int i = node->key_num - 1;

	if (node->is_leaf == TRUE) { 
		// leaf면 key를 삽입
		while (i >= 0 && key < node->key[i]) {
			node->key[i + 1] = node->key[i];			
			i--;
		}
		node->key[i + 1] = key;
		node->key_num++;
	}
	else { 
		// leaf가 아니면 자식 노드를 탐색
		while (i >= 0 && key < node->key[i]) {
			i--;
		}
		i++;
		if ( (node->child[i])->key_num == 2 * MIN_DEGREE - 1) {
			// 탐색할 자식이 full이면 분할
			split_child_Btree(node, i);
			if (key > node->key[i]) {
				i++;
			}
		}
		insert_nonfull_Btree(node->child[i], key); 
	}
}

int main(void) {
	clock_t start, end;
	double enlapsed_time;

	srand((unsigned int)time(NULL));

	Btree tree;
	create_Btree(&tree);

	start = clock();
	
	for (int i = 0; i < 1000; i++) {
		int key = rand() % 10000;
		insert_Btree(&tree, key); 
	}

	end = clock();

	enlapsed_time = (double)(end - start);

	printf(" (t = %d) enlapsed_time: %f", MIN_DEGREE, enlapsed_time);
}