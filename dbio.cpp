#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "bpt.h"


#define BUFFER_SIZE 4096
#define REL_NAME_LEN 256
#define RECORD_MAX_LEN 512


class PageInteger {

	unsigned char PageData[512];

	PageInteger* NextPageInternal;

	  signed short& FreeSpacePointer;
	unsigned short& NumberOfSlots;

	public:

	PageInteger() : NextPageInternal(NULL)
		      , FreeSpacePointer(*(signed short*) (PageData+510)), NumberOfSlots(*(unsigned short*) (PageData+508)) {
		for (int i = 0; i < 512; i++) PageData[i] = 0;
		FreeSpacePointer = 0;
		NumberOfSlots = 0;
	}

	PageInteger* NextPage() { return NextPageInternal;}

	PageInteger* AllocateNewPage() {
		try{
			NextPageInternal = new PageInteger ();
		}
		catch(const std::bad_alloc&){
			fprintf(stderr,"Error allocating Space...\n");
			exit(1);
		}
		return NextPageInternal;
	}

	int CanInsertRecord(int RecordLength) {
		// fixed length records
		// -1: normal, OK
		// -2: absolutely full with respect to RecordLength
		// +N: can insert into old vacant slots (SlotNumber N)

		if (508-4*NumberOfSlots-FreeSpacePointer <  RecordLength) {
			for (int SlotNumber = 0; SlotNumber < NumberOfSlots; SlotNumber++)
				if(*(signed short*)(PageData+508-4*(SlotNumber+1)) == -1) return SlotNumber;
			return -2;
		}
		return -1;
	}

	unsigned short InsertRecordInternal(int Key, const char* Record, int RecordLength, int SlotNumber)
	{
		int TotalLength = RecordLength;

		if (SlotNumber == -1) {
			// Update Directory
			NumberOfSlots += 1;
			*(  signed short*)(PageData+508-4*NumberOfSlots) = FreeSpacePointer;
			*(unsigned short*)(PageData+510-4*NumberOfSlots) = TotalLength;

			// Update Record Slots

			*((int*) (PageData + FreeSpacePointer)) = Key;
			strncpy((char*)(PageData + FreeSpacePointer + 4), Record, TotalLength - 4 );

			// Update FSP
			FreeSpacePointer += TotalLength;

			// Set output
			SlotNumber = NumberOfSlots - 1;
		}
		else if (SlotNumber >= 0) {

			signed short Offset = SlotNumber*TotalLength;

			*(  signed short*)(PageData+508-4*(SlotNumber+1)) = Offset;
			*(unsigned short*)(PageData+510-4*(SlotNumber+1)) = TotalLength;

			*((int*) (PageData + Offset ) ) = Key;
			strncpy((char*)(PageData + Offset + 4), Record, TotalLength - 4 );
		}
		return SlotNumber;
	}
	void DeleteRecord(int SlotNumber) {
		*(  signed short*)(PageData+508-4*(SlotNumber+1)) = -1;
		*(unsigned short*)(PageData+510-4*(SlotNumber+1)) = 0;
	}

	char* GetRecord(int SlotNumber, int& Key) {
		Key = *(int*)(PageData + (*(signed short*)(PageData+508-4*(SlotNumber+1))));
		return (char*) (PageData + 4 + (*(signed short*)(PageData+508-4*(SlotNumber+1))));
	}

	void ViewThisPage(FILE* OutputFile) {
		for (int i = 0; i < 512/16; i++) {
			for (int j = 0; j < 16; j++) {
				fprintf(OutputFile,"[%2X]",PageData[i*16+j]);
			}
			fprintf(OutputFile, "\n");
		}
		//fprintf(OutputFile,"\n");
	}
};

class PageString {

	unsigned char PageData[512];

	PageString* NextPageInternal;

	  signed short& FreeSpacePointer;
	unsigned short& NumberOfSlots;

	public:

	PageString() : NextPageInternal(NULL)
		      , FreeSpacePointer(*(signed short*) (PageData+510)), NumberOfSlots(*(unsigned short*) (PageData+508)) {
		for (int i = 0; i < 512; i++) PageData[i] = 0;
		FreeSpacePointer = 0;
		NumberOfSlots = 0;
	}

	PageString* NextPage() { return NextPageInternal;}

	PageString* AllocateNewPage() {
		try{
			NextPageInternal = new PageString ();
		}
		catch(const std::bad_alloc&){
			fprintf(stderr,"Error allocating Space...\n");
			exit(1);
		}
		return NextPageInternal;
	}

	int CanInsertRecord(int RecordLength) {
		// fixed length records
		// -1: normal, OK
		// -2: absolutely full with respect to RecordLength
		// +N: can insert into old vacant slots (SlotNumber N)

		if (508-4*NumberOfSlots-FreeSpacePointer <  RecordLength) {
			for (int SlotNumber = 0; SlotNumber < NumberOfSlots; SlotNumber++)
				if(*(signed short*)(PageData+508-4*(SlotNumber+1)) == -1) return SlotNumber;
			return -2;
		}
		return -1;
	}

