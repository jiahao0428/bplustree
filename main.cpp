#include <iostream>
#include <string>
#include <cstdio>
#include <cstring>
#include "bpt.h"

using namespace std;

int main() {

	
	// Insert Integer & Traverse Example

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




	// Insert Char & Traverse Example


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




	// Delete Char & Traverse Example

	char test[11] = "hahawidkfj";
	c_delete_from_tree("test relation", test);

	traverse("test relation");




	// Delete Integer & Traverse & Print Leaf Ascending Example

	int key_to_delete[11] = {3, 9, 5, 10, 27, 13, 2, 7, 44, 22, 6};

	for(int i=0; i<sizeof(key_to_delete)/sizeof(key_to_delete[0]); i++) {
		cout<<"====== delete key: "<<key_to_delete[i] << " ======"<<endl;

		i_delete_from_tree("test relation1", key_to_delete[i]);

		cout<<"Leaf: "<<endl;
		print_leaf_ascending("test relation1");
	}


	int key_test[290] = {0,416,40,427,4,451,426,301,335,51,214,193,477,158,179,334,435,277,58,159,72,148,20,327,232,160,117,87,420,70,47,343,121,350,227,7,86,119,441,447,480,80,300,61,253,323,360,146,466,56,445,339,219,36,6,490,55,218,157,347,98,243,53,23,472,212,454,450,28,296,256,299,374,488,440,186,330,89,419,316,359,305,90,492,213,260,195,484,467,279,115,341,171,216,168,398,254,187,140,410,17,273,432,185,120,33,409,270,105,52,73,173,438,433,474,109,302,459,22,372,26,206,64,242,348,151,60,331,399,147,248,245,406,261,240,229,346,5,21,366,280,14,156,395,83,110,499,457,437,344,353,452,50,309,483,251,226,368,306,42,332,470,190,289,283,375,203,38,439,284,81,150,170,313,215,464,312,411,267,174,41,430,238,310,355,37,144,49,458,328,446,425,46,205,63,487,415,85,197,444,142,329,155,380,181,97,428,322,292,222,199,92,9,354,496,2,461,282,231,249,340,152,485,129,176,244,88,493,362,272,455,463,401,263,402,136,495,290,317,29,385,178,76,211,387,413,325,394,462,308,382,1,126,192,194,165,497,145,153,217,196,175,320,135,54,198,35,100,443,453,71,128,396,288,268,45,252,74,486,489,24,134,449,383,371,138,204,223,403,424};


	// Insert Integer Example

	initial_i_bpt("test world");

	for(int i=0; i<sizeof(key_test)/sizeof(key_test[0]); i++) {
		entry *dummy= new entry;
		rid *test = new rid;

		test -> page_id = 90;
		test -> slot_id = 80;

		dummy -> key = key_test[i];
		dummy -> info = test;

		i_insert_into_tree("test world", dummy);
	}

	traverse("test world");	
	cout<<"========================"<<endl;
	print_leaf_descending("test world");


	// Delete Integer & Traverse & Print Leaf Ascending Example

	int key_test_delete[6] = {140, 52, 147, 499, 38, 425};

	for(int i=0; i<sizeof(key_test_delete)/sizeof(key_test_delete[0]); i++) {
		cout<<"====== delete key: "<<key_test_delete[i] << " ======"<<endl;

		i_delete_from_tree("test world", key_test_delete[i]);

		cout<<"Leaf: "<<endl;
		print_leaf_ascending("test world");
	}



	// ===================Query Example===========================

	rid* test_query = i_query("test world", 44);

	if(test_query != 0)
		cout<<"slot id: "<<test_query->slot_id<<endl;
	else
		cout<<"Key Not Found"<<endl;


	// =============Integer Range Query Example===================

	vector<rid*> haha = i_range_query("test world", 1, 10);

	if(haha.size() > 0) {

		cout<<"Integer range query sample: "<<haha.at(0)->slot_id<<endl;

	}



	// =============Character Range Query Example===================

	char key1[11] = "jnhgtvbgff";
	char key2[11] = "ujhqqazxsw";
	vector<rid*> s_rids = c_range_query("test relation", key1, key2);

	if(s_rids.size() > 0) {
		
		cout<<"Character range query sample: "<<s_rids.at(0)->slot_id<<endl;

	}



	// ===================Scan Example============================

	int lpage = 0;
	int ipage = 0;
	scan("test world", &lpage, &ipage);

	cout<< "Total leaf page: "<<lpage<<endl;
	cout<< "Total index page: "<<ipage<<endl;


	// ==============File Statistics Example======================

	lpage = 0;
	int slotted_data_page = 0;
	file_statistics( "test relation1", &lpage, &slotted_data_page);

	cout<< "Total index page in test relation1: "<<lpage<<endl;
	cout<< "Total slotted page in test relation1: "<<slotted_data_page<<endl;


	// ===============Display Page Example========================

	vector<unsigned short int> slot_ids = display_page("test relation", 90);
	if(slot_ids.size() > 0) {
		cout<<"Second slot id: "<<slot_ids.at(1);
	}


	return 0;
}