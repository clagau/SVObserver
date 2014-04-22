//******************************************************************************
//* COPYRIGHT (c) 2003 by SVResearch, Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVIOController
//* .File Name       : $Workfile:   SVIOController.cpp  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.6  $
//* .Check In Date   : $Date:   17 Apr 2014 16:59:46  $
//******************************************************************************

#include "stdafx.h"
#include "SVIOController.h"

#include "SVObjectLibrary/SVObjectManagerClass.h"
#include "SVConfigurationLibrary/SVConfigurationTags.h"
#include "SVUtilityLibrary/SVGUID.h"

#include "SVConfigurationObject.h"
#include "SVDigitalOutputObject1.h"
#include "SVInfoStructs.h"
#include "SVIODoc.h"
#include "SVObserver.h"
#include "SVOutputObjectList.h"
#include "SVOutputStreamManager.h"
#include "SVXMLLibrary/SVNavigateTreeClass.h"
#include "SVSVIMStateClass.h"

SV_IMPLEMENT_CLASS( SVIOController, SVIOControllerGuid );

SVIOController::SVIOController( LPCSTR ObjectName )
: SVObjectClass( ObjectName )
, m_pModuleReady()
, m_pRaidErrorBit()
, m_pRemoteOutputController( NULL )
{
	LocalIntialize();
}

SVIOController::SVIOController( SVObjectClass *pOwner, int StringResourceID )
: SVObjectClass( pOwner, StringResourceID )
, m_pModuleReady()
, m_pRaidErrorBit()
, m_pRemoteOutputController( NULL )
{
	LocalIntialize();
}

void SVIOController::LocalIntialize()
{
	// Set up your type...
	outObjectInfo.ObjectTypeInfo.ObjectType = SVIOControllerType;

	m_pModuleReady = new SVIOEntryHostStruct;
	m_pModuleReady->m_pValueParent = this;
	m_pModuleReady->m_ObjectType = IO_DIGITAL_OUTPUT;
	m_pModuleReady->m_pValueObject = new SVBoolValueObjectClass;
	m_pModuleReady->m_pValueObject->SetName( _T( "Module Ready" ) );
	m_pModuleReady->m_pValueObject->SetObjectDepth( 100 );
	m_pModuleReady->m_pValueObject->ResetObject();
	m_pModuleReady->m_pValueObject->SetValue( 1, TRUE );

	m_pRaidErrorBit = new SVIOEntryHostStruct;
	m_pRaidErrorBit->m_pValueParent	= this;
	m_pRaidErrorBit->m_ObjectType = IO_DIGITAL_OUTPUT;
	m_pRaidErrorBit->m_pValueObject = new SVBoolValueObjectClass;
	m_pRaidErrorBit->m_pValueObject->SetName( _T( "Raid Error Indicator" ) );
	m_pRaidErrorBit->m_pValueObject->SetObjectDepth( 100 );
	m_pRaidErrorBit->m_pValueObject->ResetObject();
	m_pRaidErrorBit->m_pValueObject->SetValue( 1, FALSE );
}

SVIOController::~SVIOController()
{
	SVIODoc* l_pIODoc( GetIODoc() );

	if ( l_pIODoc != NULL )
	{
		SVObjectManagerClass::Instance().UnregisterIODoc( GetUniqueObjectID() );

		l_pIODoc->OnCloseDocument();
		l_pIODoc = NULL;
	}

	LocalDestroy();
}

void SVIOController::LocalDestroy()
{
#ifndef _WIN64
	// Destroy PLC
	m_PLCData.Destroy();
	TheSVObserverApp.m_PLCManager.ClosePLC();
#endif

	if( m_pRemoteOutputController != NULL )
	{
		delete m_pRemoteOutputController;

		m_pRemoteOutputController = NULL;
	}

	m_RemoteMonitorListController.Clear();

	m_pModuleReady.clear();
	m_pRaidErrorBit.clear();
}

