//******************************************************************************
//* COPYRIGHT (c) 2014 by Seidenader
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : ObjectSelectorPpg
//* .File Name       : $Workfile:   ObjectSelectorPpg.inl  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.0  $
//* .Check In Date   : $Date:   17 Jul 2014 11:16:24  $
//******************************************************************************

#pragma region Public Methods
SVTreeLibrary::ObjectTreeItems& ObjectSelectorPpg::getTreeContainer() const
{
	return m_rTreeContainer;
}

SVString ObjectSelectorPpg::getNodeLocation() const
{
	return SVString(m_NodeLocation);
}

void ObjectSelectorPpg::setNodeLocation( const SVString& rNodeLocation )
{
	m_NodeLocation = rNodeLocation.c_str();
	UpdateData( FALSE );
}
#pragma endregion Public Methods

//******************************************************************************
//* LOG HISTORY:
//******************************************************************************
/*
$Log:   N:\PVCSarch65\ProjectFiles\archives\SVObserver_SRC\ObjectSelectorLibrary\ObjectSelectorPpg.inl_v  $
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
