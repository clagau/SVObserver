//******************************************************************************
//* COPYRIGHT (c) 2003 by SVResearch, Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVObjectManager
//* .File Name       : $Workfile:   SVObjectManagerClass.cpp  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.0  $
//* .Check In Date   : $Date:   22 Apr 2013 16:57:38  $
//******************************************************************************

#include "stdafx.h"
#include "SVObjectManagerClass.h"

#include "SVSystemLibrary/SVAutoLockAndReleaseTemplate.h"

#include "SVClassRegisterListClass.h"
#include "SVObjectClass.h"
#include "SVObjectLibrary.h"
#include "SVObjectSubmitCommandFacade.h"
#include "SVObserverConnectData.h"
#include "SVObserverDisconnectData.h"
#include "SVSubjectConnectData.h"
#include "SVSubjectDisconnectData.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

SVObjectManagerClass& SVObjectManagerClass::Instance()
{
	static SVObjectManagerClass l_Object;

	return l_Object;
}

SVObjectManagerClass::SVObjectManagerClass()
: m_State( ReadWrite )
, m_ConfigurationID()
, m_Lock()
, m_OnlineDisplay( true )
, m_ShortPPQIndicator( 0 )
, m_ProductIndicator( 0 )
, m_PendingImageIndicator( 0 )
, m_InspectionIndicator( 0 )
, m_LastFrameRate( 10 )
, m_ObserverCookie( 1 )
, m_FileSequenceNumber( 0 )
, m_bIsColorSVIM(false)
{
}

SVObjectManagerClass::~SVObjectManagerClass()
{
	Shutdown();
}

SVObjectManagerClass::SVObjectManagerStateEnum SVObjectManagerClass::GetState() const
{
	return static_cast< SVObjectManagerStateEnum >( m_State );
}

HRESULT SVObjectManagerClass::SetState( SVObjectManagerStateEnum p_State )
{
	::InterlockedExchange( &m_State, p_State );

	if( m_State == ReadOnly )
	{
		::InterlockedIncrement( &m_FileSequenceNumber );
	}

	return S_OK;
}

const SVGUID& SVObjectManagerClass::GetConfigurationObjectID() const
{
	return m_ConfigurationID;
}

HRESULT SVObjectManagerClass::ConstructConfigurationObject( const SVGUID& p_rClassID )
{
	if( !( m_ConfigurationID.empty() ) )
	{
		DestroyConfigurationObject();
	}

	return ConstructObject( p_rClassID, m_ConfigurationID );
}

HRESULT SVObjectManagerClass::DestroyConfigurationObject()
{
	HRESULT l_Status = S_OK;

	if( !( m_ConfigurationID.empty() ) )
	{
		SVObjectClass* l_pObject = GetObject( m_ConfigurationID );

		m_ConfigurationID.clear();

		if( l_pObject != NULL )
		{
			delete l_pObject;
		}
	}

	return l_Status;
}

void SVObjectManagerClass::Shutdown()
{
	m_State = ReadWrite;

	SVAutoLockAndReleaseTemplate< SVCriticalSection > l_AutoLock;

	l_AutoLock.Assign( &m_Lock );

	if( !( m_IPDocs.empty() ) )
	{
		m_IPDocs.clear();
	}

	if( !( m_IODocs.empty() ) )
	{
		m_IODocs.clear();
	}

	if( !( m_CookieEntries.empty() ) )
	{
		m_CookieEntries.clear();
	}

	if( !( m_UniqueObjectEntries.empty() ) )
	{
		m_UniqueObjectEntries.clear();
	}
}

HRESULT SVObjectManagerClass::ConstructObject( const SVGUID& p_rClassID, GUID& p_rObjectID )
{
	HRESULT l_Status = S_OK;

	SVObjectClass* l_pObject = NULL;

	l_Status = ConstructObject( p_rClassID, l_pObject );

	if( l_pObject != NULL )
	{
		p_rObjectID = l_pObject->GetUniqueObjectID();
	}
	else
	{
		p_rObjectID = SVInvalidGUID;

		if( l_Status == S_OK )
		{
			l_Status = E_FAIL;
		}
	}

	return l_Status;
}

HRESULT SVObjectManagerClass::ConstructObject( const SVGUID& p_rClassID, SVObjectClass*& p_rpObject )
{
	HRESULT l_Status = S_OK;

	p_rpObject = SVClassRegisterListClass::Instance().ConstructNewObject( p_rClassID );

	if( p_rpObject == NULL )
	{
		l_Status = E_FAIL;
	}

	return l_Status;
}

void SVObjectManagerClass::DestructObject( const SVGUID& p_rClassID )
{
	if( !( p_rClassID.empty() ) )
	{
		SVObjectClass* l_pObject = GetObject( p_rClassID );

		if( l_pObject != NULL )
		{
			delete l_pObject;
		}
	}
}

HRESULT SVObjectManagerClass::GetObjectByIdentifier( const SVGUID& p_rObjectID, SVObjectClass*& p_rpObject ) const
{
	HRESULT l_Result = S_OK;

	SVAutoLockAndReleaseTemplate< SVCriticalSection > l_AutoLock;
	BOOL l_Status = !( p_rObjectID.empty() );

	if( l_Status && m_State == ReadWrite )
	{
		l_Status = l_AutoLock.Assign( &m_Lock );
	}

	if( l_Status )
	{
		SVUniqueObjectEntryStructPtr pUniqueObject = getUniqueObjectEntry( p_rObjectID );

		if( !( pUniqueObject.empty() ) )
		{
			p_rpObject = pUniqueObject->PObject;
		}
		else
		{
			p_rpObject = NULL;
		}
	}
	else
	{
		p_rpObject = NULL;
	}

	if( l_Result == S_OK && p_rpObject == NULL )
	{
		l_Result = E_FAIL;
	}

	return l_Result;
}

HRESULT SVObjectManagerClass::GetObjectByDottedName( const SVString& p_rFullName, GUID& p_rObjectID ) const
{
	HRESULT l_Status = S_OK;
	SVObjectReference l_Reference;

	l_Status = GetObjectByDottedName( p_rFullName, l_Reference );

	if( l_Reference.Object() != NULL )
	{
		p_rObjectID = l_Reference.Object()->GetUniqueObjectID();
	}
	else
	{
		if( l_Status == S_OK )
		{
			l_Status = E_FAIL;
		}

		p_rObjectID = SV_GUID_NULL;
	}

	return l_Status;
}

HRESULT SVObjectManagerClass::GetObjectByDottedName( const SVString& p_rFullName, SVObjectClass*& p_rpObject ) const
{
	HRESULT l_Status = S_OK;

	SVObjectReference l_Reference;

	l_Status = GetObjectByDottedName( p_rFullName, l_Reference );

	if( l_Reference.Object() != NULL )
	{
		p_rpObject = l_Reference.Object();
	}
	else
	{
		p_rpObject = NULL;
	}

	if( l_Status == S_OK && p_rpObject == NULL )
	{
		l_Status = E_FAIL;
	}

	return l_Status;
}

HRESULT SVObjectManagerClass::GetObjectByDottedName( const SVString& p_rFullName, SVObjectReference& p_rReference ) const
{
	HRESULT l_Result = S_OK;

	SVAutoLockAndReleaseTemplate< SVCriticalSection > l_AutoLock;
	BOOL l_Status = !( p_rFullName.empty() );

	if( l_Status && m_State == ReadWrite )
	{
		l_Status = l_AutoLock.Assign( &m_Lock );
	}

	if( l_Status )
	{
		SVObjectClass* l_pConfig = NULL;
		
		l_Result = GetObjectByIdentifier( m_ConfigurationID, l_pConfig );

		if( l_pConfig != NULL )
		{
			SVObjectNameInfo l_NameInfo;
			SVObjectClass* l_pObject = NULL;

			l_NameInfo.ParseObjectName( p_rFullName );

			l_Result = l_pConfig->GetChildObject( l_pObject, l_NameInfo );

			if( l_pObject != NULL )
			{
				p_rReference = SVObjectReference( l_pObject, l_NameInfo );
			}
			else
			{
				p_rReference = SVObjectReference();
			}
		}
		else
		{
			p_rReference = SVObjectReference();
		}
	}
	else
	{
		p_rReference = SVObjectReference();
	}

	if( l_Result == S_OK && p_rReference.Object() == NULL )
	{
		l_Result = E_FAIL;
	}

	return l_Result;
}

