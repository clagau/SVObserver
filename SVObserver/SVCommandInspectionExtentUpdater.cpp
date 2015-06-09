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
#include "ObjectInterfaces\ErrorNumbers.h"
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
: m_InspectionId(), m_ToolId(), m_mode(ExtentUpdaterMode_Undefined), m_bResetInspection(false)
{
}

SVCommandInspectionExtentUpdater::SVCommandInspectionExtentUpdater(const SVCommandInspectionExtentUpdater& rObject)
: m_InspectionId( rObject.m_InspectionId ), m_ToolId( rObject.m_ToolId ), m_ImageExtent( rObject.m_ImageExtent ), m_mode( rObject.m_mode ),m_bResetInspection(false)
{
}

SVCommandInspectionExtentUpdater::SVCommandInspectionExtentUpdater(const SVGUID& rInspectionId, const SVGUID& rToolId, SVCommandExtentUpdaterModeEnum mode, const SVImageExtentClass* pImageExtent)
: m_InspectionId( rInspectionId ), m_ToolId( rToolId ), m_mode( mode ),m_bResetInspection(false)
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

		int index = dMIndexHandle.GetIndex();
		switch (m_mode)
		{
		case ExtentUpdaterMode_SetImageExtent:
			retVal = pTool->SetImageExtent(index, m_ImageExtent);
			break;
		case ExtentUpdaterMode_SetImageExtentToParent:
			retVal = pTool->SetImageExtentToParent(index);
			break;
		case ExtentUpdaterMode_SetImageExtentToFit:
			retVal = pTool->SetImageExtentToFit(index, m_ImageExtent);
			break;
		case ExtentUpdaterMode_ForwardExtent:
			retVal = S_OK;
			break;
		
		default:
			retVal = SvOi::Err_10004_SVCommandInspectionExtentUpdater_InvalidMode;
			break;
		}

		if (retVal == S_OK)
		{
			pInspection->m_bForceOffsetUpdate = true;
			if(!m_bResetInspection)
			{
				/// correct tool size when it does not fit to the parent image 
				pInspection->AddResetState( SVResetAutoMoveAndResize );
			}
			

			SVObjectClass* pResetObject(nullptr);
			SVToolClass* pToolRun(nullptr); 
			if(m_bResetInspection)
			{
				pResetObject =  dynamic_cast<SVObjectClass*>  (pInspection);
			}
			else
			{
				pToolRun = pTool;
				pResetObject =  dynamic_cast<SVObjectClass*>  (pTool);
			}
			

			if ( nullptr == pResetObject || ::SVSendMessage(pResetObject, SVM_RESET_ALL_OBJECTS, NULL, NULL) != SVMR_SUCCESS)
			{
				retVal = SvOi::Err_10005_SVCommandInspectionExtentUpdater_ResetAllObjects;
			}
			else
			{
				retVal = pInspection->RunOnce( pToolRun ) ? S_OK : SvOi::Err_10006_SVCommandInspectionExtentUpdater_RunOnce;
			}

			if(!m_bResetInspection)
			{
				pInspection->RemoveResetState( SVResetAutoMoveAndResize );
			}
			
		}
	}
	else
	{
		retVal = SvOi::Err_10007_SVCommandInspectionExtentUpdater_InvalidParameter;
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
	m_bResetInspection  = false;
}

HRESULT SVCommandInspectionExtentUpdater::SetCommandData(const SVGUID& p_rInspectionId, const SVGUID& p_rToolId, SVCommandExtentUpdaterModeEnum mode, const SVImageExtentClass& rImageExtent, bool forward)
{
	m_InspectionId = p_rInspectionId;
	m_ToolId = p_rToolId;
	m_ImageExtent = rImageExtent;
	m_mode = mode;
	m_bResetInspection  = forward;

	return S_OK;
}

void SVCommandInspectionExtentUpdater::SetResetInspection(bool forward)
{
	m_bResetInspection = forward;
}

#pragma endregion Public Methods