//******************************************************************************
//* COPYRIGHT (c) 2014 by Seidenader
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : ObjectTreeCtrl
//* .File Name       : $Workfile:   ObjectTreeCtrl.inl  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.2  $
//* .Check In Date   : $Date:   18 Aug 2014 07:46:20  $
//******************************************************************************

#pragma region Protected Methods
inline ObjectSelectorPpg& ObjectTreeCtrl::getParentPropPage() const
{
	return m_rParent;
}

inline const CPoint& ObjectTreeCtrl::getContextPoint() const
{
	return m_ContextPoint;
}

inline bool ObjectTreeCtrl::isSingleSelect() const
{
	return m_SingleSelect;
}

inline void ObjectTreeCtrl::setCurrentSelection( const SVString& rCurrentSelection )
{
	m_CurrentSelection = rCurrentSelection;
}

inline SVStringSet& ObjectTreeCtrl::getUpdateItems() const
{
	return m_UpdateItems;
}

inline const UINT& ObjectTreeCtrl::getLeftButtonCheckFlag() const
{
	return m_LeftButtonCheckFlag;
}

inline void ObjectTreeCtrl::setLeftButtonCheckFlag( const UINT& rCheckFlag )
{
	m_LeftButtonCheckFlag = rCheckFlag;
}

#pragma endregion Protected Methods

//******************************************************************************
//* LOG HISTORY:
//******************************************************************************
/*
$Log:   N:\PVCSarch65\ProjectFiles\archives\SVObserver_SRC\ObjectSelectorLibrary\ObjectTreeCtrl.inl_v  $
 * 
 *    Rev 1.2   18 Aug 2014 07:46:20   gramseier
 * Project:  SVObserver
 * Change Request (SCR) nbr:  909
 * SCR Title:  Object Selector replacing Result Picker and Output Selector SVO-72, 40, 130
 * Checked in by:  gRamseier;  Guido Ramseier
 * Change Description:  
 *   Coding guidline changes using const object for get and is methods
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.1   12 Aug 2014 12:32:38   gramseier
 * Project:  SVObserver
 * Change Request (SCR) nbr:  909
 * SCR Title:  Object Selector replacing Result Picker and Output Selector SVO-72, 40, 130
 * Checked in by:  gRamseier;  Guido Ramseier
 * Change Description:  
 *   Optional what part of the tree item is clicked for it to toggle
 * Added the methods getLeftButtonCheckFlag and setLeftButtonCheckFlag
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.0   17 Jul 2014 11:16:26   gramseier
 * Project:  SVObserver
 * Change Request (SCR) nbr:  909
 * SCR Title:  Object Selector replacing Result Picker and Output Selector SVO-72, 40, 130
 * Checked in by:  gRamseier;  Guido Ramseier
 * Change Description:  
 *   Initial check in.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
*/
