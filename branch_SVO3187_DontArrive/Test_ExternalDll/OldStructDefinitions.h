// OldStructDefinitions.h contains depreciated struct definition, which are necessary 
// to build dll for SVObserver Versions < 10.0
#pragma once
#include <WTypes.h>
#include <OAIdl.h>
#include <comutil.h>

#pragma pack( push, 1 ) 

struct InputValueDefinitionStruct
{
	long lVT;
	BSTR bstrDisplayName;
	BSTR bstrHelpText;
	BSTR bstrGroup;
	_variant_t vDefaultValue;
};

struct ResultValueDefinitionStruct
{
	long lVT;
	BSTR bstrDisplayName;	// not used at this time
};


struct ResultTableDefinitionStruct
{
	long lVT {VT_EMPTY};
	_bstr_t bstrDisplayName;	// not used at this time
	DWORD type {0}; 	// not used at this time
	long ColoumnCount {0};
	long RowCount {0};
	_variant_t ColumnNames; //smart array of bstr with names
	
};

struct ResultValueDefinitionStructAd
{
	DWORD type {0}; 	// not used at this time
	long MaxArraylen {20}; 
};

#pragma pack(pop) 

