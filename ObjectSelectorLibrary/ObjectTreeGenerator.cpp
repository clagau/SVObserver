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
#include <algorithm>
#include "ObjectTreeGenerator.h"

#include "SVObjectLibrary\SVObjectManagerClass.h"
#include "SVObjectLibrary\SVObjectLibrary.h"
#include "ObjectInterfaces\ITaskObject.h"
#include "SVTreeLibrary\ObjectSelectorItem.h"
#include "ResizablePropertySheet.h"
#include "ObjectSelectorPpg.h"
#include "ObjectFilterPpg.h"
#pragma endregion Includes

#pragma region Declarations

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif
#pragma endregion Declarations

namespace Seidenader { namespace ObjectSelectorLibrary
{
	#pragma region Constructor
	ObjectTreeGenerator::ObjectTreeGenerator()
		: m_TreeContainer()
		, m_SelectedObjects()
		, m_ModifiedObjects()
		, m_LocationInputFilters()
		, m_LocationOutputFilters()
		, m_SelectorType( ObjectTreeGenerator::TypeNone )
		, m_AttributesFilter( 0 )
		, m_LeafCount( 0 )
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
			m_SelectedObjects.clear();
			m_ModifiedObjects.clear();
		}
		m_TreeContainer.clear();
		m_LocationInputFilters.clear();
		m_LocationOutputFilters.clear();
		m_SelectorType = ObjectTreeGenerator::TypeNone;
		m_AttributesFilter = 0;
	}

	void ObjectTreeGenerator::insertTreeObjects( const SVStringArray& rLocationList )
	{
		std::for_each(rLocationList.begin(), rLocationList.end(), [this](const SVString &rLocation){ insertTreeObject(rLocation); });
	}


	void ObjectTreeGenerator::insertTreeObjects( SvOi::ISelectorItemVector& rSelectorItems )
	{
		for(int i=0; i < rSelectorItems.getSize(); ++i )
		{
			SvOi::ISelectorItem* pItem = rSelectorItems.getAt(i);
			if( nullptr != pItem )
			{
				insertTreeObject( *pItem );
			}
		}
	}

	INT_PTR ObjectTreeGenerator::showDialog( LPCTSTR title, LPCTSTR mainTabTitle, LPCTSTR filterTabTitle, CWnd* pParent )
	{
		INT_PTR Result( IDCANCEL );

		//Make sure Object result cleared could still have previous result
		m_SelectedObjects.clear();
		m_ModifiedObjects.clear();

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
		CResizablePropertySheet Sheet( title, pParent );
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
			if( SV_VIEWABLE == m_AttributesFilter )
			{
				HelpID = IDD_RESULTS_PICKER;
			}
			else if( SV_PUBLISHABLE == m_AttributesFilter )
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
			SvTrl::ObjectTreeItems::iterator Iter( m_TreeContainer.begin() );
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
				Iter->second->setCheckedState( SvTrl::IObjectSelectorItem::CheckedEnabled );
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

	SVString ObjectTreeGenerator::convertObjectArrayName( const SvOi::ISelectorItem& rItem ) const
	{
		SVString Result( rItem.getLocation() );

		//If location is array then place an additional level with the array group name
		if( rItem.isArray() )
		{
			SVString Name;
			size_t Pos = 0;

			Name = rItem.getName();
			Name.insert( Pos, _T(".") );
			Pos = Result.rfind(_T('.'));
			if( SVString::npos != Pos )
			{
				Result.insert( Pos, Name.c_str() );
			}
		}

		return Result;
	}

	#pragma endregion Public Methods

	#pragma region Private Methods
	void ObjectTreeGenerator::insertTreeObject( const SVString& rLocation )
	{
		SvTrl::ObjectSelectorItem SelectorItem;
		SvTrl::IObjectSelectorItem::AttributeEnum Attribute( static_cast<SvTrl::IObjectSelectorItem::AttributeEnum> (SvTrl::IObjectSelectorItem::Leaf | SvTrl::IObjectSelectorItem::Checkable) );

		SelectorItem.setLocation( rLocation );
		SelectorItem.setAttibute( Attribute );
		SelectorItem.setCheckedState( SvTrl::IObjectSelectorItem::UncheckedEnabled );
		//Make copy of location because reference is const
		SVString DisplayLocation = getFilteredLocation( m_LocationInputFilters, rLocation );
		m_TreeContainer.insertLeaf( DisplayLocation, SelectorItem );
		m_LeafCount++;
	}

	void ObjectTreeGenerator::insertTreeObject( const SvOi::ISelectorItem& rItem )
	{
		SvTrl::ObjectSelectorItem SelectorItem;
		SvTrl::IObjectSelectorItem::AttributeEnum Attribute( static_cast<SvTrl::IObjectSelectorItem::AttributeEnum> (SvTrl::IObjectSelectorItem::Leaf | SvTrl::IObjectSelectorItem::Checkable) );

		SVString Location( rItem.getLocation() );
		SelectorItem.setName( rItem.getName() );
		SelectorItem.setLocation( Location );
		SelectorItem.setItemKey( rItem.getItemKey() );
		SelectorItem.setItemTypeName( rItem.getItemTypeName() );
		SelectorItem.setCheckedState( SvTrl::IObjectSelectorItem::UncheckedEnabled );

		if( rItem.isArray() )
		{
			Location = convertObjectArrayName( rItem );
			SelectorItem.setArrayIndex( rItem.getArrayIndex() );
			Attribute = static_cast<SvTrl::IObjectSelectorItem::AttributeEnum> (SvTrl::IObjectSelectorItem::Leaf | SvTrl::IObjectSelectorItem::Checkable | SvTrl::IObjectSelectorItem::Array);
		}

		if( TypeSetAttributes == (m_SelectorType & TypeSetAttributes) )
		{
			if( rItem.isSelected() )
			{
				SelectorItem.setCheckedState( SvTrl::IObjectSelectorItem::CheckedEnabled );
			}
		}

		if( rItem.isArray() )
		{
			SelectorItem.setArrayIndex( rItem.getArrayIndex() );
			Attribute = static_cast<SvTrl::IObjectSelectorItem::AttributeEnum> (SvTrl::IObjectSelectorItem::Leaf | SvTrl::IObjectSelectorItem::Checkable | SvTrl::IObjectSelectorItem::Array);
		}

		SelectorItem.setAttibute( Attribute );
		Location = getFilteredLocation( m_LocationInputFilters, Location );
		m_TreeContainer.insertLeaf( Location, SelectorItem );
		m_LeafCount++;
	}

	bool ObjectTreeGenerator::checkModifiedItems()
	{
		bool Result( false );

		m_SelectedObjects.clear();
		m_ModifiedObjects.clear();
		SvTrl::ObjectTreeItems::pre_order_iterator Iter;

		for( Iter = m_TreeContainer.pre_order_begin(); m_TreeContainer.pre_order_end() != Iter; ++Iter )
		{
			bool isSelected( SvTrl::IObjectSelectorItem::CheckedEnabled == Iter->second->getCheckedState() );
			bool isModified( Iter->second->getCheckedState() != Iter->second->getOrgCheckedState() );

			if( isModified || isSelected )
			{
				SelectorItem SelectedItem;

				SelectedItem.setName( Iter->second->getName().c_str() );
				SelectedItem.setLocation( Iter->second->getLocation().c_str() );
				SelectedItem.setDisplayLocation( Iter->second->getDisplayLocation().c_str() );
				SelectedItem.setItemTypeName( Iter->second->getItemTypeName().c_str() );
				SelectedItem.setItemKey( Iter->second->getItemKey() );
				SelectedItem.setArray( Iter->second->isArray() );
				SelectedItem.setArrayIndex( Iter->second->getArrayIndex() );
				SelectedItem.setSelected( isSelected );
				convertLocation( SelectedItem );

				if( isModified )
				{
					Iter->second->setModified( isModified );
					Result = true;
					if( Iter->second->isLeaf() )
					{
						m_ModifiedObjects.push_back( SelectedItem );
					}
				}
				//Place items into the checked list when using Single or Multiple object
				if( TypeSingleObject == (TypeSingleObject & m_SelectorType) || TypeMultipleObject == (TypeMultipleObject & m_SelectorType) )
				{
					//Only place leafs into the list
					if( Iter->second->isLeaf() && isSelected )
					{
						m_SelectedObjects.push_back( SelectedItem );
					}
				}
			}
		}

		if( TypeSetAttributes == (m_SelectorType & TypeSetAttributes) )
		{
			setItemAttributes();
		}


		return Result;
	}

	void ObjectTreeGenerator::setItemAttributes()
	{
		SvTrl::ObjectTreeItems::const_pre_order_iterator Iter( m_TreeContainer.pre_order_begin() );

		while( m_TreeContainer.pre_order_end() != Iter )
		{
			//only leafs are of interest to set the attributes
			if( Iter->second->isLeaf() && Iter->second->isModified() )
			{
				//The tree item key is the object GUID
				SVGUID ObjectGuid( Iter->second->getItemKey() );

				SVObjectClass* pObject( nullptr );
				SVObjectManagerClass::Instance().GetObjectByIdentifier( ObjectGuid, pObject );

				if ( nullptr != pObject )
				{
					SVObjectReference ObjectRef = pObject;
					//If an array must set the index
					if( Iter->second->isArray() )
					{
						ObjectRef.SetArrayIndex( Iter->second->getArrayIndex() );
					}
					UINT AttributesSet = ObjectRef.ObjectAttributesSet();

					switch( Iter->second->getCheckedState() )
					{
					case SvTrl::IObjectSelectorItem::CheckedEnabled:
						AttributesSet |= m_AttributesFilter;
						break;
					case SvTrl::IObjectSelectorItem::UncheckedEnabled:
						AttributesSet &= ~m_AttributesFilter;
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
		std::string rFilterLocation(rLocation.c_str());
		bool bFound = false;

		for (TranslateMap::const_iterator Iter = rFilters.begin(); rFilters.end() != Iter && !bFound; ++Iter)
		{
			SVString Filter = Iter->first;

			// find the filter at the first position of the string
			size_t pos = rFilterLocation.find( Filter.c_str() );
			if (0 == pos)
			{
				bFound = true;
				//if the dot is not at the end of the filter add it
				if( Filter.size() != Filter.rfind( _T(".") ))
				{
					Filter += _T(".");
				}
				// Do not use SVString::replace as it will replace all occurrences!
				// Use std::string to just replace the first occurrence at the found position (position 0 in this case)
				rFilterLocation.replace( pos, Filter.size(), Iter->second.c_str() );
			}
		}
		return rFilterLocation.c_str();
	}

	void ObjectTreeGenerator::convertLocation( SelectorItem& rSelectedItem )
	{
		SVString Location = getFilteredLocation( m_LocationOutputFilters, rSelectedItem.getLocation() );
		if( rSelectedItem.getLocation() != Location )
		{
			rSelectedItem.setLocation( Location.c_str() );
		}

		SVString DisplayLocation = getFilteredLocation( m_LocationOutputFilters, rSelectedItem.getDisplayLocation() );
		//The extra group name for arrays must be removed
		if( rSelectedItem.isArray() )
		{
			SVString Name( rSelectedItem.getName() );

			SVString::size_type Pos = 0;
			//Array name will have [ ] 
			if( SVString::npos != (Pos = Name.rfind("[")) )
			{
				Name = Name.Left( Pos );
			}
			Name += _T(".");
			DisplayLocation.replace( Name.c_str(), _T("") );
		}
		if( rSelectedItem.getDisplayLocation() != DisplayLocation )
		{
			rSelectedItem.setDisplayLocation( DisplayLocation.c_str() );
		}
	}

	#pragma endregion Private Methods
} /* namespace ObjectSelectorLibrary */ } /* namespace Seidenader */

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