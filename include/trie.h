#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <limits.h>

struct Trie_Node
{
	void *data;
	struct Trie_Node *left;
	struct Trie_Node *right;
};
struct Trie
{
	size_t key_size;
	struct Trie_Node *root;
};

struct Trie_Node *trie_node_init(void *data);
struct Trie trie_init(size_t key_size);
void trie_insert(struct Trie *trie, void *key, void *data);
void *trie_search(struct Trie *trie, void *key);
void _trie_destroy(struct Trie_Node *current_node, void (*data_destructor)(void *));
void trie_destroy(struct Trie *trie, void (*data_destructor)(void *));
