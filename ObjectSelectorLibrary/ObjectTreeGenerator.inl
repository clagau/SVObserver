//******************************************************************************
//* COPYRIGHT (c) 2014 by Seidenader
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : ObjectTreeGenerator
//* .File Name       : $Workfile:   ObjectTreeGenerator.inl  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.0  $
//* .Check In Date   : $Date:   17 Jul 2014 11:16:26  $
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

inline const SVTreeLibrary::IObjectSelectorItem& ObjectTreeGenerator::getSingleObjectResult() const
{
	return m_SingleObjectResult;
}

inline void ObjectTreeGenerator::setSelectorType( const SelectorTypeEnum& rSelectorType )
{
	m_SelectorType = rSelectorType;
}
#pragma endregion Public Methods

//******************************************************************************
//* LOG HISTORY:
//******************************************************************************
/*
$Log:   N:\PVCSarch65\ProjectFiles\archives\SVObserver_SRC\ObjectSelectorLibrary\ObjectTreeGenerator.inl_v  $
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