BOOL SVObjectManagerClass::CreateUniqueObjectID( SVObjectClass* PObject )
{
	BOOL l_Status = ( m_State == ReadWrite );

	if( l_Status )
	{
		SVAutoLockAndReleaseTemplate< SVCriticalSection > l_AutoLock;

		l_Status = l_Status && l_AutoLock.Assign( &m_Lock );
		l_Status = l_Status && SV_IS_KIND_OF( PObject, SVObjectClass );

		if( l_Status )
		{
			SVUniqueObjectEntryStructPtr pUniqueObject = new SVUniqueObjectEntryStruct;

			l_Status = !( pUniqueObject.empty() );

			if( l_Status )
			{
				l_Status = ( CoCreateGuid( &( pUniqueObject->ObjectUID.ToGUID() ) ) == S_OK );

				if( l_Status )
				{
					// SEJ (OLD) pUniqueObject->ObjectList.Add( PObject );
					pUniqueObject->PObject = PObject;
					PObject->outObjectInfo.UniqueObjectID = pUniqueObject->ObjectUID; 
					m_UniqueObjectEntries[ pUniqueObject->ObjectUID ] = pUniqueObject;
				}
			}
		}
	}

	return l_Status;
}



BOOL SVObjectManagerClass::OpenUniqueObjectID( SVObjectClass* PObject )
{
	BOOL l_Status = ( m_State == ReadWrite );

	if( l_Status )
	{
		SVAutoLockAndReleaseTemplate< SVCriticalSection > l_AutoLock;

		l_Status = l_Status && l_AutoLock.Assign( &m_Lock );
		l_Status = l_Status && SV_IS_KIND_OF( PObject, SVObjectClass );

		if( l_Status )
		{
			SVUniqueObjectEntryStructPtr pUniqueObject = getUniqueObjectEntry( PObject->outObjectInfo.UniqueObjectID );

			l_Status = ( pUniqueObject.empty() );

			if( l_Status )
			{
				pUniqueObject = new SVUniqueObjectEntryStruct;

				l_Status = !( pUniqueObject.empty() );

				if( l_Status )
				{
					// SEJ (OLD) pUniqueObject->ObjectList.Add( PObject );
					pUniqueObject->PObject = PObject;
					pUniqueObject->ObjectUID = PObject->outObjectInfo.UniqueObjectID; 
					m_UniqueObjectEntries[ pUniqueObject->ObjectUID ] = pUniqueObject;
				}
			}
		}
	}

	return l_Status;
}

BOOL SVObjectManagerClass::CloseUniqueObjectID( SVObjectClass* PObject )
{
	BOOL l_Status = ( m_State == ReadWrite );

	if( l_Status )
	{
		SVAutoLockAndReleaseTemplate< SVCriticalSection > l_AutoLock;

		l_Status = l_Status && l_AutoLock.Assign( &m_Lock );
		l_Status = l_Status && SV_IS_KIND_OF( PObject, SVObjectClass );

		if( l_Status )
		{
			DetachObservers( PObject->GetUniqueObjectID() );
			DetachSubjects( PObject->GetUniqueObjectID() );

			SVUniqueObjectEntryMap::iterator l_Iter( m_UniqueObjectEntries.find( PObject->GetUniqueObjectID() ) );

			if( l_Iter != m_UniqueObjectEntries.end() )
			{
				m_UniqueObjectEntries.erase( l_Iter );
			}
		}
	}

	return l_Status;
}

BOOL SVObjectManagerClass::ChangeUniqueObjectID( SVObjectClass* PObject, const SVGUID& RNewGuid )
{
	if(	RNewGuid != SVInvalidGUID && CloseUniqueObjectID( PObject ) )
	{
		PObject->outObjectInfo.UniqueObjectID = RNewGuid;
		BOOL bRetVal = OpenUniqueObjectID( PObject );

		// Change ObjectID setting in private input interface...
		PObject->ResetPrivateInputInterface();

		return bRetVal;
	}
	return FALSE;
}

SVObjectClass* SVObjectManagerClass::GetObject( const SVGUID& RGuid )
{
	SVObjectClass* pObject = NULL;
	SVAutoLockAndReleaseTemplate< SVCriticalSection > l_AutoLock;
	BOOL l_Status = ( RGuid != SVInvalidGUID );

	if( l_Status && m_State == ReadWrite )
	{
		l_Status = l_AutoLock.Assign( &m_Lock );
	}

	if( l_Status )
	{
		SVUniqueObjectEntryStructPtr pUniqueObject = getUniqueObjectEntry( RGuid );

		if( !( pUniqueObject.empty() ) )
		{
			pObject = pUniqueObject->PObject;
		}
	}

	return pObject;
}

/*
SVObjectClass* SVObjectManagerClass::GetObject( LPCTSTR tszName )
{
	SVObjectClass* pObject = NULL;
	SVAutoLockAndReleaseTemplate< SVCriticalSection > l_AutoLock;
	CString sName = tszName;
	BOOL l_Status = !( sName.IsEmpty() );

	if( l_Status && m_State == ReadWrite )
	{
		l_Status = l_AutoLock.Assign( &m_Lock );
	}

	if( l_Status )
	{
		SVUniqueObjectEntryStructPtr pUniqueObject = getUniqueObjectEntry( sName );

		if( !( pUniqueObject.empty() ) )
		{
			pObject = pUniqueObject->PObject;
		}
	}

	return pObject;
}
*/

SVObjectClass* SVObjectManagerClass::GetObjectCompleteName( LPCTSTR tszName )
{
	SVObjectClass* pObject = NULL;
	SVAutoLockAndReleaseTemplate< SVCriticalSection > l_AutoLock;
	CString sName = tszName;
	BOOL l_Status = !( sName.IsEmpty() );

	if( l_Status && m_State == ReadWrite )
	{
		l_Status = l_AutoLock.Assign( &m_Lock );
	}

	if( l_Status )
	{
		SVObjectClass* l_pConfig = GetObject( m_ConfigurationID );

		if( l_pConfig != NULL )
		{
			SVObjectNameInfo l_NameInfo;

			l_NameInfo.ParseObjectName( static_cast< LPCTSTR >( sName ) );

			l_Status = l_pConfig->GetChildObject( pObject, l_NameInfo ) == S_OK;
		}
	}

	return pObject;
}

SVObjectReference SVObjectManagerClass::GetObjectReference( const CString& strName )
{
	SVObjectReference refObject;

	SVObjectNameInfo l_NameInfo;

	if( l_NameInfo.ParseObjectName( static_cast< LPCTSTR >( strName ) ) == S_OK )
	{
		SVObjectClass* l_pObject = GetObjectCompleteName( l_NameInfo.GetObjectName().c_str() );
		if ( l_pObject != NULL )
		{
			refObject = SVObjectReference( l_pObject, l_NameInfo );
		}
	}

	return refObject;
}

SVGUID SVObjectManagerClass::GetObjectIdFromCompleteName( const SVString& p_rName )
{
	SVGUID l_ObjectID;

	SVObjectClass* l_pObject = GetObjectCompleteName( p_rName.c_str() );

	if( l_pObject != NULL )
	{
		l_ObjectID = l_pObject->GetUniqueObjectID();
	}

	return l_ObjectID;
}

SVString SVObjectManagerClass::GetCompleteObjectName( const SVGUID& RGuid )
{
	SVString l_Name;

	SVObjectClass* l_pObject = GetObject( RGuid );

	if( l_pObject != NULL )
	{
		l_Name = static_cast< LPCTSTR >( l_pObject->GetCompleteObjectName() );
	}

	return l_Name;
}

HRESULT SVObjectManagerClass::SubmitCommand( const SVGUID& p_rObjectID, const SVCommandTemplatePtr& p_rCommandPtr )
{
	HRESULT l_Status = S_OK;

	SVObjectClass* l_pObject = GetObject( p_rObjectID );

	if( l_pObject != NULL )
	{
		SVObjectSubmitCommandFacade* l_pCommandTarget = dynamic_cast< SVObjectSubmitCommandFacade* >( l_pObject );

		if( l_pCommandTarget != NULL )
		{
			l_Status = l_pCommandTarget->SubmitCommand( p_rCommandPtr );
		}
		else
		{
			l_Status = E_FAIL;
		}
	}
	else
	{
		l_Status = E_FAIL;
	}

	return l_Status;
}

