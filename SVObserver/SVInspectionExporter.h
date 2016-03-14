//******************************************************************************
//* COPYRIGHT (c) 2012 by Seidenader Vision, Inc. Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVInspectionExporter
//* .File Name       : $Workfile:   SVInspectionExporter.h  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.0  $
//* .Check In Date   : $Date:   23 Apr 2013 11:01:58  $
//******************************************************************************

#pragma once

#pragma region Includes
#include "SVUtilityLibrary/SVString.h"
#pragma endregion Includes

class SVInspectionExporter
{
private:
	SVInspectionExporter();
	~SVInspectionExporter();

public:
	static HRESULT Export(const SVString& filename, const SVString& inspectionName, unsigned long p_version, bool bColor);
};

