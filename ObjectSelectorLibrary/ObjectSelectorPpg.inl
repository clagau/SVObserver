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
SvTrl::ObjectTreeItems& ObjectSelectorPpg::getTreeContainer() const
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

