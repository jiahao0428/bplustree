#ifndef __GP__BPT__
#define __GP__BPT__

#include <iostream>
#include <string>
#include <vector>

// Integer, M: 512 bytes page ==> 63 +1 for overflow
// Character,N: 37 ==> 36 +1 for overflow
// const int M = 64 ;
const int M = 5;
const int N = 9;

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
    void *pointer[ M + 1 ] ;

    // For leaf node
    void *previous ;
    void *next ;

    void *father ;
} ;


extern bpt_node *root ;
extern c_bpt_node *c_root ;
extern const int BPT_TYPE_INT;
extern const int BPT_TYPE_CHAR;

extern void initial_bpt(std::string relation, int data_type) ;
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

// Character

extern void initial_c_bpt(std::string relation) ;
extern void c_insert_into_tree(c_entry *child);
extern void c_traverse(c_bpt_node *nodepointer);
extern void c_delete_from_tree(char key);
extern void c_print_leaf_ascending(c_bpt_node *nodepointer);
extern void c_print_leaf_descending(c_bpt_node *nodepointer);

extern void c_scan(std::string relation);
extern rid* c_query(std::string relation, char key);
extern void c_display_page(std::string relation, unsigned short int page_id);
extern void c_file_statistics(std::string relation);


#endif