BOOL SVIOController::RebuildOutputList()
{
	SVOutputObjectList	*pOutputList;
	SVOutputObjectArray ppNewOutputs;

	SVConfigurationObject* pConfig = NULL;
	SVObjectManagerClass::Instance().GetConfigurationObject( pConfig );

	if( !pConfig->GetOutputObjectList( &pOutputList ) )
		return FALSE;

	//Start of check to see if there is a raid failure
	if( TheSVObserverApp.IsProductTypeRAID() )
	{
		if ( SVSVIMStateClass::CheckState( SV_STATE_RAID_FAILURE) )
		{
			return SetRaidErrorBit( true );
		}
		else
		{
			return SetRaidErrorBit( false );
		}//end of check to see if there is a raid failure
	}
	else // config type is raid, but not hardware, so turn to a failed state.
	{
		return SetRaidErrorBit( true );
	}

	return TRUE;
}

BOOL SVIOController::SetParameters( SVTreeType& rTree, SVTreeType::SVBranchHandle htiParent )
{
	BOOL bOk = FALSE;

	_variant_t svVariant;

	SVTreeType::SVBranchHandle htiIODoc = NULL;
	BOOL bTmp = SVNavigateTreeClass::GetItemBranch( rTree, CTAG_IODOC, htiParent, htiIODoc );
	if( bTmp )
	{
		bOk = SVNavigateTreeClass::GetItem( rTree, CTAG_UNIQUE_REFERENCE_ID, htiIODoc, svVariant );
		if ( bOk )
		{
			SVGUID ObjectID( svVariant );

			SVObjectManagerClass::Instance().CloseUniqueObjectID( this );

			outObjectInfo.UniqueObjectID = ObjectID;

			SVObjectManagerClass::Instance().OpenUniqueObjectID( this );
		}

		if( m_pRemoteOutputController != NULL )
		{
			delete m_pRemoteOutputController;

			m_pRemoteOutputController = NULL;
		}
#ifndef _WIN64
		// Set PLC Data
		bOk &= m_PLCData.SetParameters( rTree, htiIODoc );
#endif
		m_pRemoteOutputController = new SVRemoteOutputDataController;

		bOk &= ( m_pRemoteOutputController != NULL );

		if( m_pRemoteOutputController != NULL )
		{
			// Set Remote Output Data
			bOk &= m_pRemoteOutputController->SetParameters( rTree, htiIODoc );
		}
	}

	return bOk;
}

// Sets up tree - Gets Data from SVIODoc
// Creates Tree items branches so data will be ready to save...
BOOL SVIOController::GetParameters( SVTreeType& rTree, SVTreeType::SVBranchHandle htiParent )
{
	BOOL bOk = true;

	_variant_t svVariant;

	svVariant = SVGUID( outObjectInfo.UniqueObjectID ).ToVARIANT();
	SVNavigateTreeClass::AddItem( rTree, htiParent, CTAG_UNIQUE_REFERENCE_ID, svVariant );
#ifndef _WIN64
	// PLC Data....
	bOk &= m_PLCData.GetParameters( rTree, htiParent );
#endif
	if( m_pRemoteOutputController != NULL )
	{
		// Remote Outputs
		bOk &= m_pRemoteOutputController->GetParameters( rTree, htiParent );
	}

	return bOk;
}

BOOL SVIOController::SetObjectDepth( int NewObjectDepth )
{
	BOOL l_bOk = SVObjectClass::SetObjectDepth( NewObjectDepth );

	l_bOk &= !( m_pModuleReady.empty() ) && m_pModuleReady->m_pValueObject->SetObjectDepth( NewObjectDepth );
	l_bOk &= !( m_pRaidErrorBit.empty() ) && m_pRaidErrorBit->m_pValueObject->SetObjectDepth( NewObjectDepth );

	return l_bOk;
}

BOOL SVIOController::SetObjectDepthWithIndex( int NewObjectDepth, int NewLastSetIndex )
{
	BOOL l_bOk = SVObjectClass::SetObjectDepthWithIndex( NewObjectDepth, NewLastSetIndex );

	l_bOk &= !( m_pModuleReady.empty() ) && m_pModuleReady->m_pValueObject->SetObjectDepthWithIndex( NewObjectDepth, NewLastSetIndex );
	l_bOk &= !( m_pRaidErrorBit.empty() ) && m_pRaidErrorBit->m_pValueObject->SetObjectDepthWithIndex( NewObjectDepth, NewLastSetIndex );

	return l_bOk;
}

