//******************************************************************************
//* COPYRIGHT (c) 2014 by Seidenader, Markt Schwaben
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : RemoteCommand
//* .File Name       : $Workfile:   RemoteCommand.h  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.0  $
//* .Check In Date   : $Date:   04 Feb 2014 13:16:34  $
//******************************************************************************
#pragma once

namespace Seidenader
{
	namespace SVObserver
	{
		extern BOOL GlobalRCGoOnline();
		extern BOOL GlobalRCGoOffline();
		extern BOOL GlobalRCGetState( DWORD* pdwSVIMState );
		extern HRESULT GlobalRCSetMode( unsigned long lSVIMNewMode );
		extern HRESULT GlobalRCGetMode( unsigned long* p_plMode );
		extern BOOL GlobalRCGetConfigurationName( char* pszConfigName );
		extern BOOL GlobalRCSaveConfiguration();
		extern BOOL GlobalRCOpenConfiguration( char* pszConfigName );
		extern BOOL GlobalRCCloseAndCleanConfiguration();
		extern BOOL GlobalRCCloseConfiguration();
	} //namespace SVObserver
} //namespace Seidenader

//******************************************************************************
//* LOG HISTORY:
//******************************************************************************
/*
$Log:   N:\PVCSarch65\ProjectFiles\archives\SVObserver_SRC\SVObserver\RemoteCommand.h_v  $
 * 
 *    Rev 1.0   04 Feb 2014 13:16:34   bwalter
 * Project:  SVObserver
 * Change Request (SCR) nbr:  880
 * SCR Title:  Remove .SEC
 * Checked in by:  mZiegler;  Marc Ziegler
 * Change Description:  
 *   Initial check in.  Created from global remote command methods from SVObserver.cpp.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 */
