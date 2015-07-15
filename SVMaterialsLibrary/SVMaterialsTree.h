//******************************************************************************
//* COPYRIGHT (c) 2008 by Seidenader Vision Inc., Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVMaterialsTree
//* .File Name       : $Workfile:   SVMaterialsTree.h  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.0  $
//* .Check In Date   : $Date:   22 Apr 2013 14:15:00  $
//******************************************************************************

#ifndef SVMATERIALSTREE_H
#define SVMATERIALSTREE_H

#include "SVMaterials.h"
#include "SVMaterialsTreeAdapter.h"

#include "SVTreeLibrary/SVTree.h"

/**
@SVObjectName Materials Tree

@SVObjectOverview This file defines several types related to tree traversal and the interface to the Materials Tree class.

@SVObjectOperations Available operations include multiple constructors, a virtual destructor, a find function, and multiple get child tree functions.

*/
class SVMaterialsTree :	public SVTree< SVMaterialsTreeAdapter >
{
public:
	typedef SVTree< SVMaterialsTreeAdapter > SVTreeBase;
	typedef SVTreeBase::sv_tree_iterator iterator;
	typedef SVTreeBase::sv_tree_const_iterator const_iterator;

	typedef SVTreeBase::SVTreeElement SVMaterialsElement;
	typedef SVTreeBase::SVTreeContainer SVTreeContainer;

	SVMaterialsTree();
	SVMaterialsTree( const SVTreeContainer& l_rTree );

	virtual ~SVMaterialsTree();

};

#endif // #ifndef SVMATERIALSTREE_H

//******************************************************************************
//* LOG HISTORY:
//******************************************************************************
/*
$Log:   N:\PVCSarch65\ProjectFiles\archives\SVObserver_SRC\SVMaterialsLibrary\SVMaterialsTree.h_v  $
 * 
 *    Rev 1.0   22 Apr 2013 14:15:00   bWalter
 * Project:  SVObserver
 * Change Request (SCR) nbr:  814
 * SCR Title:  Upgrade SVObserver to Compile Using Visual Studio 2010
 * Checked in by:  bWalter;  Ben Walter
 * Change Description:  
 *   Initial check in to SVObserver_src.  (Merged with svo_src label SVO 6.10 Beta 008.)
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.0   17 Nov 2008 07:23:14   tbair
 * Project:  SVObserver
 * Change Request (SCR) nbr:  615
 * SCR Title:  Integrate PLC Classes into SVObserver Outputs
 * Checked in by:  tBair;  Tom Bair
 * Change Description:  
 *   Initial Check-in
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
*/

