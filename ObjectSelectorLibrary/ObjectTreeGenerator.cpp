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
//Moved to precompiled header: #include <algorithm>
#include "ObjectTreeGenerator.h"
#include "ObjectInterfaces/IObjectManager.h"
#include "Definitions/ObjectDefines.h"
#include "Definitions/GlobalConst.h"
#include "Definitions/StringTypeDef.h"
#include "SVUtilityLibrary/StringHelper.h"
#include "SVContainerLibrary/ObjectSelectorItem.h"
#include "SVMFCControls/ResizablePropertySheet.h"
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

namespace SvOsl
{
	#pragma region Constructor
	ObjectTreeGenerator::ObjectTreeGenerator()
		: m_TreeContainer()
		, m_SelectedObjects()
		, m_ModifiedObjects()
		, m_LocationInputFilters()
		, m_LocationOutputFilters()
		, m_SelectorType( ObjectTreeGenerator::TypeNone )
		, m_AttributesFilter( SvDef::SV_NO_ATTRIBUTES )
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
		m_TreeContainer.clearBranchMap();
		m_LocationInputFilters.clear();
		m_LocationOutputFilters.clear();
		m_SelectorType = ObjectTreeGenerator::TypeNone;
		m_AttributesFilter = SvDef::SV_NO_ATTRIBUTES;
	}
		
	void ObjectTreeGenerator::insertTreeObjects( const SelectorItemVector& rSelectorItems )
	{
		std::for_each(rSelectorItems.begin(), rSelectorItems.end(), [this](const SelectorItem& rItem)->void
		{
			insertTreeObject( rItem );
		});
		m_TreeContainer.clearBranchMap();

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
		SvMc::ResizablePropertySheet Sheet( title, pParent );
		ObjectSelectorPpg selectorPage( m_TreeContainer, mainTabTitle, isSingleObject );
		ObjectFilterPpg filterPage( m_TreeContainer, filterTabTitle, isSingleObject );

		//Don't display the apply button
		Sheet.m_psh.dwFlags |= PSH_NOAPPLYNOW;

		Sheet.AddPage( &selectorPage );
		Sheet.AddPage( &filterPage );
		//@TODO[gra][7.50][30.08.2017]: The help ID's should be done differently this is becoming to complicated
		int HelpID( IDD_OBJECT_SELECTOR_PPG );
		if( isSingleObject)
		{
			HelpID = IDD_OUTPUT_SELECTOR;
		}
		else
		{
			if( SvDef::SV_VIEWABLE == m_AttributesFilter )
			{
				HelpID = IDD_RESULTS_PICKER;
			}
			else if( SvDef::SV_PUBLISHABLE == m_AttributesFilter )
			{
				if (TypeSetAttributes == (m_SelectorType & TypeSetAttributes))
				{
					HelpID = IDD_PUBLISHED_RESULTS;
				}
				else
				{
					HelpID = IDD_MONITOR_LIST_SELECTOR;
				}
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

	bool ObjectTreeGenerator::setCheckItems( const SvDef::StringSet& rItems )
	{
		bool Result( false );

		SvDef::StringSet::const_iterator IterName( rItems.begin() );

		for( ; rItems.end() != IterName;  ++IterName )
		{
			SvCl::ObjectTreeItems::iterator Iter( m_TreeContainer.begin() );
			//Need to check if input filters are used
			std::string Location = getFilteredLocation( m_LocationInputFilters, *IterName );
			//If an array we need the extra branch
			std::string::size_type BracketPos( std::string::npos );
			BracketPos = Location.rfind( _T("[") );
			if( std::string::npos !=  BracketPos)
			{
				size_t DotPos = Location.rfind( _T("."), BracketPos );
				if( std::string::npos !=  DotPos)
				{
					std::string Name = Location.substr(DotPos, BracketPos - DotPos);
					Location.insert( DotPos, Name.c_str() );
				}
			}
			Iter = m_TreeContainer.findItem( Location );
			if( m_TreeContainer.end() != Iter )
			{
				Iter->second->setCheckedState( SvCl::IObjectSelectorItem::CheckedEnabled );
				Result = true;
			}
		}

		return Result;
	}

	void ObjectTreeGenerator::setLocationFilter( const FilterEnum& rType, const std::string& rFilter, const std::string& rReplace )
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

	std::string ObjectTreeGenerator::convertObjectArrayName( const SvOi::ISelectorItem& rItem ) const
	{
		std::string Result( rItem.getDisplayLocation() );
		if( Result.empty() )
		{
			Result = rItem.getLocation();
		}

		//If location is array then place an additional level with the array group name
		if( rItem.isArray() )
		{
			std::string Name;
			size_t Pos = 0;

			Name = rItem.getName();
			Name.insert( Pos, _T(".") );
			Pos = Result.rfind(_T('.'));
			if( std::string::npos != Pos )
			{
				Result.insert( Pos, Name.c_str() );
			}
		}

		return Result;
	}

	#pragma endregion Public Methods

	#pragma region Private Methods
	void ObjectTreeGenerator::insertTreeObject( const SvOi::ISelectorItem& rItem )
	{
		SvCl::ObjectSelectorItem SelectorItem;
		SvCl::IObjectSelectorItem::AttributeEnum Attribute( static_cast<SvCl::IObjectSelectorItem::AttributeEnum> (SvCl::IObjectSelectorItem::Leaf | SvCl::IObjectSelectorItem::Checkable) );
		SelectorItem.setName( rItem.getName() );
		SelectorItem.setLocation( rItem.getLocation() );
		SelectorItem.setItemKey( rItem.getItemKey() );
		SelectorItem.setItemTypeName( rItem.getItemTypeName() );
		SelectorItem.setCheckedState( SvCl::IObjectSelectorItem::UncheckedEnabled );

		std::string Location( rItem.getDisplayLocation() );
		if( Location.empty() )
		{
			Location = rItem.getLocation();
		}
		if( rItem.isArray() )
		{
			Location = convertObjectArrayName( rItem );
			SelectorItem.setArrayIndex( rItem.getArrayIndex() );
			Attribute = static_cast<SvCl::IObjectSelectorItem::AttributeEnum> (SvCl::IObjectSelectorItem::Leaf | SvCl::IObjectSelectorItem::Checkable | SvCl::IObjectSelectorItem::Array);
		}

		if( TypeSetAttributes == (m_SelectorType & TypeSetAttributes) )
		{
			if( rItem.isSelected() )
			{
				SelectorItem.setCheckedState( SvCl::IObjectSelectorItem::CheckedEnabled );
			}
		}

		if( rItem.isArray() )
		{
			SelectorItem.setArrayIndex( rItem.getArrayIndex() );
			Attribute = static_cast<SvCl::IObjectSelectorItem::AttributeEnum> (SvCl::IObjectSelectorItem::Leaf | SvCl::IObjectSelectorItem::Checkable | SvCl::IObjectSelectorItem::Array);
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
		SvCl::ObjectTreeItems::pre_order_iterator Iter;

		for( Iter = m_TreeContainer.pre_order_begin(); m_TreeContainer.pre_order_end() != Iter; ++Iter )
		{
			bool isSelected( SvCl::IObjectSelectorItem::CheckedEnabled == Iter->second->getCheckedState() );
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
		SelectorItemVector::const_iterator Iter( m_ModifiedObjects.begin() );

		//Modified objects are only leafs from the tree
		for( ;m_ModifiedObjects.end() != Iter; ++Iter )
		{
			SVGUID ObjectID( Iter->getItemKey() );

			SvOi::IObjectClass* pObject( SvOi::getObject( ObjectID ) );

			if ( nullptr != pObject )
			{
				int ObjectIndex = Iter->isArray() ? Iter->getArrayIndex() : 0;

				if ( Iter->isSelected() )
				{
					pObject->SetObjectAttributesSet( m_AttributesFilter, SvOi::SetAttributeType::AddAttribute, ObjectIndex );
				}
				else
				{
					pObject->SetObjectAttributesSet( m_AttributesFilter, SvOi::SetAttributeType::RemoveAttribute, ObjectIndex );
				}
			}
		}
	}

	std::string ObjectTreeGenerator::getFilteredLocation( const SvDef::TranslateMap& rFilters, const std::string& rLocation ) const
	{
		std::string rFilterLocation( rLocation.c_str() );
		bool bFound = false;

		for (SvDef::TranslateMap::const_iterator Iter = rFilters.begin(); rFilters.end() != Iter && !bFound; ++Iter)
		{
			std::string Filter = Iter->first;

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
				// Do not use std::string::replace as it will replace all occurrences!
				// Use std::string to just replace the first occurrence at the found position (position 0 in this case)
				rFilterLocation.replace( pos, Filter.size(), Iter->second.c_str() );
			}
		}
		return rFilterLocation.c_str();
	}

	void ObjectTreeGenerator::convertLocation( SelectorItem& rSelectedItem )
	{
		std::string Location = getFilteredLocation( m_LocationOutputFilters, rSelectedItem.getLocation() );
		if( rSelectedItem.getLocation() != Location )
		{
			rSelectedItem.setLocation( Location.c_str() );
		}

		std::string DisplayLocation = getFilteredLocation( m_LocationOutputFilters, rSelectedItem.getDisplayLocation() );
		//The extra group name for arrays must be removed
		if( rSelectedItem.isArray() )
		{
			std::string Name( rSelectedItem.getName() );

			std::string::size_type Pos = 0;
			//Array name will have [ ] 
			if( std::string::npos != (Pos = Name.rfind("[")) )
			{
				Name = Name.substr( 0, Pos );
			}
			Name += _T(".");
			SvUl::searchAndReplace( DisplayLocation, Name.c_str(), _T("") );
		}
		if( rSelectedItem.getDisplayLocation() != DisplayLocation )
		{
			rSelectedItem.setDisplayLocation( DisplayLocation.c_str() );
		}
	}
	#pragma endregion Private Methods

} //namespace SvOsl