HRESULT SVObjectManagerClass::InsertObserver( SVObserverNotificationFunctorPtr p_FunctorPtr, long& p_rCookie )
{
	HRESULT l_Status = S_OK;

	SVAutoLockAndReleaseTemplate< SVCriticalSection > l_AutoLock;

	if( m_State == ReadWrite && l_AutoLock.Assign( &m_Lock ) )
	{
		p_rCookie = ::InterlockedExchangeAdd( &m_ObserverCookie, 1 );

		m_CookieEntries[ p_rCookie ] = new SVCookieEntryStruct( p_rCookie, p_FunctorPtr );
	}
	else
	{
		l_Status = E_FAIL;
	}

	return l_Status;
}

HRESULT SVObjectManagerClass::EraseObserver( long p_Cookie )
{
	HRESULT l_Status = S_OK;

	SVAutoLockAndReleaseTemplate< SVCriticalSection > l_AutoLock;

	if( m_State == ReadWrite && l_AutoLock.Assign( &m_Lock ) )
	{
		DetachSubjects( p_Cookie );

		m_CookieEntries.erase( p_Cookie );
	}
	else
	{
		l_Status = E_FAIL;
	}

	return l_Status;
}

HRESULT SVObjectManagerClass::GetObserverSubject( const SVString& p_rSubjectDataName, const SVGUID& p_rObserverID, GUID& p_rSubjectID ) const
{
	HRESULT l_Status = S_OK;

	p_rSubjectID = GetSubjectID( p_rSubjectDataName, getUniqueObjectEntry( p_rObserverID ) );

	if( SVGUID( p_rSubjectID ).empty() )
	{
		l_Status = S_FALSE;
	}

	return l_Status;
}

HRESULT SVObjectManagerClass::GetObserverSubject( const SVString& p_rSubjectDataName, long p_Cookie, GUID& p_rSubjectID ) const
{
	HRESULT l_Status = S_OK;

	p_rSubjectID = GetSubjectID( p_rSubjectDataName, GetCookieEntry( p_Cookie ) );

	if( SVGUID( p_rSubjectID ).empty() )
	{
		l_Status = S_FALSE;
	}

	return l_Status;
}

HRESULT SVObjectManagerClass::GetObserverIds( const SVString& p_rSubjectDataName, const SVGUID& p_rSubjectID, SVObserverIdSet& p_rObserverIds )
{
	p_rObserverIds.clear();

	SVSubjectEnabledObserverMap l_Observers;

	HRESULT l_Status = GetObservers( p_rSubjectDataName, p_rSubjectID, l_Observers );

	if( l_Status == S_OK )
	{
		SVSubjectEnabledObserverMap::iterator l_Iter;

		for( l_Iter = l_Observers.begin(); l_Iter != l_Observers.end(); ++l_Iter )
		{
			if( l_Iter->second == 1 )
			{
				p_rObserverIds.insert( l_Iter->first );
			}
		}
	}

	return l_Status;
}

HRESULT SVObjectManagerClass::AttachObserver( const SVString& p_rSubjectDataName, const SVGUID& p_rSubjectID, const SVGUID& p_rObserverID )
{
	return AttachObserver( p_rSubjectDataName, p_rSubjectID, p_rObserverID, SVMaterials() );
}

HRESULT SVObjectManagerClass::AttachObserver( const SVString& p_rSubjectDataName, const SVGUID& p_rSubjectID, const SVGUID& p_rObserverID, const SVMaterials& p_rAttributes )
{
	HRESULT l_Status = S_OK;

	SVAutoLockAndReleaseTemplate< SVCriticalSection > l_AutoLock;

	if( m_State == ReadWrite && l_AutoLock.Assign( &m_Lock ) )
	{
		SVUniqueObjectEntryStructPtr pSubjectObject = getUniqueObjectEntry( p_rSubjectID );
		SVUniqueObjectEntryStructPtr pObserverObject = getUniqueObjectEntry( p_rObserverID );
		
		if( !( pSubjectObject.empty() ) && !( pObserverObject.empty() ) )
		{
			SVGUID l_SubjectID = GetSubjectID( p_rSubjectDataName, pObserverObject );

			if( !( l_SubjectID.empty() ) )
			{
				DetachObserver( p_rSubjectDataName, l_SubjectID, p_rObserverID );
			}

			pSubjectObject->m_DataNameSubjectObservers[ p_rSubjectDataName ].m_SubjectObservers[ p_rObserverID ] = 1;
			pObserverObject->m_SubjectIDs[ p_rSubjectDataName ] = p_rSubjectID;

			Notify( p_rSubjectID, SVSubjectConnectData( p_rSubjectDataName, p_rObserverID, p_rAttributes ) );
			Notify( p_rObserverID, SVObserverConnectData( p_rSubjectDataName, p_rSubjectID, p_rAttributes ) );
		}
		else
		{
			l_Status = E_FAIL;
		}
	}
	else
	{
		l_Status = E_FAIL;
	}

	return l_Status;
}

HRESULT SVObjectManagerClass::AttachObserver( const SVString& p_rSubjectDataName, const SVGUID& p_rSubjectID, long p_Cookie )
{
	HRESULT l_Status = S_OK;

	SVAutoLockAndReleaseTemplate< SVCriticalSection > l_AutoLock;

	if( m_State == ReadWrite && l_AutoLock.Assign( &m_Lock ) )
	{
		SVUniqueObjectEntryStructPtr pSubjectObject = getUniqueObjectEntry( p_rSubjectID );
		SVCookieEntryStructPtr pObserverObject = GetCookieEntry( p_Cookie );
		
		if( !( pSubjectObject.empty() ) && !( pObserverObject.empty() ) )
		{
			SVGUID l_SubjectID = GetSubjectID( p_rSubjectDataName, pObserverObject );

			if( !( l_SubjectID.empty() ) )
			{
				DetachObserver( p_rSubjectDataName, l_SubjectID, p_Cookie );
			}

			pSubjectObject->m_DataNameSubjectObservers[ p_rSubjectDataName ].m_SubjectCookies[ p_Cookie ] = 1;
			pObserverObject->m_SubjectIDs[ p_rSubjectDataName ] = p_rSubjectID;
		}
		else
		{
			l_Status = E_FAIL;
		}
	}
	else
	{
		l_Status = E_FAIL;
	}

	return l_Status;
}

HRESULT SVObjectManagerClass::EnableObserver( const SVString& p_rSubjectDataName, const SVGUID& p_rSubjectID, const SVGUID& p_rObserverID )
{
	HRESULT l_Status = S_OK;

	SVAutoLockAndReleaseTemplate< SVCriticalSection > l_AutoLock;

	if( m_State == ReadWrite )
	{
		if( !( l_AutoLock.Assign( &m_Lock ) ) )
		{
			l_Status = E_FAIL;
		}
	}

	if( l_Status == S_OK )
	{
		SVUniqueObjectEntryStructPtr pSubjectObject = getUniqueObjectEntry( p_rSubjectID );
		
		if( !( pSubjectObject.empty() ) )
		{
			pSubjectObject->m_DataNameSubjectObservers[ p_rSubjectDataName ].m_SubjectObservers[ p_rObserverID ] = 1;
		}
		else
		{
			l_Status = E_FAIL;
		}
	}

	return l_Status;
}

HRESULT SVObjectManagerClass::EnableObserver( const SVString& p_rSubjectDataName, const SVGUID& p_rSubjectID, long p_Cookie )
{
	HRESULT l_Status = S_OK;

	SVAutoLockAndReleaseTemplate< SVCriticalSection > l_AutoLock;

	if( m_State == ReadWrite )
	{
		if( !( l_AutoLock.Assign( &m_Lock ) ) )
		{
			l_Status = E_FAIL;
		}
	}

	if( l_Status == S_OK )
	{
		SVUniqueObjectEntryStructPtr pSubjectObject = getUniqueObjectEntry( p_rSubjectID );
		
		if( !( pSubjectObject.empty() ) )
		{
			pSubjectObject->m_DataNameSubjectObservers[ p_rSubjectDataName ].m_SubjectCookies[ p_Cookie ] = 1;
		}
		else
		{
			l_Status = E_FAIL;
		}
	}

	return l_Status;
}

