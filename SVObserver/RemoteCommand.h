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
#pragma region Includes
#include "SVUtilityLibrary/SVString.h"
#pragma endregion Includes

extern void GlobalRCGoOnline();
extern void GlobalRCGoOffline();
extern bool GlobalRCGetState( DWORD* pdwSVIMState );
extern HRESULT GlobalRCSetMode( unsigned long lSVIMNewMode );
extern HRESULT GlobalRCGetMode( unsigned long* p_plMode );
extern SVString GlobalRCGetConfigurationName();
extern void GlobalRCSaveConfiguration();
extern bool GlobalRCOpenConfiguration( LPCTSTR ConfigName );
extern void GlobalRCCloseAndCleanConfiguration();
extern void GlobalRCCloseConfiguration();

