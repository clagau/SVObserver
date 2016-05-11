//******************************************************************************
//* COPYRIGHT (c) 2003 by SVResearch, Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVOperator
//* .File Name       : $Workfile:   SVOperator.cpp  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.5  $
//* .Check In Date   : $Date:   15 May 2014 11:21:48  $
//******************************************************************************

#pragma region Includes
#include "stdafx.h"
#include "SVOperator.h"
#include "SVImageLibrary/SVImageBufferHandleImage.h"
#include "SVMatroxLibrary/SVMatroxLibrary.h"
#include "SVObjectLibrary/SVAnalyzerLevelCreateStruct.h"
#include "SVObjectLibrary/SVObjectAttributeClass.h"
#include "SVObjectLibrary/SVObjectManagerClass.h"
#include "SVObserver.h"
#include "SVAnalyzer.h"
#include "SVAutoThresholdEquation.h"
#include "SVGlobal.h"
#include "SVInspectionProcess.h"
#include "SVLowerThresholdEquation.h"
#include "SVUnaryImageOperatorList.h"
#include "SVUpperThresholdEquation.h"
#include "SVImageProcessingClass.h"
#include "SVToolSet.h"
#include "SVTool.h"
#include "SVMaskShape.h"
#include "SVObjectLibrary\SVToolSetScriptTags.h"
#pragma endregion Includes

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

SV_IMPLEMENT_CLASS( SVOperatorClass, SVOperatorClassGuid )

SVOperatorClass::SVOperatorClass( SVObjectClass* POwner, int StringResourceID )
				:SVTaskObjectClass( POwner, StringResourceID )
{
	init();
}

SVOperatorClass::~SVOperatorClass()
{
}

BOOL SVOperatorClass::CreateObject( SVObjectLevelCreateStruct* PCreateStructure )
{
	BOOL bOk = false;

	if( SVTaskObjectClass::CreateObject( PCreateStructure ) && nullptr != GetTool() )
	{
		bOk = true;
	}

	m_isCreated = bOk;

	return bOk;
}

DWORD_PTR SVOperatorClass::processMessage(DWORD DwMessageID, DWORD_PTR DwMessageValue, DWORD_PTR DwMessageContext)
{
	DWORD_PTR DwResult = SVMR_NOT_PROCESSED;
	// Try to process message by yourself...
	DWORD dwPureMessageID = DwMessageID & SVM_PURE_MESSAGE;
	switch( dwPureMessageID )
	{
	case SVMSGID_RESET_ALL_OBJECTS:
		{
			HRESULT l_ResetStatus = ResetObject();
			if( S_OK != l_ResetStatus )
			{
				ASSERT( SUCCEEDED( l_ResetStatus ) );

				DwResult = SVMR_NO_SUCCESS;
			}
			else
			{
				DwResult = SVMR_SUCCESS;
			}
			break;
		}
	}
	return( SVTaskObjectClass::processMessage( DwMessageID, DwMessageValue, DwMessageContext ) | DwResult );
}

void SVOperatorClass::init()
{
	m_bUseOverlays = false;	// in general, operators don't have overlays

	// Identify our output type
	m_outObjectInfo.ObjectTypeInfo.ObjectType = SVOperatorObjectType;

	// Register Embedded Object(s)

	// Set Embedded defaults

	// Set default inputs and outputs
	addDefaultInputObjects();
}

