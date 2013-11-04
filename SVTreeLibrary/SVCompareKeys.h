//******************************************************************************
//* COPYRIGHT (c) 2008 by Seidenader Vision Inc., Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVCompareKeys
//* .File Name       : $Workfile:   SVCompareKeys.h  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.0  $
//* .Check In Date   : $Date:   25 Apr 2013 18:49:10  $
//******************************************************************************

#ifndef SVCOMPAREKEYS_H
#define SVCOMPAREKEYS_H

/**
@SVObjectName Compare Keys

@SVObjectOverview

@SVObjectOperations

*/
template<typename ElementKey, typename ElementType>
class SVCompareKeys : public std::unary_function< std::pair<ElementKey, ElementType>, bool>
{
	typedef std::pair<ElementKey, ElementType> Element;

public:
	SVCompareKeys(const ElementKey& elementKey) : m_elementKey(elementKey) {}
	
	bool operator()(const Element& element) const 
	{
		return (m_elementKey == element.first);
	}
private:
	ElementKey m_elementKey;
};

#endif

//******************************************************************************
//* LOG HISTORY:
//******************************************************************************
/*
$Log:   N:\PVCSarch65\ProjectFiles\archives\SVObserver_src\SVTreeLibrary\SVCompareKeys.h_v  $
 * 
 *    Rev 1.0   25 Apr 2013 18:49:10   bWalter
 * Project:  SVObserver
 * Change Request (SCR) nbr:  814
 * SCR Title:  Upgrade SVObserver to Compile Using Visual Studio 2010
 * Checked in by:  bWalter;  Ben Walter
 * Change Description:  
 *   Initial check in to SVObserver_src.  (Merged with svo_src label SVO 6.10 Beta 008.)
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.0   17 Nov 2008 07:24:46   tbair
 * Project:  SVObserver
 * Change Request (SCR) nbr:  615
 * SCR Title:  Integrate PLC Classes into SVObserver Outputs
 * Checked in by:  tBair;  Tom Bair
 * Change Description:  
 *   Initial Check-in
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
*/
