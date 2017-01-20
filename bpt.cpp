#include <iostream>
#include <string>
#include <cstdio>
#include <cstring>
#include "bpt.h"
#include <iostream>
#include <algorithm>
#include <typeinfo>

using namespace std ;

vector<string> relations;
vector<bpt_node*> trees;

bpt_node *root ;
int node_count ;
int splite_count ;
int total_leaf_page;
int total_index_page;


bpt_node *new_bpt_node();
void initial_bpt(string relation);
bpt_node *find_leaf( bpt_node* nodepointer, int key ) ;
bpt_node *tree_search(bpt_node *nodepointer, int key);
void insert_into_tree(entry *child);
void insert(bpt_node *nodepointer, entry *entry);
void insert_in_node( bpt_node *node , entry *entry);
void split( bpt_node *node , entry *entry);
void delete_from_tree(int key);
void delete_entry(bpt_node *nodepointer, int key, entry *oldchildentry) ;
void delete_in_node( bpt_node *node , int key , entry *oldchildentry);
void redistribute(bpt_node *L, bpt_node *S) ;
void merge(bpt_node *L, bpt_node *S) ;
void replace_parent_entry(bpt_node *nodepointer, int key, bpt_node *pointer_to_replace);
void replace_key(bpt_node *nodepointer, int key, int key_to_replace);
entry* find_parent_entry(bpt_node *nodepointer, bpt_node *pointer_to_replace);
void print_leaf_ascending(bpt_node *nodepointer);
void print_leaf_descending(bpt_node *nodepointer);
void traverse(bpt_node *nodepointer);
void scan(string relation);
rid* query(std::string relation, int key);
rid* query_in_node(bpt_node* nodepointer, int key);
void range_query(std::string relation, int key1, int key2);
void display_page(std::string relation, unsigned short int page_id);
void file_statistics(std::string relation);



bpt_node *new_bpt_node()
{
	node_count ++ ;

	bpt_node *p = new bpt_node ;

	p -> is_leaf = false;
    p -> is_root = false;
    p -> key_num = 0;
    p -> pointer[0] = NULL;
    p -> previous = NULL;
    p -> next = NULL;
    p -> father = NULL;

    return p ;
}

entry *new_entry() 
{
	entry *child = new entry;

	child -> key = 0;
	child -> value = NULL;

	return child;
}


void initial_bpt(string relation)
{

	root = new_bpt_node();
    root -> is_root = true ;
    root -> is_leaf = true ;
    node_count = 0 ;
    splite_count = 0 ;

    relations.insert(relations.end(), relation);
    trees.insert(trees.end(), root);

    return;
}


bpt_node *find_leaf( bpt_node* nodepointer, int key ) 
{
	return tree_search(nodepointer, key);
}


bpt_node *tree_search(bpt_node *nodepointer, int key)
{
	if (nodepointer -> is_leaf) {
		return nodepointer;
	} else {
		if ( key < nodepointer -> key[0]) {
			return tree_search((bpt_node *)nodepointer -> pointer[0], key);
		} else {
			if ( key >= nodepointer -> key[nodepointer -> key_num - 1]) {
				return tree_search((bpt_node *)nodepointer -> pointer[nodepointer -> key_num], key);
			} else {
				for(int i=0; i<nodepointer -> key_num; i++) {
					if (nodepointer -> key[i] <= key && key < nodepointer -> key[i+1]) {
						return tree_search((bpt_node *)nodepointer->pointer[i], key);
					}
				}
			}
		}
	}
}

void insert_into_tree(entry *child) 
{
	insert(root, child);
	return;
}


