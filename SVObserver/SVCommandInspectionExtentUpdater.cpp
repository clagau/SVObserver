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
#include "SVTool.h"
#include "ErrorNumbers.h"
#pragma endregion Includes

#pragma region Declarations
#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif
#pragma endregion Declarations

#pragma region Constructor
SVCommandInspectionExtentUpdater::SVCommandInspectionExtentUpdater()
: m_InspectionId(), m_ToolId(), m_mode(ExtentUpdaterMode_Undefined)
{
}

SVCommandInspectionExtentUpdater::SVCommandInspectionExtentUpdater(const SVCommandInspectionExtentUpdater& rObject)
: m_InspectionId( rObject.m_InspectionId ), m_ToolId( rObject.m_ToolId ), m_ImageExtent( rObject.m_ImageExtent ), m_mode( rObject.m_mode )
{
}

SVCommandInspectionExtentUpdater::SVCommandInspectionExtentUpdater(const SVGUID& rInspectionId, const SVGUID& rToolId, SVCommandExtentUpdaterModeEnum mode, const SVImageExtentClass& rImageExtent)
: m_InspectionId( rInspectionId ), m_ToolId( rToolId ), m_ImageExtent(rImageExtent), m_mode( mode )
{
}

SVCommandInspectionExtentUpdater::~SVCommandInspectionExtentUpdater()
{
}
#pragma endregion Constructor

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
		SVProductInfoStruct svProduct = pInspection->LastProductGet(SV_DISPLAY);

		SVDataManagerHandle dMIndexHandle;

		svProduct.GetResultDataIndex( dMIndexHandle );

		switch (m_mode)
		{
		case ExtentUpdaterMode_SetImageExtent:
			retVal = pTool->SetImageExtent(dMIndexHandle.GetIndex(), m_ImageExtent);
			break;
		case ExtentUpdaterMode_SetImageExtentToParent:
			retVal = pTool->SetImageExtentToParent(dMIndexHandle.GetIndex());
			break;
		case ExtentUpdaterMode_SetImageExtentToFit:
			retVal = pTool->SetImageExtentToFit(dMIndexHandle.GetIndex(), m_ImageExtent);
			break;
		default:
			retVal = Err_SVCommandInspectionExtentUpdater_InvalidMode_2004;
			break;
		}

		if (retVal == S_OK)
		{
			pInspection->m_bForceOffsetUpdate = true;
			pInspection->AddResetState( SVResetAutoMoveAndResize );

			if (::SVSendMessage(pTool, SVM_RESET_ALL_OBJECTS, NULL, NULL) != SVMR_SUCCESS)
			{
				retVal = Err_SVCommandInspectionExtentUpdater_ResetAllObjects_2005;
			}
			else
			{
				retVal = pInspection->RunOnce( pTool ) ? S_OK : Err_SVCommandInspectionExtentUpdater_RunOnce_2006;
			}

			pInspection->RemoveResetState( SVResetAutoMoveAndResize );
		}
	}
	else
	{
		retVal = Err_SVCommandInspectionExtentUpdater_InvalidParameter_2007;
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

void SVCommandInspectionExtentUpdater::clear()
{
	m_InspectionId.clear();
	m_ToolId.clear();
	m_ImageExtent = SVImageExtentClass();
	m_mode = ExtentUpdaterMode_Undefined;
}

HRESULT SVCommandInspectionExtentUpdater::SetCommandData(const SVGUID& p_rInspectionId, const SVGUID& p_rToolId, SVCommandExtentUpdaterModeEnum mode, const SVImageExtentClass& rImageExtent)
{
	m_InspectionId = p_rInspectionId;
	m_ToolId = p_rToolId;
	m_ImageExtent = rImageExtent;
	m_mode = mode;

	return S_OK;
}
#pragma endregion Public Methods