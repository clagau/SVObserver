//******************************************************************************
//* COPYRIGHT (c) 2012 by Seidenader Vision, Inc. Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVInspectionImporter
//* .File Name       : $Workfile:   SVInspectionImporter.h  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.0  $
//* .Check In Date   : $Date:   23 Apr 2013 11:02:16  $
//******************************************************************************
#pragma once

#pragma region Includes
#include "SVImportedInspectionInfo.h"
#include "Definitions/GlobalConstantData.h"
#pragma endregion Includes

class SVIProgress;

class SVInspectionImporter
{
private:
	SVInspectionImporter();
	~SVInspectionImporter();

public:
	static HRESULT Import(const std::string& filename, const std::string& inspectionName, const std::string& cameraName, SVImportedInspectionInfo& inspectionInfo, SvDef::GlobalConflictPairVector& rGlobalConflicts, SVIProgress& rProgress);
	static HRESULT GetProperties(const std::string& filename, long& rNewDisbaleMethod, long& rEnableAuxExtents, unsigned long& rVersionNumber);
};

struct SVInspectionImportHelper
{
	std::string filename;
	std::string inspectionName;
	std::string cameraName;
	SVImportedInspectionInfo info;
	SvDef::GlobalConflictPairVector GlobalConflicts;

	SVInspectionImportHelper(const std::string& p_filename, const std::string& p_inspectionName, const std::string& p_cameraName)
	: filename(p_filename)
	, inspectionName(p_inspectionName)
	, cameraName(p_cameraName)
	{
	}

	HRESULT Import(SVIProgress& rProgress)
	{
		return SVInspectionImporter::Import(filename, inspectionName, cameraName, info, GlobalConflicts,rProgress);
	}
};


