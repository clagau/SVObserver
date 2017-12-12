//*****************************************************************************
/// \copyright (c) 2015,2015 by Seidenader Maschinenbau GmbH
/// All Rights Reserved 
//*****************************************************************************
/// This is a tree template with key, data pairs as node
//******************************************************************************
#pragma once

#pragma region Includes
//Moved to precompiled header: #include <algorithm>
//Moved to precompiled header: #include <memory>
//Moved to precompiled header: #include <utility>
//Moved to precompiled header: #include <sequential_tree.h>
#include "SVCompareKeys.h"
#pragma endregion Includes

namespace SvCl
{
	template <typename Key, typename Data>
	class SVTree: public tcl::sequential_tree< std::pair<Key, std::shared_ptr<Data> > >
	{
	#pragma region Declarations
	public:
		typedef Data ElementData;
		typedef std::shared_ptr< Data > DataPtr;
		typedef std::pair<Key, DataPtr > SVTreeElement;
		typedef tcl::sequential_tree< SVTreeElement > SVTreeContainer;
	#pragma endregion Declarations

	#pragma region Constructor
	public:
		SVTree();

		//************************************
		//! The class constructor
		//! \param rTree [in] reference to a copy of the tree to create
		//************************************
		SVTree( const SVTreeContainer& rTree );

		virtual ~SVTree();
	#pragma endregion Constructor

	#pragma region Public Methods
	public:
		//************************************
		//! Static method to get the total count of leafs and nodes
		//! \param rTree [in] reference to the tree to get the total
		//! \returns the size of the tree elements
		//************************************
		static size_t getCount( const SVTreeContainer& rTree );

		//************************************
		//! Static method to get the element data of a child corresponding to the key
		//! \param rTree [in] reference to the tree parent node
		//! \param rKey [in] reference to the key to search for in the children's list
		//! \param rData [out] reference to the data to fill 
		//! \returns S_OK when data found
		//************************************
		static HRESULT getData( const SVTreeContainer& rTree, const Key& rKey, ElementData& rData );

		//************************************
		//! Static method to find the element of a child corresponding to the key
		//! \param rTree [in] reference to the tree parent node
		//! \param rKey [in] reference to the key to search for in the children's list
		//! \returns iterator to the corresponding element
		//************************************
		static iterator find( SVTreeContainer& rTree, const Key& rKey );

		//************************************
		//! Static method to find the element of a child corresponding to the key
		//! \param rTree [in] reference to the tree parent node
		//! \param rKey [in] reference to the key to search for in the children's list
		//! \returns a constant iterator to the corresponding element
		//************************************
		static const_iterator find( const SVTreeContainer& rTree, const Key& rKey );
	#pragma endregion Public Methods
	};
} //namespace SvCl

#include "SVTree.inl"
