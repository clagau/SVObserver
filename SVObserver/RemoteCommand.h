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
#pragma endregion Includes

enum PutConfigType
{
	PackedFormat,
	SvzFormatWithName,
	SvzFormatDefaultName,
};

void GlobalRCGoOnline();
void GlobalRCGoOffline();
bool GlobalRCGetState( DWORD* pdwSVIMState );
HRESULT GlobalRCSetMode( unsigned long newMode );
HRESULT GlobalRCGetMode( unsigned long* plMode );
std::string GlobalRCGetConfigurationName();
HRESULT GlobalRCSaveConfiguration(LPCTSTR pFileName);
bool GlobalRCOpenConfiguration( LPCTSTR ConfigName );
void GlobalRCCloseAndCleanConfiguration();
void GlobalRCCloseConfiguration();
HRESULT GlobalRCLoadPackedConfiguration(LPCTSTR pFileName, PutConfigType type);
HRESULT GlobalRCAddFileToConfig(LPCTSTR pFileName);

