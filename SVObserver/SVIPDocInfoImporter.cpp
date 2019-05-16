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
#include "SVXMLLibrary/SVConfigurationTags.h"
#include "SVIPDocInfoImporter.h"
#include "SVXMLLibrary/SVXMLMaterialsTree.h"
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
