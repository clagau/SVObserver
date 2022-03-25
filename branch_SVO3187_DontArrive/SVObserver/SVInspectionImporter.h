//******************************************************************************
//* COPYRIGHT (c) 2012 by Körber Pharma Inspection GmbH. All Rights Reserved Harrisburg
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
#include "SVUtilityLibrary/GlobalConstantData.h"
#pragma endregion Includes

class SVIProgress;

class SVInspectionImporter
{
private:
	SVInspectionImporter() = default;
	~SVInspectionImporter() = default;

public:
	static HRESULT Import(const std::string& filename, const std::string& inspectionName, const std::string& cameraName, SVImportedInspectionInfo& inspectionInfo, SvUl::GlobalConflictPairVector& rGlobalConflicts, SVIProgress& rProgress);
	static HRESULT GetProperties(const std::string& filename, long& rNewDisbaleMethod, long& rEnableAuxExtents, unsigned long& rVersionNumber);
private:
	static HRESULT loadAndReplaceData(const std::string& inFileName, const std::string& rNewInspectionName, SvXml::SVXMLMaterialsTree& rTree);
};

struct SVInspectionImportHelper
{
	std::string filename;
	std::string inspectionName;
	std::string cameraName;
	SVImportedInspectionInfo info;
	SvUl::GlobalConflictPairVector GlobalConflicts;

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


