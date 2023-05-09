//******************************************************************************
//* COPYRIGHT (c) 2003 by Körber Pharma Inspection GmbH. All Rights Reserved
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
				m_pModuleReady->setValueObject(pObject);
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
