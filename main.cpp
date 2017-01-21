#include <iostream>
#include <string>
#include <cstdio>
#include <cstring>
#include "bpt.h"

using namespace std;

int main() {

	int key[23] = {1,2,3,0,5,6,7,8,9,10,31,27,44,37,41,22,13,24,11,55,51,84,64};
	initial_i_bpt("test relation1");

	for(int i=0; i<sizeof(key)/sizeof(key[0]); i++) {
		entry *dummy= new entry;
		rid *test = new rid;

		test -> page_id = 90;
		test -> slot_id = 80;

		dummy -> key = key[i];
		dummy -> info = test;

		i_insert_into_tree("test relation1", dummy);
	}

	traverse("test relation1");

	char s_key[9][11] = {"hahawidkfj", "jnhgtvbgff","jjjuhgrfff","oookjhygtf","ikjnbgtfvc","ujhqqazxsw","prfvbjughf","uhyedscvfo","uhyedscvfs"};

	initial_c_bpt("test relation");

	for(int i=0; i<sizeof(s_key)/sizeof(s_key[0]); i++) {

		c_entry *dummy= new c_entry;
		rid *test = new rid;

		test -> page_id = 90;
		test -> slot_id = 80;

		strcpy(dummy -> key, s_key[i]);
		dummy -> info = test;

		c_insert_into_tree("test relation", dummy);
	}

	traverse("test relation");

	char test[11] = "hahawidkfj";
	c_delete_from_tree("test relation", test);

	traverse("test relation");

	//c_print_leaf_descending(c_root);

	int key_to_delete[11] = {3,9, 5,10, 27, 13, 2, 7,44,22,6};

	for(int i=0; i<sizeof(key_to_delete)/sizeof(key_to_delete[0]); i++) {
		cout<<"====== delete key: "<<key_to_delete[i] << " ======"<<endl;

		i_delete_from_tree("test relation1", key_to_delete[i]);

		cout<<"leaf: "<<endl;
		print_leaf_ascending("test relation1");
		//traverse(root);
	}

	initial_i_bpt("test world");

	for(int i=0; i<sizeof(key)/sizeof(key[0]); i++) {
		entry *dummy= new entry;
		rid *test = new rid;

		test -> page_id = 90;
		test -> slot_id = 80;

		dummy -> key = key[i];
		dummy -> info = test;

		i_insert_into_tree("test world", dummy);
	}

	traverse("test world");
	rid* test_query = i_query("test world", 44);

	vector<rid*> haha = range_query("test world", 1, 10);
	cout<<"range query sample: "<<haha.at(0)->slot_id;

	if(test_query != 0)
		cout<<"slot id: "<<test_query->slot_id<<endl;
	else
		cout<<"Key Not Found"<<endl;

	int lpage = 0;
	int ipage = 0;
	scan("test world", &lpage, &ipage);

	cout<< "total leaf page: "<<lpage<<endl;
	cout<< "total index page: "<<ipage<<endl;

	lpage = 0;
	int slotted_data_page = 0;
	file_statistics( "test relation1", &lpage, &slotted_data_page);

	cout<< "total index page in test relation1: "<<lpage<<endl;
	cout<< "total slotted page in test relation1: "<<slotted_data_page<<endl;

	//root->pointer[0] = ((bpt_node*)root->pointer[0]);
	//cout<<root->data_type;

	return 0;
}