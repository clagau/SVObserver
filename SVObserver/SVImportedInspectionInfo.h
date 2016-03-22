//******************************************************************************
//* COPYRIGHT (c) 2012 by Seidenader Vision, Inc. Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVImportedInspectionInfo
//* .File Name       : $Workfile:   SVImportedInspectionInfo.h  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.0  $
//* .Check In Date   : $Date:   23 Apr 2013 10:57:06  $
//******************************************************************************

#pragma once

//Moved to precompiled header: #include <deque>
#include "SVXMLLibrary/SVXMLMaterialsTree.h"
#include "SVImportedInputList.h"

class SVImportedInspectionInfo
{
public:
	GUID m_inspectionGuid;
	SVImportedInputList m_inputList;
	SVMaterialsTree m_materialsTree;

	SVImportedInspectionInfo();
	~SVImportedInspectionInfo();

	SVImportedInspectionInfo(const SVImportedInspectionInfo& rInfo);
	SVImportedInspectionInfo& operator=(const SVImportedInspectionInfo& rInfo);
};

typedef std::deque<SVImportedInspectionInfo> SVImportedInspectionInfoList;

