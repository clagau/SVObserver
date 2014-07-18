//******************************************************************************
//* COPYRIGHT (c) 2014 by Seidenader
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : ObjectTreeGenerator
//* .File Name       : $Workfile:   ObjectTreeGenerator.cpp  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.0  $
//* .Check In Date   : $Date:   17 Jul 2014 11:16:26  $
//******************************************************************************

#pragma region Includes
#include "stdafx.h"
#include "ObjectTreeGenerator.h"

#include "SVObjectLibrary\SVObjectManagerClass.h"
#include "SVTreeLibrary\ObjectSelectorItem.h"
#include "SVSystemLibrary\LoadDll.h"
#include "ResizablePropertySheet.h"
#include "ObjectSelectorPpg.h"
#pragma endregion Includes

#pragma region Declarations
using namespace Seidenader::ObjectSelectorLibrary;
using namespace Seidenader::SVTreeLibrary;
using namespace Seidenader::SVSystemLibrary;

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif
#pragma endregion Declarations

#pragma region Constructor
ObjectTreeGenerator::ObjectTreeGenerator()
	: m_TreeContainer()
	, m_SingleObjectResult()
	, m_LocationInputFilters()
	, m_AttributesSetFilter( 0 )
	, m_AttributesAllowedFilter( 0 )
	, m_AllowWholeArray( false )
	, m_SelectorType( ObjectTreeGenerator::TypeNone )
{
}

ObjectTreeGenerator::~ObjectTreeGenerator()
{
}
#pragma endregion Constructor

#pragma region Public Methods
ObjectTreeGenerator& ObjectTreeGenerator::Instance()
{
	static ObjectTreeGenerator Object;

	return Object;
}

void ObjectTreeGenerator::Clear( bool ClearAll )
{
	if ( ClearAll )
	{
		m_SingleObjectResult.Clear();
	}
	m_TreeContainer.Clear();
	m_LocationInputFilters.clear();
	m_AttributesSetFilter = 0;
	m_AttributesAllowedFilter = 0;
	m_AllowWholeArray = false;
	m_SelectorType = ObjectTreeGenerator::TypeNone;
}

void ObjectTreeGenerator::setAttributeFilters( UINT AttributesSetFilter, UINT AttributesAllowedFilter )
{
	m_AttributesSetFilter = AttributesSetFilter;
	if ( 0 == AttributesAllowedFilter )
	{
		m_AttributesAllowedFilter = m_AttributesSetFilter;
	}
	else
	{
		m_AttributesAllowedFilter = AttributesAllowedFilter;
	}
}

void ObjectTreeGenerator::insertTreeObjects( const SVString& rTreeName )
{
	SVObjectReferenceVector ObjectList;

	SVObjectManagerClass::Instance().getTreeList( rTreeName, ObjectList, m_AttributesAllowedFilter );
	insertTreeObjects( ObjectList );
}

void ObjectTreeGenerator::insertTreeObjects( const SVStringArray& rLocationList )
{
	SVStringArray::const_iterator Iter( rLocationList.begin() );

	while( rLocationList.end() != Iter )
	{
		insertTreeObject( *Iter );
		++Iter;
	}
}

void ObjectTreeGenerator::insertTreeObjects( const SVObjectReferenceVector& rObjectList )
{
	SVObjectReferenceVector::const_iterator Iter( rObjectList.begin() );

	while( rObjectList.end() != Iter )
	{
		insertTreeObject( *Iter );
		++Iter;
	}
}

void ObjectTreeGenerator::insertTreeObjects( const ObjectTreeItems& rTree )
{
	ObjectTreeItems::SVTree_const_pre_order_iterator Iter( rTree.pre_order_begin() );
	
	while( rTree.pre_order_end() != Iter )
	{
		if( Iter->second.isLeaf() )
		{
			//InsertLeaf needs a reference to a non const item
			ObjectSelectorItem SelectorItem( Iter->second );
			m_TreeContainer.insertLeaf( Iter->second.getLocation(), SelectorItem );
		}
		++Iter;
	}
}

