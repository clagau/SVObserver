// ******************************************************************************
// * COPYRIGHT (c) 2009 by Körber Pharma Inspection GmbH. All Rights Reserved
// * All Rights Reserved
// ******************************************************************************
// * .Module Name     : SVIMCommand
// * .File Name       : $Workfile:   SVIMCommand.h  $
// * ----------------------------------------------------------------------------
// * .Current Version : $Revision:   1.0  $
// * .Check In Date   : $Date:   22 Apr 2013 11:26:48  $
// ******************************************************************************

#pragma once

namespace SvDef
{
	constexpr unsigned long SVIM_CONFIG_LOADING =	0x0001;
	constexpr unsigned long SVIM_CONFIG_LOADED =	0x0002;
	constexpr unsigned long SVIM_ONLINE_PENDING =	0x0004;
	constexpr unsigned long SVIM_ONLINE =			0x0008;
	constexpr unsigned long SVIM_RUNNING_TEST =		0x0010;
	constexpr unsigned long SVIM_RUNNING =			0x0020;
	constexpr unsigned long SVIM_REGRESSION_TEST =	0x0040;
	constexpr unsigned long SVIM_SETUPMODE =		0x0080;
	constexpr unsigned long SVIM_SAVING_CONFIG =	0x0100;
	constexpr unsigned long SVIM_STOPPING =			0x0200;
} //namespace SvDef
