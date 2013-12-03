// ******************************************************************************
// * COPYRIGHT (c) 2013 by Seidenader, Harrisburg
// * All Rights Reserved
// ******************************************************************************
// * .Module Name     : SVDataItemManagerTemplate
// * .File Name       : $Workfile:   SVDataItemManagerTemplate.h  $
// * ----------------------------------------------------------------------------
// * .Current Version : $Revision:   1.2  $
// * .Check In Date   : $Date:   02 Oct 2013 08:55:16  $
// ******************************************************************************

#ifndef SVDATAITEMMANAGERTEMPLATE_H
#define SVDATAITEMMANAGERTEMPLATE_H

#include <map>
#include <boost/config.hpp>
#include <boost/function.hpp>

template< typename SVItemData, typename SVIndexType = int >
class SVDataItemManagerTemplate
{
public:
	typedef boost::function< DWORD_PTR ( SVIndexType ) > SVGetItemDataFunctor;
	typedef boost::function< BOOL ( SVIndexType, DWORD_PTR ) > SVSetItemDataFunctor;
	typedef std::map< unsigned long, SVItemData > SVItemDataMap;
	typedef typename SVItemDataMap::iterator iterator;
	typedef typename SVItemDataMap::const_iterator const_iterator;
	typedef typename SVItemDataMap::key_type key_type;
	typedef typename SVItemDataMap::size_type size_type;

	SVDataItemManagerTemplate( SVGetItemDataFunctor p_GetFunctor, SVSetItemDataFunctor p_SetFunctor );
	SVDataItemManagerTemplate( const SVDataItemManagerTemplate& p_rObject );
	virtual ~SVDataItemManagerTemplate();

	bool empty() const;
	void clear();

	iterator end();
	const_iterator end() const;

	iterator find( const key_type& p_Keyval );
	const_iterator find( const key_type& p_Keyval ) const;

	void erase( iterator p_Where );
	size_type erase( const key_type& p_Keyval );

	iterator GetItemData( const SVIndexType& p_IndexVal );
	const_iterator GetItemData( const SVIndexType& p_IndexVal ) const;
	bool SetItemData( const SVIndexType& p_IndexVal, const SVItemData& p_rItem );
	void ClearItemData( const SVIndexType& p_IndexVal );

protected:
	SVDataItemManagerTemplate();

	SVGetItemDataFunctor m_GetFunctor;
	SVSetItemDataFunctor m_SetFunctor;

	unsigned long m_LastUsedIdentifier;
	SVItemDataMap m_Items;

};

#include "SVDataItemManagerTemplate.inl"

#endif

// ******************************************************************************
// * LOG HISTORY:
// ******************************************************************************
/*
$Log:   N:\PVCSarch65\ProjectFiles\archives\SVObserver_SRC\SVOMFCLibrary\SVDataItemManagerTemplate.h_v  $
 * 
 *    Rev 1.2   02 Oct 2013 08:55:16   tbair
 * Project:  SVObserver
 * Change Request (SCR) nbr:  852
 * SCR Title:  Add Multiple Platform Support to SVObserver's Visual Studio Solution
 * Checked in by:  tBair;  Tom Bair
 * Change Description:  
 *   Add x64 platform.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.1   13 May 2013 16:23:00   bWalter
 * Project:  SVObserver
 * Change Request (SCR) nbr:  814
 * SCR Title:  Upgrade SVObserver to Compile Using Visual Studio 2010
 * Checked in by:  bWalter;  Ben Walter
 * Change Description:  
 *   Merged with svo_src label SVO 6.10 Beta 015.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.2   08 May 2013 09:27:58   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  826
 * SCR Title:  Fix Memory Leaks in I/O Adjustment Dialogs
 * Checked in by:  tBair;  Tom Bair
 * Change Description:  
 *   Added two new methods to get the item data.  One method is const and one is non-const.  This will reduce the neccesity to get the value in the gui item data and do the find in the Item Data Manager.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.0   08 May 2013 15:32:54   bWalter
 * Project:  SVObserver
 * Change Request (SCR) nbr:  814
 * SCR Title:  Upgrade SVObserver to Compile Using Visual Studio 2010
 * Checked in by:  bWalter;  Ben Walter
 * Change Description:  
 *   Initial check in to SVObserver_src.  (Merged with svo_src label SVO 6.10 Beta 012.)
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.1   29 Apr 2013 08:20:46   tbair
 * Project:  SVObserver
 * Change Request (SCR) nbr:  826
 * SCR Title:  Fix Memory Leaks in I/O Adjustment Dialogs
 * Checked in by:  tBair;  Tom Bair
 * Change Description:  
 *   Changed parameter names from key to index on SetItemData and ClearItemData.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.0   24 Apr 2013 13:31:14   tbair
 * Project:  SVObserver
 * Change Request (SCR) nbr:  826
 * SCR Title:  Fix Memory Leaks in I/O Adjustment Dialogs
 * Checked in by:  tBair;  Tom Bair
 * Change Description:  
 *   SVDataItemManagerTemplate to support GUI Item Data.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
*/