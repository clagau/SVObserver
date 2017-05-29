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
#include "SVStatusLibrary/SVSVIMStateClass.h"
#include "TextDefinesSvO.h"
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
	m_outObjectInfo.m_ObjectTypeInfo.ObjectType = SVIOControllerType;

	m_pModuleReady = new SVIOEntryHostStruct;
	m_pModuleReady->m_ObjectType = IO_DIGITAL_OUTPUT;
	SVBoolValueObjectClass* pValueObject = new SVBoolValueObjectClass;
	m_pModuleReady->setObject(dynamic_cast<SVObjectClass*> (pValueObject));
	//! For Module Ready do not set the parent owner
	pValueObject->SetName(SvO::cModuleReady);
	pValueObject->SetObjectDepth( 100 );
	pValueObject->ResetObject();
	pValueObject->setValue( _variant_t(true) );

	m_pRaidErrorBit = new SVIOEntryHostStruct;
	m_pRaidErrorBit->m_ObjectType = IO_DIGITAL_OUTPUT;
	pValueObject = new SVBoolValueObjectClass;
	m_pRaidErrorBit->setObject(dynamic_cast<SVObjectClass*> (pValueObject));
	//! For Raid Error Indicator do not set the parent owner
	pValueObject->SetName(SvO::cRaidErrorIndicator);
	pValueObject->SetObjectDepth( 100 );
	pValueObject->ResetObject();
	pValueObject->setValue( _variant_t(false) );
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