HRESULT SVObjectManagerClass::DisableObserver( const SVString& p_rSubjectDataName, const SVGUID& p_rSubjectID, const SVGUID& p_rObserverID )
{
	HRESULT l_Status = S_OK;

	SVAutoLockAndReleaseTemplate< SVCriticalSection > l_AutoLock;

	if( m_State == ReadWrite )
	{
		if( !( l_AutoLock.Assign( &m_Lock ) ) )
		{
			l_Status = E_FAIL;
		}
	}

	if( l_Status == S_OK )
	{
		SVUniqueObjectEntryStructPtr pSubjectObject = getUniqueObjectEntry( p_rSubjectID );
		
		if( !( pSubjectObject.empty() ) )
		{
			pSubjectObject->m_DataNameSubjectObservers[ p_rSubjectDataName ].m_SubjectObservers[ p_rObserverID ] = 0;
		}
		else
		{
			l_Status = E_FAIL;
		}
	}

	return l_Status;
}

HRESULT SVObjectManagerClass::DisableObserver( const SVString& p_rSubjectDataName, const SVGUID& p_rSubjectID, long p_Cookie )
{
	HRESULT l_Status = S_OK;

	SVAutoLockAndReleaseTemplate< SVCriticalSection > l_AutoLock;

	if( m_State == ReadWrite )
	{
		if( !( l_AutoLock.Assign( &m_Lock ) ) )
		{
			l_Status = E_FAIL;
		}
	}

	if( l_Status == S_OK )
	{
		SVUniqueObjectEntryStructPtr pSubjectObject = getUniqueObjectEntry( p_rSubjectID );
		
		if( !( pSubjectObject.empty() ) )
		{
			pSubjectObject->m_DataNameSubjectObservers[ p_rSubjectDataName ].m_SubjectCookies[ p_Cookie ] = 0;
		}
		else
		{
			l_Status = E_FAIL;
		}
	}

	return l_Status;
}

HRESULT SVObjectManagerClass::DetachObserver( const SVString& p_rSubjectDataName, const SVGUID& p_rSubjectID, const SVGUID& p_rObserverID )
{
	HRESULT l_Status = S_OK;

	SVAutoLockAndReleaseTemplate< SVCriticalSection > l_AutoLock;

	if( m_State == ReadWrite && l_AutoLock.Assign( &m_Lock ) )
	{
		SVUniqueObjectEntryStructPtr pObserverObject = getUniqueObjectEntry( p_rObserverID );
		
		if( !( pObserverObject.empty() ) )
		{
			Notify( p_rObserverID, SVObserverDisconnectData( p_rSubjectDataName, p_rSubjectID ) );

			pObserverObject->m_SubjectIDs.erase( p_rSubjectDataName );
		}

		SVUniqueObjectEntryStructPtr pSubjectObject = getUniqueObjectEntry( p_rSubjectID );
		
		if( !( pSubjectObject.empty() ) )
		{
			Notify( p_rSubjectID, SVSubjectDisconnectData( p_rSubjectDataName, p_rObserverID ) );

			pSubjectObject->m_DataNameSubjectObservers[ p_rSubjectDataName ].m_SubjectObservers.erase( p_rObserverID );
		}
	}
	else
	{
		l_Status = E_FAIL;
	}

	return l_Status;
}

HRESULT SVObjectManagerClass::DetachObserver( const SVString& p_rSubjectDataName, const SVGUID& p_rSubjectID, long p_Cookie )
{
	HRESULT l_Status = S_OK;

	SVAutoLockAndReleaseTemplate< SVCriticalSection > l_AutoLock;

	if( m_State == ReadWrite && l_AutoLock.Assign( &m_Lock ) )
	{
		SVCookieEntryStructPtr pObserverObject = GetCookieEntry( p_Cookie );
		
		if( !( pObserverObject.empty() ) )
		{
			pObserverObject->m_SubjectIDs.erase( p_rSubjectDataName );
		}

		SVUniqueObjectEntryStructPtr pSubjectObject = getUniqueObjectEntry( p_rSubjectID );
		
		if( !( pSubjectObject.empty() ) )
		{
			pSubjectObject->m_DataNameSubjectObservers[ p_rSubjectDataName ].m_SubjectCookies.erase( p_Cookie );
		}
	}
	else
	{
		l_Status = E_FAIL;
	}

	return l_Status;
}

HRESULT SVObjectManagerClass::DetachObservers( const SVString& p_rSubjectDataName, const SVGUID& p_rSubjectID )
{
	HRESULT l_Status = S_OK;

	SVSubjectEnabledObserverMap l_Observers;
	SVSubjectEnabledCookieMap l_CookieObservers;

	l_Status = GetObservers( p_rSubjectDataName, p_rSubjectID, l_Observers, l_CookieObservers );

	if( l_Status == S_OK )
	{
		SVSubjectEnabledObserverMap::iterator l_Iter = l_Observers.begin();

		while( l_Iter != l_Observers.end() )
		{
			HRESULT l_Temp = DetachObserver( p_rSubjectDataName, p_rSubjectID, l_Iter->first ) ;

			if( l_Status == S_OK )
			{
				l_Status = l_Temp;
			}

			++l_Iter;
		}

		SVSubjectEnabledCookieMap::iterator l_CookieIter = l_CookieObservers.begin();

		while( l_CookieIter != l_CookieObservers.end() )
		{
			HRESULT l_Temp = DetachObserver( p_rSubjectDataName, p_rSubjectID, l_CookieIter->first ) ;

			if( l_Status == S_OK )
			{
				l_Status = l_Temp;
			}

			++l_CookieIter;
		}
	}

	return l_Status;
}

HRESULT SVObjectManagerClass::DetachSubjectsAndObservers( const SVGUID& p_rObjectID )
{
	HRESULT l_Status = DetachObservers( p_rObjectID );
	HRESULT l_SubjectStatus = DetachSubjects( p_rObjectID );

	if( l_Status == S_OK )
	{
		l_Status = l_SubjectStatus;
	}

	return l_Status;
}

HRESULT SVObjectManagerClass::DetachSubjects( long p_Cookie )
{
	HRESULT l_Status = S_OK;

	SVSubjectDataNameDeque l_SubjectDataNames;

	l_Status = GetObserverDataNames( p_Cookie, l_SubjectDataNames );

	if( l_Status == S_OK )
	{
		SVSubjectDataNameDeque::iterator l_Iter;
		
		for( l_Iter = l_SubjectDataNames.begin(); l_Iter != l_SubjectDataNames.end(); ++l_Iter )
		{
			SVGUID l_SubjectId;

			GetObserverSubject( *l_Iter, p_Cookie, l_SubjectId );

			DetachObserver( *l_Iter, l_SubjectId, p_Cookie );
		}
	}

	return l_Status;
}

HRESULT SVObjectManagerClass::DetachSubjects( const SVGUID& p_rObserverID )
{
	HRESULT l_Status = S_OK;

	SVSubjectDataNameDeque l_SubjectDataNames;

	l_Status = GetObserverDataNames( p_rObserverID, l_SubjectDataNames );

	if( l_Status == S_OK )
	{
		SVSubjectDataNameDeque::iterator l_Iter;
		
		for( l_Iter = l_SubjectDataNames.begin(); l_Iter != l_SubjectDataNames.end(); ++l_Iter )
		{
			SVGUID l_SubjectId;

			GetObserverSubject( *l_Iter, p_rObserverID, l_SubjectId );

			DetachObserver( *l_Iter, l_SubjectId, p_rObserverID );
		}
	}

	return l_Status;
}

