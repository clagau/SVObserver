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
#ifndef INCL_SVCONFIGURATIONTREEWRITER_H
#define INCL_SVCONFIGURATIONTREEWRITER_H

#include "SVObjectLibrary/SVObjectWriter.h"

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

	virtual void WriteAttribute(const SVString& rName, const _variant_t& value);
	virtual void WriteAttribute(const SVString& rName, const SVVariantList& rValues);

	virtual void StartElement(const SVString& rName);
	virtual void EndElement();
	virtual void ElementAttribute(const SVString& rAttrName, const variant_t& value);
};

#include "SVConfigurationTreeWriter.inl"

#endif

//******************************************************************************
//* LOG HISTORY:
//******************************************************************************
/*
$Log:   N:\PVCSarch65\ProjectFiles\archives\SVObserver_src\SVObserver\SVConfigurationTreeWriter.h_v  $
 * 
 *    Rev 1.0   23 Apr 2013 10:03:08   bWalter
 * Project:  SVObserver
 * Change Request (SCR) nbr:  814
 * SCR Title:  Upgrade SVObserver to Compile Using Visual Studio 2010
 * Checked in by:  bWalter;  Ben Walter
 * Change Description:  
 *   Initial check in to SVObserver_src.  (Merged with svo_src label SVO 6.10 Beta 008.)
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.2   28 Sep 2012 14:28:10   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  602
 * SCR Title:  Revise the Toolset Parsing and Object Creation Methodology
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Migrated object to templates.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.1   25 Sep 2012 15:16:40   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  602
 * SCR Title:  Revise the Toolset Parsing and Object Creation Methodology
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated to use new tree type.
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