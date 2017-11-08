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
#include "SVContainerLibrary/SVObjectArrayClassTemplate.h"
#pragma endregion Includes

enum SV_OBJECT_TYPE_ENUM
{
	SV_OBJECT_TYPE_UNKNOWN = 0,
	SV_OBJECT_TYPE_BOOL    = 1,
	SV_OBJECT_TYPE_BYTE    = 2,
	SV_OBJECT_TYPE_STRING  = 3,
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

	SVObjectAttributeClass( const SVObjectAttributeClass& rRhs );

	virtual ~SVObjectAttributeClass();

	SVObjectAttributeClass& operator=(const SVObjectAttributeClass& rRhs); // @WARNING Should be const & - See Implementation

	LPCTSTR GetName() const;
	int GetType() const;

	SVObjectScriptDataObjectTypeEnum GetSVObjectScriptDataObjectTypeEnum();

	//These functions return the configuration object attribute data.
	bool GetData(SvCl::SVObjectBoolArrayClass& svData);
	bool GetData(SvCl::SVObjectByteArrayClass& svData);
	bool GetData(SvCl::SVObjectArrayClassTemplate<char>& svData);	// added to allow templates to use this code more easily
	bool GetData(SvCl::SVObjectSVStringArrayClass& svData);	// changed to allow templates to use this code more easily
	bool GetData(SvCl::SVObjectDoubleArrayClass& svData);
	bool GetData(SvCl::SVObjectDWordArrayClass& svData);
	bool GetData(SvCl::SVObjectLongArrayClass& svData);
	bool GetData(SVObjectSVPointArrayClass& svData);
	bool GetData(SVObjectDPointArrayClass& svData);
	bool GetData(SvCl::SVObjectVariantArrayClass& svData);
	bool GetData(SvCl::SVObjectInt64ArrayClass& svData);

	// this function handles old-style configs
	template <typename T>	
	bool GetAttributeData(LPCTSTR szName, std::vector< std::vector<T> >& raaData, T defaultValue)
	{// VC6 compiler limitation: code must be here (inside the class definition)
		SvCl::SVObjectArrayClassTemplate<T> svTempArray;
		bool bOk = GetAttributeData( szName, svTempArray );
		if ( bOk )
		{
			int iSourceDataSize = svTempArray.GetSize();
			int iNewBucketSize = __max(iSourceDataSize, 2);
			int iOldBucketSize = static_cast< int >( raaData.size() );
			int iArraySize = 1;
			if ( 0 < iOldBucketSize )
			{
				iArraySize = static_cast< int >( raaData[0].size() );
			}

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

	//// new code to load values into array positions in a single bucket.
	template <typename T>	
	bool GetArrayData(LPCTSTR szName, std::vector<T>& raData, T defaultValue)
	{// VC6 compiler limitation: code must be here (inside the class definition)
		SvCl::SVObjectArrayClassTemplate<T> svTempArray;
		bool bOk = GetAttributeData( szName, svTempArray );
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
	bool GetAttributeData(LPCTSTR szName, SvCl::SVObjectArrayClassTemplate<T>& svData)
	{
		bool bOk = (m_Name == szName );

		if ( bOk )
		{
			bOk = GetData( svData );
		}

		return bOk;
	}

	//This operator sets the internal name of the 
	//configuration object attribute.  This operator also 
	//resets the other internal data.
	void SetName(LPCTSTR szName);

	//This operator sets the internal type of the 
	//configuration object attribute.
	bool SetType(int iType);

	//This operator sets the internal type of the 
	//configuration object attribute.
	bool SetType(SVObjectScriptDataObjectTypeEnum eType);

	bool AddData(BOOL Value);
	bool AddData(BYTE Value);
	bool AddData(LPCTSTR Value);
	bool AddData(double Value);
	bool AddData(DWORD Value);
	bool AddData(long Value);
	bool AddData(POINT Value);
	bool AddData(SVDPointClass Value);
	bool AddData(VARIANT Value);
	bool AddData(__int64 Value);

private:
	//This attribute is responsible for maintaining the 
	//configuration object attribute name.
	std::string m_Name;

	//This attribute is responsible for maintaining the 
	//configuration object attribute type.
	int miType;

	SvCl::SVObjectBoolArrayClass m_BoolArray;
	SvCl::SVObjectSVStringArrayClass m_StringArray;
	SvCl::SVObjectDoubleArrayClass m_DoubleArray;
	SvCl::SVObjectDWordArrayClass m_DWordArray;
	SvCl::SVObjectLongArrayClass m_LongArray;
	SVObjectSVPointArrayClass m_PointArray;
	SvCl::SVObjectByteArrayClass m_ByteArray;
	SVObjectDPointArrayClass m_DPointArray;
	SvCl::SVObjectVariantArrayClass m_VariantArray;
	SvCl::SVObjectInt64ArrayClass m_Int64Array;
};

