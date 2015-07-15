//******************************************************************************
//* COPYRIGHT (c) 2008 by Seidenader Vision Inc., Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVMaterialsTreeAdapter
//* .File Name       : $Workfile:   SVMaterialsTreeAdapter.h  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.0  $
//* .Check In Date   : $Date:   22 Apr 2013 14:14:08  $
//******************************************************************************

#ifndef SVMATERIALSTREEADAPTER_H
#define SVMATERIALSTREEADAPTER_H

#include "SVMaterials.h"

#include "SVTreeLibrary/SVTreeAdapter.h"

/**
@SVObjectName Materials Tree Adapter

@SVObjectOverview

@SVObjectOperations

*/
class SVMaterialsTreeAdapter : public SVTreeAdapter< SVString, SVMaterials, SVString >
{
public:
	typedef SVTreeAdapter< SVString, SVMaterials, SVString > SVTreeBase;
	typedef SVTreeBase::sv_tree_iterator iterator;
	typedef SVTreeBase::sv_tree_const_iterator const_iterator;

	typedef SVTreeBase::SVTreeElement SVMaterialsElement;
	typedef SVTreeBase::SVTreeContainer SVTreeContainer;

	static size_t GetCount( const SVMaterialsTreeAdapter& p_rTree );

	SVMaterialsTreeAdapter( SVTreeContainer& p_rTree );

	virtual ~SVMaterialsTreeAdapter();

	iterator FindBranchWithMaterial( const SVMaterials::SVIdentifier& p_rKey, const SVMaterials::SVData& p_rData );
	const_iterator FindBranchWithMaterial( const SVMaterials::SVIdentifier& p_rKey, const SVMaterials::SVData& p_rData ) const;

	SVMaterialsTreeAdapter GetChildTree( iterator& it );
	SVMaterialsTreeAdapter GetChildTree( const_iterator& it ) const;

protected:
	explicit SVMaterialsTreeAdapter( SVTreeContainer& p_rTree, bool bUseUniqueKey );

};

#endif

//******************************************************************************
//* LOG HISTORY:
//******************************************************************************
/*
$Log:   N:\PVCSarch65\ProjectFiles\archives\SVObserver_SRC\SVMaterialsLibrary\SVMaterialsTreeAdapter.h_v  $
 * 
 *    Rev 1.0   22 Apr 2013 14:14:08   bWalter
 * Project:  SVObserver
 * Change Request (SCR) nbr:  814
 * SCR Title:  Upgrade SVObserver to Compile Using Visual Studio 2010
 * Checked in by:  bWalter;  Ben Walter
 * Change Description:  
 *   Initial check in to SVObserver_src.  (Merged with svo_src label SVO 6.10 Beta 008.)
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.1   25 Sep 2012 13:26:56   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  602
 * SCR Title:  Revise the Toolset Parsing and Object Creation Methodology
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Changed storage from map to deque for materials class.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.0   17 Nov 2008 07:23:10   tbair
 * Project:  SVObserver
 * Change Request (SCR) nbr:  615
 * SCR Title:  Integrate PLC Classes into SVObserver Outputs
 * Checked in by:  tBair;  Tom Bair
 * Change Description:  
 *   Initial Check-in
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
*/