BOOL SVIOController::SetImageDepth( long lDepth )
{
	BOOL l_bOk = SVObjectClass::SetImageDepth( lDepth );

	l_bOk &= !( m_pModuleReady.empty() ) && m_pModuleReady->m_pValueObject->SetImageDepth( lDepth );
	l_bOk &= !( m_pRaidErrorBit.empty() ) && m_pRaidErrorBit->m_pValueObject->SetImageDepth( lDepth );

	return l_bOk;
}

void SVIOController::ResetName()
{
	SVObjectClass::ResetName();

	if( GetIODoc() != NULL )
	{
		GetIODoc()->SetTitle( GetName() );
	}
}

void SVIOController::SetName( const CString& StrString )
{
	SVObjectClass::SetName( StrString );

	if( GetIODoc() != NULL )
	{
		GetIODoc()->SetTitle( GetName() );
	}
}

LONG_PTR SVIOController::processMessage( DWORD DwMessageID, LONG_PTR DwMessageValue, LONG_PTR DwMessageContext )
{
	LONG_PTR DwResult = NULL;

	// Try to process message by yourself...
	DWORD dwPureMessageID = DwMessageID & SVM_PURE_MESSAGE;
	switch( dwPureMessageID )
	{
	case SVMSGID_RESET_ALL_OBJECTS:
		{
			HRESULT l_ResetStatus = ResetObject();
			if( l_ResetStatus != S_OK )
			{
				ASSERT( SUCCEEDED( l_ResetStatus ) );

				DwResult = SVMR_NO_SUCCESS;
			}
			else
			{
				DwResult = SVMR_SUCCESS;
			}
			break;
		}
	}

	if( DwResult == SVMR_NOT_PROCESSED || 
		( DwMessageID & SVM_NOTIFY_FIRST_RESPONDING ) != SVM_NOTIFY_FIRST_RESPONDING )
	{
		DwResult = ::SVSendMessage( m_pModuleReady->m_pValueObject, DwMessageID, DwMessageValue, DwMessageContext ) | DwResult;
	}

	if( DwResult == SVMR_NOT_PROCESSED || 
		( DwMessageID & SVM_NOTIFY_FIRST_RESPONDING ) != SVM_NOTIFY_FIRST_RESPONDING )
	{
		DwResult = ::SVSendMessage( m_pRaidErrorBit->m_pValueObject, DwMessageID, DwMessageValue, DwMessageContext ) | DwResult;
	}

	if( DwResult == SVMR_NOT_PROCESSED || 
		( DwMessageID & SVM_NOTIFY_FIRST_RESPONDING ) != SVM_NOTIFY_FIRST_RESPONDING )
	{
		DwResult = SVObjectClass::processMessage( DwMessageID, DwMessageValue, DwMessageContext ) | DwResult;
	}

	return DwResult;
}

HRESULT SVIOController::ResetObject()
{
	HRESULT l_hrOk = SVObjectClass::ResetObject();

	if( ::SVSendMessage( m_pModuleReady->m_pValueObject, SVM_RESET_ALL_OBJECTS, NULL, NULL ) != SVMR_SUCCESS )
	{
		l_hrOk = S_FALSE;
	}

	if( ::SVSendMessage( m_pRaidErrorBit->m_pValueObject, SVM_RESET_ALL_OBJECTS, NULL, NULL ) != SVMR_SUCCESS )
	{
		l_hrOk = S_FALSE;
	}

	//
	m_RemoteMonitorListController.ResetObject();

	if( ! RebuildOutputList() )
	{
		l_hrOk = S_FALSE;
	}

	return l_hrOk;
}

BOOL SVIOController::OnValidate()
{
	BOOL retVal = SVObjectClass::OnValidate();

	retVal = retVal && !( m_pModuleReady.empty() ) && m_pModuleReady->m_pValueObject->IsCreated();
	retVal = retVal && !( m_pRaidErrorBit.empty() ) && m_pRaidErrorBit->m_pValueObject->IsCreated();

	if( !retVal )
		SetInvalid();

	return retVal;
}

