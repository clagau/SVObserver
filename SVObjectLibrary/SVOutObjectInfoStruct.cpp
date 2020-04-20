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
: SVObjectInfoStruct(), m_UserInfoList()
{
	
}

SVOutObjectInfoStruct::SVOutObjectInfoStruct( const SVOutObjectInfoStruct& p_rsvValue )
: SVObjectInfoStruct( p_rsvValue ), m_UserInfoList( p_rsvValue.m_UserInfoList )
{
	
}

SVOutObjectInfoStruct::~SVOutObjectInfoStruct()
{
	
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

HRESULT SVOutObjectInfoStruct::AddInput(SvOl::SVInObjectInfoStruct& p_rsvInput )
{
	// Update Pointer...
	p_rsvInput.SetInputObject( SVObjectManagerClass::Instance().GetObject( p_rsvInput.GetInputObjectInfo().getObjectId() ) );

	m_UserInfoList.push_back( p_rsvInput );	

	return S_OK;
}

HRESULT SVOutObjectInfoStruct::RemoveInput(SvOl::SVInObjectInfoStruct& rInputInfo )
{
	bool l_bFound = false;

	long l_lCount = static_cast<long> (m_UserInfoList.size());

	for( long l = l_lCount - 1; 0 <= l; l-- )
	{
		SvOl::SVInObjectInfoStruct& inObjectInfo = m_UserInfoList[ l ];

		l_bFound = inObjectInfo.getObjectId() == rInputInfo.getObjectId();

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
		SvOl::SVInObjectInfoStruct& inObjectInfo = m_UserInfoList[l];

		inObjectInfo.SetInputObject( getObjectId() );

		if (SvDef::InvalidObjectId != inObjectInfo.getObjectId())
		{
			// Send to the Object that is using this output
			SVObjectManagerClass::Instance().DisconnectObjectInput(inObjectInfo.getObjectId(), &inObjectInfo);
		}
	}

	m_UserInfoList.clear();

	return S_OK;
}



