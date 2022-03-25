//******************************************************************************
//* COPYRIGHT (c) 2008 by Körber Pharma Inspection GmbH. All Rights Reserved
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVCompareKeys
//* .File Name       : $Workfile:   SVCompareKeys.h  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.0  $
//* .Check In Date   : $Date:   25 Apr 2013 18:49:10  $
//******************************************************************************

#pragma once

namespace SvCl
{
	template<typename Key, typename Data>
	struct SVCompareKeys
	{
		typedef std::pair<Key, Data > Element;

	public:
		explicit SVCompareKeys(const Key& rElementKey) : m_elementKey(rElementKey) {}
	
		bool operator()(const Element& rElement) const 
		{
			return (m_elementKey == rElement.first);
		}
	private:
		Key m_elementKey;
	};
} //namespace SvCl



