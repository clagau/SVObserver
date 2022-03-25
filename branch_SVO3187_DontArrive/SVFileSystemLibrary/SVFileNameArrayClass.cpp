//******************************************************************************
//* COPYRIGHT (c) 2003 by Körber Pharma Inspection GmbH. All Rights Reserved
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVFileNameArrayClass
//* .File Name       : $Workfile:   SVFileNameArrayClass.cpp  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.0  $
//* .Check In Date   : $Date:   23 Apr 2013 10:36:20  $
//******************************************************************************

#pragma region Includes
#include "stdafx.h"
#include "SVFileNameArrayClass.h"
#include "SVUtilityLibrary/StringHelper.h"
#pragma endregion Includes

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

const SVFileNameArrayClass& SVFileNameArrayClass::operator=(const SVFileNameArrayClass& rRhs)
{
	m_FileNameVector = rRhs.getFileNameVector();
	return *this;
}

LPCTSTR SVFileNameArrayClass::GetFileNameList()
{
	mcsFileList.clear();

	for (int i = 0; i < static_cast<int> (m_FileNameVector.size()); i++)
	{
		if (0 != i )
		{
			mcsFileList += ";";
		}

		mcsFileList += m_FileNameVector[i].GetFullFileName();
	}

	return mcsFileList.c_str();
}

bool SVFileNameArrayClass::SetFileNameList(LPCTSTR szFileNameList)
{
	bool bOk = true;
	bool bDone = false;

	std::string sTemp;
	std::string sFileNameList = szFileNameList;

	m_FileNameVector.clear();

	do
	{
		bDone = sFileNameList.empty();

		if ( !bDone )
		{
			bOk = false;

			size_t i = sFileNameList.find_first_of( ";," );

			if ( std::string::npos == i )
			{
				sTemp = sFileNameList;
				sFileNameList.clear();
			}
			else
			{
				sTemp = SvUl::Left(sFileNameList, i );
				sFileNameList.erase( 0 , i + 1 );
			}

			if ( !sTemp.empty() )
			{
				m_FileNameVector.push_back( SVFileNameClass(sTemp.c_str()) );
				bOk = true;
			}
		}
	}
	while ( bOk && !bDone );

	return bOk;
}
