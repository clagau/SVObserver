//******************************************************************************
//* COPYRIGHT (c) 2008 by Seidenader Vision Inc., Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : ManagedIndexArrayList.h
//* .File Name       : $Workfile:   ManagedIndexArrayList.h  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.0  $
//* .Check In Date   : $Date:   22 Apr 2013 07:53:42  $
//******************************************************************************

#ifndef _INC_MANAGEDINDEXARRAYLIST_3C8E8E400133_INCLUDED
#define _INC_MANAGEDINDEXARRAYLIST_3C8E8E400133_INCLUDED

#include "SVContainerLibrary/SVVector.h"

class SVManagedIndexArray;

class SVManagedIndexArrayList :
	public SVVector< SVManagedIndexArray* >
{
public:
	SVManagedIndexArrayList();

	virtual ~SVManagedIndexArrayList();

};

#endif /* _INC_MANAGEDINDEXARRAYLIST_3C8E8E400133_INCLUDED */

//******************************************************************************
//* LOG HISTORY:
//******************************************************************************
/*
$Log:   N:\PVCSarch65\ProjectFiles\archives\SVObserver_src\SVDataManagerLibrary\ManagedIndexArrayList.h_v  $
 * 
 *    Rev 1.0   22 Apr 2013 07:53:42   bWalter
 * Project:  SVObserver
 * Change Request (SCR) nbr:  814
 * SCR Title:  Upgrade SVObserver to Compile Using Visual Studio 2010
 * Checked in by:  bWalter;  Ben Walter
 * Change Description:  
 *   Initial check in to SVObserver_src.  (Merged with svo_src label SVO 6.10 Beta 008.)
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.0   01 Sep 2009 16:47:16   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  665
 * SCR Title:  Fix unrecoverable failure when processing acquisitions at high speed
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Moved code from SVLibrary to SVDataManagerLibrary
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.2   10 Jan 2003 13:31:50   cschmittinger
 * Project:  SVObserver
 * Change Request (SCR) nbr:  318
 * SCR Title:  Image Transfer Compression
 * Checked in by:  cSchmittinger;  Carl Schmittinger
 * Change Description:  
 *   Code was moved to the SVLibrary Project
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
*/
