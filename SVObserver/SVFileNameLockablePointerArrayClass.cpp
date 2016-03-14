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
#include "SVOMFCLibrary/SVFileNameClass.h"
#pragma endregion Includes

//##ModelId=3A54D7EC0251
SVFileNameLockablePointerArrayClass::SVFileNameLockablePointerArrayClass()
:mcmArrayLock()
{
}

//##ModelId=3A54A0EB03D8
SVFileNameLockablePointerArrayClass::SVFileNameLockablePointerArrayClass(const SVFileNameLockablePointerArrayClass& orig)
{
	*this = orig;
}

SVFileNameLockablePointerArrayClass::~SVFileNameLockablePointerArrayClass()
{
}


//##ModelId=3A54A0EC003E
const SVFileNameLockablePointerArrayClass SVFileNameLockablePointerArrayClass::operator=(const SVFileNameLockablePointerArrayClass& rhs)
{
	Copy( rhs );

	return *this;
}

//##ModelId=3A54A6750280
LPCTSTR SVFileNameLockablePointerArrayClass::GetFileNameList()
{
	mcsFileList.Empty();

	for (int i = 0; i < GetSize(); i++)
	{
		if ( i )
		{
			mcsFileList += ";";
		}

		mcsFileList += GetAt(i)->GetFullFileName();
	}

	return mcsFileList;
}

//##ModelId=3A535EFE009C
BOOL SVFileNameLockablePointerArrayClass::Lock()
{
	return mcmArrayLock.Lock( 300000 );
}

//##ModelId=3A535F67000F
BOOL SVFileNameLockablePointerArrayClass::Unlock()
{
	return mcmArrayLock.Unlock();
}

