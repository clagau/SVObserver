//******************************************************************************
//* COPYRIGHT (c) 2003 by SVResearch, Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVObjectAttributeClass
//* .File Name       : $Workfile:   SVObjectAttributeClass.h  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.1  $
//* .Check In Date   : $Date:   01 Oct 2013 11:27:02  $
//******************************************************************************

#pragma once

#pragma region Includes
//Moved to precompiled header: #include <vector>
#include "SVObjectPointArrayClass.h"
#include "SVObjectScriptEnums.h"
#include "SVObjectDPointArrayClass.h"
#include "SVContainerLibrary\SVObjectArrayClassTemplate.h"
#pragma endregion Includes

enum SV_OBJECT_TYPE_ENUM
{
	SV_OBJECT_TYPE_UNKNOWN = 0,
	SV_OBJECT_TYPE_BOOL    = 1,
	SV_OBJECT_TYPE_BYTE    = 2,
	SV_OBJECT_TYPE_CSTRING = 3,
	SV_OBJECT_TYPE_DOUBLE  = 4,
	SV_OBJECT_TYPE_DWORD   = 5,
	SV_OBJECT_TYPE_LONG    = 6,
	SV_OBJECT_TYPE_POINT   = 7,
	SV_OBJECT_TYPE_DPOINT  = 8,
	SV_OBJECT_TYPE_VARIANT = 9,
	SV_OBJECT_TYPE_INT64   = 10,
};

//This class contains the information about one of the 
//attributes of a define object in the configuration file.  
//This class will perform simple data conversion from one 
//data type to another.
class SVObjectAttributeClass 
{
public:
	SVObjectAttributeClass();

	SVObjectAttributeClass(SVObjectAttributeClass& orig); // @WARNING Should be const & - See Implementation

	virtual ~SVObjectAttributeClass();

	SVObjectAttributeClass& operator=(SVObjectAttributeClass& rhs); // @WARNING Should be const & - See Implementation

	LPCTSTR GetName();
	int GetType();

	SVObjectScriptDataObjectTypeEnum GetSVObjectScriptDataObjectTypeEnum();

	//These functions return the configuration object attribute data.
	BOOL GetData(SvCl::SVObjectBoolArrayClass& svData);
	BOOL GetData(SvCl::SVObjectByteArrayClass& svData);
	BOOL GetData(SvCl::SVObjectArrayClassTemplate<char>& svData);	// added to allow templates to use this code more easily
	BOOL GetData(SvCl::SVObjectCStringArrayClass& svData);	// changed to allow templates to use this code more easily
	BOOL GetData(SvCl::SVObjectArrayClassTemplate<SVString>& svData);	// changed to allow templates to use this code more easily
	BOOL GetData(SvCl::SVObjectDoubleArrayClass& svData);
	BOOL GetData(SvCl::SVObjectDWordArrayClass& svData);
	BOOL GetData(SvCl::SVObjectLongArrayClass& svData);
	BOOL GetData(SVObjectSVPointArrayClass& svData);
	BOOL GetData(SVObjectDPointArrayClass& svData);
	BOOL GetData(SvCl::SVObjectVariantArrayClass& svData);
	BOOL GetData(SvCl::SVObjectInt64ArrayClass& svData);

	// this function handles old-style configs
	template <typename T>	
	BOOL GetAttributeData(LPCTSTR szName, std::vector< std::vector<T> >& raaData, T defaultValue)
	{// VC6 compiler limitation: code must be here (inside the class definition)
		BOOL bOk = FALSE;
		SvCl::SVObjectArrayClassTemplate<T> svTempArray;
		bOk = GetAttributeData( szName, svTempArray );
		if ( bOk )
		{
			int iSourceDataSize = svTempArray.GetSize();
			int iNewBucketSize = __max(iSourceDataSize, 2);
			int iOldBucketSize = static_cast< int >( raaData.size() );
			int iArraySize = 1;
			if ( iOldBucketSize > 0 )
				iArraySize = static_cast< int >( raaData[0].size() );

			std::vector<T> aArrayData(iArraySize, defaultValue);
			std::vector< std::vector<T> > aaNewData(iNewBucketSize, aArrayData);

			// copy data
			for ( int i = 0; i < iSourceDataSize; i++ )
			{
				aaNewData[i][0] = svTempArray[i];
			}

			// move new to original
			raaData.swap(aaNewData);
		}
		return bOk;
	}

	// new code to load values into array positions in a single bucket.
	template <typename T>	
	BOOL GetArrayData(LPCTSTR szName, std::vector<T>& raData, T defaultValue)
	{// VC6 compiler limitation: code must be here (inside the class definition)
		BOOL bOk = FALSE;
		SvCl::SVObjectArrayClassTemplate<T> svTempArray;
		bOk = GetAttributeData( szName, svTempArray );
		if ( bOk )
		{
			int iSourceDataSize = svTempArray.GetSize();

			if ( iSourceDataSize > 0 )
			{
				raData.resize( iSourceDataSize );

				// copy data
				for ( int i = 0; i < iSourceDataSize; i++ )
				{
					raData[i] = svTempArray[i];
				}
			}
		}
		return bOk;
	}

	template <typename T>
	BOOL GetAttributeData(LPCTSTR szName, SvCl::SVObjectArrayClassTemplate<T>& svData)
	{
		BOOL bOk = mcsName.Compare( szName ) == 0;

		if ( bOk )
		{
			bOk = GetData( svData );
		}

		return bOk;
	}

	//This operator sets the internal name of the 
	//configuration object attribute.  This operator also 
	//resets the other internal data.
	BOOL SetName(LPCTSTR szName);

	//This operator sets the internal type of the 
	//configuration object attribute.
	BOOL SetType(int iType);

	//This operator sets the internal type of the 
	//configuration object attribute.
	BOOL SetType(SVObjectScriptDataObjectTypeEnum eType);

	BOOL AddData(BOOL Value);
	BOOL AddData(BYTE Value);
	BOOL AddData(LPCTSTR Value);
	BOOL AddData(double Value);
	BOOL AddData(DWORD Value);
	BOOL AddData(long Value);
	BOOL AddData(POINT Value);
	BOOL AddData(SVDPointClass Value);
	BOOL AddData(VARIANT Value);
	BOOL AddData(__int64 Value);

private:
	//This attribute is responsible for maintaining the 
	//configuration object attribute name.
	CString mcsName;

	//This attribute is responsible for maintaining the 
	//configuration object attribute type.
	int miType;

	SvCl::SVObjectBoolArrayClass msvBoolArray;
	SvCl::SVObjectCStringArrayClass msvCStringArray;
	SvCl::SVObjectDoubleArrayClass msvDoubleArray;
	SvCl::SVObjectDWordArrayClass msvDWordArray;
	SvCl::SVObjectLongArrayClass msvLongArray;
	SVObjectSVPointArrayClass msvPointArray;
	SvCl::SVObjectByteArrayClass msvByteArray;
	SVObjectDPointArrayClass msvDPointArray;
	SvCl::SVObjectVariantArrayClass msvVariantArray;
	SvCl::SVObjectInt64ArrayClass msvInt64Array;
};

