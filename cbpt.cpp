#include <iostream>
#include <string>
#include <cstdio>
#include <cstring>
#include "bpt.h"
#include <iostream>
#include <algorithm>
#include <typeinfo>

using namespace std ;

vector<string> c_relations;
vector<c_bpt_node*> c_trees;

c_bpt_node *c_root ;
int c_total_leaf_page;
int c_total_index_page;


c_bpt_node *new_c_bpt_node();
void initial_c_bpt(string relation);
c_bpt_node *c_find_leaf( c_bpt_node* nodepointer, char* key ) ;
c_bpt_node *c_tree_search(c_bpt_node *nodepointer, char* key);
void c_insert_into_tree(string relation, c_entry *child);
void c_insert(c_bpt_node *nodepointer, c_entry *c_entry);
void c_insert_in_node( c_bpt_node *node , c_entry *c_entry);
void c_split( c_bpt_node *node , c_entry *c_entry);
void c_delete_from_tree(string relation, char* key);
void delete_c_entry(c_bpt_node *nodepointer, char* key, c_entry *oldchildc_entry) ;
void c_delete_in_node( c_bpt_node *node , char* key , c_entry *oldchildc_entry);
void c_redistribute(c_bpt_node *L, c_bpt_node *S) ;
void c_merge(c_bpt_node *L, c_bpt_node *S) ;
void replace_parent_c_entry(c_bpt_node *nodepointer, char* key, c_bpt_node *pointer_to_replace);
void c_replace_key(c_bpt_node *nodepointer, char* key, char* key_to_replace);
c_entry* find_parent_c_entry(c_bpt_node *nodepointer, c_bpt_node *pointer_to_replace);
void c_print_leaf_ascending(c_bpt_node *nodepointer);
void c_print_leaf_descending(c_bpt_node *nodepointer);
void c_traverse(c_bpt_node *nodepointer);
rid* c_query(std::string relation, char* key);
rid* c_query_in_node(c_bpt_node* nodepointer, char* key);
void c_calculate_slotted_page(c_bpt_node* nodepointer, int* slotted_data_page);



c_bpt_node *new_c_bpt_node()
{

	c_bpt_node *p = new c_bpt_node ;

	p -> is_leaf = false;
    p -> is_root = false;
    p -> key_num = 0;
    p -> pointer[0] = NULL;
    p -> previous = NULL;
    p -> next = NULL;
    p -> father = NULL;

    return p ;
}


void initial_c_bpt(string relation)
{

	c_root = new_c_bpt_node();
	c_root -> is_root = true ;
	c_root -> is_leaf = true ;

	c_relations.insert(c_relations.end(), relation);
	c_trees.insert(c_trees.end(), c_root);

    return;
}


c_bpt_node *c_find_leaf( c_bpt_node* nodepointer, char* key ) 
{
	return c_tree_search(nodepointer, key);
}


c_bpt_node *c_tree_search(c_bpt_node *nodepointer, char* key)
{
	if (nodepointer -> is_leaf) {
		return nodepointer;
	} else {
		if ( strcmp(key, nodepointer -> key[0]) < 0) {
			return c_tree_search((c_bpt_node *)nodepointer -> pointer[0], key);
		} else {
			if ( key >= nodepointer -> key[nodepointer -> key_num - 1]) {
				return c_tree_search((c_bpt_node *)nodepointer -> pointer[nodepointer -> key_num], key);
			} else {
				for(int i=0; i<nodepointer -> key_num; i++) {
					if (nodepointer -> key[i] <= key && key < nodepointer -> key[i+1]) {
						return c_tree_search((c_bpt_node *)nodepointer->pointer[i], key);
					}
				}
			}
		}
	}
}

void c_insert_into_tree(string relation, c_entry *child) 
{

	ptrdiff_t pos = find(c_relations.begin(), c_relations.end(), relation) - c_relations.begin();

	if(pos >= c_relations.size()) {
		
		printf("Relation Not Found\n");

	} else {

		c_insert(c_trees.at(pos), child);
	}

	return;
}


