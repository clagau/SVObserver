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
#include "SVUtilityLibrary/SVString.h"
#include "SVIProgress.h"
#include "SVImportedInspectionInfo.h"
#include "ObjectInterfaces/GlobalConstantData.h"
#pragma endregion Includes

class SVInspectionImporter
{
private:
	SVInspectionImporter();
	~SVInspectionImporter();

public:
	static HRESULT Import(const SVString& filename, const SVString& inspectionName, const SVString& cameraName, SVImportedInspectionInfo& inspectionInfo, SvOi::GlobalConflictPairVector& rGlobalConflicts, SVIProgress& rProgress);
	static HRESULT GetProperties(const SVString& filename, long& rNewDisbaleMethod, long& rEnableAuxExtents, unsigned long& rVersionNumber);
};

struct SVInspectionImportHelper
{
	SVString filename;
	SVString inspectionName;
	SVString cameraName;
	SVImportedInspectionInfo info;
	SvOi::GlobalConflictPairVector GlobalConflicts;

	SVInspectionImportHelper(const SVString& p_filename, const SVString& p_inspectionName, const SVString& p_cameraName)
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


