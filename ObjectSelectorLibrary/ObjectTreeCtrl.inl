//******************************************************************************
//* COPYRIGHT (c) 2014 by Seidenader
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : ObjectTreeCtrl
//* .File Name       : $Workfile:   ObjectTreeCtrl.inl  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.3  $
//* .Check In Date   : $Date:   04 Dec 2014 03:23:44  $
//******************************************************************************

namespace SvOsl
{
	#pragma region Public Methods
	inline bool ObjectTreeCtrl::isSingleSelect() const
	{
		return m_SingleSelect;
	}
	#pragma endregion Public Methods

	#pragma region Protected Methods
	inline ObjectSelectorPpg& ObjectTreeCtrl::getParentPropPage() const
	{
		return m_rParent;
	}

	inline const CPoint& ObjectTreeCtrl::getContextPoint() const
	{
		return m_ContextPoint;
	}

	inline void ObjectTreeCtrl::setCurrentSelection( const std::string& rCurrentSelection )
	{
		m_CurrentSelection = rCurrentSelection;
	}

	inline SvDef::StringSet& ObjectTreeCtrl::getUpdateItems() const
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

} //namespace SvOsl
