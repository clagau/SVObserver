//******************************************************************************
//* COPYRIGHT (c) 2014 by Seidenader
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : ObjectTreeGenerator
//* .File Name       : $Workfile:   ObjectTreeGenerator.inl  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.1  $
//* .Check In Date   : $Date:   04 Dec 2014 03:34:44  $
//******************************************************************************

namespace SvOsl
{
	#pragma region Public Methods
	inline const  SvCl::SelectorItemVector& ObjectTreeGenerator::getSelectedObjects() const
	{
		return m_SelectedObjects;
	}

	inline const  SvCl::SelectorItemVector& ObjectTreeGenerator::getModifiedObjects() const
	{
		return m_ModifiedObjects;
	}

	inline SvCl::SelectorItem ObjectTreeGenerator::getSingleObjectResult() const
	{
		//If Single select then it is the first result
		if( 0 < m_SelectedObjects.size() )
		{
			return m_SelectedObjects[0];
		}
		return SvCl::SelectorItem();
	}

	inline void ObjectTreeGenerator::setSelectorType(const SelectorTypeEnum& rSelectorType, UINT attribute)
	{
		m_SelectorType = rSelectorType;
		m_AttributesFilter = attribute;
		//Reset the leaf count here as this method is called at the start of inserting the objects into the selector
		m_LeafCount = 0;
	}

	#pragma endregion Public Methods
} //namespace SvOsl