void c_insert(c_bpt_node *nodepointer, c_entry *child)
{

	if (!nodepointer -> is_leaf) {
		for(int i=0; i<nodepointer->key_num; i++) {
			if (strcmp(child -> key, nodepointer -> key[nodepointer->key_num-1])>=0) {
				c_insert((c_bpt_node *)nodepointer -> pointer[nodepointer->key_num], child);
			} else if(strcmp(child -> key, nodepointer -> key[0])) {
				c_insert((c_bpt_node *)nodepointer -> pointer[0], child);
			} else if (strcmp(nodepointer -> key[i], child -> key) <= 0 && strcmp(child -> key, nodepointer -> key[i+1]) < 0) {
				c_insert((c_bpt_node *)nodepointer -> pointer[i + 1], child);
			}

			if((strcmp(nodepointer -> key[i], child -> key) <= 0 && strcmp(child -> key, nodepointer -> key[i+1])) < 0 || strcmp(child -> key, nodepointer -> key[0]) < 0 || strcmp(child -> key, nodepointer -> key[nodepointer->key_num-1]) > 0) {
				// usual case, didn't split child
				// Testing
				if(strcmp(child->key, "") == 0) {
					return;
				} else {

					if (nodepointer -> key_num  < N - 1) {
						c_insert_in_node(nodepointer, child);

						if(((c_bpt_node *)child -> value) !=0 && ((c_bpt_node *)child -> value) -> is_leaf) {
							int x = 0;
							while (nodepointer -> pointer[ x ] != child -> value) x ++ ;

							if (x < N && ((c_bpt_node *)nodepointer -> pointer[ x ]) -> next != 0)
								((c_bpt_node *)nodepointer -> pointer[ x ]) -> next = nodepointer -> pointer[ x + 1 ];

						}

						memset(child, 0, sizeof(child));
					} else {
						c_insert_in_node(nodepointer, child);
					}

					return;
				}
			}
			
		}
	} else {

		if (nodepointer -> key_num < N-1) {

			c_insert_in_node(nodepointer, child);
			memset(child, 0, sizeof(child));
		} else {
			c_insert_in_node(nodepointer, child);
		}

		return;
	}
}


void c_insert_in_node( c_bpt_node *node , c_entry *child) 
{

	int x = 0 ;

    while ( x < node -> key_num && node -> key[ x ] < child -> key ) x ++ ;
    for ( int i = node -> key_num ; i > x ; i -- )
        strcpy(node -> key[ i ], node -> key[ i - 1 ] );
    for ( int i = node -> key_num + 1 ; i > x + 1 ; i -- )
        node -> pointer[ i ] = node -> pointer[ i - 1 ] ;

    strcpy(node -> key[ x ], child -> key) ;

    if(node->is_leaf && child -> info != 0) {
    	node -> pointer[ x + 1 ] = child -> info;
    	child -> info = NULL;
    }
    else
    	node -> pointer[ x + 1 ] = child -> value ;
    
    node -> key_num += 1 ;

    if ( node -> key_num == N ) { // need to split
        c_split( node, child ) ;
    }
    else {
    	memset(child, 0, sizeof(child));
    }

    return;
}


