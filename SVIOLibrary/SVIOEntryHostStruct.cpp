//******************************************************************************
//* COPYRIGHT (c) 2011 by Körber Pharma Inspection GmbH. All Rights Reserved
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
#include "SVOutputObject.h"
#include "ObjectInterfaces/IValueObject.h"
#include "SVObjectLibrary/SVObjectClass.h"
#include "SVObjectLibrary/SVObjectManagerClass.h"
#include "SVUtilityLibrary/StringHelper.h"
#pragma endregion Includes

void SVIOEntryHostStruct::clear()
{
	for (auto& rEntry : m_ioLinkedValues)
	{
		if (nullptr != rEntry.m_pObject && SvDef::InvalidObjectId != rEntry.m_inspectionId)
		{
			rEntry.m_pObject->disconnectObject(rEntry.m_inspectionId);
		}
		rEntry.m_inspectionId = SvDef::InvalidObjectId;
		rEntry.m_pObject = nullptr;
		rEntry.m_pValueObject = nullptr;
	}
	m_Enabled = false;
	m_PPQIndex = -1;
	m_ObjectType = IO_INVALID_OBJECT;
	m_IOId = SvDef::InvalidObjectId;
}

void SVIOEntryHostStruct::setValueObject(SVObjectClass* pObject, DWORD objectIDIndex /*= 0*/)
{
	if (objectIDIndex < SvDef::cObjectIndexMaxNr)
	{
		if (nullptr != m_ioLinkedValues[objectIDIndex].m_pObject)
		{
			m_ioLinkedValues[objectIDIndex].m_pObject->disconnectObject(m_ioLinkedValues[objectIDIndex].m_inspectionId);
		}
		if (nullptr != pObject)
		{
			//Required to avoid dynamic cast at run time
			m_ioLinkedValues[objectIDIndex].m_pObject = pObject;
			m_ioLinkedValues[objectIDIndex].m_pValueObject = dynamic_cast<SvOi::IValueObject*> (pObject);
			SVObjectClass* pInspection = pObject->GetAncestor(SvPb::SVInspectionObjectType);
			if (nullptr != pInspection)
			{
				m_ioLinkedValues[objectIDIndex].m_inspectionId = pInspection->getObjectId();
				pObject->connectObject(m_ioLinkedValues[objectIDIndex].m_inspectionId);
			}
			SVObjectClass* pIOObject = SVObjectManagerClass::Instance().GetObject(m_IOId);
			SVOutputObject* pOutput = dynamic_cast<SVOutputObject*> (pIOObject);
			if (nullptr != pOutput)
			{
				pOutput->SetValueObjectID(pObject->getObjectId(), objectIDIndex);
			}
		}
		else
		{
			m_ioLinkedValues[objectIDIndex].m_pObject = nullptr;
			m_ioLinkedValues[objectIDIndex].m_pValueObject = nullptr;
			SVObjectClass* pIOObject = SVObjectManagerClass::Instance().GetObject(m_IOId);
			SVOutputObject* pOutput = dynamic_cast<SVOutputObject*> (pIOObject);
			if (nullptr != pOutput)
			{
				pOutput->SetValueObjectID(SvDef::InvalidObjectId, objectIDIndex);
			}
		}
	}
}

SVObjectClass* SVIOEntryHostStruct::getObject(DWORD objectIDIndex /*= 0*/) const
{
	if (objectIDIndex < SvDef::cObjectIndexMaxNr)
	{
		return m_ioLinkedValues[objectIDIndex].m_pObject;
	}
	return nullptr;
}

SvOi::IValueObject* SVIOEntryHostStruct::getValueObject(DWORD objectIDIndex /*= 0*/) const
{
	if (objectIDIndex < SvDef::cObjectIndexMaxNr)
	{
		return m_ioLinkedValues[objectIDIndex].m_pValueObject;
	}
	return nullptr;
}

uint32_t SVIOEntryHostStruct::getInspectionID(DWORD objectIDIndex /*= 0*/) const
{
	if (objectIDIndex < SvDef::cObjectIndexMaxNr)
	{
		return m_ioLinkedValues[objectIDIndex].m_inspectionId;
	}
	return 0;
}

bool SVIOEntryHostStruct::PtrGreater(SVIOEntryHostStructPtr elem1, SVIOEntryHostStructPtr elem2)
{
	bool Greater = false;

	if(nullptr != elem1 && nullptr != elem2 )
	{
		if( ( nullptr != elem1->m_ioLinkedValues[0].m_pObject ) && ( nullptr != elem2->m_ioLinkedValues[0].m_pObject ) )
		{
			std::string Name1 = elem1->m_ioLinkedValues[0].m_pObject->GetCompleteName();
			std::string Name2 = elem2->m_ioLinkedValues[0].m_pObject->GetCompleteName();

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

