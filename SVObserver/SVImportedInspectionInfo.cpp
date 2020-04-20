//******************************************************************************
//* COPYRIGHT (c) 2012 by Seidenader Vision, Inc. Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVImportedInspectionInfo
//* .File Name       : $Workfile:   SVImportedInspectionInfo.cpp  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.0  $
//* .Check In Date   : $Date:   23 Apr 2013 10:56:58  $
//******************************************************************************
#include "stdafx.h"
#include "SVImportedInspectionInfo.h"

SVImportedInspectionInfo::SVImportedInspectionInfo()
	: m_inspectionId(SvDef::InvalidObjectId)
{
}

SVImportedInspectionInfo::~SVImportedInspectionInfo()
{
}

SVImportedInspectionInfo::SVImportedInspectionInfo(const SVImportedInspectionInfo& rInfo)
{
	if (this != &rInfo)
	{
		*this = rInfo;
	}
}

SVImportedInspectionInfo& SVImportedInspectionInfo::operator=(const SVImportedInspectionInfo& rInfo)
{
	m_inspectionId = rInfo.m_inspectionId;
	m_inputList = rInfo.m_inputList;
	m_materialsTree = rInfo.m_materialsTree;
	return *this;
}

