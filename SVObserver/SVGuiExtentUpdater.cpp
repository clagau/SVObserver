//******************************************************************************
//* COPYRIGHT (c) 2012 by Körber Pharma Inspection GmbH. All Rights Reserved
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
#pragma endregion Includes

HRESULT SVGuiExtentUpdater::UpdateImageExtent(SvIe::SVTaskObjectClass* pTaskObject, const SVImageExtentClass& rExtents)
{
	CWaitCursor wait;

	HRESULT status = E_FAIL;
	if ( nullptr != pTaskObject )
	{
		SVInspectionProcess* pInspection = dynamic_cast<SVInspectionProcess*>(pTaskObject->GetInspection());
		if ( nullptr != pInspection )
		{
			
			CommandInspectionExtentUpdater inspectionExtentUpdateFunctor(pInspection->getObjectId(), pTaskObject->getObjectId(), ExtentUpdaterMode_SetImageExtent, &rExtents);
			std::packaged_task<HRESULT()>  inspectionExtentUpdateTask(std::move(inspectionExtentUpdateFunctor));
	
		
			std::future<HRESULT>  inspectionUpdateFuture = inspectionExtentUpdateTask.get_future();
			SvOi::ICommandPtr pCommand = std::make_shared<  SvOi::CTaskWrapper<std::packaged_task<HRESULT()>>>(std::move(inspectionExtentUpdateTask));
			pInspection->SubmitCommand(pCommand);

			std::future_status fstatus = inspectionUpdateFuture.wait_for(std::chrono::milliseconds{ TIMEOUT_FOR_SYNCHRONOUS_EXECUTE_IN_MS });

			if (fstatus == std::future_status::ready)
			{
				status = inspectionUpdateFuture.get();
			}
		}
	}

	return status;
}

HRESULT SVGuiExtentUpdater::SetImageExtentToFit(SvIe::SVTaskObjectClass* pTaskObject, const SVImageExtentClass& rExtents)
{
	CWaitCursor wait;
	HRESULT status = E_FAIL;

	if ( nullptr != pTaskObject )
	{
		SVInspectionProcess* pInspection = dynamic_cast<SVInspectionProcess*>(pTaskObject->GetInspection());
		if ( nullptr != pInspection )
		{
	
			std::packaged_task<HRESULT()>  inspectionExtentUpdateTask(CommandInspectionExtentUpdater(pInspection->getObjectId(), pTaskObject->getObjectId(), ExtentUpdaterMode_SetImageExtentToFit, &rExtents));
			std::future<HRESULT>  inspectionUpdateFuture = inspectionExtentUpdateTask.get_future();
			SvOi::ICommandPtr pCommand = std::make_shared<  SvOi::CTaskWrapper<std::packaged_task<HRESULT()>>>(std::move(inspectionExtentUpdateTask));
			pInspection->SubmitCommand(pCommand);

			std::future_status fstatus = inspectionUpdateFuture.wait_for(std::chrono::milliseconds{ TIMEOUT_FOR_SYNCHRONOUS_EXECUTE_IN_MS });

			if (fstatus == std::future_status::ready)
			{
				status = inspectionUpdateFuture.get();
			}



		}
	}

	return status;
}