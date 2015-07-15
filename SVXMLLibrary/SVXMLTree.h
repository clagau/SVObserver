//******************************************************************************
//* COPYRIGHT (c) 2008 by Seidenader Vision Inc., Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVXMLTree
//* .File Name       : $Workfile:   SVXMLTree.h  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.0  $
//* .Check In Date   : $Date:   25 Apr 2013 19:57:44  $
//******************************************************************************

#ifndef SVXMLTREE_H
#define SVXMLTREE_H

/**
@SVObjectName XML Tree Base

@SVObjectOverview This class is a base class from which to derive tree classes that will interact with the SVR XML classes (SVXMLClass, and SVDOMClass).  The derived class and associated tree structure must have a root node defined (same type as other nodes, not a list).  This root node may contain children, but may not contain data. Zero is not allowed to be a legitimate tree handle.  A handle of zero will represent no handle.... as in could not be found or allocated.

@SVObjectOperations

*/
template<typename SVT_TREE, typename SVT_BRANCH_HANDLE, typename SVT_LEAF_HANDLE>
class SVXMLTree  
{
public:
	typedef SVT_BRANCH_HANDLE SVBranchHandle;
	typedef SVT_LEAF_HANDLE SVLeafHandle;

	SVXMLTree();

	virtual ~SVXMLTree();

	HRESULT	IncrementRefCount();

	HRESULT	DecrementRefCount();

	virtual HRESULT	Clear();

	virtual size_t GetCount() const = 0;

	virtual HRESULT	GetRoot( SVBranchHandle& p_rRoot ) = 0;

	virtual HRESULT	IsRoot( const SVBranchHandle& p_rBranch ) = 0;

	virtual HRESULT	DoesBranchHaveBranches( const SVBranchHandle& p_rBranch ) = 0;

	virtual HRESULT	GetParentBranch( const SVBranchHandle& p_rChild, SVBranchHandle& p_rParent ) = 0;

	virtual HRESULT	GetFirstBranch( const SVBranchHandle& p_rParent, SVBranchHandle& p_rChild ) = 0;

	virtual HRESULT	GetNextBranch( const SVBranchHandle& p_rParent, SVBranchHandle& p_rChild ) = 0;

	virtual HRESULT	FindBranch( const SVBranchHandle& p_rParent, const BSTR& p_rName, SVBranchHandle& p_rChild ) = 0;

	virtual HRESULT	IsValidBranch( const SVBranchHandle& p_rParent ) = 0;

	virtual HRESULT	CreateBranch( const SVBranchHandle& p_rParent, const BSTR& p_rName, SVBranchHandle* p_pChild = NULL ) = 0;

	virtual HRESULT DeleteBranch( SVBranchHandle& p_rBranch ) = 0;

	virtual HRESULT	GetBranchName( const SVBranchHandle& p_rBranch, BSTR& p_rName ) = 0;

	virtual HRESULT	DoesBranchHaveLeaves( const SVBranchHandle& p_rBranch ) = 0;

	virtual HRESULT	GetFirstLeaf( const SVBranchHandle& p_rParent, SVLeafHandle& p_rChild ) = 0;

	virtual HRESULT	GetNextLeaf( const SVBranchHandle& p_rParent, SVLeafHandle& p_rChild ) = 0;

	virtual HRESULT	FindLeaf( const SVBranchHandle& p_rParent, const BSTR& p_rName, SVLeafHandle& p_rChild ) = 0;

	virtual HRESULT	IsValidLeaf( const SVBranchHandle& p_rParent, const SVLeafHandle& p_rLeaf ) = 0;

	virtual HRESULT	CreateLeaf( const SVBranchHandle& p_rParent, const BSTR& p_rName, const VARIANT& p_rData, SVLeafHandle* p_pChild = NULL ) = 0;

	virtual HRESULT DeleteLeaf( const SVBranchHandle& p_rParent, SVLeafHandle& p_rChild ) = 0;

	virtual HRESULT	GetLeafName( const SVLeafHandle& p_rLeaf, BSTR& p_rName ) = 0;

	virtual HRESULT	GetLeafData( const SVLeafHandle& p_rLeaf, VARIANT& p_rData ) = 0;

	virtual HRESULT	SetLeafData( const SVLeafHandle& p_rLeaf, const VARIANT& p_rData ) = 0;

protected:
	// svmlRefCount - used by IncrementRefCount () and DecrementRefCount ().
	long svmlRefCount;

};

#include "SVXMLTree.inl"

#endif // #ifndef SVTREE_H

//******************************************************************************
//* LOG HISTORY:
//******************************************************************************
/*
$Log:   N:\PVCSarch65\ProjectFiles\archives\SVObserver_src\SVXMLLibrary\SVXMLTree.h_v  $
 * 
 *    Rev 1.0   25 Apr 2013 19:57:44   bWalter
 * Project:  SVObserver
 * Change Request (SCR) nbr:  814
 * SCR Title:  Upgrade SVObserver to Compile Using Visual Studio 2010
 * Checked in by:  bWalter;  Ben Walter
 * Change Description:  
 *   Initial check in to SVObserver_src.  (Merged with svo_src label SVO 6.10 Beta 008.)
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.2   11 Oct 2012 10:40:12   jspila
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
 *    Rev 1.0   18 Sep 2012 18:12:50   jspila
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