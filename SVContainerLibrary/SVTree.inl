//******************************************************************************
//* COPYRIGHT (c) 2008 by Körber Pharma Inspection GmbH. All Rights Reserved
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVTreeAdapter
//* .File Name       : $Workfile:   SVTreeAdapter.inl  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.1  $
//* .Check In Date   : $Date:   17 Jul 2014 16:54:28  $
//******************************************************************************

namespace SvCl
{
	template<typename Key, typename Data>
	SVTree<Key, Data>::SVTree( )
	{
		SVTreeElement* pTreeElement( this->get() );
		if( nullptr != pTreeElement && nullptr == pTreeElement->second)
		{
			pTreeElement->second = std::shared_ptr<Data>{ new Data };
		}
	}

	template<typename Key, typename Data>
	SVTree<Key, Data>::SVTree( const SVTreeContainer& rTree ) : tcl::sequential_tree< std::pair< Key, std::shared_ptr<Data> > >( rTree)
	{
		SVTreeElement* pTreeElement(this->get() );
		if( nullptr != pTreeElement && nullptr == pTreeElement->second )
		{
			pTreeElement->second = std::shared_ptr<Data>{ new Data };
		}
	}

	template<typename Key, typename Data>
	SVTree<Key, Data>::~SVTree() 
	{ 
	}

	template<typename Key, typename Data>
	size_t SVTree<Key, Data>::getCount( const SVTreeContainer& rTree )
	{
		size_t l_Count = 0;

		for(auto Iter( rTree.begin() ); Iter != rTree.end(); ++Iter)
		{
			if( nullptr != Iter.node() )
			{
				l_Count += getCount( *Iter.node() );
			}
		}

		l_Count += rTree.size();

		return l_Count;
	}

	template<typename Key, typename Data>
	HRESULT SVTree<Key, Data>::getData( const SVTreeContainer& rTree, const Key& rKey, ElementData& rData )
	{
		HRESULT Result( E_FAIL );

		const auto Iter( find( rTree, rKey ) );

		if( rTree.end() != Iter && nullptr != Iter->second )
		{
			rData = *Iter->second;
			Result = S_OK;
		}

		return Result;
	}

	template<typename Key, typename Data>
	typename SVTree<Key, Data>::iterator SVTree<Key, Data>::find( SVTreeContainer& rTree, const Key& rKey )
	{
		auto Result = std::find_if( rTree.begin(), rTree.end(), SVCompareKeys<Key, std::shared_ptr<Data>>(rKey) );

		return Result;
	}

	template<typename Key, typename Data>
	typename SVTree<Key, Data>::const_iterator SVTree<Key, Data>::find( const SVTreeContainer& rTree, const Key& rKey )
	{
		const auto Result = std::find_if( rTree.begin(), rTree.end(), SVCompareKeys<Key, std::shared_ptr<Data>>(rKey) );

		return Result;
	}
} //namespace SvCl
