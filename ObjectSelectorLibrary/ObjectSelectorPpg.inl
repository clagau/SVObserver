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

namespace SvOsl
{
	#pragma region Public Methods
	SvCl::ObjectTreeItems& ObjectSelectorPpg::getTreeContainer() const
	{
		return m_rTreeContainer;
	}

	std::string ObjectSelectorPpg::getHighlightedNode() const
	{
		return m_HighlightedNode;
	}

	void ObjectSelectorPpg::setHighlightedNode( const std::string& rHighlightedNode )
	{
		m_HighlightedNode = rHighlightedNode;
		UpdateData( FALSE );
	}

	void ObjectSelectorPpg::setHelpID( int HelpID )
	{
		m_HelpID = HelpID;
	}
	#pragma endregion Public Methods

} //namespace SvOsl
