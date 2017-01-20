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

#pragma region Includes
#include "stdafx.h"
#include "SVInputInfoListClass.h"

#include "SVInObjectInfoStruct.h"
#include "SVObjectLibrary.h"
#include "SVObjectManagerClass.h"
#pragma endregion Includes

#pragma region Declarations
#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif
#pragma endregion Declarations

SVInputInfoListClass::SVInputInfoListClass()
{
}

SVInputInfoListClass::~SVInputInfoListClass()
{
}

BOOL SVInputInfoListClass::CheckExistence( int Index /*= -1*/ )
{
	BOOL BRetVal = FALSE;
	if( Index < GetSize() && Index >= -1 )
	{
		if( Index > -1 )
		{
			// Check only this entry...
			if( GetAt( Index ) )
			{
				BRetVal = ( nullptr != ( GetAt( Index )->PObject = SVObjectManagerClass::Instance().GetObject( GetAt( Index )->UniqueObjectID ) ) ) ? true : false;
			}
		}
		else
		{
			// Check all entries...
			BRetVal = TRUE;
			for( int i = 0; i < GetSize(); ++i )
			{
				if( GetAt( i ) )
				{
					BRetVal = ( ( nullptr != ( GetAt( i )->PObject = SVObjectManagerClass::Instance().GetObject( GetAt( i )->UniqueObjectID ) ) ) ? true : false ) && BRetVal;
				}
				else
				{
					BRetVal = false;
				}
			}
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