void c_split( c_bpt_node *node, c_entry *child)
{
	//traverse(root);

    c_bpt_node *nodd = new_c_bpt_node() ;

    char* mid_key = node -> key[ N / 2 ] ;

	nodd -> key_num = N - N / 2 ; 

	if(!node->is_leaf) {
    	nodd -> key_num = N - N / 2 - 1; 

    	for ( int i = 0 ; i < nodd -> key_num ; i ++ )
    	{
	        strcpy(nodd -> key[ i ], node -> key[ i + ( N / 2 ) + 1] );
	        nodd -> pointer[ i ] = node -> pointer[ i + ( N / 2 ) + 1] ;

	        //empty it
	        strcpy(node -> key[ i + ( N / 2 ) + 1], "");
	        //cout<<"clear:"<<node -> key[ i + ( N / 2 ) + 1];

	        // transfer father
	        ((c_bpt_node *)nodd -> pointer[ i ]) -> father = nodd;

    	}
	}
    else 
    {

    	for ( int i = 0 ; i < nodd -> key_num ; i ++ )
    	{
	        strcpy(nodd -> key[ i ], node -> key[ i + ( N / 2 ) ] );
	        nodd -> pointer[ i + 1 ] = node -> pointer[ i + ( N / 2 ) + 1] ;


	        //empty it
	        strcpy(node -> key[ i + ( N / 2 ) ], "");
			//node -> pointer[ i + ( M / 2 ) + 1] = NULL;
    	}
    }

    nodd -> pointer[ nodd -> key_num ] = node -> pointer[ N ] ; // pointer when key > km

    if(!node->is_leaf) {
    	((c_bpt_node *)nodd -> pointer[ nodd -> key_num ])->father = nodd;
    }
    
    node -> pointer[ N ] = NULL;
    node -> key_num = N / 2 ;

    if(node->is_leaf)
	    strcpy(child -> key, nodd -> key[0]);
	else
		strcpy(child -> key, mid_key);


    child -> value = nodd ;

    if(node->is_root) {

		node -> is_root = false ;
		c_root = new_c_bpt_node() ;
		c_trees.back() = c_root;
		c_root -> is_root = true ;

		//cout<<"root"<<mid_key;
		// Modify mid key to child key
		strcpy(c_root -> key[ 0 ], child ->key );

		//cout<<"root"<<c_root->key[0];
		c_root -> pointer[ 0 ] = node ;
		c_root -> pointer[ 1 ] = nodd ;
		c_root -> key_num = 1 ;
		node -> father = nodd -> father = c_root ;

		c_root -> is_leaf = false;
		
		if(node -> is_leaf) {
			nodd -> is_leaf = true;
			node -> next = nodd;
        	nodd -> previous = node;
		} 

	} else {
		nodd -> father = node -> father ;

		if(node->is_leaf) {
        	nodd -> is_leaf = true ;

        	//Set Sibling pointer
        	if (node -> next != 0)
        		((c_bpt_node *)node -> next) -> previous = nodd;

        	node -> next = nodd;
        	nodd -> previous = node;    	

    	}
	}

	//traverse(root);
	//cout<<root->key_num;
    return;
}

void c_delete_from_tree(string relation, char* key) 
{	

	ptrdiff_t pos = find(c_relations.begin(), c_relations.end(), relation) - c_relations.begin();

	if(pos >= c_relations.size()) {
		
		printf("Relation Not Found\n");

	} else {

		c_entry *dummy = new c_entry;

		delete_c_entry(c_trees.at(pos), key, dummy);

	}

	return;
}

