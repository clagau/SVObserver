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

#ifndef INCL_SVCONFIGURATIONTREEWRITER_INL
#define INCL_SVCONFIGURATIONTREEWRITER_INL

#include <map>
#include <boost/foreach.hpp>
#include <boost/assign/list_of.hpp>
#include "SVUtilityLibrary/SVStringConversions.h"
#include "SVConfigurationLibrary/SVConfigurationTags.h"
#include "SVXMLLibrary/SVNavigateTree.h"
#include "SVConfigurationTreeWriter.h"

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
	//m_elements[0]->attr(pAttrName.c_str(), VariantToString(value));
}

#endif

//******************************************************************************
//* LOG HISTORY:
//******************************************************************************
/*
$Log:   N:\PVCSarch65\ProjectFiles\archives\SVObserver_src\SVObserver\SVConfigurationTreeWriter.inl_v  $
 * 
 *    Rev 1.0   23 Apr 2013 10:03:16   bWalter
 * Project:  SVObserver
 * Change Request (SCR) nbr:  814
 * SCR Title:  Upgrade SVObserver to Compile Using Visual Studio 2010
 * Checked in by:  bWalter;  Ben Walter
 * Change Description:  
 *   Initial check in to SVObserver_src.  (Merged with svo_src label SVO 6.10 Beta 008.)
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.1   11 Oct 2012 10:54:18   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  602
 * SCR Title:  Revise the Toolset Parsing and Object Creation Methodology
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated include information based on the navigate tree class moving to the XML library.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.0   28 Sep 2012 14:28:10   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  602
 * SCR Title:  Revise the Toolset Parsing and Object Creation Methodology
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Migrated object to templates.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.2   25 Sep 2012 15:16:42   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  602
 * SCR Title:  Revise the Toolset Parsing and Object Creation Methodology
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated to use new tree type.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.1   18 Sep 2012 18:19:14   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  602
 * SCR Title:  Revise the Toolset Parsing and Object Creation Methodology
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated code to use the templated XML objects that moved from the SVLibrary to the new XML Library and to the Configuration Library.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.0   04 Sep 2012 14:00:54   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  602
 * SCR Title:  Revise the Toolset Parsing and Object Creation Methodology
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Initial check-in of new files.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
*/