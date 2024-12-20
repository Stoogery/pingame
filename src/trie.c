#include "trie.h"
struct Trie_Node *trie_node_init(void *data)
{
	struct Trie_Node *out;

	if((out = malloc(sizeof(struct Trie_Node))) == NULL)
	{
		fprintf(stderr, "trie_node_init: malloc error\n");
	}
	out->data = data;
	out->left = NULL;
	out->right = NULL;
	
	return out;
}
struct Trie trie_init(size_t key_size)
{
	struct Trie out;
	out.key_size = key_size;
	out.root = NULL;

	return out;
}
void trie_insert(struct Trie *trie, void *key, void *data)
{
	char *key_mask = key;
	struct Trie_Node *current;
	size_t i;

	if(trie->root == NULL)
	{
		trie->root = trie_node_init(NULL);
	}
	current = trie->root;
	for(i = 0; i < trie->key_size * CHAR_BIT; i++)
	{
		int bit = (key_mask[i/CHAR_BIT] & (1 << (i % CHAR_BIT))) != 0;
		struct Trie_Node **target;
		if(bit)
		{
			target = &(current->right);
		}
		else
		{
			target = &(current->left);
		}

		if(*target == NULL)
		{
			*target = trie_node_init(NULL);
		}
		current = *target;
	}
	current->data = data;
}
void *trie_search(struct Trie *trie, void *key)
{
	char *key_mask = key;
	struct Trie_Node *current;
	size_t i;

	current = trie->root;
	for(i = 0; i < trie->key_size * CHAR_BIT; i++)
	{
		if(current == NULL)
		{
			return NULL;
		}
		int bit = (key_mask[i/CHAR_BIT] & (1 << (i % CHAR_BIT))) != 0;
		struct Trie_Node **target;
		if(bit)
		{
			target = &(current->right);
		}
		else
		{
			target = &(current->left);
		}
		current = *target;
	}
	return current->data;
}

void _trie_destroy(struct Trie_Node *current_node, void (*data_destructor)(void *))
{
	if(current_node == NULL)
	{
		return;
	}

	if(data_destructor != NULL && current_node->data != NULL)
	{
		(*data_destructor)(current_node->data);
	}
	_trie_destroy(current_node->left, data_destructor);
	_trie_destroy(current_node->right, data_destructor);

	free(current_node);
}
void trie_destroy(struct Trie *trie, void (*data_destructor)(void *))
{
	_trie_destroy(trie->root, data_destructor);
}