void delete_c_entry(c_bpt_node *nodepointer, char* key, c_entry *oldchildc_entry) 
{
	if (!nodepointer -> is_leaf) {
		for(int i=0; i<nodepointer->key_num; i++) {
			if (strcmp(key, nodepointer -> key[nodepointer->key_num-1]) >= 0) {
				delete_c_entry((c_bpt_node *)nodepointer -> pointer[nodepointer->key_num], key, oldchildc_entry);
			} else if(strcmp(key, nodepointer -> key[0]) < 0) {
				delete_c_entry((c_bpt_node *)nodepointer -> pointer[0], key, oldchildc_entry);
			} else if (strcmp(nodepointer -> key[i], key) <= 0 && strcmp(key, nodepointer -> key[i+1]) < 0) {
				delete_c_entry((c_bpt_node *)nodepointer -> pointer[i + 1], key, oldchildc_entry);
			}

			/*cout<<"out"<<nodepointer -> key[i];
			cout<<"in"<<key;
			cout<<"num:"<<nodepointer -> key_num;
			cout<<"oldchild:"<<oldchildc_entry->key;*/
	
			
			if((strcmp(nodepointer -> key[i], key) <= 0 && strcmp(key, nodepointer -> key[i+1])) < 0 || strcmp(key, nodepointer -> key[0]) < 0 || strcmp(key, nodepointer -> key[nodepointer->key_num-1]) >= 0) {

				// Testing
				if(oldchildc_entry->value == 0) {
					return;
				} else {
	
					c_delete_in_node((c_bpt_node *) nodepointer, key, oldchildc_entry);
					//cout<<"he";
					//traverse(root);


					if(nodepointer -> key_num >= N/2) {
						memset(oldchildc_entry, 0, sizeof(oldchildc_entry));
					} else {
						
						c_bpt_node * s = new c_bpt_node;
						//cout<<nodepointer->key_num;

						if(!nodepointer -> is_root) {

							bool is_left = false;
							bool need_to_merge = false;
							
							int count = ((c_bpt_node *)nodepointer->father) -> key_num;
							c_bpt_node * parent = (c_bpt_node *)nodepointer -> father;

							for(int j=0; j<count; j++) {
								if(parent -> pointer[j] == nodepointer && j > 0) {
									if(((c_bpt_node *) parent -> pointer[j+1]) -> key_num > N/2) {
										s = (c_bpt_node *)parent -> pointer[j+1];
									} else if(((c_bpt_node *)parent -> pointer[j-1]) -> key_num > N/2){
										s = (c_bpt_node *)parent -> pointer[j-1];
										is_left = true;
									} else {
										need_to_merge = true;

										if(((c_bpt_node *)parent -> pointer[j-1]) == 0) {
											s = (c_bpt_node *)parent -> pointer[j+1];
										} else {
											s = (c_bpt_node *)parent -> pointer[j-1];
											is_left = true;
										}

									}
									break;
								} else if (parent -> pointer[0] == nodepointer) {
									if(((c_bpt_node *)parent->pointer[1]) -> key_num > N/2) {
										s = (c_bpt_node *)parent -> pointer[1];
									} else {
										need_to_merge = true;
										s = (c_bpt_node *)parent -> pointer[1];
									}

									break;
								} else if(parent -> pointer[count] == nodepointer) {
									if(((c_bpt_node *)parent -> pointer[count - 1]) -> key_num > N/2) {
										s = (c_bpt_node *)parent -> pointer[count - 1];
										is_left = true;
									} else {
										need_to_merge = true;
										is_left = true;
										s = (c_bpt_node *)parent -> pointer[count - 1];
									}

									break;
								}
							}

							c_entry *m = new c_entry;

							if(!need_to_merge) {
								//cout<<"re";
								if(!is_left) {
									//printf("right\n");
									c_redistribute((c_bpt_node *) nodepointer, s);
								}
								else {
									//printf("left\n");
									printf("%s\n", nodepointer->key[0]);
									c_redistribute(s, (c_bpt_node *) nodepointer);


								}
								memset(oldchildc_entry, 0, sizeof(oldchildc_entry));
							} else {
								//cout<<"mer";

								c_bpt_node* pointer_to_replace = new c_bpt_node;
								pointer_to_replace = s;


								if(!is_left) {
									c_entry* m = find_parent_c_entry((c_bpt_node *)s -> father, s);
									strcpy(oldchildc_entry -> key, m -> key);
									oldchildc_entry -> value = m -> value;
									c_insert_in_node(nodepointer, m);
									c_merge((c_bpt_node *) nodepointer, s);
								}
								else {
									//cout<<"left";
									//traverse(root);
									c_entry* m = find_parent_c_entry((c_bpt_node *)nodepointer -> father, nodepointer);
									strcpy(oldchildc_entry -> key, m -> key);
									oldchildc_entry -> value = m -> value;
									c_insert_in_node(s, m);
									c_merge(s, (c_bpt_node *) nodepointer);
								}			
							}
						} else {
							// Not sure...
							if(nodepointer->key_num <= 1) {
								//printf("here?\n");
								//cout<<nodepointer->key_num;
								if(((c_bpt_node *)nodepointer->pointer[0]) -> key_num + ((c_bpt_node *)nodepointer -> pointer[1]) -> key_num + nodepointer -> key_num < N) {
									//merge with root
									// Problem with lost rid content
									if(((c_bpt_node *)nodepointer -> pointer[0]) -> key_num > 0) {
										c_root = (c_bpt_node *)nodepointer -> pointer[0];
										c_trees.back() = c_root;
									} else {
										c_root = (c_bpt_node *)nodepointer -> pointer[1];
										c_trees.back() = c_root;
									}

									c_root -> next = NULL;
									c_root -> previous = NULL;
									c_root -> is_root = true;
									c_root -> father = NULL;

								}
							}
						}
					}

					return;
				}
			}
		}
	} else {
		//cout<<"leaf";
		//cout<<((c_bpt_node *) nodepointer) -> key_num;

		//cout<<((c_bpt_node *) nodepointer) -> key[0];
		c_delete_in_node((c_bpt_node *) nodepointer, key, oldchildc_entry);
		//cout<<((c_bpt_node*) nodepointer -> previous)->key[0];

		if(nodepointer->is_root && nodepointer->is_leaf) {
			return;
		}

		if (nodepointer -> key_num >= N/2) {
			//cout<<"nothing happen?";
			memset(oldchildc_entry, 0, sizeof(oldchildc_entry));
			return;
		} else {

			//cout<<((c_bpt_node *) nodepointer) -> key_num;
			c_bpt_node *s = new c_bpt_node; 
			c_entry* n = new c_entry;
			//cout<< (c_bpt_node*) nodepointer -> next;
			//cout<< "previous"<<(c_bpt_node*) nodepointer -> previous;


			n = find_parent_c_entry((c_bpt_node *) nodepointer -> father, nodepointer);

			bool need_to_merge = false;
			bool is_left = false;

			//cout<< "count:" <<((c_bpt_node*) nodepointer -> previous)->key_num;
			//cout<< ((c_bpt_node*) nodepointer -> previous) -> key[1];
			//cout<< nodepointer->key[0];
			//cout<<"check";
			//print_leaf_descending(root);


			if(nodepointer -> next != 0 && ((c_bpt_node*) nodepointer -> next) -> key_num > N/2 && ((c_bpt_node*) nodepointer -> next) -> father == nodepointer -> father) {
				s = (c_bpt_node*) nodepointer -> next;
				//cout<<"here";
			} else if(nodepointer -> previous != 0 && ((c_bpt_node*) nodepointer -> previous) -> key_num > N/2 && ((c_bpt_node*) nodepointer -> previous) -> father == nodepointer -> father) {
				if(((c_bpt_node*) nodepointer -> previous) -> key_num > N/2) {
					s = (c_bpt_node*) nodepointer -> previous;
					is_left = true;
					//cout<<"there";
				}
			} else {

				need_to_merge = true;
				//cout<<((c_bpt_node*) nodepointer -> previous);
				if(nodepointer -> previous == 0 || ((c_bpt_node *)nodepointer->father) -> pointer[0] == nodepointer || ((c_bpt_node*) nodepointer -> previous) -> father != nodepointer -> father) {
					s = (c_bpt_node*) nodepointer -> next;
				} else {

					s = (c_bpt_node*) nodepointer -> previous;
					is_left = true;
				}
			}
			
			if(!need_to_merge) { 
				//cout<<"redistribute";

				if(!is_left) {
					c_redistribute((c_bpt_node *) nodepointer, s);
					//cout<<s -> key[0];
					//cout<<"here";
					// father problem
					//cout<<((c_bpt_node *)s->father)->key[0];
					replace_parent_c_entry((c_bpt_node *) s -> father, s -> key[0], s);
				} else {
					c_redistribute(s,(c_bpt_node *) nodepointer);
					//cout<<nodepointer->key[0];
					replace_parent_c_entry((c_bpt_node *) nodepointer -> father, nodepointer -> key[0], nodepointer);
				}
				// place most left key of s into parent
				//print_leaf_ascending(root);
				
				//cout<<"fuck";
				//cout<<((c_bpt_node *) s -> father) -> key[0];

				memset(oldchildc_entry, 0, sizeof(oldchildc_entry));
			} else {
				//cout<<"merge";

				c_entry* m = new c_entry;

				if(!is_left) {
					c_merge(nodepointer, s);
					m = find_parent_c_entry((c_bpt_node *) nodepointer -> father, s);
				} else {
					//cout<<"left";
					//problem
					c_merge(s, nodepointer);
					m = find_parent_c_entry((c_bpt_node *) nodepointer -> father, nodepointer);
				}

				//cout<<"after merge:"<<s->key[s -> key_num - 1];
				strcpy(oldchildc_entry -> key, m -> key);
				//cout<<"old:"<<oldchildc_entry -> key;
				oldchildc_entry -> value = m -> value;
			}

			return;
		}
	}
}


