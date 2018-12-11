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
#include "SVObjectScriptEnums.h"
#include "SVContainerLibrary/SVObjectArrayClassTemplate.h"
#include "SVUtilityLibrary/SVPoint.h"
#pragma endregion Includes

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

	SVObjectAttributeClass& operator=(const SVObjectAttributeClass& rRhs);

	LPCTSTR GetName() const;
	SVObjectScriptDataObjectTypeEnum GetType() const;

	typedef std::vector<SVPoint<long>> SVObjectSVPointArrayClass;
	typedef std::vector<SVPoint<double>> SVObjectDPointArrayClass;

	//These functions return the configuration object attribute data.
	bool GetData(SvCl::SVObjectBoolArrayClass& rData);
	bool GetData(SvCl::SVObjectByteArrayClass& rData);
	bool GetData(SvCl::SVObjectCharArrayClass& rData);
	bool GetData(SvCl::SVObjectStdStringArrayClass& rData);
	bool GetData(SvCl::SVObjectDoubleArrayClass& rData);
	bool GetData(SvCl::SVObjectDWordArrayClass& rData);
	bool GetData(SvCl::SVObjectLongArrayClass& rData);
	bool GetData(SVObjectSVPointArrayClass& svData);
	bool GetData(SVObjectDPointArrayClass& rData);
	bool GetData(SvCl::SVObjectVariantArrayClass& rData);
	bool GetData(SvCl::SVObjectInt64ArrayClass& rData);

	// this function handles old-style configs
	template <typename T>	
	bool GetAttributeData(LPCTSTR szName, std::vector< std::vector<T> >& raaData, T defaultValue)
	{// VC6 compiler limitation: code must be here (inside the class definition)
		std::vector<T> svTempArray;
		bool bOk = GetAttributeData( szName, svTempArray );
		if ( bOk )
		{
			int iSourceDataSize = static_cast<int> (svTempArray.size());
			int iNewBucketSize = __max(iSourceDataSize, 2);
			int iOldBucketSize = static_cast<int> (raaData.size());
			int iArraySize = 1;
			if ( 0 < iOldBucketSize )
			{
				iArraySize = static_cast<int> ( raaData[0].size() );
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
		std::vector<T> svTempArray;
		bool bOk = GetAttributeData( szName, svTempArray );
		if ( bOk )
		{
			int iSourceDataSize = static_cast<int> (svTempArray.size());

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
	bool GetAttributeData(LPCTSTR szName, std::vector<T>& rData)
	{
		bool bOk = (m_Name == szName );

		if ( bOk )
		{
			bOk = GetData( rData );
		}

		return bOk;
	}

	//This operator sets the internal name of the 
	//configuration object attribute.  This operator also 
	//resets the other internal data.
	void SetName(LPCTSTR szName);

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
	bool AddData(SVPoint<double> Value);
	bool AddData(VARIANT Value);
	bool AddData(__int64 Value);

private:
	//This attribute is responsible for maintaining the 
	//configuration object attribute name.
	std::string m_Name;

	//This attribute is responsible for maintaining the 
	//configuration object attribute type.
	SVObjectScriptDataObjectTypeEnum m_Type;

	SvCl::SVObjectBoolArrayClass m_BoolArray;
	SvCl::SVObjectStdStringArrayClass m_StringArray;
	SvCl::SVObjectDoubleArrayClass m_DoubleArray;
	SvCl::SVObjectDWordArrayClass m_DWordArray;
	SvCl::SVObjectLongArrayClass m_LongArray;
	SvCl::SVObjectByteArrayClass m_ByteArray;
	SVObjectSVPointArrayClass m_PointArray;
	SVObjectDPointArrayClass m_DPointArray;
	SvCl::SVObjectVariantArrayClass m_VariantArray;
	SvCl::SVObjectInt64ArrayClass m_Int64Array;
};

