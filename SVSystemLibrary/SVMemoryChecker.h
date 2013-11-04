//******************************************************************************
//* COPYRIGHT (c) 2008 by Seidenader Vision Inc., Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVMemoryChecker
//* .File Name       : $Workfile:   SVMemoryChecker.h  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.0  $
//* .Check In Date   : $Date:   25 Apr 2013 17:52:30  $
//******************************************************************************

#ifndef SVMEMORYCHECKER_H
#define SVMEMORYCHECKER_H

#pragma comment( lib, "Psapi" )

class SVMemoryChecker
{
public:
	static size_t GetFreeProcessMemorySize();

	virtual ~SVMemoryChecker();

private:
	SVMemoryChecker();

};

#endif

//******************************************************************************
//* LOG HISTORY:
//******************************************************************************
/*
$Log:   N:\PVCSarch65\ProjectFiles\archives\SVObserver_src\SVSystemLibrary\SVMemoryChecker.h_v  $
 * 
 *    Rev 1.0   25 Apr 2013 17:52:30   bWalter
 * Project:  SVObserver
 * Change Request (SCR) nbr:  814
 * SCR Title:  Upgrade SVObserver to Compile Using Visual Studio 2010
 * Checked in by:  bWalter;  Ben Walter
 * Change Description:  
 *   Initial check in to SVObserver_src.  (Merged with svo_src label SVO 6.10 Beta 008.)
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.0   13 Jul 2010 12:11:42   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  697
 * SCR Title:  Add Free Memory Information to Status Bar
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Added new memory checker class to library to caculate free memory.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
*/