//******************************************************************************
//* COPYRIGHT (c) 2008 by Seidenader Vision Inc., Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVMaterialsTreeAdapter
//* .File Name       : $Workfile:   SVMaterialsTreeAdapter.cpp  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.0  $
//* .Check In Date   : $Date:   22 Apr 2013 14:15:04  $
//******************************************************************************

#include "stdafx.h"
#include "SVMaterialsTreeAdapter.h"

size_t SVMaterialsTreeAdapter::GetCount( const SVMaterialsTreeAdapter& p_rTree )
{
	size_t l_Count = 0;

	const_iterator l_Iter( p_rTree.begin() );

	while( l_Iter != p_rTree.end() )
	{
		SVTreeContainer* l_pChildTree = const_cast< SVTreeContainer* >( l_Iter.GetChildTree() );

		if( l_pChildTree != NULL )
		{
			l_Count += GetCount( *l_pChildTree );
		}

		++l_Iter;
	}

	l_Count += p_rTree.size();
	l_Count += p_rTree.get()->second.size();

	return l_Count;
}

SVMaterialsTreeAdapter::SVMaterialsTreeAdapter( SVTreeContainer& p_rTree )
: SVTreeBase( p_rTree )
{
}

SVMaterialsTreeAdapter::~SVMaterialsTreeAdapter()
{
}

SVMaterialsTreeAdapter::iterator SVMaterialsTreeAdapter::FindBranchWithMaterial( const SVMaterials::SVIdentifier& p_rKey, const SVMaterials::SVData& p_rData )
{
	iterator l_Iter( begin() );

	while( l_Iter != end() )
	{
		SVTreeContainer* l_pChildTree( l_Iter.GetChildTree() );

		if( l_pChildTree != NULL )
		{
			SVMaterials& l_rMaterials( l_pChildTree->get()->second );

			SVMaterials::iterator l_DataIter( l_rMaterials.find( p_rKey ) );

			if( l_DataIter != l_rMaterials.end() )
			{
				if( l_DataIter->second == p_rData )
				{
					break;
				}
			}
		}

		++l_Iter;
	}

	return l_Iter;
}

SVMaterialsTreeAdapter::const_iterator SVMaterialsTreeAdapter::FindBranchWithMaterial( const SVMaterials::SVIdentifier& p_rKey, const SVMaterials::SVData& p_rData ) const
{
	SVMaterialsTreeAdapter* l_pTree( const_cast< SVMaterialsTreeAdapter* >( this ) );

	return l_pTree->FindBranchWithMaterial( p_rKey, p_rData );
}

SVMaterialsTreeAdapter SVMaterialsTreeAdapter::GetChildTree(iterator& it)
{
	return SVMaterialsTreeAdapter( *( it.GetChildTree() ) );
}

SVMaterialsTreeAdapter SVMaterialsTreeAdapter::GetChildTree(const_iterator& it) const
{
	return SVMaterialsTreeAdapter( *( const_cast< SVTreeContainer* >( it.GetChildTree() ) ) );
}

SVMaterialsTreeAdapter::SVMaterialsTreeAdapter( SVTreeContainer& p_rTree, bool bUseUniqueKey )
: SVTreeBase( p_rTree, bUseUniqueKey )
{
}

//******************************************************************************
//* LOG HISTORY:
//******************************************************************************
/*
$Log:   N:\PVCSarch65\ProjectFiles\archives\SVObserver_SRC\SVMaterialsLibrary\SVMaterialsTreeAdapter.cpp_v  $
 * 
 *    Rev 1.0   22 Apr 2013 14:15:04   bWalter
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

