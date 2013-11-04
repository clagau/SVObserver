//******************************************************************************
//* COPYRIGHT (c) 2003 by SVResearch, Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : ISVPropertyPageDialog
//* .File Name       : $Workfile:   ISVPropertyPageDialog.h  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.0  $
//* .Check In Date   : $Date:   23 Apr 2013 09:13:46  $
//******************************************************************************

#ifndef ISVPROPERTYPAGEDIALOG_H
#define ISVPROPERTYPAGEDIALOG_H
#pragma once

class ISVPropertyPageDialog
{
public:
	virtual bool QueryAllowExit() = 0;
};

#endif	//#ifndef ISVPROPERTYPAGEDIALOG_H

//******************************************************************************
//* LOG HISTORY:
//******************************************************************************
/*
$Log:   N:\PVCSarch65\ProjectFiles\archives\SVObserver_src\SVObserver\ISVPropertyPageDialog.h_v  $
 * 
 *    Rev 1.0   23 Apr 2013 09:13:46   bWalter
 * Project:  SVObserver
 * Change Request (SCR) nbr:  814
 * SCR Title:  Upgrade SVObserver to Compile Using Visual Studio 2010
 * Checked in by:  bWalter;  Ben Walter
 * Change Description:  
 *   Initial check in to SVObserver_src.  (Merged with svo_src label SVO 6.10 Beta 008.)
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.0   12 Oct 2004 12:48:34   ebeyeler
 * Project:  SVObserver
 * Change Request (SCR) nbr:  448
 * SCR Title:  Fix External Tool Change DLL functionality
 * Checked in by:  eBeyeler;  Eric Beyeler
 * Change Description:  
 *   first iteration
 * allows property pages to prevent OnOK
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
*/