void ObjectTreeGenerator::insertTreeObject( const SVObjectReference& rObjectRef )
{
	ObjectSelectorItem SelectorItem;
	IObjectSelectorItem::AttributeEnum Attribute( static_cast<IObjectSelectorItem::AttributeEnum> (IObjectSelectorItem::Leaf | IObjectSelectorItem::Checkable) );

	setSelectorItemType(rObjectRef, SelectorItem);

	SVString Location;
	Location = rObjectRef.GetCompleteOneBasedObjectName();

	//If location is array then place an additional level with the array group name
	if( rObjectRef->IsArray() )
	{
		SVString Name;
		size_t Pos = 0;

		Name = rObjectRef->GetName();
		Name.insert( Pos, _T(".") );
		Pos = Location.rfind(_T('.'));
		if( SVString::npos != Pos )
		{
			Location.insert( Pos, Name.c_str() );
		}
		SelectorItem.setArrayIndex( rObjectRef.ArrayIndex( true ) );
		Attribute = static_cast<IObjectSelectorItem::AttributeEnum> (IObjectSelectorItem::Leaf | IObjectSelectorItem::Checkable | IObjectSelectorItem::Array);
	}

	SelectorItem.setName( rObjectRef->GetName() );
	SelectorItem.setItemKey( rObjectRef->GetUniqueObjectID().ToVARIANT() );
	//Only set the checked states if of type attributes
	if( TypeSetAttributes == (m_SelectorType & TypeSetAttributes) )
	{
		UINT AttributesSet = rObjectRef.ObjectAttributesSet();
		bool Checked = (AttributesSet & m_AttributesSetFilter) == m_AttributesSetFilter;
		if( Checked )
		{
			SelectorItem.setCheckedState( IObjectSelectorItem::Checked ); 
		}
	}
	SelectorItem.setAttibute( Attribute );
	checkLocationFilters( m_LocationInputFilters, Location );
	m_TreeContainer.insertLeaf( Location, SelectorItem );
}

void ObjectTreeGenerator::insertTreeObject( const SVString& rLocation )
{
	ObjectSelectorItem SelectorItem;
	IObjectSelectorItem::AttributeEnum Attribute( static_cast<IObjectSelectorItem::AttributeEnum> (IObjectSelectorItem::Leaf | IObjectSelectorItem::Checkable) );

	SelectorItem.setName( rLocation );
	SelectorItem.setAttibute( Attribute );
	//Make copy of location because reference is const
	SVString Location( rLocation );
	checkLocationFilters( m_LocationInputFilters, Location );
	m_TreeContainer.insertLeaf( Location, SelectorItem );
}

void ObjectTreeGenerator::insertOutputList( SVOutputInfoListClass& rOutputList )
{
	CWaitCursor Wait;

	SVObjectReferenceVector ObjectList;

	filterObjects( rOutputList, ObjectList );

	insertTreeObjects( ObjectList );
}

INT_PTR ObjectTreeGenerator::showDialog( const SVString& rTitle, const SVString& rTabTitle, CWnd* pParent )
{
	INT_PTR Result( IDCANCEL );
	HINSTANCE ResourceInstance( NULL );

	//Make sure Object result cleared could still have previous result
	m_SingleObjectResult.Clear();

	if( S_OK == LoadDll::Instance().getDll( SVOResourceDll, ResourceInstance ) && ( NULL != ResourceInstance) )
	{
		CWaitCursor* pWait = new CWaitCursor;

		m_TreeContainer.setNodeCheckedStates();
		m_TreeContainer.synchronizeCheckedStates();

		//Set the resource instance to the resource dll
		AfxSetResourceHandle( ResourceInstance );

		//If no parent then set it to the main application window
		if( nullptr == pParent )
		{
			pParent = AfxGetApp()->GetMainWnd();
		}
		CResizablePropertySheet Sheet( rTitle.c_str(), pParent );

		bool isSingleObject = TypeSingleObject == (TypeSingleObject & m_SelectorType);
		ObjectSelectorPpg ObjectSelectorPage( m_TreeContainer, rTabTitle,  isSingleObject );

		//Don't display the apply button
		Sheet.m_psh.dwFlags |= PSH_NOAPPLYNOW;

		Sheet.AddPage( &ObjectSelectorPage );

		delete pWait;
		pWait = nullptr;

		Result = Sheet.DoModal();

		AfxSetResourceHandle( AfxGetApp()->m_hInstance );

		if( IDOK == Result )
		{
			checkModifiedItems();
		}

		//Clear except for single select results which is still needed
		Clear( false );
	}
	
	return Result;
}

bool ObjectTreeGenerator::setCheckItems( const SVStringSet& rItems )
{
	bool Result( false );

	SVStringSet::const_iterator IterName( rItems.begin() );

	while( rItems.end() != IterName )
	{
		ObjectTreeItems::SVTree_iterator Iter( m_TreeContainer.begin() );
		//Need to check if input filters are used
		SVString Location( *IterName );
		checkLocationFilters( m_LocationInputFilters,  Location );
		//If an array we need the extra branch
		SVString::size_type BracketPos( SVString::npos );
		BracketPos = Location.rfind( _T("[") );
		if( SVString::npos !=  BracketPos)
		{
			SVString::size_type DotPos( SVString::npos );
			DotPos = Location.rfind( _T("."), BracketPos );
			if( SVString::npos !=  DotPos)
			{
				SVString Name;
				Name = Location.Mid(DotPos, BracketPos - DotPos);
				Location.insert( DotPos, Name.c_str() );
			}
		}
		Iter = m_TreeContainer.findItem( Location );
		if( m_TreeContainer.end() != Iter )
		{
			Iter->second.setCheckedState( IObjectSelectorItem::Checked );
			Result = true;
		}
		++IterName;
	}

	return Result;
}

