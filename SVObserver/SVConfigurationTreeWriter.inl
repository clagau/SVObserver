//******************************************************************************
//* COPYRIGHT (c) 2012 by K�rber Pharma Inspection GmbH. All Rights Reserved
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVConfigurationTreeWriter
//* .File Name       : $Workfile:   SVConfigurationTreeWriter.inl  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.0  $
//* .Check In Date   : $Date:   23 Apr 2013 10:03:16  $
//******************************************************************************

#pragma once

#pragma region Includes
#include "SVXMLLibrary/SVNavigateTree.h"
#pragma endregion Includes

template< typename SVTreeType >
SVConfigurationTreeWriter< SVTreeType >::SVConfigurationTreeWriter(SVTreeType &rTree, typename SVTreeType::SVBranchHandle htiParent)
: m_rTree(rTree), m_htiParent(htiParent)
{
}

template< typename SVTreeType >
SVConfigurationTreeWriter< SVTreeType >::~SVConfigurationTreeWriter()
{
}

template< typename SVTreeType >
void SVConfigurationTreeWriter< SVTreeType >::WriteAttribute(LPCTSTR pName, const _variant_t& value)
{
	if (nullptr != pName && (VT_NULL != value.vt || VT_EMPTY != value.vt))
	{
		typename SVTreeType::SVBranchHandle hItem = m_nodes[0];
		_variant_t v(value);
		
		SvXml::SVNavigateTree::AddItem(m_rTree, hItem, pName, v);
	}
	else
	{
		// throw an exception
	}
}

template< typename SVTreeType >
void SVConfigurationTreeWriter< SVTreeType >::WriteAttribute(LPCTSTR pName, const std::vector<_variant_t>& rValues)
{
	for(const auto& rValue : rValues)
	{
		WriteAttribute(pName, rValue);
	}
}

template< typename SVTreeType >
void SVConfigurationTreeWriter< SVTreeType >::StartElement(LPCTSTR pName)
{
	typename SVTreeType::SVBranchHandle hItem;
	typename SVTreeType::SVBranchHandle hParentItem = (m_nodes.size()) ? m_nodes[0] : m_htiParent;
	SvXml::SVNavigateTree::AddBranch(m_rTree, hParentItem, pName, &hItem);
	m_nodes.push_front(hItem);
}

template< typename SVTreeType >
void SVConfigurationTreeWriter< SVTreeType >::EndElement()
{
	m_nodes.pop_front();
}

template< typename SVTreeType >
void SVConfigurationTreeWriter< SVTreeType >::ElementAttribute(LPCTSTR pAttrName, const variant_t& value)
{
	WriteAttribute(pAttrName, value);
}
