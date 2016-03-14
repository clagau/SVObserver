//******************************************************************************
//* COPYRIGHT (c) 2003 by SVResearch, Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVPublishList.cpp
//* .File Name       : $Workfile:   SVPublishList.cpp  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.7  $
//* .Check In Date   : $Date:   15 May 2014 12:31:44  $
//******************************************************************************

#pragma region Includes
#include "stdafx.h"
#include "SVPublishList.h"
#include "SVObjectLibrary\SVObjectManagerClass.h"
#include "SVDigitalOutputObject1.h"
#include "SVTaskObject.h"
#include "SVOutputObjectList.h"
#include "SVInfoStructs.h"
#include "SVObserver.h"
#include "SVInspectionProcess.h"
#include "SVConfigurationObject.h"
#include "SVPPQObject.h"
#include "ObjectInterfaces\ErrorNumbers.h"
#include "SVStatusLibrary\MessageManager.h"
#include "TextDefinesSvO.h"
#pragma endregion Includes

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

SVPublishListClass::SVPublishListClass()
{
	hProtectionMutex   = hProtectionMutex = CreateMutex( NULL, FALSE, NULL );
	dwWaitTime		   = 2000;
}

SVPublishListClass::~SVPublishListClass()
{
	Destroy();
}

void SVPublishListClass::Destroy()
{
	if( hProtectionMutex )
	{
		if( WaitForSingleObject( hProtectionMutex, dwWaitTime ) != WAIT_OBJECT_0 )
			return;

		CloseHandle( hProtectionMutex );
		hProtectionMutex = NULL;
	}

	RemoveAll();
}

