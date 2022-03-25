//******************************************************************************
//* COPYRIGHT (c) 2012 by Körber Pharma Inspection GmbH. All Rights Reserved Harrisburg
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
#pragma endregion Includes

class SVInspectionExporter
{
private:
	SVInspectionExporter();
	~SVInspectionExporter();

public:
	static HRESULT Export(const std::string& filename, const std::string& inspectionName, unsigned long p_version, bool bColor);
};

