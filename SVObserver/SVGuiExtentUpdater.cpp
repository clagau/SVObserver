//******************************************************************************
//* COPYRIGHT (c) 2012 by Seidenader Vision, Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVGuiExtentUpdater
//* .File Name       : $Workfile:   SVGuiExtentUpdater.cpp  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.2  $
//* .Check In Date   : $Date:   12 Feb 2015 03:00:46  $
//******************************************************************************
#include "stdafx.h"
#include "SVGuiExtentUpdater.h"
#include "SVInspectionProcess.h"
#include "SVTool.h"
#include "SVObjectLibrary/SVObjectSynchronousCommandTemplate.h"
#include "SVCommandInspectionExtentUpdater.h"
#include "ErrorNumbers.h"

HRESULT SVGuiExtentUpdater::SetImageExtent(SVTaskObjectClass* p_pTaskObject, const SVImageExtentClass& p_rExtents)
{
	HRESULT status = Err_SetImageExtent_InvalidParameter_2001;
	
	if ( nullptr != p_pTaskObject )
	{
		SVInspectionProcess* pInspection = p_pTaskObject->GetInspection();
		if ( nullptr != pInspection )
		{
			SVCommandInspectionSetImageExtentPtr commandPtr = new SVCommandInspectionExtentUpdater( pInspection->GetUniqueObjectID(), p_pTaskObject->GetUniqueObjectID(), ExtentUpdaterMode_SetImageExtent, p_rExtents );
			SVObjectSynchronousCommandTemplate< SVCommandInspectionSetImageExtentPtr > command( pInspection->GetUniqueObjectID(), commandPtr );
			status = command.Execute( TIMEOUT_FOR_SYNCHRONOUS_EXECUTE_IN_MS );
		}
	}

	return status;
}

HRESULT SVGuiExtentUpdater::SetImageExtentToParent(SVTaskObjectClass* p_pTaskObject)
{
	HRESULT status = Err_SetImageExtentToParent_InvalidParameter_2002;

	if ( nullptr != p_pTaskObject )
	{
		SVInspectionProcess* pInspection = p_pTaskObject->GetInspection();
		if ( nullptr != pInspection )
		{
			SVCommandInspectionSetImageExtentPtr commandPtr = new SVCommandInspectionExtentUpdater( pInspection->GetUniqueObjectID(), p_pTaskObject->GetUniqueObjectID(), ExtentUpdaterMode_SetImageExtentToParent);
			SVObjectSynchronousCommandTemplate< SVCommandInspectionSetImageExtentPtr > command( pInspection->GetUniqueObjectID(), commandPtr );
			status = command.Execute( TIMEOUT_FOR_SYNCHRONOUS_EXECUTE_IN_MS );
		}
	}

	return status;
}


HRESULT SVGuiExtentUpdater::SetImageExtentToFit(SVTaskObjectClass* p_pTaskObject, const SVImageExtentClass& p_rExtents)
{
	HRESULT status = Err_SetImageExtentToFit_InvalidParameter_2003;

	if ( nullptr != p_pTaskObject )
	{
		SVInspectionProcess* pInspection = p_pTaskObject->GetInspection();
		if ( nullptr != pInspection )
		{
			SVCommandInspectionSetImageExtentPtr commandPtr = new SVCommandInspectionExtentUpdater( pInspection->GetUniqueObjectID(), p_pTaskObject->GetUniqueObjectID(), ExtentUpdaterMode_SetImageExtentToFit, p_rExtents );
			SVObjectSynchronousCommandTemplate< SVCommandInspectionSetImageExtentPtr > command( pInspection->GetUniqueObjectID(), commandPtr );
			status = command.Execute( TIMEOUT_FOR_SYNCHRONOUS_EXECUTE_IN_MS );
		}
	}

	return status;
}

//******************************************************************************
//* LOG HISTORY:
//******************************************************************************
/*
$Log:   N:\PVCSarch65\ProjectFiles\archives\SVObserver_SRC\SVObserver\SVGuiExtentUpdater.cpp_v  $
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
 *    Rev 1.1   07 May 2013 08:20:26   bWalter
 * Project:  SVObserver
 * Change Request (SCR) nbr:  814
 * SCR Title:  Upgrade SVObserver to Compile Using Visual Studio 2010
 * Checked in by:  bWalter;  Ben Walter
 * Change Description:  
 *   Merged with svo_src label SVO 6.10 Beta 011.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.2   19 Apr 2013 08:29:08   tbair
 * Project:  SVObserver
 * Change Request (SCR) nbr:  827
 * SCR Title:  Add button to the adjust tool size/position to set size to parent image.
 * Checked in by:  tBair;  Tom Bair
 * Change Description:  
 *   Added SetImageExtentToParent function to support Full Image button in SVAdjustToolSizePositionDialog.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.0   23 Apr 2013 10:43:46   bWalter
 * Project:  SVObserver
 * Change Request (SCR) nbr:  814
 * SCR Title:  Upgrade SVObserver to Compile Using Visual Studio 2010
 * Checked in by:  bWalter;  Ben Walter
 * Change Description:  
 *   Initial check in to SVObserver_src.  (Merged with svo_src label SVO 6.10 Beta 008.)
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.1   17 Aug 2012 16:16:58   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  769
 * SCR Title:  Fix Problems and Crashes with Inspection Document Display Updates
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Added new flag around reset object call to force an extent check and resize if necessary.
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