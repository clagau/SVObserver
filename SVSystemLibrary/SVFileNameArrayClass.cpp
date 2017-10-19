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
	mcsFileList.clear();

	for (int i = 0; i < GetSize(); i++)
	{
		if ( i )
		{
			mcsFileList += ";";
		}

		mcsFileList += GetAt(i).GetFullFileName();
	}

	return mcsFileList.c_str();
}

bool SVFileNameArrayClass::SetFileNameList(LPCTSTR szFileNameList)
{
	bool bOk = true;
	bool bDone = false;

	SVString sTemp;
	SVString sFileNameList = szFileNameList;

	RemoveAll();

	do
	{
		bDone = sFileNameList.empty();

		if ( !bDone )
		{
			bOk = false;

			size_t i = sFileNameList.find_first_of( ";," );

			if ( SVString::npos == i )
			{
				sTemp = sFileNameList;
				sFileNameList.clear();
			}
			else
			{
				sTemp = SvUl_SF::Left(sFileNameList, i );
				sFileNameList.erase( 0 , i + 1 );
			}

			if ( !sTemp.empty() )
			{
				bOk = 0 <= Add( SVFileNameClass( sTemp.c_str() ) );
			}
		}
	}
	while ( bOk && !bDone );

	return bOk;
}
