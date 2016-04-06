//******************************************************************************
//* COPYRIGHT (c) 2012 by Seidenader Vision, Inc.
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVConfigurationTreeWriter
//* .File Name       : $Workfile:   SVConfigurationTreeWriter.h  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.0  $
//* .Check In Date   : $Date:   23 Apr 2013 10:03:08  $
//******************************************************************************
#pragma once

#pragma region Includes

//Moved to precompiled header: #include <map>
//Moved to precompiled header: #include <boost/foreach.hpp>
//Moved to precompiled header: #include <boost/assign/list_of.hpp>
#include "SVUtilityLibrary/SVStringConversions.h"
#include "SVConfigurationLibrary/SVConfigurationTags.h"
#include "SVXMLLibrary/SVNavigateTree.h"
#include "SVConfigurationTreeWriter.h"
#include "SVObjectLibrary/SVObjectWriter.h"
#pragma endregion Includes

template< typename SVTreeType >
class SVConfigurationTreeWriter : public SVObjectWriter
{
private:
	typedef std::deque<typename SVTreeType::SVBranchHandle> TreeNodes;
	TreeNodes m_nodes;

	typename SVTreeType::SVBranchHandle m_htiParent;
	SVTreeType& m_rTree;

public:
	SVConfigurationTreeWriter(SVTreeType &rTree, typename SVTreeType::SVBranchHandle htiParent);
	virtual ~SVConfigurationTreeWriter();

	virtual void WriteAttribute(LPCTSTR pName, const _variant_t& value) override;
	virtual void WriteAttribute(LPCTSTR pName, const SVVariantList& rValues) override;

	virtual void StartElement(LPCTSTR pName) override;
	virtual void EndElement();
	virtual void ElementAttribute(LPCTSTR pAttrName, const variant_t& value) override;
};

#include "SVConfigurationTreeWriter.inl"

