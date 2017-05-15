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
#include "SVDigitalOutputObject.h"
#include "SVOCore/SVTaskObject.h"
#include "SVOutputObjectList.h"
#include "SVInfoStructs.h"
#include "SVObserver.h"
#include "SVInspectionProcess.h"
#include "SVConfigurationObject.h"
#include "SVPPQObject.h"
#include "SVStatusLibrary/ErrorNumbers.h"
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
	hProtectionMutex   = hProtectionMutex = CreateMutex( nullptr, false, nullptr );
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
		hProtectionMutex = nullptr;
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

	l_OutputList.GetSetAttributesList( SvOi::SV_PUBLISHABLE, &newList );

	SVPPQObject* pPPQ( nullptr );
	SVOutputObjectList* pOutputList ( nullptr );
	SVIOEntryHostStructPtrVector ppPPQEntries;
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
			bOk = ( nullptr != SVObjectManagerClass::Instance().GetObject( pPublishedOutObjectInfo->m_UniqueObjectID ) );
			if( !bOk )
			{
				RemoveAt( i );
				continue;
			}// end if

			SVInObjectInfoStruct InObjectInfo;

			InObjectInfo.SetObject( m_pInspection );
			InObjectInfo.SetInputObject( pPublishedOutObjectInfo->m_UniqueObjectID );

			// Disconnect
			SVObjectManagerClass::Instance().DisconnectObjectInput(pPublishedOutObjectInfo->m_UniqueObjectID, &InObjectInfo);

			//if( should not be anymore published )
			SVPublicAttributeEntryStruct* pPublicAttribute = pPublishedOutObjectInfo->m_pObject->GetPublicAttribute();

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
					if( pIOEntry->getObject()->GetCompleteName() == pPublishedOutObjectInfo->m_pObject->GetCompleteName() )
					{
						pPPQ->RemoveOutput( pIOEntry );
						found = true;
					}// end if
				}// end for
			}
			if( !found )
			{
				SvStl::MessageMgrStd e( SvStl::LogOnly );
				e.setMessage( SVMSG_SVO_55_DEBUG_BREAK_ERROR, SvStl::Tid_ErrorFindingPPQEntries, SvStl::SourceFileParams(StdMessageParams), SvStl::Err_17043_ErrorFindingPPQEntries );
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
			SVPublicAttributeEntryStruct* pPublicAttribute = pOutObjectInfo->m_pObject->GetPublicAttribute();

			SVInObjectInfoStruct InObjectInfo;

			InObjectInfo.SetObject( m_pInspection );
			InObjectInfo.SetInputObject( pOutObjectInfo->m_UniqueObjectID );

			// connect to the object
			if( pOutObjectInfo->m_pObject->ConnectObjectInput(&InObjectInfo) )
			{
				SVObjectClass* pObject = SVObjectManagerClass::Instance().GetObject( pOutObjectInfo->m_UniqueObjectID );
				if( nullptr != dynamic_cast<SvOi::IValueObject*> (pObject) )
				{
					SVDigitalOutputObject* pDigital( nullptr );

					if( nullptr != pConfig ){ pOutputList = pConfig->GetOutputObjectList( ); }

					if( nullptr != pOutputList )
					{ 
						pDigital = dynamic_cast<SVDigitalOutputObject*> (pOutputList->GetOutput( pObject->GetCompleteName().c_str())); 
					}

					// Add Outputs to the PPQ
					SVIOEntryHostStructPtr pIOEntry;

					pPPQ = m_pInspection->GetPPQ();
					BOOL bDigital = SV_IS_KIND_OF( pObject, SVBoolValueObjectClass );

					if( bDigital )
					{
						pIOEntry = new SVIOEntryHostStruct;
						pIOEntry->m_DeleteValueObject = false;
						pIOEntry->setObject(pObject);
						pIOEntry->getObject()->SetObjectOwner(pObject->GetOwner());
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
						pIOEntry->setObject(pObject);
						pIOEntry->getObject()->SetObjectOwner(pObject->GetOwner());
						pIOEntry->m_ObjectType		= IO_REMOTE_OUTPUT;
						pIOEntry->m_PPQIndex		= -1;
						pIOEntry->m_Enabled			= true;

						pIOEntry->m_IOId = pObject->GetUniqueObjectID();

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
	SVIOEntryHostStructPtrVector ppPPQEntries;
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
		InObjectInfo.SetInputObject( pPublishedOutObjectInfo->m_UniqueObjectID );

		// Disconnect
		SVObjectManagerClass::Instance().DisconnectObjectInput(pPublishedOutObjectInfo->m_UniqueObjectID, &InObjectInfo);
		
		//if( should not be anymore published )
		SVPublicAttributeEntryStruct* pPublicAttribute = pPublishedOutObjectInfo->m_pObject->GetPublicAttribute();

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
				if( pIOEntry->getObject()->GetCompleteName() == pPublishedOutObjectInfo->m_pObject->GetCompleteName() )
				{
					pPPQ->RemoveOutput( pIOEntry );
					found = true;
				}// end if

			}// end for
		}

	}// end for

}// end Release

bool SVPublishListClass::RemovePublishedEntry( const SVGUID& RGuid )
{
	SVPPQObject* pPPQ( nullptr );
	SVIOEntryHostStructPtrVector ppPPQEntries;
	SVIOEntryHostStructPtr pIOEntry;
	long lPPQSize = 0;
	long lPPQ = 0;

	for( int i = GetSize() - 1; i >= 0; i-- )
	{
		SVOutObjectInfoStruct* pPublishedOutObjectInfo = GetAt( i );
		if( pPublishedOutObjectInfo && pPublishedOutObjectInfo->m_UniqueObjectID == RGuid )
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
				if( pIOEntry->getObject()->GetCompleteName() == pPublishedOutObjectInfo->m_pObject->GetCompleteName() )
				{
					pPPQ->RemoveOutput( pIOEntry );
					found = true;
				}// end if

			}// end for

			return true;
		}// end if

	}// end for

	return false;
}// end RemovePublishedEntry

