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
#include "SVFileNameClass.h"
#pragma endregion Includes

const SVFileNameLockablePointerArrayClass SVFileNameLockablePointerArrayClass::operator=(const SVFileNameLockablePointerArrayClass& rRhs)
{
	m_FileNamePtrVector = rRhs.getFileNamePtrVector();

	return *this;
}

SvDef::StringVector SVFileNameLockablePointerArrayClass::GetFileNameList() const
{
	SvDef::StringVector FileNameList;

	for (const auto& rFile : m_FileNamePtrVector)
	{
		std::string FileName{rFile->GetFullFileName()};
		if (0 == _access(FileName.c_str(), 0))
		{
			//Don't place filenames already in list
			if(FileNameList.end() == std::find(FileNameList.begin(), FileNameList.end(), FileName))
			{
				FileNameList.emplace_back(FileName);
			}
		}
	}

	return FileNameList;
}

bool SVFileNameLockablePointerArrayClass::Lock()
{
	return mcmArrayLock.Lock( 300000 );
}

bool SVFileNameLockablePointerArrayClass::Unlock()
{
	return mcmArrayLock.Unlock();
}

