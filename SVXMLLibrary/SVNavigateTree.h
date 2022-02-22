//******************************************************************************
//* COPYRIGHT (c) 2003 by Körber Pharma Inspection GmbH. All Rights Reserved
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVNavigateTreeClass
//* .File Name       : $Workfile:   SVNavigateTreeClass.h  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.0  $
//* .Check In Date   : $Date:   25 Apr 2013 19:47:32  $
//******************************************************************************

#pragma once

#pragma region Includes
//Moved to precompiled header: #include <comdef.h>
#pragma endregion Includes

namespace  SvXml
{
	class SVNavigateTree
	{
	private:
		SVNavigateTree();
		~SVNavigateTree();

	public:
		template< typename SVTreeType >
		static bool AddItem( SVTreeType &rTree, typename SVTreeType::SVBranchHandle pParent, LPCTSTR Name, const _variant_t &rVariant, typename SVTreeType::SVLeafHandle* ppItem = nullptr );

		template< typename SVTreeType >
		static bool SetItem( SVTreeType &rTree, typename SVTreeType::SVBranchHandle pParent, LPCTSTR Name, const _variant_t &rVariant, typename SVTreeType::SVLeafHandle* ppItem = nullptr );

		template< typename SVTreeType >
		static bool AddBranch( SVTreeType &rTree, typename SVTreeType::SVBranchHandle pParent, LPCTSTR Name, typename SVTreeType::SVBranchHandle* ppItem = nullptr );

		template< typename SVTreeType >
		static bool SetBranch( SVTreeType &rTree, typename SVTreeType::SVBranchHandle pParent, LPCTSTR Name, typename SVTreeType::SVBranchHandle* ppItem = nullptr );

		template< typename SVTreeType >
		static bool GetItemBranch( SVTreeType &rTree, LPCTSTR Name, typename SVTreeType::SVBranchHandle pParent, typename SVTreeType::SVBranchHandle& rpItem );

		template< typename SVTreeType >
		static bool GetItemLeaf( SVTreeType &rTree, LPCTSTR Name, typename SVTreeType::SVBranchHandle pParent, typename SVTreeType::SVLeafHandle& rpItem );

		template< typename SVTreeType >
		static bool GetItem( SVTreeType &rTree, LPCTSTR Name, typename SVTreeType::SVBranchHandle pParent, _variant_t& rData );

		template< typename SVTreeType >
		static bool DeleteItem( SVTreeType &rTree, typename SVTreeType::SVBranchHandle pItem );

		template< typename SVTreeType >
		static bool DeleteItem( SVTreeType &rTree, typename SVTreeType::SVBranchHandle pParent, typename SVTreeType::SVLeafHandle pItem );

		template< typename SVTreeType >
		static bool DeleteAllItems( SVTreeType &rTree );

		template< typename SVTreeType >
		static bool HasChildren(SVTreeType& rTree, typename SVTreeType::SVBranchHandle pItem);

		template< typename SVTreeType, typename SVPredicate >
		static bool FindBranch( SVTreeType &rTree, typename SVTreeType::SVBranchHandle startBranch, const SVPredicate& rPredicate, typename SVTreeType::SVBranchHandle& rFoundItem );

	};
} //namespace SvXml

#pragma region Inline
#include "SVNavigateTree.inl"
#pragma endregion Inline

