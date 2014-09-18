//******************************************************************************
//* COPYRIGHT (c) 2014 by Seidenader Vision Inc., Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : stdafx
//* .File Name       : $Workfile:   StdAfx.cpp  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.0  $
//* .Check In Date   : $Date:   17 Sep 2014 15:57:30  $
//******************************************************************************

#include "stdafx.h"

#define SV_EXPAND( x ) #x
#define SV_STRINGIZE( x ) SV_EXPAND( x )
#pragma message( "_WIN32_WINNT=" SV_STRINGIZE( _WIN32_WINNT ) )

//******************************************************************************
//* LOG HISTORY:
//******************************************************************************
/*
$Log:   N:\PVCSarch65\ProjectFiles\archives\SVObserver_SRC\SVLptIO\StdAfx.cpp_v  $
 * 
 *    Rev 1.0   17 Sep 2014 15:57:30   sjones
 * Project:  SVObserver
 * Change Request (SCR) nbr:  943
 * SCR Title:  Parallel Port I/O for 64 Bit and Windows 7
 * Checked in by:  sJones;  Steve Jones
 * Change Description:  
 *   Initial checkin
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
*/
