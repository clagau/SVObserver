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

#pragma region Includes
#include "stdafx.h"
#include "SVIOController.h"

#include "SVObjectLibrary/SVObjectManagerClass.h"
#include "SVXMLLibrary/SVConfigurationTags.h"
#include "SVUtilityLibrary/SVGUID.h"

#include "SVIODoc.h"
#include "SVConfigurationObject.h"
#include "SVDigitalOutputObject.h"
#include "SVInfoStructs.h"
#include "SVObserver.h"
#include "SVOutputObjectList.h"
#include "SVOutputStreamManager.h"
#include "SVXMLLibrary/SVNavigateTree.h"
#include "SVSVIMStateClass.h"
#pragma endregion Includes

SV_IMPLEMENT_CLASS( SVIOController, SVIOControllerGuid );

SVIOController::SVIOController( LPCTSTR ObjectName )
: SVObjectClass( ObjectName )
, m_pModuleReady()
, m_pRaidErrorBit()
, m_pRemoteOutputController( nullptr )
, m_pIODoc(nullptr)
{
	LocalIntialize();
}

SVIOController::SVIOController( SVObjectClass *pOwner, int StringResourceID )
: SVObjectClass( pOwner, StringResourceID )
, m_pModuleReady()
, m_pRaidErrorBit()
, m_pRemoteOutputController( nullptr )
, m_pIODoc(nullptr)
{
	LocalIntialize();
}

void SVIOController::LocalIntialize()
{
	// Set up your type...
	m_outObjectInfo.ObjectTypeInfo.ObjectType = SVIOControllerType;

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
	if ( nullptr != m_pIODoc )
	{
		m_pIODoc->OnCloseDocument();
		m_pIODoc = nullptr;
	}

	LocalDestroy();
}

void SVIOController::LocalDestroy()
{
	if( nullptr != m_pRemoteOutputController )
	{
		delete m_pRemoteOutputController;

		m_pRemoteOutputController = nullptr;
	}

	m_RemoteMonitorListController.Clear();

	m_pModuleReady.clear();
	m_pRaidErrorBit.clear();
}

void SVIOController::SetIODoc(SVIODoc* pDoc)
{
	m_pIODoc = pDoc;
}

