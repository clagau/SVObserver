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

#pragma region Includes
#include "stdafx.h"
#include "SVGuiExtentUpdater.h"
#include "SVInspectionProcess.h"
#include "SVTool.h"
#include "SVObjectLibrary/SVObjectSynchronousCommandTemplate.h"
#include "SVCommandInspectionExtentUpdater.h"
#include "ObjectInterfaces\ErrorNumbers.h"
#pragma endregion Includes

HRESULT SVGuiExtentUpdater::SetImageExtent(SVTaskObjectClass* pTaskObject, const SVImageExtentClass& rExtents, SVCommandExtentResetModeEnum resetMode )
{
	CWaitCursor wait;

	HRESULT status = SvOi::Err_10001_SetImageExtent_InvalidParameter;
	if ( nullptr != pTaskObject )
	{
		SVInspectionProcess* pInspection = pTaskObject->GetInspection();
		if ( nullptr != pInspection )
		{
			SVCommandInspectionSetImageExtentPtr commandPtr = new SVCommandInspectionExtentUpdater( pInspection->GetUniqueObjectID(), pTaskObject->GetUniqueObjectID(), ExtentUpdaterMode_SetImageExtent, &rExtents );
			commandPtr->SetResetInspection(resetMode);
			SVObjectSynchronousCommandTemplate< SVCommandInspectionSetImageExtentPtr > command( pInspection->GetUniqueObjectID(), commandPtr );
			status = command.Execute( TIMEOUT_FOR_SYNCHRONOUS_EXECUTE_IN_MS );
		}
	}

	return status;
}

HRESULT SVGuiExtentUpdater::SetImageExtentToParent(SVTaskObjectClass* pTaskObject,SVCommandExtentResetModeEnum resetMode)
{
	CWaitCursor wait;
	HRESULT status = SvOi::Err_10002_SetImageExtentToParent_InvalidParameter;

	if ( nullptr != pTaskObject )
	{
		SVInspectionProcess* pInspection = pTaskObject->GetInspection();
		if ( nullptr != pInspection )
		{
			SVCommandInspectionSetImageExtentPtr commandPtr = new SVCommandInspectionExtentUpdater( pInspection->GetUniqueObjectID(), pTaskObject->GetUniqueObjectID(), ExtentUpdaterMode_SetImageExtentToParent);
			commandPtr->SetResetInspection(resetMode);
			SVObjectSynchronousCommandTemplate< SVCommandInspectionSetImageExtentPtr > command( pInspection->GetUniqueObjectID(), commandPtr );
			status = command.Execute( TIMEOUT_FOR_SYNCHRONOUS_EXECUTE_IN_MS );
		}
	}

	return status;
}


HRESULT SVGuiExtentUpdater::SetImageExtentToFit(SVTaskObjectClass* pTaskObject, const SVImageExtentClass& rExtents ,SVCommandExtentResetModeEnum resetMode )
{
	CWaitCursor wait;
	HRESULT status = SvOi::Err_10003_SetImageExtentToFit_InvalidParameter;

	if ( nullptr != pTaskObject )
	{
		SVInspectionProcess* pInspection = pTaskObject->GetInspection();
		if ( nullptr != pInspection )
		{
			SVCommandInspectionSetImageExtentPtr commandPtr = new SVCommandInspectionExtentUpdater( pInspection->GetUniqueObjectID(), pTaskObject->GetUniqueObjectID(), ExtentUpdaterMode_SetImageExtentToFit, &rExtents );
			commandPtr->SetResetInspection(resetMode);
			SVObjectSynchronousCommandTemplate< SVCommandInspectionSetImageExtentPtr > command( pInspection->GetUniqueObjectID(), commandPtr );
			status = command.Execute( TIMEOUT_FOR_SYNCHRONOUS_EXECUTE_IN_MS );
		}
	}

	return status;
}


HRESULT SVGuiExtentUpdater::ForwardSizeAndPosition(SVTaskObjectClass* pTaskObject,SVCommandExtentResetModeEnum resetMode )
{
	CWaitCursor wait;
	HRESULT status = SvOi::Err_10002_SetImageExtentToParent_InvalidParameter;

	if ( nullptr != pTaskObject )
	{
		SVInspectionProcess* pInspection = pTaskObject->GetInspection();
		if ( nullptr != pInspection )
		{
			SVCommandInspectionSetImageExtentPtr commandPtr = new SVCommandInspectionExtentUpdater( pInspection->GetUniqueObjectID(), pTaskObject->GetUniqueObjectID(), ExtentUpdaterMode_ForwardExtent);
			commandPtr->SetResetInspection(resetMode);
			SVObjectSynchronousCommandTemplate< SVCommandInspectionSetImageExtentPtr > command( pInspection->GetUniqueObjectID(), commandPtr );
			status = command.Execute( TIMEOUT_FOR_SYNCHRONOUS_EXECUTE_IN_MS );
		}
	}

	return status;
}