HRESULT SVIOController::SetModuleReady( bool p_Value )
{
	HRESULT l_Status( S_OK );

	// Don't set Module Ready if it isn't in the output list
	if( !( m_pModuleReady->m_IOId.empty() ) )
	{
		SVOutputObjectList *pOutputList;

		m_pModuleReady->m_pValueObject->SetValue( 1, p_Value );

		SVConfigurationObject* pConfig = NULL;
		SVObjectManagerClass::Instance().GetConfigurationObject( pConfig );

		pConfig->GetOutputObjectList( &pOutputList );

		if( !pOutputList->WriteOutput( m_pModuleReady, 1, true, false ) )
		{
			l_Status = S_FALSE; // JMS ERROR - Cannot write to module ready output.
		}
	}// end if

	return l_Status;
}

HRESULT SVIOController::SetRaidErrorBit( bool p_Value )
{
	HRESULT l_Status( S_OK );

	// Don't set Module Ready if it isn't in the output list
	if( !( m_pRaidErrorBit->m_IOId.empty() ) )
	{
		SVOutputObjectList *pOutputList;

		m_pRaidErrorBit->m_pValueObject->SetValue( 1, p_Value );

		SVConfigurationObject* pConfig = NULL;
		SVObjectManagerClass::Instance().GetConfigurationObject( pConfig );

		pConfig->GetOutputObjectList( &pOutputList );

		if( !pOutputList->WriteOutput( m_pRaidErrorBit, 1, true, false ) )
		{
			l_Status = S_FALSE; // JMS ERROR - Cannot write to module ready output.
		}
	}// end if

	return l_Status;
}

SVIOEntryHostStructPtr SVIOController::GetModuleReady()
{
	return m_pModuleReady;
}

SVIOEntryHostStructPtr SVIOController::GetRaidErrorBit()
{
	return m_pRaidErrorBit;
}

SVIODoc* SVIOController::GetIODoc() const
{
	return SVObjectManagerClass::Instance().GetIODoc( GetUniqueObjectID() );
}

#ifndef _WIN64
SVPLCDataController* SVIOController::GetPLCData()
{
	return &m_PLCData;
}

size_t SVIOController::GetPLCCount() const
{
	return m_PLCData.GetPLCCount();
}

void SVIOController::SetupPLC(SVConfigurationObject* p_pConfig )
{
	m_PLCData.SetupPLC( p_pConfig );
}

HRESULT SVIOController::GetPLCs( std::vector<CString>& p_astrPLCIds )
{
	return m_PLCData.GetPLCs( p_astrPLCIds );
}

LPCSTR SVIOController::GetConnectString()
{
	return m_PLCData.GetConnectString();
}

long SVIOController::GetQueueSize( const CString& p_strPLC )
{
	return m_PLCData.GetQueueSize( p_strPLC );
}

HRESULT SVIOController::GetHeartBeatAddress( CString& p_strHeartBeatAddress )
{
	return m_PLCData.GetHeartBeatAddress( p_strHeartBeatAddress );
}

HRESULT SVIOController::GetHeartBeatTime( long& p_lTime )
{
	return m_PLCData.GetHeartBeatTime( p_lTime );
}

HRESULT SVIOController::GetPLCControlData( SVMaterials& p_rMaterials, const CString& p_strPLC )
{
	return m_PLCData.GetPLCControlData( p_rMaterials, p_strPLC );
}

HRESULT SVIOController::SetPLCControlData( SVMaterials& p_rMaterials, const CString& p_strPLC )
{
	return m_PLCData.SetPLCControlData( p_rMaterials, p_strPLC );
}

CString SVIOController::AssociatePPQToPLC( const CString& p_strPPQ )
{
	return m_PLCData.AssociatePPQToPLC( p_strPPQ );
}

HRESULT SVIOController::WriteOutputs( const CString& p_strPLCName, SVProductInfoStruct *pProduct)
{
	return m_PLCData.WriteOutputs( p_strPLCName, pProduct );
}
#endif

SVGUID SVIOController::GetRemoteOutputController() const
{
	SVGUID l_ObjectId;

	if( m_pRemoteOutputController != NULL )
	{
		l_ObjectId = m_pRemoteOutputController->GetUniqueObjectID();
	}

	return l_ObjectId;
}