bool SVIOController::RebuildOutputList()
{
	SVOutputObjectPtrVector ppNewOutputs;

	SVConfigurationObject* pConfig( nullptr );
	SVObjectManagerClass::Instance().GetConfigurationObject( pConfig );

	if( nullptr == pConfig || nullptr == pConfig->GetOutputObjectList( ) )
	{
		return false;
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

	return true;
}

BOOL SVIOController::SetParameters( SVTreeType& rTree, SVTreeType::SVBranchHandle htiParent )
{
	BOOL bOk = FALSE;

	_variant_t svVariant;

	SVTreeType::SVBranchHandle htiIODoc = nullptr;
	BOOL bTmp = SvXml::SVNavigateTree::GetItemBranch( rTree, SvXml::CTAG_IODOC, htiParent, htiIODoc );
	if( bTmp )
	{
		bOk = SvXml::SVNavigateTree::GetItem( rTree, SvXml::CTAG_UNIQUE_REFERENCE_ID, htiIODoc, svVariant );
		if ( bOk )
		{
			SVGUID ObjectID( svVariant );

			SVObjectManagerClass::Instance().CloseUniqueObjectID( this );

			m_outObjectInfo.m_UniqueObjectID = ObjectID;

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
bool SVIOController::GetParameters( SvXml::SVObjectXMLWriter& rWriter ) const
{
	bool bOk = true;

	_variant_t svVariant;

	svVariant = SVGUID( m_outObjectInfo.m_UniqueObjectID ).ToVARIANT();
	rWriter.WriteAttribute( SvXml::CTAG_UNIQUE_REFERENCE_ID, svVariant );

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

	l_bOk &= !( m_pModuleReady.empty() ) && m_pModuleReady->getObject()->SetObjectDepth( NewObjectDepth );
	l_bOk &= !( m_pRaidErrorBit.empty() ) && m_pRaidErrorBit->getObject()->SetObjectDepth( NewObjectDepth );

	return l_bOk;
}

BOOL SVIOController::SetObjectDepthWithIndex( int NewObjectDepth, int NewLastSetIndex )
{
	BOOL l_bOk = SVObjectClass::SetObjectDepthWithIndex( NewObjectDepth, NewLastSetIndex );

	l_bOk &= !( m_pModuleReady.empty() ) && m_pModuleReady->getObject()->SetObjectDepthWithIndex( NewObjectDepth, NewLastSetIndex );
	l_bOk &= !( m_pRaidErrorBit.empty() ) && m_pRaidErrorBit->getObject()->SetObjectDepthWithIndex( NewObjectDepth, NewLastSetIndex );

	return l_bOk;
}

BOOL SVIOController::SetImageDepth( long lDepth )
{
	BOOL l_bOk = SVObjectClass::SetImageDepth( lDepth );

	l_bOk &= !( m_pModuleReady.empty() ) && m_pModuleReady->getObject()->SetImageDepth( lDepth );
	l_bOk &= !( m_pRaidErrorBit.empty() ) && m_pRaidErrorBit->getObject()->SetImageDepth( lDepth );

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

bool SVIOController::ResetObject(SvStl::MessageContainerVector *pErrorMessages)
{
	bool Result = __super::ResetObject(pErrorMessages);

	if ( nullptr == m_pModuleReady || nullptr == m_pModuleReady->getObject() || !m_pModuleReady->getObject()->IsCreated() )
	{
		Result = false;
		if (nullptr != pErrorMessages)
		{
			SvStl::MessageContainer Msg( SVMSG_SVO_92_GENERAL_ERROR, SvStl::Tid_InvalidModuleReadyPointer, SvStl::SourceFileParams(StdMessageParams), 0, GetUniqueObjectID() );
			pErrorMessages->push_back(Msg);
	}
	}
	else
	{
		Result = m_pModuleReady->getObject()->ResetObject(pErrorMessages);
	}
	if ( nullptr == m_pRaidErrorBit || nullptr == m_pRaidErrorBit->getObject() || !m_pRaidErrorBit->getObject()->IsCreated() )
	{
		Result = false;
		if (nullptr != pErrorMessages)
		{
			SvStl::MessageContainer Msg( SVMSG_SVO_92_GENERAL_ERROR, SvStl::Tid_InvalidRaidBitPointer, SvStl::SourceFileParams(StdMessageParams), 0, GetUniqueObjectID() );
			pErrorMessages->push_back(Msg);
		}
	}
	else
	{
		Result = m_pRaidErrorBit->getObject()->ResetObject(pErrorMessages) && Result;
	}

	m_RemoteMonitorListController.ResetObject();

	if( ! RebuildOutputList() )
	{
		Result = false;
		if (nullptr != pErrorMessages)
	{
			SvStl::MessageContainer Msg( SVMSG_SVO_92_GENERAL_ERROR, SvStl::Tid_IoController_RebuildOutpuListFailed, SvStl::SourceFileParams(StdMessageParams), 0, GetUniqueObjectID() );
			pErrorMessages->push_back(Msg);
	}
	}
	return Result;
}

HRESULT SVIOController::SetModuleReady( bool Value )
{
	HRESULT l_Status( S_OK );

	// Don't set Module Ready if it isn't in the output list
	if( !( m_pModuleReady->m_IOId.empty() ) )
	{
		SVOutputObjectList* pOutputList( nullptr );
		if(nullptr != m_pModuleReady->getValueObject())
		{
			m_pModuleReady->getValueObject()->setValue( _variant_t(Value) );
		}

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

bool SVIOController::SetRaidErrorBit( bool Value )
{
	bool Result( true );

	// Don't set Module Ready if it isn't in the output list
	if( !( m_pRaidErrorBit->m_IOId.empty() ) )
	{
		SVOutputObjectList *pOutputList( nullptr );
		if(nullptr != m_pRaidErrorBit->getValueObject())
		{
			m_pRaidErrorBit->getValueObject()->setValue( _variant_t(Value) );
		}

		SVConfigurationObject* pConfig( nullptr );
		SVObjectManagerClass::Instance().GetConfigurationObject( pConfig );

		if( nullptr != pConfig ){ pOutputList = pConfig->GetOutputObjectList( ); }

		if( nullptr == pOutputList || !pOutputList->WriteOutput( m_pRaidErrorBit, 1, true, false ) )
		{
			Result = false; // JMS ERROR - Cannot write to module ready output.
		}
	}// end if

	return Result;
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

int SVIOController::GetActiveMonitorListCount() const
{
	return m_RemoteMonitorListController.GetActiveMonitorListCount();
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

