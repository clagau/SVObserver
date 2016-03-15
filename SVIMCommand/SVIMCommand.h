// ******************************************************************************
// * COPYRIGHT (c) 2009 by Seidenader Vision, Inc.
// * All Rights Reserved
// ******************************************************************************
// * .Module Name     : SVIMCommand
// * .File Name       : $Workfile:   SVIMCommand.h  $
// * ----------------------------------------------------------------------------
// * .Current Version : $Revision:   1.0  $
// * .Check In Date   : $Date:   22 Apr 2013 11:26:48  $
// ******************************************************************************

#pragma once

#pragma region Includes
#include "SVStatusLibrary\MessageContainer.h"
#pragma endregion Includes

#define SVIOBUFFSIZE 4096

#define SVPUTCONFIG 0 
#define SVGETCONFIG 1
#define SVGETCURRENTCONFIG 2
#define SVSETCURRENTCONFIG 3
#define SVGOOFFLINE 4
#define SVGOONLINE 5
#define SVGETSTATE 6
#define SVCANCEL 7
#define SVDATA 8
#define	SVPUTFILE 9

#define SVIM_CONFIG_LOADING		0x0001
#define SVIM_CONFIG_LOADED		0x0002
#define SVIM_ONLINE_PENDING		0x0004
#define SVIM_ONLINE						0x0008
#define SVIM_RUNNING_TEST			0x0010
#define SVIM_RUNNING					0x0020
#define SVIM_REGRESSION_TEST	0x0040
#define SVIM_SETUPMODE				0x0080
#define SVIM_SAVING_CONFIG		0x0100
#define SVIM_STOPPING					0x0200
#define SVIM_RAID_FAILURE			0x0400

typedef void (CALLBACK * SVCOMMANDCOMPLETEPROC)(CString &szFileName, SvStl::MessageContainer *pException, void *pUserData);

class SVIMCommand 
{
public:
	BOOL unescape (char *pInput, char *pOutput, int cbOutput);
	BOOL escape (char *pInput, char *pOutput, int cbOutput);

	SVIMCommand();

	virtual ~SVIMCommand();

	int GetCommandToken(char * pszCommandText);

	char *   GetCommandText(int CommandToken   );

private:
	CArray< char*, char* > mszCommandList;

};