void insert(bpt_node *nodepointer, entry *child)
{
	if (!nodepointer -> is_leaf) {
		for(int i=0; i<nodepointer->key_num; i++) {
			if (child -> key >= nodepointer -> key[nodepointer->key_num-1]) {
				insert((bpt_node *)nodepointer -> pointer[nodepointer->key_num], child);
			} else if(child -> key < nodepointer -> key[0]) {
				insert((bpt_node *)nodepointer -> pointer[0], child);
			} else if ((nodepointer -> key[i] <= child -> key && child -> key < nodepointer -> key[i+1])) {
				insert((bpt_node *)nodepointer -> pointer[i + 1], child);
			}

			if((nodepointer -> key[i] <= child -> key && child -> key < nodepointer -> key[i+1]) || child -> key < nodepointer -> key[0] || child -> key > nodepointer -> key[nodepointer->key_num-1]) {
				// usual case, didn't split child
				// Testing
				if(child->key == 0) {
					return;
				} else {
					if (nodepointer -> key_num  < M - 1) {
						insert_in_node((bpt_node *) nodepointer, child);

						if(((bpt_node *)child -> value) -> is_leaf) {
							int x = 0;
							while (nodepointer -> pointer[ x ] != child -> value) x ++ ;

							if (x < M)
								((bpt_node *)nodepointer -> pointer[ x ]) -> next = nodepointer -> pointer[ x + 1 ];

						}

						memset(child, 0, sizeof(child));
						return;
					} else {
						insert_in_node((bpt_node *) nodepointer, child);
						return;
					}
				}
			}
			
		}
	} else {

		if (nodepointer -> key_num < M-1) {

			insert_in_node((bpt_node *) nodepointer, child);
			memset(child, 0, sizeof(child));

			return;
		} else {
			insert_in_node((bpt_node *) nodepointer, child);
			return;
		}
	}
}


void insert_in_node( bpt_node *node , entry *child) 
{
	int x = 0 ;

    while ( x < node -> key_num && node -> key[ x ] < child -> key ) x ++ ;
    for ( int i = node -> key_num ; i > x ; i -- )
        node -> key[ i ] = node -> key[ i - 1 ] ;
    for ( int i = node -> key_num + 1 ; i > x + 1 ; i -- )
        node -> pointer[ i ] = node -> pointer[ i - 1 ] ;

    node -> key[ x ] = child -> key ;

    if(node->is_leaf && child -> info != 0) {
    	node -> pointer[ x + 1 ] = child -> info;
    	child -> info = NULL;
    }
    else
    	node -> pointer[ x + 1 ] = child -> value ;
    
    node -> key_num += 1 ;

    if ( node -> key_num == M ) { // need to split
        split( node, child ) ;
    }
    else {
    	memset(child, 0, sizeof(child));
    }

    return;
}


void split( bpt_node *node, entry *child)
{
	//traverse(root);
    splite_count ++ ;
    bpt_node *nodd = new_bpt_node() ;

    int mid_key = node -> key[ M / 2 ] ;
	nodd -> key_num = M - M / 2 ; 

	if(!node->is_leaf) {
    	nodd -> key_num = M - M / 2 - 1; 

    	for ( int i = 0 ; i < nodd -> key_num ; i ++ )
    	{
	        nodd -> key[ i ] = node -> key[ i + ( M / 2 ) + 1] ;
	        nodd -> pointer[ i ] = node -> pointer[ i + ( M / 2 ) + 1] ;

	        //empty it
	        node -> key[ i + ( M / 2 ) + 1] = 0;

	        // transfer father
	        ((bpt_node *)nodd -> pointer[ i ]) -> father = nodd;

    	}
	}
    else 
    {

    	for ( int i = 0 ; i < nodd -> key_num ; i ++ )
    	{
	        nodd -> key[ i ] = node -> key[ i + ( M / 2 ) ] ;
	        nodd -> pointer[ i + 1 ] = node -> pointer[ i + ( M / 2 ) + 1] ;


	        //empty it
	        node -> key[ i + ( M / 2 ) ] = 0;
			//node -> pointer[ i + ( M / 2 ) + 1] = NULL;
    	}
    }

    nodd -> pointer[ nodd -> key_num ] = node -> pointer[ M ] ; // pointer when key > km

    if(!node->is_leaf) {
    	((bpt_node *)nodd -> pointer[ nodd -> key_num ])->father = nodd;
    }
    
    node -> pointer[ M ] = NULL;
    node -> key_num = M / 2 ;

    if(node->is_leaf)
	    child -> key = nodd -> key[0];
	else
		child -> key = mid_key;

    child -> value = nodd ;

    if(node->is_root) {

		node -> is_root = false ;
		root = new_bpt_node() ;
		trees.back() = root;
		root -> is_root = true ;
		root -> key[ 0 ] = mid_key ;
		root -> pointer[ 0 ] = node ;
		root -> pointer[ 1 ] = nodd ;
		root -> key_num = 1 ;
		node -> father = nodd -> father = root ;

		root -> is_leaf = false;
		
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
        		((bpt_node *)node -> next) -> previous = nodd;

        	node -> next = nodd;
        	nodd -> previous = node;    	

    	}
	}

	//traverse(root);
	//cout<<root->key_num;
    return;
}

