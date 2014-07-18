//******************************************************************************
//* COPYRIGHT (c) 2014 by Seidenader
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : ObjectTreeCtrl
//* .File Name       : $Workfile:   ObjectTreeCtrl.inl  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.0  $
//* .Check In Date   : $Date:   17 Jul 2014 11:16:26  $
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
#pragma endregion Protected Methods

//******************************************************************************
//* LOG HISTORY:
//******************************************************************************
/*
$Log:   N:\PVCSarch65\ProjectFiles\archives\SVObserver_SRC\ObjectSelectorLibrary\ObjectTreeCtrl.inl_v  $
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
