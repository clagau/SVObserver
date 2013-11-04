//******************************************************************************
//* COPYRIGHT (c) 2012 by Seidenader Vision, Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVGuiExtentUpdater
//* .File Name       : $Workfile:   SVGuiExtentUpdater.cpp  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.1  $
//* .Check In Date   : $Date:   07 May 2013 08:20:26  $
//******************************************************************************
#include "stdafx.h"
#include "SVGuiExtentUpdater.h"
#include "SVInspectionProcess.h"
#include "SVTool.h"
#include "SVCommandInspectionRunOnce.h"
#include "SVObjectLibrary/SVObjectSynchronousCommandTemplate.h"

static bool RunOnce(SVInspectionProcess* p_pInspection, SVToolClass* p_pTool)
{
	bool l_Status = (p_pInspection != NULL);

	if (l_Status)
	{
		SVGUID l_ToolId;

		if (p_pTool != NULL)
		{
			l_ToolId = p_pTool->GetUniqueObjectID();
		}

		SVCommandInspectionRunOncePtr l_CommandPtr = new SVCommandInspectionRunOnce( p_pInspection->GetUniqueObjectID(), l_ToolId );
		SVObjectSynchronousCommandTemplate< SVCommandInspectionRunOncePtr > l_Command( p_pInspection->GetUniqueObjectID(), l_CommandPtr );

		l_Status = ( l_Command.Execute( 120000 ) == S_OK );
	}
	return l_Status;
}

HRESULT SVGuiExtentUpdater::SetImageExtent(SVTaskObjectClass* p_pTaskObject, const SVImageExtentClass& p_rExtents)
{
	HRESULT l_Status = S_OK;

	SVInspectionProcess* l_pInspection = p_pTaskObject->GetInspection();

	if (l_pInspection != NULL)
	{
		SVProductInfoStruct l_svProduct = l_pInspection->LastProductGet(SV_DISPLAY);

		SVDataManagerHandle l_DMIndexHandle;

		l_svProduct.GetResultDataIndex( l_DMIndexHandle );

		l_Status = p_pTaskObject->SetImageExtent(l_DMIndexHandle.GetIndex(), p_rExtents);

		if (l_Status == S_OK)
		{
			l_pInspection->m_bForceOffsetUpdate = true;
			l_pInspection->AddResetState( SVResetAutoMoveAndResize );

			if (::SVSendMessage(p_pTaskObject, SVM_RESET_ALL_OBJECTS, NULL, NULL) != SVMR_SUCCESS)
			{
				l_Status = S_FALSE;
			}
			else
			{
				RunOnce(l_pInspection, NULL);
			}

			l_pInspection->RemoveResetState( SVResetAutoMoveAndResize );
		}
	}
	return l_Status;
}

HRESULT SVGuiExtentUpdater::SetImageExtentToParent(SVTaskObjectClass* p_pTaskObject)
{
	HRESULT l_Status = S_OK;

	SVInspectionProcess* l_pInspection = p_pTaskObject->GetInspection();

	if (l_pInspection != NULL)
	{
		SVProductInfoStruct l_svProduct = l_pInspection->LastProductGet(SV_DISPLAY);

		SVDataManagerHandle l_DMIndexHandle;

		l_svProduct.GetResultDataIndex( l_DMIndexHandle );

		l_Status = p_pTaskObject->SetImageExtentToParent(l_DMIndexHandle.GetIndex());

		if (l_Status == S_OK)
		{
			l_pInspection->m_bForceOffsetUpdate = true;
			l_pInspection->AddResetState( SVResetAutoMoveAndResize );

			if (::SVSendMessage(p_pTaskObject, SVM_RESET_ALL_OBJECTS, NULL, NULL ) != SVMR_SUCCESS)
			{
				l_Status = S_FALSE;
			}
			else
			{
				RunOnce(l_pInspection, NULL);
			}

			l_pInspection->RemoveResetState( SVResetAutoMoveAndResize );
		}
	}
	return l_Status;
}


HRESULT SVGuiExtentUpdater::SetImageExtentToFit(SVTaskObjectClass* p_pTaskObject, const SVImageExtentClass& p_rExtents)
{
	HRESULT l_Status = S_OK;

	SVInspectionProcess* l_pInspection = p_pTaskObject->GetInspection();

	if (l_pInspection != NULL)
	{
		SVProductInfoStruct l_svProduct = l_pInspection->LastProductGet(SV_DISPLAY);

		SVDataManagerHandle l_DMIndexHandle;

		l_svProduct.GetResultDataIndex( l_DMIndexHandle );

		l_Status = p_pTaskObject->SetImageExtentToFit(l_DMIndexHandle.GetIndex(), p_rExtents);

		if (l_Status == S_OK)
		{
			l_pInspection->m_bForceOffsetUpdate = true;
			l_pInspection->AddResetState( SVResetAutoMoveAndResize );

			if (::SVSendMessage(p_pTaskObject, SVM_RESET_ALL_OBJECTS, NULL, NULL ) != SVMR_SUCCESS)
			{
				l_Status = S_FALSE;
			}
			else
			{
				RunOnce(l_pInspection, NULL);
			}

			l_pInspection->RemoveResetState( SVResetAutoMoveAndResize );
		}
	}
	return l_Status;
}

//******************************************************************************
//* LOG HISTORY:
//******************************************************************************
/*
$Log:   N:\PVCSarch65\ProjectFiles\archives\SVObserver_src\SVObserver\SVGuiExtentUpdater.cpp_v  $
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