void c_delete_in_node( c_bpt_node *node , char* key , c_entry *oldchildc_entry)
{


    int x = 0 ;
    
    if(strcmp(oldchildc_entry->key, "") != 0) {
    	key = oldchildc_entry -> key;
    }

    while ( strcmp(key, node -> key[ x ]) != 0  && x < node -> key_num) x ++ ;

    if(strcmp(node -> key[x], key) == 0) {
	    for ( int i = x ; i < node -> key_num - 1 ; i ++ ) {
	        strcpy(node -> key[ i ], node -> key[ i + 1 ]) ;
	        strcpy(node -> key[i + 1], "");
	    }
	    for ( int i = x + 1 ; i < node -> key_num ; i ++ ) {
	        node -> pointer[ i ] = node -> pointer[ i + 1 ] ;
	        node -> pointer[ i + 1 ] = NULL;
	    }
	    node -> key_num -- ;

	    // Testing propagate delete key to parent and replace it
	    if(node->is_leaf) {
			c_bpt_node * dummy = node;

			while(dummy->father != 0) {
				dummy = (c_bpt_node*) dummy->father;
				c_replace_key(dummy, node->key[x], key);
			}
		}

	} else {
		oldchildc_entry = NULL;
		return;
	}
}

void c_redistribute(c_bpt_node *L, c_bpt_node *S) 
{
	//l key num is d - 1
	for(int i=0; i<N; i++) {
		if(L->is_leaf) {
			
			// Moving from right
			if(L -> key_num < S -> key_num) { 
				strcpy(L -> key[L -> key_num], S -> key[i]);
				L -> pointer[L->key_num + 1] = S -> pointer[i + 1];

				c_entry *dummy = new c_entry;
				c_delete_in_node(S, S->key[i], dummy);
				++L -> key_num;

				if(L -> key_num >= S -> key_num) {
					break;
				}
			} else {
				//Shift S key
				for(int j=S -> key_num;j>0;j--) {
 					strcpy(S -> key[j], S -> key[j-1]);
 					S -> pointer[j+1] = S -> pointer[j]; 					
				}

				strcpy(S -> key[i], L -> key[L->key_num - 1 - i]);
				S -> pointer[i + 1] = L -> pointer[L -> key_num - i];

				c_entry *dummy = new c_entry;
				c_delete_in_node(L, L->key[L->key_num -1 - i], dummy);
				++S -> key_num;

				if(S -> key_num >= L -> key_num) {
					break;
				}

			}
		} else {

			if(L -> key_num < S -> key_num) { 

				c_entry* dummy = new c_entry;
				dummy = find_parent_c_entry((c_bpt_node*)S->father, S);

				strcpy(L -> key[L -> key_num], dummy -> key);
				L -> pointer[L -> key_num + 1] = S -> pointer[i];

				// move child father
				((c_bpt_node *)L -> pointer[L -> key_num + 1])->father = L;

				for(int j=0;j<((c_bpt_node*)S->father)->key_num; j++) {
					if(strcmp(((c_bpt_node*)S->father) -> key[j], dummy -> key) == 0) {
						strcpy(((c_bpt_node*)S->father) -> key[j], S -> key[i]);
						break;
					}
				}


				for(int k=0; k<S->key_num; k ++) {
					if(k < S->key_num - 1) {
						strcpy(S -> key[k], S -> key[k+1]);
						strcpy(S -> key[k + 1], "");
					}

					S->pointer[k] = S->pointer[k+1];
					S -> pointer[k + 1] = NULL;
				}

				--S -> key_num;
				++L -> key_num;

				if(L -> key_num >= N/2) {
					break;
				}
			} else {
				c_entry* dummy = new c_entry;
				dummy = find_parent_c_entry((c_bpt_node*)S->father, S);


				//cout<< ((c_bpt_node*)L->father)->key[0];
				//cout<<L->key[0];
				for(int j=0;j<((c_bpt_node*)L->father)->key_num; j++) {
					if(strcmp(((c_bpt_node*)L->father) -> key[j], dummy -> key) == 0) {
						strcpy(((c_bpt_node*)L->father) -> key[j], L -> key[L -> key_num -1 - i]);
						break;
					}
				}

				for(int k=0; k<S->key_num; k ++) {
					
					if(k == 0) {
						S -> pointer[S -> key_num + 1] = S -> pointer[S -> key_num];
					}

					strcpy(S -> key[S -> key_num - k], S -> key[k]);
					S -> pointer[S -> key_num - k] = S -> pointer[k];
	
				}

				strcpy(S -> key[0], dummy -> key);
				S -> pointer[0] = L -> pointer[L -> key_num - i];

				L -> pointer[L -> key_num - i] = NULL; 
				strcpy(L -> key[L -> key_num -1 - i], "");

				// move child father
				((c_bpt_node *)S -> pointer[0]) -> father = S;

				++S -> key_num;
				--L -> key_num;

				if(S -> key_num >= N/2) {
					break;
				}

			}
		}
	}

	return;
}

