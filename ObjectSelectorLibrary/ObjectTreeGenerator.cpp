//******************************************************************************
//* COPYRIGHT (c) 2014 by Seidenader
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : ObjectTreeGenerator
//* .File Name       : $Workfile:   ObjectTreeGenerator.cpp  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.7  $
//* .Check In Date   : $Date:   20 Jan 2015 09:50:48  $
//******************************************************************************

#pragma region Includes
#include "stdafx.h"
#include "ObjectTreeGenerator.h"

#include "SVObjectLibrary\SVObjectManagerClass.h"
#include "SVTreeLibrary\ObjectSelectorItem.h"
#include "SVSystemLibrary\LoadDll.h"
#include "ResizablePropertySheet.h"
#include "ObjectSelectorPpg.h"
#include "ObjectFilterPpg.h"
#include "SVObjectLibrary\SVObjectLibrary.h"
#include "SVObserver\SVValueObjectClass.h"
#include "SVObserver\BasicValueObject.h"
#pragma endregion Includes

#pragma region Declarations
using namespace Seidenader::ObjectSelectorLibrary;

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif
#pragma endregion Declarations

#pragma region Constructor
ObjectTreeGenerator::ObjectTreeGenerator()
	: m_TreeContainer()
	, m_Results()
	, m_LocationInputFilters()
	, m_LocationOutputFilters()
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
		m_Results.clear();
	}
	m_TreeContainer.Clear();
	m_LocationInputFilters.clear();
	m_LocationOutputFilters.clear();
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

void ObjectTreeGenerator::insertTreeObjects( const SvTrl::ObjectTreeItems& rTree )
{
	SvTrl::ObjectTreeItems::SVTree_const_pre_order_iterator Iter( rTree.pre_order_begin() );
	
	while( rTree.pre_order_end() != Iter )
	{
		if( Iter->second.isLeaf() )
		{
			//InsertLeaf needs a reference to a non const item
			SvTrl::ObjectSelectorItem SelectorItem( Iter->second );
			m_TreeContainer.insertLeaf( Iter->second.getDisplayLocation(), SelectorItem );
		}
		++Iter;
	}
}

void ObjectTreeGenerator::insertTreeObject( const SVObjectReference& rObjectRef )
{
	SvTrl::ObjectSelectorItem SelectorItem;
	SvTrl::IObjectSelectorItem::AttributeEnum Attribute( static_cast<SvTrl::IObjectSelectorItem::AttributeEnum> (SvTrl::IObjectSelectorItem::Leaf | SvTrl::IObjectSelectorItem::Checkable) );

	setSelectorItemType( rObjectRef, SelectorItem );

	SVString Location;
	Location = rObjectRef.GetCompleteOneBasedObjectName();

	SelectorItem.setName( rObjectRef->GetName() );
	SelectorItem.setLocation( Location );
	SelectorItem.setItemKey( rObjectRef->GetUniqueObjectID().ToVARIANT() );
	SelectorItem.setCheckedState( SvTrl::IObjectSelectorItem::UncheckedEnabled );

	if( rObjectRef->IsArray() )
	{
		int ArrayIndex( -1 );
		ArrayIndex = convertObjectArrayName( rObjectRef, Location );
		SelectorItem.setArrayIndex( ArrayIndex );
		Attribute = static_cast<SvTrl::IObjectSelectorItem::AttributeEnum> (SvTrl::IObjectSelectorItem::Leaf | SvTrl::IObjectSelectorItem::Checkable | SvTrl::IObjectSelectorItem::Array);
	}

	SelectorItem.setAttibute( Attribute );
	//Only set the checked states if of type attributes
	if( TypeSetAttributes == (m_SelectorType & TypeSetAttributes) )
	{
		UINT AttributesSet = rObjectRef.ObjectAttributesSet();
		bool Checked = (AttributesSet & m_AttributesSetFilter) == m_AttributesSetFilter;
		if( Checked )
		{
			SelectorItem.setCheckedState( SvTrl::IObjectSelectorItem::CheckedEnabled );
		}
	}
	Location = getFilteredLocation( m_LocationInputFilters, Location );
	m_TreeContainer.insertLeaf( Location, SelectorItem );
}