	unsigned short InsertRecordInternal(char* Key, const char* Record, int RecordLength, int SlotNumber)
	{
		int TotalLength = RecordLength;

		if (SlotNumber == -1) {
			// Update Directory
			NumberOfSlots += 1;
			*(  signed short*)(PageData+508-4*NumberOfSlots) = FreeSpacePointer;
			*(unsigned short*)(PageData+510-4*NumberOfSlots) = TotalLength;

			// Update Record Slots

			strncpy((char*)(PageData + FreeSpacePointer), Key, 10);
			strncpy((char*)(PageData + FreeSpacePointer + 10), Record, TotalLength - 10 );

			// Update FSP
			FreeSpacePointer += TotalLength;

			// Set output
			SlotNumber = NumberOfSlots - 1;
		}
		else if (SlotNumber >= 0) {

			signed short Offset = SlotNumber*TotalLength;

			*(  signed short*)(PageData+508-4*(SlotNumber+1)) = Offset;
			*(unsigned short*)(PageData+510-4*(SlotNumber+1)) = TotalLength;

			strncpy((char*)(PageData + Offset), Key,10);
			strncpy((char*)(PageData + Offset + 10), Record, TotalLength - 4 );
		}
		return SlotNumber;
	}
	void DeleteRecord(int SlotNumber) {
		*(  signed short*)(PageData+508-4*(SlotNumber+1)) = -1;
		*(unsigned short*)(PageData+510-4*(SlotNumber+1)) = 0;
	}

	char* GetRecord(int SlotNumber, char* Key) {
		// Remember to append '\0', not done here
		strncpy(Key, (char*) (PageData + *(signed short*)(PageData+508-4*(SlotNumber+1))),10);
		return (char*) (PageData + 10 + (*(signed short*)(PageData+508-4*(SlotNumber+1))));
	}

	void ViewThisPage(FILE* OutputFile) {
		for (int i = 0; i < 512/16; i++) {
			for (int j = 0; j < 16; j++) {
				fprintf(OutputFile,"[%2X]",PageData[i*16+j]);
			}
			fprintf(OutputFile, "\n");
		}
		//fprintf(OutputFile,"\n");
	}
};


class RelationInteger {

	PageInteger* PageList;

	int NumberOfPages;
	char RelationName[REL_NAME_LEN];

	int RecordMaxLength;

	bool initialized;

	public:

	RelationInteger() : NumberOfPages(1), initialized(false) {
		try{
			PageList = new PageInteger ();
		}
		catch(const std::bad_alloc&){
			fprintf(stderr,"Error allocating Space...\n");
			exit(1);
		}
	}
	~RelationInteger() {
		PageInteger* Temp = PageList->NextPage();
		PageInteger* Temp2;
		delete PageList;
		while (Temp != NULL) {
			Temp2 = Temp;
			Temp = Temp->NextPage();
			delete Temp2;
		}
	}

	char* GetName() {return RelationName;}
	int GetRecordLength() {return RecordMaxLength;}

	void SetName (char* ParamRelationName) {
		// must be a valid string
		int i;
		for (i = 0; i < REL_NAME_LEN && ParamRelationName[i] != '\0'; i++)
			this->RelationName[i] = ParamRelationName[i];
		if (i != REL_NAME_LEN) RelationName[i] = '\0';
	}
	void SetRecordMaxLength(int ParamRecordLength) {
		RecordMaxLength = ParamRecordLength;
	}

	bool IsInitialized(){ return initialized;}
	void SetInitialized(){ initialized = true;}

	// for "c relation-name"
	int GetNumberOfPages() {return NumberOfPages;}



	rid InsertRecord(int Key, char* Record){
		PageInteger* CurrentPage = PageList;
		unsigned short SlotNumber;

		int N, i = 0;
		N = CurrentPage->CanInsertRecord(RecordMaxLength);
		while (N == -2) {
			if (CurrentPage->NextPage() == NULL) {
				CurrentPage = CurrentPage->AllocateNewPage();
				NumberOfPages++;
			} else {
				CurrentPage = CurrentPage->NextPage();
			}
			i++;
			N = CurrentPage->CanInsertRecord(RecordMaxLength);
		}

		SlotNumber = CurrentPage->InsertRecordInternal(Key,Record,RecordMaxLength,N);
		return rid({(unsigned short)i,SlotNumber});
	}

	void DeleteRecord(rid ParamRID, FILE* OutputFile) {

		unsigned short PageID = ParamRID.page_id;

		PageInteger* Temp = PageList;
		for (int i = 0; i < PageID; i++) {
			if (Temp->NextPage() != NULL)
				Temp = Temp->NextPage();
			else {
				fprintf(OutputFile,"Invalid Page Access: asked for Page %d, but there are only %d pages starting at page 0.\n",PageID,GetNumberOfPages());
				return;
			}
		}
		Temp->DeleteRecord(ParamRID.slot_id);
	}