HRESULT SVObjectManagerClass::DetachObservers( const SVGUID& p_rSubjectID )
{
	HRESULT l_Status = S_OK;

	SVSubjectDataNameDeque l_SubjectDataNames;

	l_Status = GetSubjectDataNames( p_rSubjectID, l_SubjectDataNames );

	if( l_Status == S_OK )
	{
		SVSubjectDataNameDeque::iterator l_Iter;
		
		for( l_Iter = l_SubjectDataNames.begin(); l_Iter != l_SubjectDataNames.end(); ++l_Iter )
		{
			HRESULT l_Temp = DetachObservers( *l_Iter, p_rSubjectID ) ;

			if( l_Status == S_OK )
			{
				l_Status = l_Temp;
			}
		}
	}

	return l_Status;
}

HRESULT SVObjectManagerClass::DisconnectObjects( const SVGUID& p_rObjectID, const SVGUID& p_rRemoteID )
{
	HRESULT l_Status = S_OK;

	SVObjectClass* l_pObject = GetObject( p_rObjectID );

	if( l_pObject != NULL )
	{
		l_Status = l_pObject->RemoveObjectConnection( p_rRemoteID );
	}

	l_pObject = GetObject( p_rRemoteID );

	if( l_pObject != NULL )
	{
		HRESULT l_Temp = l_pObject->RemoveObjectConnection( p_rObjectID );

		if( l_Status == S_OK )
		{
			l_Status = l_Temp;
		}
	}

	return l_Status;
}

SVIPDoc* SVObjectManagerClass::GetIPDoc( const SVGUID& p_rIPGuid ) const
{
	SVIPDoc* l_pIPDoc( NULL );

	SVAutoLockAndReleaseTemplate< SVCriticalSection > l_AutoLock;

	BOOL l_Status = true;

	if( m_State == ReadWrite )
	{
		l_Status = l_AutoLock.Assign( &m_Lock );
	}

	if( l_Status )
	{
		SVIPDocMap::const_iterator l_Iter( m_IPDocs.find( p_rIPGuid ) );

		if( l_Iter != m_IPDocs.end() )
		{
			l_pIPDoc = l_Iter->second;
		}
	}

	return l_pIPDoc;
}

HRESULT SVObjectManagerClass::RegisterIPDoc( const SVGUID& p_rIPGuid, SVIPDoc* p_pIPDoc )
{
	HRESULT l_Status( S_OK );

	SVAutoLockAndReleaseTemplate< SVCriticalSection > l_AutoLock;

	if( m_State == ReadWrite && l_AutoLock.Assign( &m_Lock ) )
	{
		SVIPDocMap::iterator l_Iter( m_IPDocs.find( p_rIPGuid ) );

		if( l_Iter == m_IPDocs.end() )
		{
			m_IPDocs[ p_rIPGuid ] = p_pIPDoc;
		}
	}
	else
	{
		l_Status = E_FAIL;
	}

	return l_Status;
}

HRESULT SVObjectManagerClass::UnregisterIPDoc( const SVGUID& p_rIPGuid )
{
	HRESULT l_Status( S_OK );

	SVAutoLockAndReleaseTemplate< SVCriticalSection > l_AutoLock;

	if( m_State == ReadWrite && l_AutoLock.Assign( &m_Lock ) )
	{
		m_IPDocs.erase( p_rIPGuid );
	}
	else
	{
		l_Status = E_FAIL;
	}

	return l_Status;
}

SVIODoc* SVObjectManagerClass::GetIODoc( const SVGUID& p_rIOGuid ) const
{
	SVIODoc* l_pIODoc( NULL );

	SVAutoLockAndReleaseTemplate< SVCriticalSection > l_AutoLock;

	BOOL l_Status = true;

	if( m_State == ReadWrite )
	{
		l_Status = l_AutoLock.Assign( &m_Lock );
	}

	if( l_Status )
	{
		SVIODocMap::const_iterator l_Iter( m_IODocs.find( p_rIOGuid ) );

		if( l_Iter != m_IODocs.end() )
		{
			l_pIODoc = l_Iter->second;
		}
	}

	return l_pIODoc;
}

HRESULT SVObjectManagerClass::RegisterIODoc( const SVGUID& p_rIOGuid, SVIODoc* p_pIODoc )
{
	HRESULT l_Status( S_OK );

	SVAutoLockAndReleaseTemplate< SVCriticalSection > l_AutoLock;

	if( m_State == ReadWrite && l_AutoLock.Assign( &m_Lock ) )
	{
		SVIODocMap::iterator l_Iter( m_IODocs.find( p_rIOGuid ) );

		if( l_Iter == m_IODocs.end() )
		{
			m_IODocs[ p_rIOGuid ] = p_pIODoc;
		}
	}
	else
	{
		l_Status = E_FAIL;
	}

	return l_Status;
}

HRESULT SVObjectManagerClass::UnregisterIODoc( const SVGUID& p_rIOGuid )
{
	HRESULT l_Status( S_OK );

	SVAutoLockAndReleaseTemplate< SVCriticalSection > l_AutoLock;

	if( m_State == ReadWrite && l_AutoLock.Assign( &m_Lock ) )
	{
		m_IODocs.erase( p_rIOGuid );
	}
	else
	{
		l_Status = E_FAIL;
	}

	return l_Status;
}

int SVObjectManagerClass::GetOnlineDisplay() const
{
	return m_OnlineDisplay;
}

void SVObjectManagerClass::SetOnlineDisplay( int p_State )
{
	m_OnlineDisplay = p_State;
}

long SVObjectManagerClass::GetShortPPQIndicator() const
{
	return m_ShortPPQIndicator;
}

void SVObjectManagerClass::ClearShortPPQIndicator()
{
	::InterlockedExchange( &m_ShortPPQIndicator, 0 );
}

void SVObjectManagerClass::IncrementShortPPQIndicator()
{
	::InterlockedIncrement( &m_ShortPPQIndicator );
}

void SVObjectManagerClass::DecrementShortPPQIndicator()
{
	::InterlockedDecrement( &m_ShortPPQIndicator );
}

long SVObjectManagerClass::GetProductIndicator() const
{
	return m_ProductIndicator;
}

void SVObjectManagerClass::ClearProductIndicator()
{
	::InterlockedExchange( &m_ProductIndicator, 0 );
}

void SVObjectManagerClass::IncrementProductIndicator()
{
	::InterlockedIncrement( &m_ProductIndicator );
}

void SVObjectManagerClass::DecrementProductIndicator()
{
	::InterlockedDecrement( &m_ProductIndicator );
}

void SVObjectManagerClass::AdjustProductIndicator( long p_Amount )
{
	::InterlockedExchangeAdd( &m_ProductIndicator, p_Amount );
}

long SVObjectManagerClass::GetPendingImageIndicator() const
{
	return m_PendingImageIndicator;
}

void SVObjectManagerClass::ClearPendingImageIndicator()
{
	::InterlockedExchange( &m_PendingImageIndicator, 0 );
}

void SVObjectManagerClass::IncrementPendingImageIndicator()
{
	::InterlockedIncrement( &m_PendingImageIndicator );
}

void SVObjectManagerClass::DecrementPendingImageIndicator()
{
	::InterlockedDecrement( &m_PendingImageIndicator );
}

void SVObjectManagerClass::AdjustPendingImageIndicator( long p_Amount )
{
	::InterlockedExchangeAdd( &m_PendingImageIndicator, p_Amount );
}

long SVObjectManagerClass::GetInspectionIndicator() const
{
	return m_InspectionIndicator;
}

void SVObjectManagerClass::ClearInspectionIndicator()
{
	::InterlockedExchange( &m_InspectionIndicator, 0 );
}

void SVObjectManagerClass::IncrementInspectionIndicator()
{
	::InterlockedIncrement( &m_InspectionIndicator );
}

void SVObjectManagerClass::DecrementInspectionIndicator()
{
	::InterlockedDecrement( &m_InspectionIndicator );
}

void SVObjectManagerClass::AdjustInspectionIndicator( long p_Amount )
{
	::InterlockedExchangeAdd( &m_InspectionIndicator, p_Amount );
}

long SVObjectManagerClass::GetNextFrameRate( long p_LastFrameRate )
{
	long l_FrameRate = m_LastFrameRate;

	::InterlockedExchange( &m_LastFrameRate, p_LastFrameRate );

	return l_FrameRate;
}

long SVObjectManagerClass::GetFileSequenceNumber() const
{
	return m_FileSequenceNumber;
}

