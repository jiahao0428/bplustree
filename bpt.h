#ifndef __GP__BPT__
#define __GP__BPT__

#include <iostream>
#include <string>
#include <vector>

// Integer, 512 bytes page, +1 for overflow
//const int M = 64 ;
const int M = 5 ;
struct rid 
{
	unsigned short int page_id ;
	unsigned short int slot_id ;
} ;

struct entry
{
	int key;
	void *value;
} ;

struct bpt_node
{
    bool is_leaf ;
    bool is_root ;
    int key_num ;
    int key[ M ] ;
    void *pointer[ M + 1 ] ;

    // For leaf node
    void *previous ;
    void *next ;

    void *father ;
} ;

extern int node_count ;
extern int splite_count ;
extern void initial_bpt() ;
extern void insert(bpt_node *nodepointer, entry *entry);
extern void traverse(bpt_node *nodepointer);
extern void delete_entry(bpt_node *nodepointer, int key, entry *oldchildentry) ;


#endif