SVIODoc* SVIOController::GetIODoc() const
{
	return m_pIODoc;
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

	SVTreeType::SVBranchHandle htiIODoc = nullptr;
	BOOL bTmp = SVNavigateTree::GetItemBranch( rTree, CTAG_IODOC, htiParent, htiIODoc );
	if( bTmp )
	{
		bOk = SVNavigateTree::GetItem( rTree, CTAG_UNIQUE_REFERENCE_ID, htiIODoc, svVariant );
		if ( bOk )
		{
			SVGUID ObjectID( svVariant );

			SVObjectManagerClass::Instance().CloseUniqueObjectID( this );

			m_outObjectInfo.UniqueObjectID = ObjectID;

			SVObjectManagerClass::Instance().OpenUniqueObjectID( this );
		}

		if( nullptr != m_pRemoteOutputController )
		{
			delete m_pRemoteOutputController;

			m_pRemoteOutputController = nullptr;
		}

		m_pRemoteOutputController = new SVRemoteOutputDataController;

		bOk &= ( nullptr != m_pRemoteOutputController );

		if( nullptr != m_pRemoteOutputController )
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

	svVariant = SVGUID( m_outObjectInfo.UniqueObjectID ).ToVARIANT();
	rWriter.WriteAttribute( CTAG_UNIQUE_REFERENCE_ID, svVariant );

	if( nullptr != m_pRemoteOutputController )
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

	if( nullptr != m_pIODoc )
	{
		m_pIODoc->SetTitle( GetName() );
	}
}

void SVIOController::SetName( LPCTSTR StrString )
{
	SVObjectClass::SetName( StrString );

	if( nullptr != m_pIODoc )
	{
		m_pIODoc->SetTitle( GetName() );
	}
}

bool SVIOController::resetAllObjects( bool shouldNotifyFriends, bool silentReset )
{
	bool Result = ( S_OK == ResetObject() );
	ASSERT( Result );
	return Result;
}

HRESULT SVIOController::ResetObject()
{
	HRESULT l_hrOk = S_OK;

	if (nullptr == m_pModuleReady || nullptr == m_pModuleReady->m_pValueObject || !m_pModuleReady->m_pValueObject->resetAllObjects(true, false) )
	{
		l_hrOk = S_FALSE;
	}
	if (nullptr == m_pRaidErrorBit || nullptr == m_pRaidErrorBit->m_pValueObject || !m_pRaidErrorBit->m_pValueObject->resetAllObjects(true, false) )
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
	{
		SetInvalid();
	}
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
			l_Status = E_FAIL;
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

SVGUID SVIOController::GetRemoteOutputController() const
{
	SVGUID l_ObjectId;

	if( nullptr != m_pRemoteOutputController )
	{
		l_ObjectId = m_pRemoteOutputController->GetUniqueObjectID();
	}

	return l_ObjectId;
}

size_t SVIOController::GetRemoteOutputGroupCount() const
{
	size_t l_Count = 0;

	if( nullptr != m_pRemoteOutputController )
	{
		l_Count = m_pRemoteOutputController->GetRemoteOutputGroupCount();
	}

	return l_Count;
}

void SVIOController::SetupRemoteOutput( SVConfigurationObject* pConfig )
{
	if( nullptr == m_pRemoteOutputController )
	{
		m_pRemoteOutputController = new SVRemoteOutputDataController;
	}
	if( nullptr != m_pRemoteOutputController )
	{
		m_pRemoteOutputController->SetupRemoteOutput( pConfig );
	}
}

HRESULT SVIOController::ClearRemoteOutputUnUsedData()
{
	HRESULT l_Status = S_OK;

	if( nullptr != m_pRemoteOutputController )
	{
		l_Status = m_pRemoteOutputController->ClearUnUsedData();
	}
	else
	{
		l_Status = E_FAIL;
	}
	return l_Status;
}

HRESULT SVIOController::GetRemoteOutputGroupNames( SVStringVector& rPPQs )
{
	HRESULT l_Status = S_OK;

	if( nullptr !=  m_pRemoteOutputController )
	{
		l_Status = m_pRemoteOutputController->GetGroupNames( rPPQs );
	}
	else
	{
		rPPQs.clear();

		l_Status = E_FAIL;
	}

	return l_Status;
}

SVRemoteOutputGroup* SVIOController::GetRemoteOutputGroup( const SVString& rRemoteGroupID ) const
{
	SVRemoteOutputGroup* l_pObject = nullptr;

	if( nullptr != m_pRemoteOutputController )
	{
		l_pObject = m_pRemoteOutputController->GetControlPar( rRemoteGroupID );
	}

	return l_pObject;
}

size_t SVIOController::GetRemoteOutputGroupItemCount( const SVString& rRemoteGroupID ) const
{
	size_t l_Count = 0;

	if( nullptr != m_pRemoteOutputController )
	{
		l_Count = m_pRemoteOutputController->GetItemCount( rRemoteGroupID );
	}

	return l_Count;
}

HRESULT SVIOController::GetRemoteOutputItem( const SVString& rRemoteGroupId, long l_lIndex, SVRemoteOutputObject*& p_rItem ) const
{
	HRESULT l_Status = S_OK;

	if( nullptr != m_pRemoteOutputController )
	{
		l_Status = m_pRemoteOutputController->GetItem( rRemoteGroupId, l_lIndex, p_rItem );
	}
	else
	{
		l_Status = E_FAIL;
	}

	return l_Status;
}

SVRemoteOutputObject* SVIOController::GetFirstRemoteOutputObject( const SVString& rRemoteGroupId )
{
	SVRemoteOutputObject* l_pObject = nullptr;

	if( nullptr != m_pRemoteOutputController )
	{
		l_pObject = m_pRemoteOutputController->GetFirstObject( rRemoteGroupId );
	}

	return l_pObject;
}

HRESULT SVIOController::AddRemoteOutputItem( const SVString& rRemoteGroupId, SVRemoteOutputObject*& p_pNewOutput, GUID p_InputObjectID, const SVString& rPPQ )
{
	HRESULT l_Status = S_OK;

	if( nullptr != m_pRemoteOutputController )
	{
		l_Status = m_pRemoteOutputController->AddItem( rRemoteGroupId, p_pNewOutput, p_InputObjectID, rPPQ );
	}
	else
	{
		l_Status = E_FAIL;
	}

	return l_Status;
}

HRESULT SVIOController::DeleteRemoteOutput( const SVString& rRemoteGroupId )
{
	HRESULT l_Status = S_OK;

	if( nullptr != m_pRemoteOutputController )
	{
		l_Status = m_pRemoteOutputController->DeleteRemoteOutput( rRemoteGroupId );
	}
	else
	{
		l_Status = E_FAIL;
	}

	return l_Status;
}

HRESULT SVIOController::DeleteRemoteOutputEntry( const SVString& rRemoteGroupId, SVRemoteOutputObject* p_pOutputObject )
{
	HRESULT l_Status = S_OK;

	if( nullptr != m_pRemoteOutputController )
	{
		l_Status = m_pRemoteOutputController->DeleteRemoteOutputEntry( rRemoteGroupId, p_pOutputObject );
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

	if( nullptr != m_pRemoteOutputController )
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

HRESULT SVIOController::SetRemoteMonitorListProductFilter(const SVString& rListName, SvSml::SVProductFilterEnum Filter)
{
	return m_RemoteMonitorListController.SetRemoteMonitorListProductFilter( rListName, Filter );
}

HRESULT SVIOController::GetRemoteMonitorListProductFilter(const SVString& rListName, SvSml::SVProductFilterEnum& rFilter) const
{
	return m_RemoteMonitorListController.GetRemoteMonitorListProductFilter( rListName, rFilter );
}

