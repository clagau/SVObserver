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

namespace SvOi
{
	const DWORD SVIM_CONFIG_LOADING =	0x0001;
	const DWORD SVIM_CONFIG_LOADED =	0x0002;
	const DWORD SVIM_ONLINE_PENDING =	0x0004;
	const DWORD SVIM_ONLINE =			0x0008;
	const DWORD SVIM_RUNNING_TEST =		0x0010;
	const DWORD SVIM_RUNNING =			0x0020;
	const DWORD SVIM_REGRESSION_TEST =	0x0040;
	const DWORD SVIM_SETUPMODE =		0x0080;
	const DWORD SVIM_SAVING_CONFIG =	0x0100;
	const DWORD SVIM_STOPPING =			0x0200;
	const DWORD SVIM_RAID_FAILURE =		0x0400;
} //namespace SvOi
