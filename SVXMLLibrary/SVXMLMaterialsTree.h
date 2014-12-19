//******************************************************************************
//* COPYRIGHT (c) 2008 by Seidenader Vision Inc., Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVXMLMaterialsTree
//* .File Name       : $Workfile:   SVXMLMaterialsTree.h  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.1  $
//* .Check In Date   : $Date:   19 Dec 2014 04:46:14  $
//******************************************************************************

#ifndef SVXMLMATERIALSTREE_H
#define SVXMLMATERIALSTREE_H

#include "SVXMLTree.h"

#include "SVMaterialsLibrary/SVMaterialsTreeAdapter.h"

class SVXMLMaterialsTree : public SVXMLTree< SVXMLMaterialsTree, SVMaterialsTreeAdapter::SVTreeContainer*, SVMaterials::iterator >  
{
public:
	SVXMLMaterialsTree( SVMaterialsTreeAdapter& p_rTree );
	SVXMLMaterialsTree( SVMaterialsTreeAdapter& p_rTree, SVMaterialsTreeAdapter::SVTreeContainer*& p_rBranch );
	SVXMLMaterialsTree( SVXMLMaterialsTree& p_rTree );
	SVXMLMaterialsTree( SVXMLMaterialsTree& p_rTree, SVXMLMaterialsTree::SVBranchHandle& p_rBranch );

	virtual ~SVXMLMaterialsTree();

	virtual size_t GetCount() const;

	virtual HRESULT	GetRoot( SVBranchHandle& p_rRoot );

	virtual HRESULT	IsRoot( const SVBranchHandle& p_rBranch );

	virtual HRESULT	DoesBranchHaveBranches( const SVBranchHandle& p_rBranch );

	virtual HRESULT	GetParentBranch( const SVBranchHandle& p_rChild, SVBranchHandle& p_rParent );

	virtual HRESULT	GetFirstBranch( const SVBranchHandle& p_rParent, SVBranchHandle& p_rChild );

	virtual HRESULT	GetNextBranch( const SVBranchHandle& p_rParent, SVBranchHandle& p_rChild );

	virtual HRESULT	FindBranch( const SVBranchHandle& p_rParent, const BSTR& p_rName, SVBranchHandle& p_rChild );

	virtual HRESULT	IsValidBranch( const SVBranchHandle& p_rParent );

	virtual HRESULT	CreateBranch( const SVBranchHandle& p_rParent, const BSTR& p_rName, SVBranchHandle* p_pChild = NULL );

	virtual HRESULT DeleteBranch( SVBranchHandle& p_rBranch );

	virtual HRESULT	GetBranchName( const SVBranchHandle& p_rBranch, BSTR& p_rName );

	virtual HRESULT	DoesBranchHaveLeaves( const SVBranchHandle& p_rBranch );

	virtual HRESULT	GetFirstLeaf( const SVBranchHandle& p_rParent, SVLeafHandle& p_rChild );

	virtual HRESULT	GetNextLeaf( const SVBranchHandle& p_rParent, SVLeafHandle& p_rChild );

	virtual HRESULT	FindLeaf( const SVBranchHandle& p_rParent, const BSTR& p_rName, SVLeafHandle& p_rChild );

	virtual HRESULT	IsValidLeaf( const SVBranchHandle& p_rParent, const SVLeafHandle& p_rLeaf );

	virtual HRESULT	CreateLeaf( const SVBranchHandle& p_rParent, const BSTR& p_rName, const VARIANT& p_rData, SVLeafHandle* p_pChild = NULL );

	virtual HRESULT DeleteLeaf( const SVBranchHandle& p_rParent, SVLeafHandle& p_rChild );

	virtual HRESULT	GetLeafName( const SVLeafHandle& p_rLeaf, BSTR& p_rName );

	virtual HRESULT	GetLeafData( const SVLeafHandle& p_rLeaf, VARIANT& p_rData );

	virtual HRESULT	SetLeafData( const SVLeafHandle& p_rLeaf, const VARIANT& p_rData );

	virtual HRESULT	ReplaceName( const SVBranchHandle& rParent, const VARIANT& rSearchName, const VARIANT& rReplaceName );

	virtual HRESULT	getLeafValues( const SVBranchHandle& rParent, const SVString& rSearchName, SVStringSet& rLeafValues );

protected:
	SVMaterialsTreeAdapter m_rTree;

};

#endif // #ifndef SVXMLMATERIALSTREE_H

//******************************************************************************
//* LOG HISTORY:
//******************************************************************************
/*
$Log:   N:\PVCSarch65\ProjectFiles\archives\SVObserver_SRC\SVXMLLibrary\SVXMLMaterialsTree.h_v  $
 * 
 *    Rev 1.1   19 Dec 2014 04:46:14   gramseier
 * Project:  SVObserver
 * Change Request (SCR) nbr:  978
 * SCR Title:  Copy and Paste a Tool within an Inspection or Between Different Inspections
 * Checked in by:  gRamseier;  Guido Ramseier
 * Change Description:  
 *   Method added: ReplaceName and getLeafValues
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.0   25 Apr 2013 19:56:28   bWalter
 * Project:  SVObserver
 * Change Request (SCR) nbr:  814
 * SCR Title:  Upgrade SVObserver to Compile Using Visual Studio 2010
 * Checked in by:  bWalter;  Ben Walter
 * Change Description:  
 *   Initial check in to SVObserver_src.  (Merged with svo_src label SVO 6.10 Beta 008.)
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.2   11 Oct 2012 10:40:10   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  602
 * SCR Title:  Revise the Toolset Parsing and Object Creation Methodology
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated code and projects to fix whitespace issues in the XML file and consolidate functionality for load and saving to this library.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.1   25 Sep 2012 14:57:28   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  602
 * SCR Title:  Revise the Toolset Parsing and Object Creation Methodology
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Added the count method to the tree classes.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.0   18 Sep 2012 18:12:48   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  602
 * SCR Title:  Revise the Toolset Parsing and Object Creation Methodology
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated code to use the templated XML objects that moved from the SVLibrary to the new XML Library.
 * 
 * This code is the refacting of the items from the SVLibrary.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
*/
