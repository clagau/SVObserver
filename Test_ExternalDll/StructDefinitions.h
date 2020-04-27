#pragma once 

#include <WTypes.h>
#include <OAIdl.h>
#include <comutil.h>


#pragma pack( push, 1 ) 
/// The structure definition must be the same as the one in the SVObserver
struct DefinitionStructEx
{
	long vt {VT_EMPTY};
	_bstr_t Name;
	_bstr_t HelpText;
	_bstr_t Group;
	_variant_t Reseved1; // not used at this time
	_variant_t Reserved2; // not used at this time
};

struct InputValueDefinitionStructEx : public DefinitionStructEx
{
	_variant_t vDefaultValue;
};

struct ResultValueDefinitionStructEx : public DefinitionStructEx
{
	long ArraySize {20};
};

struct ResultTableDefinitionStructEx : public DefinitionStructEx
{
	long ColoumnCount {0};
	long RowCount {10};
	_variant_t ColumnNames; //smart array of bstr with names

};

enum SVImageFormatEnum
{
	SVImageFormatUnknown = 0,
	SVImageFormatMono8	 = 10,
	SVImageFormatBGR888X = 35
};

struct ImageDefinitionStruct
{
	BSTR bstrDisplayName;
	long lHeight;
	long lWidth;
	SVImageFormatEnum eImageFormat;
};


#pragma pack(pop) 
