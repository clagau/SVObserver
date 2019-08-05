#ifndef STRUCTDEFINITIONS_H_
#define STRUCTDEFINITIONS_H_

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

enum SVImageFormatEnum
{
	SVImageFormatUnknown = 0,
	SVImageFormatMono8	 = 10,
	SVImageFormatRGB8888 = 35
};

struct ImageDefinitionStruct
{
	BSTR bstrDisplayName;
	long lHeight;
	long lWidth;
	SVImageFormatEnum eImageFormat;
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


#pragma pack(pop) 

#endif	// STRUCTDEFINITIONS_H_