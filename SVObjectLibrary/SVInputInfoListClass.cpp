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
#include "SVObjectManagerClass.h"
#pragma endregion Includes

#pragma region Declarations
#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif
#pragma endregion Declarations

namespace SvOl
{
SVInputInfoListClass::SVInputInfoListClass()
{
}

SVInputInfoListClass::~SVInputInfoListClass()
{
}

const SVInputInfoListClass& SVInputInfoListClass::operator=(const SVInputInfoListClass& rRhs)
{
	m_InObjectInfoVector = rRhs.getInObjectInfoVector();
	return *this;
}

bool SVInputInfoListClass::CheckExistence( int Index /*= -1*/ )
{
	bool Result{ false };

	if( Index < static_cast<int> (m_InObjectInfoVector.size()) && Index >= -1 )
	{
		if( Index > -1 )
		{
			// Check only this entry...
			SVInObjectInfoStruct* pInfoObject = m_InObjectInfoVector[Index];
			if( nullptr != pInfoObject)
			{
				SVObjectClass* pObject = pInfoObject->getObject();
				pObject = SVObjectManagerClass::Instance().GetObject(pInfoObject->getObjectId());
				Result = (nullptr != pObject) ? true : false;
			}
		}
		else
		{
			// Check all entries...
			Result = true;
			for( int i = 0; i < static_cast<int> (m_InObjectInfoVector.size()); ++i )
			{
				SVInObjectInfoStruct* pInfoObject = m_InObjectInfoVector[i];
				if(nullptr != pInfoObject)
				{
					Result = ( nullptr != SVObjectManagerClass::Instance().GetObject(pInfoObject->getObjectId()) ? true : false ) && Result;
				}
				else
				{
					Result = false;
				}
			}
		}
	}
	return Result;
}

////////////////////////////////////////////////////////////////////////////////
// Override for Add - Do not allow duplicates
////////////////////////////////////////////////////////////////////////////////
int SVInputInfoListClass::push_back( SVInObjectInfoStruct* pInObjectInfo )
{
	if (!checkDuplicates(pInObjectInfo))
	{
		m_InObjectInfoVector.push_back(pInObjectInfo);
	}

	return static_cast<int> (m_InObjectInfoVector.size());
}

int SVInputInfoListClass::remove(SVInObjectInfoStruct* pInObjectInfo)
{
	m_InObjectInfoVector.erase(std::remove(m_InObjectInfoVector.begin(), m_InObjectInfoVector.end(), pInObjectInfo), m_InObjectInfoVector.end());
	return static_cast<int> (m_InObjectInfoVector.size());
}

////////////////////////////////////////////////////////////////////////////////
// check for duplicates
////////////////////////////////////////////////////////////////////////////////
bool SVInputInfoListClass::checkDuplicates( SVInObjectInfoStruct* pInObjectInfo )
{
	for(auto const rInObjectInfo : m_InObjectInfoVector)
	{
		if( rInObjectInfo == pInObjectInfo )
		{
			return true;
		}
	}
	return false;
}

} //namespace SvOl