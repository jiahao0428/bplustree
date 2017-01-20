#include <iostream>
#include <string>
#include <cstdio>
#include <cstring>
#include "bpt.h"
#include <iostream>

using namespace std;

int main() {

	int key[23] = {1,2,3,0,5,6,7,8,9,10,31,27,44,37,41,22,13,24,11,55,51,84,64};
	initial_bpt("test relation");

	for(int i=0; i<sizeof(key)/sizeof(key[0]); i++) {
		entry *dummy= new entry;
		rid *test = new rid;

		test -> page_id = 90;
		test -> slot_id = 80;

		dummy -> key = key[i];
		dummy -> info = test;

		insert_into_tree(dummy);
	}


	int key_to_delete[11] = {3,9, 5,10, 27, 13, 2, 7,44,22,6};

	for(int i=0; i<sizeof(key_to_delete)/sizeof(key_to_delete[0]); i++) {
		cout<<"====== delete key: "<<key_to_delete[i] << " ======"<<endl;

		delete_from_tree(key_to_delete[i]);

		cout<<"leaf: "<<endl;
		print_leaf_ascending(root);
		//traverse(root);
	}

	initial_bpt("test world");

	for(int i=0; i<sizeof(key)/sizeof(key[0]); i++) {
		entry *dummy= new entry;
		rid *test = new rid;

		test -> page_id = 90;
		test -> slot_id = 80;

		dummy -> key = key[i];
		dummy -> info = test;

		insert_into_tree(dummy);
	}

	traverse(root);
	rid* test_query = query("test world", 44);

	if(test_query != 0)
		cout<<"slot id: "<<test_query->slot_id<<endl;
	else
		cout<<"Key Not Found"<<endl;

	scan("test world");

	cout<<relations.at(1)<<endl;
	bpt_node * bb = trees.at(0);
	bpt_node * cc = trees.at(1);
	cout<<"0:"<<bb -> key[0];
	cout<<"1:"<<cc -> key[0];

	return 0;
}