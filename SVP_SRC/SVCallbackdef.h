//******************************************************************************
//* COPYRIGHT (c) 2008 by SVResearch, Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVCallbackdef
//* .File Name       : $Workfile:   SVCallbackdef.h  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.0  $
//* .Check In Date   : $Date:   25 Apr 2013 15:47:58  $
//******************************************************************************

#ifndef SVCallbackdef_h
#define SVCallbackdef_h

#include <boost/config.hpp>
#include <boost/function.hpp>

typedef boost::function<HRESULT (DWORD *)> SVGetStateCallback;

#endif // SVCallbackdef_h

//******************************************************************************
//* LOG HISTORY:
//******************************************************************************
/*
$Log:   N:\PVCSarch65\ProjectFiles\archives\SVObserver_src\SVP_SRC\SVCallbackdef.h_v  $
 * 
 *    Rev 1.0   25 Apr 2013 15:47:58   bWalter
 * Project:  SVObserver
 * Change Request (SCR) nbr:  814
 * SCR Title:  Upgrade SVObserver to Compile Using Visual Studio 2010
 * Checked in by:  bWalter;  Ben Walter
 * Change Description:  
 *   Initial check in to SVObserver_src.  (Merged with svo_src label SVO 6.10 Beta 008.)
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
*/