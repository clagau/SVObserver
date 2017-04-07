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
#include "SVUtilityLibrary/SVString.h"
#pragma endregion Includes

#pragma region Declarations
#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif
#pragma endregion Declarations

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
	p_rsvInput.SetInputObject( SVObjectManagerClass::Instance().GetObject( p_rsvInput.GetInputObjectInfo().m_UniqueObjectID ) );

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

		l_bFound = inObjectInfo.m_UniqueObjectID == p_rsvInput.m_UniqueObjectID;

		if ( l_bFound )
		{
			UserInfoList.RemoveAt( l );

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
	long l_lCount = UserInfoList.GetSize();

	for( long l = l_lCount - 1; 0 <= l; l-- )
	{
		SVInObjectInfoStruct& inObjectInfo = UserInfoList[ l ];

		inObjectInfo.SetInputObject( m_UniqueObjectID );

		if( SV_GUID_NULL != inObjectInfo.m_UniqueObjectID )
		{
			// Send to the Object that is using this output
			SVObjectManagerClass::Instance().DisconnectObjectInput(inObjectInfo.m_UniqueObjectID, &inObjectInfo);
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

		if( rInInfo.m_pObject && rInInfo.m_pObject != p_psvObject )
		{
			if( rInInfo.CheckExistence() )
			{
				SVString strTempName;
				SVString strName = p_psvObject->GetCompleteObjectNameToObjectType( nullptr, SVToolObjectType ) + _T( "." );

				// Who is using
				strTempName = rInInfo.m_pObject->GetCompleteObjectNameToObjectType( nullptr, SVToolObjectType );
				
				// exclude ourself or our children and the document (published)
				SVObjectInfoStruct objectTypeInfo = rInInfo.m_pObject->GetObjectInfo();

				if( SVString::npos == strTempName.find( strName ) && 
					objectTypeInfo.m_ObjectTypeInfo.ObjectType != SVInspectionObjectType )
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