void SVPublishListClass::Refresh(SVTaskObjectClass * pRootObject)
{
	SVOutputInfoListClass newList;

	// Find all outputs marked as selected for publishing
	// Note:: both Lists must be from the same root object
	// which at this point is SVToolSetClass
	SVOutputInfoListClass l_OutputList;

	pRootObject->GetOutputList( l_OutputList );

	l_OutputList.GetSetAttributesList( SV_PUBLISHABLE, &newList );

	SVPPQObject* pPPQ( nullptr );
	SVOutputObjectList* pOutputList ( nullptr );
	SVIOEntryHostStructPtrList ppPPQEntries;
	SVIOEntryHostStructPtr pIOEntry;
	BOOL bOk;
	long lPPQSize = 0;
	long lPPQ = 0;

	SVConfigurationObject* pConfig( nullptr );
	SVObjectManagerClass::Instance().GetConfigurationObject( pConfig );

	// check for removed items
	for( long i = GetSize() - 1; i >= 0; i-- )
	{
		SVOutObjectInfoStruct* pPublishedOutObjectInfo = GetAt( i );
		bool found = false;

		// try to find item in new list
		for( int j = 0; j < newList.GetSize(); j++ )
		{
			SVOutObjectInfoStruct* pOutObjectInfo = newList.GetAt( j );
			if( pOutObjectInfo == pPublishedOutObjectInfo )
			{
				found = true;
				break;
			}
		}

		if( !found ) // not found - have to remove it
		{
			bOk = ( SVObjectManagerClass::Instance().GetObject( pPublishedOutObjectInfo->UniqueObjectID ) != NULL );
			if( !bOk )
			{
				RemoveAt( i );
				continue;
			}// end if

			SVInObjectInfoStruct InObjectInfo;

			InObjectInfo.SetObject( m_pInspection );
			InObjectInfo.SetInputObject( pPublishedOutObjectInfo->UniqueObjectID );

			// Disconnect
			::SVSendMessage(pPublishedOutObjectInfo->UniqueObjectID, SVM_DISCONNECT_OBJECT_INPUT, reinterpret_cast<DWORD_PTR>(&InObjectInfo), NULL );

			//if( should not be anymore published )
			SVPublicAttributeEntryStruct* pPublicAttribute = pPublishedOutObjectInfo->PObject->GetPublicAttribute();

			// remove from the list
			RemoveAt(i);

			// Now remove it from all of the PPQs
			pPPQ = m_pInspection->GetPPQ();

			if( nullptr != pPPQ )
			{
				pPPQ->GetAllOutputs( ppPPQEntries );

				lPPQSize = static_cast<long>(ppPPQEntries.size());

				for( lPPQ = 0, found = false; lPPQ < lPPQSize; lPPQ++ )
				{
					pIOEntry = ppPPQEntries[lPPQ];
					if( 0 == strcmp( pIOEntry->m_pValueObject->GetCompleteObjectName(), 
									 pPublishedOutObjectInfo->PObject->GetCompleteObjectName() ) )
					{
						pPPQ->RemoveOutput( pIOEntry );
						found = true;
					}// end if
				}// end for
			}
			if( !found )
			{
				SvStl::MessageMgrNoDisplay e( SvStl::LogOnly );
				e.setMessage( SVMSG_SVO_55_DEBUG_BREAK_ERROR, SvO::ErrorFindingPPQEntries, StdMessageParams, SvOi::Err_17043_ErrorFindingPPQEntries );
				DebugBreak();
			}
		}// end if
	}// end for

	// check for new items
	for( int n = 0; n < newList.GetSize(); n++ )
	{
		SVOutObjectInfoStruct* pOutObjectInfo = newList.GetAt( n );

		bool found = false;

		// check for new items
		for( int j = 0; j < GetSize(); j++ )
		{
			SVOutObjectInfoStruct* pPublishedOutObjectInfo = GetAt( j );
			if( pOutObjectInfo == pPublishedOutObjectInfo )
			{
				found = true;
				break;
			}
		}

		if( !found ) // not in original list - have to add it
		{
			//if( not yet published )
			SVPublicAttributeEntryStruct* pPublicAttribute = pOutObjectInfo->PObject->GetPublicAttribute();

			SVInObjectInfoStruct InObjectInfo;

			InObjectInfo.SetObject( m_pInspection );
			InObjectInfo.SetInputObject( pOutObjectInfo->UniqueObjectID );

			// connect to the object
			if( ::SVSendMessage( pOutObjectInfo->PObject, SVM_CONNECT_OBJECT_INPUT, reinterpret_cast<DWORD_PTR>(&InObjectInfo), NULL ) == SVMR_SUCCESS )
			{
				SVValueObjectClass* pValueObject = dynamic_cast< SVValueObjectClass* >( SVObjectManagerClass::Instance().GetObject( pOutObjectInfo->UniqueObjectID ) );
				if( pValueObject )
				{
					SVDigitalOutputObject* pDigital( nullptr );
					CString strName = pValueObject->GetCompleteObjectName();

					if( nullptr != pConfig ){ pOutputList = pConfig->GetOutputObjectList( ); }

					if( nullptr != pOutputList ){ pOutputList->GetOutput( strName, pDigital ); }

					// Add Outputs to the PPQ
					SVIOEntryHostStructPtr pIOEntry;

					SVObjectClass* pValueParent = pValueObject->GetInspection();

					if( nullptr == pValueParent )
					{
						pValueParent = pValueObject->GetOwner();
						while( pValueParent != NULL && !SV_IS_KIND_OF( pValueParent, SVInspectionProcess ) )
							pValueParent = pValueParent->GetOwner();
					}

					pPPQ = m_pInspection->GetPPQ();
					BOOL bDigital = SV_IS_KIND_OF( pValueObject, SVBoolValueObjectClass );

					if( bDigital )
					{
						pIOEntry = new SVIOEntryHostStruct;
						pIOEntry->m_DeleteValueObject = false;
						pIOEntry->m_pValueObject	= pValueObject;
						pIOEntry->m_pValueParent	= pValueParent;
						pIOEntry->m_ObjectType		= IO_DIGITAL_OUTPUT;
						pIOEntry->m_PPQIndex		= -1;
						pIOEntry->m_Enabled			= ( nullptr != pDigital );

						if( pIOEntry->m_Enabled )
						{
							pIOEntry->m_IOId = pDigital->GetUniqueObjectID();
						}

						if( nullptr != pPPQ ){ pPPQ->AddOutput( pIOEntry ); }
					}
					else
					{
						pIOEntry = new SVIOEntryHostStruct;
						pIOEntry->m_DeleteValueObject = false;
						pIOEntry->m_pValueObject	= pValueObject;
						pIOEntry->m_pValueParent	= pValueParent;
						pIOEntry->m_ObjectType		= IO_REMOTE_OUTPUT;
						pIOEntry->m_PPQIndex		= -1;
						pIOEntry->m_Enabled			= true;

						pIOEntry->m_IOId = pValueObject->GetUniqueObjectID();

						if( nullptr != pPPQ ){ pPPQ->AddOutput( pIOEntry ); }
					}
				}// end if

				// add to the list
				Add( pOutObjectInfo );
			}// end if
		}// end if
	}// end for
}// end Refresh

