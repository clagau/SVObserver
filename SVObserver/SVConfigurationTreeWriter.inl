//******************************************************************************
//* COPYRIGHT (c) 2012 by Seidenader Vision, Inc.
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVObjectWriter
//* .File Name       : $Workfile:   SVConfigurationTreeWriter.inl  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.0  $
//* .Check In Date   : $Date:   23 Apr 2013 10:03:16  $
//******************************************************************************

#pragma once

#pragma region Includes
#include <map>
#include <boost/foreach.hpp>
#include <boost/assign/list_of.hpp>
#include "SVUtilityLibrary/SVStringConversions.h"
#include "SVConfigurationLibrary/SVConfigurationTags.h"
#include "SVXMLLibrary/SVNavigateTree.h"
#include "SVConfigurationTreeWriter.h"
#pragma endregion

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
	if (nullptr != pName && (value.vt != VT_NULL || value.vt != VT_EMPTY))
	{
		SVTreeType::SVBranchHandle hItem = m_nodes[0];
		_variant_t v(value);
		
		SVNavigateTree::AddItem(m_rTree, hItem, pName, v);
	}
	else
	{
		// throw an exception
	}
}

template< typename SVTreeType >
void SVConfigurationTreeWriter< SVTreeType >::WriteAttribute(LPCTSTR pName, const SVVariantList& rValues)
{
	BOOST_FOREACH(variant_t value, rValues)
	{
		WriteAttribute(pName, value);
	}
}

template< typename SVTreeType >
void SVConfigurationTreeWriter< SVTreeType >::StartElement(LPCTSTR pName)
{
	SVTreeType::SVBranchHandle hItem;
	SVTreeType::SVBranchHandle hParentItem = (m_nodes.size()) ? m_nodes[0] : m_htiParent;
	SVNavigateTree::AddBranch(m_rTree, hParentItem, pName, &hItem);
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
