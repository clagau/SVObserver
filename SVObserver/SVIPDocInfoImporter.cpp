//******************************************************************************
//* COPYRIGHT (c) 2012 by Seidenader Vision, Inc. Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVIPDocInfoImporter
//* .File Name       : $Workfile:   SVIPDocInfoImporter.cpp  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.0  $
//* .Check In Date   : $Date:   23 Apr 2013 11:17:06  $
//******************************************************************************
#include "stdafx.h"
#include "SVConfigurationLibrary/SVConfigurationTags.h"
#include "SVIPDocInfoImporter.h"
#include "SVXMLLibrary/SVXMLMaterialsTree.h"
#include "SVXMLLibrary/SVNavigateTree.h"
#include "SVIPDoc.h"
#include "SVImportedInspectionInfo.h"

void SVIPDocInfoImporter::Import(SVIPDoc* pDoc, const SVImportedInspectionInfo& info)
{
	// Copy from Materials Tree
	SvXml::SVXMLMaterialsTree l_Tree( info.m_materialsTree );
	
	SvXml::SVXMLMaterialsTree::SVBranchHandle hItem( l_Tree.getRoot() );
	
	if( l_Tree.isValidBranch( hItem ) )
	{
		pDoc->SetParameters(l_Tree, hItem);
	}
}
//******************************************************************************
//* LOG HISTORY:
//******************************************************************************
/*
$Log:   N:\PVCSarch65\ProjectFiles\archives\SVObserver_src\SVObserver\SVIPDocInfoImporter.cpp_v  $
 * 
 *    Rev 1.0   23 Apr 2013 11:17:06   bWalter
 * Project:  SVObserver
 * Change Request (SCR) nbr:  814
 * SCR Title:  Upgrade SVObserver to Compile Using Visual Studio 2010
 * Checked in by:  bWalter;  Ben Walter
 * Change Description:  
 *   Initial check in to SVObserver_src.  (Merged with svo_src label SVO 6.10 Beta 008.)
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.0   05 Mar 2013 15:30:04   sjones
 * Project:  SVObserver
 * Change Request (SCR) nbr:  817
 * SCR Title:  Import/Export of Inspections
 * Checked in by:  sJones;  Steve Jones
 * Change Description:  
 *   Initial checkin
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
*/
