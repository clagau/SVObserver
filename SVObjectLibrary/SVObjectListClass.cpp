//******************************************************************************
//* COPYRIGHT (c) 2003 by SVResearch, Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVObjectListClass
//* .File Name       : $Workfile:   SVObjectListClass.cpp  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.0  $
//* .Check In Date   : $Date:   22 Apr 2013 16:57:16  $
//******************************************************************************

//******************************************************************************
//* INCLUDE(S):
//******************************************************************************

#include "stdafx.h"
#include "SVObjectListClass.h"

#include "SVObjectClass.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif


SVObjectListClass::SVObjectListClass()
{
	SetSize( 0 );
}

SVObjectListClass::~SVObjectListClass()
{
    //int nCount = GetSize();           23 Jul 1999 - frb - don't mess with this.
	//for (int i=0;i<nCount;i++)
	//{
	//	SVObjectClass* pObject = GetAt(i);
	//	delete pObject;
	//}
	RemoveAll();
}

BOOL SVObjectListClass::InitObjectAt( int Index, SVObjectClass* PObject )
{
	if( Index >= 0 && Index < GetSize() && GetAt( Index ).Object() )
		return GetAt( Index )->InitObject( PObject );

	return FALSE;
}

void SVObjectListClass::ResetNameAt( int Index )
{
	if( Index >= 0 && Index < GetSize() && GetAt( Index ).Object() )
		GetAt( Index )->ResetName();
}

void SVObjectListClass::SetNameAt( int Index, const CString& STRString )
{
	if( Index >= 0 && Index < GetSize() && GetAt( Index ).Object() )
		GetAt( Index )->SetName( STRString );
}

void SVObjectListClass::SetObjectOwnerAt( int Index, SVObjectClass* PNewOwner )
{
	if( Index >= 0 && Index < GetSize() && GetAt( Index ).Object() )
		GetAt( Index )->SetObjectOwner( PNewOwner );
}

CString SVObjectListClass::GetNameAt( int Index )
{
	if( Index >= 0 && Index < GetSize() && GetAt( Index ).Object() )
		return GetAt( Index )->GetName();

	return _T( "" );
}

int SVObjectListClass::GetNameLengthAt( int Index )
{
	if( Index >= 0 && Index < GetSize() && GetAt( Index ).Object() )
		return GetAt( Index )->GetNameLength();

	return 0;
}

CString SVObjectListClass::GetObjectNameAt( int Index )
{
	if( Index >= 0 && Index < GetSize() && GetAt( Index ).Object() )
		return GetAt( Index )->GetObjectName();

	return _T( "" );
}

int SVObjectListClass::GetObjectNameLengthAt( int Index )
{
	if( Index >= 0 && Index < GetSize() && GetAt( Index ).Object() )
		return GetAt( Index )->GetObjectNameLength();

	return 0;
}

const SVObjectInfoStruct& SVObjectListClass::GetObjectOwnerAt( int Index )
{
	if( Index >= 0 && Index < GetSize() && GetAt( Index ).Object() )
		return GetAt( Index )->GetOwnerInfo();

	return SVInvalidObjectInfo;
}

CString SVObjectListClass::GetCompleteObjectNameAt( int Index )
{
	if( Index >= 0 && Index < GetSize() && GetAt( Index ).Object() )
		return GetAt( Index )->GetCompleteObjectName();

	return _T( "" );
}

int SVObjectListClass::GetResourceIDAt( int Index )
{
	if( Index >= 0 && Index < GetSize() && GetAt( Index ).Object() )
		return GetAt( Index )->GetResourceID();

	return 0;
}

const GUID& SVObjectListClass::GetUniqueObjectIDAt( int Index ) const
{
	if( Index >= 0 && Index < GetSize() && GetAt( Index ).Object() )
		return GetAt( Index )->GetUniqueObjectID();

	return SVInvalidGUID;
}

