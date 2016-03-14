//******************************************************************************
//* COPYRIGHT (c) 2014 by Seidenader, Markt Schwaben
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : RemoteCommand
//* .File Name       : $Workfile:   RemoteCommand.h  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.1  $
//* .Check In Date   : $Date:   17 Jul 2014 17:56:30  $
//******************************************************************************

#pragma once

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