	void GetRecord(rid ParamRID,FILE* OutputFile) {

		unsigned short PageID = ParamRID.page_id;

		PageInteger* Temp = PageList;
		for (int i = 0; i < PageID; i++) {
			if (Temp->NextPage() != NULL)
				Temp = Temp->NextPage();
			else {
				fprintf(OutputFile,"Invalid Page Access: asked for Page %d, but there are only %d pages starting at page 0.\n",PageID,GetNumberOfPages());
				return;
			}
		}
		int Key;
		char Record[RECORD_MAX_LEN];

		char* Pointer = Temp->GetRecord(ParamRID.slot_id, Key);
		strncpy(Record,Pointer,this->GetRecordLength()-4);

		fprintf(OutputFile,"Retrieved Record (%d,\"%s\")\n",Key,Record);
	}

	// for p relation-name page-id
	// output full 512 bytes in %X to writable stream (stdout or file)
	void ViewPage(FILE* OutputFile, int PageID) {
		PageInteger* Temp = PageList;
		for (int i = 0; i < PageID; i++) {
			if (Temp->NextPage() != NULL) {
				Temp = Temp->NextPage();
			}
			else {
				fprintf(OutputFile,"Invalid Page Access: asked for Page %d, but there are only %d pages starting at page 0.\n",PageID,GetNumberOfPages());
				return;
			}
		}
		Temp->ViewThisPage(OutputFile);
	}

};

class RelationString {

	PageString* PageList;

	int NumberOfPages;
	char RelationName[REL_NAME_LEN];

	int RecordMaxLength;

	bool initialized;

	public:

	RelationString() : NumberOfPages(1), initialized(false) {
		try{
			PageList = new PageString ();
		}
		catch(const std::bad_alloc&){
			fprintf(stderr,"Error allocating Space...\n");
			exit(1);
		}
	}
	~RelationString() {
		PageString* Temp = PageList->NextPage();
		PageString* Temp2;
		delete PageList;
		while (Temp != NULL) {
			Temp2 = Temp;
			Temp = Temp->NextPage();
			delete Temp2;
		}
	}

	char* GetName() {return RelationName;}
	int GetRecordLength() {return RecordMaxLength;}

	void SetName (char* ParamRelationName) {
		// must be a valid string
		int i;
		for (i = 0; i < REL_NAME_LEN && ParamRelationName[i] != '\0'; i++)
			this->RelationName[i] = ParamRelationName[i];
		if (i != REL_NAME_LEN) RelationName[i] = '\0';
	}
	void SetRecordMaxLength(int ParamRecordLength) {
		RecordMaxLength = ParamRecordLength;
	}

	bool IsInitialized(){ return initialized;}
	void SetInitialized(){ initialized = true;}

	// for "c relation-name"
	int GetNumberOfPages() {return NumberOfPages;}


	rid InsertRecord(char* Key, char* Record){
		PageString* CurrentPage = PageList;
		unsigned short SlotNumber;

		int N, i = 0;
		N = CurrentPage->CanInsertRecord(RecordMaxLength);
		while (N == -2) {
			if (CurrentPage->NextPage() == NULL) {
				CurrentPage = CurrentPage->AllocateNewPage();
				NumberOfPages++;
			} else {
				CurrentPage = CurrentPage->NextPage();
			}
			i++;
			N = CurrentPage->CanInsertRecord(RecordMaxLength);
		}

		SlotNumber = CurrentPage->InsertRecordInternal(Key,Record,RecordMaxLength,N);
		return rid({(unsigned short)i,SlotNumber});
	}

	void DeleteRecord(rid ParamRID,FILE* OutputFile) {

		unsigned short PageID = ParamRID.page_id;

		PageString* Temp = PageList;
		for (int i = 0; i < PageID; i++) {
			if (Temp->NextPage() != NULL)
				Temp = Temp->NextPage();
			else {
				fprintf(OutputFile,"Invalid Page Access: asked for Page %d, but there are only %d pages starting at page 0.\n",PageID,GetNumberOfPages());
				return;
			}
		}
		Temp->DeleteRecord(ParamRID.slot_id);
	}

	void GetRecord(rid ParamRID,FILE* OutputFile) {

		unsigned short PageID = ParamRID.page_id;

		PageString* Temp = PageList;
		for (int i = 0; i < PageID; i++) {
			if (Temp->NextPage() != NULL)
				Temp = Temp->NextPage();
			else {
				fprintf(OutputFile,"Invalid Page Access: asked for Page %d, but there are only %d pages starting at page 0.\n",PageID,GetNumberOfPages());
				return;
			}
		}
		char Key[11];
		Key[10] = '\0';

		char Record[RECORD_MAX_LEN];

		char* Pointer = Temp->GetRecord(ParamRID.slot_id, Key);
		strncpy(Record,Pointer,this->GetRecordLength()-10);

		fprintf(OutputFile,"Retrieved Record (\"%s\",\"%s\")\n",Key,Record);
	}

