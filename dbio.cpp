#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "bpt.h"


#define BUFFER_SIZE 4096
#define REL_NAME_LEN 256
#define RECORD_MAX_LEN 512

#define DBIO_LOG_OUTPUT

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

	bool WriteEmptyMode;
	PageInteger* WriteEmptyModePagePointer;
	int WriteEmptyModePageID;

	public:

	RelationInteger() : NumberOfPages(1), initialized(false), WriteEmptyMode(true), WriteEmptyModePageID(0) {
		try{
			PageList = new PageInteger ();
		}
		catch(const std::bad_alloc&){
			fprintf(stderr,"Error allocating Space...\n");
			exit(1);
		}
		WriteEmptyModePagePointer = PageList;
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

		int N = -1, PageCounter = 0;

		if (WriteEmptyMode && WriteEmptyModePagePointer->CanInsertRecord(RecordMaxLength) == -1) {
			CurrentPage = WriteEmptyModePagePointer;
			PageCounter = WriteEmptyModePageID;
		} else {
			WriteEmptyMode = false;
			N = CurrentPage->CanInsertRecord(RecordMaxLength);
			while (N == -2) {
				PageCounter++;
				if (CurrentPage->NextPage() == NULL) {
					CurrentPage = CurrentPage->AllocateNewPage();
					NumberOfPages++;
					WriteEmptyMode = true;
					WriteEmptyModePagePointer = CurrentPage;
					WriteEmptyModePageID = PageCounter;
				} else {
					CurrentPage = CurrentPage->NextPage();
				}
				N = CurrentPage->CanInsertRecord(RecordMaxLength);
			}
		}

		SlotNumber = CurrentPage->InsertRecordInternal(Key,Record,RecordMaxLength,N);

		rid Temp;
		Temp.page_id = (unsigned short) PageCounter;
		Temp.slot_id = SlotNumber;

		return Temp;
	}

	void DeleteRecord(rid ParamRID, FILE* OutputFile) {

		unsigned short PageID = ParamRID.page_id;

		PageInteger* Temp = PageList;
		for (int i = 0; i < PageID; i++) {
			if (Temp->NextPage() != NULL)
				Temp = Temp->NextPage();
			else {
#ifdef DBIO_LOG_OUTPUT
				fprintf(OutputFile,"Error: Invalid Page Access: asked for Page %d, but there are only %d pages starting at page 0.\n",PageID,GetNumberOfPages());
#endif
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
				fprintf(OutputFile,"Error: Invalid Page Access: asked for Page %d, but there are only %d pages starting at page 0.\n",PageID,GetNumberOfPages());
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
				fprintf(OutputFile,"Error: Invalid Page Access: asked for Page %d, but there are only %d pages starting at page 0.\n",PageID,GetNumberOfPages());
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

	bool WriteEmptyMode;
	PageString* WriteEmptyModePagePointer;
	int WriteEmptyModePageID;

	public:

	RelationString() : NumberOfPages(1), initialized(false), WriteEmptyMode(true), WriteEmptyModePageID(0) {
		try{
			PageList = new PageString ();
		}
		catch(const std::bad_alloc&){
			fprintf(stderr,"Error allocating Space...\n");
			exit(1);
		}
		WriteEmptyModePagePointer = PageList;
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

		int N = -1, PageCounter = 0;

		if (WriteEmptyMode && WriteEmptyModePagePointer->CanInsertRecord(RecordMaxLength) == -1) {
			CurrentPage = WriteEmptyModePagePointer;
			PageCounter = WriteEmptyModePageID;
		} else {
			WriteEmptyMode = false;
			N = CurrentPage->CanInsertRecord(RecordMaxLength);
			while (N == -2) {
				PageCounter++;
				if (CurrentPage->NextPage() == NULL) {
					CurrentPage = CurrentPage->AllocateNewPage();
					NumberOfPages++;
					WriteEmptyMode = true;
					WriteEmptyModePagePointer = CurrentPage;
					WriteEmptyModePageID = PageCounter;
				} else {
					CurrentPage = CurrentPage->NextPage();
				}
				N = CurrentPage->CanInsertRecord(RecordMaxLength);
			}
		}

		SlotNumber = CurrentPage->InsertRecordInternal(Key,Record,RecordMaxLength,N);

		rid Temp;
		Temp.page_id = (unsigned short) PageCounter;
		Temp.slot_id = SlotNumber;

		return Temp;
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

int MatchRelation(char* RelationName, RelationInteger& RelInt, RelationString RelStr) {

	if (RelInt.IsInitialized() && strcmp(RelationName,RelInt.GetName()) == 0) return 1; // Matched RelInt
	else if (RelStr.IsInitialized() && strcmp(RelationName,RelStr.GetName()) == 0) return 2; // Matched RelStr
	else return -4; // relname mismatch
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
	if (i == 0) return -8; // bad RelName
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
	else return -7; // bad data-type
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
#ifdef DBIO_LOG_OUTPUT
		fprintf(OutputFile,"Creating integer relation with name \"%s\" with length %d\n", RelationName, RecordLength);
#endif
		RelInt.SetName(RelationName);
		RelInt.SetRecordMaxLength(RecordLength);
		RelInt.SetInitialized();

		/* set the tree*/
		initial_i_bpt(RelationName);
		/*             */
	} else {
#ifdef DBIO_LOG_OUTPUT
		fprintf(OutputFile,"Creating String relation with name \"%s\" with length %d\n", RelationName, RecordLength);
#endif
		RelStr.SetName(RelationName);
		RelStr.SetRecordMaxLength(RecordLength);
		RelStr.SetInitialized();

		/* set the tree */
		initial_c_bpt(RelationName);
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
	if (*Current != ',') return -1; // bad comma
	++Current;
	SkipWhiteSpaces(Current);

	// Read the relation to insert in to parameter RelationName:
	int i;
	for (i = 0; i < 256 && *Current != ' ' && *Current != ',' && *Current != '\n' ;i++) {
		RelationName[i] = *Current++;
	}
	if (i == 256) return -2; // bad relnamelen
	if (i == 0) return -3; // empty relname
	RelationName[i] = '\0';
	SkipWhiteSpaces(Current);

	// Check second comma,
	if (*Current != ',') return -1; // bad comma
	++Current;
	SkipWhiteSpaces(Current);

	// match the relation-name read to either of our relations,
	bool InsertToInt;
	if (RelInt.IsInitialized() && strcmp(RelationName,RelInt.GetName()) == 0) InsertToInt = true; // Matched RelInt
	else if (RelStr.IsInitialized() && strcmp(RelationName,RelStr.GetName()) == 0) InsertToInt = false; // Matched RelStr
	else return -4; // relname mismatch

	for(i = 0; i < 7; i++) {
		// read key,
		if(InsertToInt) {
			IntegerKey = parseInt(Current);
			if (IntegerKey == -1) return -5; // bad integer
		} else {
			if (*Current != '\"') return -6; // bad dquote
			++Current;
			strncpy(StringKey,Current,10);
			while(*Current != '\n' && *Current != '\"' ) Current++;
			if (*Current != '\"') return -6; // bad dquote
			++Current;
		}
		SkipWhiteSpaces(Current);

		// Check third comma,
		if (*Current != ',') return -1; // bad comma
		++Current;
		SkipWhiteSpaces(Current);

		// Check for '\"'
		if (*Current != '\"') return -6; // bad dquote
		++Current;

		// Read record
		int j;
		for (j = 0; j < RECORD_MAX_LEN && *Current != '\n' && *Current != '\"'; j++) {
			Record[j] = *Current++;
		}
		if (j == RECORD_MAX_LEN) {

			while(*Current != '\n' && *Current != '\"') Current++;
			if (*Current == '\n') return -6; // bad dquote
			else return -7; // bad reclen
		}
		Record[j] = '\0';

		// Check for '\"'
		if (*Current != '\"') return -6; // bad dquote
		++Current;
		SkipWhiteSpaces(Current);


		if (InsertToInt) {
			// RelInt
			if((int) strlen(Record)+4 > RelInt.GetRecordLength()) {
				fprintf(OutputFile,"Warning: specified record has total length = %d, ", (int) strlen(Record)+4);
				fprintf(OutputFile,"which cannot fit in the specified record length (%d). ", RelStr.GetRecordLength());
				fprintf(OutputFile,"Only the first %d bytes of the string will be stored.\n", RelStr.GetRecordLength()-4);
				fprintf(OutputFile,"For debugging purposes, the record is printed here: \"%s\"\n", Record);
				Record[RelInt.GetRecordLength()-4] = '\0';
			}
			rid RIDHolder = RelInt.InsertRecord(IntegerKey,Record);

			/* Insert into tree */
			entry *Entry= new entry ();
			rid *PointerRID = new rid (RIDHolder);

			Entry -> key = IntegerKey;
			Entry -> info = PointerRID;

			i_insert_into_tree(RelationName,Entry);
			/*                  */
#ifdef DBIO_LOG_OUTPUT
			fprintf(OutputFile,"Inserting record (%d,\"%s\") into \"%s\"\n", IntegerKey, Record, RelationName);
#endif

		}
		else {
			// RelStr
			if((int) strlen(Record)+10 > RelStr.GetRecordLength()) {
				fprintf(OutputFile,"Warning: specified record has total length = %d,", (int) strlen(Record)+10);
				fprintf(OutputFile,"which cannot fit with the specified record length (%d). ", RelStr.GetRecordLength());
				fprintf(OutputFile,"Only the first %d bytes of the string will be stored.\n",RelStr.GetRecordLength()-10);
				fprintf(OutputFile,"For debugging purposes, the record is printed here: \"%s\"\n", Record);
				Record[RelStr.GetRecordLength()-10] = '\0';
			}
			rid RIDHolder = RelStr.InsertRecord(StringKey,Record);

			/* Insert into tree */
			c_entry *Entry= new c_entry ();
			rid *PointerRID = new rid (RIDHolder);

			strcpy(Entry -> key, StringKey);
			Entry -> info = PointerRID;

			c_insert_into_tree(RelationName,Entry);
			/*                  */
#ifdef DBIO_LOG_OUTPUT
			fprintf(OutputFile,"Inserting record (\"%s\",\"%s\") into \"%s\"\n", StringKey, Record, RelationName);
#endif
		}

		if (*Current == '\n') break;

		if (i != 6 && *Current != ';') return -8; // bad multiple read

		++Current;
		SkipWhiteSpaces(Current);
	}

	if (*Current == '\n') return i+1;
	return -9; // trailing
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
	if (*Current != ',') return -1; // bad comma
	++Current;
	SkipWhiteSpaces(Current);

	// Read Relation Name (max length 256 counting '\0')
	int i;
	for (i = 0; i < 256 && *Current != ' ' && *Current != ',' && *Current != '\n' ;i++) {
		RelationName[i] = *Current++;
	}
	if (i == 256) return -2; // bad relnamelen
	if (i == 0) return -3; // empty relname
	RelationName[i] = '\0';
	SkipWhiteSpaces(Current);

	// Check for second comma,
	if (*Current != ',') return -1; // bad comma
	++Current;
	SkipWhiteSpaces(Current);

	// match the relation-name read to either of our relations,
	bool IsIntRelation;
	if (RelInt.IsInitialized() && strcmp(RelationName,RelInt.GetName()) == 0) IsIntRelation = true; // Matched RelInt
	else if (RelStr.IsInitialized() && strcmp(RelationName,RelStr.GetName()) == 0) IsIntRelation = false; // Matched RelStr
	else return -4; // relname mismatch

	// Read Key
	if(IsIntRelation) {
		IntegerKey = parseInt(Current);
		if (IntegerKey == -1) return -5; // bad integer
	} else {
		if (*Current != '\"') return -6; // bad dquote
		++Current;
		strncpy(StringKey,Current,10);
		while(*Current != '\n' && *Current != '\"' ) Current++;
		if (*Current != '\"') return -6; // bad dquote
		++Current;
	}
	SkipWhiteSpaces(Current);

	// Check for '\n'
	if (*Current != '\n') return -7; // trailing

	// Do BPT query + BPT deletion + page slot deletion
	if(IsIntRelation) {
		/* Tree query */
		rid* PointerRID = i_query(RelationName, IntegerKey);
		/*            */
		if (PointerRID == NULL) {
#ifdef DBIO_LOG_OUTPUT
			fprintf(OutputFile,"Attempted to delete record with key %d, but it was not found.\n", IntegerKey);
#endif
			return 1; // key not found
		}
		/* Tree delete entry */
		i_delete_from_tree(RelationName,IntegerKey);
		/*                   */
#ifdef DBIO_LOG_OUTPUT
		fprintf(OutputFile,"Deleting record with key %d.\n", IntegerKey);
#endif
		RelInt.DeleteRecord(*PointerRID,OutputFile);
	} else {
		/* Tree query */ fflush(OutputFile);
		rid* PointerRID = c_query(RelationName, StringKey);
		/*            */
		if (PointerRID == NULL) {
#ifdef DBIO_LOG_OUTPUT
			fprintf(OutputFile,"Attempted to delete record with key \"%s\", but it was not found.\n", StringKey);
#endif
			return 1; // key not found
		}
		/* Tree delete entry */
		c_delete_from_tree(RelationName,StringKey);
		/*                   */
#ifdef DBIO_LOG_OUTPUT
		fprintf(OutputFile,"Deleting record with key \"%s\".\n", StringKey);
#endif
		RelStr.DeleteRecord(*PointerRID,OutputFile);
	}
	return 1;
}

int ParseInstructionScan(char* Buffer, RelationInteger& RelInt, RelationString& RelStr, FILE* OutputFile) {

	char RelationName[REL_NAME_LEN];

	char* Current = Buffer + 4;
	// Check it's actually "Scan"
	if (strncmp(Buffer,"Scan",4)!=0) return -8; // Not Scan
	SkipWhiteSpaces(Current);

	// Read RelationName
	int i;
	for (i = 0; i < 256 && *Current != ' ' && *Current != ',' && *Current != '\n' ;i++) {
		RelationName[i] = *Current++;
	}
	if (i == 256) return -2; // bad relnamelen
	if (i == 0) return -3; // empty relname
	RelationName[i] = '\0';
	SkipWhiteSpaces(Current);

	// Check for '\n'
	if (*Current != '\n') return -7; // trailing

	// match the relation-name read to either of our relations,
	if (RelInt.IsInitialized() && strcmp(RelationName,RelInt.GetName()) == 0) ; // Matched RelInt
	else if (RelStr.IsInitialized() && strcmp(RelationName,RelStr.GetName()) == 0) ; // Matched RelStr
	else return -4; // relname mismatch

	// Output index node/leaf count
	/* Get tree info */
	int NumberOfLeafPages = 0;
	int NumberOfNonleafIndexPages = 0;
	scan(RelationName, &NumberOfLeafPages, &NumberOfNonleafIndexPages);
	/*               */
	fprintf(OutputFile, "Index information of relation \"%s\": %d leaf pages, %d total index pages.\n", RelationName,NumberOfLeafPages,NumberOfLeafPages+NumberOfNonleafIndexPages);

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
	if (i == 256) return -2; // bad relnamelen
	if (i == 0) return -3; // empty relname
	RelationName[i] = '\0';
	SkipWhiteSpaces(Current);

	// match the relation-name read to either of our relations,
	bool IsIntRelation;
	if (RelInt.IsInitialized() && strcmp(RelationName,RelInt.GetName()) == 0) IsIntRelation = true; // Matched RelInt
	else if (RelStr.IsInitialized() && strcmp(RelationName,RelStr.GetName()) == 0) IsIntRelation = false; // Matched RelStr
	else return -4; // relname mismatch

	// Read Key1
	if(IsIntRelation) {
		IntegerKey1 = parseInt(Current);
		if (IntegerKey1 == -1) return -5; // bad integer1
	} else {
		if (*Current != '\"') return -6; // bad dquote
		++Current;
		strncpy(StringKey1,Current,10);
		while(*Current != '\n' && *Current != '\"' ) Current++;
		if (*Current != '\"') return -6; // bad dquote
		++Current;
	}
	SkipWhiteSpaces(Current);

	if (*Current == '\n') {
		if(IsIntRelation) {
			/* RID = tree query */
			rid* PointerRID = i_query(RelationName,IntegerKey1);
			/*                  */
			if (PointerRID == NULL) {
				fprintf(OutputFile,"Error: Attempted to retrieve record with key %d, but it was not found.\n",IntegerKey1);
			} else {
				RelInt.GetRecord(*PointerRID,OutputFile);
			}
			return 1;
		} else {
			/* RID = tree query */
			rid* PointerRID = c_query(RelationName,StringKey1);
			/*                  */
			if (PointerRID == NULL) {
				fprintf(OutputFile,"Error: Attempted to retrieve record with key \"%s\", but it was not found.\n",StringKey1);
			} else {
				RelStr.GetRecord(*PointerRID,OutputFile);
			}
			return 1;
		}
	}

	// Read Key2 if not '\n'
	if(IsIntRelation) {
		IntegerKey2 = parseInt(Current);
		if (IntegerKey2 == -1) return -8; // bad integer2
	} else {
		if (*Current != '\"') return -6; // bad dquote
		++Current;
		strncpy(StringKey2,Current,10);
		while(*Current != '\n' && *Current != '\"' ) Current++;
		if (*Current != '\"') return -6; // bad dquote
		++Current;
	}
	SkipWhiteSpaces(Current);

	if (*Current == '\n') {
		if(IsIntRelation) {

			/* tree ranged query */
			vector<rid*> PointersRID = i_range_query(RelationName, IntegerKey1, IntegerKey2);
			/*                   */

			size_t n = PointersRID.size();
			if (n == 0) {
				fprintf(OutputFile,"No records with key in [%d,%d] were found.\n", IntegerKey1, IntegerKey2);
			} else {
				for (size_t i = 0; i < n; i++) {
					RelInt.GetRecord(*PointersRID.at(i),OutputFile);
				}
			}

			fprintf(OutputFile, "> Retrieved %d record", n);
			if (n != 1) fprintf(OutputFile,"s");
			fprintf(OutputFile,".\n");

			return 3;
		} else {


			/* tree ranged query */
			vector<rid*> PointersRID = c_range_query(RelationName, StringKey1, StringKey2);
			/*                   */

			size_t n = PointersRID.size();
			if (n == 0) {
				fprintf(OutputFile,"No records with key in [\"%s\",\"%s\"] were found.\n", StringKey1, StringKey2);
			} else {
				for (size_t i = 0; i < n; i++)
					RelStr.GetRecord(*PointersRID.at(i),OutputFile);
			}
			fprintf(OutputFile, "> Retrieved %d record", n);
			if (n != 1) fprintf(OutputFile,"s");
			fprintf(OutputFile,".\n");

			return 4;
		}

	}
	return -7; // trailing
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
	if (i == 256) return -2; // bad relnamelen
	if (i == 0) return -3; // empty relname
	RelationName[i] = '\0';
	SkipWhiteSpaces(Current);

	// Read PageID
	PageID = parseInt(Current);
	if (PageID == -1) return -5; // bad integer
	SkipWhiteSpaces(Current);

	// match the relation-name read to either of our relations,
	bool IsIntRelation;
	if (RelInt.IsInitialized() && strcmp(RelationName,RelInt.GetName()) == 0) IsIntRelation = true; // Matched RelInt
	else if (RelStr.IsInitialized() && strcmp(RelationName,RelStr.GetName()) == 0) IsIntRelation = false; // Matched RelStr
	else return -4; // RelName did not match

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
	return -7;
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
	if (i == 256) return -2; // too long
	if (i == 0) return -3;
	RelationName[i] = '\0';
	SkipWhiteSpaces(Current);

	// match the relation-name read to either of our relations,
	bool IsIntRelation;
	if (RelInt.IsInitialized() && strcmp(RelationName,RelInt.GetName()) == 0) IsIntRelation = true; // Matched RelInt
	else if (RelStr.IsInitialized() && strcmp(RelationName,RelStr.GetName()) == 0) IsIntRelation = false; // Matched RelStr
	else return -4; // RelName did not match

	fprintf(OutputFile,"Displaying statistics of relation \"%s\":\n",RelationName);

	if (IsIntRelation) {
		fprintf(OutputFile,"  %d slotted pages.\n",RelInt.GetNumberOfPages());
	} else {
		fprintf(OutputFile,"  %d slotted pages.\n",RelStr.GetNumberOfPages());
	}

	/* Get tree info */
	int NumberOfLeafPages = 0;
	int NumberOfNonleafIndexPages = 0;
	scan(RelationName, &NumberOfLeafPages, &NumberOfNonleafIndexPages);
	/*               */

	fprintf(OutputFile,"  %d index pages.\n",NumberOfLeafPages+NumberOfNonleafIndexPages);

	if (*Current == '\n') return 1;
	return -7;
}

int main (int argc, char* argv[]) {

	FILE* InputFile;
	FILE* OutputFile;

	if (argc == 1) {

		InputFile = fopen("ProjectB_data.txt","r");
		OutputFile = fopen("log.txt","w");

	} else if (argc == 3) {

		InputFile = fopen(argv[1],"r");

		if (strcmp("stdout",argv[2])==0) {
			OutputFile = stdout;
		}
		else {
			OutputFile = fopen(argv[2],"w");
		}
	} else {
		fprintf(stderr,"Wrong number of arguments, expecting none (default filenames) or two (custom filenames).\n");
	}

	if (InputFile == NULL || OutputFile == NULL) {
		fprintf(stderr,"Error opening files.\n");
		exit(1);
	}

	char Buffer[BUFFER_SIZE];

	RelationInteger RelInt;
	RelationString RelStr;

	while(fgets(Buffer,sizeof(Buffer),InputFile) != NULL)
	{
//fprintf(stderr,"what happen; Buffer = %s",Buffer); //DEBUG
		// kill 0d in 0d 0a in windows environment
		{
			char *ch = strchr(Buffer,'\n');
			if (ch != Buffer && ch != NULL && *(ch-1) == 0x0d) {
				*(ch-1) = '\n';
				*ch = '\0';
			}
		}
		switch (Buffer[0]) {
			case 'R':
				switch(ParseInstructionR(Buffer,RelInt,RelStr,OutputFile)){
					case -8: fprintf(OutputFile,"Error: relation-name should not be empty.\n");break;
					case -7: fprintf(OutputFile,"Error: data-type field should match \"integer\" or \"String\" exactly (no double quotes).\n"); break;
					case -6: fprintf(OutputFile,"Error: Trailing symbols found.\n"); break;
					case -5: fprintf(OutputFile,"Error: Expected a nonnegative integer for record-length field.\n"); break;
					case -4: fprintf(OutputFile,"Error: The name is already in use.\n"); break;
					case -3: fprintf(OutputFile,"Error: The relation (integer or string) is already initialized. This program can only initialize one of each.\n"); break;
					case -2: fprintf(OutputFile,"Error: relation-name field exceeded the expected length %d.\n", REL_NAME_LEN); break;
					case -1: fprintf(OutputFile,"Error: Comma ',' expected but it was not present.\n"); break;
					default: break;
				}
				break;
			case 'I':
				switch(int N = ParseInstructionI(Buffer, RelInt, RelStr,OutputFile)){
					case -9: fprintf(OutputFile,"Error: Trailing symbols found.\n"); break;
					case -8: fprintf(OutputFile,"Error: Trailing symbols found, or incorrect usage of multiple inserts.\n"); break;
					case -7: fprintf(OutputFile,"Error: Record length exceeded the expected maximum record length %d.\n",RECORD_MAX_LEN); break;
					case -6: fprintf(OutputFile,"Error: Double quotation mark '\"' expected but it was not present.\n"); break;
					case -5: fprintf(OutputFile,"Error: Expected a nonnegative integer for key-value field.\n"); break;
					case -4: fprintf(OutputFile,"Error: relation-name did not match any relations.\n"); break;
					case -3: fprintf(OutputFile,"Error: relation-name should not be empty.\n");break;
					case -2: fprintf(OutputFile,"Error: relation-name field to be read is longer than expected length (%d).\n", REL_NAME_LEN); break;
					case -1: fprintf(OutputFile,"Error: Comma ',' expected but it was not present.\n"); break;
					default:
#ifdef DBIO_LOG_OUTPUT
						fprintf(OutputFile,"> Inserted %d record", N);
						if (N!=1) fprintf(OutputFile,"s");
						fprintf(OutputFile,".\n");
#endif
						break;
				}
				break;
			case 'D':
				switch(ParseInstructionD(Buffer, RelInt, RelStr,OutputFile)) {
					case -7: fprintf(OutputFile,"Error: Trailing symbols found.\n"); break;
					case -6: fprintf(OutputFile,"Error: Double quotation mark '\"' expected but it was not present.\n"); break;
					case -5: fprintf(OutputFile,"Error: Expected a nonnegative integer for key-value field.\n"); break;
					case -4: fprintf(OutputFile,"Error: relation-name did not match any relations specified.\n"); break;
					case -3: fprintf(OutputFile,"Error: relation-name should not be empty.\n");break;
					case -2: fprintf(OutputFile,"Error: relation-name field to be read is longer than expected length (%d).\n", REL_NAME_LEN); break;
					case -1: fprintf(OutputFile,"Error: Comma ',' expected but it was not present.\n"); break;
					default: break;
				}
				break;
			case 'S':
				switch(ParseInstructionScan(Buffer, RelInt, RelStr,OutputFile)) {
					case -7: fprintf(OutputFile,"Error: Trailing symbols found.\n"); break;
					case -4: fprintf(OutputFile,"Error: relation-name did not match any relations specified.\n"); break;
					case -3: fprintf(OutputFile,"Error: relation-name should not be empty.\n");break;
					case -2: fprintf(OutputFile,"Error: relation-name field to be read is longer than expected length (%d).\n", REL_NAME_LEN); break;
					case -8:
						char* ch;
						ch = strchr(Buffer,' '); if(ch != NULL) *ch = '\0';
						ch = strchr(Buffer,'\"'); if(ch != NULL) *ch = '\0';
						ch = strchr(Buffer,'\n'); if(ch != NULL) *ch = '\0';
						fprintf(OutputFile,"Error: No such instruction: \"%s\".\n", Buffer);
						break;
					default: break;
				}
				break;
			case 'q':
				switch(ParseInstructionLQ(Buffer, RelInt, RelStr,OutputFile)) {
					case -8: fprintf(OutputFile,"Error: Expected a nonnegative integer for key-value2 field.\n"); break;
					case -7: fprintf(OutputFile,"Error: Trailing symbols found.\n"); break;
					case -6: fprintf(OutputFile,"Error: Double quotation mark '\"' expected but it was not present.\n"); break;
					case -5: fprintf(OutputFile,"Error: Expected a nonnegative integer for key-value/key-value1 field.\n"); break;
					case -4: fprintf(OutputFile,"Error: relation-name did not match any relations specified.\n"); break;
					case -3: fprintf(OutputFile,"Error: relation-name should not be empty.\n");break;
					case -2: fprintf(OutputFile,"Error: relation-name field to be read is longer than expected length (%d).\n", REL_NAME_LEN); break;
					default: break;
				}
				break;
			case 'p':
				switch(ParseInstructionLP(Buffer,RelInt,RelStr,OutputFile)) {
					case -7: fprintf(OutputFile,"Error: Trailing symbols found.\n"); break;
					case -5: fprintf(OutputFile,"Error: Expected a nonnegative integer page-id field.\n"); break;
					case -4: fprintf(OutputFile,"Error: relation-name did not match any relations specified.\n"); break;
					case -3: fprintf(OutputFile,"Error: relation-name should not be empty.\n");break;
					case -2: fprintf(OutputFile,"Error: relation-name field to be read is longer than expected length (%d).\n", REL_NAME_LEN); break;
					default: break;
				}
				break;
			case 'c':
				switch(ParseInstructionLC(Buffer,RelInt,RelStr,OutputFile)) {
					case -7: fprintf(OutputFile,"Error: Trailing symbols found.\n"); break;
					case -4: fprintf(OutputFile,"Error: relation-name did not match any relations specified.\n"); break;
					case -3: fprintf(OutputFile,"Error: relation-name should not be empty.\n");break;
					case -2: fprintf(OutputFile,"Error: relation-name field to be read is longer than expected length (%d).\n", REL_NAME_LEN); break;
					default: break;
				}
				break;
			case '\n':
#ifdef DBIO_LOG_OUTPUT
				fprintf(OutputFile,"\n");
#endif
				break;
			case ' ':
				fprintf(OutputFile,"Error: Input should not start with an empty space.\n");
				break;
			default:
				char* ch;
				ch = strchr(Buffer,' '); if(ch != NULL) *ch = '\0';
				ch = strchr(Buffer,'\"'); if(ch != NULL) *ch = '\0';
				ch = strchr(Buffer,'\n'); if(ch != NULL) *ch = '\0';

				fprintf(OutputFile,"Error: No such instruction: \"%s\".\n", Buffer);
		}
	}
	if (ferror(InputFile) != 0) {
		fprintf(stderr,"Error reading file.\n");
	} else if (feof(InputFile) != 0){
		//fprintf(stderr,"End of file reached.\n");
	}

	fclose(InputFile);
	fclose(OutputFile);

fprintf(stderr,"\n\n\n");
//traverse(RelInt.GetName());
fprintf(stderr,"\n\n\n");
//traverse(RelStr.GetName());
	return 0;
}
