//******************************************************************************
//* COPYRIGHT (c) 2011 by Seidenader Vision, Inc.
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVIOEntryHostStruct
//* .File Name       : $Workfile:   SVIOEntryHostStruct.cpp  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.1  $
//* .Check In Date   : $Date:   27 Aug 2014 01:24:38  $
//******************************************************************************

#pragma region Includes
#include "stdafx.h"

#include "SVIOEntryHostStruct.h"
#include "SVObjectClass.h"
#include "SVUtilityLibrary/StringHelper.h"
#pragma endregion Includes

SVIOEntryHostStruct::SVIOEntryHostStruct()
: m_Enabled( false )
, m_PPQIndex( -1 )
, m_ObjectType( IO_INVALID_OBJECT )
, m_IOId()
, m_DeleteValueObject( true )
, m_pObject(nullptr)
, m_pValueObject(nullptr)
{
}

SVIOEntryHostStruct::~SVIOEntryHostStruct()
{
	clear();
}

void SVIOEntryHostStruct::clear()
{
	if( m_DeleteValueObject && ( nullptr != m_pObject ) )
	{
		delete m_pObject;
	}

	m_Enabled = false;
	m_PPQIndex = -1;
	m_ObjectType = IO_INVALID_OBJECT;
	m_IOId.clear();
	m_DeleteValueObject = true;
	m_pObject = nullptr;
	m_pValueObject = nullptr;
}

void SVIOEntryHostStruct::setObject(SVObjectClass* pObject)
{
	//Required to avoid dynamic cast at run time
	m_pObject = pObject;
	m_pValueObject = dynamic_cast<SvOi::IValueObject*> (m_pObject);
}

bool SVIOEntryHostStruct::PtrGreater(SVIOEntryHostStructPtr elem1, SVIOEntryHostStructPtr elem2)
{
	bool Greater = false;

	if(nullptr != elem1 && nullptr != elem2 )
	{
		if( ( nullptr != elem1->m_pObject ) && ( nullptr != elem2->m_pObject ) )
		{
			std::string Name1 = elem1->m_pObject->GetCompleteName();
			std::string Name2 = elem2->m_pObject->GetCompleteName();

			//We assume the name is a dotted name and only the last part of the name may have a number

			std::string::size_type Pos = Name1.rfind(_T("."));
			if( std::string::npos != Pos )
			{

				Pos = Name1.find_first_of( _T("0123456789"), Pos );
				if( std::string::npos != Pos  && 0 != Pos )
				{
					if( Name2.size() > Pos )
					{
						int Compare = Name2.substr(0, Pos).compare( Name1.substr(0, Pos ) );
						if( 0 == Compare )
						{
							long Value1 = atol( SvUl::Mid( Name1, Pos ).c_str() );
							long Value2 = atol( SvUl::Mid( Name2, Pos ).c_str() );
							Greater = Value2 > Value1;
						}
						else
						{
							Greater = Compare > 0;
						}
					}
					else
					{
						int Compare = Name2.compare( Name1 );
						Greater = Compare > 0;
					}
				}
				else
				{
					int Compare = Name2.compare( Name1 );
					Greater = Compare > 0;
				}
			}
			else
			{
				int Compare = Name2.compare( Name1 );
				Greater = Compare > 0;
			}
		}
	}

	return Greater;
}

