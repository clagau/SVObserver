//******************************************************************************
//* COPYRIGHT (c) 2014 by Seidenader
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : RootObject
//* .File Name       : $Workfile:   RootObject.cpp  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.2  $
//* .Check In Date   : $Date:   07 Jan 2015 17:37:28  $
//******************************************************************************

#pragma region Includes
#include "stdafx.h"
#include "RootObject.h"

#include "SVObserver.h"
#include "SVIODoc.h"
#include "SVObjectLibrary\SVObjectManagerClass.h"
#include "SVObjectLibrary\SVObjectLibrary.h"
#include "Definitions/GlobalConst.h"
#include "SVMessage\SVMessage.h"
#include "SVStatusLibrary\MessageManager.h"
#include "ObjectInterfaces\IRootObject.h"
#include "SVStatusLibrary/ErrorNumbers.h"
#include "SVContainerLibrary/SelectorItem.h"
#include "ExtrasEngine.h"
#pragma endregion Includes

#pragma region Declarations
#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

SV_IMPLEMENT_CLASS( RootObject, RootObjectGuid );
#pragma endregion Declarations

#pragma region Constructor
RootObject::RootObject( LPCSTR ObjectName )
: SVObjectClass( ObjectName )
, m_pConfigurationObject(nullptr)
{
	Initialize();
}

RootObject::RootObject( SVObjectClass* POwner, int StringResourceID )
: SVObjectClass( POwner, StringResourceID )
, m_pConfigurationObject(nullptr)
{
	Initialize();
}

RootObject::~RootObject()
{
}
#pragma endregion Constructor

#pragma region Public Methods
HRESULT RootObject::GetChildObject( SVObjectClass*& rpObject, const SVObjectNameInfo& rNameInfo, long Index ) const
{
	HRESULT l_Status = S_OK;

	l_Status =  m_RootChildren.GetChildObject( rpObject, rNameInfo, Index );

	return l_Status;
}

HRESULT RootObject::RefreshObject( const SVObjectClass* const pSender, RefreshObjectType Type )
{
	HRESULT Result = S_OK;

	if (!m_Initialize && PostRefresh == Type)
	{
		//When it is of type Global Constant we need to update the IO view
		if (SvPb::SVGlobalConstantObjectType == pSender->GetObjectInfo().m_ObjectTypeInfo.SubType)
		{
			SVIODoc* pIODoc = TheSVObserverApp.GetIODoc();
			if(nullptr != pIODoc )
			{
				pIODoc->updateGlobalConstantsView();
			}
		}

		//When certain objects are changed need to do post processing
		if (EnvironmentDiskProtectionUidGuid == pSender->GetUniqueObjectID())
		{
			ExtrasEngine::Instance().ChangeFbwfState();
		}
		else if (EnvironmentStartLastConfigUidGuid == pSender->GetUniqueObjectID())
		{
			//Save Start Last Configuration in registry
			double Value {0.0};
			pSender->getValue(Value);
			TheSVObserverApp.WriteProfileInt(_T("Settings"), _T("Run Last Configuration Automatically"), static_cast<int> (Value));
		}
	}

	return Result;
}

bool RootObject::createConfigurationObject()
{
	m_pConfigurationObject.reset();
	m_pConfigurationObject = SVConfigurationObjectPtr{ new SVConfigurationObject };
	m_pConfigurationObject->SetObjectOwner(this);
	SVObjectManagerClass::Instance().setRootChildID( SvDef::FqnConfiguration, m_pConfigurationObject->GetUniqueObjectID() );

	BasicValueObjectPtr pValueObject( nullptr );
	//Default update views is true
	bool Update( true );
	pValueObject = m_RootChildren.setValue( SvDef::FqnEnvironmentImageUpdate, Update );
	if(nullptr != pValueObject)
	{
		//Need to set the attributes to settable remotely and online but should not be selectable
		pValueObject->SetObjectAttributesAllowed( SvDef::SV_REMOTELY_SETABLE | SvDef::SV_SETABLE_ONLINE, SvOi::SetAttributeType::AddAttribute );
		pValueObject->SetObjectAttributesAllowed( SvDef::SV_SELECTABLE_ATTRIBUTES, SvOi::SetAttributeType::RemoveAttribute );
	}

	pValueObject = m_RootChildren.setValue( SvDef::FqnEnvironmentResultUpdate, Update );
	if(nullptr != pValueObject)
	{
		//Need to set the attributes to settable remotely and online but should not be selectable
		pValueObject->SetObjectAttributesAllowed( SvDef::SV_REMOTELY_SETABLE | SvDef::SV_SETABLE_ONLINE, SvOi::SetAttributeType::AddAttribute );
		pValueObject->SetObjectAttributesAllowed( SvDef::SV_SELECTABLE_ATTRIBUTES, SvOi::SetAttributeType::RemoveAttribute );
	}

	return true;
}

void RootObject::destroyConfigurationObject()
{
	if(nullptr != m_pConfigurationObject)
	{
		m_pConfigurationObject.reset();
	}
}

