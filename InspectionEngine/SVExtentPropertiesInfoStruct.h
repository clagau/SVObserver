//******************************************************************************
//* COPYRIGHT (c) 2004 by SVResearch, Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVExtentPropertiesInfoStruct
//* .File Name       : $Workfile:   SVExtentPropertiesInfoStruct.h  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.0  $
//* .Check In Date   : $Date:   23 Apr 2013 10:30:12  $
//******************************************************************************

#pragma once

namespace SvIe
{

struct SVExtentPropertyInfoStruct
{
	bool bFormula;
	bool bHidden;
	bool bSetByReset; //< property will be set in ToolSizeAdjustTask::ResetObject()

	SVExtentPropertyInfoStruct() : bFormula(false), bHidden(false), bSetByReset(false) {}
};

} //namespace SvIe