void ObjectTreeGenerator::setLocationFilter( const FilterEnum& rType, const SVString& rFilter, const SVString& rReplace )
{
	switch( rType )
	{
	case FilterInput:
		{
			m_LocationInputFilters[ rFilter ] = rReplace;
		}
		break;

	case FilterOutput:
		{
			m_LocationOutputFilters[ rFilter ] = rReplace;
		}
		break;

	default:
		break;
	}
}
#pragma endregion Public Methods

#pragma region Private Methods
void ObjectTreeGenerator::setSelectorItemType( const SVObjectReference& rObjectRef, ObjectSelectorItem &rSelectorItem )
{
	switch( rObjectRef->GetObjectType() )
	{
	case SVDWordValueObjectType:
		{
			rSelectorItem.setItemType( ObjectSelectorItem::ItDword );
		}
		break;
	case SVLongValueObjectType:
		{
			rSelectorItem.setItemType( ObjectSelectorItem::ItLong );
		}
		break;
	case SVDoubleValueObjectType:
		{
			rSelectorItem.setItemType( ObjectSelectorItem::ItDouble );
		}
		break;
	case SVBoolValueObjectType:
		{
			rSelectorItem.setItemType( ObjectSelectorItem::ItBool );
		}
		break;
	case SVPointValueObjectType:
		{
			rSelectorItem.setItemType( ObjectSelectorItem::ItPoint );
		}
		break;
	case SVByteValueObjectType:
		{
			rSelectorItem.setItemType( ObjectSelectorItem::ItByte );
		}
		break;
	case SVStringValueObjectType:
		{
			rSelectorItem.setItemType( ObjectSelectorItem::ItString );
		}
		break;
	case SVInt64ValueObjectType:
		{
			rSelectorItem.setItemType( ObjectSelectorItem::ItInt64 );
		}
		break;
	case SVCharValueObjectType:
		{
			rSelectorItem.setItemType( ObjectSelectorItem::ItChar );
		}
		break;
	case SVVariantValueObjectType:
		{
			rSelectorItem.setItemType( ObjectSelectorItem::ItVariant );
		}
		break;
	default:
		break;
	}
}

void ObjectTreeGenerator::filterObjects( SVOutputInfoListClass& rOutputList, SVObjectReferenceVector& rObjectList )
{
	int nCount = rOutputList.GetSize();
	for( int i = 0; i < nCount; i++ )
	{
		const SVOutObjectInfoStruct* pInfoItem = NULL;

		try
		{
			pInfoItem = rOutputList.GetAt(i);
		}
		catch( ... )
		{
			::AfxMessageBox( "EXCEPTION: Error in getting Output Object Information Structure" );
		}

		if( pInfoItem != NULL )
		{
			SVObjectReference ObjectRef;

			try
			{
				ObjectRef = pInfoItem->GetObjectReference();
			}
			catch( ... )
			{
				::AfxMessageBox( "EXCEPTION: Error in getting Object" );
			}

			SVObjectClass* pValueObject = ObjectRef.Object();
			if ( pValueObject != NULL && pValueObject->IsArray() )
			{
				bool bAttributesOK = (pValueObject->ObjectAttributesAllowed() & m_AttributesAllowedFilter) == m_AttributesAllowedFilter;
				bAttributesOK = bAttributesOK && ( (pValueObject->ObjectAttributesAllowed() & SV_HIDDEN) == 0 );
				if( bAttributesOK )
				{
					if ( getAllowWholeArrays() )
					{
						//place entire array specifier at the top of the list
						ObjectRef.SetEntireArray();
						rObjectList.push_back( ObjectRef );
					}

					// add array elements
					int iArraySize = pValueObject->GetArraySize();
					for ( int i = 0; i < iArraySize; i++ )
					{
						ObjectRef.SetArrayIndex(i);
						rObjectList.push_back( ObjectRef );
					}
				}
			}// end if array value object
			else
			{
				// Check for the required Output object attributes.
				if( pValueObject )
				{
					bool bAttributesOK = (ObjectRef->ObjectAttributesAllowed() & m_AttributesAllowedFilter) == m_AttributesAllowedFilter;
					bAttributesOK = bAttributesOK && ( (pValueObject->ObjectAttributesAllowed() & SV_HIDDEN) == 0 );
					if( bAttributesOK )
					{
						rObjectList.push_back( ObjectRef );
					}
				}
			}
		}
		else
		{
			::AfxMessageBox( "ERROR: Error in getting Output Object Information Structure" );
		}
	}
}

