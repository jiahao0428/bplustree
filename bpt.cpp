#include <iostream>
#include <string>
#include <cstdio>
#include <cstring>
#include "bpt.h"
#include <iostream>

using namespace std ;

bpt_node *root ;
int node_count ;
int splite_count ;

static const struct bpt_node* EmptyStruct;

bpt_node *new_bpt_node();
void initial_bpt();
bpt_node *find_leaf( int key ) ;
bpt_node *tree_search(bpt_node *nodepointer, int key);
void insert(bpt_node *nodepointer, entry *entry);
void insert_in_node( bpt_node *node , entry *entry);
void split( bpt_node *node , entry *entry);
void delete_entry(bpt_node *nodepointer, int key, entry *oldchildentry) ;
void delete_in_node( bpt_node *node , int key , entry *oldchildentry);
void redistribute(bpt_node *L, bpt_node *S) ;
void merge(bpt_node *L, bpt_node *S) ;
void replace_parent_entry(bpt_node *nodepointer, int key, bpt_node *pointer_to_replace);
entry* find_parent_entry(bpt_node *nodepointer, bpt_node *pointer_to_replace);
void traverse(bpt_node *nodepointer);



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

    return p ;
}

entry *new_entry() {
	entry *child = new entry;

	child -> key = 0;
	child -> value = NULL;

	return child;
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
				if(child->key == 0) {
					return;
				} else {
					if (nodepointer -> key_num  < M - 1) {
						insert_in_node((bpt_node *) nodepointer, child);
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
		//cout<<"leaf";
		if (nodepointer -> key_num < M-1) {
			//cout<< "before:" <<child;
			insert_in_node((bpt_node *) nodepointer, child);
			memset(child, 0, sizeof(child));
			//cout<< "after:" <<child;
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
    node -> pointer[ x + 1 ] = child -> value ;
    node -> key_num += 1 ;

    if ( node -> key_num == M ) // need to split
        split( node, child ) ;
    else
    	memset(child, 0, sizeof(child));

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
    else {
    	nodd -> key_num = M - M / 2;

    	for ( int i = 0 ; i < nodd -> key_num ; i ++ )
    	{
	        nodd -> key[ i ] = node -> key[ i + ( M / 2 ) ] ;
	        nodd -> pointer[ i ] = node -> pointer[ i + ( M / 2 ) ] ;


	        //empty it
	        node -> key[ i + ( M / 2 ) ] = 0;
	        if(node->is_leaf)
	        	node -> pointer[ i + ( M / 2 ) ] = NULL;
    	}
    }

    nodd -> pointer[ nodd -> key_num ] = node -> pointer[ M ] ; // pointer when key > km
    node -> pointer[ M ] = NULL;
    node -> key_num = M / 2 ;

    if(node->is_leaf)
	    child -> key = nodd -> key[0];
	else
		child -> key = mid_key;

    child -> value = nodd ;

    if(node->is_root) {

		node -> is_root = false ;
		//node -> is_leaf = false;
		root = new_bpt_node() ;
		root -> is_root = true ;
		root -> key[ 0 ] = mid_key ;
		root -> pointer[ 0 ] = node ;
		root -> pointer[ 1 ] = nodd ;
		root -> key_num = 1 ;
		node -> father = nodd -> father = root ;

		// is leaf ??
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
        	node -> next = nodd;
        	nodd -> previous = node;
    	}
	}

	//traverse(root);
	//cout<<root->key_num;
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

				if(oldchildentry->key == 0) {
					return;
				} else {
	
					delete_in_node((bpt_node *) nodepointer, key, oldchildentry);
					cout<<"he";
					traverse(root);


					if(nodepointer -> key_num >= M/2) {
						memset(oldchildentry, 0, sizeof(oldchildentry));
					} else {
						
						bpt_node * s = new bpt_node;
						cout<<nodepointer->key_num;
						
						if(!nodepointer->is_root) {

							bool is_left = false;
							bool need_to_merge = false;


							//Weird
							/*for(int j=0; j<((bpt_node *)nodepointer->father) -> key_num; j++) {
								if(((bpt_node *) nodepointer -> father) -> pointer[((bpt_node *)nodepointer->father) -> key_num] == nodepointer) { //most right
									s = (bpt_node *)((bpt_node *)nodepointer->father)->pointer[((bpt_node *)nodepointer->father) -> key_num - 1]; //left sibling
									is_right = true;
									break;
								} else if(((bpt_node *) nodepointer -> father) -> pointer[j] == nodepointer) {
									s = (bpt_node *)((bpt_node *)nodepointer->father)->pointer[j+1];
									break;
								}
							}*/
							
							int count = ((bpt_node *)nodepointer->father) -> key_num;
							bpt_node * parent = (bpt_node *)nodepointer->father;

							for(int j=0; j<count; j++) {
								if(parent -> pointer[j] == nodepointer && j > 0) {
									if(((bpt_node *) parent->pointer[j+1]) -> key_num > M/2) {
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
								cout<<"re";
								if(!is_left) {
									redistribute((bpt_node *) nodepointer, s);
								}
								else {
									redistribute(s, (bpt_node *) nodepointer);
								}
								memset(oldchildentry, 0, sizeof(oldchildentry));
							} else {
								cout<<"mer";

								bpt_node* pointer_to_replace = new bpt_node;
								pointer_to_replace = s;


								if(!is_left) {
									entry* m = find_parent_entry((bpt_node *)s->father, s);
									oldchildentry -> key = m -> key;
									oldchildentry -> value = m -> value;
									insert_in_node(nodepointer, m);
									merge((bpt_node *) nodepointer, s);
								}
								else {
									cout<<"left";
									traverse(root);
									entry* m = find_parent_entry((bpt_node *)nodepointer->father, nodepointer);
									oldchildentry -> key = m -> key;
									oldchildentry -> value = m -> value;
									insert_in_node(s, m);
									merge(s, (bpt_node *) nodepointer);
								}			
							}
						} else {
							// Not sure...
							cout<<"nabeh";
							if(nodepointer->key_num <= 1) {
								if(((bpt_node *)nodepointer->pointer[0])->key_num + ((bpt_node *)nodepointer->pointer[1])->key_num + nodepointer->key_num < M-1) {
									//merge with root

									if(((bpt_node *)nodepointer->pointer[0])-> key_num > 0) {
										root = (bpt_node *)nodepointer->pointer[0];
									} else {
										root = (bpt_node *)nodepointer->pointer[1];
									}

									root -> is_root = true;
									root -> is_leaf = true;
								}
							}
						}
					}

					return;
				}
			}
		}
	} else {
		cout<<"leaf";
		cout<<((bpt_node *) nodepointer)->key[0];
		delete_in_node((bpt_node *) nodepointer, key, oldchildentry);


		if (nodepointer -> key_num >= M/2) {
			cout<<"nothing happen?";
			memset(oldchildentry, 0, sizeof(oldchildentry));
			return;
		} else {
			bpt_node *s = new bpt_node; 
			entry* n = new entry;
			cout<< (bpt_node*) nodepointer -> next;
			cout<< "previous"<<(bpt_node*) nodepointer -> previous;
			n = find_parent_entry((bpt_node *) nodepointer -> father, nodepointer);

			bool need_to_merge = false;
			bool is_left = false;

			// Decide which sibling should be taken
			/*if(n->key == key || nodepointer -> next == 0) {
				s = (bpt_node*) nodepointer -> previous;
			} else {
				s = (bpt_node*) nodepointer -> next;
			}*/

			if(((bpt_node*) nodepointer -> next)->key_num > M/2) {
				s = (bpt_node*) nodepointer -> next;
			} else if(((bpt_node*) nodepointer -> previous)->key_num > M/2){
				s = (bpt_node*) nodepointer -> previous;
				is_left = true;
			} else {
				need_to_merge = true;

				if(nodepointer -> previous == 0) {
					s = (bpt_node*) nodepointer -> next;
				} else {
					s = (bpt_node*) nodepointer -> previous;
					is_left = true;
				}
			}

			//if(key == 9)
			//	return;

			if(!need_to_merge) { 
				cout<<"redistribute";

				if(!is_left) {
					redistribute((bpt_node *) nodepointer, s);
				} else {
					redistribute(s,(bpt_node *) nodepointer);
				}
				// place most left key of s into parent
				

				replace_parent_entry((bpt_node *) nodepointer -> father, nodepointer -> key[0], nodepointer);
				replace_parent_entry((bpt_node *) nodepointer -> father, s -> key[0], s);
				cout<<"fuck";
				cout<<((bpt_node *) s -> father) -> key[0];

				memset(oldchildentry, 0, sizeof(oldchildentry));
			} else {
				cout<<"merge";

				entry* m = new entry;

				if(!is_left) {
					merge(nodepointer, s);
					m = find_parent_entry((bpt_node *) nodepointer -> father, s);
				} else {
					merge(s, nodepointer);
					m = find_parent_entry((bpt_node *) nodepointer -> father, nodepointer);
				}
				cout<<"after merge:"<<nodepointer->key[0];

				
				oldchildentry -> key = m -> key;
				cout<<"old:"<<oldchildentry->key;
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
    //cout<<"previous"<<node->previous;
    //cout<<"next"<<((bpt_node *)node->next)->key[0];
    //cout<<"key: "<<key;

    while ( key != node -> key[ x ] ) x ++ ;
    for ( int i = x ; i < node -> key_num - 1 ; i ++ ) {
        node -> key[ i ] = node -> key[ i + 1 ] ;
        node -> key[i + 1] = 0;
    }
    for ( int i = x + 1 ; i < node -> key_num ; i ++ ) {
        node -> pointer[ i ] = node -> pointer[ i + 1 ] ;
        node -> pointer[ i + 1 ] = NULL;
    }
    node -> key_num -- ;

    //traverse(root);
}

void redistribute(bpt_node *L, bpt_node *S) 
{
	//l key num is d - 1
	for(int i=0; i<M; i++) {
		if(L->is_leaf) {
			L -> key[L->key_num] = S -> key[i];
			L -> pointer[L->key_num] = S -> pointer[i];

			entry *test = new entry;
			delete_in_node(S, S->key[i], test);
			++L -> key_num;

			if(L -> key_num >= S -> key_num) {
				break;
			}
		} else {
			entry* test = new entry;
			test = find_parent_entry((bpt_node*)S->father, S);

			L -> key[L -> key_num] = test -> key;
			L -> pointer[L -> key_num + 1] = S -> pointer[i];

			for(int j=0;j<((bpt_node*)S->father)->key_num; j++) {
				if(((bpt_node*)S->father) -> key[j] = test -> key) {
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
		}
	}

	return;
}

void merge(bpt_node *L, bpt_node *S) 
{
	for(int i=0; i<M; i++) {
		L -> key[L -> key_num] = S -> key[i];
		L -> pointer[L -> key_num] = S -> pointer[i];

		++L -> key_num;
		--S -> key_num;

		if(S -> key_num == 0) {
			if(L-> is_leaf) {
				L -> next = S -> next;
			} else {
				L -> pointer[L -> key_num] = S -> pointer[i + 1];
			}

			delete S;

			return;
		}
	}
}

void replace_parent_entry(bpt_node *nodepointer, int key, bpt_node *pointer_to_replace) {

	for(int i=0; i<nodepointer->key_num; i++) {
		// Not sure having other conditions....
		if (nodepointer -> pointer[i + 1] == pointer_to_replace) {
			nodepointer -> key[i] = key;
			return;
		}
	}
}
		
entry* find_parent_entry(bpt_node *nodepointer, bpt_node *pointer_to_replace) {

	for(int i=0; i<nodepointer->key_num; i++) {
		if (nodepointer -> pointer[i + 1] == pointer_to_replace) {
			entry* M = new entry; 
			M -> key = nodepointer -> key[i];
			M -> value = nodepointer -> pointer[i + 1];

			return M;
		}
	}
}


void traverse(bpt_node *nodepointer) {

	for(int i=0; i<nodepointer -> key_num; i++) {
		if(i == 0 && nodepointer->is_leaf)
			printf("leaf: ");

		printf("%d ", nodepointer->key[i]);
	}

	printf("\n");

	if(!nodepointer -> is_leaf) {
		for(int i=0; i<=nodepointer -> key_num; i++) {
			traverse((bpt_node *)nodepointer -> pointer[i]);
			continue;
		}
	}

	return;
}

int main() {
	//int key[13] = {3,5,6,8,9,22,44,13,27,31,24,37,41};
	//int key[8] = {3,4,1,5,2,9,6,8};
	int key[18] = {1,2,3,4,5,6,7,8,9,10,31,27,44,37,41,22,13,24};
	initial_bpt();

	for(int i=0; i<sizeof(key)/sizeof(key[0]); i++) {
		entry *test= new entry;
		test -> key = key[i];
		//cout<<test;
		insert(root, test);
	}

	traverse(root);
	cout << "split count: "<< splite_count << endl;

	//int key_to_delete[2] = {27,37};
	int key_to_delete[2] = {3,9};

	for(int i=0; i<sizeof(key_to_delete)/sizeof(key_to_delete[0]); i++) {
		cout<<"delete key: "<<key_to_delete[i] << endl;

		entry *test = new entry;
		delete_entry(root, key_to_delete[i], test);
		traverse(root);
	}

	return 0;
}