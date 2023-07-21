//******************************************************************************
//* COPYRIGHT (c) 2014 by Körber Pharma Inspection GmbH. All Rights Reserved
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
#include "ObjectFilterPpg.h"
#include "ObjectTreeGenerator.h"
#include "ObjectSelectorPpg.h"
#include "Definitions/StringTypeDef.h"
#include "SVContainerLibrary/ObjectSelectorItem.h"
#include "SVMFCControls/ResizablePropertySheet.h"
#include "SVProtoBuf/ConverterHelper.h"
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
		int childrenSize = rTreeItem.children_size();
		SvPb::TreeItem mainTreeItem;
		if (0 < childrenSize)
		{
			std::vector<SvPb::TreeItem> itemVector;
			itemVector.insert(itemVector.begin(), rTreeItem.children().begin(), rTreeItem.children().end());
			SvPb::convertVectorToTree(itemVector, &mainTreeItem);
		}

		insertChildren(mainTreeItem);

		m_TreeContainer.clearBranchMap();
	}

	INT_PTR ObjectTreeGenerator::showDialog( LPCTSTR title, LPCTSTR mainTabTitle, LPCTSTR filterTabTitle, CWnd* pParent, LPCTSTR nodeToBeSelected)
	{
		//Make sure Object result cleared could still have previous result
		m_SelectedObjects.clear();
		m_ModifiedObjects.clear();

		CWaitCursor* pWait = new CWaitCursor;

		bool isSingleObject = TypeSingleObject == (TypeSingleObject & m_SelectorType);

		m_TreeContainer.setNodeCheckedStates();
		m_TreeContainer.synchronizeCheckedStates();

		//If no parent then set it to the main application window
		if( nullptr == pParent )
		{
			pParent = AfxGetApp()->GetMainWnd();
		}
		SvMc::ResizablePropertySheet Sheet( title, pParent );
		ObjectSelectorPpg selectorPage(m_TreeContainer, mainTabTitle, isSingleObject, nodeToBeSelected, m_showAllNodes);
		ObjectFilterPpg filterPage( m_TreeContainer, filterTabTitle, isSingleObject );

		selectorPage.setHelpID(m_helpID);
		//Don't display the apply button
		Sheet.m_psh.dwFlags |= PSH_NOAPPLYNOW;

		Sheet.AddPage( &selectorPage );
		Sheet.AddPage( &filterPage );

		delete pWait;
		pWait = nullptr;

		INT_PTR Result {Sheet.DoModal()};

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

		m_showAllNodes = false;
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
			//Check if objectID format
			if (0 == rItemName.find(_T("{#")))
			{
				SvCl::ObjectTreeItems::pre_order_iterator iter(m_TreeContainer.pre_order_begin());

				while (m_TreeContainer.pre_order_end() != iter)
				{
					if (iter->second->m_ItemKey == rItemName)
					{
						iter->second->m_CheckedState = SvCl::ObjectSelectorItem::CheckedEnabled;
						m_showAllNodes |= (false == iter->second->m_shortMode);
						break;
					}
					++iter;
				}
			}
			else
			{
				SvCl::ObjectTreeItems::iterator Iter = m_TreeContainer.findItem(Location);
				if (m_TreeContainer.end() != Iter)
				{
					Iter->second->m_CheckedState = SvCl::ObjectSelectorItem::CheckedEnabled;
					m_showAllNodes |= (false == Iter->second->m_shortMode);
					Result = true;
				}
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

		m_TreeContainer.setTreeType(TypeSingleObject == (TypeSingleObject & m_SelectorType));
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
			if(false == rChild.objectidindex().empty())
			{
				insertTreeObject(rChild);
			}
			insertChildren(rChild);
		}
	}

	void ObjectTreeGenerator::insertTreeObject(const SvPb::TreeItem& rTreeItem)
	{
		SvCl::ObjectSelectorItem SelectorItem{rTreeItem};
		if (0 == rTreeItem.children_size())
		{
			SelectorItem.m_Attribute = SvCl::ObjectSelectorItem::Leaf;
		}
		else
		{
			SelectorItem.m_Attribute = static_cast<SvCl::ObjectSelectorItem::AttributeEnum> (SvCl::ObjectSelectorItem::Node | SvCl::ObjectSelectorItem::Leaf);
		}

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

