//******************************************************************************
//* COPYRIGHT (c) 2012 by Seidenader Vision, Inc.
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVGuidList
//* .File Name       : $Workfile:   SVGuidList.h  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.0  $
//* .Check In Date   : $Date:   25 Apr 2013 19:21:42  $
//******************************************************************************
#ifndef INCL_SVGUIDLIST_H
#define INCL_SVGUIDLIST_H

#include <deque>
#include <set>
#include <map>
#include <guiddef.h>

typedef std::deque<GUID> SVGuidList;
typedef std::set<GUID> SVUniqueGuidList;
typedef std::map<_bstr_t, GUID> SVNameGuidList;

#endif

//******************************************************************************
//* LOG HISTORY:
//******************************************************************************
/*
$Log:   N:\PVCSarch65\ProjectFiles\archives\SVObserver_src\SVUtilityLibrary\SVGuidList.h_v  $
 * 
 *    Rev 1.0   25 Apr 2013 19:21:42   bWalter
 * Project:  SVObserver
 * Change Request (SCR) nbr:  814
 * SCR Title:  Upgrade SVObserver to Compile Using Visual Studio 2010
 * Checked in by:  bWalter;  Ben Walter
 * Change Description:  
 *   Initial check in to SVObserver_src.  (Merged with svo_src label SVO 6.10 Beta 008.)
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.0   04 Sep 2012 13:44:58   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  787
 * SCR Title:  Fix problem when Discrete I/O does not change when forced attribute is set
 * Checked in by:  tBair;  Tom Bair
 * Change Description:  
 *   Initial check-in for new files.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
*/
