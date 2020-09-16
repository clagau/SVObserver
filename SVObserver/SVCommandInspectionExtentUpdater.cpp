//*****************************************************************************
/// \copyright (c) 2015,2015 by Seidenader Maschinenbau GmbH
/// All Rights Reserved 
//*****************************************************************************
/// Class for synchronize method call (extent update methods) with the inspection thread. 
//*****************************************************************************

#pragma region Includes
#include "stdafx.h"

#include "SVCommandInspectionExtentUpdater.h"
#include "SVInspectionProcess.h"
#include "SVObjectLibrary/SVObjectManagerClass.h"
#include "SVStatusLibrary/ErrorNumbers.h"
#include "Tools/SVTool.h"
#pragma endregion Includes

#pragma region Declarations
#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif
#pragma endregion Declarations


CommandInspectionExtentUpdater::CommandInspectionExtentUpdater(uint32_t inspectionId, uint32_t toolId, SVCommandExtentUpdaterModeEnum mode, const SVImageExtentClass* pImageExtent)
	: m_InspectionId(inspectionId), m_ToolId(toolId), m_mode(mode)
{
	if (nullptr != pImageExtent)
	{
		m_ImageExtent = *(pImageExtent);
	}
}

CommandInspectionExtentUpdater::~CommandInspectionExtentUpdater()
{
}



HRESULT CommandInspectionExtentUpdater::operator()()
{
	HRESULT retVal = S_OK;

	SVObjectClass* pObject = SVObjectManagerClass::Instance().GetObject(m_InspectionId);
	SVInspectionProcess* pInspection = dynamic_cast<SVInspectionProcess*>(pObject);
	pObject = SVObjectManagerClass::Instance().GetObject(m_ToolId);
	SvTo::SVToolClass* pTool = dynamic_cast<SvTo::SVToolClass*> (pObject);

	if (nullptr != pInspection && nullptr != pTool)
	{
		switch (m_mode)
		{
		case ExtentUpdaterMode_SetImageExtent:
			retVal = pTool->SetImageExtent(m_ImageExtent);
			break;
		case ExtentUpdaterMode_SetImageExtentToFit:
			retVal = pTool->SetImageExtentToFit(m_ImageExtent);
			break;
		default:
			retVal = SvStl::Err_10004_SVCommandInspectionExtentUpdater_InvalidMode;
			break;
		}

		if (S_OK == retVal)
		{
			pInspection->ForceOffsetUpdate();
			/// correct tool size when it does not fit to the parent image 
			pInspection->AddResetState(SvDef::SVResetAutoMoveAndResize);

			bool result(false);
			SVObjectClass* pResetObject = dynamic_cast<SVObjectClass*>  (pTool);
			if (nullptr != pResetObject)
			{
				result = pResetObject->resetAllObjects();
			}

			if (result)
			{
				retVal = pInspection->RunOnce();

			}
			else
			{
				retVal = SvStl::Err_10005_SVCommandInspectionExtentUpdater_ResetAllObjects;
			}
			pInspection->RemoveResetState(SvDef::SVResetAutoMoveAndResize);
		}
	}
	else
	{
		retVal = SvStl::Err_10007_SVCommandInspectionExtentUpdater_InvalidParameter;
	}

	return retVal;
}

bool CommandInspectionExtentUpdater::empty() const
{
	bool bRet = true;

	bRet = bRet && (SvDef::InvalidObjectId == m_InspectionId);
	bRet = bRet && (SvDef::InvalidObjectId == m_ToolId);
	bRet = bRet && (m_mode == ExtentUpdaterMode_Undefined);

	return bRet;
}