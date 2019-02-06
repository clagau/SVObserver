//******************************************************************************
//* COPYRIGHT (c) 2012 by Seidenader Vision, Inc.
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVShiftToolUtility
//* .File Name       : $Workfile:   SVShiftToolUtility.h  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.0  $
//* .Check In Date   : $Date:   23 Apr 2013 15:03:46  $
//******************************************************************************

#pragma once
namespace SvTo
{
class SVShiftTool;
}

class SVShiftToolUtility
{
public:
	static void SetToolSetReference(SvTo::SVShiftTool* pShiftTool);
	static void SetToolNormalize(SvTo::SVShiftTool* pShiftTool);

private:
	SVShiftToolUtility() = delete;
	virtual ~SVShiftToolUtility() = delete;
};
