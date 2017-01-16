#include <iostream>
#include <string>
#include "bpt.h"

using namespace std ;

bpt_node *root ;
int node_count ;
int splite_count ;


bpt_node *new_bpt_node()
{
	node_count ++ ;

	bpt_node *p = new bpt_node ;

	p- > is_leaf = false;
    p -> is_root = false;
    p -> key_num = 0;
    p -> pointer[0] = NULL;
    p -> previous = NULL;
    p -> next = NULL;

    return p ;
}


void initial_bpt()
{
    root = new_bpt_node() ;
    root -> is_root = true ;
    root -> is_leaf = true ;
    node_count = 0 ;
    splite_count = 0 ;
}


bpt_node *find_leaf( int key ) 
{
	return tree_search(root, key);
}


bpt_node *tree_search(bpt_node *nodepointer, int key)
{
	if (nodepointer -> is_leaf) {
		return nodepointer;
	} else {
		if ( key < nodepointer -> key[0]) {
			return tree_search((bpt_node *)nodepointer -> pointer[0], key);
		} else {
			if ( key >= nodepointer -> key[key_num - 1]) {
				return tree_search((bpt_node *)nodepointer -> pointer[key_num], key);
			} else {
				for(int i=0; i<key_num; i++) {
					if (nodepointer -> key[i] <= key && key < nodepointer -> key[i+1]) {
						return tree_search((bpt_node *)nodepointer->pointer[i], key);
					}
				}
			}
		}
	}
}


void insert(bpt_node *nodepointer, entry *entry)
{
	if (!nodepointer -> is_leaf) {
		for(int i=0; i<M; i++) {
			if (nodepointer -> key[i] <= entry -> key && entry -> key < nodepointer -> key[i+1]) {

				insert((bpt_node *)nodepointer -> pointer[i], entry);
				
				// usual case, didn't split child
				if(entry == NULL) {
					return;
				} else {
					if (nodepointer -> key_num  < M) {
						insert_in_node((bpt_node *) nodepointer, entry);
						entry = NULL;
						return;
					} else {
						insert_in_node((bpt_node *) nodepointer, entry);
						return;
					}
				}
			}
		}
	} else {
		if (nodepointer -> key_num < M) {
			insert_in_node((bpt_node *) nodepointer, entry);
			entry = NULL;
			return;
		} else {
			insert_in_node((bpt_node *) nodepointer, entry);
			return;
		}
	}
}


void insert_in_node( bpt_node *node , entry *entry) 
{
	int x = 0 ;

    while ( x < node -> key_num && node -> key[ x ] < entry -> key ) x ++ ;
    for ( int i = node -> key_num ; i > x ; i -- )
        node -> key[ i ] = node -> key[ i - 1 ] ;
    for ( int i = node -> key_num + 1 ; i > x + 1 ; i -- )
        node -> pointer[ i ] = node -> pointer[ i - 1 ] ;

    node -> key[ x ] = entry -> key ;
    node -> pointer[ x + 1 ] = entry -> value ;
    node -> key_num ++ ;

    if ( node -> key_num == M ) // need to split
        split( node ) ;

    return;
}


void split( bpt_node *node )
{
    splite_count ++ ;
    bpt_node *nodd = new_bpt_node() ;
    int mid_key = node -> key[ M / 2 ] ;

    nodd -> key_num = M - M / 2 + 1 ; // is that right? -1 or?
    for ( int i = 0 ; i < nodd -> key_num ; i ++ )
    {
        nodd -> key[ i ] = node -> key[ i + ( M / 2 ) ] ;
        nodd -> pointer[ i ] = node -> pointer[ i + ( M / 2 ) ] ;
    }
    // Q: if node is leaf? it always put >km pointer on most right side?
    nodd -> pointer[ nodd -> key_num ] = node -> pointer[ M ] ; // pointer when key > km
    node -> key_num = M / 2 ;

    // what is the meaning? record?
    entry -> key = nodd -> key[0];
    entry -> value = &(nodd) ;

    if(node->is_root) {

		node -> is_root = false ;
		root = new_bpt_node() ;
		root -> is_root = true ;
		root -> key[ 0 ] = mid_key ;
		root -> pointer[ 0 ] = node ;
		root -> pointer[ 1 ] = nodd ;
		root -> key_num = 1 ;
		node -> father = nodd -> father = root ;
		// no need to return root?????
	} else {
		nodd -> father = node -> father ;

		if(node->is_leaf) {
        	nodd -> is_leaf = true ;

        	//Set Sibling pointer
        	node -> next = nodd;
        	nodd -> previous = node;
    	}
	}

    return;
}


