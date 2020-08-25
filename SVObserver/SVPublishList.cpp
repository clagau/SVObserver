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
#include "SVObjectLibrary/SVObjectClass.h"
#include "SVObjectLibrary/SVObjectManagerClass.h"
#include "InspectionEngine/SVTaskObject.h"
#include "SVIOLibrary/SVOutputObjectList.h"
#include "SVInfoStructs.h"
#include "SVInspectionProcess.h"
#include "SVConfigurationObject.h"
#include "SVPPQObject.h"
#include "SVStatusLibrary/ErrorNumbers.h"
#include "SVStatusLibrary\MessageManager.h"
#include "TriggerInformation/SVHardwareManifest.h"
#pragma endregion Includes

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

SVPublishList::SVPublishList()
{
	hProtectionMutex   = hProtectionMutex = CreateMutex( nullptr, false, nullptr );
	dwWaitTime		   = 2000;
	m_pInspection	   = nullptr;
}

SVPublishList::~SVPublishList()
{
	Destroy();
}

void SVPublishList::Destroy()
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

void SVPublishList::Refresh(SvIe::SVTaskObjectClass * pRootObject)
{
	SVOutputInfoListClass newList;

	// Find all outputs marked as selected for publishing
	// Note:: both Lists must be from the same root object
	// which at this point is SVToolSet
	SVOutputInfoListClass l_OutputList;

	pRootObject->GetOutputList( l_OutputList );

	l_OutputList.GetSetAttributesList( SvPb::publishable, &newList );

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
			bOk = ( nullptr != SVObjectManagerClass::Instance().GetObject( pPublishedOutObjectInfo->getObjectId() ) );
			if( !bOk )
			{
				RemoveAt( i );
				continue;
			}// end if

			SvOl::SVInObjectInfoStruct InObjectInfo;

			InObjectInfo.SetObject( m_pInspection );
			InObjectInfo.SetInputObject( pPublishedOutObjectInfo->getObjectId() );

			// Disconnect
			SVObjectManagerClass::Instance().DisconnectObjectInput(pPublishedOutObjectInfo->getObjectId(), &InObjectInfo);

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
					//@WARNING [gra][7.50][09.08.2017] Not this does not work for objects with index!
					if( pIOEntry->getObject()->getObjectId() == pPublishedOutObjectInfo->getObjectId() )
					{
						pPPQ->RemoveOutput( pIOEntry );
						found = true;
					}// end if
				}// end for
			}
			if( !found )
			{
				SvStl::MessageManager e(SvStl::MsgType::Log );
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
			SvOl::SVInObjectInfoStruct InObjectInfo;

			InObjectInfo.SetObject( m_pInspection );
			InObjectInfo.SetInputObject( pOutObjectInfo->getObjectId() );

			// connect to the object
			if( pOutObjectInfo->getObject()->ConnectObjectInput(&InObjectInfo) )
			{
				SVObjectClass* pObject = SVObjectManagerClass::Instance().GetObject( pOutObjectInfo->getObjectId() );
				if( nullptr != dynamic_cast<SvOi::IValueObject*> (pObject) )
				{
					SVOutputObjectPtr pOutput;

					if( nullptr != pConfig ){ pOutputList = pConfig->GetOutputObjectList( ); }

					if( nullptr != pOutputList )
					{ 
						pOutput = pOutputList->GetOutput( pObject->GetCompleteName().c_str()); 
					}

					// Add Outputs to the PPQ
					pPPQ = m_pInspection->GetPPQ();

					if(SvPb::SVBoolValueObjectType == pObject->GetObjectSubType())
					{
						pIOEntry = std::make_shared<SVIOEntryHostStruct>();
						pIOEntry->setLinkedObject(pObject);
						pIOEntry->getObject()->SetObjectOwner(pObject->GetParent());
						pIOEntry->m_ObjectType		= SvTi::SVHardwareManifest::isPlcSystem(pConfig->GetProductType()) ? IO_PLC_OUTPUT : IO_DIGITAL_OUTPUT;
						pIOEntry->m_PPQIndex		= -1;
						pIOEntry->m_Enabled			= ( nullptr != pOutput);

						if( pIOEntry->m_Enabled )
						{
							pIOEntry->m_IOId = pOutput->getObjectId();
						}

						if( nullptr != pPPQ ){ pPPQ->AddOutput( pIOEntry ); }
					}
					else
					{
						pIOEntry = std::make_shared<SVIOEntryHostStruct>();
						pIOEntry->setLinkedObject(pObject);
						pIOEntry->getObject()->SetObjectOwner(pObject->GetParent());
						pIOEntry->m_ObjectType		= IO_REMOTE_OUTPUT;
						pIOEntry->m_PPQIndex		= -1;
						pIOEntry->m_Enabled			= true;

						pIOEntry->m_IOId = pObject->getObjectId();

						if( nullptr != pPPQ ){ pPPQ->AddOutput( pIOEntry ); }
					}
				}// end if

				// add to the list
				Add( pOutObjectInfo );
			}// end if
		}// end if
	}// end for
}// end Refresh

void SVPublishList::Release(SvIe::SVTaskObjectClass*)
{
	// We must release all the outputs that had been marked for publishing
	// all outputs marked as selected for publishing
	// Note:: both Lists must be from the same root object
	// which at this point is SVToolSet

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

		SvOl::SVInObjectInfoStruct InObjectInfo;

		InObjectInfo.SetObject( m_pInspection );
		InObjectInfo.SetInputObject( pPublishedOutObjectInfo->getObjectId() );

		// Disconnect
		SVObjectManagerClass::Instance().DisconnectObjectInput(pPublishedOutObjectInfo->getObjectId(), &InObjectInfo);
		
		// remove from the list
		RemoveAt(i);

		// Now remove it from all of the PPQs
        assert( m_pInspection );
		pPPQ = m_pInspection->GetPPQ();
		if ( nullptr != pPPQ )
		{
			pPPQ->GetAllOutputs( ppPPQEntries );

			lPPQSize = static_cast<long>(ppPPQEntries.size());

			for( lPPQ = 0, found = false; lPPQ < lPPQSize; lPPQ++ )
			{
				pIOEntry = ppPPQEntries[lPPQ];
				//@WARNING [gra][7.50][09.08.2017] Not this does not work for objects with index!
				if( pIOEntry->getObject()->getObjectId() == pPublishedOutObjectInfo->getObjectId() )
				{
					pPPQ->RemoveOutput( pIOEntry );
					found = true;
				}// end if

			}// end for
		}

	}// end for

}// end Release

bool SVPublishList::RemovePublishedEntry(uint32_t id )
{
	SVPPQObject* pPPQ( nullptr );
	SVIOEntryHostStructPtrVector ppPPQEntries;
	SVIOEntryHostStructPtr pIOEntry;
	long lPPQSize = 0;
	long lPPQ = 0;

	for( int i = GetSize() - 1; i >= 0; i-- )
	{
		SVOutObjectInfoStruct* pPublishedOutObjectInfo = GetAt( i );
		if( pPublishedOutObjectInfo && pPublishedOutObjectInfo->getObjectId() == id )
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
				//@WARNING [gra][7.50][09.08.2017] Not this does not work for objects with index!
				if( pIOEntry->getObject()->getObjectId() == pPublishedOutObjectInfo->getObjectId() )
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

