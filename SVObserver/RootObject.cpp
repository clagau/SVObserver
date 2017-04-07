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
#include "SVObjectLibrary\GlobalConst.h"
#include "SVMessage\SVMessage.h"
#include "SVStatusLibrary\MessageManager.h"
#include "ObjectInterfaces\IRootObject.h"
#include "ObjectInterfaces\ErrorNumbers.h"
#include "ObjectSelectorLibrary/SelectorItemVector.h"
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

	if( PostRefresh == Type )
	{
		//When its of type Global Constant we need to update the IO view
		if( SVGlobalConstantObjectType == pSender->GetObjectInfo().m_ObjectTypeInfo.ObjectType )
		{
			SVIODoc* pIODoc = TheSVObserverApp.GetIODoc();
			if(nullptr != pIODoc )
			{
				pIODoc->updateGlobalConstantsView();
			}
		}
	}

	return Result;
}

bool RootObject::createConfigurationObject()
{
	m_pConfigurationObject.clear();
	m_pConfigurationObject = new SVConfigurationObject;
	m_pConfigurationObject->SetObjectOwner(this);
	SVObjectManagerClass::Instance().setRootChildID( SvOl::FqnConfiguration, m_pConfigurationObject->GetUniqueObjectID() );

	BasicValueObjectPtr pValueObject( nullptr );
	//Default update views is true
	bool Update( true );
	pValueObject = m_RootChildren.setValue( SvOl::FqnEnvironmentImageUpdate, Update );
	if( !pValueObject.empty() )
	{
		//Need to set the attributes to settable remotely and online for the Image Update object but should not be selectable
		pValueObject->SetObjectAttributesAllowed( SV_REMOTELY_SETABLE | SV_SETABLE_ONLINE, SvOi::SetAttributeType::AddAttribute );
		pValueObject->SetObjectAttributesAllowed( SV_SELECTABLE_ATTRIBUTES, SvOi::SetAttributeType::RemoveAttribute );
	}

	pValueObject = m_RootChildren.setValue( SvOl::FqnEnvironmentResultUpdate, Update );
	if( !pValueObject.empty() )
	{
		//Need to set the attributes to settable remotely and online for the Result Update object but should not be selectable
		pValueObject->SetObjectAttributesAllowed( SV_REMOTELY_SETABLE | SV_SETABLE_ONLINE, SvOi::SetAttributeType::AddAttribute );
		pValueObject->SetObjectAttributesAllowed( SV_SELECTABLE_ATTRIBUTES, SvOi::SetAttributeType::RemoveAttribute );
	}

	return true;
}

void RootObject::destroyConfigurationObject()
{
	if( !m_pConfigurationObject.empty() )
	{
		m_pConfigurationObject.clear();
	}
}

/*static*/ BasicValueObjectPtr RootObject::getRootChildObjectValue( LPCTSTR DottedName )
{
	RootObject* pRoot( nullptr );
	BasicValueObjectPtr pValue( nullptr );

	SVObjectManagerClass::Instance().GetRootChildObject( pRoot, SvOl::FqnRoot );

	if(nullptr != pRoot)
	{
		pValue = pRoot->getRootChildren().getValueObject( DottedName );
	}

	return pValue;
}

/*static*/ void RootObject::getRootChildObjectList( BasicValueObjects::ValueVector& rObjectList, LPCTSTR Path, UINT AttributesAllowedFilter )
{
	RootObject* pRoot( nullptr );

	SVObjectManagerClass::Instance().GetRootChildObject( pRoot, SvOl::FqnRoot );

	if(nullptr != pRoot)
	{
		pRoot->getRootChildren().getObjectList( rObjectList, Path, AttributesAllowedFilter );
	}
}

/*static*/ void RootObject::getRootChildNameList( SVStringVector& rObjectNameList, LPCTSTR Path, UINT AttributesAllowedFilter )
{
	BasicValueObjects::ValueVector ObjectList;

	getRootChildObjectList( ObjectList, Path, AttributesAllowedFilter );
	for( BasicValueObjects::ValueVector::const_iterator Iter = ObjectList.cbegin(); Iter != ObjectList.cend(); ++Iter )
	{
		rObjectNameList.push_back( SVString((*Iter)->GetCompleteName()) );
	}
}