	// for p relation-name page-id
	// output full 512 bytes in %X to writable stream (stdout or file)
	void ViewPage(FILE* OutputFile, int PageID) {
		PageString* Temp = PageList;
		for (int i = 0; i < PageID; i++) {
			if (Temp->NextPage() != NULL) {
				Temp = Temp->NextPage();
			}
			else {
				fprintf(OutputFile,"Invalid Page Access: asked for Page %d, but there are only %d pages starting at page 0.\n",PageID,GetNumberOfPages());
				return;
			}
		}
		Temp->ViewThisPage(OutputFile);
	}

};

void SkipWhiteSpaces(char*& Current) {

	while(*Current == ' ') Current++;
}

int parseInt(char*& str)
{
	int val = ('0' <= str[0] && str[0] <= '9') ? 0 : -1;
	while('0' <= str[0] && str[0] <= '9') {
		val = val*10 + (str[0]-'0');
		str++;
	}
	return val;
}

int ParseInstructionR(char* Buffer, RelationInteger& RelInt, RelationString& RelStr, FILE* OutputFile)
{
	char RelationName[REL_NAME_LEN];
	int RecordLength;

	//R, Relation-name, key-type, record-length

	int output_code;

	char* Current = Buffer+1;
	SkipWhiteSpaces(Current);

	// Read first comma,
	if (*Current != ',') return -1; // bad comma
	++Current;
	SkipWhiteSpaces(Current);

	// Read RelationName
	int i;
	for (i = 0; i < REL_NAME_LEN && *Current != ' ' && *Current != ',' && *Current != '\n' ;i++) {
		RelationName[i] = *Current++;
	}
	if (i == REL_NAME_LEN) return -2; // bad RelNameLen
	RelationName[i] = '\0';
	SkipWhiteSpaces(Current);

	// Read second comma,
	if (*Current != ',') return -1; // bad comma
	++Current;
	SkipWhiteSpaces(Current);

	// Read key type ("integer" or "String")
	if(strncmp(Current,"integer",7) == 0) {
		Current += 7;
		output_code = 1;
	}
	else if (strncmp(Current, "String",6) == 0) {
		Current += 6;
		output_code = 2;
	}
	else return -1;
	SkipWhiteSpaces(Current);

	// Read third comma,
	if (*Current != ',') return -1; // bad comma
	++Current;
	SkipWhiteSpaces(Current);

	// Read record length
	RecordLength = parseInt(Current);
	if (RecordLength == -1) return -5; // bad integer
	SkipWhiteSpaces(Current);

	// Check for '\n'
	if (*Current != '\n') return -6; // excessive input

	// use output_code to attempt to initialize the right Relation
	if (RelInt.IsInitialized() && output_code == 1) return -3; // already initialized
	if (RelStr.IsInitialized() && output_code == 2) return -3; // already initialized
	if (RelInt.IsInitialized() && strcmp(RelationName,RelInt.GetName()) == 0) return -4; // Relation name already in use
	if (RelStr.IsInitialized() && strcmp(RelationName,RelStr.GetName()) == 0) return -4; // Relation name already in use

	if (output_code == 1) {
		fprintf(OutputFile,"Creating integer relation with name \"%s\" with length %d\n", RelationName, RecordLength);
		RelInt.SetName(RelationName);
		RelInt.SetRecordMaxLength(RecordLength);
		RelInt.SetInitialized();

		/* set the tree*/
		initial_bpt(RelationName);
		/*             */
	} else {
		fprintf(OutputFile,"Creating String relation with name \"%s\" with length %d\n", RelationName, RecordLength);
		RelStr.SetName(RelationName);
		RelStr.SetRecordMaxLength(RecordLength);
		RelStr.SetInitialized();

		/* set the tree */
		//initial_bpt(RelationName); // not done yet
		/*              */
	}
	return output_code;
}

