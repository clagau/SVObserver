//******************************************************************************
//* COPYRIGHT (c) 2003 by SVResearch, Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVAcquisitionSource
//* .File Name       : $Workfile:   SVAcquisitionSource.cpp  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.0  $
//* .Check In Date   : $Date:   23 Apr 2013 09:26:28  $
//******************************************************************************

//******************************************************************************
//* INCLUDE(S):
//******************************************************************************

#include "stdafx.h"
#include "SVAcquisitionSource.h"

////////////////////////////////////////////////////////////////////////////////
// General Include File(s)
////////////////////////////////////////////////////////////////////////////////

#include "SVFileNameManagerClass.h"

////////////////////////////////////////////////////////////////////////////////
// Other Necessary Include File(s) - Module Link(s)
////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////
// Prototyping
////////////////////////////////////////////////////////////////////////////////


//******************************************************************************
//* DEFINITIONS OF MODULE-LOCAL VARIABLES:
//******************************************************************************

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//******************************************************************************
//* CLASS METHOD IMPLEMENTATION(S):
//******************************************************************************


//*/*\*/*\*/*\*/*\*/*\*/*\*/*\*/*\*/*\*/*\*/*\*/*\*/*\*/*\*/*\*/*\*/*\*/*\*/*\*/
//* Class Name : SVAcquisitionSourceClass
//* Note(s)    : 
//*/*\*/*\*/*\*/*\*/*\*/*\*/*\*/*\*/*\*/*\*/*\*/*\*/*\*/*\*/*\*/*\*/*\*/*\*/*\*/

SVAcquisitionSourceClass::SVAcquisitionSourceClass()
{
	AcquisitionType = AcNone;
	AcquisitionTriggerType = 0;
}

SVAcquisitionSourceClass::SVAcquisitionSourceClass( const SVAcquisitionSourceClass& svASC )
{
	*this = svASC;
}

SVAcquisitionSourceClass::SVAcquisitionSourceClass( int iAcqType, 
                                                    int iAcqTrigType, 
                                                    const SVFileNameArrayClass& svFileNameArray, 
                                                    const SVImageInfoClass& svImageInfo, 
                                                    const SVLightReference& svLightRef )
{
	AcquisitionType = iAcqType;
	AcquisitionTriggerType = iAcqTrigType;
	AcquisitionImageInfo = svImageInfo;
	AcquisitionLightReferenceArray	= svLightRef;

	SetFileNameArray( svFileNameArray );
}

SVAcquisitionSourceClass::SVAcquisitionSourceClass( int iAcqType, 
                                                    int iAcqTrigType, 
                                                    LPCTSTR szFileNameList, 
                                                    const SVImageInfoClass& svImageInfo, 
                                                    const SVLightReference& svLightRef )
{
	AcquisitionType = iAcqType;
	AcquisitionTriggerType = iAcqTrigType;
	AcquisitionImageInfo = svImageInfo;
	AcquisitionLightReferenceArray	= svLightRef;

	SetFileNameArray( szFileNameList );
}

SVAcquisitionSourceClass::~SVAcquisitionSourceClass()
{
	SVFileNameManagerClass svFileNameManager;

	for ( int i = 0; i < msvFileNameArray.GetSize(); i++ )
	{
		svFileNameManager.RemoveItem( &(msvFileNameArray.ElementAt(i)) );
	}
}

const SVAcquisitionSourceClass& SVAcquisitionSourceClass::operator=(const SVAcquisitionSourceClass& svASC)
{
	AcquisitionType = svASC.AcquisitionType;
	AcquisitionTriggerType = svASC.AcquisitionTriggerType;
	AcquisitionImageInfo = svASC.AcquisitionImageInfo;
	AcquisitionLightReferenceArray = svASC.AcquisitionLightReferenceArray;

	SetFileNameArray( svASC.msvFileNameArray );

	return *this;
}

SVFileNameArrayClass SVAcquisitionSourceClass::GetFileNameArray()
{
	return msvFileNameArray;
}

BOOL SVAcquisitionSourceClass::SetFileNameArray(LPCTSTR szFileNameList)
{
	BOOL bOk = TRUE;

	int i( 0 );

	SVFileNameManagerClass svFileNameManager;

	for ( i = 0; i < msvFileNameArray.GetSize(); i++ )
	{
		svFileNameManager.RemoveItem( &(msvFileNameArray.ElementAt(i)) );
	}

	msvFileNameArray.SetFileNameList( szFileNameList );

	for ( i = 0; i < msvFileNameArray.GetSize(); i++ )
	{
		svFileNameManager.AddItem( &(msvFileNameArray.ElementAt(i)) );

		CString csFullFileName = msvFileNameArray.ElementAt(i).GetFullFileName();
	}

	return bOk;
}

BOOL SVAcquisitionSourceClass::SetFileNameArray(const SVFileNameArrayClass& svFileNameArray)
{
	BOOL bOk = TRUE;

	int i( 0 );

	SVFileNameManagerClass svFileNameManager;

	for ( i = 0; i < msvFileNameArray.GetSize(); i++ )
	{
		svFileNameManager.RemoveItem( &(msvFileNameArray.ElementAt(i)) );
	}

	msvFileNameArray = svFileNameArray;

	for ( i = 0; i < msvFileNameArray.GetSize(); i++ )
	{
		svFileNameManager.AddItem( &(msvFileNameArray.ElementAt(i)) );
	}

	return bOk;
}


//*/*\*/*\*/*\*/*\*/*\*/*\*/*\*/*\*/*\*/*\*/*\*/*\*/*\*/*\*/*\*/*\*/*\*/*\*/*\*/
//* Class Name : SVAcquisitionSourceListClass
//* Note(s)    : 
//*/*\*/*\*/*\*/*\*/*\*/*\*/*\*/*\*/*\*/*\*/*\*/*\*/*\*/*\*/*\*/*\*/*\*/*\*/*\*/


//******************************************************************************
// Constructor(s):
//******************************************************************************
SVAcquisitionSourceListClass::SVAcquisitionSourceListClass() 
{
}

SVAcquisitionSourceListClass::SVAcquisitionSourceListClass(const SVAcquisitionSourceListClass& svASLC )
{
	*this = svASLC;
}

//******************************************************************************
// Destructor(s):
//******************************************************************************
SVAcquisitionSourceListClass::~SVAcquisitionSourceListClass() 
{
	DeleteAll();
}

//******************************************************************************
// Operator(s):
//******************************************************************************

const SVAcquisitionSourceListClass& SVAcquisitionSourceListClass::operator=(const SVAcquisitionSourceListClass& svASLC)
{
	DeleteAll();

	for( int i = 0; i < svASLC.GetSize(); i++ )
	{
		if( svASLC[i] )
		{
			Add( new SVAcquisitionSourceClass( *(svASLC[i]) ) );
		}
	}

	return *this;
}

////////////////////////////////////////////////////////////////////////////////
// Destroy Operator
////////////////////////////////////////////////////////////////////////////////
void SVAcquisitionSourceListClass::DeleteAll() 
{
	for( int i = GetSize() - 1; i >= 0; -- i )
		if( GetAt( i ) )
			delete( GetAt( i ) );

	RemoveAll();
}






//** EOF **

