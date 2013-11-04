//******************************************************************************
//* COPYRIGHT (c) 2012 by Seidenader Vision, Inc.
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVGetObjectListVisitor
//* .File Name       : $Workfile:   SVGetObjectListVisitor.inl  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.0  $
//* .Check In Date   : $Date:   22 Apr 2013 16:35:48  $
//******************************************************************************

template<typename Filter, typename Insertor>
SVGetObjectListVisitor<Filter, Insertor>::SVGetObjectListVisitor(Filter p_Allow, Insertor p_Insertor)
: m_Allow(p_Allow)
, m_insertor(p_Insertor)
{
}

template<typename Filter, typename Insertor>
SVGetObjectListVisitor<Filter, Insertor>::~SVGetObjectListVisitor()
{
}

template<typename Filter, typename Insertor>
HRESULT SVGetObjectListVisitor<Filter, Insertor>::VisitElement(SVObjectClass& p_rElement)
{
	if (m_Allow(p_rElement))
	{
		*m_insertor = p_rElement;
	}
	return S_OK;
}

//******************************************************************************
//* LOG HISTORY:
//******************************************************************************
/*
$Log:   N:\PVCSarch65\ProjectFiles\archives\SVObserver_src\SVObjectLibrary\SVGetObjectListVisitor.inl_v  $
 * 
 *    Rev 1.0   22 Apr 2013 16:35:48   bWalter
 * Project:  SVObserver
 * Change Request (SCR) nbr:  814
 * SCR Title:  Upgrade SVObserver to Compile Using Visual Studio 2010
 * Checked in by:  bWalter;  Ben Walter
 * Change Description:  
 *   Initial check in to SVObserver_src.  (Merged with svo_src label SVO 6.10 Beta 008.)
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.0   30 Jul 2012 12:32:16   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  769
 * SCR Title:  Fix Problems and Crashes with Inspection Document Display Updates
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated object manager to change the functionality for the object visitor.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
*/