SVObjectManagerClass::SVCookieEntryStructPtr SVObjectManagerClass::GetCookieEntry( long p_Cookie ) const
{
	SVCookieEntryStructPtr l_CookieEntryPtr;

	SVAutoLockAndReleaseTemplate< SVCriticalSection > l_AutoLock;

	BOOL l_Status = true;

	if( m_State == ReadWrite )
	{
		l_Status = l_AutoLock.Assign( &m_Lock );
	}

	if( l_Status )
	{
		SVCookieEntryMap::const_iterator l_Iter( m_CookieEntries.find( p_Cookie ) );

		if( l_Iter != m_CookieEntries.end() )
		{
			l_CookieEntryPtr = l_Iter->second;
		}
	}

	return l_CookieEntryPtr;
}

SVGUID SVObjectManagerClass::GetSubjectID( const SVString& p_rSubjectDataName, SVUniqueObjectEntryStructPtr p_ObjectEntryPtr ) const
{
	SVGUID l_SubjectID;

	if( !( p_ObjectEntryPtr.empty() ) )
	{
		SVAutoLockAndReleaseTemplate< SVCriticalSection > l_AutoLock;

		BOOL l_Status = true;

		if( m_State == ReadWrite )
		{
			l_Status = l_AutoLock.Assign( &m_Lock );
		}

		if( l_Status )
		{
			SVSubjectDataNameSubjectIDMap::const_iterator l_Iter = p_ObjectEntryPtr->m_SubjectIDs.find( p_rSubjectDataName );

			if( l_Iter != p_ObjectEntryPtr->m_SubjectIDs.end() )
			{
				l_SubjectID = l_Iter->second;
			}
		}
	}

	return l_SubjectID;
}

SVGUID SVObjectManagerClass::GetSubjectID( const SVString& p_rSubjectDataName, SVCookieEntryStructPtr p_CookieEntryPtr ) const
{
	SVGUID l_SubjectID;

	if( !( p_CookieEntryPtr.empty() ) )
	{
		SVAutoLockAndReleaseTemplate< SVCriticalSection > l_AutoLock;

		BOOL l_Status = true;

		if( m_State == ReadWrite )
		{
			l_Status = l_AutoLock.Assign( &m_Lock );
		}

		if( l_Status )
		{
			SVSubjectDataNameSubjectIDMap::const_iterator l_Iter = p_CookieEntryPtr->m_SubjectIDs.find( p_rSubjectDataName );

			if( l_Iter != p_CookieEntryPtr->m_SubjectIDs.end() )
			{
				l_SubjectID = l_Iter->second;
			}
		}
	}

	return l_SubjectID;
}

SVObjectManagerClass::SVUniqueObjectEntryStructPtr SVObjectManagerClass::getUniqueObjectEntry( const SVGUID& RGuid ) const
{
	SVUniqueObjectEntryStructPtr pUniqueObjectEntry;

	SVAutoLockAndReleaseTemplate< SVCriticalSection > l_AutoLock;

	BOOL l_Status = true;

	if( m_State == ReadWrite )
	{
		l_Status = l_AutoLock.Assign( &m_Lock );
	}

	if( l_Status )
	{
		SVUniqueObjectEntryMap::const_iterator l_Iter( m_UniqueObjectEntries.find( RGuid ) );

		if( l_Iter != m_UniqueObjectEntries.end() )
		{
			pUniqueObjectEntry = l_Iter->second;
		}
	}

	return pUniqueObjectEntry;
}

SVObjectManagerClass::SVUniqueObjectEntryStructPtr SVObjectManagerClass::getUniqueObjectEntry( const CString& sName ) const
{
	SVUniqueObjectEntryStructPtr pUniqueObjectEntry;

	if( !sName.IsEmpty() )
	{
		SVAutoLockAndReleaseTemplate< SVCriticalSection > l_AutoLock;

		BOOL l_Status = true;

		if( m_State == ReadWrite )
		{
			l_Status = l_AutoLock.Assign( &m_Lock );
		}

		if( l_Status )
		{
			SVUniqueObjectEntryMap::const_iterator l_Iter( m_UniqueObjectEntries.begin() );

			while( pUniqueObjectEntry.empty() && l_Iter != m_UniqueObjectEntries.end() )
			{
				pUniqueObjectEntry = l_Iter->second ;

				if( !( pUniqueObjectEntry.empty() ) )
				{
					if( pUniqueObjectEntry->PObject == NULL ||
						pUniqueObjectEntry->PObject->GetName() != sName )
					{
						pUniqueObjectEntry.clear();
					}
				}

				if( pUniqueObjectEntry.empty() )
				{
					++l_Iter;
				}
			}
		}
	}

	return pUniqueObjectEntry;
}

SVObjectManagerClass::SVUniqueObjectEntryStructPtr SVObjectManagerClass::getUniqueObjectEntryCompleteObjectName( const CString& sName ) const
{
	SVUniqueObjectEntryStructPtr pUniqueObjectEntry;

	if( !sName.IsEmpty() )
	{
		SVAutoLockAndReleaseTemplate< SVCriticalSection > l_AutoLock;

		BOOL l_Status = true;

		if( m_State == ReadWrite )
		{
			l_Status = l_AutoLock.Assign( &m_Lock );
		}

		if( l_Status )
		{
			SVUniqueObjectEntryMap::const_iterator l_Iter( m_UniqueObjectEntries.begin() );

			while( pUniqueObjectEntry.empty() && l_Iter != m_UniqueObjectEntries.end() )
			{
				pUniqueObjectEntry = l_Iter->second ;

				if( !( pUniqueObjectEntry.empty() ) )
				{
					if( pUniqueObjectEntry->PObject == NULL ||
						pUniqueObjectEntry->PObject->GetCompleteObjectName() != sName )
					{
						pUniqueObjectEntry.clear();
					}
				}

				if( pUniqueObjectEntry.empty() )
				{
					++l_Iter;
				}
			}
		}
	}

	return pUniqueObjectEntry;
}

HRESULT SVObjectManagerClass::GetSubjectDataNames( const SVGUID& p_rSubjectID, SVSubjectDataNameDeque& p_rSubjectDataNames ) const
{
	HRESULT l_Status = S_OK;

	p_rSubjectDataNames.clear();

	SVUniqueObjectEntryStructPtr pUniqueObject = getUniqueObjectEntry( p_rSubjectID );

	if( !( pUniqueObject.empty() ) )
	{
		SVAutoLockAndReleaseTemplate< SVCriticalSection > l_AutoLock;

		BOOL l_LockStatus = true;

		if( m_State == ReadWrite )
		{
			l_LockStatus = l_AutoLock.Assign( &m_Lock );
		}

		if( l_LockStatus )
		{
			SVSubjectDataNameObserverMap::const_iterator l_Iter;
			
			for( l_Iter = pUniqueObject->m_DataNameSubjectObservers.begin(); l_Iter != pUniqueObject->m_DataNameSubjectObservers.end(); ++l_Iter )
			{
				p_rSubjectDataNames.push_back( l_Iter->first );
			}
		}
		else
		{
			l_Status = E_FAIL;
		}
	}
	else
	{
		l_Status = E_FAIL;
	}

	return l_Status;
}

HRESULT SVObjectManagerClass::GetObserverDataNames( long p_Cookie, SVSubjectDataNameDeque& p_rSubjectDataNames ) const
{
	HRESULT l_Status = S_OK;

	p_rSubjectDataNames.clear();

	SVCookieEntryStructPtr pCookie = GetCookieEntry( p_Cookie );

	if( !( pCookie.empty() ) )
	{
		SVAutoLockAndReleaseTemplate< SVCriticalSection > l_AutoLock;

		BOOL l_LockStatus = true;

		if( m_State == ReadWrite )
		{
			l_LockStatus = l_AutoLock.Assign( &m_Lock );
		}

		if( l_LockStatus )
		{
			SVSubjectDataNameSubjectIDMap::const_iterator l_Iter;
			
			for( l_Iter = pCookie->m_SubjectIDs.begin(); l_Iter != pCookie->m_SubjectIDs.end(); ++l_Iter )
			{
				p_rSubjectDataNames.push_back( l_Iter->first );
			}
		}
		else
		{
			l_Status = E_FAIL;
		}
	}
	else
	{
		l_Status = E_FAIL;
	}

	return l_Status;
}