void delete(bpt_node *nodepointer, int key, entry *oldchildentry) 
{
	if (!nodepointer -> is_leaf) {
		for(int i=0; i<M; i++) {
			if (nodepointer -> key[i] <= entry -> key && entry -> key < nodepointer -> key[i+1]) {
				delete((bpt_node *)nodepointer -> pointer[i], key, oldchildentry);
				
				if(oldchildentry == NULL) {
					return;
				} else {
					
					delete_in_node((bpt_node *) nodepointer, key, oldchildentry);

					if(nodepointer -> key_num >= M/2) {
						oldchildentry = NULL;
						return;
					} else {
						
						bpt_node * s;

						for(int j=0; j<nodepointer->father->key_num; j++) {
							if(nodepointer->father->pointer[j] == &nodepointer) {
								s = nodepointer->father->pointer[j+1];
							}
						}

						if(s -> key_num > M/2) {
							redistribute(nodepointer, s);
							oldchildentry = NULL;
							return;
						} else {
							int key_to_replace = s -> key[0];
							bpt_node* pointer_to_replace = s;

							merge(nodepointer, s);
							entry* M = find_parent_entry(root, s->key[0], key_to_replace, s);
							oldchildentry -> key = M -> key;
							oldchildentry -> value = M -> value;

							insert_in_node(nodepointer, M);
							
							return;
						}
					}

				}


			}
		}
	} else {
		
		delete_in_node((bpt_node *) nodepointer, key, oldchildentry);

		if (nodepointer -> key_num >= M/2) {
			oldchildentry = NULL;
			return;
		} else {
			// next or previous??
			bpt_node *s = (bpt_node*) nodepointer -> next;
			
			int key_to_replace = s -> key[0];
			//bpt_node* pointer_to_replace = s;

			if(s -> key_num > M/2) { 
				redistribute(nodepointer, s);
				// place most left key of s into parent
				replace_parent_entry(nodepointer -> father, s -> key[0], key_to_replace, s);
				oldchildentry = NULL;
				return;
			} else {

				merge(nodepointer, s);
				entry* M = find_parent_entry(nodepointer -> father, s->key[0], key_to_replace, s);
				oldchildentry -> key = M -> key;
				oldchildentry -> value = M -> value;

				return;
			}
		}
	}
}


void delete_in_node( bpt_node *node , int key , entry *oldchildentry)
{
    int x = 0 ;

    if(oldchildentry != NULL) {
    	key = oldchildentry -> key;
    }

    while ( key != node -> key[ x ] ) x ++ ;
    for ( int i = x ; i < node -> key_num - 1 ; i ++ )
        node -> key[ i ] = node -> key[ i + 1 ] ;
    for ( int i = x + 1 ; i < node -> key_num ; i ++ )
        node -> pointer[ i ] = node -> pointer[ i + 1 ] ;
    node -> key_num -- ;
}

void redistribute(bpt_node *L, bpt_node *S) 
{
	//l key num is d - 1
	for(int i=0; i<M; i++) {
		l -> key[i] = S ->key[S -> key_num -1];
		l -> pointer[i] = S -> pointer[S -> key_num -1];

		S ->key[S -> key_num -1] = NULL;
		S -> pointer[S -> key_num -1] = NULL;

		++S -> key_num;
		--l -> key_num;

		if(L -> key_num >= S -> key_num) {
			break;
		}
	}

	return;
}

void merge(bpt_node *L, bpt_node *S) 
{
	for(int i=0; i<M; i++) {
		L -> key[key_num] = S -> key[i];
		L -> pointer[key_num] = L -> pointer[i];

		++L -> key_num;
		--S -> key_num;

		if(S -> key_num == 0) {
			if(L-> is_leaf) {
				L -> next = S -> next;
			} else {

			}

			delete S;

			break;
		}
	}
}

void replace_parent_entry(bpt_node *nodepointer, int key, int key_to_replace, bpt_node *pointer_to_replace) {

	for(int i=0; i<nodepointer->key_num; i++) {
		if (nodepointer -> pointer[i] == pointer_to_replace) {
			nodepointer -> key[i] = key;
			return;
		}
	}
}
		
entry* find_parent_entry(bpt_node *nodepointer, int key, int key_to_replace, bpt_node *pointer_to_replace) {

	for(int i=0; i<nodepointer->key_num; i++) {
		if (nodepointer -> pointer[i] == pointer_to_replace) {
			entry* M; 
			M -> key = key[i];
			M -> value = pointer[i];

			return M;
		}
	}
}

/*entry* find_parent_entry(bpt_node *nodepointer, int key, int key_to_replace, bpt_node *pointer_to_replace) {
	if ( key_to_replace < nodepointer -> key[0]) {
			return replace_parent_entry((bpt_node *)nodepointer -> pointer[0], key_to_replace);
	} else {
		if ( key_to_replace > nodepointer -> key[M - 1]) {
				return replace_parent_entry((bpt_node *)nodepointer -> pointer[M], key_to_replace);
		} else {
			for(int i=0; i<nodepointer->key_num; i++) {
				if (nodepointer -> pointer[i] == pointer_to_replace) {
					entry* M; 
					M -> key = key[i];
					M -> value = pointer[i];

					return M;
				}
			}
		}
	}
}*/