void delete_from_tree(int key) 
{	

	entry *dummy = new entry;

	delete_entry(root, key, dummy);

	return;
}

void delete_entry(bpt_node *nodepointer, int key, entry *oldchildentry) 
{
	if (!nodepointer -> is_leaf) {
		for(int i=0; i<nodepointer->key_num; i++) {
			if (key >= nodepointer -> key[nodepointer->key_num-1]) {
				delete_entry((bpt_node *)nodepointer -> pointer[nodepointer->key_num], key, oldchildentry);
			} else if(key < nodepointer -> key[0]) {
				delete_entry((bpt_node *)nodepointer -> pointer[0], key, oldchildentry);
			} else if (nodepointer -> key[i] <= key && key < nodepointer -> key[i+1]) {
				delete_entry((bpt_node *)nodepointer -> pointer[i + 1], key, oldchildentry);
			}

			/*cout<<"out"<<nodepointer -> key[i];
			cout<<"in"<<key;
			cout<<"num:"<<nodepointer -> key_num;
			cout<<"oldchild:"<<oldchildentry->key;*/
	
			
			if((nodepointer -> key[i] <= key && key < nodepointer -> key[i+1]) || key < nodepointer -> key[0] || key >= nodepointer -> key[nodepointer->key_num-1]) {

				// Testing
				if(oldchildentry->value == 0) {
					return;
				} else {
	
					delete_in_node((bpt_node *) nodepointer, key, oldchildentry);
					//cout<<"he";
					//traverse(root);


					if(nodepointer -> key_num >= M/2) {
						memset(oldchildentry, 0, sizeof(oldchildentry));
					} else {
						
						bpt_node * s = new bpt_node;
						//cout<<nodepointer->key_num;

						if(!nodepointer -> is_root) {

							bool is_left = false;
							bool need_to_merge = false;
							
							int count = ((bpt_node *)nodepointer->father) -> key_num;
							bpt_node * parent = (bpt_node *)nodepointer -> father;

							for(int j=0; j<count; j++) {
								if(parent -> pointer[j] == nodepointer && j > 0) {
									if(((bpt_node *) parent -> pointer[j+1]) -> key_num > M/2) {
										s = (bpt_node *)parent -> pointer[j+1];
									} else if(((bpt_node *)parent -> pointer[j-1]) -> key_num > M/2){
										s = (bpt_node *)parent -> pointer[j-1];
										is_left = true;
									} else {
										need_to_merge = true;

										if(((bpt_node *)parent -> pointer[j-1]) == 0) {
											s = (bpt_node *)parent -> pointer[j+1];
										} else {
											s = (bpt_node *)parent -> pointer[j-1];
											is_left = true;
										}

									}
									break;
								} else if (parent -> pointer[0] == nodepointer) {
									if(((bpt_node *)parent->pointer[1]) -> key_num > M/2) {
										s = (bpt_node *)parent -> pointer[1];
									} else {
										need_to_merge = true;
										s = (bpt_node *)parent -> pointer[1];
									}

									break;
								} else if(parent -> pointer[count] == nodepointer) {
									if(((bpt_node *)parent -> pointer[count - 1]) -> key_num > M/2) {
										s = (bpt_node *)parent -> pointer[count - 1];
										is_left = true;
									} else {
										need_to_merge = true;
										is_left = true;
										s = (bpt_node *)parent -> pointer[count - 1];
									}

									break;
								}
							}

							entry *m = new entry;

							if(!need_to_merge) {
								//cout<<"re";
								if(!is_left) {
									//printf("right\n");
									redistribute((bpt_node *) nodepointer, s);
								}
								else {
									//printf("left\n");
									printf("%d\n", nodepointer->key[0]);
									redistribute(s, (bpt_node *) nodepointer);


								}
								memset(oldchildentry, 0, sizeof(oldchildentry));
							} else {
								//cout<<"mer";

								bpt_node* pointer_to_replace = new bpt_node;
								pointer_to_replace = s;


								if(!is_left) {
									entry* m = find_parent_entry((bpt_node *)s -> father, s);
									oldchildentry -> key = m -> key;
									oldchildentry -> value = m -> value;
									insert_in_node(nodepointer, m);
									merge((bpt_node *) nodepointer, s);
								}
								else {
									//cout<<"left";
									//traverse(root);
									entry* m = find_parent_entry((bpt_node *)nodepointer -> father, nodepointer);
									oldchildentry -> key = m -> key;
									oldchildentry -> value = m -> value;
									insert_in_node(s, m);
									merge(s, (bpt_node *) nodepointer);
								}			
							}
						} else {
							// Not sure...
							if(nodepointer->key_num <= 1) {
								//printf("here?\n");
								//cout<<nodepointer->key_num;
								if(((bpt_node *)nodepointer->pointer[0]) -> key_num + ((bpt_node *)nodepointer -> pointer[1]) -> key_num + nodepointer -> key_num < M) {
									//merge with root
									// Problem with lost rid content
									if(((bpt_node *)nodepointer -> pointer[0]) -> key_num > 0) {
										root = (bpt_node *)nodepointer -> pointer[0];
										trees.back() = root;
									} else {
										root = (bpt_node *)nodepointer -> pointer[1];
										trees.back() = root;
									}

									root -> next = NULL;
									root -> previous = NULL;
									root -> is_root = true;
									root -> father = NULL;

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
		//cout<<((bpt_node *) nodepointer) -> key_num;

		//cout<<((bpt_node *) nodepointer) -> key[0];
		delete_in_node((bpt_node *) nodepointer, key, oldchildentry);
		//cout<<((bpt_node*) nodepointer -> previous)->key[0];

		if(nodepointer->is_root && nodepointer->is_leaf) {
			return;
		}

		if (nodepointer -> key_num >= M/2) {
			//cout<<"nothing happen?";
			memset(oldchildentry, 0, sizeof(oldchildentry));
			return;
		} else {

			//cout<<((bpt_node *) nodepointer) -> key_num;
			bpt_node *s = new bpt_node; 
			entry* n = new entry;
			//cout<< (bpt_node*) nodepointer -> next;
			//cout<< "previous"<<(bpt_node*) nodepointer -> previous;


			n = find_parent_entry((bpt_node *) nodepointer -> father, nodepointer);

			bool need_to_merge = false;
			bool is_left = false;

			//cout<< "count:" <<((bpt_node*) nodepointer -> previous)->key_num;
			//cout<< ((bpt_node*) nodepointer -> previous) -> key[1];
			//cout<< nodepointer->key[0];
			//cout<<"check";
			//print_leaf_descending(root);


			if(nodepointer -> next != 0 && ((bpt_node*) nodepointer -> next) -> key_num > M/2 && ((bpt_node*) nodepointer -> next) -> father == nodepointer -> father) {
				s = (bpt_node*) nodepointer -> next;
				//cout<<"here";
			} else if(nodepointer -> previous != 0 && ((bpt_node*) nodepointer -> previous) -> key_num > M/2 && ((bpt_node*) nodepointer -> previous) -> father == nodepointer -> father) {
				if(((bpt_node*) nodepointer -> previous) -> key_num > M/2) {
					s = (bpt_node*) nodepointer -> previous;
					is_left = true;
					//cout<<"there";
				}
			} else {

				need_to_merge = true;
				//cout<<((bpt_node*) nodepointer -> previous);
				if(nodepointer -> previous == 0 || ((bpt_node *)nodepointer->father) -> pointer[0] == nodepointer || ((bpt_node*) nodepointer -> previous) -> father != nodepointer -> father) {
					s = (bpt_node*) nodepointer -> next;
				} else {

					s = (bpt_node*) nodepointer -> previous;
					is_left = true;
				}
			}
			
			if(!need_to_merge) { 
				//cout<<"redistribute";

				if(!is_left) {
					redistribute((bpt_node *) nodepointer, s);
					//cout<<s -> key[0];
					//cout<<"here";
					// father problem
					//cout<<((bpt_node *)s->father)->key[0];
					replace_parent_entry((bpt_node *) s -> father, s -> key[0], s);
				} else {
					redistribute(s,(bpt_node *) nodepointer);
					//cout<<nodepointer->key[0];
					replace_parent_entry((bpt_node *) nodepointer -> father, nodepointer -> key[0], nodepointer);
				}
				// place most left key of s into parent
				//print_leaf_ascending(root);
				
				//cout<<"fuck";
				//cout<<((bpt_node *) s -> father) -> key[0];

				memset(oldchildentry, 0, sizeof(oldchildentry));
			} else {
				//cout<<"merge";

				entry* m = new entry;

				if(!is_left) {
					merge(nodepointer, s);
					m = find_parent_entry((bpt_node *) nodepointer -> father, s);
				} else {
					//cout<<"left";
					//problem
					merge(s, nodepointer);
					m = find_parent_entry((bpt_node *) nodepointer -> father, nodepointer);
				}

				//cout<<"after merge:"<<s->key[s -> key_num - 1];
				oldchildentry -> key = m -> key;
				//cout<<"old:"<<oldchildentry -> key;
				oldchildentry -> value = m -> value;
			}

			return;
		}
	}
}


void delete_in_node( bpt_node *node , int key , entry *oldchildentry)
{
    int x = 0 ;
    
    if(oldchildentry->key != 0) {
    	key = oldchildentry -> key;
    }

    while ( key != node -> key[ x ]  && x < node -> key_num) x ++ ;

    if(node -> key[x] == key) {
	    for ( int i = x ; i < node -> key_num - 1 ; i ++ ) {
	        node -> key[ i ] = node -> key[ i + 1 ] ;
	        node -> key[i + 1] = 0;
	    }
	    for ( int i = x + 1 ; i < node -> key_num ; i ++ ) {
	        node -> pointer[ i ] = node -> pointer[ i + 1 ] ;
	        node -> pointer[ i + 1 ] = NULL;
	    }
	    node -> key_num -- ;

	    // Testing propagate delete key to parent and replace it
	    if(node->is_leaf) {
			bpt_node * dummy = node;

			while(dummy->father != 0) {
				dummy = (bpt_node*) dummy->father;
				replace_key(dummy, node->key[x], key);
			}
		}

	} else {
		oldchildentry = NULL;
		return;
	}
}

void redistribute(bpt_node *L, bpt_node *S) 
{
	//l key num is d - 1
	for(int i=0; i<M; i++) {
		if(L->is_leaf) {
			
			// Moving from right
			if(L -> key_num < S -> key_num) { 
				L -> key[L -> key_num] = S -> key[i];
				L -> pointer[L->key_num + 1] = S -> pointer[i + 1];

				entry *dummy = new entry;
				delete_in_node(S, S->key[i], dummy);
				++L -> key_num;

				if(L -> key_num >= S -> key_num) {
					break;
				}
			} else {
				//Shift S key
				for(int j=S -> key_num;j>0;j--) {
 					S -> key[j] = S -> key[j-1];
 					S -> pointer[j+1] = S -> pointer[j]; 					
				}

				S -> key[i] = L -> key[L->key_num - 1 - i];
				S -> pointer[i + 1] = L -> pointer[L -> key_num - i];

				entry *dummy = new entry;
				delete_in_node(L, L->key[L->key_num -1 - i], dummy);
				++S -> key_num;

				if(S -> key_num >= L -> key_num) {
					break;
				}

			}
		} else {

			if(L -> key_num < S -> key_num) { 

				entry* dummy = new entry;
				dummy = find_parent_entry((bpt_node*)S->father, S);

				L -> key[L -> key_num] = dummy -> key;
				L -> pointer[L -> key_num + 1] = S -> pointer[i];

				// move child father
				((bpt_node *)L -> pointer[L -> key_num + 1])->father = L;

				for(int j=0;j<((bpt_node*)S->father)->key_num; j++) {
					if(((bpt_node*)S->father) -> key[j] = dummy -> key) {
						((bpt_node*)S->father) -> key[j] = S -> key[i];
						break;
					}
				}


				for(int k=0; k<S->key_num; k ++) {
					if(k < S->key_num - 1) {
						S -> key[k] = S -> key[k+1];
						S -> key[k + 1] = 0;
					}

					S->pointer[k] = S->pointer[k+1];
					S -> pointer[k + 1] = NULL;
				}

				--S -> key_num;
				++L -> key_num;

				if(L -> key_num >= M/2) {
					break;
				}
			} else {
				entry* dummy = new entry;
				dummy = find_parent_entry((bpt_node*)S->father, S);


				cout<< ((bpt_node*)L->father)->key[0];
				cout<<L->key[0];
				for(int j=0;j<((bpt_node*)L->father)->key_num; j++) {
					if(((bpt_node*)L->father) -> key[j] = dummy -> key) {
						((bpt_node*)L->father) -> key[j] = L -> key[L -> key_num -1 - i];
						break;
					}
				}

				for(int k=0; k<S->key_num; k ++) {
					
					if(k == 0) {
						S -> pointer[S -> key_num + 1] = S -> pointer[S -> key_num];
					}

					S -> key[S -> key_num - k] = S -> key[k];
					S -> pointer[S -> key_num - k] = S -> pointer[k];
	
				}

				S -> key[0] = dummy -> key;
				S -> pointer[0] = L -> pointer[L -> key_num - i];

				L -> pointer[L -> key_num - i] = NULL; 
				L -> key[L -> key_num -1 - i] = 0;

				// move child father
				((bpt_node *)S -> pointer[0]) -> father = S;

				++S -> key_num;
				--L -> key_num;

				if(S -> key_num >= M/2) {
					break;
				}

			}
		}
	}

	return;
}

void merge(bpt_node *L, bpt_node *S) 
{
	for(int i=0; i<M; i++) {
		if(!L-> is_leaf) {
			L -> key[L -> key_num] = S -> key[i];
			L -> pointer[L -> key_num] = S -> pointer[i];
		} else {
			L -> key[L -> key_num] = S -> key[i];
			L -> pointer[L -> key_num + 1] = S -> pointer[i + 1];
		}

		//cout<<((bpt_node *)L -> pointer[L -> key_num]);

		if(((bpt_node *)L -> pointer[L -> key_num]) != 0 && !L->is_leaf)
			((bpt_node *)L -> pointer[L -> key_num]) -> father = L;

		++L -> key_num;
		--S -> key_num;

		if(S -> key_num == 0) {
			if(L-> is_leaf && S->next != 0) {
				((bpt_node *)(S->next)) -> previous = L;
				L -> next = S -> next;
			} else {
				L -> pointer[L -> key_num] = S -> pointer[i + 1];
				((bpt_node *)L -> pointer[L -> key_num]) -> father = L;
			}

			delete S;

			return;
		}
	}
}

void replace_parent_entry(bpt_node *nodepointer, int key, bpt_node *pointer_to_replace) 
{

	for(int i=0; i<nodepointer->key_num; i++) {
		// Not sure having other conditions....
		if (nodepointer -> pointer[i + 1] == pointer_to_replace) {
			nodepointer -> key[i] = key;
			return;
		}
	}
}

void replace_key(bpt_node *nodepointer, int key, int key_to_replace) 
{

	for(int i=0; i<nodepointer->key_num; i++) {
		if (nodepointer -> key[i] == key_to_replace) {
			nodepointer -> key[i] = key;
			return;
		}
	}
}
		
entry* find_parent_entry(bpt_node *nodepointer, bpt_node *pointer_to_replace) 
{

	for(int i=0; i<nodepointer->key_num; i++) {
		if (nodepointer -> pointer[i + 1] == pointer_to_replace) {
			entry* M = new entry; 
			M -> key = nodepointer -> key[i];
			M -> value = nodepointer -> pointer[i + 1];

			return M;
		}
	}
}


void traverse(bpt_node *nodepointer) 
{

	for(int i=0; i<nodepointer -> key_num; i++) {
		if(i == 0 && nodepointer->is_leaf) {
			total_leaf_page++;
			printf("leaf: ");
		}

		printf("%d ", nodepointer->key[i]);
	}

	if (!nodepointer -> is_root) {
		printf("\nfather 1st key: %d\n", ((bpt_node *)nodepointer->father)->key[0]);
	}
	else {
		printf("\n");
	}

	if(!nodepointer -> is_leaf) {
		
		total_index_page ++;

		for(int i=0; i<=nodepointer -> key_num; i++) {
			traverse((bpt_node *)nodepointer -> pointer[i]);
			continue;
		}
	}

	return;
}

void print_leaf_ascending(bpt_node *nodepointer) 
{
	if(!nodepointer->is_leaf) {
		print_leaf_ascending((bpt_node *)nodepointer->pointer[0]);
	} else {
		while(nodepointer != 0) {
			for (int i = 0; i < nodepointer->key_num; i++)
			{
				printf("key: %d ,", nodepointer->key[i]);
				//cout<< ((rid*)nodepointer->pointer[i+1]);
				printf("slot id: %u ,", ((rid*)nodepointer->pointer[i+1])->slot_id);
				printf("page id: %u ;", ((rid*)nodepointer->pointer[i+1])->page_id);
			}

			printf("\nkey num: %d\n", nodepointer->key_num);
			nodepointer = (bpt_node *)nodepointer->next;
		}
	}
}

void print_leaf_descending(bpt_node *nodepointer) 
{
	if(!nodepointer->is_leaf) {
		print_leaf_descending((bpt_node *)nodepointer->pointer[nodepointer -> key_num]);
	} else {
		while(nodepointer != 0) {
			for (int i = 0; i < nodepointer->key_num; i++)
			{
				printf("%d ", nodepointer->key[i]);
			}

			printf("\nkey num: %d\n", nodepointer->key_num);
			nodepointer = (bpt_node *)nodepointer->previous;
		}
	}
}

void scan(string relation) {
	ptrdiff_t pos = find(relations.begin(), relations.end(), relation) - relations.begin();

	if(pos >= relations.size()) {
    	printf("Relation Not Found\n");
	} else {
		//cout<<trees.at(pos) -> key[0];
		total_index_page = 0;
		total_leaf_page = 0;

		traverse(trees.at(pos));

		//char test[11] = "asdasdasdd";

		//i, A11_c
		/*cout << typeid(pos).name() << endl;

		if(typeid(test).name() == TYPE_CHAR) {
			cout<<"true";
		} else {
			cout<<"false";
		}*/


		cout<<"Total Leaf Page: "<<total_leaf_page<<endl;
		cout<<"Total Index Page: "<<total_index_page<<endl;
	}

	return;
}

rid* query(string relation, int key) {
	ptrdiff_t pos = find(relations.begin(), relations.end(), relation) - relations.begin();

	if(pos >= relations.size()) {
    	printf("Relation Not Found\n");
    	return NULL;
	} else {
		//cout<<trees.at(pos) -> key[0];
		return query_in_node(trees.at(pos), key);
	}
}

rid* query_in_node(bpt_node* nodepointer, int key) {
	
	bpt_node* leaf = find_leaf(nodepointer, key);
	int i = 0;

	while(true) {
		if(leaf->key[i] == key) {
			return ((rid*)leaf->pointer[i+1]);
			break;
		} else if(leaf->key[i] > key) {
			break;
		} else if (i == leaf->key_num-1 && leaf->key[i] < key  && leaf->next != 0) {
			leaf = (bpt_node *)leaf->next;
			i = -1;
		}

		i++;
	}

	return  NULL; 
}

void range_query(string relation, int key1, int key2) {

}

void display_page(string relation, unsigned short int page_id) {

}


void file_statistics(string relation) {
	ptrdiff_t pos = find(relations.begin(), relations.end(), relation) - relations.begin();

	if(pos >= relations.size()) {
    	printf("Relation Not Found\n");
	} else {
		//cout<<trees.at(pos) -> key[0];
		total_index_page = 0;
		total_leaf_page = 0;

		traverse(trees.at(pos));
		cout<<"Total Index Page: "<<total_index_page<<endl;

	}

	return;
}