bool ObjectTreeGenerator::checkModifiedItems()
{
	bool Result( false );
	ObjectSelectorItem SingleObjectResult;

	ObjectTreeItems::SVTree_pre_order_iterator Iter( m_TreeContainer.pre_order_begin() );

	while( m_TreeContainer.pre_order_end() != Iter )
	{
		bool Modified( Iter->second.getCheckedState() != Iter->second.getOrgCheckedState() );
		Iter->second.setModified( Modified );
		if( Modified )
		{
			Result = true;
		}

		if( IObjectSelectorItem::Checked == Iter->second.getCheckedState() && TypeSingleObject == (TypeSingleObject & m_SelectorType) )
		{
			SingleObjectResult = Iter->second;
			//If it is an array we need to modify the location back to its original form
			if( Iter->second.isArray() )
			{
				SVString Location = SingleObjectResult.getLocation();

				SVString Name( SingleObjectResult.getName() );

				SVString::size_type Pos = 0;
				//Array name will have [ ] 
				if( SVString::npos != (Pos = Name.rfind("[")) )
				{
					Name = Name.Left( Pos );
				}
				Name += _T(".");
				Location.replace( Name.c_str(), _T("") );
				SingleObjectResult.setLocation( Location );
			}
		}
		++Iter;
	}

	if( TypeSetAttributes == (m_SelectorType & TypeSetAttributes) )
	{
		setItemAttributes();
	}
	if( TypeSingleObject == (m_SelectorType & TypeSingleObject) )
	{
		m_SingleObjectResult = SingleObjectResult;
		SVString Location( m_SingleObjectResult.getLocation() );
		checkLocationFilters( m_LocationOutputFilters, Location );
		if( m_SingleObjectResult.getLocation() != Location )
		{
			m_SingleObjectResult.setLocation( Location );
		}
	}

	return Result;
}

void ObjectTreeGenerator::setItemAttributes()
{
	ObjectTreeItems::SVTree_const_pre_order_iterator Iter( m_TreeContainer.pre_order_begin() );

	while( m_TreeContainer.pre_order_end() != Iter )
	{
		//only leafs are of interest to set the attributes
		if( Iter->second.isLeaf() && Iter->second.isModified() )
		{
			//The tree item key is the object GUID
			SVGUID ObjectGuid( Iter->second.getItemKey() );

			SVObjectClass* pObject(NULL);
			SVObjectManagerClass::Instance().GetObjectByIdentifier( ObjectGuid, pObject );

			if ( NULL != pObject )
			{
				SVObjectReference ObjectRef = pObject;
				//If an array must set the index
				if( Iter->second.isArray() )
				{
					ObjectRef.SetArrayIndex( Iter->second.getArrayIndex() );
				}
				UINT AttributesSet = ObjectRef.ObjectAttributesSet();

				switch( Iter->second.getCheckedState() )
				{
				case IObjectSelectorItem::Checked:
					AttributesSet |= m_AttributesSetFilter;
					break;
				case IObjectSelectorItem::Unchecked:
					AttributesSet &= ~m_AttributesSetFilter;
					break;
				default:
					break;
				}
				ObjectRef.ObjectAttributesSetRef() = AttributesSet;
			}
		}
		++Iter;
	}
}

void ObjectTreeGenerator::checkLocationFilters( const TranslateMap& rFilters, SVString& rLocation ) const
{
	TranslateMap::const_iterator Iter( rFilters.begin() );

	while( rFilters.end() != Iter )
	{
		SVString Filter = Iter->first;

		if( 0 == rLocation.find( Filter.c_str() ) )
		{
			//if the dot is not at the end of the filter add it
			if( Filter.size() != Filter.rfind( _T(".") ))
			{
				Filter += _T(".");
			}

			rLocation.replace( Filter.c_str(), Iter->second.c_str() );
		}
		++Iter;
	}
}
#pragma endregion Private Methods

//******************************************************************************
//* LOG HISTORY:
//******************************************************************************
/*
$Log:   N:\PVCSarch65\ProjectFiles\archives\SVObserver_SRC\ObjectSelectorLibrary\ObjectTreeGenerator.cpp_v  $
 * 
 *    Rev 1.0   17 Jul 2014 11:16:26   gramseier
 * Project:  SVObserver
 * Change Request (SCR) nbr:  909
 * SCR Title:  Object Selector replacing Result Picker and Output Selector SVO-72, 40, 130
 * Checked in by:  gRamseier;  Guido Ramseier
 * Change Description:  
 *   Initial check in.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
*/