void c_merge(c_bpt_node *L, c_bpt_node *S) 
{
	for(int i=0; i<N; i++) {
		if(!L-> is_leaf) {
			strcpy(L -> key[L -> key_num], S -> key[i]);
			L -> pointer[L -> key_num] = S -> pointer[i];
		} else {
			strcpy(L -> key[L -> key_num], S -> key[i]);
			L -> pointer[L -> key_num + 1] = S -> pointer[i + 1];
		}

		//cout<<((c_bpt_node *)L -> pointer[L -> key_num]);

		if(((c_bpt_node *)L -> pointer[L -> key_num]) != 0 && !L->is_leaf)
			((c_bpt_node *)L -> pointer[L -> key_num]) -> father = L;

		++L -> key_num;
		--S -> key_num;

		if(S -> key_num == 0) {
			if(L-> is_leaf && S->next != 0) {
				((c_bpt_node *)(S->next)) -> previous = L;
				L -> next = S -> next;
			} else {
				L -> pointer[L -> key_num] = S -> pointer[i + 1];
				((c_bpt_node *)L -> pointer[L -> key_num]) -> father = L;
			}

			delete S;

			return;
		}
	}
}

void replace_parent_c_entry(c_bpt_node *nodepointer, char* key, c_bpt_node *pointer_to_replace) 
{

	for(int i=0; i<nodepointer->key_num; i++) {
		// Not sure having other conditions....
		if (nodepointer -> pointer[i + 1] == pointer_to_replace) {
			strcpy(nodepointer -> key[i], key);
			return;
		}
	}
}