int ParseInstructionI (char* Buffer, RelationInteger& RelInt, RelationString& RelStr, FILE* OutputFile) {

	// I, Chat, 0, "Hello world!"

	// various buffers
	int IntegerKey;
	char StringKey[11]; StringKey[10] = '\0';
	char Record[RECORD_MAX_LEN];
	char RelationName[REL_NAME_LEN];

	// Start after the 'I' read,

	char* Current = Buffer + 1;
	SkipWhiteSpaces(Current);

	// Check first comma,
	if (*Current != ',') return -1; // illegal input
	++Current;
	SkipWhiteSpaces(Current);

	// Read the relation to insert in to parameter RelationName:
	int i;
	for (i = 0; i < 256 && *Current != ' ' && *Current != ',' && *Current != '\n' ;i++) {
		RelationName[i] = *Current++;
	}
	if (i == 256) return -1;
	RelationName[i] = '\0';
	SkipWhiteSpaces(Current);

	// Check second comma,
	if (*Current != ',') return -1; // illegal input
	++Current;
	SkipWhiteSpaces(Current);

	// match the relation-name read to either of our relations,
	bool InsertToInt;
	if (RelInt.IsInitialized() && strcmp(RelationName,RelInt.GetName()) == 0) InsertToInt = true; // Insert to RelInt
	else if (RelStr.IsInitialized() && strcmp(RelationName,RelStr.GetName()) == 0) InsertToInt = false; // Insert to RelStr
	else return -3; // RelName did not match

	for(i = 0; i < 7; i++) {
		// read key,
		if(InsertToInt) {
			IntegerKey = parseInt(Current);
			if (IntegerKey == -1) return -1; // bad integer
		} else {
			if (*Current != '\"') return -1; // illegal input
			++Current;
			strncpy(StringKey,Current,10);
			while(*Current != '\n' && *Current != '\"' ) Current++;
			if (*Current != '\"') return -1; // illegal input
			++Current;
		}
		SkipWhiteSpaces(Current);

		// Check third comma,
		if (*Current != ',') return -1; // illegal input
		++Current;
		SkipWhiteSpaces(Current);

		// Check for '\"'
		if (*Current != '\"') return -1; // illegal input
		++Current;

		// Read record
		int j;
		for (j = 0; j < RECORD_MAX_LEN && *Current != '\n' && *Current != '\"'; j++) {
			Record[j] = *Current++;
		}
		if (j == RECORD_MAX_LEN) {

			while(*Current != '\n' && *Current != '\"') Current++;
			if (*Current == '\n') {
				fprintf(OutputFile,"Double quote \" expected but missing. \n");
				return -1;
			}
			else return -2;
		}
		Record[j] = '\0';

		// Check for '\"'
		if (*Current != '\"') {
			fprintf(OutputFile,"Double quote \" expected but missing. \n");
			return -1;
		}
		++Current;
		SkipWhiteSpaces(Current);


		if (InsertToInt) {
			// RelInt
			if((int) strlen(Record)+4 > RelInt.GetRecordLength()) {
				fprintf(OutputFile,"Warning: specified record has total length = %d,", (int) strlen(Record)+4);
				fprintf(OutputFile,"which cannot fit in the specified record length (%d). ", RelStr.GetRecordLength());
				fprintf(OutputFile,"Only the first %d bytes of the string will be stored.\n",RelStr.GetRecordLength()-4);
				Record[RelInt.GetRecordLength()-4] = '\0';
			}
			rid RIDHolder = RelInt.InsertRecord(IntegerKey,Record);
			fprintf(OutputFile,"Inserting record (%d,\"%s\") into \"%s\"\n", IntegerKey, Record, RelationName);

			/* Insert into tree */
			entry *Entry= new entry;
			rid *PointerRID = new rid (RIDHolder);

			Entry -> key = IntegerKey;
			Entry -> info = PointerRID;

			insert_into_tree(Entry);
			/*                  */
		}
		else {
			// RelStr
			if((int) strlen(Record)+10 > RelStr.GetRecordLength()) {
				fprintf(OutputFile,"Warning: specified record has total length = %d,", (int) strlen(Record)+10);
				fprintf(OutputFile,"which cannot fit with the specified record length (%d). ", RelStr.GetRecordLength());
				fprintf(OutputFile,"Only the first %d bytes of the string will be stored.\n",RelStr.GetRecordLength()-10);
				Record[RelStr.GetRecordLength()-10] = '\0';
			}
			rid RIDHolder = RelStr.InsertRecord(StringKey,Record);
			fprintf(OutputFile,"Inserting record (\"%s\",\"%s\") into \"%s\"\n", StringKey, Record, RelationName);
		}

		if (*Current == '\n') break;

		if (i != 6 && *Current != ';') return -1; // bad multiple read

		++Current;
		SkipWhiteSpaces(Current);
	}

	if (*Current == '\n') return i+1;
	return -1;
}

