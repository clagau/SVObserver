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

#ifndef INCL_SVINSPECTIONIMPORTER_H
#define INCL_SVINSPECTIONIMPORTER_H

#include "SVUtilityLibrary/SVString.h"
#include "SVIProgress.h"
#include "SVImportedInspectionInfo.h"

class SVInspectionImporter
{
private:
	SVInspectionImporter();
	~SVInspectionImporter();

public:
	static HRESULT Import(const SVString& filename, const SVString& inspectionName, const SVString& cameraName, SVImportedInspectionInfo& inspectionInfo, SVIProgress& rProgress);
	static HRESULT GetProperties(const SVString& filename, long& rNewDisbaleMethod, long& rEnableAuxExtents, unsigned long& rVersionNumber);
};

struct SVInspectionImportHelper
{
	SVString filename;
	SVString inspectionName;
	SVString cameraName;
	SVImportedInspectionInfo info;

	SVInspectionImportHelper(const SVString& p_filename, const SVString& p_inspectionName, const SVString& p_cameraName)
	: filename(p_filename)
	, inspectionName(p_inspectionName)
	, cameraName(p_cameraName)
	{
	}

	HRESULT Import(SVIProgress& rProgress)
	{
		return SVInspectionImporter::Import(filename, inspectionName, cameraName, info, rProgress);
	}
};

#endif

//******************************************************************************
//* LOG HISTORY:
//******************************************************************************
/*
$Log:   N:\PVCSarch65\ProjectFiles\archives\SVObserver_src\SVObserver\SVInspectionImporter.h_v  $
 * 
 *    Rev 1.0   23 Apr 2013 11:02:16   bWalter
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
