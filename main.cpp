#include <iostream>
#include <cstdio>
#include <cstring>
#include "bpt.h"

using namespace std;

int main() {
	int key[] = {3,5,6,8,9,22,44};
	initial_bpt();


	for(int i=0; i<sizeof(key); i++) {
		entry * entry;
		cout << sizeof(key);
		//entry -> key = key[i];

		insert(root, entry);
	}

	traverse();

	return 0;
}