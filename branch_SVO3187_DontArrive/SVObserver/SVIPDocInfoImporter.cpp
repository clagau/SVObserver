//******************************************************************************
//* COPYRIGHT (c) 2012 by Körber Pharma Inspection GmbH. All Rights Reserved Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVIPDocInfoImporter
//* .File Name       : $Workfile:   SVIPDocInfoImporter.cpp  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.0  $
//* .Check In Date   : $Date:   23 Apr 2013 11:17:06  $
//******************************************************************************

#pragma region Includes
#include "stdafx.h"
#include "SVIPDocInfoImporter.h"
#include "SVImportedInspectionInfo.h"
#include "SVIPDoc.h"
#include "SVXMLLibrary/SVXMLMaterialsTree.h"
#pragma endregion Includes

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