size_t SVIOController::GetRemoteOutputGroupCount() const
{
	size_t l_Count = 0;

	if( m_pRemoteOutputController != NULL )
	{
		l_Count = m_pRemoteOutputController->GetRemoteOutputGroupCount();
	}

	return l_Count;
}

void SVIOController::SetupRemoteOutput( SVConfigurationObject* p_pConfig )
{
	if( m_pRemoteOutputController == NULL)
		m_pRemoteOutputController = new SVRemoteOutputDataController;

	if( m_pRemoteOutputController != NULL )
	{
		m_pRemoteOutputController->SetupRemoteOutput( p_pConfig );
	}
}

HRESULT SVIOController::ClearRemoteOutputUnUsedData()
{
	HRESULT l_Status = S_OK;

	if( m_pRemoteOutputController != NULL )
	{
		l_Status = m_pRemoteOutputController->ClearUnUsedData();
	}
	else
	{
		l_Status = E_FAIL;
	}

	return l_Status;
}

HRESULT SVIOController::GetRemoteOutputGroupNames( std::vector<CString>& p_astrPPQs )
{
	HRESULT l_Status = S_OK;

	if( m_pRemoteOutputController != NULL )
	{
		l_Status = m_pRemoteOutputController->GetGroupNames( p_astrPPQs );
	}
	else
	{
		p_astrPPQs.clear();

		l_Status = E_FAIL;
	}

	return l_Status;
}

SVRemoteOutputGroup* SVIOController::GetRemoteOutputGroup( const CString& p_strRemoteGroupID ) const
{
	SVRemoteOutputGroup* l_pObject = NULL;

	if( m_pRemoteOutputController != NULL )
	{
		l_pObject = m_pRemoteOutputController->GetControlPar( p_strRemoteGroupID );
	}

	return l_pObject;
}

size_t SVIOController::GetRemoteOutputGroupItemCount( const CString& p_strRemoteGroupID ) const
{
	size_t l_Count = 0;

	if( m_pRemoteOutputController != NULL )
	{
		l_Count = m_pRemoteOutputController->GetItemCount( p_strRemoteGroupID );
	}

	return l_Count;
}

HRESULT SVIOController::GetRemoteOutputItem( const CString& p_strRemoteGroupId, long l_lIndex, SVRemoteOutputObject*& p_rItem ) const
{
	HRESULT l_Status = S_OK;

	if( m_pRemoteOutputController != NULL )
	{
		l_Status = m_pRemoteOutputController->GetItem( p_strRemoteGroupId, l_lIndex, p_rItem );
	}
	else
	{
		l_Status = E_FAIL;
	}

	return l_Status;
}

SVRemoteOutputObject* SVIOController::GetFirstRemoteOutputObject( const CString& p_strRemoteGroupId )
{
	SVRemoteOutputObject* l_pObject = NULL;

	if( m_pRemoteOutputController != NULL )
	{
		l_pObject = m_pRemoteOutputController->GetFirstObject( p_strRemoteGroupId );
	}

	return l_pObject;
}

HRESULT SVIOController::AddRemoteOutputItem( const CString& p_strRemoteGroupId, SVRemoteOutputObject*& p_pNewOutput, GUID p_InputObjectID, const CString p_strPPQ )
{
	HRESULT l_Status = S_OK;

	if( m_pRemoteOutputController != NULL )
	{
		l_Status = m_pRemoteOutputController->AddItem( p_strRemoteGroupId, p_pNewOutput, p_InputObjectID, p_strPPQ );
	}
	else
	{
		l_Status = E_FAIL;
	}

	return l_Status;
}

HRESULT SVIOController::DeleteRemoteOutput( const CString& p_strRemoteGroupId )
{
	HRESULT l_Status = S_OK;

	if( m_pRemoteOutputController != NULL )
	{
		l_Status = m_pRemoteOutputController->DeleteRemoteOutput( p_strRemoteGroupId );
	}
	else
	{
		l_Status = E_FAIL;
	}

	return l_Status;
}

HRESULT SVIOController::DeleteRemoteOutputEntry( const CString& p_strRemoteGroupId, SVRemoteOutputObject* p_pOutputObject )
{
	HRESULT l_Status = S_OK;

	if( m_pRemoteOutputController != NULL )
	{
		l_Status = m_pRemoteOutputController->DeleteRemoteOutputEntry( p_strRemoteGroupId, p_pOutputObject );
	}
	else
	{
		l_Status = E_FAIL;
	}

	return l_Status;
}

