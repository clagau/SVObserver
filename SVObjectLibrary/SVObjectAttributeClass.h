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

#ifdef SV_SHOW_INCLUDES
	#pragma message( "Including " __FILE__ )
#endif

#include <vector>
#include "SVObjectPointArrayClass.h"
#include "SVObjectScriptEnums.h"
#include "SVObjectDPointArrayClass.h"
#include "SVContainerLibrary\SVObjectArrayClassTemplate.h"

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

	SVObjectAttributeClass(SVObjectAttributeClass& orig);

	virtual ~SVObjectAttributeClass();

	SVObjectAttributeClass& operator=(SVObjectAttributeClass& rhs);

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

//******************************************************************************
//* LOG HISTORY:
//******************************************************************************
/*
$Log:   N:\PVCSarch65\ProjectFiles\archives\SVObserver_SRC\SVObjectLibrary\SVObjectAttributeClass.h_v  $
 * 
 *    Rev 1.1   01 Oct 2013 11:27:02   tbair
 * Project:  SVObserver
 * Change Request (SCR) nbr:  852
 * SCR Title:  Add Multiple Platform Support to SVObserver's Visual Studio Solution
 * Checked in by:  tBair;  Tom Bair
 * Change Description:  
 *   Add x64 platform.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.0   22 Apr 2013 16:38:34   bWalter
 * Project:  SVObserver
 * Change Request (SCR) nbr:  814
 * SCR Title:  Upgrade SVObserver to Compile Using Visual Studio 2010
 * Checked in by:  bWalter;  Ben Walter
 * Change Description:  
 *   Initial check in to SVObserver_src.  (Merged with svo_src label SVO 6.10 Beta 008.)
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.6   04 Sep 2012 13:22:44   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  602
 * SCR Title:  Revise the Toolset Parsing and Object Creation Methodology
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Added 64 bit integer type to the parsing and saving functionality.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.5   21 Feb 2012 13:20:30   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  748
 * SCR Title:  Add Remote Output Steams to SVObserver
 * Checked in by:  tBair;  Tom Bair
 * Change Description:  
 *   Updated code to fix loading issue associated with POINT.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.4   05 Nov 2010 10:05:58   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  703
 * SCR Title:  Allow SVObserver to Change Remote Input Values
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated source code to remove redundent data objects.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.3   01 Jun 2010 08:29:46   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  693
 * SCR Title:  Fix Performance Issue with Inspection Process
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated source code to remove duplicate definitions and redundunt containers.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.2   14 Jul 2005 11:44:30   ebeyeler
 * Project:  SVObserver
 * Change Request (SCR) nbr:  464
 * SCR Title:  Add array indexing for value objects
 * Checked in by:  eBeyeler;  Eric Beyeler
 * Change Description:  
 *   implemented new array parsing capability
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.1   20 Jun 2005 15:12:48   ebeyeler
 * Project:  SVObserver
 * Change Request (SCR) nbr:  464
 * SCR Title:  Add array indexing for value objects
 * Checked in by:  eBeyeler;  Eric Beyeler
 * Change Description:  
 *   templatized GetAttributeData
 * added GetData functions for char and CPoint
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.0   15 Feb 2005 14:52:50   ryoho
 * Project:  SVObserver
 * Change Request (SCR) nbr:  440
 * SCR Title:  Create Internal Tool Object to Managing Tool Extents and Result Extents
 * Checked in by:  rYoho;  Rob Yoho
 * Change Description:  
 *   First version of file for SVObjectLibrary
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.4   07 Jul 2004 08:48:00   ebeyeler
 * Project:  SVObserver
 * Change Request (SCR) nbr:  416
 * SCR Title:  Add Variant Value Object
 * Checked in by:  eBeyeler;  Eric Beyeler
 * Change Description:  
 *   added SV_OBJECT_TYPE_VARIANT
 * added #include "SVObjectArrayClassTemplate.h"
 * added GetData, GetAttributeData, SetData, AddData, msvVariantArray for Variant type
 * cleaned up clutter
 * 
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.3   22 Apr 2003 11:34:54   rschock
 * Project:  SVObserver
 * Change Request (SCR) nbr:  346
 * SCR Title:  Update SVObserver to Version 4.21 Release
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Redid the #include defines and standardized the Tracker log headers and removed warning from release mode builds.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.2   08 Nov 2001 15:00:04   Joe
 * Project:  SVObserver
 * Change Request (SCR) nbr:  255
 * SCR Title:  Add Sub-pixel resolution to Profile and Line Gage Tool Analyzers
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Added sub-pixel value array include and value array attribute.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.1   12 Jun 2001 16:21:18   ed
 * Project:  SVObserver
 * Change Request (SCR) nbr:  200
 * SCR Title:  Update code to implement SVIM COM Server
 * Checked in by:  Ed;  Ed Chobanoff
 * Change Description:  
 *   Modified the following functions: GetAttributeData.  Revised DEBUG pragma pertaining to including this file.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.0   19 Apr 2001 14:49:24   Steve
 * Project:  SVObserver
 * Change Request (SCR) nbr:  196
 * SCR Title:  Restructure Scripted Load/Save Procedures Functional Requirement
 * Checked in by:  Steve;  Stephen E. Steffan
 * Change Description:  
 *   New file due to header file changes due to include file restructuring.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
*/

