//******************************************************************************
//* COPYRIGHT (c) 2003 by SVResearch, Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVOutObjectInfoStruct
//* .File Name       : $Workfile:   SVOutObjectInfoStruct.cpp  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.2  $
//* .Check In Date   : $Date:   15 May 2014 09:42:24  $
//******************************************************************************

#pragma region Includes
#include "stdafx.h"
#include "SVOutObjectInfoStruct.h"
#include "SVObjectLibrary.h"
#include "SVObjectClass.h"
#include "SVObjectManagerClass.h"

#pragma endregion Includes

#pragma region Declarations
#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif
#pragma endregion Declarations

SVOutObjectInfoStruct::SVOutObjectInfoStruct()
: SVObjectInfoStruct(), m_UserInfoList(), m_CriticalSectionPtr()
{
	m_CriticalSectionPtr = new SVCriticalSection;
}

SVOutObjectInfoStruct::SVOutObjectInfoStruct( const SVOutObjectInfoStruct& p_rsvValue )
: SVObjectInfoStruct( p_rsvValue ), m_UserInfoList( p_rsvValue.m_UserInfoList ), m_CriticalSectionPtr()
{
	m_CriticalSectionPtr = new SVCriticalSection;
}

SVOutObjectInfoStruct::~SVOutObjectInfoStruct()
{
	m_CriticalSectionPtr.clear();
}

const SVOutObjectInfoStruct& SVOutObjectInfoStruct::operator=( const SVOutObjectInfoStruct& p_rsvValue )
{
	if( this != &p_rsvValue )
	{
		SVObjectInfoStruct::operator =( p_rsvValue );

		m_UserInfoList =  p_rsvValue.m_UserInfoList;
	}

	return *this;
}

HRESULT SVOutObjectInfoStruct::AddInput( SVInObjectInfoStruct& p_rsvInput )
{
	// Update Pointer...
	p_rsvInput.SetInputObject( SVObjectManagerClass::Instance().GetObject( p_rsvInput.GetInputObjectInfo().m_UniqueObjectID ) );

	m_UserInfoList.push_back( p_rsvInput );	

	return S_OK;
}

HRESULT SVOutObjectInfoStruct::RemoveInput( SVInObjectInfoStruct& p_rsvInput )
{
	bool l_bFound = false;

	long l_lCount = static_cast<long> (m_UserInfoList.size());

	for( long l = l_lCount - 1; 0 <= l; l-- )
	{
		SVInObjectInfoStruct& inObjectInfo = m_UserInfoList[ l ];

		l_bFound = inObjectInfo.m_UniqueObjectID == p_rsvInput.m_UniqueObjectID;

		if ( l_bFound )
		{
			m_UserInfoList.erase(m_UserInfoList.begin() + l);

			// Update Pointer...
			p_rsvInput.SetInputObject( nullptr );

			if( p_rsvInput.CheckExistence() )
			{
				p_rsvInput.m_pObject->ResetObjectInputs();
			}
		}
	}

	return S_OK;
}

HRESULT SVOutObjectInfoStruct::DisconnectAllInputs()
{
	long l_lCount = static_cast<long> (m_UserInfoList.size());

	for( long l = l_lCount - 1; 0 <= l; l-- )
	{
		SVInObjectInfoStruct& inObjectInfo = m_UserInfoList[l];

		inObjectInfo.SetInputObject( m_UniqueObjectID );

		if( SV_GUID_NULL != inObjectInfo.m_UniqueObjectID )
		{
			// Send to the Object that is using this output
			SVObjectManagerClass::Instance().DisconnectObjectInput(inObjectInfo.m_UniqueObjectID, &inObjectInfo);
		}
	}

	m_UserInfoList.clear();

	return S_OK;
}

HRESULT SVOutObjectInfoStruct::GetDependentsList( SVObjectClass* p_psvObject, SVObjectPairVector& rListOfDependents )
{
	long l_lCount = static_cast<long> (m_UserInfoList.size());

	for( long l = l_lCount - 1; 0 <= l; l-- )
	{
		SVInObjectInfoStruct& rInInfo = m_UserInfoList[l];

		if( rInInfo.m_pObject && rInInfo.m_pObject != p_psvObject )
		{
			if( rInInfo.CheckExistence() )
			{
				std::string strTempName;
				std::string strName = p_psvObject->GetCompleteObjectNameToObjectType( nullptr, SvDef::SVToolObjectType ) + _T( "." );

				// Who is using
				strTempName = rInInfo.m_pObject->GetCompleteObjectNameToObjectType( nullptr, SvDef::SVToolObjectType );
				
				// exclude ourself or our children and the document (published)
				SVObjectInfoStruct objectTypeInfo = rInInfo.m_pObject->GetObjectInfo();

				if( std::string::npos == strTempName.find( strName ) && 
					objectTypeInfo.m_ObjectTypeInfo.ObjectType != SvDef::SVInspectionObjectType )
				{
					SVObjectPair pair;
					
					pair.first = rInInfo.m_pObject; // who is using
					pair.second = m_pObject; // who is being used

					rListOfDependents.push_back( pair );
				}
			}
		}// end if( rInInfo.PObject && rInInfo.PObject != this )
	}// end for( int n = 0; n < pOutput->UserInfoList.GetSize(); n++ )

	return S_OK;
}

bool SVOutObjectInfoStruct::Lock( DWORD p_TimeOutMilliseconds ) const
{
	bool l_Status = true;

	if( m_CriticalSectionPtr.empty() )
	{
		m_CriticalSectionPtr = new SVCriticalSection;
	}

	l_Status = !( m_CriticalSectionPtr.empty() );

	if( l_Status )
	{
		l_Status = m_CriticalSectionPtr->Lock( p_TimeOutMilliseconds );
	}

	return l_Status;
}

bool SVOutObjectInfoStruct::Unlock() const
{
	bool l_Status = !( m_CriticalSectionPtr.empty() );

	if( l_Status )
	{
		l_Status = m_CriticalSectionPtr->Unlock();
	}

	return l_Status;
}

