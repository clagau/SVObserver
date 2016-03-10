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
#pragma region Includes
#include <assert.h>
#pragma endregion Includes

#pragma region Public Methods
inline const  SelectorItemVector& ObjectTreeGenerator::getSelectedObjects() const
{
	return m_SelectedObjects;
}

inline const  SelectorItemVector& ObjectTreeGenerator::getModifiedObjects() const
{
	return m_ModifiedObjects;
}

inline SelectorItem ObjectTreeGenerator::getSingleObjectResult() const
{
	//If Single select then it is the first result
	if( 0 < m_SelectedObjects.size() )
	{
		return m_SelectedObjects[0];
	}
	return SelectorItem();
}

inline void ObjectTreeGenerator::setSelectorType( const SelectorTypeEnum& rSelectorType )
{
	m_SelectorType = rSelectorType;
	//Reset the leaf count here as this method is called at the start of inserting the objects into the selector
	m_LeafCount = 0;
}

template <typename GlobalSelector, typename PPQSelector, typename ToolsetItemSelector>
void ObjectTreeGenerator::BuildSelectableItems( const SelectorOptions& rOptions )
{
	GlobalSelector globalSelector;
	PPQSelector ppqSelector;
	ToolsetItemSelector toolsetItemSelector;

	m_AttributesFilter = rOptions.getAttributesFilter();
	SvOi::ISelectorItemVectorPtr GlobalList = globalSelector( m_AttributesFilter );
	const SelectorItemVector* pGlobalList = static_cast<const SelectorItemVector*>(GlobalList.get());
	if (nullptr != pGlobalList)
	{
		insertTreeObjects( *pGlobalList );
	}

	SvOi::ISelectorItemVectorPtr PpqList = ppqSelector( rOptions.getInspectionID(), m_AttributesFilter );
	const SelectorItemVector* pPPQList = static_cast<const SelectorItemVector*>(PpqList.get());
	if (nullptr != pPPQList)
	{
		insertTreeObjects( *pPPQList );
	}

	SvOi::ISelectorItemVectorPtr ToolsetList = toolsetItemSelector( rOptions );
	const SelectorItemVector* pToolsetList = static_cast<const SelectorItemVector*>(ToolsetList.get());
	if (nullptr != pToolsetList)
	{
		insertTreeObjects( *pToolsetList );
	}
	else
	{
		assert(false); // Shouldn't happen...
	}
}
#pragma endregion Public Methods

//******************************************************************************
//* LOG HISTORY:
//******************************************************************************
/*
$Log:   N:\PVCSarch65\ProjectFiles\archives\SVObserver_SRC\ObjectSelectorLibrary\ObjectTreeGenerator.inl_v  $
 * 
 *    Rev 1.1   04 Dec 2014 03:34:44   gramseier
 * Project:  SVObserver
 * Change Request (SCR) nbr:  965
 * SCR Title:  Update Object Selector Text Label; Update Icons; Add List Output
 * Checked in by:  gRamseier;  Guido Ramseier
 * Change Description:  
 *   Added Methds getResults
 * Changed getSingleObjectResult gets the first item of the results list
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
