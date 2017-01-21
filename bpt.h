#ifndef __GP__BPT__
#define __GP__BPT__

#include <iostream>
#include <string>
#include <vector>

using namespace std;

// Integer, M: 512 bytes page ==> 63 +1 for overflow
// Character, N: 37 ==> 36 +1 for overflow

const int M = 64;
const int N = 37;


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
    int data_type;
    void *pointer[ M + 1 ] ;

    // For leaf node
    void *previous ;
    void *next ;

    void *father ;
} ;


struct c_entry
{
    char key[11];
    void *value;
    rid *info;
} ;


struct c_bpt_node
{
    bool is_leaf ;
    bool is_root ;
    int key_num ;
    char key[ N ] [11];
    void *pointer[ N + 1 ] ;

    // For leaf node
    void *previous ;
    void *next ;

    void *father ;
} ;


extern bpt_node *root ;
extern c_bpt_node *c_root ;

extern std::vector<std::string> relations;
extern std::vector<bpt_node*> trees;

extern std::vector<std::string> c_relations;
extern std::vector<c_bpt_node*> c_trees;

extern int total_leaf_page;
extern int total_index_page;
extern int c_total_leaf_page;
extern int c_total_index_page;

// Unified interface

extern void scan(std::string relation, int* leaf_page, int * index_page);
extern vector<unsigned short int> display_page(string relation, unsigned short int page_id);
extern void file_statistics(std::string relation, int* index_page, int* slotted_data_page);
extern void print_leaf_ascending(string relation);
extern void print_leaf_descending(string relation);
extern void traverse(string relation);

// Integer

extern void initial_i_bpt(std::string relation) ;
extern void i_insert_into_tree(std::string relation, entry *child);
extern void i_delete_from_tree(std::string relation, int key);
extern rid* i_query(std::string relation, int key);
extern std::vector<rid*> i_range_query(std::string relation, int key1, int key2);

extern void i_traverse(bpt_node *nodepointer, bool print);
extern void i_print_leaf_ascending(bpt_node *nodepointer);
extern void i_print_leaf_descending(bpt_node *nodepointer);
extern void i_find_slotted_page(bpt_node *nodepointer, vector<unsigned short int>* slot_ids, unsigned short int page_id); 
extern void i_calculate_slotted_page(bpt_node* nodepointer, int* slotted_data_page);

// Character

extern void initial_c_bpt(std::string relation) ;
extern void c_insert_into_tree(std::string relation, c_entry *child);
extern void c_delete_from_tree(std::string relation, char* key);
extern rid* c_query(std::string relation, char* key);
extern std::vector<rid*> c_range_query(std::string relation, char* key1, char* key2);

extern void c_traverse(c_bpt_node *nodepointer, bool print);
extern void c_print_leaf_ascending(c_bpt_node *nodepointer);
extern void c_print_leaf_descending(c_bpt_node *nodepointer);
void c_find_slotted_page(c_bpt_node *nodepointer, vector<unsigned short int>* slot_ids, unsigned short int page_id);
extern void c_calculate_slotted_page(c_bpt_node* nodepointer, int* slotted_data_page);


#endif