HRESULT SVIOController::RemoteOutputValidateInputs()
{
	HRESULT l_Status = S_OK;

	if( m_pRemoteOutputController != NULL )
	{
		l_Status = m_pRemoteOutputController->ValidateInputs();
	}
	else
	{
		l_Status = E_FAIL;
	}

	return l_Status;
}

bool SVIOController::SetupRemoteMonitorList(SVConfigurationObject* pConfig)
{
	return m_RemoteMonitorListController.Setup(pConfig);
}

void SVIOController::ClearRemoteMonitorList()
{
	m_RemoteMonitorListController.Clear();
}

const RemoteMonitorList& SVIOController::GetRemoteMonitorList() const
{
	return m_RemoteMonitorListController.GetRemoteMonitorList();
}

void SVIOController::SetRemoteMonitorList(const RemoteMonitorList& rList)
{
	m_RemoteMonitorListController.SetRemoteMonitorList(rList);
}

//******************************************************************************
//* LOG HISTORY:
//******************************************************************************
/*
$Log:   N:\PVCSarch65\ProjectFiles\archives\SVObserver_SRC\SVObserver\SVIOController.cpp_v  $
 * 
 *    Rev 1.6   17 Apr 2014 16:59:46   ryoho
 * Project:  SVObserver
 * Change Request (SCR) nbr:  886
 * SCR Title:  Add RunReject Server Support to SVObserver
 * Checked in by:  rYoho;  Rob Yoho
 * Change Description:  
 *   added functionality for the Remote Monitor List
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.5   01 Feb 2014 10:48:56   tbair
 * Project:  SVObserver
 * Change Request (SCR) nbr:  852
 * SCR Title:  Add Multiple Platform Support to SVObserver's Visual Studio Solution
 * Checked in by:  tBair;  Tom Bair
 * Change Description:  
 *   Changed SVSendmessage and processmessage to use LONG_PTR instead of DWORD.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.4   30 Oct 2013 10:45:20   tbair
 * Project:  SVObserver
 * Change Request (SCR) nbr:  852
 * SCR Title:  Add Multiple Platform Support to SVObserver's Visual Studio Solution
 * Checked in by:  tBair;  Tom Bair
 * Change Description:  
 *   Added #ifndef _WIN64 to prevent depricated PLC code from compiling in 64bit.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.3   01 Oct 2013 14:31:02   tbair
 * Project:  SVObserver
 * Change Request (SCR) nbr:  852
 * SCR Title:  Add Multiple Platform Support to SVObserver's Visual Studio Solution
 * Checked in by:  tBair;  Tom Bair
 * Change Description:  
 *   Add x64 platform.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.2   11 Jun 2013 15:26:22   bWalter
 * Project:  SVObserver
 * Change Request (SCR) nbr:  814
 * SCR Title:  Upgrade SVObserver to Compile Using Visual Studio 2010
 * Checked in by:  bWalter;  Ben Walter
 * Change Description:  
 *   Merged with svo_src label SVO 6.10 Beta 017.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.17   21 May 2013 13:39:12   tbair
 * Project:  SVObserver
 * Change Request (SCR) nbr:  822
 * SCR Title:  Remove DDE Input and Output functionality from SVObserver
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Remove Dead Code
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.1   13 May 2013 12:16:24   bWalter
 * Project:  SVObserver
 * Change Request (SCR) nbr:  814
 * SCR Title:  Upgrade SVObserver to Compile Using Visual Studio 2010
 * Checked in by:  bWalter;  Ben Walter
 * Change Description:  
 *   Merged with svo_src label SVO 6.10 Beta 014.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.16   02 May 2013 11:05:48   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  810
 * SCR Title:  Remove support for Legacy Tools
 * Checked in by:  rYoho;  Rob Yoho
 * Change Description:  
 *   Updated process message method to fix formatting issues and updated the reset object section to only assert when the HRESULT is a failed status.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.0   23 Apr 2013 11:08:48   bWalter
 * Project:  SVObserver
 * Change Request (SCR) nbr:  814
 * SCR Title:  Upgrade SVObserver to Compile Using Visual Studio 2010
 * Checked in by:  bWalter;  Ben Walter
 * Change Description:  
 *   Initial check in to SVObserver_src.  (Merged with svo_src label SVO 6.10 Beta 008.)
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.15   10 Jan 2013 15:04:30   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  805
 * SCR Title:  Fix Value Object Indexing Problem when Shortening PPQ
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Merged 6.01 branch code into main development code base.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.14   11 Oct 2012 10:54:20   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  602
 * SCR Title:  Revise the Toolset Parsing and Object Creation Methodology
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated include information based on the navigate tree class moving to the XML library.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.13   25 Sep 2012 15:19:20   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  602
 * SCR Title:  Revise the Toolset Parsing and Object Creation Methodology
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated to use new tree type.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.12   18 Sep 2012 18:21:40   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  602
 * SCR Title:  Revise the Toolset Parsing and Object Creation Methodology
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated code to use the templated XML objects that moved from the SVLibrary to the new XML Library and to the Configuration Library.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.11   04 Sep 2012 15:27:22   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  602
 * SCR Title:  Revise the Toolset Parsing and Object Creation Methodology
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated Navigate Tree class to not force the programmer to create an instance of the class to use it.  Converted all functionality to static.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.10.1.0   08 Jan 2013 12:15:16   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  805
 * SCR Title:  Fix Value Object Indexing Problem when Shortening PPQ
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Added new virtual method to classes to fix adjusting Object Depth of Value Object and Last Set Index.  Also, updated code with new methodology.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.10   09 Jul 2012 14:03:52   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  769
 * SCR Title:  Fix Problems and Crashes with Inspection Document Display Updates
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated source code to change the include for SVIM State.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.9   19 Jun 2012 12:54:34   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  744
 * SCR Title:  Add Shared Memory and Socket Functionality for Run Page Web Server
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Optimized the application state functionality.  The application state class was altered to simplify its usage and elements were added to object manager to separate tracking states.
 * Centralized the location of the configuration object identifier and removed synchronization problems.
 * 
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.8   10 Feb 2012 09:29:28   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  747
 * SCR Title:  Add New Remote Input Tool to SVObserver
 * Checked in by:  jSpila;  Joseph Spila
 * Change Description:  
 *   Updated functionality to remove bugs associated with the remote input tool and communications to that tool.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.7   30 Jan 2012 11:12:40   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  748
 * SCR Title:  Add Remote Output Steams to SVObserver
 * Checked in by:  jSpila;  Joseph Spila
 * Change Description:  
 *   Modified source code to consolidate PLC and New Output stream functionality to I/O Controller.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.6   06 Jun 2011 13:06:18   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  722
 * SCR Title:  Fix a problem with Raid Error Information on X2 systems
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated source code to correct issues with the RAID error information associated with the X2 products.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.5   19 Apr 2011 16:17:06   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  707
 * SCR Title:  Change Inspection Display Functionality to Force Display of Last Inspected
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated code to fix issues with IO Assignment.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.4   18 Apr 2011 10:24:14   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  707
 * SCR Title:  Change Inspection Display Functionality to Force Display of Last Inspected
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated code to move the shared data for the IO Entry data to a different class to fix issues with PPQ and Inspection shared data.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.3   15 Dec 2010 14:05:08   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  707
 * SCR Title:  Change Inspection Display Functionality to Force Display of Last Inspected
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated combind logic back into Output List from the PPQ.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.2   15 Dec 2010 10:00:20   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  707
 * SCR Title:  Change Inspection Display Functionality to Force Display of Last Inspected
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated source code to fix connections and synchronization issues with the IO Sub-system.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.1   13 Dec 2010 11:23:18   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  707
 * SCR Title:  Change Inspection Display Functionality to Force Display of Last Inspected
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Change the usage of _variant_t to clear the variable after use to eliminate invalid variant type for assignment.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.0   07 Dec 2010 15:13:46   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  707
 * SCR Title:  Change Inspection Display Functionality to Force Display of Last Inspected
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated source code to include changes in notification functionality using the Observer Design Pattern.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
*/
