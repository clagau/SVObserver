//******************************************************************************
//* COPYRIGHT (c) 2003 by SVResearch, Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVFileNameArrayClass
//* .File Name       : $Workfile:   SVFileNameArrayClass.cpp  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.0  $
//* .Check In Date   : $Date:   23 Apr 2013 10:36:20  $
//******************************************************************************

#include "stdafx.h"
#include "SVFileNameArrayClass.h"

SVFileNameArrayClass::SVFileNameArrayClass()
{
}

SVFileNameArrayClass::SVFileNameArrayClass(const SVFileNameArrayClass& orig)
{
	*this = orig;
}

SVFileNameArrayClass::~SVFileNameArrayClass()
{
}

const SVFileNameArrayClass& SVFileNameArrayClass::operator=(const SVFileNameArrayClass& rhs)
{
	Copy( rhs );

	return *this;
}

LPCTSTR SVFileNameArrayClass::GetFileNameList()
{
	mcsFileList.Empty();

	for (int i = 0; i < GetSize(); i++)
	{
		if ( i )
		{
			mcsFileList += ";";
		}

		mcsFileList += GetAt(i).GetFullFileName();
	}

	return mcsFileList;
}

BOOL SVFileNameArrayClass::SetFileNameList(LPCTSTR szFileNameList)
{
	BOOL bOk = TRUE;
	BOOL bDone = FALSE;

	int i = 0;

	CString csTemp;
	CString csFileNameList = szFileNameList;

	RemoveAll();

	do
	{
		bDone = csFileNameList.IsEmpty();

		if ( !bDone )
		{
			bOk = FALSE;

			i = csFileNameList.FindOneOf( ";," );

			if ( i < 0 )
			{
				csTemp = csFileNameList;
				csFileNameList.Empty();
			}
			else
			{
				csTemp = csFileNameList.Left( i );
				csFileNameList.Delete( 0 , i + 1 );
			}

      if ( !csTemp.IsEmpty() )
			{
				bOk = 0 <= Add( SVFileNameClass( csTemp ) );
			}
		}
	}
	while ( bOk && !bDone );

	return bOk;
}
