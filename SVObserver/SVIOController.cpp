//******************************************************************************
//* COPYRIGHT (c) 2003 by K�rber Pharma Inspection GmbH. All Rights Reserved
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
#include "SVConfigurationObject.h"
#include "SVInfoStructs.h"
#include "SVIODoc.h"
#include "ObjectInterfaces/IObjectWriter.h"
#include "SVIOLibrary/SVOutputObjectList.h"
#include "SVObjectLibrary/SVObjectManagerClass.h"
#include "SVXMLLibrary/SVConfigurationTags.h"
#include "SVXMLLibrary/SVNavigateTree.h"
#include "SVOLibrary/SVHardwareManifest.h"
#pragma endregion Includes

///For this class it is not necessary to call SV_IMPLEMENT_CLASS as it is a base class and only derived classes are instantiated.
//SV_IMPLEMENT_CLASS( SVIOController, SVIOControllerId );

SVIOController::SVIOController( LPCTSTR ObjectName ) : SVObjectClass( ObjectName )
{
	LocalIntialize();
}

SVIOController::SVIOController( SVObjectClass *pOwner, int StringResourceID ) : SVObjectClass( pOwner, StringResourceID )
{
	LocalIntialize();
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

void SVIOController::SetIODoc(SVIODoc* pDoc)
{
	m_pIODoc = pDoc;
}

SVIODoc* SVIOController::GetIODoc() const
{
	return m_pIODoc;
}

void SVIOController::initializeOutputs()
{
	SVConfigurationObject* pConfig = dynamic_cast<SVConfigurationObject*> (GetParent());

	if (nullptr != pConfig && SVHardwareManifest::isDiscreteIOSystem(pConfig->GetProductType()))
	{
		m_discreteIO = true;
		if(nullptr == m_pModuleReady)
		{
			m_pModuleReady = std::make_shared<SVIOEntryHostStruct>();
			m_pModuleReady->m_ObjectType = IO_DIGITAL_OUTPUT;
			SVObjectClass* pObject = SVObjectManagerClass::Instance().GetObject(ObjectIdEnum::ModuleReadyId);
			if (nullptr != pObject)
			{
				m_pModuleReady->setLinkedObject(pObject);
			}
		}

	}
	else
	{
		if (nullptr != m_pModuleReady)
		{
			m_pModuleReady->clear();
			m_pModuleReady.reset();
		}
	}
}

bool SVIOController::SetParameters( SVTreeType& rTree, SVTreeType::SVBranchHandle htiParent )
{
	bool result {false};

	_variant_t svVariant;

	SVTreeType::SVBranchHandle htiIODoc = nullptr;
	BOOL bTmp = SvXml::SVNavigateTree::GetItemBranch( rTree, SvXml::CTAG_IODOC, htiParent, htiIODoc );
	if( bTmp )
	{
		result = SvXml::SVNavigateTree::GetItem( rTree, SvXml::CTAG_UNIQUE_REFERENCE_ID, htiIODoc, svVariant );

		if( nullptr != m_pRemoteOutputController )
		{
			m_pRemoteOutputController.reset();
		}

		m_pRemoteOutputController = std::make_unique<SVRemoteOutputDataController>();

		result &= ( nullptr != m_pRemoteOutputController );

		if( nullptr != m_pRemoteOutputController )
		{
			// Set Remote Output Data
			result &= m_pRemoteOutputController->SetParameters( rTree, htiIODoc );
		}
	}
	return result;
}

// Sets up tree - Gets Data from SVIODoc
// Creates Tree items branches so data will be ready to save...
bool SVIOController::GetParameters(SvOi::IObjectWriter& rWriter) const
{
	bool bOk = true;

	if( nullptr != m_pRemoteOutputController )
	{
		// Remote Outputs
		bOk &= m_pRemoteOutputController->GetParameters( rWriter );
	}
	return bOk;
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

	if(m_discreteIO)
	{
		if ( nullptr == m_pModuleReady || nullptr == m_pModuleReady->getObject() || !m_pModuleReady->getObject()->IsCreated() )
		{
			Result = false;
			if (nullptr != pErrorMessages)
			{
				SvStl::MessageContainer Msg( SVMSG_SVO_92_GENERAL_ERROR, SvStl::Tid_InvalidModuleReadyPointer, SvStl::SourceFileParams(StdMessageParams), getObjectId() );
				pErrorMessages->push_back(Msg);
		}
		}
		else
		{
			Result = m_pModuleReady->getObject()->ResetObject(pErrorMessages);
		}
	}

	m_RemoteMonitorListController.ResetObject();

	return Result;
}

HRESULT SVIOController::SetModuleReady( bool Value )
{
	HRESULT result{S_OK};

	SVOutputObjectList* pOutputList(nullptr);
	SVConfigurationObject* pConfig(nullptr);
	SVObjectManagerClass::Instance().GetConfigurationObject(pConfig);
	if (nullptr != pConfig) { pOutputList = pConfig->GetOutputObjectList(); }

	SvOi::IValueObject* pValueObject = dynamic_cast<SvOi::IValueObject*> (SVObjectManagerClass::Instance().GetObject(ObjectIdEnum::ModuleReadyId));
	if (nullptr != pValueObject)
	{
		pValueObject->setValue(_variant_t(Value));
	}

	// Don't set Module Ready if it isn't in the output list
	if (nullptr != m_pModuleReady && SvDef::InvalidObjectId != m_pModuleReady->m_IOId)
	{
		if(nullptr != pOutputList)
		{
			result = pOutputList->WriteOutput(m_pModuleReady, true, false) ? S_OK : E_FAIL;
		}
		else
		{
			result = E_FAIL;
		}
	}
	if(S_OK == result)
	{
		pOutputList->setModuleReady(Value);
	}

	return result;
}

SVIOEntryHostStructPtr SVIOController::GetModuleReady()
{
	return m_pModuleReady;
}

uint32_t SVIOController::GetRemoteOutputController() const
{
	uint32_t l_ObjectId = SvDef::InvalidObjectId;

	if( nullptr != m_pRemoteOutputController )
	{
		l_ObjectId = m_pRemoteOutputController->getObjectId();
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
		m_pRemoteOutputController = std::make_unique<SVRemoteOutputDataController>();
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

HRESULT SVIOController::GetRemoteOutputGroupNames( SvDef::StringVector& rPPQs )
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

SVRemoteOutputGroup* SVIOController::GetRemoteOutputGroup( const std::string& rRemoteGroupID ) const
{
	SVRemoteOutputGroup* l_pObject = nullptr;

	if( nullptr != m_pRemoteOutputController )
	{
		l_pObject = m_pRemoteOutputController->GetControlPar( rRemoteGroupID );
	}

	return l_pObject;
}

size_t SVIOController::GetRemoteOutputGroupItemCount( const std::string& rRemoteGroupID ) const
{
	size_t l_Count = 0;

	if( nullptr != m_pRemoteOutputController )
	{
		l_Count = m_pRemoteOutputController->GetItemCount( rRemoteGroupID );
	}

	return l_Count;
}

HRESULT SVIOController::GetRemoteOutputItem( const std::string& rRemoteGroupId, long l_lIndex, SVRemoteOutputObject*& p_rItem ) const
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

SVRemoteOutputObject* SVIOController::GetFirstRemoteOutputObject( const std::string& rRemoteGroupId )
{
	SVRemoteOutputObject* l_pObject = nullptr;

	if( nullptr != m_pRemoteOutputController )
	{
		l_pObject = m_pRemoteOutputController->GetFirstObject( rRemoteGroupId );
	}

	return l_pObject;
}

HRESULT SVIOController::AddRemoteOutputItem(const std::string& rRemoteGroupId, SVRemoteOutputObject*& p_pNewOutput, uint32_t p_InputObjectID, const std::string& rPPQ)
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

HRESULT SVIOController::DeleteRemoteOutput( const std::string& rRemoteGroupId )
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

HRESULT SVIOController::DeleteRemoteOutputEntry( const std::string& rRemoteGroupId, SVRemoteOutputObject* p_pOutputObject )
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

const RemoteMonitorListMap& SVIOController::GetRemoteMonitorList() const
{
	return m_RemoteMonitorListController.GetRemoteMonitorList();
}

void SVIOController::SetRemoteMonitorList(const RemoteMonitorListMap& rList)
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

HRESULT SVIOController::ActivateRemoteMonitorList(const std::string& listName, bool bActivate)
{
	return m_RemoteMonitorListController.ActivateRemoteMonitorList(listName, bActivate);
}

int SVIOController::GetActiveMonitorListCount() const
{
	return m_RemoteMonitorListController.GetActiveMonitorListCount();
}

void SVIOController::BuildPPQMonitorList(PPQMonitorList& ppqMonitorList) const
{
	m_RemoteMonitorListController.BuildPPQMonitorList(ppqMonitorList);
}

HRESULT SVIOController::SetRemoteMonitorListProductFilter(const std::string& rListName, SvSml::SVProductFilterEnum Filter)
{
	return m_RemoteMonitorListController.SetRemoteMonitorListProductFilter( rListName, Filter );
}

HRESULT SVIOController::GetRemoteMonitorListProductFilter(const std::string& rListName, SvSml::SVProductFilterEnum& rFilter) const
{
	return m_RemoteMonitorListController.GetRemoteMonitorListProductFilter( rListName, rFilter );
}

void SVIOController::LocalIntialize()
{
	SVObjectManagerClass::Instance().ChangeUniqueObjectID(this, ObjectIdEnum::IOControllerId);

	// Set up your type...
	m_ObjectTypeInfo.m_ObjectType = SvPb::SVIOControllerType;

	initializeOutputs();
}

void SVIOController::LocalDestroy()
{
	m_RemoteMonitorListController.Clear();

	m_pModuleReady.reset();
}
