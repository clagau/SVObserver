//******************************************************************************
//* COPYRIGHT (c) 2003 by SVResearch, Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVIOController
//* .File Name       : $Workfile:   SVIOController.cpp  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.14  $
//* .Check In Date   : $Date:   12 Dec 2014 13:10:18  $
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
#include "SVXMLLibrary/SVNavigateTree.h"
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
	SVOutputObjectArray ppNewOutputs;

	SVConfigurationObject* pConfig( nullptr );
	SVObjectManagerClass::Instance().GetConfigurationObject( pConfig );

	if( nullptr == pConfig || nullptr == pConfig->GetOutputObjectList( ) )
	{
		return FALSE;
	}

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
	BOOL bTmp = SVNavigateTree::GetItemBranch( rTree, CTAG_IODOC, htiParent, htiIODoc );
	if( bTmp )
	{
		bOk = SVNavigateTree::GetItem( rTree, CTAG_UNIQUE_REFERENCE_ID, htiIODoc, svVariant );
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
bool SVIOController::GetParameters( SVObjectXMLWriter& rWriter ) const
{
	bool bOk = true;

	_variant_t svVariant;

	svVariant = SVGUID( outObjectInfo.UniqueObjectID ).ToVARIANT();
	rWriter.WriteAttribute( CTAG_UNIQUE_REFERENCE_ID, svVariant );

	if( m_pRemoteOutputController != NULL )
	{
		// Remote Outputs
		bOk &= m_pRemoteOutputController->GetParameters( rWriter );
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

DWORD_PTR SVIOController::processMessage( DWORD DwMessageID, DWORD_PTR DwMessageValue, DWORD_PTR DwMessageContext )
{
	DWORD_PTR DwResult = NULL;

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
		SVOutputObjectList *pOutputList( nullptr );

		m_pModuleReady->m_pValueObject->SetValue( 1, p_Value );

		SVConfigurationObject* pConfig( nullptr );
		SVObjectManagerClass::Instance().GetConfigurationObject( pConfig );

		if( nullptr != pConfig ){ pOutputList = pConfig->GetOutputObjectList( ); }

		if( nullptr == pOutputList || !pOutputList->WriteOutput( m_pModuleReady, 1, true, false ) )
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
		SVOutputObjectList *pOutputList( nullptr );

		m_pRaidErrorBit->m_pValueObject->SetValue( 1, p_Value );

		SVConfigurationObject* pConfig( nullptr );
		SVObjectManagerClass::Instance().GetConfigurationObject( pConfig );

		if( nullptr != pConfig ){ pOutputList = pConfig->GetOutputObjectList( ); }

		if( nullptr == pOutputList || !pOutputList->WriteOutput( m_pRaidErrorBit, 1, true, false ) )
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

void SVIOController::SetupRemoteOutput( SVConfigurationObject* pConfig )
{
	if( m_pRemoteOutputController == NULL)
		m_pRemoteOutputController = new SVRemoteOutputDataController;

	if( m_pRemoteOutputController != NULL )
	{
		m_pRemoteOutputController->SetupRemoteOutput( pConfig );
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

void SVIOController::ReplaceOrAddMonitorList( const RemoteMonitorNamedList& rList )
{
	m_RemoteMonitorListController.ReplaceOrAddMonitorList(rList);
}

void SVIOController::ShowMonitorListTab()
{
	m_RemoteMonitorListController.HideShowViewTab();
}

void SVIOController::ValidateRemoteMonitorList()
{
	m_RemoteMonitorListController.ValidateInputs();
}

HRESULT SVIOController::ActivateRemoteMonitorList(const SVString& listName, bool bActivate)
{
	return m_RemoteMonitorListController.ActivateRemoteMonitorList(listName, bActivate);
}

void SVIOController::GetActiveRemoteMonitorList(RemoteMonitorList& rActiveList) const
{
	m_RemoteMonitorListController.GetActiveRemoteMonitorList(rActiveList);
}

void SVIOController::BuildPPQMonitorList(PPQMonitorList& ppqMonitorList) const
{
	m_RemoteMonitorListController.BuildPPQMonitorList(ppqMonitorList);
}

HRESULT SVIOController::SetRemoteMonitorListProductFilter(const SVString& listName, SVProductFilterEnum filter)
{
	return m_RemoteMonitorListController.SetRemoteMonitorListProductFilter(listName, filter);
}

HRESULT SVIOController::GetRemoteMonitorListProductFilter(const SVString& listName, SVProductFilterEnum& rFilter) const
{
	return m_RemoteMonitorListController.GetRemoteMonitorListProductFilter(listName, rFilter);
}