int ParseInstructionD(char* Buffer, RelationInteger& RelInt, RelationString& RelStr, FILE* OutputFile) {


	// various buffers
	int IntegerKey;
	char StringKey[11]; StringKey[10] = '\0';
	char RelationName[REL_NAME_LEN];

	// D, Chat, 499
	char* Current = Buffer + 1;
	SkipWhiteSpaces(Current);

	// Check first comma,
	if (*Current != ',') return -1; // illegal input
	++Current;
	SkipWhiteSpaces(Current);

	// Read Relation Name (max length 256 counting '\0')
	int i;
	for (i = 0; i < 256 && *Current != ' ' && *Current != ',' && *Current != '\n' ;i++) {
		RelationName[i] = *Current++;
	}
	if (i == 256) return -1;
	RelationName[i] = '\0';
	SkipWhiteSpaces(Current);

	// Check for second comma,
	if (*Current != ',') return -1; // illegal input
	++Current;
	SkipWhiteSpaces(Current);

	// match the relation-name read to either of our relations,
	bool IsIntRelation;
	if (RelInt.IsInitialized() && strcmp(RelationName,RelInt.GetName()) == 0) IsIntRelation = true; // Insert to RelInt
	else if (RelStr.IsInitialized() && strcmp(RelationName,RelStr.GetName()) == 0) IsIntRelation = false; // Insert to RelStr
	else return -3; // RelName did not match

	// Read Key
	if(IsIntRelation) {
		IntegerKey = parseInt(Current);
		if (IntegerKey == -1) return -1; // bad integer
	} else {
		if (*Current != '\"') return -1; // illegal input
		++Current;
		strncpy(StringKey,Current,10);
		while(*Current != '\n' && *Current != '\"' ) Current++;
		if (*Current != '\"') return -1; // illegal input
		++Current;
	}
	SkipWhiteSpaces(Current);

	// Check for '\n'
	if (*Current != '\n') return -1; // illegal input

	// Do BPT query + BPT deletion + page slot deletion
	if(IsIntRelation) {
		/* Tree query + Deletion */
		rid* PointerRID = query(RelationName, IntegerKey);
		delete_from_tree(IntegerKey);
		/*                       */

		fprintf(OutputFile,"Deleting record with key %d\n", IntegerKey);
		RelInt.DeleteRecord(*PointerRID,OutputFile);
	} else {
		fprintf(OutputFile,"Need to delete record with key \"%s\", but this is not implemented yet.\n", StringKey);

	}
	return 1;
}

int ParseInstructionScan(char* Buffer, RelationInteger& RelInt, RelationString& RelStr, FILE* OutputFile) {

	char RelationName[REL_NAME_LEN];

	char* Current = Buffer + 4;
	// Check it's actually "Scan"
	if (strncmp(Buffer,"Scan",4)!=0) return -2; // Not Scan
	SkipWhiteSpaces(Current);

	// Read RelationName
	int i;
	for (i = 0; i < 256 && *Current != ' ' && *Current != ',' && *Current != '\n' ;i++) {
		RelationName[i] = *Current++;
	}
	if (i == 256) return -1; // too long
	RelationName[i] = '\0';
	SkipWhiteSpaces(Current);

	// Check for '\n'
	if (*Current != '\n') return -1;

	// match the relation-name read to either of our relations,
	bool IsIntRelation;
	if (RelInt.IsInitialized() && strcmp(RelationName,RelInt.GetName()) == 0) IsIntRelation = true; // Insert to RelInt
	else if (RelStr.IsInitialized() && strcmp(RelationName,RelStr.GetName()) == 0) IsIntRelation = false; // Insert to RelStr
	else return -3; // RelName did not match

	/* Output BPT node/leaf count */
	if (IsIntRelation) {
		fprintf(OutputFile, "Telling tree info of \"%s\": # of leaf pages, # of total index pages.\n", RelationName);
		/* Get tree info */
		scan(RelationName);
		/*               */
	} else {
		fprintf(OutputFile, "Telling tree info of \"%s\": # of leaf pages, # of total index pages.\n", RelationName);
		fprintf(OutputFile, "This is not implemented yet.\n");
	}

	return 1;
}