void ObjectTreeGenerator::insertTreeObject( const SVString& rLocation )
{
	SVObjectReference objectRef;
	HRESULT hOk = SVObjectManagerClass::Instance().GetObjectByDottedName( rLocation, objectRef );
	if (S_OK == hOk)
	{
		insertTreeObject( objectRef );
	}
	else
	{
		SvTrl::ObjectSelectorItem SelectorItem;
		SvTrl::IObjectSelectorItem::AttributeEnum Attribute( static_cast<SvTrl::IObjectSelectorItem::AttributeEnum> (SvTrl::IObjectSelectorItem::Leaf | SvTrl::IObjectSelectorItem::Checkable) );

		SelectorItem.setLocation( rLocation );
		SelectorItem.setAttibute( Attribute );
		SelectorItem.setCheckedState( SvTrl::IObjectSelectorItem::UncheckedEnabled );
		//Make copy of location because reference is const
		SVString DisplayLocation = getFilteredLocation( m_LocationInputFilters, rLocation );
		m_TreeContainer.insertLeaf( DisplayLocation, SelectorItem );
	}
}

void ObjectTreeGenerator::insertTreeObject( const SvTrl::ObjectSelectorItem& rObjectItem )
{
	SvTrl::ObjectSelectorItem SelectorItem(rObjectItem);
	SvTrl::IObjectSelectorItem::AttributeEnum Attribute( static_cast<SvTrl::IObjectSelectorItem::AttributeEnum> (SvTrl::IObjectSelectorItem::Leaf | SvTrl::IObjectSelectorItem::Checkable) );

	SelectorItem.setAttibute( Attribute );
	SelectorItem.setCheckedState( SvTrl::IObjectSelectorItem::UncheckedEnabled );
	//Make copy of location because reference is const
	SVString DisplayLocation = getFilteredLocation( m_LocationInputFilters, SelectorItem.getLocation() );
	m_TreeContainer.insertLeaf( DisplayLocation, SelectorItem );
}

void ObjectTreeGenerator::insertOutputList( SvOi::IOutputInfoListClass& rOutputList )
{
	CWaitCursor Wait;

	SVObjectReferenceVector ObjectList;

	filterObjects( rOutputList, ObjectList );

	insertTreeObjects( ObjectList );
}

INT_PTR ObjectTreeGenerator::showDialog( const SVString& title, const SVString& mainTabTitle, const SVString& filterTabTitle, CWnd* pParent )
{
	INT_PTR Result( IDCANCEL );

	//Make sure Object result cleared could still have previous result
	m_Results.clear();

	CWaitCursor* pWait = new CWaitCursor;

	bool isSingleObject = TypeSingleObject == (TypeSingleObject & m_SelectorType);

	m_TreeContainer.setTreeType( isSingleObject );
	m_TreeContainer.setNodeCheckedStates();
	m_TreeContainer.synchronizeCheckedStates();

	//If no parent then set it to the main application window
	if( nullptr == pParent )
	{
		pParent = AfxGetApp()->GetMainWnd();
	}
	CResizablePropertySheet Sheet( title.c_str(), pParent );
	ObjectSelectorPpg selectorPage( m_TreeContainer, mainTabTitle, isSingleObject );
	ObjectFilterPpg filterPage( m_TreeContainer, filterTabTitle, isSingleObject );

	//Don't display the apply button
	Sheet.m_psh.dwFlags |= PSH_NOAPPLYNOW;

	Sheet.AddPage( &selectorPage );
	Sheet.AddPage( &filterPage );
	int HelpID( IDD_OBJECT_SELECTOR_PPG );
	if( isSingleObject)
	{
		HelpID = IDD_OUTPUT_SELECTOR;
	}
	else
	{
		if( SV_VIEWABLE == m_AttributesSetFilter )
		{
			HelpID = IDD_RESULTS_PICKER;
		}
		else if( SV_PUBLISHABLE == m_AttributesSetFilter )
		{
			HelpID = IDD_PUBLISHED_RESULTS;
		}
	}
	selectorPage.setHelpID( HelpID );

	delete pWait;
	pWait = nullptr;

	Result = Sheet.DoModal();

	if( IDOK == Result )
	{
		checkModifiedItems();
	}

	//Clear except for single select results which is still needed
	Clear( false );
	
	return Result;
}

