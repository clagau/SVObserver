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
: m_inspectionGuid(GUID_NULL)
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
	m_inspectionGuid = rInfo.m_inspectionGuid;
	m_inputList = rInfo.m_inputList;
	m_materialsTree = rInfo.m_materialsTree;
	return *this;
}

//******************************************************************************
//* LOG HISTORY:
//******************************************************************************
/*
$Log:   N:\PVCSarch65\ProjectFiles\archives\SVObserver_src\SVObserver\SVImportedInspectionInfo.cpp_v  $
 * 
 *    Rev 1.0   23 Apr 2013 10:56:58   bWalter
 * Project:  SVObserver
 * Change Request (SCR) nbr:  814
 * SCR Title:  Upgrade SVObserver to Compile Using Visual Studio 2010
 * Checked in by:  bWalter;  Ben Walter
 * Change Description:  
 *   Initial check in to SVObserver_src.  (Merged with svo_src label SVO 6.10 Beta 008.)
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.0   05 Mar 2013 15:30:04   sjones
 * Project:  SVObserver
 * Change Request (SCR) nbr:  817
 * SCR Title:  Import/Export of Inspections
 * Checked in by:  sJones;  Steve Jones
 * Change Description:  
 *   Initial checkin
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
*/