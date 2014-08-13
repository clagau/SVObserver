//******************************************************************************
//* COPYRIGHT (c) 2014 by Seidenader
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : ObjectTreeCtrl
//* .File Name       : $Workfile:   ObjectTreeCtrl.inl  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.1  $
//* .Check In Date   : $Date:   12 Aug 2014 12:32:38  $
//******************************************************************************

#pragma region Protected Methods
inline ObjectSelectorPpg& ObjectTreeCtrl::getParentPropPage()
{
	return m_rParent;
}

inline const CPoint& ObjectTreeCtrl::getContextPoint()
{
	return m_ContextPoint;
}

inline bool ObjectTreeCtrl::isSingleSelect()
{
	return m_SingleSelect;
}

inline void ObjectTreeCtrl::setCurrentSelection( const SVString& rCurrentSelection )
{
	m_CurrentSelection = rCurrentSelection;
}

inline SVStringSet& ObjectTreeCtrl::getUpdateItems()
{
	return m_UpdateItems;
}

inline UINT& ObjectTreeCtrl::getLeftButtonCheckFlag()
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
