//******************************************************************************
//* COPYRIGHT (c) 2003 by SVResearch, Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVObjectManager
//* .File Name       : $Workfile:   SVObjectManagerClass.cpp  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.6  $
//* .Check In Date   : $Date:   13 Nov 2014 10:01:18  $
//******************************************************************************

#pragma region Includes
#include "stdafx.h"
#include "SVObjectManagerClass.h"
#include "ObjectInterfaces/IObjectManager.h"
#include "ObjectInterfaces/IObjectClass.h"
#include "ObjectInterfaces/IValueObject.h"
#include "ObjectInterfaces/ISVImage.h"
#include "ObjectInterfaces/ErrorNumbers.h"
#include "SVSystemLibrary/SVAutoLockAndReleaseTemplate.h"
#include "SVImageLibrary\SVImageInfoClass.h"
#include "SVClassRegisterListClass.h"
#include "SVObjectLibrary.h"
#include "SVObjectSubmitCommandFacade.h"
#include "TextDefinesSvOl.h"
#include "SVMessage\SVMessage.h"
#include "SVStatusLibrary/MessageManager.h"
#pragma endregion Includes

#pragma region Declarations
using namespace Seidenader::SVObjectLibrary;

#ifdef _DEBUG
#define new DEBUG_NEW
#endif
#pragma endregion Declarations

SVObjectManagerClass& SVObjectManagerClass::Instance()
{
	static SVObjectManagerClass l_Object;

	return l_Object;
}

