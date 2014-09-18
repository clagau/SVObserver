//******************************************************************************
//* COPYRIGHT (c) 2014 by Seidenader Vision Inc., Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVLptIOCallback
//* .File Name       : $Workfile:   SVLptIOCallback.h  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.0  $
//* .Check In Date   : $Date:   17 Sep 2014 15:57:34  $
//******************************************************************************

#pragma once

typedef HRESULT (CALLBACK *SVLptIOCallbackPtr)(void* pOwner, void* pData);

//******************************************************************************
//* LOG HISTORY:
//******************************************************************************
/*
$Log:   N:\PVCSarch65\ProjectFiles\archives\SVObserver_SRC\SVLptIO\SVLptIOCallback.h_v  $
 * 
 *    Rev 1.0   17 Sep 2014 15:57:34   sjones
 * Project:  SVObserver
 * Change Request (SCR) nbr:  943
 * SCR Title:  Parallel Port I/O for 64 Bit and Windows 7
 * Checked in by:  sJones;  Steve Jones
 * Change Description:  
 *   Initial checkin
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
*/