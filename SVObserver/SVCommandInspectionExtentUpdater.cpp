//*****************************************************************************
/// \copyright (c) 2015,2015 by Seidenader Maschinenbau GmbH
/// All Rights Reserved 
//*****************************************************************************
/// Class for synchronize method call (extent update methods) with the inspection thread. 
//*****************************************************************************

#pragma region Includes
#include "stdafx.h"

#include "SVCommandInspectionExtentUpdater.h"
#include "SVObjectLibrary/SVObjectManagerClass.h"
#include "SVInspectionProcess.h"
#include "InspectionEngine/SVTool.h"
#include "SVStatusLibrary/ErrorNumbers.h"
#include "SVToolSet.h"
#include "ToolSizeAdjustTask.h"
#pragma endregion Includes

#pragma region Declarations
#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif
#pragma endregion Declarations

#pragma region Constructor
SVCommandInspectionExtentUpdater::SVCommandInspectionExtentUpdater(const SVCommandInspectionExtentUpdater& rObject)
: m_InspectionId( rObject.m_InspectionId ), m_ToolId( rObject.m_ToolId ), m_ImageExtent( rObject.m_ImageExtent ), m_mode( rObject.m_mode ),m_ResetMode(ResetMode_Tool)
{
}

SVCommandInspectionExtentUpdater::SVCommandInspectionExtentUpdater(const SVGUID& rInspectionId, const SVGUID& rToolId, SVCommandExtentUpdaterModeEnum mode, const SVImageExtentClass* pImageExtent)
: m_InspectionId( rInspectionId ), m_ToolId( rToolId ), m_mode( mode ),m_ResetMode(ResetMode_Tool)
{
	if(nullptr != pImageExtent )
	{
		m_ImageExtent = *(pImageExtent);
	}
}

SVCommandInspectionExtentUpdater::~SVCommandInspectionExtentUpdater()
{
}
#pragma endregion Constructor

int SVCommandInspectionExtentUpdater::ResetToolSizeAdjustTool(SVObjectClass* pObject)
{

	if( TRUE == ToolSizeAdjustTask::UseSizeAdjust(pObject))
	{
		bool Result = false;
		if (nullptr != pObject)
		{
			Result = pObject->resetAllObjects();
		}
		if(!Result)
		{
			ASSERT(FALSE);
			return 0;
		}

	}
	return 1;

}


#pragma region Public Methods
HRESULT SVCommandInspectionExtentUpdater::Execute()
{
	HRESULT retVal = S_OK;

	SVObjectClass* pObject = SVObjectManagerClass::Instance().GetObject( m_InspectionId );
	SVInspectionProcess* pInspection = dynamic_cast< SVInspectionProcess* >( pObject );
	pObject = SVObjectManagerClass::Instance().GetObject( m_ToolId );
	SVToolClass* pTool = dynamic_cast< SVToolClass* >( pObject );

	if( nullptr != pInspection && nullptr != pTool)
	{
		SVProductInfoStruct svProduct = pInspection->LastProductGet();

		SVDataManagerHandle dMIndexHandle;

		svProduct.GetResultDataIndex( dMIndexHandle );
		bool ResetModeAuto(true);
		if(m_ResetMode == ResetMode_Inspection || m_ResetMode == ResetMode_ToolList ||  m_mode == ExtentUpdaterMode_ForwardExtent)
		{
			ResetModeAuto = false;
		}
		int index = dMIndexHandle.GetIndex();
		switch (m_mode)
		{
		case ExtentUpdaterMode_SetImageExtent:
			retVal = pTool->SetImageExtent(m_ImageExtent);
			break;
		case ExtentUpdaterMode_SetImageExtentToParent:
			retVal = pTool->SetImageExtentToParent();
			break;
		case ExtentUpdaterMode_SetImageExtentToFit:
			retVal = pTool->SetImageExtentToFit(m_ImageExtent);
			break;
		case ExtentUpdaterMode_ForwardExtent:
			ResetModeAuto = false;
			retVal = S_OK;
			break;
		
		default:
			retVal = SvStl::Err_10004_SVCommandInspectionExtentUpdater_InvalidMode;
			break;
		}

		if (S_OK == retVal)
		{
			pInspection->m_bForceOffsetUpdate = true;
			if(ResetModeAuto)
			{
				/// correct tool size when it does not fit to the parent image 
				pInspection->AddResetState( SvDef::SVResetAutoMoveAndResize );
			}
			
			SVToolClass* pToolRun(nullptr); 
			bool result(false); 
			if(m_ResetMode == ResetMode_Inspection )
			{
				SVObjectClass*  pResetObject =  dynamic_cast<SVObjectClass*>  (pInspection);
				if (nullptr != pResetObject)
				{
					result = pResetObject->resetAllObjects();
				}
			}
			else if ( m_ResetMode == ResetMode_ToolList)
			{
				if(pInspection)
				{
					
					int LoopCount(0);
					pInspection->LoopOverTools( SVCommandInspectionExtentUpdater::ResetToolSizeAdjustTool, LoopCount); 
					if(LoopCount > 0)
					{
						result = true;
					}
				}
			
			}
			else
			{
				pToolRun = pTool;
				SVObjectClass*  pResetObject =  dynamic_cast<SVObjectClass*>  (pTool);
				if (nullptr != pResetObject)
				{
					result = pResetObject->resetAllObjects();
				}
			}

			if ( result )
			{
				retVal = pInspection->RunOnce( pToolRun ) ? S_OK : SvStl::Err_10006_SVCommandInspectionExtentUpdater_RunOnce;
				
			}
			else
			{
				retVal = SvStl::Err_10005_SVCommandInspectionExtentUpdater_ResetAllObjects;
			}

			if(ResetModeAuto)
			{
				pInspection->RemoveResetState(SvDef::SVResetAutoMoveAndResize );
			}
			
		}
	}
	else
	{
		retVal = SvStl::Err_10007_SVCommandInspectionExtentUpdater_InvalidParameter;
	}

	return retVal;
}

bool SVCommandInspectionExtentUpdater::empty() const
{
	bool bRet = true;

	bRet = bRet && ( m_InspectionId.empty() );
	bRet = bRet && ( m_ToolId.empty() );
	bRet = bRet && ( m_mode == ExtentUpdaterMode_Undefined );

	return bRet;
}

void SVCommandInspectionExtentUpdater::SetResetInspection(SVCommandExtentResetModeEnum resetMode)
{
	
	m_ResetMode = resetMode;
}

#pragma endregion Public Methods