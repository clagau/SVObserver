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
#include "SVCommandInspectionExtentUpdater.h"
#include "SVInspectionProcess.h"
#include "InspectionEngine/SVTaskObject.h"
#include "SVCommandLibrary/SVObjectSynchronousCommandTemplate.h"
#include "SVStatusLibrary/ErrorNumbers.h"
#pragma endregion Includes

HRESULT SVGuiExtentUpdater::SetImageExtent(SvIe::SVTaskObjectClass* pTaskObject, const SVImageExtentClass& rExtents)
{
	CWaitCursor wait;

	HRESULT status = SvStl::Err_10001_SetImageExtent_InvalidParameter;
	if ( nullptr != pTaskObject )
	{
		SVInspectionProcess* pInspection = dynamic_cast<SVInspectionProcess*>(pTaskObject->GetInspection());
		if ( nullptr != pInspection )
		{
			SVCommandInspectionSetImageExtentPtr commandPtr{ new SVCommandInspectionExtentUpdater(pInspection->getObjectId(), pTaskObject->getObjectId(), ExtentUpdaterMode_SetImageExtent, &rExtents) };
			SVObjectSynchronousCommandTemplate< SVCommandInspectionSetImageExtentPtr > command( pInspection->getObjectId(), commandPtr );
			status = command.Execute( TIMEOUT_FOR_SYNCHRONOUS_EXECUTE_IN_MS );
		}
	}

	return status;
}

HRESULT SVGuiExtentUpdater::SetImageExtentToFit(SvIe::SVTaskObjectClass* pTaskObject, const SVImageExtentClass& rExtents)
{
	CWaitCursor wait;
	HRESULT status = SvStl::Err_10003_SetImageExtentToFit_InvalidParameter;

	if ( nullptr != pTaskObject )
	{
		SVInspectionProcess* pInspection = dynamic_cast<SVInspectionProcess*>(pTaskObject->GetInspection());
		if ( nullptr != pInspection )
		{
			SVCommandInspectionSetImageExtentPtr commandPtr{ new SVCommandInspectionExtentUpdater(pInspection->getObjectId(), pTaskObject->getObjectId(), ExtentUpdaterMode_SetImageExtentToFit, &rExtents) };
			SVObjectSynchronousCommandTemplate< SVCommandInspectionSetImageExtentPtr > command( pInspection->getObjectId(), commandPtr );
			status = command.Execute( TIMEOUT_FOR_SYNCHRONOUS_EXECUTE_IN_MS );
		}
	}

	return status;
}