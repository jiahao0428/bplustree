#ifndef __GP__BPT__
#define __GP__BPT__

#include <iostream>
#include <string>
#include <vector>

// Integer, M: 512 bytes page ==> 63 +1 for overflow
// Character,N: 37 ==> 36 +1 for overflow
// const int M = 64 ;
const int M = 64 ;
const int N = 37;


#define TYPE_INT = "i";
#define TYPE_CHAR = "A11_c";

struct rid 
{
	unsigned short int page_id ;
	unsigned short int slot_id ;
} ;

struct entry
{
	int key;
	void *value;
	rid *info;
} ;

struct bpt_node
{
    bool is_leaf ;
    bool is_root ;
    int key_num ;
    int key[ M ] ;
    bool data_type;
    void *pointer[ M + 1 ] ;

    // For leaf node
    void *previous ;
    void *next ;

    void *father ;
} ;

//extern int node_count ;
//extern int splite_count ;
extern bpt_node *root ;
//extern std::vector<std::string> relations;
//extern std::vector<bpt_node*> trees;
extern const bool BPT_TYPE_INT;
extern const bool BPT_TYPE_CHAR;

extern void initial_bpt(std::string relation, bool data_type) ;
extern void insert_into_tree(entry *child);
extern void traverse(bpt_node *nodepointer);
extern void delete_from_tree(int key);
extern void print_leaf_ascending(bpt_node *nodepointer);
extern void print_leaf_descending(bpt_node *nodepointer);

extern void scan(std::string relation);
extern rid* query(std::string relation, int key);
extern void range_query(std::string relation, int key1, int key2);
extern void display_page(std::string relation, unsigned short int page_id);
extern void file_statistics(std::string relation);


#endif
