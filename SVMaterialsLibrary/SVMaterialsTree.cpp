//******************************************************************************
//* COPYRIGHT (c) 2008 by Seidenader Vision Inc., Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVMaterialsTree
//* .File Name       : $Workfile:   SVMaterialsTree.cpp  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.0  $
//* .Check In Date   : $Date:   22 Apr 2013 14:14:54  $
//******************************************************************************

#include "stdafx.h"
#include "SVMaterialsTree.h"

SVMaterialsTree::SVMaterialsTree()
: SVTreeBase()
{
}

SVMaterialsTree::SVMaterialsTree( const SVTreeContainer& l_rTree )
: SVTreeBase( l_rTree )
{
}

SVMaterialsTree::~SVMaterialsTree()
{
}

//******************************************************************************
//* LOG HISTORY:
//******************************************************************************
/*
$Log:   N:\PVCSarch65\ProjectFiles\archives\SVObserver_SRC\SVMaterialsLibrary\SVMaterialsTree.cpp_v  $
 * 
 *    Rev 1.0   22 Apr 2013 14:14:54   bWalter
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

