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

#include "stdafx.h"
#include "SVOutObjectInfoStruct.h"
#include "SVObjectLibrary.h"
#include "SVObjectClass.h"
#include "SVObjectManagerClass.h"

SVOutObjectInfoStruct::SVOutObjectInfoStruct()
: SVObjectInfoStruct(), UserInfoList(), m_CriticalSectionPtr()
{
	m_CriticalSectionPtr = new SVCriticalSection;
}

SVOutObjectInfoStruct::SVOutObjectInfoStruct( const SVOutObjectInfoStruct& p_rsvValue )
: SVObjectInfoStruct( p_rsvValue ), UserInfoList( p_rsvValue.UserInfoList ), m_CriticalSectionPtr()
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

		UserInfoList.Copy( p_rsvValue.UserInfoList );
	}

	return *this;
}

HRESULT SVOutObjectInfoStruct::AddInput( SVInObjectInfoStruct& p_rsvInput )
{
	// Update Pointer...
	p_rsvInput.SetInputObject( SVObjectManagerClass::Instance().GetObject( p_rsvInput.GetInputObjectInfo().UniqueObjectID ) );

	UserInfoList.Add( p_rsvInput );	

	return S_OK;
}

HRESULT SVOutObjectInfoStruct::RemoveInput( SVInObjectInfoStruct& p_rsvInput )
{
	BOOL l_bFound = FALSE;

	long l_lCount = UserInfoList.GetSize();

	for( long l = l_lCount - 1; 0 <= l; l-- )
	{
		SVInObjectInfoStruct& inObjectInfo = UserInfoList[ l ];

		l_bFound = inObjectInfo.UniqueObjectID == p_rsvInput.UniqueObjectID;

		if ( l_bFound )
		{
			UserInfoList.RemoveAt( l );

			// Update Pointer...
			p_rsvInput.SetInputObject( nullptr );

			if( p_rsvInput.CheckExistence() )
			{
				p_rsvInput.PObject->ResetObjectInputs();
			}
		}
	}

	return S_OK;
}

HRESULT SVOutObjectInfoStruct::DisconnectAllInputs()
{
	long l_lCount = UserInfoList.GetSize();

	for( long l = l_lCount - 1; 0 <= l; l-- )
	{
		SVInObjectInfoStruct& inObjectInfo = UserInfoList[ l ];

		inObjectInfo.SetInputObject( UniqueObjectID );

		if( SV_GUID_NULL != inObjectInfo.UniqueObjectID )
		{
			// Send to the Object that is using this output
			::SVSendMessage(inObjectInfo.UniqueObjectID,
							SVM_DISCONNECT_OBJECT_INPUT, 
							reinterpret_cast<DWORD_PTR>(&inObjectInfo), 0 );
		}
	}

	UserInfoList.RemoveAll();

	return S_OK;
}

HRESULT SVOutObjectInfoStruct::GetDependentsList( SVObjectClass* p_psvObject, SVObjectPairVector& rListOfDependents )
{
	long l_lCount = UserInfoList.GetSize();

	for( long l = l_lCount - 1; 0 <= l; l-- )
	{
		SVInObjectInfoStruct& rInInfo = UserInfoList[ l ];

		if( rInInfo.PObject && rInInfo.PObject != p_psvObject )
		{
			if( rInInfo.CheckExistence() )
			{
				CString strTempName;
				CString strName = p_psvObject->GetCompleteObjectNameToObjectType( nullptr, SVToolObjectType ) + _T( "." );

				// Who is using
				strTempName = rInInfo.PObject->GetCompleteObjectNameToObjectType( nullptr, SVToolObjectType );
				
				// exclude ourself or our children and the document (published)
				SVObjectInfoStruct objectTypeInfo = rInInfo.PObject->GetObjectInfo();

				if( strTempName.Find( strName ) == -1 && 
					objectTypeInfo.ObjectTypeInfo.ObjectType != SVInspectionObjectType )
				{
					SVObjectPair pair;
					
					pair.first = rInInfo.PObject; // who is using
					pair.second = PObject; // who is being used

					rListOfDependents.push_back( pair );
				}
			}
		}// end if( rInInfo.PObject && rInInfo.PObject != this )
	}// end for( int n = 0; n < pOutput->UserInfoList.GetSize(); n++ )

	return S_OK;
}

BOOL SVOutObjectInfoStruct::Lock( DWORD p_TimeOutMilliseconds ) const
{
	BOOL l_Status = true;

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

BOOL SVOutObjectInfoStruct::Unlock() const
{
	BOOL l_Status = !( m_CriticalSectionPtr.empty() );

	if( l_Status )
	{
		l_Status = m_CriticalSectionPtr->Unlock();
	}

	return l_Status;
}