int ParseInstructionLQ(char* Buffer, RelationInteger& RelInt, RelationString& RelStr, FILE* OutputFile) {

	int IntegerKey1, IntegerKey2;
	char StringKey1[11], StringKey2[11];
	StringKey1[10] = 0;
	StringKey2[10] = 0;
	char RelationName[REL_NAME_LEN];

	// q Movie "owjafbcada" "xzrlijkeuufoyxj"
	// although this one is for int

	char* Current = Buffer+1;
	SkipWhiteSpaces(Current);

	// Read RelationName
	int i;
	for (i = 0; i < 256 && *Current != ' ' && *Current != ',' && *Current != '\n' ;i++) {
		RelationName[i] = *Current++;
	}
	if (i == 256) return -1; // too long
	RelationName[i] = '\0';
	SkipWhiteSpaces(Current);

	// match the relation-name read to either of our relations,
	bool IsIntRelation;
	if (RelInt.IsInitialized() && strcmp(RelationName,RelInt.GetName()) == 0) IsIntRelation = true; // Insert to RelInt
	else if (RelStr.IsInitialized() && strcmp(RelationName,RelStr.GetName()) == 0) IsIntRelation = false; // Insert to RelStr
	else return -3; // RelName did not match

	// Read Key1
	if(IsIntRelation) {
		IntegerKey1 = parseInt(Current);
		if (IntegerKey1 == -1) return -1; // bad integer
	} else {
		if (*Current != '\"') return -1; // illegal input
		++Current;
		strncpy(StringKey1,Current,10);
		while(*Current != '\n' && *Current != '\"' ) Current++;
		if (*Current != '\"') return -1; // illegal input
		++Current;
	}
	SkipWhiteSpaces(Current);


	if (*Current == '\n') {
		if(IsIntRelation) {
			/* RID = tree query */
			rid* PointerRID = query(RelationName,IntegerKey1);
			/*                  */

			RelInt.GetRecord(*PointerRID,OutputFile);
			return 1;
		} else {
			fprintf(OutputFile,"Equality query, key = \"%s\". This is not implemented yet.\n", StringKey1);
			/* RID = bpt-function */
			return 2;
		}
	}

	// Read Key2 if not '\n'
	if(IsIntRelation) {
		IntegerKey2 = parseInt(Current);
		if (IntegerKey2 == -1) return -1; // bad integer
	} else {
		if (*Current != '\"') return -1; // illegal input
		++Current;
		strncpy(StringKey2,Current,10);
		while(*Current != '\n' && *Current != '\"' ) Current++;
		if (*Current != '\"') return -1; // illegal input
		++Current;
	}
	SkipWhiteSpaces(Current);

	if (*Current == '\n') {
		if(IsIntRelation) {
			fprintf(OutputFile,"Range query, key in [%d,%d]. This is not implemented yet.\n", IntegerKey1, IntegerKey2);
			// RIDs = bpt-functions ?
			// RelInt.GetRecord(RIDs)
			return 3;
		} else {

			fprintf(OutputFile,"Range query, key in [\"%s\",\"%s\"]. This is not implemented yet.\n", StringKey1, StringKey2);
			// RIDs = bpt-functions ?
			// RelInt.GetRecord(RIDs)
			return 4;
		}

	}
	return -1;
}

int ParseInstructionLP(char* Buffer, RelationInteger& RelInt, RelationString& RelStr, FILE* OutputFile){

	char RelationName[REL_NAME_LEN];
	int PageID;

	// p Chat 1
	// p Movie 2

	char* Current = Buffer+1;
	SkipWhiteSpaces(Current);

	// Read relation name
	int i;
	for (i = 0; i < 256 && *Current != ' ' && *Current != ',' && *Current != '\n' ;i++) {
		RelationName[i] = *Current++;
	}
	if (i == 256) return -1; // too long
	RelationName[i] = '\0';
	SkipWhiteSpaces(Current);

	// Read PageID
	PageID = parseInt(Current);
	if (PageID == -1) return -1; // bad integer
	SkipWhiteSpaces(Current);

	// match the relation-name read to either of our relations,
	bool IsIntRelation;
	if (RelInt.IsInitialized() && strcmp(RelationName,RelInt.GetName()) == 0) IsIntRelation = true; // Insert to RelInt
	else if (RelStr.IsInitialized() && strcmp(RelationName,RelStr.GetName()) == 0) IsIntRelation = false; // Insert to RelStr
	else return -3; // RelName did not match

	if (*Current == '\n') {
		if (IsIntRelation) {
			fprintf(OutputFile,"Displaying page %d of relation \"%s\":\n",PageID, RelationName);
			RelInt.ViewPage(OutputFile,PageID);
		} else {
			fprintf(OutputFile,"Displaying page %d of relation \"%s\":\n",PageID, RelationName);
			RelStr.ViewPage(OutputFile,PageID);
		}
		return 1;
	}
	return -1;
}

int ParseInstructionLC(char* Buffer, RelationInteger& RelInt, RelationString& RelStr, FILE* OutputFile) {

	char RelationName[REL_NAME_LEN];
	// c Chat

	char* Current = Buffer+1;
	SkipWhiteSpaces(Current);

	// Read relation name
	int i;
	for (i = 0; i < 256 && *Current != ' ' && *Current != ',' && *Current != '\n' ;i++) {
		RelationName[i] = *Current++;
	}
	if (i == 256) return -1; // too long
	RelationName[i] = '\0';
	SkipWhiteSpaces(Current);

	// match the relation-name read to either of our relations,
	bool IsIntRelation;
	if (RelInt.IsInitialized() && strcmp(RelationName,RelInt.GetName()) == 0) IsIntRelation = true; // Insert to RelInt
	else if (RelStr.IsInitialized() && strcmp(RelationName,RelStr.GetName()) == 0) IsIntRelation = false; // Insert to RelStr
	else return -3; // RelName did not match

	fprintf(OutputFile,"Displaying statistics of relation \"%s\":\n",RelationName);

	if (IsIntRelation) {
		fprintf(OutputFile,"  %d slotted pages.\n",RelInt.GetNumberOfPages());
		fprintf(OutputFile,"  ___ index pages. Refer to console.\n");
		/* Tree total index page */
		scan(RelationName);
		/*  */
	} else {
		fprintf(OutputFile,"  %d slotted pages.\n",RelStr.GetNumberOfPages());
		fprintf(OutputFile,"  ___ index pages. Not implemented yet.\n");
	}

	if (*Current == '\n') return 1;
	return -1;
}

