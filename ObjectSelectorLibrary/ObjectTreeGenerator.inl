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

#pragma region Public Methods
inline void ObjectTreeGenerator::setAllowWholeArrays( bool AllowWholeArrays )
{
	m_AllowWholeArray = AllowWholeArrays;
}

inline bool ObjectTreeGenerator::getAllowWholeArrays() const
{
	return m_AllowWholeArray;
}

inline const SvTrl::ObjectSelectorItems& ObjectTreeGenerator::getResults() const
{
	return m_Results;
}

inline SvTrl::ObjectSelectorItem ObjectTreeGenerator::getSingleObjectResult() const
{
	SvTrl::ObjectSelectorItem Result;
	//If Single select then it is the first result
	if( 0 < m_Results.size() )
	{
		Result = m_Results.at(0);
	}
	return Result;
}

inline void ObjectTreeGenerator::setSelectorType( const SelectorTypeEnum& rSelectorType )
{
	m_SelectorType = rSelectorType;
}

///SEJ99 - This method should be refactored to trim down the insert methods in the object selector to only one...
template <typename GlobalSelector, typename PPQSelector, typename PPQVariablesSelector, typename ToolsetItemSelector>
void ObjectTreeGenerator::BuildSelectableItems( const SVGUID& rInspectionID, const SVGUID& rInstanceID )
{
	GlobalSelector globalSelector;
	PPQSelector ppqSelector;
	PPQVariablesSelector ppqVariablesSelector;
	ToolsetItemSelector toolsetItemSelector;

	insertTreeObjects(globalSelector(m_AttributesAllowedFilter));
	insertTreeObjects(ppqSelector(rInspectionID));
	insertTreeObjects(ppqVariablesSelector(rInspectionID));
	SvOi::IOutputInfoListClassPtr outputList = toolsetItemSelector(rInspectionID, rInstanceID);
	if (!outputList.empty())
	{
		insertOutputList(*outputList.get());
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
