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

#ifndef INCL_SVIMPORTEDINSPECTIONINFO_H
#define INCL_SVIMPORTEDINSPECTIONINFO_H

#include <deque>
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

#endif

//******************************************************************************
//* LOG HISTORY:
//******************************************************************************
/*
$Log:   N:\PVCSarch65\ProjectFiles\archives\SVObserver_src\SVObserver\SVImportedInspectionInfo.h_v  $
 * 
 *    Rev 1.0   23 Apr 2013 10:57:06   bWalter
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