int main () {

	FILE* InputFile = fopen("projectB_data.txt","r");
	FILE* OutputFile = fopen("log.txt","w");

	if (InputFile == NULL || OutputFile == NULL) {
		fprintf(stderr,"Error opening files.\n");
		exit(1);
	}

	char Buffer[BUFFER_SIZE];

	RelationInteger RelInt;
	RelationString RelStr;

	while(fgets(Buffer,sizeof(Buffer),InputFile) != NULL)
	{
		switch (Buffer[0]) {
			case 'R':
				switch(ParseInstructionR(Buffer,RelInt,RelStr,OutputFile)){
					case -6: break;
					case -5: break;
					case -4: fprintf(OutputFile,"The name is already in use.\n"); break;
					case -3: fprintf(OutputFile,"The relation (int or string) is already initialized. This program can only initialize one of each.\n"); break;
					case -2: fprintf(OutputFile,"relation-name field in \"R, relation-name, data-type, record-length\" to be read is longer than expected length (%d).\n", REL_NAME_LEN); break;
					case -1: fprintf(OutputFile,"Comma ',' is expected but missing.\n"); break;
					case 1:
					case 2:
						break;
				}
				break;
			case 'I':
				switch(int N = ParseInstructionI(Buffer, RelInt, RelStr,OutputFile)){
					case -4: fprintf(OutputFile,"This line requires inserting a record that's too long, hence cancelled regardless of its validness.\n");
					case -3: fprintf(OutputFile,"No matching Relation with name.\n"); break;
					case -2: fprintf(OutputFile,"Input is illegal, or string to be read is longer than expected length.\n"); break;
					case -1: fprintf(OutputFile,"Input is illegal.\n"); break;
					default:
						fprintf(OutputFile,"> Inserted %d record", N);
						if (N!=1) fprintf(OutputFile,"s");
						fprintf(OutputFile,".\n");
						break;
				}
				break;
			case 'D':
				switch(ParseInstructionD(Buffer, RelInt, RelStr,OutputFile)) {
					case -1: fprintf(OutputFile,"Input is illegal.\n"); break;
					case 1: break;
				}
				break;
			case 'S':
				switch(ParseInstructionScan(Buffer, RelInt, RelStr,OutputFile)) {
					case -2:
						char* ch;
						ch = strchr(Buffer,' '); if(ch != NULL) *ch = '\0';
						ch = strchr(Buffer,'\"'); if(ch != NULL) *ch = '\0';
						ch = strchr(Buffer,'\n'); if(ch != NULL) *ch = '\0';
						fprintf(OutputFile,"No such instruction: \"%s\".\n", Buffer);
						break;
					case -1: fprintf(OutputFile,"Input is illegal.\n"); break;
					case 1: break;
				}
				break;
			case 'q':
				switch(ParseInstructionLQ(Buffer, RelInt, RelStr,OutputFile)) {
					case -1: fprintf(OutputFile,"Input is illegal.\n"); break;
					case 1:
					case 2:
					case 3:
					case 4:
						break;
				}
				break;
			case 'p':
				switch(ParseInstructionLP(Buffer,RelInt,RelStr,OutputFile)) {
					case -1: fprintf(OutputFile,"Input is illegal.\n"); break;
					case -3: fprintf(OutputFile,"No Relation matched the name.\n"); break;
					case 1:
						break;
				}
				break;
			case 'c':
				switch(ParseInstructionLC(Buffer,RelInt,RelStr,OutputFile)) {
					case -1: fprintf(OutputFile,"Input is illegal.\n"); break;
					case -3: fprintf(OutputFile,"No Relation matched the name.\n"); break;
					case 1:
						break;
				}
				break;
			case '\n':
				fprintf(OutputFile,"\n");
				break;
			case ' ':
				fprintf(OutputFile,"Input should not start with an empty space.\n");
				break;
			default:
				char* ch;
				ch = strchr(Buffer,' '); if(ch != NULL) *ch = '\0';
				ch = strchr(Buffer,'\"'); if(ch != NULL) *ch = '\0';
				ch = strchr(Buffer,'\n'); if(ch != NULL) *ch = '\0';

				fprintf(OutputFile,"No such instruction: \"%s\".\n", Buffer);
		}
	}
	if (ferror(InputFile) != 0) {
		fprintf(stderr,"Error reading file.\n");
	} else if (feof(InputFile) != 0){
		fprintf(stderr,"End of file reached.\n");
	}

	return 0;
}