void c_replace_key(c_bpt_node *nodepointer, char* key, char* key_to_replace) 
{

	for(int i=0; i<nodepointer->key_num; i++) {
		if (strcmp(nodepointer -> key[i], key_to_replace) == 0) {
			strcpy(nodepointer -> key[i], key);
			return;
		}
	}
}
		
c_entry* find_parent_c_entry(c_bpt_node *nodepointer, c_bpt_node *pointer_to_replace) 
{

	for(int i=0; i<nodepointer->key_num; i++) {
		if (nodepointer -> pointer[i + 1] == pointer_to_replace) {
			c_entry* dummy = new c_entry; 
			strcpy(dummy -> key, nodepointer -> key[i]);
			dummy -> value = nodepointer -> pointer[i + 1];

			return dummy;
		}
	}
}


void c_traverse(c_bpt_node *nodepointer) 
{

	for(int i=0; i<nodepointer -> key_num; i++) {
		if(i == 0 && nodepointer->is_leaf) {
			c_total_leaf_page++;
			printf("leaf: ");
		}

		printf("%s ", nodepointer->key[i]);
	}

	if (!nodepointer -> is_root) {
		printf("\nfather 1st key: %s\n", ((c_bpt_node *)nodepointer->father)->key[0]);
	}
	else {
		printf("\n");
	}

	if(!nodepointer -> is_leaf) {
		
		c_total_index_page ++;

		for(int i=0; i<=nodepointer -> key_num; i++) {
			c_traverse((c_bpt_node *)nodepointer -> pointer[i]);
			continue;
		}
	}

	return;
}

