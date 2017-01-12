#ifndef __GP__BPT__
#define __GP__BPT__

#include <iostream>
#include <string>
#include <vector>

// Integer, 512 bytes page
const int M = 64 ;

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
} ;

extern int node_count ;
extern int splite_count ;
extern void initial_bpt() ;
extern bool insert_in_bpt( int key , void *value ) ;
extern bool delete_in_bpt( int key ) ;
extern char *query_in_bpt( int key ) ;


#endif