/*static*/ HRESULT RootObject::deleteRootChildValue( LPCTSTR DottedName )
{
	HRESULT Result( S_FALSE );
	RootObject* pRoot( nullptr );

	SVObjectManagerClass::Instance().GetRootChildObject( pRoot, SvOl::FqnRoot );

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

	SVObjectManagerClass::Instance().GetRootChildObject( pRoot, SvOl::FqnRoot );

	if(nullptr != pRoot)
	{
		SVObjectClass *pRootChild( nullptr );
		SVObjectManagerClass::Instance().GetRootChildObject( pRootChild, Name );
		if( nullptr != pRootChild )
		{
			SVObjectTypeEnum ObjectType = pRootChild->GetObjectInfo().m_ObjectTypeInfo.ObjectType;
			Result = pRoot->m_RootChildren.deleteValue( Name );
			if( S_OK == Result )
			{
				Result = pRoot->createRootChild( Name, ObjectType );
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

	m_outObjectInfo.m_ObjectTypeInfo.ObjectType =  SVRootObjectType;

	//The Root object should have an empty name
	SetName(_T(""));

	Result = createRootChildren();

	return Result;
}

bool RootObject::createRootChildren()
{
	bool Result( false );

	Result = createRootChild( SvOl::FqnEnvironment, SVEnvironmentObjectType );
	if( Result )
	{
		//When Environment created then create the following variables to set their attributes
		BasicValueObjectPtr pValueObject( nullptr );
		pValueObject = m_RootChildren.setValue( SvOl::FqnEnvironmentModelNumber, _T("") );
		if( !pValueObject.empty() )
		{
			pValueObject->SetObjectAttributesAllowed( SV_SELECTABLE_ATTRIBUTES, SvOi::SetAttributeType::RemoveAttribute );
		}
		pValueObject = m_RootChildren.setValue( SvOl::FqnEnvironmentSerialNumber , _T("") );
		if( !pValueObject.empty() )
		{
			pValueObject->SetObjectAttributesAllowed( SV_SELECTABLE_ATTRIBUTES, SvOi::SetAttributeType::RemoveAttribute );
		}
		pValueObject = m_RootChildren.setValue( SvOl::FqnEnvironmentWinKey, _T("") );
		if( !pValueObject.empty() )
		{
			pValueObject->SetObjectAttributesAllowed( SV_SELECTABLE_ATTRIBUTES, SvOi::SetAttributeType::RemoveAttribute );
		}

		Result = createRootChild( SvOl::FqnGlobal, SVGlobalConstantObjectType );
	}

	return Result;
}
bool RootObject::createRootChild( LPCTSTR ChildName, SVObjectTypeEnum ObjectType ) 
{
	bool Result( false );
	_variant_t Node;

	//Setting the variant to VT_EMPTY will cause the value to be a node
	Node.Clear();
	BasicValueObjectPtr pRootChild( nullptr);
	pRootChild = m_RootChildren.setValue( ChildName, Node, this, ObjectType );
	if( !pRootChild.empty() )
	{
		SVObjectManagerClass::Instance().setRootChildID( pRootChild->GetName(), pRootChild->GetUniqueObjectID() );
		Result = true;
	}
	else
	{
		SvStl::MessageMgrStd Exception( SvStl::LogOnly );
		Exception.setMessage( SVMSG_SVO_67_MAIN_BRANCH_NOT_CREATED, ChildName, SvStl::SourceFileParams(StdMessageParams), SvOi::Err_25017_RootChildCreate, GetUniqueObjectID() );
	}
	
	return Result;
}
#pragma endregion Private Methods

#pragma region IRootObject-function
void SvOi::getRootChildNameList( SVStringVector& rObjectNameList, LPCTSTR Path, UINT AttributesAllowedFilter )
{
	//To have the function available without knowing the class RootObject
	RootObject::getRootChildNameList( rObjectNameList, Path, AttributesAllowedFilter );
}

SvOi::ISelectorItemVectorPtr SvOi::getRootChildSelectorList( LPCTSTR Path, UINT AttributesAllowedFilter )
{
	SvOsl::SelectorItemVector *pSelectorList = new SvOsl::SelectorItemVector();
	SvOi::ISelectorItemVectorPtr Result = static_cast<SvOi::ISelectorItemVector*> (pSelectorList);
	BasicValueObjects::ValueVector ObjectList;
	
	//To have the function available without knowing the class RootObject
	RootObject::getRootChildObjectList( ObjectList, Path, AttributesAllowedFilter );
	BasicValueObjects::ValueVector::const_iterator Iter;
	for( Iter = ObjectList.begin(); ObjectList.end() != Iter; ++Iter )
	{
		SvOsl::SelectorItem InsertItem;

		InsertItem.setName( (*Iter)->GetName() );
		InsertItem.setLocation( (*Iter)->GetCompleteName().c_str() );
		InsertItem.setItemKey( (*Iter)->GetUniqueObjectID().ToVARIANT() );
		InsertItem.setItemTypeName( (*Iter)->getTypeName().c_str() );

		pSelectorList->push_back( InsertItem );

	}

	return Result;
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