HRESULT SVObjectManagerClass::GetObserverDataNames( const SVGUID& p_rObserverID, SVSubjectDataNameDeque& p_rSubjectDataNames ) const
{
	HRESULT l_Status = S_OK;

	p_rSubjectDataNames.clear();

	SVUniqueObjectEntryStructPtr pUniqueObject = getUniqueObjectEntry( p_rObserverID );

	if( !( pUniqueObject.empty() ) )
	{
		SVAutoLockAndReleaseTemplate< SVCriticalSection > l_AutoLock;

		BOOL l_LockStatus = true;

		if( m_State == ReadWrite )
		{
			l_LockStatus = l_AutoLock.Assign( &m_Lock );
		}

		if( l_LockStatus )
		{
			SVSubjectDataNameSubjectIDMap::const_iterator l_Iter;
			
			for( l_Iter = pUniqueObject->m_SubjectIDs.begin(); l_Iter != pUniqueObject->m_SubjectIDs.end(); ++l_Iter )
			{
				p_rSubjectDataNames.push_back( l_Iter->first );
			}
		}
		else
		{
			l_Status = E_FAIL;
		}
	}
	else
	{
		l_Status = E_FAIL;
	}

	return l_Status;
}

HRESULT SVObjectManagerClass::GetObservers( const SVString& p_rSubjectDataName, const SVGUID& p_rSubjectID, SVSubjectEnabledObserverMap& p_rObservers )
{
	HRESULT l_Status = S_OK;

	p_rObservers.clear();

	SVUniqueObjectEntryStructPtr pUniqueObject = getUniqueObjectEntry( p_rSubjectID );

	if( !( pUniqueObject.empty() ) )
	{
		SVAutoLockAndReleaseTemplate< SVCriticalSection > l_AutoLock;

		BOOL l_LockStatus = true;

		if( m_State == ReadWrite )
		{
			l_LockStatus = l_AutoLock.Assign( &m_Lock );
		}

		if( l_LockStatus )
		{
			SVSubjectDataNameObserverMap::iterator l_Iter = pUniqueObject->m_DataNameSubjectObservers.find( p_rSubjectDataName );

			if( l_Iter != pUniqueObject->m_DataNameSubjectObservers.end() )
			{
				p_rObservers = l_Iter->second.m_SubjectObservers;
			}
			else
			{
				l_Status = E_FAIL;
			}
		}
		else
		{
			l_Status = E_FAIL;
		}
	}
	else
	{
		l_Status = E_FAIL;
	}

	return l_Status;
}

HRESULT SVObjectManagerClass::GetObservers( const SVString& p_rSubjectDataName, const SVGUID& p_rSubjectID, SVSubjectEnabledObserverMap& p_rObservers, SVSubjectEnabledCookieMap& p_rObserverCookies )
{
	HRESULT l_Status = S_OK;

	p_rObservers.clear();

	SVUniqueObjectEntryStructPtr pUniqueObject = getUniqueObjectEntry( p_rSubjectID );

	if( !( pUniqueObject.empty() ) )
	{
		SVAutoLockAndReleaseTemplate< SVCriticalSection > l_AutoLock;

		BOOL l_LockStatus = true;

		if( m_State == ReadWrite )
		{
			l_LockStatus = l_AutoLock.Assign( &m_Lock );
		}

		if( l_LockStatus )
		{
			SVSubjectDataNameObserverMap::iterator l_Iter = pUniqueObject->m_DataNameSubjectObservers.find( p_rSubjectDataName );

			if( l_Iter != pUniqueObject->m_DataNameSubjectObservers.end() )
			{
				p_rObservers = l_Iter->second.m_SubjectObservers;
				p_rObserverCookies = l_Iter->second.m_SubjectCookies;
			}
			else
			{
				l_Status = E_FAIL;
			}
		}
		else
		{
			l_Status = E_FAIL;
		}
	}
	else
	{
		l_Status = E_FAIL;
	}

	return l_Status;
}

// EOF //

