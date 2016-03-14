//******************************************************************************
//* COPYRIGHT (c) 2003 by SVResearch, Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVInputInfoListClass
//* .File Name       : $Workfile:   SVInputInfoListClass.cpp  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.0  $
//* .Check In Date   : $Date:   22 Apr 2013 16:37:28  $
//******************************************************************************

#include "stdafx.h"
#include "SVInputInfoListClass.h"

#include "SVInObjectInfoStruct.h"
#include "SVObjectLibrary.h"
#include "SVObjectManagerClass.h"

//*/*\*/*\*/*\*/*\*/*\*/*\*/*\*/*\*/*\*/*\*/*\*/*\*/*\*/*\*/*\*/*\*/*\*/*\*/*\*/
//* Class Name : SVInputInfoListClass
//* Note(s)    :
//*/*\*/*\*/*\*/*\*/*\*/*\*/*\*/*\*/*\*/*\*/*\*/*\*/*\*/*\*/*\*/*\*/*\*/*\*/*\*/

//******************************************************************************
// Adjustments
//******************************************************************************

//******************************************************************************
// Serialization:
//******************************************************************************



//******************************************************************************
// Message Map Entries
//******************************************************************************

//******************************************************************************
// Constructor(s):
//******************************************************************************

////////////////////////////////////////////////////////////////////////////////
// .Title       : SVInputInfoListClass
// -----------------------------------------------------------------------------
// .Description : This constructor inits ...
//              :
// -----------------------------------------------------------------------------
// .Import Function Reference(s)
//	:
// -----------------------------------------------------------------------------
// .Import Variable Reference(s)
//	:
////////////////////////////////////////////////////////////////////////////////
// .History
//	 Date		Author		Comment                                       
//  :07.01.1999 RO			First Implementation
////////////////////////////////////////////////////////////////////////////////
SVInputInfoListClass::SVInputInfoListClass()
{
}

//******************************************************************************
// Destructor(s):
//******************************************************************************
SVInputInfoListClass::~SVInputInfoListClass()
{
}

//******************************************************************************
// Operator(s):
//******************************************************************************
BOOL SVInputInfoListClass::CheckExistence( int Index /*= -1*/ )
{
	BOOL BRetVal = FALSE;
	if( Index < GetSize() && Index >= -1 )
	{
		if( Index > -1 )
		{
			// Check only this entry...
			if( GetAt( Index ) )
				BRetVal = ( ( GetAt( Index )->PObject = SVObjectManagerClass::Instance().GetObject( GetAt( Index )->UniqueObjectID ) ) != NULL ) ? TRUE : FALSE;
		}
		else
		{
			// Check all entries...
			BRetVal = TRUE;
			for( int i = 0; i < GetSize(); ++i )
				if( GetAt( i ) )
					BRetVal = ( ( ( GetAt( i )->PObject = SVObjectManagerClass::Instance().GetObject( GetAt( i )->UniqueObjectID ) ) != NULL ) ? TRUE : FALSE ) && BRetVal;
				else
					BRetVal = FALSE;
		}
	}
	return BRetVal;
}

////////////////////////////////////////////////////////////////////////////////
// Override for Add - Do not allow duplicates
////////////////////////////////////////////////////////////////////////////////
int SVInputInfoListClass::Add( SVInObjectInfoStruct* PInObjectInfo )
{
	if( !checkDuplicates( PInObjectInfo ) )
		return SVVector< SVInObjectInfoStruct* >::Add( PInObjectInfo );

	return GetSize();
}

////////////////////////////////////////////////////////////////////////////////
// Override for InsertAt - Do not allow duplicates,
// never more than one item inserted
////////////////////////////////////////////////////////////////////////////////
void SVInputInfoListClass::InsertAt( int nIndex, SVInObjectInfoStruct* PInObjectInfo )
{
	if( !checkDuplicates( PInObjectInfo ) )
		SVVector< SVInObjectInfoStruct* >::InsertAt( nIndex, PInObjectInfo );
}

////////////////////////////////////////////////////////////////////////////////
// check for duplicates
////////////////////////////////////////////////////////////////////////////////
bool SVInputInfoListClass::checkDuplicates( SVInObjectInfoStruct* PInObjectInfo )
{
	bool duplicateFound = false;

	SVInObjectInfoStruct* pInObjectInfo;

	for( int i = 0;i < GetSize() && !duplicateFound;i++ )
	{
		pInObjectInfo = GetAt( i );
		if( pInObjectInfo )
		{
			if( pInObjectInfo == PInObjectInfo )
				duplicateFound = true;
		}
	}
	return duplicateFound;
}

