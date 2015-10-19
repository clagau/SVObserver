//******************************************************************************
//* COPYRIGHT (c) 2012 by Seidenader Vision, Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVGuiExtentUpdater
//* .File Name       : $Workfile:   SVGuiExtentUpdater.h  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.2  $
//* .Check In Date   : $Date:   12 Feb 2015 03:00:46  $
//******************************************************************************
#ifndef INCL_SVGUIEXTENTUPDATER_H
#define INCL_SVGUIEXTENTUPDATER_H

#include "SVTaskObject.h"
#include "SVImageLibrary/SVImageExtentClass.h"
#include "SVCommandInspectionExtentUpdater.h"

class SVGuiExtentUpdater
{
public:
	static HRESULT SetImageExtent(SVTaskObjectClass* pTaskObject, const SVImageExtentClass& rExtents, SVCommandExtentResetModeEnum resetMode = ResetMode_Tool);
	static HRESULT SetImageExtentToParent(SVTaskObjectClass* pTaskObject, SVCommandExtentResetModeEnum resetMode = ResetMode_Tool);
	static HRESULT SetImageExtentToFit(SVTaskObjectClass* pTaskObject, const SVImageExtentClass& rExtents, SVCommandExtentResetModeEnum resetMode = ResetMode_Tool);
	
	//************************************
	//! Update the size of the Image in pTool 
	//! and if forward size is true 	all Images which are dependent from this Image
	//! \param p_pTaskObject [in]
	//! \param ForwardSize [in]
	//! \returns HRESULT
	//************************************
	static HRESULT ForwardSizeAndPosition(SVTaskObjectClass* p_pTaskObject, SVCommandExtentResetModeEnum resetMode  ) ;

private:
	static const int TIMEOUT_FOR_SYNCHRONOUS_EXECUTE_IN_MS = 120000;
};

#endif

//******************************************************************************
//* LOG HISTORY:
//******************************************************************************
/*
$Log:   N:\PVCSarch65\ProjectFiles\archives\SVObserver_SRC\SVObserver\SVGuiExtentUpdater.h_v  $
 * 
 *    Rev 1.2   12 Feb 2015 03:00:46   mziegler
 * Project:  SVObserver
 * Change Request (SCR) nbr:  983
 * SCR Title:  Linear Tool - Crash while continuous resizing tool SVO-392
 * Checked in by:  mZiegler;  Marc Ziegler
 * Change Description:  
 *   Do functionality not directly in this class, instead use new class SVCommandInspectionExtetnUpdater to run it in inspection thread
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.1   07 May 2013 08:20:28   bWalter
 * Project:  SVObserver
 * Change Request (SCR) nbr:  814
 * SCR Title:  Upgrade SVObserver to Compile Using Visual Studio 2010
 * Checked in by:  bWalter;  Ben Walter
 * Change Description:  
 *   Merged with svo_src label SVO 6.10 Beta 011.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.1   19 Apr 2013 08:29:06   tbair
 * Project:  SVObserver
 * Change Request (SCR) nbr:  827
 * SCR Title:  Add button to the adjust tool size/position to set size to parent image.
 * Checked in by:  tBair;  Tom Bair
 * Change Description:  
 *   Added SetImageExtentToParent function to support Full Image button in SVAdjustToolSizePositionDialog.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.0   23 Apr 2013 10:42:08   bWalter
 * Project:  SVObserver
 * Change Request (SCR) nbr:  814
 * SCR Title:  Upgrade SVObserver to Compile Using Visual Studio 2010
 * Checked in by:  bWalter;  Ben Walter
 * Change Description:  
 *   Initial check in to SVObserver_src.  (Merged with svo_src label SVO 6.10 Beta 008.)
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.0   07 Aug 2012 16:01:28   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  769
 * SCR Title:  Fix Problems and Crashes with Inspection Document Display Updates
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Fixed issues with updating tool extents when the tool is off the image.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
*/