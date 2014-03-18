//******************************************************************************
//* COPYRIGHT (c) 2012 by Seidenader Vision, Inc.
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVObjectNameInfo
//* .File Name       : $Workfile:   SVObjectNameInfo.h  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.2  $
//* .Check In Date   : $Date:   17 Mar 2014 14:19:08  $
//******************************************************************************

#ifndef SVOBJECTNAMEINFO_H
#define SVOBJECTNAMEINFO_H

#pragma region Includes
#include <deque>
#include "SVUtilityLibrary/SVString.h"
#pragma endregion Includes

struct SVObjectNameInfo
{
	typedef std::deque< SVString > SVNameDeque;

	static HRESULT ParseObjectName( SVObjectNameInfo& p_rNameInfo, const SVString& p_rObjectName );

	SVObjectNameInfo();
	SVObjectNameInfo( const SVNameDeque& p_rNameArray );
	SVObjectNameInfo( const SVNameDeque& p_rNameArray, const SVString& p_rIndex );
	SVObjectNameInfo( const SVNameDeque& p_rNameArray, const SVString& p_rIndex, const SVString& p_rDefaultValue );
	SVObjectNameInfo( const SVObjectNameInfo& p_rObject );

	void clear();

	HRESULT ParseObjectName( const SVString& p_rObjectName );
	HRESULT UpdateObjectNameOnly( const SVString& p_rObjectName );

	SVString GetObjectName( size_t p_StartIndex = 0 ) const;
	SVString GetObjectArrayName( size_t p_StartIndex = 0 ) const;

	void RemoveTopName();
	void RemoveBottomName();
	long GetIndexValue() const;

	SVNameDeque m_NameArray;

	bool m_IndexPresent;
	SVString m_Index;

	bool m_DefaultValuePresent;
	SVString m_DefaultValue;
};

#endif

//******************************************************************************
//* LOG HISTORY:
//******************************************************************************
/*
$Log:   N:\PVCSarch65\ProjectFiles\archives\SVObserver_SRC\SVObjectLibrary\SVObjectNameInfo.h_v  $
 * 
 *    Rev 1.2   17 Mar 2014 14:19:08   bwalter
 * Project:  SVObserver
 * Change Request (SCR) nbr:  869
 * SCR Title:  Add PPQ and Environment Variables to Object Manager and Update Pickers
 * Checked in by:  bWalter;  Ben Walter
 * Change Description:  
 *   Added Includes region.
 *   Added methods to remove either the top or bottom part of an FQN.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.1   10 Jun 2013 17:42:12   bWalter
 * Project:  SVObserver
 * Change Request (SCR) nbr:  814
 * SCR Title:  Upgrade SVObserver to Compile Using Visual Studio 2010
 * Checked in by:  bWalter;  Ben Walter
 * Change Description:  
 *   Merged with svo_src label SVO 6.10 Beta 017.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.1   21 May 2013 12:48:46   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  824
 * SCR Title:  Add Remote Access to SVObserver Application Configuration Variables
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Update Object Name Information class to allow for starting the name at a specified index position.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.0   22 Apr 2013 16:58:06   bWalter
 * Project:  SVObserver
 * Change Request (SCR) nbr:  814
 * SCR Title:  Upgrade SVObserver to Compile Using Visual Studio 2010
 * Checked in by:  bWalter;  Ben Walter
 * Change Description:  
 *   Initial check in to SVObserver_src.  (Merged with svo_src label SVO 6.10 Beta 008.)
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.0   18 Jun 2012 17:25:16   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  744
 * SCR Title:  Add Shared Memory and Socket Functionality for Run Page Web Server
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Separated dotted name parsing and management to its own class.  This allows for the use of this functionality without have to involve the Reference Class.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.0   18 Jun 2012 17:20:10   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  744
 * SCR Title:  Add Shared Memory and Socket Functionality for Run Page Web Server
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Separated dotted name parsing and management to its own class.  This allows for the use of this functionality without have to involve the Reference Class.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
*/