/*static*/ BasicValueObjectPtr RootObject::getRootChildObjectValue( LPCTSTR DottedName )
{
	RootObject* pRoot( nullptr );
	BasicValueObjectPtr pValue( nullptr );

	SVObjectManagerClass::Instance().GetRootChildObject( pRoot, SvDef::FqnRoot );

	if(nullptr != pRoot)
	{
		pValue = pRoot->getRootChildren().getValueObject( DottedName );
	}

	return pValue;
}

/*static*/ void RootObject::getRootChildObjectList( BasicValueObjects::ValueVector& rObjectList, LPCTSTR Path, UINT AttributesAllowedFilter )
{
	RootObject* pRoot( nullptr );

	SVObjectManagerClass::Instance().GetRootChildObject( pRoot, SvDef::FqnRoot );

	if(nullptr != pRoot)
	{
		pRoot->getRootChildren().getObjectList( rObjectList, Path, AttributesAllowedFilter );
	}
}

/*static*/ void RootObject::getRootChildNameList( SvDef::StringVector& rObjectNameList, LPCTSTR Path, UINT AttributesAllowedFilter )
{
	BasicValueObjects::ValueVector ObjectList;

	getRootChildObjectList( ObjectList, Path, AttributesAllowedFilter );
	for( BasicValueObjects::ValueVector::const_iterator Iter = ObjectList.cbegin(); Iter != ObjectList.cend(); ++Iter )
	{
		rObjectNameList.push_back( std::string((*Iter)->GetCompleteName()) );
	}
}

/*static*/ HRESULT RootObject::deleteRootChildValue( LPCTSTR DottedName )
{
	HRESULT Result( S_FALSE );
	RootObject* pRoot( nullptr );

	SVObjectManagerClass::Instance().GetRootChildObject( pRoot, SvDef::FqnRoot );

	if(nullptr != pRoot)
	{
		Result = pRoot->getRootChildren().deleteValue( DottedName );
	}

	return Result;
}

/*static*/ HRESULT RootObject::resetRootChildValue( LPCTSTR Name )
{
	HRESULT Result( S_FALSE );
	RootObject* pRoot( nullptr );

	SVObjectManagerClass::Instance().GetRootChildObject( pRoot, SvDef::FqnRoot );

	if(nullptr != pRoot)
	{
		SVObjectClass *pRootChild( nullptr );
		SVObjectManagerClass::Instance().GetRootChildObject( pRootChild, Name );
		if( nullptr != pRootChild )
		{
			SvPb::SVObjectSubTypeEnum ObjectSubType = pRootChild->GetObjectInfo().m_ObjectTypeInfo.SubType;
			Result = pRoot->m_RootChildren.deleteValue( Name );
			if( S_OK == Result )
			{
				Result = pRoot->createRootChild( Name, ObjectSubType );
			}
		}
	}

	return Result;
}
#pragma endregion Public Methods

#pragma region Private Methods
bool RootObject::Initialize()
{
	bool Result(true);

	m_Initialize = true;
	m_outObjectInfo.m_ObjectTypeInfo.ObjectType = SvPb::SVRootObjectType;
	SVObjectManagerClass::Instance().ChangeUniqueObjectID(this, RootUidGuid);
	//The Root object should have an empty name
	SetName(_T(""));

	Result = createRootChildren();
	m_Initialize = false;

	return Result;
}

