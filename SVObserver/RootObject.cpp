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
#include "SVStatusLibrary\MessageManagerResource.h"
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
		if( SVGlobalConstantObjectType == pSender->GetObjectInfo().ObjectTypeInfo.ObjectType )
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
	pValueObject = m_RootChildren.setValue( ::EnvironmentImageUpdate, Update );
	if( !pValueObject.empty() )
	{
		//Need to set the attributes to settable remotely and online for the Image Update object but should not appear in rest of GUI
		pValueObject->ObjectAttributesAllowedRef() |= SV_REMOTELY_SETABLE | SV_SETABLE_ONLINE;
		pValueObject->ObjectAttributesAllowedRef() &= ~SV_VIEWABLE & ~SV_SELECTABLE_FOR_EQUATION;
	}

	pValueObject = m_RootChildren.setValue( ::EnvironmentResultUpdate, Update );
	if( !pValueObject.empty() )
	{
		//Need to set the attributes to settable remotely and online for the Result Update object but should not appear in rest of GUI
		pValueObject->ObjectAttributesAllowedRef() |= SV_REMOTELY_SETABLE | SV_SETABLE_ONLINE;
		pValueObject->ObjectAttributesAllowedRef() &= ~SV_VIEWABLE & ~SV_SELECTABLE_FOR_EQUATION;
	}

	return true;
}

/*static*/ BasicValueObjectPtr RootObject::getRootChildObjectValue( LPCSTR DottedName )
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

/*static*/ void RootObject::getRootChildNameList( SVStringArray& rObjectNameList, LPCTSTR Path, UINT AttributesAllowedFilter )
{
	BasicValueObjects::ValueVector ObjectList;

	getRootChildObjectList( ObjectList, Path, AttributesAllowedFilter );
	for( BasicValueObjects::ValueVector::const_iterator Iter = ObjectList.cbegin(); Iter != ObjectList.cend(); ++Iter )
	{
		rObjectNameList.push_back( (*Iter)->GetCompleteObjectName() );
	}
}

/*static*/ HRESULT RootObject::deleteRootChildValue( LPCSTR DottedName )
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

/*static*/ HRESULT RootObject::resetRootChildValue( LPCSTR Name )
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
			SVObjectTypeEnum ObjectType = pRootChild->GetObjectInfo().ObjectTypeInfo.ObjectType;
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

	outObjectInfo.ObjectTypeInfo.ObjectType =  SVRootObjectType;

	//The Root object should have an empty name
	SetName(_T(""));

	//@WARNING [gra][7.20][02.07.2015] This should be removed that the Configuration Object is only created when needed
	Result = createConfigurationObject();
	if( Result )
	{
		Result = createRootChildren();
	}

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
		pValueObject = m_RootChildren.setValue( ::EnvironmentModelNumber, _T("") );
		if( !pValueObject.empty() )
		{
			pValueObject->ObjectAttributesAllowedRef() &=~SV_VIEWABLE & ~SV_SELECTABLE_FOR_EQUATION;
		}
		pValueObject = m_RootChildren.setValue( ::EnvironmentSerialNumber , _T("") );
		if( !pValueObject.empty() )
		{
			pValueObject->ObjectAttributesAllowedRef() &=~SV_VIEWABLE & ~SV_SELECTABLE_FOR_EQUATION;
		}
		pValueObject = m_RootChildren.setValue( ::EnvironmentWinKey, _T("") );
		if( !pValueObject.empty() )
		{
			pValueObject->ObjectAttributesAllowedRef() &=~SV_VIEWABLE & ~SV_SELECTABLE_FOR_EQUATION;
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
		SvStl::MessageMgrDisplayAndNotify Exception( SvStl::LogAndDisplay );
		Exception.setMessage( SVMSG_SVO_67_MAIN_BRANCH_NOT_CREATED, ChildName, StdMessageParams, SvOi::Err_25017_RootChildCreate );
	}
	
	return Result;
}
#pragma endregion Private Methods

#pragma region IRootObject-function
void SvOi::getRootChildNameList( SVStringArray& rObjectNameList, LPCTSTR Path, UINT AttributesAllowedFilter )
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
		InsertItem.setLocation( (*Iter)->GetCompleteObjectName() );
		InsertItem.setItemKey( (*Iter)->GetUniqueObjectID().ToVARIANT() );
		InsertItem.setItemTypeName( (*Iter)->getTypeName().c_str() );

		pSelectorList->push_back( InsertItem );

	}

	return Result;
}
#pragma endregion IRootObject-function

//******************************************************************************
//* LOG HISTORY:
//******************************************************************************
/*
$Log:   N:\PVCSarch65\ProjectFiles\archives\SVObserver_SRC\SVObserver\RootObject.cpp_v  $
 * 
 *    Rev 1.2   07 Jan 2015 17:37:28   bwalter
 * Project:  SVObserver
 * Change Request (SCR) nbr:  980
 * SCR Title:  Add Non-Inspection Objects to the Result View
 * Checked in by:  mEichengruen;  Marcus Eichengruen
 * Change Description:  
 *   Changed constructors to set the name of the RootObject to empty.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.1   17 Jul 2014 17:56:30   gramseier
 * Project:  SVObserver
 * Change Request (SCR) nbr:  909
 * SCR Title:  Object Selector replacing Result Picker and Output Selector SVO-72, 40, 130
 * Checked in by:  gRamseier;  Guido Ramseier
 * Change Description:  
 *   Removed namespaces and code review changes
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.0   17 Mar 2014 15:10:24   bwalter
 * Project:  SVObserver
 * Change Request (SCR) nbr:  869
 * SCR Title:  Add PPQ and Environment Variables to Object Manager and Update Pickers
 * Checked in by:  bWalter;  Ben Walter
 * Change Description:  
 *   Initial check in.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
*/