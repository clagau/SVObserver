//******************************************************************************
//* COPYRIGHT (c) 2008 by Seidenader Vision Inc., Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVTree
//* .File Name       : $Workfile:   SVTree.h  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.0  $
//* .Check In Date   : $Date:   25 Apr 2013 18:49:24  $
//******************************************************************************

#ifndef INCL_SVTREE_H
#define INCL_SVTREE_H

/**
@SVObjectName Tree

@SVObjectOverview This file defines the SVAdapter type and the interface to the SVTree class.

@SVObjectOperations Available operations include a constructor and a virtual destructor.

*/
template< class SVT_TREE_ADAPTER >
class SVTree :
	public SVT_TREE_ADAPTER
{
public:
	typedef SVT_TREE_ADAPTER SVAdapter;
	typedef typename SVAdapter::SVTreeContainer SVTreeContainer;

	SVTree( bool bUseUniqueKey = false );
	SVTree( const SVTree& p_rTree );
	SVTree operator=( const SVTree& p_rTree );
	virtual ~SVTree();

protected:
	explicit SVTree( const SVTreeContainer& l_rTree, bool bUseUniqueKey = false );

	SVTreeContainer m_tree;

private:
	void copy( const SVTree& p_rTree );
};

#include "SVTree.inl"

#endif

//******************************************************************************
//* LOG HISTORY:
//******************************************************************************
/*
$Log:   N:\PVCSarch65\ProjectFiles\archives\SVObserver_src\SVTreeLibrary\SVTree.h_v  $
 * 
 *    Rev 1.0   25 Apr 2013 18:49:24   bWalter
 * Project:  SVObserver
 * Change Request (SCR) nbr:  814
 * SCR Title:  Upgrade SVObserver to Compile Using Visual Studio 2010
 * Checked in by:  bWalter;  Ben Walter
 * Change Description:  
 *   Initial check in to SVObserver_src.  (Merged with svo_src label SVO 6.10 Beta 008.)
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.1   05 Dec 2008 15:27:40   sjones
 * Project:  SVObserver
 * Change Request (SCR) nbr:  634
 * SCR Title:  Implement a File Acquistion Device
 * Checked in by:  sJones;  Steve Jones
 * Change Description:  
 *   Revised to upgrade to TCL 5.3.0
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.0   17 Nov 2008 07:24:46   tbair
 * Project:  SVObserver
 * Change Request (SCR) nbr:  615
 * SCR Title:  Integrate PLC Classes into SVObserver Outputs
 * Checked in by:  tBair;  Tom Bair
 * Change Description:  
 *   Initial Check-in
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
*/
