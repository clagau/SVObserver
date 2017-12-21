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
	m_CriticalSectionPtr = SVCriticalSectionPtr{ new SVCriticalSection };
}

SVOutObjectInfoStruct::SVOutObjectInfoStruct( const SVOutObjectInfoStruct& p_rsvValue )
: SVObjectInfoStruct( p_rsvValue ), m_UserInfoList( p_rsvValue.m_UserInfoList ), m_CriticalSectionPtr()
{
	m_CriticalSectionPtr = SVCriticalSectionPtr{ new SVCriticalSection };
}

SVOutObjectInfoStruct::~SVOutObjectInfoStruct()
{
	m_CriticalSectionPtr.reset();
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
	p_rsvInput.SetInputObject( SVObjectManagerClass::Instance().GetObject( p_rsvInput.GetInputObjectInfo().getUniqueObjectID() ) );

	m_UserInfoList.push_back( p_rsvInput );	

	return S_OK;
}

HRESULT SVOutObjectInfoStruct::RemoveInput( SVInObjectInfoStruct& rInputInfo )
{
	bool l_bFound = false;

	long l_lCount = static_cast<long> (m_UserInfoList.size());

	for( long l = l_lCount - 1; 0 <= l; l-- )
	{
		SVInObjectInfoStruct& inObjectInfo = m_UserInfoList[ l ];

		l_bFound = inObjectInfo.getUniqueObjectID() == rInputInfo.getUniqueObjectID();

		if ( l_bFound )
		{
			m_UserInfoList.erase(m_UserInfoList.begin() + l);

			// Update Pointer...
			rInputInfo.SetInputObject( nullptr );

			if( rInputInfo.CheckExistence() )
			{
				rInputInfo.getObject()->ResetObjectInputs();
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

		inObjectInfo.SetInputObject( getUniqueObjectID() );

		if( SV_GUID_NULL != inObjectInfo.getUniqueObjectID() )
		{
			// Send to the Object that is using this output
			SVObjectManagerClass::Instance().DisconnectObjectInput(inObjectInfo.getUniqueObjectID(), &inObjectInfo);
		}
	}

	m_UserInfoList.clear();

	return S_OK;
}

bool SVOutObjectInfoStruct::Lock( DWORD p_TimeOutMilliseconds ) const
{
	bool l_Status = true;

	if(nullptr == m_CriticalSectionPtr )
	{
		m_CriticalSectionPtr = SVCriticalSectionPtr{ new SVCriticalSection };
	}

	l_Status = (nullptr != m_CriticalSectionPtr);

	if( l_Status )
	{
		l_Status = m_CriticalSectionPtr->Lock( p_TimeOutMilliseconds );
	}

	return l_Status;
}

bool SVOutObjectInfoStruct::Unlock() const
{
	bool l_Status = (nullptr != m_CriticalSectionPtr);

	if( l_Status )
	{
		l_Status = m_CriticalSectionPtr->Unlock();
	}

	return l_Status;
}