SVObjectManagerClass::SVObjectManagerClass()
: m_State( ReadWrite )
, m_Lock()
, m_ShortPPQIndicator( 0 )
, m_ProductIndicator( 0 )
, m_PendingImageIndicator( 0 )
, m_InspectionIndicator( 0 )
, m_LastFrameRate( 10 )
, m_ObserverCookie( 1 )
, m_FileSequenceNumber( 0 )
{
	m_TranslationMap[FqnInspections] = FqnConfiguration;
	m_TranslationMap[FqnPPQs] = FqnConfiguration;
	m_TranslationMap[FqnCameras] = FqnConfiguration;
	SVString ReplaceName;
	ReplaceName = FqnConfiguration;
	ReplaceName += _T(".");
	ReplaceName += FqnRemoteInputs;
	m_TranslationMap[FqnRemoteInputs] = ReplaceName;
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

const SVGUID SVObjectManagerClass::GetChildRootObjectID(const SVString& rRootName) const
{
	SVGUID ObjectID;

	if( m_RootNameChildren.end() != m_RootNameChildren.find(rRootName)  )
	{
		ObjectID = m_RootNameChildren.at(rRootName);
	}
	//If the root node is not found then return the configuration for backward compatibility
	else if( m_RootNameChildren.end() != m_RootNameChildren.find( FqnConfiguration ) )
	{
		ObjectID = m_RootNameChildren.at( FqnConfiguration );
	}
	return ObjectID;
}

HRESULT SVObjectManagerClass::ConstructRootObject( const SVGUID& rClassID )
{
	HRESULT Status = S_OK;

	if( m_RootNameChildren.end() != m_RootNameChildren.find( FqnRoot ) && !( m_RootNameChildren[FqnRoot].empty() ) )
	{
		DestroyRootObject();
	}
	
	SVGUID ObjectID;

	Status = ConstructObject( rClassID, ObjectID);
	if(S_OK == Status)
	{
		SVObjectClass* pRootObject;
		GetObjectByIdentifier(ObjectID, pRootObject);
		if(NULL != pRootObject)
		{
			m_RootNameChildren[FqnRoot] = ObjectID;
		}
	}
	
	return Status;
}

HRESULT SVObjectManagerClass::DestroyRootObject()
{
	HRESULT l_Status = S_OK;

	SVGUID RootID = GetChildRootObjectID(FqnRoot);
	if( !( RootID.empty() ) )
	{
		SVObjectClass* l_pObject = GetObject( RootID );

		if( l_pObject != NULL )
		{
			delete l_pObject;
		}
	}

	return l_Status;
}

void SVObjectManagerClass::setRootChildID(const SVString& rRootChild, const SVGUID& rUniqueID)
{
	SVObjectClass* pRootObject(nullptr);
	GetObjectByIdentifier( rUniqueID, pRootObject);
	if( nullptr != pRootObject)
	{
		m_RootNameChildren[rRootChild] = rUniqueID;
	}
}

void SVObjectManagerClass::Translation(SVString& Name)
{
	bool NameChanged = false;

	SVObjectNameInfo NameInfo;
	SVObjectNameInfo::ParseObjectName( NameInfo, Name );
	SVObjectNameInfo::SVNameDeque::iterator NameIter;
	for(NameIter = NameInfo.m_NameArray.begin(); NameIter != NameInfo.m_NameArray.end(); ++NameIter)
	{
		TranslateMap::const_iterator TranslationIter;
		TranslationIter =  m_TranslationMap.find( *NameIter );
		if(TranslationIter != m_TranslationMap.end())
		{
			NameChanged = true;
			*NameIter = TranslationIter->second;
		}
	}
	if(NameChanged)
	{
		Name = NameInfo.GetObjectArrayName(0);
	}
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

HRESULT SVObjectManagerClass::ConstructObject( const SVGUID& rClassID, GUID& rObjectID )
{
	HRESULT l_Status = S_OK;

	SVObjectClass* l_pObject = NULL;

	l_Status = ConstructObject( rClassID, l_pObject );

	if( l_pObject != NULL )
	{
		rObjectID = l_pObject->GetUniqueObjectID();
	}
	else
	{
		rObjectID = SVInvalidGUID;

		if( l_Status == S_OK )
		{
			l_Status = E_FAIL;
		}
	}

	return l_Status;
}

HRESULT SVObjectManagerClass::ConstructObject( const SVGUID& rClassID, SVObjectClass*& rpObject )
{
	HRESULT l_Status = S_OK;

	rpObject = SVClassRegisterListClass::Instance().ConstructNewObject( rClassID );

	if( rpObject == NULL )
	{
		l_Status = E_FAIL;
	}

	return l_Status;
}

void SVObjectManagerClass::DestructObject( const SVGUID& rClassID )
{
	if( !( rClassID.empty() ) )
	{
		SVObjectClass* l_pObject = GetObject( rClassID );

		if( l_pObject != NULL )
		{
			delete l_pObject;
		}
	}
}

HRESULT SVObjectManagerClass::GetObjectByIdentifier( const SVGUID& rObjectID, SVObjectClass*& rpObject ) const
{
	HRESULT l_Result = S_OK;

	SVAutoLockAndReleaseTemplate< SVCriticalSection > l_AutoLock;
	BOOL l_Status = !( rObjectID.empty() );

	if( l_Status && m_State == ReadWrite )
	{
		l_Status = l_AutoLock.Assign( &m_Lock );
	}

	if( l_Status )
	{
		SVUniqueObjectEntryStructPtr pUniqueObject = getUniqueObjectEntry( rObjectID );

		if( !( pUniqueObject.empty() ) )
		{
			rpObject = pUniqueObject->PObject;
		}
		else
		{
			rpObject = NULL;
		}
	}
	else
	{
		rpObject = NULL;
	}

	if( l_Result == S_OK && rpObject == NULL )
	{
		l_Result = E_FAIL;
	}

	return l_Result;
}

HRESULT SVObjectManagerClass::GetObjectByDottedName( const SVString& rFullName, GUID& rObjectID ) const
{
	HRESULT Status = S_OK;
	SVObjectReference Reference;

	Status = GetObjectByDottedName( rFullName, Reference );

	if( Reference.Object() != NULL )
	{
		rObjectID = Reference.Object()->GetUniqueObjectID();
	}
	else
	{
		if( Status == S_OK )
		{
			Status = E_FAIL;
		}

		rObjectID = SV_GUID_NULL;
	}

	return Status;
}

HRESULT SVObjectManagerClass::GetObjectByDottedName( const SVString& rFullName, SVObjectClass*& rpObject ) const
{
	HRESULT Status = S_OK;

	SVObjectReference Reference;

	Status = GetObjectByDottedName( rFullName, Reference );

	if( Reference.Object() != NULL )
	{
		rpObject = Reference.Object();
	}
	else
	{
		rpObject = NULL;
	}

	if( Status == S_OK && rpObject == NULL )
	{
		Status = E_FAIL;
	}

	return Status;
}

HRESULT SVObjectManagerClass::GetObjectByDottedName( const SVString& rFullName, SVObjectReference& rReference ) const
{
	HRESULT Result = S_OK;

	if( 0 == rFullName.find( SvOl::ToolSetName ) )
	{
		SvStl::MessageMgrNoDisplay Exception( SvStl::LogOnly );
		Exception.setMessage( SVMSG_SVO_96_DOTTED_NAME_NOT_UNIQUE, rFullName.c_str(), StdMessageParams, SvOi::Err_25049_DottedName );
		ASSERT(false);
		Result = E_FAIL;
		rReference = SVObjectReference();
		return Result;
	}

	SVAutoLockAndReleaseTemplate< SVCriticalSection > l_AutoLock;
	BOOL l_Status = !( rFullName.empty() );

	if( l_Status && m_State == ReadWrite )
	{
		l_Status = l_AutoLock.Assign( &m_Lock );
	}

	if( l_Status )
	{
		SVString Name = rFullName;
		Instance().Translation(Name);
		SVObjectClass* pChildRootObject( nullptr );
		SVObjectNameInfo NameInfo;

		NameInfo.ParseObjectName( Name );

		SVGUID ChildRootID = GetChildRootObjectID( NameInfo.m_NameArray[ 0 ] );
		SVGUID ConfigID = GetChildRootObjectID( FqnConfiguration );
		Result = GetObjectByIdentifier( ChildRootID, pChildRootObject );

		if( nullptr != pChildRootObject )
		{
			//Check if it is configuration object as it needs to be stripped of the FqnConfiguration name
			if( ChildRootID == ConfigID )
			{
				if(0 == Name.find( pChildRootObject->GetName() ) )
				{
					NameInfo.RemoveTopName();
				}
			}
			SVObjectClass* pObject( nullptr );
			//Only child root object
			if( 1 == NameInfo.m_NameArray.size() && m_RootNameChildren.end() != m_RootNameChildren.find( NameInfo.m_NameArray[0] ) )
			{
				pObject = pChildRootObject;
			}
			else
			{
				SVObjectClass* pParent( nullptr );
				if( ChildRootID == ConfigID )
				{
					pParent = pChildRootObject;
				}
				else
				{
					SVGUID RootObjetctID = GetChildRootObjectID( FqnRoot );
					Result = GetObjectByIdentifier( RootObjetctID, pParent );
				}
				if( nullptr != pParent )
				{
					Result = pParent->GetChildObject( pObject, NameInfo );
				}
			}

			if( nullptr != pObject )
			{
				rReference = SVObjectReference( pObject, NameInfo );
			}
			else
			{
				rReference = SVObjectReference();
			}
		}
		else
		{
			rReference = SVObjectReference();
		}
	}
	else
	{
		rReference = SVObjectReference();
	}

	if( Result == S_OK && rReference.Object() == NULL )
	{
		Result = E_FAIL;
	}

	return Result;
}

BOOL SVObjectManagerClass::CreateUniqueObjectID( SVObjectClass* PObject )
{
	BOOL l_Status = ( m_State == ReadWrite );

	if( l_Status )
	{
		SVAutoLockAndReleaseTemplate< SVCriticalSection > l_AutoLock;

		l_Status = l_Status && l_AutoLock.Assign( &m_Lock );
		l_Status = l_Status && nullptr != PObject;

		if( l_Status )
		{
			SVUniqueObjectEntryStructPtr pUniqueObject = new SVUniqueObjectEntryStruct;

			l_Status = !( pUniqueObject.empty() );

			if( l_Status )
			{
				l_Status = ( CoCreateGuid( &( pUniqueObject->ObjectUID.ToGUID() ) ) == S_OK );

				if( l_Status )
				{
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
		l_Status = l_Status && PObject != nullptr;

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
		l_Status = l_Status && nullptr != PObject;

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

SVObjectClass* SVObjectManagerClass::GetObject( const SVGUID& RGuid ) const
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
		SVObjectClass* l_pConfig;
		GetRootChildObject( l_pConfig, FqnConfiguration );

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

SVGUID SVObjectManagerClass::GetObjectIdFromCompleteName( const SVString& rName )
{
	SVGUID l_ObjectID;

	SVObjectClass* l_pObject = GetObjectCompleteName( rName.c_str() );

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

HRESULT SVObjectManagerClass::SubmitCommand( const SVGUID& rObjectID, const SVCommandTemplatePtr& rCommandPtr )
{
	HRESULT hRet = E_FAIL;
	SVObjectClass* l_pObject = GetObject( rObjectID );

	if( nullptr != l_pObject )
	{
		hRet = SubmitCommand( *l_pObject, rCommandPtr );
	}

	return hRet;
}

HRESULT SVObjectManagerClass::SubmitCommand( SVObjectClass& rObject, const SVCommandTemplatePtr& rCommandPtr )
{
	HRESULT hRet = E_FAIL;
	SVObjectSubmitCommandFacade* l_pCommandTarget = dynamic_cast< SVObjectSubmitCommandFacade* >( &rObject );

	if( nullptr != l_pCommandTarget )
	{
		hRet = l_pCommandTarget->SubmitCommand( rCommandPtr );
	}

	return hRet;
}

HRESULT SVObjectManagerClass::InsertObserver( SVObserverNotificationFunctorPtr p_FunctorPtr, long& rCookie )
{
	HRESULT l_Status = S_OK;

	SVAutoLockAndReleaseTemplate< SVCriticalSection > l_AutoLock;

	if( m_State == ReadWrite && l_AutoLock.Assign( &m_Lock ) )
	{
		rCookie = ::InterlockedExchangeAdd( &m_ObserverCookie, 1 );

		m_CookieEntries[ rCookie ] = new SVCookieEntryStruct( rCookie, p_FunctorPtr );
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

HRESULT SVObjectManagerClass::GetObserverSubject( const SVString& rSubjectDataName, const SVGUID& rObserverID, GUID& rSubjectID ) const
{
	HRESULT l_Status = S_OK;

	rSubjectID = GetSubjectID( rSubjectDataName, getUniqueObjectEntry( rObserverID ) );

	if( SVGUID( rSubjectID ).empty() )
	{
		l_Status = S_FALSE;
	}

	return l_Status;
}

HRESULT SVObjectManagerClass::GetObserverSubject( const SVString& rSubjectDataName, long p_Cookie, GUID& rSubjectID ) const
{
	HRESULT l_Status = S_OK;

	rSubjectID = GetSubjectID( rSubjectDataName, GetCookieEntry( p_Cookie ) );

	if( SVGUID( rSubjectID ).empty() )
	{
		l_Status = S_FALSE;
	}

	return l_Status;
}

HRESULT SVObjectManagerClass::GetObserverIds( const SVString& rSubjectDataName, const SVGUID& rSubjectID, GuidSet& rObserverIds )
{
	rObserverIds.clear();

	SVSubjectEnabledObserverMap l_Observers;

	HRESULT l_Status = GetObservers( rSubjectDataName, rSubjectID, l_Observers );

	if( l_Status == S_OK )
	{
		SVSubjectEnabledObserverMap::iterator l_Iter;

		for( l_Iter = l_Observers.begin(); l_Iter != l_Observers.end(); ++l_Iter )
		{
			if( l_Iter->second == 1 )
			{
				rObserverIds.insert( l_Iter->first );
			}
		}
	}

	return l_Status;
}

HRESULT SVObjectManagerClass::AttachObserver( const SVString& rSubjectDataName, const SVGUID& rSubjectID, const SVGUID& rObserverID )
{
	HRESULT l_Status = S_OK;

	SVAutoLockAndReleaseTemplate< SVCriticalSection > l_AutoLock;

	if( m_State == ReadWrite && l_AutoLock.Assign( &m_Lock ) )
	{
		SVUniqueObjectEntryStructPtr pSubjectObject = getUniqueObjectEntry( rSubjectID );
		SVUniqueObjectEntryStructPtr pObserverObject = getUniqueObjectEntry( rObserverID );
		
		if( !( pSubjectObject.empty() ) && !( pObserverObject.empty() ) )
		{
			SVGUID l_SubjectID = GetSubjectID( rSubjectDataName, pObserverObject );

			if( !( l_SubjectID.empty() ) )
			{
				DetachObserver( rSubjectDataName, l_SubjectID, rObserverID );
			}

			pSubjectObject->m_DataNameSubjectObservers[ rSubjectDataName ].m_SubjectObservers[ rObserverID ] = 1;
			pObserverObject->m_SubjectIDs[ rSubjectDataName ] = rSubjectID;
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

HRESULT SVObjectManagerClass::AttachObserver( const SVString& rSubjectDataName, const SVGUID& rSubjectID, long p_Cookie )
{
	HRESULT l_Status = S_OK;

	SVAutoLockAndReleaseTemplate< SVCriticalSection > l_AutoLock;

	if( m_State == ReadWrite && l_AutoLock.Assign( &m_Lock ) )
	{
		SVUniqueObjectEntryStructPtr pSubjectObject = getUniqueObjectEntry( rSubjectID );
		SVCookieEntryStructPtr pObserverObject = GetCookieEntry( p_Cookie );
		
		if( !( pSubjectObject.empty() ) && !( pObserverObject.empty() ) )
		{
			SVGUID l_SubjectID = GetSubjectID( rSubjectDataName, pObserverObject );

			if (!l_SubjectID.empty() && l_SubjectID == rSubjectID)
			{
				DetachObserver( rSubjectDataName, l_SubjectID, p_Cookie );
			}

			pSubjectObject->m_DataNameSubjectObservers[ rSubjectDataName ].m_SubjectCookies[ p_Cookie ] = 1;
			pObserverObject->m_SubjectIDs[ rSubjectDataName ] = rSubjectID;
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

HRESULT SVObjectManagerClass::EnableObserver( const SVString& rSubjectDataName, const SVGUID& rSubjectID, const SVGUID& rObserverID )
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
		SVUniqueObjectEntryStructPtr pSubjectObject = getUniqueObjectEntry( rSubjectID );
		
		if( !( pSubjectObject.empty() ) )
		{
			pSubjectObject->m_DataNameSubjectObservers[ rSubjectDataName ].m_SubjectObservers[ rObserverID ] = 1;
		}
		else
		{
			l_Status = E_FAIL;
		}
	}

	return l_Status;
}

HRESULT SVObjectManagerClass::EnableObserver( const SVString& rSubjectDataName, const SVGUID& rSubjectID, long p_Cookie )
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
		SVUniqueObjectEntryStructPtr pSubjectObject = getUniqueObjectEntry( rSubjectID );
		
		if( !( pSubjectObject.empty() ) )
		{
			pSubjectObject->m_DataNameSubjectObservers[ rSubjectDataName ].m_SubjectCookies[ p_Cookie ] = 1;
		}
		else
		{
			l_Status = E_FAIL;
		}
	}

	return l_Status;
}

HRESULT SVObjectManagerClass::DisableObserver( const SVString& rSubjectDataName, const SVGUID& rSubjectID, const SVGUID& rObserverID )
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
		SVUniqueObjectEntryStructPtr pSubjectObject = getUniqueObjectEntry( rSubjectID );
		
		if( !( pSubjectObject.empty() ) )
		{
			pSubjectObject->m_DataNameSubjectObservers[ rSubjectDataName ].m_SubjectObservers[ rObserverID ] = 0;
		}
		else
		{
			l_Status = E_FAIL;
		}
	}

	return l_Status;
}

HRESULT SVObjectManagerClass::DisableObserver( const SVString& rSubjectDataName, const SVGUID& rSubjectID, long p_Cookie )
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
		SVUniqueObjectEntryStructPtr pSubjectObject = getUniqueObjectEntry( rSubjectID );
		
		if( !( pSubjectObject.empty() ) )
		{
			pSubjectObject->m_DataNameSubjectObservers[ rSubjectDataName ].m_SubjectCookies[ p_Cookie ] = 0;
		}
		else
		{
			l_Status = E_FAIL;
		}
	}

	return l_Status;
}

HRESULT SVObjectManagerClass::DetachObserver( const SVString& rSubjectDataName, const SVGUID& rSubjectID, const SVGUID& rObserverID )
{
	HRESULT l_Status = S_OK;

	SVAutoLockAndReleaseTemplate< SVCriticalSection > l_AutoLock;

	if( m_State == ReadWrite && l_AutoLock.Assign( &m_Lock ) )
	{
		SVUniqueObjectEntryStructPtr pObserverObject = getUniqueObjectEntry( rObserverID );
		
		if( !( pObserverObject.empty() ) )
		{
			pObserverObject->m_SubjectIDs.erase( rSubjectDataName );
		}

		SVUniqueObjectEntryStructPtr pSubjectObject = getUniqueObjectEntry( rSubjectID );
		
		if( !( pSubjectObject.empty() ) )
		{
			pSubjectObject->m_DataNameSubjectObservers[ rSubjectDataName ].m_SubjectObservers.erase( rObserverID );
		}
	}
	else
	{
		l_Status = E_FAIL;
	}

	return l_Status;
}

HRESULT SVObjectManagerClass::DetachObserver( const SVString& rSubjectDataName, const SVGUID& rSubjectID, long p_Cookie )
{
	HRESULT l_Status = S_OK;

	SVAutoLockAndReleaseTemplate< SVCriticalSection > l_AutoLock;

	if( m_State == ReadWrite && l_AutoLock.Assign( &m_Lock ) )
	{
		SVCookieEntryStructPtr pObserverObject = GetCookieEntry( p_Cookie );
		
		if( !( pObserverObject.empty() ) )
		{
			pObserverObject->m_SubjectIDs.erase( rSubjectDataName );
		}

		SVUniqueObjectEntryStructPtr pSubjectObject = getUniqueObjectEntry( rSubjectID );
		
		if( !( pSubjectObject.empty() ) )
		{
			pSubjectObject->m_DataNameSubjectObservers[ rSubjectDataName ].m_SubjectCookies.erase( p_Cookie );
		}
	}
	else
	{
		l_Status = E_FAIL;
	}

	return l_Status;
}

HRESULT SVObjectManagerClass::DetachObservers( const SVString& rSubjectDataName, const SVGUID& rSubjectID )
{
	HRESULT l_Status = S_OK;

	SVSubjectEnabledObserverMap l_Observers;
	SVSubjectEnabledCookieMap l_CookieObservers;

	l_Status = GetObservers( rSubjectDataName, rSubjectID, l_Observers, l_CookieObservers );

	if( l_Status == S_OK )
	{
		SVSubjectEnabledObserverMap::iterator l_Iter = l_Observers.begin();

		while( l_Iter != l_Observers.end() )
		{
			HRESULT l_Temp = DetachObserver( rSubjectDataName, rSubjectID, l_Iter->first ) ;

			if( l_Status == S_OK )
			{
				l_Status = l_Temp;
			}

			++l_Iter;
		}

		SVSubjectEnabledCookieMap::iterator l_CookieIter = l_CookieObservers.begin();

		while( l_CookieIter != l_CookieObservers.end() )
		{
			HRESULT l_Temp = DetachObserver( rSubjectDataName, rSubjectID, l_CookieIter->first ) ;

			if( l_Status == S_OK )
			{
				l_Status = l_Temp;
			}

			++l_CookieIter;
		}
	}

	return l_Status;
}

HRESULT SVObjectManagerClass::DetachSubjectsAndObservers( const SVGUID& rObjectID )
{
	HRESULT l_Status = DetachObservers( rObjectID );
	HRESULT l_SubjectStatus = DetachSubjects( rObjectID );

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

HRESULT SVObjectManagerClass::DetachSubjects( const SVGUID& rObserverID )
{
	HRESULT l_Status = S_OK;

	SVSubjectDataNameDeque l_SubjectDataNames;

	l_Status = GetObserverDataNames( rObserverID, l_SubjectDataNames );

	if( l_Status == S_OK )
	{
		SVSubjectDataNameDeque::iterator l_Iter;
		
		for( l_Iter = l_SubjectDataNames.begin(); l_Iter != l_SubjectDataNames.end(); ++l_Iter )
		{
			SVGUID l_SubjectId;

			GetObserverSubject( *l_Iter, rObserverID, l_SubjectId );

			DetachObserver( *l_Iter, l_SubjectId, rObserverID );
		}
	}

	return l_Status;
}

HRESULT SVObjectManagerClass::DetachObservers( const SVGUID& rSubjectID )
{
	HRESULT l_Status = S_OK;

	SVSubjectDataNameDeque l_SubjectDataNames;

	l_Status = GetSubjectDataNames( rSubjectID, l_SubjectDataNames );

	if( l_Status == S_OK )
	{
		SVSubjectDataNameDeque::iterator l_Iter;
		
		for( l_Iter = l_SubjectDataNames.begin(); l_Iter != l_SubjectDataNames.end(); ++l_Iter )
		{
			HRESULT l_Temp = DetachObservers( *l_Iter, rSubjectID ) ;

			if( l_Status == S_OK )
			{
				l_Status = l_Temp;
			}
		}
	}

	return l_Status;
}

HRESULT SVObjectManagerClass::DisconnectObjects( const SVGUID& rObjectID, const SVGUID& rRemoteID )
{
	HRESULT l_Status = S_OK;

	SVObjectClass* l_pObject = GetObject( rObjectID );

	if( l_pObject != NULL )
	{
		l_Status = l_pObject->RemoveObjectConnection( rRemoteID );
	}

	l_pObject = GetObject( rRemoteID );

	if( l_pObject != NULL )
	{
		HRESULT l_Temp = l_pObject->RemoveObjectConnection( rObjectID );

		if( l_Status == S_OK )
		{
			l_Status = l_Temp;
		}
	}

	return l_Status;
}

SVIPDoc* SVObjectManagerClass::GetIPDoc( const SVGUID& rIPGuid ) const
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
		SVIPDocMap::const_iterator l_Iter( m_IPDocs.find( rIPGuid ) );

		if( l_Iter != m_IPDocs.end() )
		{
			l_pIPDoc = l_Iter->second;
		}
	}

	return l_pIPDoc;
}

HRESULT SVObjectManagerClass::RegisterIPDoc( const SVGUID& rIPGuid, SVIPDoc* p_pIPDoc )
{
	HRESULT l_Status( S_OK );

	SVAutoLockAndReleaseTemplate< SVCriticalSection > l_AutoLock;

	if( m_State == ReadWrite && l_AutoLock.Assign( &m_Lock ) )
	{
		SVIPDocMap::iterator l_Iter( m_IPDocs.find( rIPGuid ) );

		if( l_Iter == m_IPDocs.end() )
		{
			m_IPDocs[ rIPGuid ] = p_pIPDoc;
		}
	}
	else
	{
		l_Status = E_FAIL;
	}

	return l_Status;
}

HRESULT SVObjectManagerClass::UnregisterIPDoc( const SVGUID& rIPGuid )
{
	HRESULT l_Status( S_OK );

	SVAutoLockAndReleaseTemplate< SVCriticalSection > l_AutoLock;

	if( m_State == ReadWrite && l_AutoLock.Assign( &m_Lock ) )
	{
		m_IPDocs.erase( rIPGuid );
	}
	else
	{
		l_Status = E_FAIL;
	}

	return l_Status;
}

SVIODoc* SVObjectManagerClass::GetIODoc( const SVGUID& rIOGuid ) const
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
		SVIODocMap::const_iterator l_Iter( m_IODocs.find( rIOGuid ) );

		if( l_Iter != m_IODocs.end() )
		{
			l_pIODoc = l_Iter->second;
		}
	}

	return l_pIODoc;
}

HRESULT SVObjectManagerClass::RegisterIODoc( const SVGUID& rIOGuid, SVIODoc* p_pIODoc )
{
	HRESULT l_Status( S_OK );

	SVAutoLockAndReleaseTemplate< SVCriticalSection > l_AutoLock;

	if( m_State == ReadWrite && l_AutoLock.Assign( &m_Lock ) )
	{
		SVIODocMap::iterator l_Iter( m_IODocs.find( rIOGuid ) );

		if( l_Iter == m_IODocs.end() )
		{
			m_IODocs[ rIOGuid ] = p_pIODoc;
		}
	}
	else
	{
		l_Status = E_FAIL;
	}

	return l_Status;
}

HRESULT SVObjectManagerClass::UnregisterIODoc( const SVGUID& rIOGuid )
{
	HRESULT l_Status( S_OK );

	SVAutoLockAndReleaseTemplate< SVCriticalSection > l_AutoLock;

	if( m_State == ReadWrite && l_AutoLock.Assign( &m_Lock ) )
	{
		m_IODocs.erase( rIOGuid );
	}
	else
	{
		l_Status = E_FAIL;
	}

	return l_Status;
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

HRESULT SVObjectManagerClass::getTreeList(const SVString& rPath, SVObjectReferenceVector& rObjectList, UINT AttributesAllowedFilter) const
{
	HRESULT Result = S_OK;
	GuidSet GuidObjectList;

	SVObjectClass* pStartObject = nullptr;
	GetObjectByDottedName(rPath, pStartObject);

	if( nullptr != pStartObject )
	{
		SVString InternalPath = pStartObject->GetCompleteObjectName();
		if( (pStartObject->ObjectAttributesAllowed() & AttributesAllowedFilter) == AttributesAllowedFilter )
		{
			GuidObjectList.insert(pStartObject->GetUniqueObjectID());
		}

		SVUniqueObjectEntryStructPtr pUniqueObjectEntry;

		SVAutoLockAndReleaseTemplate< SVCriticalSection > l_AutoLock;

		BOOL l_Status = true;

		if( m_State == ReadWrite )
		{
			l_Status = l_AutoLock.Assign( &m_Lock );
		}

		if( l_Status )
		{
			SVUniqueObjectEntryMap::const_iterator Iter( m_UniqueObjectEntries.begin() );

			while( Iter != m_UniqueObjectEntries.end() )
			{
				pUniqueObjectEntry = Iter->second ;

				if( !pUniqueObjectEntry.empty() &&  NULL != pUniqueObjectEntry->PObject )
				{
					if( (pUniqueObjectEntry->PObject->ObjectAttributesAllowed() & AttributesAllowedFilter) == AttributesAllowedFilter )
					{
						//Check if owner is in list
						SVString ObjectPath = pUniqueObjectEntry->PObject->GetCompleteObjectName();
						SVString::size_type Pos = ObjectPath.find( InternalPath.c_str() );
						if( SVString::npos != Pos )
						{
							if( ObjectPath.size() == Pos + InternalPath.size() || ObjectPath[Pos+InternalPath.size()] == '.')
							{
								GuidObjectList.insert(pUniqueObjectEntry->PObject->GetUniqueObjectID());
							}
						}
					}
				}
				++Iter;
			}
		}
	}

	GuidSet::iterator Iter(GuidObjectList.begin());
	while( Iter != GuidObjectList.end() )
	{
		SVObjectReference ObjectRef( GetObject(*Iter) );
		rObjectList.push_back( ObjectRef );
		++Iter;
	}

	return Result;
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

SVGUID SVObjectManagerClass::GetSubjectID( const SVString& rSubjectDataName, SVUniqueObjectEntryStructPtr p_ObjectEntryPtr ) const
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
			SVSubjectDataNameSubjectIDMap::const_iterator l_Iter = p_ObjectEntryPtr->m_SubjectIDs.find( rSubjectDataName );

			if( l_Iter != p_ObjectEntryPtr->m_SubjectIDs.end() )
			{
				l_SubjectID = l_Iter->second;
			}
		}
	}

	return l_SubjectID;
}

SVGUID SVObjectManagerClass::GetSubjectID( const SVString& rSubjectDataName, SVCookieEntryStructPtr p_CookieEntryPtr ) const
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
			SVSubjectDataNameSubjectIDMap::const_iterator l_Iter = p_CookieEntryPtr->m_SubjectIDs.find( rSubjectDataName );

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

HRESULT SVObjectManagerClass::GetSubjectDataNames( const SVGUID& rSubjectID, SVSubjectDataNameDeque& rSubjectDataNames ) const
{
	HRESULT l_Status = S_OK;

	rSubjectDataNames.clear();

	SVUniqueObjectEntryStructPtr pUniqueObject = getUniqueObjectEntry( rSubjectID );

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
				rSubjectDataNames.push_back( l_Iter->first );
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

HRESULT SVObjectManagerClass::GetObserverDataNames( long p_Cookie, SVSubjectDataNameDeque& rSubjectDataNames ) const
{
	HRESULT l_Status = S_OK;

	rSubjectDataNames.clear();

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
				rSubjectDataNames.push_back( l_Iter->first );
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

HRESULT SVObjectManagerClass::GetObserverDataNames( const SVGUID& rObserverID, SVSubjectDataNameDeque& rSubjectDataNames ) const
{
	HRESULT l_Status = S_OK;

	rSubjectDataNames.clear();

	SVUniqueObjectEntryStructPtr pUniqueObject = getUniqueObjectEntry( rObserverID );

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
				rSubjectDataNames.push_back( l_Iter->first );
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

HRESULT SVObjectManagerClass::GetObservers( const SVString& rSubjectDataName, const SVGUID& rSubjectID, SVSubjectEnabledObserverMap& rObservers )
{
	HRESULT l_Status = S_OK;

	rObservers.clear();

	SVUniqueObjectEntryStructPtr pUniqueObject = getUniqueObjectEntry( rSubjectID );

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
			SVSubjectDataNameObserverMap::iterator l_Iter = pUniqueObject->m_DataNameSubjectObservers.find( rSubjectDataName );

			if( l_Iter != pUniqueObject->m_DataNameSubjectObservers.end() )
			{
				rObservers = l_Iter->second.m_SubjectObservers;
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

HRESULT SVObjectManagerClass::GetObservers( const SVString& rSubjectDataName, const SVGUID& rSubjectID, SVSubjectEnabledObserverMap& rObservers, SVSubjectEnabledCookieMap& rObserverCookies )
{
	HRESULT l_Status = S_OK;

	rObservers.clear();

	SVUniqueObjectEntryStructPtr pUniqueObject = getUniqueObjectEntry( rSubjectID );

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
			SVSubjectDataNameObserverMap::iterator l_Iter = pUniqueObject->m_DataNameSubjectObservers.find( rSubjectDataName );

			if( l_Iter != pUniqueObject->m_DataNameSubjectObservers.end() )
			{
				rObservers = l_Iter->second.m_SubjectObservers;
				rObserverCookies = l_Iter->second.m_SubjectCookies;
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

#pragma region IObjectManager-function
SvOi::IObjectClass* SvOi::getObjectByDottedName( const SVString& rFullName )
{
	SVObjectClass* pObject = nullptr;
	//To have the function available without to know the class EvironmentObject
	SVObjectManagerClass::Instance().GetObjectByDottedName(rFullName, pObject);
	return pObject;
}

SvOi::IObjectClass* SvOi::ConstructObject( const SVGUID& rClassID )
{
	SVObjectClass* pObject = nullptr;
	SVObjectManagerClass::Instance().ConstructObject(rClassID, pObject);
	return pObject;
}

SvOi::IObjectClass* SvOi::getObject( const SVGUID& rObjectID )
{
	SVObjectClass* pObject = nullptr;
	SVObjectManagerClass::Instance().GetObjectByIdentifier(rObjectID, pObject);
	return pObject;
}

SvOi::IObjectClass* SvOi::FindObject(const SVGUID& rParentID, const SVObjectTypeInfoStruct& rInfo)
{
	SVObjectClass* pObject = reinterpret_cast<SVObjectClass *>(::SVSendMessage(rParentID, SVM_GETFIRST_OBJECT, 0, reinterpret_cast<DWORD_PTR>(&rInfo)));
	return dynamic_cast<SvOi::IObjectClass *>(pObject);
}

SvOi::IValueObject* SvOi::FindValueObject(const SVGUID& rParentID, const SVObjectTypeInfoStruct& rInfo)
{
	SVObjectClass* pObject = reinterpret_cast<SVObjectClass *>(::SVSendMessage(rParentID, SVM_GETFIRST_OBJECT, NULL, reinterpret_cast<DWORD_PTR>(&rInfo)));
	
	return dynamic_cast<SvOi::IValueObject *>(pObject);
}

SvOi::ISVImage* SvOi::FindImageObject(const SVGUID& rParentID, const SVObjectTypeInfoStruct& rInfo)
{
	SVObjectClass* pObject = reinterpret_cast<SVObjectClass*>(::SVSendMessage(rParentID, SVM_GETFIRST_OBJECT, NULL, reinterpret_cast<DWORD_PTR>(&rInfo)));
	
	return dynamic_cast<SvOi::ISVImage *>(pObject);
}
#pragma endregion IObjectManager-function

