//******************************************************************************
//* COPYRIGHT (c) 2014 by Seidenader
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : ObjectSelectorPpg
//* .File Name       : $Workfile:   ObjectSelectorPpg.inl  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.2  $
//* .Check In Date   : $Date:   04 Dec 2014 03:13:50  $
//******************************************************************************

#pragma region Public Methods
SVTreeLibrary::ObjectTreeItems& ObjectSelectorPpg::getTreeContainer() const
{
	return m_rTreeContainer;
}

SVString ObjectSelectorPpg::getHighlightedNode() const
{
	return SVString( m_HighlightedNode );
}

void ObjectSelectorPpg::setHighlightedNode( const SVString& rHighlightedNode )
{
	m_HighlightedNode = rHighlightedNode.c_str();
	UpdateData( FALSE );
}

void ObjectSelectorPpg::setHelpID( int HelpID )
{
	m_HelpID = HelpID;
}
#pragma endregion Public Methods

//******************************************************************************
//* LOG HISTORY:
//******************************************************************************
/*
$Log:   N:\PVCSarch65\ProjectFiles\archives\SVObserver_SRC\ObjectSelectorLibrary\ObjectSelectorPpg.inl_v  $
 * 
 *    Rev 1.2   04 Dec 2014 03:13:50   gramseier
 * Project:  SVObserver
 * Change Request (SCR) nbr:  965
 * SCR Title:  Update Object Selector Text Label; Update Icons; Add List Output
 * Checked in by:  gRamseier;  Guido Ramseier
 * Change Description:  
 *   Changed field name from Location to Highlighted Node
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.1   08 Sep 2014 09:44:14   gramseier
 * Project:  SVObserver
 * Change Request (SCR) nbr:  909
 * SCR Title:  Object Selector replacing Result Picker and Output Selector SVO-72, 40, 130
 * Checked in by:  gRamseier;  Guido Ramseier
 * Change Description:  
 *   Object Selector property page has a variable context ID
 * Added methods: setHelpID
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.0   17 Jul 2014 11:16:24   gramseier
 * Project:  SVObserver
 * Change Request (SCR) nbr:  909
 * SCR Title:  Object Selector replacing Result Picker and Output Selector SVO-72, 40, 130
 * Checked in by:  gRamseier;  Guido Ramseier
 * Change Description:  
 *   Initial check in.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
*/