bool ObjectTreeGenerator::setCheckItems( const SVStringSet& rItems )
{
	bool Result( false );

	SVStringSet::const_iterator IterName( rItems.begin() );

	while( rItems.end() != IterName )
	{
		SvTrl::ObjectTreeItems::SVTree_iterator Iter( m_TreeContainer.begin() );
		//Need to check if input filters are used
		SVString Location = getFilteredLocation( m_LocationInputFilters,  *IterName );
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
			Iter->second.setCheckedState( SvTrl::IObjectSelectorItem::CheckedEnabled );
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

int ObjectTreeGenerator::convertObjectArrayName( const SVObjectReference& rObjectRef, SVString& rLocation ) const
{
	int Result( -1 );

	//If location is array then place an additional level with the array group name
	if( rObjectRef->IsArray() )
	{
		SVString Name;
		size_t Pos = 0;

		Name = rObjectRef->GetName();
		Name.insert( Pos, _T(".") );
		Pos = rLocation.rfind(_T('.'));
		if( SVString::npos != Pos )
		{
			rLocation.insert( Pos, Name.c_str() );
		}
		Result = rObjectRef.ArrayIndex( true );
	}

	return Result;
}

void ObjectTreeGenerator::setSelectorItemType( const SVObjectReference& rObjectRef, SvTrl::ObjectSelectorItem &rSelectorItem )
{
	SVObjectClass *pObject = rObjectRef.Object();
	setSelectorItemType(pObject, rSelectorItem);
}

void ObjectTreeGenerator::setSelectorItemType( const SvOi::IObjectClass* pObject, SvTrl::ObjectSelectorItem &rSelectorItem )
{
	const SVObjectClass *pObjectClass = dynamic_cast<const SVObjectClass*>(pObject);
	setSelectorItemType(pObjectClass, rSelectorItem);
}

void ObjectTreeGenerator::setSelectorItemType( const SVObjectClass* pObject, SvTrl::ObjectSelectorItem &rSelectorItem )
{
	if ( const SVValueObjectClass* pValueObject = dynamic_cast<const SVValueObjectClass*>(pObject) )
	{
		CString typeName = _T("");
		pValueObject->GetTypeName(typeName);
		rSelectorItem.setItemTypeName(typeName);
	}
	else if( const BasicValueObject* pBasicObject = dynamic_cast<const BasicValueObject*>(pObject) )
	{
		rSelectorItem.setItemTypeName(pBasicObject->getTypeName());
	}
	else
	{
		rSelectorItem.setItemTypeName(_T(""));
	}
}
#pragma endregion Public Methods

#pragma region Private Methods
void ObjectTreeGenerator::filterObjects( SvOi::IOutputInfoListClass& rOutputList, SVObjectReferenceVector& rObjectList )
{
	int nCount = rOutputList.GetSize();
	for( int i = 0; i < nCount; i++ )
	{
		const SVOutObjectInfoStruct* pInfoItem = NULL;

		try
		{
			pInfoItem = dynamic_cast<SVOutObjectInfoStruct*>(rOutputList.GetInterfaceAt(i));
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

	m_Results.clear();
	SvTrl::ObjectTreeItems::SVTree_pre_order_iterator Iter( m_TreeContainer.pre_order_begin() );

	while( m_TreeContainer.pre_order_end() != Iter )
	{
		bool Modified( Iter->second.getCheckedState() != Iter->second.getOrgCheckedState() );
		if( Modified )
		{
			Iter->second.setModified( Modified );
			Result = true;
		}
		//Place items into the checked list when using Single or Multiple object
		if( TypeSingleObject == (TypeSingleObject & m_SelectorType) || TypeMultipleObject == (TypeMultipleObject & m_SelectorType) )
		{
			if( SvTrl::IObjectSelectorItem::CheckedEnabled == Iter->second.getCheckedState() )
			{
				m_Results.push_back( Iter->second );
			}
		}
		++Iter;
	}

	convertLocation();
	if( TypeSetAttributes == (m_SelectorType & TypeSetAttributes) )
	{
		setItemAttributes();
	}


	return Result;
}

void ObjectTreeGenerator::setItemAttributes()
{
	SvTrl::ObjectTreeItems::SVTree_const_pre_order_iterator Iter( m_TreeContainer.pre_order_begin() );

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
				case SvTrl::IObjectSelectorItem::CheckedEnabled:
					AttributesSet |= m_AttributesSetFilter;
					break;
				case SvTrl::IObjectSelectorItem::UncheckedEnabled:
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

SVString ObjectTreeGenerator::getFilteredLocation( const TranslateMap& rFilters, const SVString& rLocation ) const
{
	TranslateMap::const_iterator Iter( rFilters.begin() );
	SVString rFilterLocation(rLocation);

	while( rFilters.end() != Iter )
	{
		SVString Filter = Iter->first;

		if( 0 == rFilterLocation.find( Filter.c_str() ) )
		{
			//if the dot is not at the end of the filter add it
			if( Filter.size() != Filter.rfind( _T(".") ))
			{
				Filter += _T(".");
			}

			rFilterLocation.replace( Filter.c_str(), Iter->second.c_str() );
		}
		++Iter;
	}
	return rFilterLocation;
}

void ObjectTreeGenerator::convertLocation()
{
	SvTrl::ObjectSelectorItems::iterator Iter( m_Results.begin() );
	while( m_Results.end() != Iter )
	{
		SVString Location = getFilteredLocation( m_LocationOutputFilters, Iter->getLocation() );
		if( Iter->getLocation() != Location )
		{
			Iter->setLocation( Location );
		}

		SVString DisplayLocation = getFilteredLocation( m_LocationOutputFilters, Iter->getDisplayLocation() );
		//The extra group name for arrays must be removed
		if( Iter->isArray() )
		{
			SVString Name( Iter->getName() );

			SVString::size_type Pos = 0;
			//Array name will have [ ] 
			if( SVString::npos != (Pos = Name.rfind("[")) )
			{
				Name = Name.Left( Pos );
			}
			Name += _T(".");
			DisplayLocation.replace( Name.c_str(), _T("") );
		}
		if( Iter->getDisplayLocation() != DisplayLocation )
		{
			Iter->setDisplayLocation( DisplayLocation );
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
 *    Rev 1.7   20 Jan 2015 09:50:48   gramseier
 * Project:  SVObserver
 * Change Request (SCR) nbr:  965
 * SCR Title:  Update Object Selector Text Label; Update Icons; Add List Output
 * Checked in by:  gRamseier;  Guido Ramseier
 * Change Description:  
 *   Fix: Output filters not cleared in Clear method of singelton
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.6   07 Jan 2015 18:10:20   bwalter
 * Project:  SVObserver
 * Change Request (SCR) nbr:  933
 * SCR Title:  Add Filter Tab to Object Selector (SVO-377)
 * Checked in by:  mZiegler;  Marc Ziegler
 * Change Description:  
 *   Added method insertTreeObject.
 * Changed method setSelectorItemType to public.
 * Renamed method checkLocationFilters to getFilteredLocation to fix the issue where environment items did not show type information on the Object Selector Filter page.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.5   16 Dec 2014 17:42:54   bwalter
 * Project:  SVObserver
 * Change Request (SCR) nbr:  933
 * SCR Title:  Add Filter Tab to Object Selector (SVO-377)
 * Checked in by:  mZiegler;  Marc Ziegler
 * Change Description:  
 *   Changed showDialog method to show Filter page.  Simplified setSelectorItemType method.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.4   12 Dec 2014 03:28:20   gramseier
 * Project:  SVObserver
 * Change Request (SCR) nbr:  965
 * SCR Title:  Update Object Selector Text Label; Update Icons; Add List Output
 * Checked in by:  gRamseier;  Guido Ramseier
 * Change Description:  
 *   Fix: DisplayLocation use Output filters
 * Changed Method: convertLocation
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.3   04 Dec 2014 03:29:20   gramseier
 * Project:  SVObserver
 * Change Request (SCR) nbr:  965
 * SCR Title:  Update Object Selector Text Label; Update Icons; Add List Output
 * Checked in by:  gRamseier;  Guido Ramseier
 * Change Description:  
 *   Added support for list of multiple item checked results
 * Added DisplayLocation while Location is the original set dotted path name
 * Added Methds convertObjectArrayName; convertLocation
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.2   08 Sep 2014 09:47:22   gramseier
 * Project:  SVObserver
 * Change Request (SCR) nbr:  909
 * SCR Title:  Object Selector replacing Result Picker and Output Selector SVO-72, 40, 130
 * Checked in by:  gRamseier;  Guido Ramseier
 * Change Description:  
 *   Object Selector property page has a variable context ID
 * Changed methods: showDialog
 * 
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.1   25 Aug 2014 07:46:48   gramseier
 * Project:  SVObserver
 * Change Request (SCR) nbr:  909
 * SCR Title:  Object Selector replacing Result Picker and Output Selector SVO-72, 40, 130
 * Checked in by:  gRamseier;  Guido Ramseier
 * Change Description:  
 *   Added disabled checked states
 * Object Selector displays nodes disabled when in single select mode
 * Changed methods: insertTreeObject, showDialog, setCheckItems, checkModifiedItems, setItemAttributes
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
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