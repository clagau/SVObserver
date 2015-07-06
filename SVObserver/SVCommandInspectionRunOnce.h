//******************************************************************************
//* COPYRIGHT (c) 2012 by Seidenader Vision, Inc.
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVCommandInspectionRunOnce
//* .File Name       : $Workfile:   SVCommandInspectionRunOnce.h  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.0  $
//* .Check In Date   : $Date:   23 Apr 2013 09:56:42  $
//******************************************************************************

#ifndef SVCOMMANDINSPECTIONRUNONCE_H
#define SVCOMMANDINSPECTIONRUNONCE_H

#include "SVUtilityLibrary/SVGUID.h"
#include "SVUtilityLibrary/SVSharedPtr.h"

class SVCommandInspectionRunOnce
{
public:
	SVCommandInspectionRunOnce(const SVCommandInspectionRunOnce& p_rObject);
	SVCommandInspectionRunOnce(const SVGUID& p_rInspectionId, const SVGUID& p_rToolId = SVGUID());

	virtual ~SVCommandInspectionRunOnce();

	HRESULT Execute();

	bool empty() const;

	const SVGUID& GetInspectionId() const;
	const SVGUID& GetToolId() const;

private:
	SVGUID m_InspectionId;
	SVGUID m_ToolId;

};

typedef SVSharedPtr< SVCommandInspectionRunOnce > SVCommandInspectionRunOncePtr;


#endif

//******************************************************************************
//* LOG HISTORY:
//******************************************************************************
/*
$Log:   N:\PVCSarch65\ProjectFiles\archives\SVObserver_src\SVObserver\SVCommandInspectionRunOnce.h_v  $
 * 
 *    Rev 1.0   23 Apr 2013 09:56:42   bWalter
 * Project:  SVObserver
 * Change Request (SCR) nbr:  814
 * SCR Title:  Upgrade SVObserver to Compile Using Visual Studio 2010
 * Checked in by:  bWalter;  Ben Walter
 * Change Description:  
 *   Initial check in to SVObserver_src.  (Merged with svo_src label SVO 6.10 Beta 008.)
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.0   01 Aug 2012 11:44:46   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  769
 * SCR Title:  Fix Problems and Crashes with Inspection Document Display Updates
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Added new command functionality to allow for synchronous command execution through the inspection thread.  This will fix problems with race conditions with the inspection process.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
*/