bool RootObject::createRootChildren()
{
	bool Result( false );

	Result = createRootChild( SvDef::FqnEnvironment, SvPb::SVEnvironmentObjectType );
	if( Result )
	{
		//Default values for the mode
		m_RootChildren.setValue(SvDef::FqnEnvironmentModeValue, 0L);
		m_RootChildren.setValue(SvDef::FqnEnvironmentModeIsRun, false);
		m_RootChildren.setValue(SvDef::FqnEnvironmentModeIsStop, false);
		m_RootChildren.setValue(SvDef::FqnEnvironmentModeIsRegressionTest, false);
		m_RootChildren.setValue(SvDef::FqnEnvironmentModeIsTest, false);
		m_RootChildren.setValue(SvDef::FqnEnvironmentModeIsEdit, false);

		//When Environment created then create the following variables to set their attributes
		BasicValueObjectPtr pValueObject( nullptr );
		pValueObject = m_RootChildren.setValue( SvDef::FqnEnvironmentModelNumber, _T("") );
		if(nullptr != pValueObject)
		{
			pValueObject->SetObjectAttributesAllowed( SvDef::SV_SELECTABLE_ATTRIBUTES, SvOi::SetAttributeType::RemoveAttribute );
		}
		pValueObject = m_RootChildren.setValue( SvDef::FqnEnvironmentSerialNumber , _T("") );
		if (nullptr != pValueObject)
		{
			pValueObject->SetObjectAttributesAllowed( SvDef::SV_SELECTABLE_ATTRIBUTES, SvOi::SetAttributeType::RemoveAttribute );
		}
		pValueObject = m_RootChildren.setValue( SvDef::FqnEnvironmentWinKey, _T("") );
		if (nullptr != pValueObject)
		{
			pValueObject->SetObjectAttributesAllowed( SvDef::SV_SELECTABLE_ATTRIBUTES, SvOi::SetAttributeType::RemoveAttribute );
		}
		pValueObject = m_RootChildren.setValue(SvDef::FqnEnvironmentAutoSave, true);
		if (nullptr != pValueObject)
		{
			//Need to set the attributes to settable remotely and online but should not be selectable
			pValueObject->SetObjectAttributesAllowed(SvDef::SV_REMOTELY_SETABLE | SvDef::SV_SETABLE_ONLINE, SvOi::SetAttributeType::AddAttribute);
			pValueObject->SetObjectAttributesAllowed(SvDef::SV_SELECTABLE_ATTRIBUTES, SvOi::SetAttributeType::RemoveAttribute);
		}
		bool DiskProtectionEnabled = ExtrasEngine::Instance().ReadCurrentFbwfSettings();
		pValueObject = m_RootChildren.setValue(SvDef::FqnEnvironmentDiskProtection, DiskProtectionEnabled);
		if (nullptr != pValueObject)
		{
			//Need to set the attributes to settable remotely but should not be selectable
			pValueObject->SetObjectAttributesAllowed(SvDef::SV_REMOTELY_SETABLE, SvOi::SetAttributeType::AddAttribute);
			pValueObject->SetObjectAttributesAllowed(SvDef::SV_SELECTABLE_ATTRIBUTES, SvOi::SetAttributeType::RemoveAttribute);
		}
		// Load special profile settings
		bool StartLastConfiguration(false);
		if (TheSVObserverApp.GetProfileInt(_T("Settings"), _T("Run Last Configuration Automatically"), 0))
		{
			StartLastConfiguration = true;
		}
		else
		{
			StartLastConfiguration = false;
		}
		pValueObject = m_RootChildren.setValue(SvDef::FqnEnvironmentStartLastConfig, StartLastConfiguration);
		if (nullptr != pValueObject)
		{
			//Need to set the attributes to settable remotely but should not be selectable
			pValueObject->SetObjectAttributesAllowed(SvDef::SV_REMOTELY_SETABLE, SvOi::SetAttributeType::AddAttribute);
			pValueObject->SetObjectAttributesAllowed(SvDef::SV_SELECTABLE_ATTRIBUTES, SvOi::SetAttributeType::RemoveAttribute);
		}

		Result = createRootChild( SvDef::FqnGlobal, SvPb::SVGlobalConstantObjectType );
	}

	return Result;
}
bool RootObject::createRootChild( LPCTSTR ChildName, SvPb::SVObjectSubTypeEnum ObjectSubType )
{
	bool Result( false );
	_variant_t Node;

	//Setting the variant to VT_EMPTY will cause the value to be a node
	Node.Clear();
	BasicValueObjectPtr pRootChild( nullptr);
	pRootChild = m_RootChildren.setValue( ChildName, Node, this, ObjectSubType );
	if(nullptr != pRootChild)
	{
		SVObjectManagerClass::Instance().setRootChildID( pRootChild->GetName(), pRootChild->GetUniqueObjectID() );
		Result = true;
	}
	else
	{
		SvStl::MessageMgrStd Exception(SvStl::MsgType::Log );
		Exception.setMessage( SVMSG_SVO_67_MAIN_BRANCH_NOT_CREATED, ChildName, SvStl::SourceFileParams(StdMessageParams), SvStl::Err_25017_RootChildCreate, GetUniqueObjectID() );
	}
	
	return Result;
}
#pragma endregion Private Methods

#pragma region IRootObject-function
void SvOi::getRootChildNameList( SvDef::StringVector& rObjectNameList, LPCTSTR Path, UINT AttributesAllowedFilter )
{
	//To have the function available without knowing the class RootObject
	RootObject::getRootChildNameList( rObjectNameList, Path, AttributesAllowedFilter );
}

void SvOi::getRootChildSelectorList(SvCl::SelectorItemInserter Inserter, LPCTSTR Path, UINT AttributesAllowedFilter)
{
	BasicValueObjects::ValueVector ObjectList;
	
	//To have the function available without knowing the class RootObject
	RootObject::getRootChildObjectList( ObjectList, Path, AttributesAllowedFilter );
	for(const auto rpObject : ObjectList)
	{
		SvCl::SelectorItem InsertItem;

		InsertItem.m_Name = rpObject->GetName();
		InsertItem.m_Location = rpObject->GetCompleteName();
		InsertItem.m_ItemKey = rpObject->GetUniqueObjectID();
		InsertItem.m_ItemTypeName = rpObject->getTypeName().c_str();
		Inserter = InsertItem;
	}
}

void SvOi::addRootChildObjects(SVOutputInfoListClass& rList)
{
	BasicValueObjects::ValueVector list;

	RootObject::getRootChildObjectList( list, _T(""), 0 );
	for (auto iter = list.begin(); iter != list.end(); ++iter) 
	{
		rList.Add( &((*iter)->GetObjectOutputInfo()) );
	}
}
#pragma endregion IRootObject-function

