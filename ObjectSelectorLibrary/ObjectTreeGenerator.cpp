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
#include "Definitions/StringTypeDef.h"
#include "SVContainerLibrary/ObjectSelectorItem.h"
#include "SVMFCControls/ResizablePropertySheet.h"
#include "ObjectSelectorPpg.h"
#include "ObjectFilterPpg.h"
#include "SVOResource/ConstGlobalSvOr.h"
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
		m_SelectorType = ObjectTreeGenerator::TypeNone;
		m_helpID = 0;
	}
		
	void ObjectTreeGenerator::insertTreeObjects(const SvPb::TreeItem& rTreeItem)
	{
		insertChildren(rTreeItem);

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

		selectorPage.setHelpID(m_helpID);
		//Don't display the apply button
		Sheet.m_psh.dwFlags |= PSH_NOAPPLYNOW;

		Sheet.AddPage( &selectorPage );
		Sheet.AddPage( &filterPage );

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

		for(const auto& rItemName : rItems)
		{
			std::string Location = rItemName;
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
			SvCl::ObjectTreeItems::iterator Iter = m_TreeContainer.findItem( Location );
			if( m_TreeContainer.end() != Iter )
			{
				Iter->second->m_CheckedState = SvCl::ObjectSelectorItem::CheckedEnabled;
				Result = true;
			}
		}

		return Result;
	}

	std::string ObjectTreeGenerator::getSingleObjectResult() const
	{
		//If Single select then it is the first result
		if (0 < m_SelectedObjects.size())
		{
			return m_SelectedObjects[0];
		}
		return std::string();
	}

	void ObjectTreeGenerator::setSelectorType(const SelectorTypeEnum& rSelectorType, int helpID)
	{
		m_SelectorType = rSelectorType;
		m_helpID = helpID;
		if(0 == m_helpID)
		{
			m_helpID = (TypeSingleObject == m_SelectorType) ? IDD_OUTPUT_SELECTOR : IDD_OBJECT_SELECTOR_PPG;
			m_helpID += SvOr::HELPFILE_DLG_IDD_OFFSET;
		}
		//Reset the leaf count here as this method is called at the start of inserting the objects into the selector
		m_LeafCount = 0;
	}
	#pragma endregion Public Methods

	#pragma region Private Methods
	void ObjectTreeGenerator::insertChildren(const SvPb::TreeItem& rTreeItem)
	{
		for (int i = 0; i < rTreeItem.children_size(); i++)
		{
			const SvPb::TreeItem& rChild = rTreeItem.children(i);
			//If no children then it is a leaf
			if(0 == rChild.children_size())
			{
				insertTreeObject(rChild);
			}
			else
			{
				insertChildren(rChild);
			}
		}
	}

	void ObjectTreeGenerator::insertTreeObject(const SvPb::TreeItem& rTreeItem)
	{
		SvCl::ObjectSelectorItem SelectorItem{rTreeItem};
		if(TypeMultipleObject == m_SelectorType)
		{
			SelectorItem.m_CheckedState = rTreeItem.selected() ? SvCl::ObjectSelectorItem::CheckedEnabled : SvCl::ObjectSelectorItem::UncheckedEnabled;
		}
		SelectorItem.m_Attribute = static_cast<SvCl::ObjectSelectorItem::AttributeEnum> (SvCl::ObjectSelectorItem::Leaf | SvCl::ObjectSelectorItem::Checkable);

		m_TreeContainer.insertLeaf(SelectorItem.m_Location, SelectorItem );
		m_LeafCount++;
	}

	bool ObjectTreeGenerator::checkModifiedItems()
	{
		bool Result( false );

		m_SelectedObjects.clear();
		m_ModifiedObjects.clear();
		SvCl::ObjectTreeItems::pre_order_iterator Iter;

		for (Iter = m_TreeContainer.pre_order_begin(); m_TreeContainer.pre_order_end() != Iter; ++Iter)
		{
			bool isSelected{ SvCl::ObjectSelectorItem::CheckedEnabled == Iter->second->m_CheckedState };
			bool isModified{ Iter->second->m_CheckedState != Iter->second->m_OrgCheckedState };

			if( isModified || isSelected )
			{
				if( isModified )
				{
					Iter->second->m_Modified = isModified;
					Result = true;
					if( Iter->second->isLeaf() )
					{
						m_ModifiedObjects.push_back(Iter->second->m_ItemKey);
					}
				}
				//Place items into the checked list when using Single or Multiple object
				if( TypeSingleObject == (TypeSingleObject & m_SelectorType) || TypeMultipleObject == (TypeMultipleObject & m_SelectorType) )
				{
					//Only place leafs into the list
					if( Iter->second->isLeaf() && isSelected )
					{
						m_SelectedObjects.push_back(Iter->second->m_ItemKey);
					}
				}
			}
		}

		return Result;
	}
	#pragma endregion Private Methods

} //namespace SvOsl

