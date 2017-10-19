//******************************************************************************
//* COPYRIGHT (c) 2003 by SVResearch, Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVLockableFileNameArrayClass
//* .File Name       : $Workfile:   SVFileNameLockablePointerArrayClass.cpp  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.0  $
//* .Check In Date   : $Date:   23 Apr 2013 10:36:54  $
//******************************************************************************

#pragma region Includes
#include "stdafx.h"
#include "SVFileNameLockablePointerArrayClass.h"
#include "SVLibrary/SVFileNameClass.h"
#pragma endregion Includes

SVFileNameLockablePointerArrayClass::SVFileNameLockablePointerArrayClass()
{
}

SVFileNameLockablePointerArrayClass::SVFileNameLockablePointerArrayClass(const SVFileNameLockablePointerArrayClass& orig)
{
	*this = orig;
}

SVFileNameLockablePointerArrayClass::~SVFileNameLockablePointerArrayClass()
{
}

const SVFileNameLockablePointerArrayClass SVFileNameLockablePointerArrayClass::operator=(const SVFileNameLockablePointerArrayClass& rhs)
{
	Copy( rhs );

	return *this;
}

LPCTSTR SVFileNameLockablePointerArrayClass::GetFileNameList() const
{
	mcsFileList.clear();

	for (int i = 0; i < GetSize(); i++)
	{
		if ( i )
		{
			mcsFileList += ";";
		}

		mcsFileList += GetAt(i)->GetFullFileName();
	}

	return mcsFileList.c_str();
}

bool SVFileNameLockablePointerArrayClass::Lock()
{
	return mcmArrayLock.Lock( 300000 );
}

bool SVFileNameLockablePointerArrayClass::Unlock()
{
	return mcmArrayLock.Unlock();
}