void c_print_leaf_ascending(c_bpt_node *nodepointer) 
{
	if(!nodepointer->is_leaf) {
		c_print_leaf_ascending((c_bpt_node *)nodepointer->pointer[0]);
	} else {
		while(nodepointer != 0) {
			for (int i = 0; i < nodepointer->key_num; i++)
			{
				printf("key: %s ,", nodepointer->key[i]);
				//cout<< ((rid*)nodepointer->pointer[i+1]);
				printf("slot id: %u ,", ((rid*)nodepointer->pointer[i+1])->slot_id);
				printf("page id: %u ;", ((rid*)nodepointer->pointer[i+1])->page_id);
			}

			printf("\nkey num: %d\n", nodepointer->key_num);
			nodepointer = (c_bpt_node *)nodepointer->next;
		}
	}
}

void c_print_leaf_descending(c_bpt_node *nodepointer) 
{
	if(!nodepointer->is_leaf) {
		c_print_leaf_descending((c_bpt_node *)nodepointer->pointer[nodepointer -> key_num]);
	} else {
		while(nodepointer != 0) {
			for (int i = 0; i < nodepointer->key_num; i++)
			{
				printf("%s ", nodepointer->key[i]);
			}

			printf("\nkey num: %d\n", nodepointer->key_num);
			nodepointer = (c_bpt_node *)nodepointer->previous;
		}
	}
}


rid* c_query(string relation, char* key) {
	ptrdiff_t pos = find(c_relations.begin(), c_relations.end(), relation) - c_relations.begin();

	if(pos >= c_relations.size()) {
    	printf("*********Relation Not Found**********\n");
    	return NULL;
	} else {
		//cout<<trees.at(pos) -> key[0];
		return c_query_in_node(c_trees.at(pos), key);
	}
}

rid* c_query_in_node(c_bpt_node* nodepointer, char* key) {
	
	c_bpt_node* leaf = c_find_leaf(nodepointer, key);
	int i = 0;

	while(true) {
		if(strcmp(leaf->key[i], key) == 0) {
			return ((rid*)leaf->pointer[i+1]);
			break;
		} else if(strcmp(leaf->key[i], key) > 0) {
			break;
		} else if (i == leaf->key_num-1 && strcmp(leaf->key[i], key) < 0  && leaf->next != 0) {
			leaf = (c_bpt_node *)leaf->next;
			i = -1;
		}

		i++;
	}

	return  NULL; 
}


void c_calculate_slotted_page(c_bpt_node* nodepointer, int* slotted_data_page) {
	if(!nodepointer->is_leaf) {
		c_calculate_slotted_page((c_bpt_node *)nodepointer->pointer[0], slotted_data_page);
	} else {

		vector<unsigned short int> pages;

		while(nodepointer != 0) {
			for (int i = 0; i < nodepointer->key_num; i++)
			{

				pages.insert(pages.end(), ((rid*)nodepointer->pointer[i+1])->page_id);
				sort( pages.begin(), pages.end() );
				pages.erase( unique( pages.begin(), pages.end() ), pages.end() );

				*slotted_data_page = pages.size();

			}

			nodepointer = (c_bpt_node *)nodepointer->next;
		}
	}


	return;
}