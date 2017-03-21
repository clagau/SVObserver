//******************************************************************************
//* COPYRIGHT (c) 2008 by Seidenader Vision Inc., Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVCompareKeys
//* .File Name       : $Workfile:   SVCompareKeys.h  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.0  $
//* .Check In Date   : $Date:   25 Apr 2013 18:49:10  $
//******************************************************************************

#pragma once

namespace Sv { namespace Container
{
	template<typename Key, typename Data>
	class SVCompareKeys : public std::unary_function< std::pair<Key, Data >, bool>
	{
		typedef std::pair<Key, Data > Element;

	public:
		SVCompareKeys(const Key& elementKey) : m_elementKey(elementKey) {}
	
		bool operator()(const Element& element) const 
		{
			return (m_elementKey == element.first);
		}
	private:
		Key m_elementKey;
	};
} /* namespace SVContainerLibrary */ } /* namespace Seidenader */

namespace SvCl = Sv::Container;