void SVPublishListClass::Release(SVTaskObjectClass * pRootObject)
{
	// We must release all the outputs that had been marked for publishing
	// all outputs marked as selected for publishing
	// Note:: both Lists must be from the same root object
	// which at this point is SVToolSetClass

	SVPPQObject* pPPQ( nullptr );
	SVIOEntryHostStructPtrList ppPPQEntries;
	SVIOEntryHostStructPtr pIOEntry;
	bool found;
	long lPPQSize;
	long lPPQ;

	// check for removed items
	for( long i = GetSize() - 1; i >= 0; i-- )
	{
		SVOutObjectInfoStruct* pPublishedOutObjectInfo = GetAt( i );

		SVInObjectInfoStruct InObjectInfo;

		InObjectInfo.SetObject( m_pInspection );
		InObjectInfo.SetInputObject( pPublishedOutObjectInfo->UniqueObjectID );

		// Disconnect
		::SVSendMessage(pPublishedOutObjectInfo->UniqueObjectID, SVM_DISCONNECT_OBJECT_INPUT, reinterpret_cast<DWORD_PTR>(&InObjectInfo), NULL );
		
		//if( should not be anymore published )
		SVPublicAttributeEntryStruct* pPublicAttribute = pPublishedOutObjectInfo->PObject->GetPublicAttribute();

		// remove from the list
		RemoveAt(i);

		// Now remove it from all of the PPQs
        ASSERT( m_pInspection );
		pPPQ = m_pInspection->GetPPQ();
		if ( nullptr != pPPQ )
		{
			pPPQ->GetAllOutputs( ppPPQEntries );

			lPPQSize = static_cast<long>(ppPPQEntries.size());

			for( lPPQ = 0, found = false; lPPQ < lPPQSize; lPPQ++ )
			{
				pIOEntry = ppPPQEntries[lPPQ];
				if( 0 == strcmp( pIOEntry->m_pValueObject->GetCompleteObjectName(), 
								 pPublishedOutObjectInfo->PObject->GetCompleteObjectName() ) )
				{
					pPPQ->RemoveOutput( pIOEntry );
					found = true;
				}// end if

			}// end for
		}

	}// end for

}// end Release

BOOL SVPublishListClass::RemovePublishedEntry( const SVGUID& RGuid )
{
	SVPPQObject* pPPQ( nullptr );
	SVIOEntryHostStructPtrList ppPPQEntries;
	SVIOEntryHostStructPtr pIOEntry;
	long lPPQSize = 0;
	long lPPQ = 0;

	for( int i = GetSize() - 1; i >= 0; i-- )
	{
		SVOutObjectInfoStruct* pPublishedOutObjectInfo = GetAt( i );
		if( pPublishedOutObjectInfo && pPublishedOutObjectInfo->UniqueObjectID == RGuid )
		{
			// remove from the list
			RemoveAt(i);
			bool found = false;

			// Now remove it from all of the PPQs
			pPPQ = m_pInspection->GetPPQ();

			if( nullptr != pPPQ )
			{
				pPPQ->GetAllOutputs( ppPPQEntries );
				lPPQSize = static_cast<long>(ppPPQEntries.size());
			}

			for( lPPQ = 0, found = false; lPPQ < lPPQSize; lPPQ++ )
			{
				pIOEntry = ppPPQEntries[lPPQ];
				if( 0 == strcmp( pIOEntry->m_pValueObject->GetCompleteObjectName(), 
								 pPublishedOutObjectInfo->PObject->GetCompleteObjectName() ) )
				{
					pPPQ->RemoveOutput( pIOEntry );
					found = true;
				}// end if

			}// end for

			return TRUE;
		}// end if

	}// end for

	return FALSE;
}// end RemovePublishedEntry

