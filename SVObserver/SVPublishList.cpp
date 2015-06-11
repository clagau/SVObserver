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
#include "SVObjectLibrary/SVObjectManagerClass.h"
#include "SVDigitalOutputObject1.h"
#include "SVTaskObject.h"
#include "SVOutputObjectList.h"
#include "SVInfoStructs.h"
#include "SVObserver.h"
#include "SVInspectionProcess.h"
#include "SVConfigurationObject.h"
#include "SVPPQObject.h"
#include "ObjectInterfaces\ErrorNumbers.h"
#include "SVStatusLibrary/ExceptionManager.h"
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
				SvStl::ExceptionMgr1 e; // The default constructor sets the type to LogOnly.
				e.setMessage( SVMSG_SVO_55_DEBUG_BREAK_ERROR, SvO::ErrorFindingPPQEntries, StdExceptionParams, SvOi::Err_17043_ErrorFindingPPQEntries );
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

//******************************************************************************
//* LOG HISTORY:
//******************************************************************************
/*
$Log:   N:\PVCSarch65\ProjectFiles\archives\SVObserver_SRC\SVObserver\SVPublishList.cpp_v  $
 * 
 *    Rev 1.7   15 May 2014 12:31:44   tbair
 * Project:  SVObserver
 * Change Request (SCR) nbr:  852
 * SCR Title:  Add Multiple Platform Support to SVObserver's Visual Studio Solution
 * Checked in by:  tBair;  Tom Bair
 * Change Description:  
 *   Changed SendMessage to use proper type cast of DWORD_PTR.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.6   01 Feb 2014 12:03:54   tbair
 * Project:  SVObserver
 * Change Request (SCR) nbr:  852
 * SCR Title:  Add Multiple Platform Support to SVObserver's Visual Studio Solution
 * Checked in by:  tBair;  Tom Bair
 * Change Description:  
 *   Changed SVSendmessage and processmessage to use LONG_PTR instead of DWORD.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.5   02 Oct 2013 07:12:22   tbair
 * Project:  SVObserver
 * Change Request (SCR) nbr:  852
 * SCR Title:  Add Multiple Platform Support to SVObserver's Visual Studio Solution
 * Checked in by:  tBair;  Tom Bair
 * Change Description:  
 *   Add x64 platform.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.4   07 Aug 2013 13:23:12   sjones
 * Project:  SVObserver
 * Change Request (SCR) nbr:  809
 * SCR Title:  Non I/O SVIM
 * Checked in by:  tBair;  Tom Bair
 * Change Description:  
 *   Renamed SVIOEntryHostStructPtrVector to SVIOEntryHostStructPtrList.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.3   05 Jul 2013 15:01:28   bwalter
 * Project:  SVObserver
 * Change Request (SCR) nbr:  814
 * SCR Title:  Upgrade SVObserver to Compile Using Visual Studio 2010
 * Checked in by:  bWalter;  Ben Walter
 * Change Description:  
 *   Merged with svo_src label SVO 6.10 Beta 021.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.25   11 Jun 2013 13:11:28   tbair
 * Project:  SVObserver
 * Change Request (SCR) nbr:  822
 * SCR Title:  Remove DDE Input and Output functionality from SVObserver
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Added logic to refresh that adds other non boolean types as IO_REMOTE_OUTPUT type outputs to the available outputs list.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.2   11 Jun 2013 15:37:38   bWalter
 * Project:  SVObserver
 * Change Request (SCR) nbr:  814
 * SCR Title:  Upgrade SVObserver to Compile Using Visual Studio 2010
 * Checked in by:  bWalter;  Ben Walter
 * Change Description:  
 *   Merged with svo_src label SVO 6.10 Beta 017.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.24   21 May 2013 13:39:14   tbair
 * Project:  SVObserver
 * Change Request (SCR) nbr:  822
 * SCR Title:  Remove DDE Input and Output functionality from SVObserver
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Remove Dead Code
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.1   06 May 2013 20:12:04   bWalter
 * Project:  SVObserver
 * Change Request (SCR) nbr:  814
 * SCR Title:  Upgrade SVObserver to Compile Using Visual Studio 2010
 * Checked in by:  bWalter;  Ben Walter
 * Change Description:  
 *   Merged with svo_src label SVO 6.10 Beta 010.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.23   16 Apr 2013 15:35:48   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  822
 * SCR Title:  Remove DDE Input and Output functionality from SVObserver
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Removed DDE functionality from the source code.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.0   23 Apr 2013 14:38:24   bWalter
 * Project:  SVObserver
 * Change Request (SCR) nbr:  814
 * SCR Title:  Upgrade SVObserver to Compile Using Visual Studio 2010
 * Checked in by:  bWalter;  Ben Walter
 * Change Description:  
 *   Initial check in to SVObserver_src.  (Merged with svo_src label SVO 6.10 Beta 008.)
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.22   23 Aug 2012 10:27:04   tbair
 * Project:  SVObserver
 * Change Request (SCR) nbr:  779
 * SCR Title:  Fix Problems with DDE Inputs and DDE Outputs
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Fix issue with available outputs for dee and discretes.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.21   16 Aug 2012 17:07:48   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  779
 * SCR Title:  Fix Problems with DDE Inputs and DDE Outputs
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated output list to be able to collect an output if it exists in the output list.  This adjust the behavior in the publish list to make sure the lists are updated correctly in the PPQ.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.20   15 Aug 2012 14:41:12   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  779
 * SCR Title:  Fix Problems with DDE Inputs and DDE Outputs
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated source code to use the new Singleton version of the SVIOConfigurationInterfaceClass.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.19   07 Aug 2012 10:47:40   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  779
 * SCR Title:  Fix Problems with DDE Inputs and DDE Outputs
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated Refresh method to fix object assignment problem with DDE Inputs and DDE Outputs.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.18   18 Jul 2012 14:19:46   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  769
 * SCR Title:  Fix Problems and Crashes with Inspection Document Display Updates
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Removed obsolete methods assiciated with overlay drawling and fixed problem with copying data to byte buffer have wrong format.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.17   02 Jul 2012 17:30:12   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  769
 * SCR Title:  Fix Problems and Crashes with Inspection Document Display Updates
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated source code to promote new display functionality.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.16   19 Jun 2012 14:05:56   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  744
 * SCR Title:  Add Shared Memory and Socket Functionality for Run Page Web Server
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Protected Object Information structure attributes.  This change will add accessor methods to get and set attributes.
 * Converted GUID to SVGUID.   This allows the use of standard operators for comparison and methods for conversion.
 * Centralized the location of the configuration object identifier and removed synchronization problems.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.15   19 Apr 2011 16:22:08   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  707
 * SCR Title:  Change Inspection Display Functionality to Force Display of Last Inspected
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated code to fix issues with IO Assignment.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.14   18 Apr 2011 10:59:38   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  707
 * SCR Title:  Change Inspection Display Functionality to Force Display of Last Inspected
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated code to move the shared data for the IO Entry data to a different class to fix issues with PPQ and Inspection shared data.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.13   15 Dec 2010 10:36:00   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  707
 * SCR Title:  Change Inspection Display Functionality to Force Display of Last Inspected
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated source code to fix connections and synchronization issues with the IO Sub-system.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.12   08 Dec 2010 13:29:24   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  707
 * SCR Title:  Change Inspection Display Functionality to Force Display of Last Inspected
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated source code to include changes in notification functionality using the Observer Design Pattern.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.11   08 Nov 2010 14:46:28   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  704
 * SCR Title:  Upgrade SVObserver Version for 5.33 Release
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated source code to remove duplicate container types.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.10   29 Jun 2010 14:08:44   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  693
 * SCR Title:  Fix Performance Issue with Inspection Process
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated source to change object validity test and casting changes.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.9   01 Jun 2010 15:05:06   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  693
 * SCR Title:  Fix Performance Issue with Inspection Process
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated source code to improve ability to track performance.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.8   16 Dec 2009 12:29:32   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  677
 * SCR Title:  Fix problem in camera notify thread
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Fix issues with includes and comments.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.7   16 Feb 2005 15:04:22   ryoho
 * Project:  SVObserver
 * Change Request (SCR) nbr:  440
 * SCR Title:  Create Internal Tool Object to Managing Tool Extents and Result Extents
 * Checked in by:  rYoho;  Rob Yoho
 * Change Description:  
 *   updated header files
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.6   28 May 2003 10:38:00   rschock
 * Project:  SVObserver
 * Change Request (SCR) nbr:  341
 * SCR Title:  DDE Input and Outputs are no longer working correctly
 * Checked in by:  rSchock;  Rosco Schock
 * Change Description:  
 *   Made fix to delete all outputs, ie all types, when a published output is removed.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.5   25 Apr 2003 11:25:56   rschock
 * Project:  SVObserver
 * Change Request (SCR) nbr:  341
 * SCR Title:  DDE Input and Outputs are no longer working correctly
 * Checked in by:  rSchock;  Rosco Schock
 * Change Description:  
 *   Fixed several problems so that the DDE inputs and outputs will work like they used to in pre SVO 4.00 versions.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.4   22 Apr 2003 14:19:44   rschock
 * Project:  SVObserver
 * Change Request (SCR) nbr:  346
 * SCR Title:  Update SVObserver to Version 4.21 Release
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Redid the #include defines and standardized the Tracker log headers and removed warning from release mode builds.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.3   16 Apr 2003 17:31:46   rschock
 * Project:  SVObserver
 * Change Request (SCR) nbr:  341
 * SCR Title:  DDE Input and Outputs are no longer working correctly
 * Checked in by:  rSchock;  Rosco Schock
 * Change Description:  
 *   Made modifications to the DDE inputs/outputs. Some objects are now available again and fixed some bugs with existing code. State output and PPQ inputs are again available as DDE outputs.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.2   20 Nov 2002 11:09:44   ryoho
 * Project:  SVObserver
 * Change Request (SCR) nbr:  272
 * SCR Title:  Outputs do not always get set correctly.
 * Checked in by:  rYoho;  Rob Yoho
 * Change Description:  
 *   changes made due to changes in the way outputs are handled.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.1   13 Jun 2001 09:42:06   ed
 * Project:  SVObserver
 * Change Request (SCR) nbr:  200
 * SCR Title:  Update code to implement SVIM COM Server
 * Checked in by:  Ed;  Ed Chobanoff
 * Change Description:  
 *   Rearranged #include statements.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.0   23 Nov 1999 13:29:06   mike
 * Project:  SVObserver
 * Change Request (SCR) nbr:  61
 * SCR Title:  Update PVCS versioning to version 3.0.
 * Checked in by:  Mike;  Mike McCarl
 * Change Description:  
 *   
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.4   18 Nov 1999 08:47:04   sjones
 * Project:  SVObserver
 * Change Request (SCR) nbr:  9
 * SCR Title:  Integr. a common object interface to handle and to identify obj. in-/outputs
 * Checked in by:  sJones;  Steve Jones
 * Change Description:  
 *   Added RebuildPublicResultTable method in support of renaming documents 
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.3   19 Oct 1999 13:25:20   sjones
 * Project:  SVObserver
 * Change Request (SCR) nbr:  1
 * SCR Title:  Integrate a common validity check for all tool base class inputs and outputs.
 * Checked in by:  sJones;  Steve Jones
 * Change Description:  
 *   Revised to send Disconnect message when removing
 * published item
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.2   13 Oct 1999 12:59:28   sjones
 * Project:  SVObserver
 * Change Request (SCR) nbr:  9
 * SCR Title:  Integr. a common object interface to handle and to identify obj. in-/outputs
 * Checked in by:  sJones;  Steve Jones
 * Change Description:  
 *   Added RemovePublishedEntry method.
 * Revised Refresh method to send connect message to
 * the output objects (embeddeds) when adding to published list. 
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.1   10 Sep 1999 17:40:46   sjones
 * Project:  SVObserver
 * Change Request (SCR) nbr:  9
 * SCR Title:  Integr. a common object interface to handle and to identify obj. in-/outputs
 * Checked in by:  sJones;  Steve Jones
 * Change Description:  
 *   Revised Refreshing of the published list
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.0   08 Sep 1999 09:05:00   sjones
 * Project:  SVObserver
 * Change Request (SCR) nbr:  9
 * SCR Title:  Integr. a common object interface to handle and to identify obj. in-/outputs
 * Checked in by:  sJones;  Steve Jones
 * Change Description:  
 *   New Modules for maintaining a list of published outputs.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
*/