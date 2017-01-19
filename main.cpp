#include <iostream>
#include <string>
#include <cstdio>
#include <cstring>
#include "bpt.h"
#include <iostream>

using namespace std;

int main() {
	//int key[13] = {3,5,6,8,9,22,44,13,27,31,24,37,41};
	int key[7] = {2,6,8,9,22,44,13};
	//int key[23] = {1,2,3,4,5,6,7,8,9,10,31,27,44,37,41,22,13,24,11,55,51,84,64};
	initial_bpt();

	for(int i=0; i<sizeof(key)/sizeof(key[0]); i++) {
		entry *dummy= new entry;
		rid *test = new rid;

		test->page_id = 90;
		test->slot_id = 80;

		dummy -> key = key[i];
		dummy -> info = test;

		insert(root, dummy);
		print_leaf_ascending(root);
	}

	traverse(root);
	cout << "split count: "<< splite_count << endl;
	//print_leaf_ascending(root);
	//int key_to_delete[5] = {27,1, 9, 44, 5};
	int key_to_delete[11] = {3,9, 5,10, 27, 13, 2, 7,44,22,6};

	for(int i=0; i<sizeof(key_to_delete)/sizeof(key_to_delete[0]); i++) {
		cout<<"======delete key: "<<key_to_delete[i] << " ======"<<endl;

		entry *dummy = new entry;
		delete_entry(root, key_to_delete[i], dummy);
		cout<<"leaf: "<<endl;
		print_leaf_ascending(root);
		traverse(root);
	}

	//cout<<"leaf: "<<endl;
	//print_leaf_descending(root);
	traverse(root);

	return 0;
}