//******************************************************************************
//* LOG HISTORY:
//******************************************************************************
/*
$Log:   N:\PVCSarch65\ProjectFiles\archives\SVObserver_src\SVObjectLibrary\SVObjectManagerClass.cpp_v  $
 * 
 *    Rev 1.0   22 Apr 2013 16:57:38   bWalter
 * Project:  SVObserver
 * Change Request (SCR) nbr:  814
 * SCR Title:  Upgrade SVObserver to Compile Using Visual Studio 2010
 * Checked in by:  bWalter;  Ben Walter
 * Change Description:  
 *   Initial check in to SVObserver_src.  (Merged with svo_src label SVO 6.10 Beta 008.)
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.25   11 Feb 2013 12:19:44   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  812
 * SCR Title:  Add New Remote Command Functionality
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Update object manager to fix issues with subject/observer pattern functionality.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.24   16 Jan 2013 15:55:56   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  802
 * SCR Title:  Add new product type GD1A
 * Checked in by:  bWalter;  Ben Walter
 * Change Description:  
 *   Migrating branch code into primary code base.  Updated type of color flag and initialized it to false.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.23   01 Aug 2012 11:35:28   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  769
 * SCR Title:  Fix Problems and Crashes with Inspection Document Display Updates
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Added new command functionality to allow for synchronous command execution through the inspection thread.  This will fix problems with race conditions with the inspection process.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.22   18 Jul 2012 13:15:40   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  769
 * SCR Title:  Fix Problems and Crashes with Inspection Document Display Updates
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Removed unused include.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.21   09 Jul 2012 13:51:44   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  769
 * SCR Title:  Fix Problems and Crashes with Inspection Document Display Updates
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated source code to remove display attributes and SVIM State functoinality.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.20   02 Jul 2012 16:02:28   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  769
 * SCR Title:  Fix Problems and Crashes with Inspection Document Display Updates
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated source code to promote new display functionality.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.19   18 Jun 2012 17:31:26   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  744
 * SCR Title:  Add Shared Memory and Socket Functionality for Run Page Web Server
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Separated dotted name parsing and management to its own class.  This allows for the use of this functionality without have to involve the Reference Class.
 * Optimized the application state functionality.  The application state class was altered to simplify its usage and elements were added to object manager to separate tracking states.
 * Centralized the location of the configuration object identifier and removed synchronization problems.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.18   24 May 2012 13:42:28   jHanebach
 * Project:  SVObserver
 * Change Request (SCR) nbr:  768
 * SCR Title:  Add the display time-out and throttling options for 5.50
 * Checked in by:  tBair;  Tom Bair
 * Change Description:  
 *   Added ResetTimeout on changing online display option.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.17   23 May 2012 13:20:02   jHanebach
 * Project:  SVObserver
 * Change Request (SCR) nbr:  768
 * SCR Title:  Add the display time-out and throttling options for 5.50
 * Checked in by:  tBair;  Tom Bair
 * Change Description:  
 *   Fixed codition in MinutesTillTimeout
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.16   16 May 2012 19:00:28   jHanebach
 * Project:  SVObserver
 * Change Request (SCR) nbr:  763
 * SCR Title:  Add the display time-out and throttling options.
 * Checked in by:  tBair;  Tom Bair
 * Change Description:  
 *   Merge 5.43 changes
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.15   10 Feb 2012 09:25:16   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  747
 * SCR Title:  Add New Remote Input Tool to SVObserver
 * Checked in by:  jSpila;  Joseph Spila
 * Change Description:  
 *   Updated functionality to remove bugs associated with the remote input tool and communications to that tool.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.14   30 Jan 2012 10:33:56   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  748
 * SCR Title:  Add Remote Output Steams to SVObserver
 * Checked in by:  jSpila;  Joseph Spila
 * Change Description:  
 *   Updated object manager to reduce unnecessary methods.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.13   16 Jan 2012 15:04:34   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  747
 * SCR Title:  Add New Remote Input Tool to SVObserver
 * Checked in by:  jSpila;  Joseph Spila
 * Change Description:  
 *   Updated object manager to include the new notification functionality to managed object.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.12.1.1   15 May 2012 10:34:18   tbair
 * Project:  SVObserver
 * Change Request (SCR) nbr:  763
 * SCR Title:  Add the display time-out and throttling options.
 * Checked in by:  tBair;  Tom Bair
 * Change Description:  
 *   Set ini path for display timeout to c:\SVObserver\bin\SVIM.ini
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.12.1.0   14 May 2012 13:36:04   tbair
 * Project:  SVObserver
 * Change Request (SCR) nbr:  763
 * SCR Title:  Add the display time-out and throttling options.
 * Checked in by:  tBair;  Tom Bair
 * Change Description:  
 *   Added functionallity for display update rate and display timeout feature.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.12   16 Sep 2011 14:35:24   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  730
 * SCR Title:  Adjust SVObserver to fix issues with Inspection resource handshaking
 * Checked in by:  tBair;  Tom Bair
 * Change Description:  
 *   Added a new method to adjust the processing indicator variable.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.11   05 Jul 2011 14:38:40   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  724
 * SCR Title:  Add Resource Logging to SVObserver
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated object manager to include a file sequence number used for the offline log files.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.10   29 Apr 2011 07:57:12   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  707
 * SCR Title:  Change Inspection Display Functionality to Force Display of Last Inspected
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated code to allow for inspection to for the display to give up when an inspection get queued.  The display is now regulated to a maximum of 10 frames per second.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.9   16 Feb 2011 11:59:56   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  707
 * SCR Title:  Change Inspection Display Functionality to Force Display of Last Inspected
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated Object manager to use new Critical Section class and updated Lockable Class to remove unused attribute.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.8   14 Feb 2011 14:42:36   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  707
 * SCR Title:  Change Inspection Display Functionality to Force Display of Last Inspected
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated object manager to allow for high speed multi-threaded access of the object manager.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.7   07 Dec 2010 15:56:24   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  707
 * SCR Title:  Change Inspection Display Functionality to Force Display of Last Inspected
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated source code to include changes in notification functionality using the Observer Design Pattern.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.6   01 Jun 2010 08:29:50   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  693
 * SCR Title:  Fix Performance Issue with Inspection Process
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated source code to remove duplicate definitions and redundunt containers.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.5   30 Jul 2009 08:35:34   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  665
 * SCR Title:  Fix unrecoverable failure when processing acquisitions at high speed
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Merged branced changes into current code branch with appropriate updates.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.4   20 Jun 2007 13:27:54   Joe
 * Project:  SVObserver
 * Change Request (SCR) nbr:  598
 * SCR Title:  Upgrade SVObserver to compile using vc++ in VS2005
 * Checked in by:  jSpila;  Joseph Spila
 * Change Description:  
 *   These changes include modification based on fixing compiler-based and project-based differences between VC6 and VC8.  These changes mainly include casting issues, but some include type conversion and assignment of new compiler controlling defines.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.3   30 Aug 2005 14:07:44   ebeyeler
 * Project:  SVObserver
 * Change Request (SCR) nbr:  492
 * SCR Title:  Upgrade SVObserver to version 4.60
 * Checked in by:  tBair;  Tom Bair
 * Change Description:  
 *   made hash table size prime for more efficient hashing / less collisions
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.2   27 Jul 2005 15:49:56   ebeyeler
 * Project:  SVObserver
 * Change Request (SCR) nbr:  464
 * SCR Title:  Add array indexing for value objects
 * Checked in by:  eBeyeler;  Eric Beyeler
 * Change Description:  
 *   added support for SVObjectReference (get by name)
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.1   07 Mar 2005 11:44:28   Joe
 * Project:  SVObserver
 * Change Request (SCR) nbr:  262
 * SCR Title:  Improve performance when loading a configuration in SVObserver
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Added pragma to remove waring message and added functionality to check for existance in object manager.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.0   15 Feb 2005 14:58:32   ryoho
 * Project:  SVObserver
 * Change Request (SCR) nbr:  440
 * SCR Title:  Create Internal Tool Object to Managing Tool Extents and Result Extents
 * Checked in by:  rYoho;  Rob Yoho
 * Change Description:  
 *   First version of file for SVObjectLibrary
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
   
      Rev 3.6   12 Jul 2004 12:38:00   ebeyeler
   Project:  SVObserver
   Change Request (SCR) nbr:  428
   SCR Title:  Improve Load Configuration time
   Checked in by:  eBeyeler;  Eric Beyeler
   Change Description:  
     implemented hash table initialization
   
   /////////////////////////////////////////////////////////////////////////////////////
   
      Rev 3.5   22 Apr 2003 11:49:10   rschock
   Project:  SVObserver
   Change Request (SCR) nbr:  346
   SCR Title:  Update SVObserver to Version 4.21 Release
   Checked in by:  Joe;  Joe Spila
   Change Description:  
     Redid the #include defines and standardized the Tracker log headers and removed warning from release mode builds.
   
   /////////////////////////////////////////////////////////////////////////////////////
   
      Rev 3.4   03 Jan 2003 15:29:40   ryoho
   Project:  SVObserver
   Change Request (SCR) nbr:  307
   SCR Title:  Implement the ability to remotely change Tool parameters on the SVIM
   Checked in by:  rYoho;  Rob Yoho
   Change Description:  
     added new methods for getting the correct object
   
   /////////////////////////////////////////////////////////////////////////////////////
   
      Rev 3.3   20 Nov 2002 09:37:10   ryoho
   Project:  SVObserver
   Change Request (SCR) nbr:  226, 272
   SCR Title:  Monochrome SVIM configuration compatibility between ViperQUAD and ViperDUAL
   Checked in by:  rYoho;  Rob Yoho
   Change Description:  
     Removed references to  SVPQDataLinkInfo
   
   /////////////////////////////////////////////////////////////////////////////////////
   
      Rev 3.2   07 Aug 2001 15:14:08   rschock
   Project:  SVObserver
   Change Request (SCR) nbr:  208
   SCR Title:  Improve configuration load time
   Checked in by:  rSchock;  Rosco Schock
   Change Description:  
     Changed TheObjectManager's internal object list from a array to a map to allow for quicker lookups
   
   /////////////////////////////////////////////////////////////////////////////////////
   
      Rev 3.1   19 Apr 2001 14:58:58   Steve
   Project:  SVObserver
   Change Request (SCR) nbr:  196
   SCR Title:  Restructure Scripted Load/Save Procedures Functional Requirement
   Checked in by:  Steve;  Stephen E. Steffan
   Change Description:  
     Changes due to addition of the SVFileNameObjectClass and new GUID for the class.
   
   /////////////////////////////////////////////////////////////////////////////////////
   
      Rev 3.0   23 Nov 1999 13:17:58   mike
   Project:  SVObserver
   Change Request (SCR) nbr:  61
   SCR Title:  Update PVCS versioning to version 3.0.
   Checked in by:  Mike;  Mike McCarl
   Change Description:  
     
   
   /////////////////////////////////////////////////////////////////////////////////////
   
      Rev 1.5   Oct 18 1999 11:55:48   Nick
   Project:  SVObserver
   Change Request (SCR) nbr:  35
   SCR Title:  PPQ mode to wait for input data
   Checked in by:  Nick;  F Roland "Nick" Bjorklund
   Change Description:  
     RO - Bugfix: ProducerID will be noew updated right.
   
   /////////////////////////////////////////////////////////////////////////////////////
   
      Rev 1.4   Oct 13 1999 15:14:26   Nick
   Project:  SVObserver
   Change Request (SCR) nbr:  21
   SCR Title:  Port Gage Tool code from program version 2.42.
   Checked in by:  Nick;  F Roland "Nick" Bjorklund
   Change Description:  
     Roberts bug fixes for Beta 10.
   
   /////////////////////////////////////////////////////////////////////////////////////
   
      Rev 1.3   Sep 20 1999 15:40:48   Nick
   Project:  SVObserver
   Change Request (SCR) nbr:  35
   SCR Title:  PPQ mode to wait for input data
   Checked in by:  Nick;  F Roland "Nick" Bjorklund
   Change Description:  
     Introduced new Broadcast Message Router.
   
   /////////////////////////////////////////////////////////////////////////////////////
*/
