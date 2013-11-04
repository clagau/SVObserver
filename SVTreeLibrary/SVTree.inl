//******************************************************************************
//* COPYRIGHT (c) 2008 by Seidenader Vision Inc., Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVTree
//* .File Name       : $Workfile:   SVTree.inl  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.0  $
//* .Check In Date   : $Date:   25 Apr 2013 18:49:40  $
//******************************************************************************

///////////////////////////////////////////////////////////////////////
// 
///////////////////////////////////////////////////////////////////////
template<class SVT_TREE_ADAPTER>
SVTree<SVT_TREE_ADAPTER>::SVTree( bool bUseUniqueKey ) 
: SVAdapter( m_tree, bUseUniqueKey )
{
}

///////////////////////////////////////////////////////////////////////
// 
///////////////////////////////////////////////////////////////////////
template<class SVT_TREE_ADAPTER>
SVTree<SVT_TREE_ADAPTER>::~SVTree() 
{ 
	m_tree.clear(); 
}

///////////////////////////////////////////////////////////////////////
// 
///////////////////////////////////////////////////////////////////////
template<class SVT_TREE_ADAPTER>
SVTree<SVT_TREE_ADAPTER>::SVTree( const SVTreeContainer& l_rTree, bool bUseUniqueKey ) 
: SVAdapter( m_tree, bUseUniqueKey ), m_tree( l_rTree )
{
}

template
<
	class SVT_TREE_ADAPTER
>
SVTree<SVT_TREE_ADAPTER>::SVTree( const SVTree& p_rTree )
: SVAdapter( m_tree, p_rTree.m_bUseUniqueKey ), m_tree( p_rTree.m_tree )
{
}

template
<
	class SVT_TREE_ADAPTER
>
SVTree<SVT_TREE_ADAPTER> SVTree<SVT_TREE_ADAPTER>::operator=( const SVTree& p_rTree )
{
	if (this != &p_rTree)
		copy(p_rTree);
	return *this;
}

template
<
	class SVT_TREE_ADAPTER
>
void SVTree<SVT_TREE_ADAPTER>::copy(const SVTree& p_rTree)
{
	m_tree = p_rTree.m_tree;
	m_bUseUniqueKey = p_rTree.m_bUseUniqueKey;
}

//******************************************************************************
//* LOG HISTORY:
//******************************************************************************
/*
$Log:   N:\PVCSarch65\ProjectFiles\archives\SVObserver_src\SVTreeLibrary\SVTree.inl_v  $
 * 
 *    Rev 1.0   25 Apr 2013 18:49:40   bWalter
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
