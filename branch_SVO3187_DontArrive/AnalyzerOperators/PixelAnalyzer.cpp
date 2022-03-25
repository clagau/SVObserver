//*****************************************************************************
/// \copyright (c) 2003,2018 by Körber Pharma Inspection GmbH. All Rights Reserved
/// \file PixelAnalyzer.cpp
/// All Rights Reserved 
//*****************************************************************************
/// This class is the TaskObjectList-Implementation of the PixelAnalyzer 
//*****************************************************************************
#pragma region Includes
#include "stdafx.h"
#include "PixelAnalyzer.h"
#include "InspectionEngine/SVImageClass.h"
#include "InspectionEngine/SVImageProcessingClass.h"
#include "Operators/SVRange.h"
#include "Operators/SVResultLong.h"
#include "SVMatroxLibrary/SVMatroxBuffer.h"
#include "SVMatroxLibrary/SVMatroxImageInterface.h"
#include "SVMatroxLibrary/SVMatroxResourceMonitor.h"
#include "SVStatusLibrary/MessageManager.h"
#include "InspectionEngine/RunStatus.h"
#pragma endregion Includes

namespace SvAo
{

#pragma region Declarations
#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif
#pragma endregion Declarations

SV_IMPLEMENT_CLASS( PixelAnalyzer, SvPb::PixelAnalyzerClassId);

PixelAnalyzer::PixelAnalyzer( LPCSTR ObjectName )
: ImageAnalyzer( ObjectName )
{
	init();
}

PixelAnalyzer::PixelAnalyzer(SVObjectClass* POwner, int StringResourceID )
: ImageAnalyzer( POwner, StringResourceID ) 
{
	init();
}

void PixelAnalyzer::init()
{
	// Identify our output type
	m_ObjectTypeInfo.m_SubType = SvPb::PixelAnalyzerObjectType;

	// Identify our input type needs

	// Register Embedded Objects
	RegisterEmbeddedObject(&m_pixelCount, SvPb::PixelCountEId, IDS_OBJECTNAME_PIXEL_COUNT, false, SvOi::SVResetItemNone);

	RegisterEmbeddedObject(&m_pixelCountColor, SvPb::PixelColorIndexEId,	IDS_OBJECTNAME_PIXEL_COLOR_INDEX, false, SvOi::SVResetItemNone);

	// Set Embedded defaults
	m_pixelCountColor. SetDefaultValue( 255, true ); // White
	m_pixelCount. SetDefaultValue( 0, true );
	m_pixelCount.setSaveValueFlag(false);

	// Instantiate Children
	SvOp::SVLongResult* pAnalyzerResult = new SvOp::SVLongResult(this, IDS_CLASSNAME_SVPIXELANALYZERRESULT);

	if (nullptr != pAnalyzerResult)
	{
		Add(pAnalyzerResult);
	}
	else
	{
		SvStl::MessageManager MesMan(SvStl::MsgType::Log );
		MesMan.setMessage( SVMSG_SVO_103_REPLACE_ERROR_TRAP, SvStl::Tid_UnexpectedError, SvStl::SourceFileParams(StdMessageParams), 0, getObjectId());
	}
}

PixelAnalyzer::~PixelAnalyzer()
{
	PixelAnalyzer::CloseObject();
}

bool PixelAnalyzer::CloseObject()
{
	SVMatroxImageInterface::Destroy(m_contextID);
	SVMatroxImageInterface::Destroy(m_ResultID);
	m_contextID = M_NULL;
	m_ResultID = M_NULL;
	return ImageAnalyzer::CloseObject();
}

bool PixelAnalyzer::CreateObject( const SVObjectLevelCreateStruct& rCreateStructure )
{
	bool isError(false);
    	
    while (1)
    {
        if (! ImageAnalyzer::CreateObject(rCreateStructure) )
        {
			
			SvStl::MessageManager MesMan(SvStl::MsgType::Log );
			MesMan.setMessage( SVMSG_SVO_103_REPLACE_ERROR_TRAP, SvStl::Tid_UnexpectedError, SvStl::SourceFileParams(StdMessageParams), 0, getObjectId());
			isError = true;
			break;
        }
		
		SVMatroxImageInterface::Destroy(m_contextID);
		SVMatroxImageInterface::Destroy(m_ResultID);
		
		m_contextID = MimAlloc(M_DEFAULT_HOST, M_STATISTICS_CONTEXT, M_DEFAULT, M_NULL);
		m_ResultID = MimAllocResult(M_DEFAULT_HOST, M_DEFAULT, M_STATISTICS_RESULT, M_NULL);
		
		if( M_NULL == m_contextID || M_NULL == m_ResultID )
		{
			SvStl::MessageManager MesMan(SvStl::MsgType::Log );
			MesMan.setMessage( SVMSG_SVO_103_REPLACE_ERROR_TRAP, SvStl::Tid_UnexpectedError, SvStl::SourceFileParams(StdMessageParams), 0, getObjectId());
			isError = true;
			break;
        }
		else
		{
			SVMatroxResourceMonitor::InsertIdentifier(SVImageID, m_contextID);
			SVMatroxResourceMonitor::InsertIdentifier(SVImageID, m_ResultID);
		}
        break;
    }
	
	m_isCreated = !isError;
	
	// Set / Reset Printable Flags
	m_pixelCount.SetObjectAttributesAllowed( SvPb::audittrail, SvOi::SetAttributeType::RemoveAttribute );
	m_pixelCountColor.SetObjectAttributesAllowed( SvPb::audittrail, SvOi::SetAttributeType::AddAttribute );
	m_pixelCountColor.SetOutputFormat(SvVol::OutputFormat_int);
	
	return m_isCreated;
}

bool PixelAnalyzer::onRun(SvIe::RunStatus &rRunStatus, SvStl::MessageContainerVector *pErrorMessages)
{
	bool Result{true};
	while (1)
	{
		SvIe::SVImageClass* pInputImage = getInputImage(true);

		if( ! pInputImage )
		{
			Result = false;
			if (nullptr != pErrorMessages)
			{
				SvStl::MessageContainer Msg( SVMSG_SVO_103_REPLACE_ERROR_TRAP, SvStl::Tid_UnexpectedError, SvStl::SourceFileParams(StdMessageParams), 0, getObjectId() );
				pErrorMessages->push_back(Msg);
			}
			break;
		}

		SvOi::ITRCImagePtr pImageBuffer = pInputImage->getImageReadOnly(rRunStatus.m_triggerRecord.get());
		if ( nullptr == pImageBuffer || pImageBuffer->isEmpty())
		{
			Result = false;
			if (nullptr != pErrorMessages)
			{
				SvStl::MessageContainer Msg( SVMSG_SVO_103_REPLACE_ERROR_TRAP, SvStl::Tid_UnexpectedError, SvStl::SourceFileParams(StdMessageParams), 0, getObjectId() );
				pErrorMessages->push_back(Msg);
			}
			break;
		}

		MimStatCalculate(m_contextID, pImageBuffer->getHandle()->GetBuffer().GetIdentifier(), m_ResultID, M_DEFAULT);
		MIL_INT32 NumerOfPixels = 0;
		MimGetResult(m_ResultID, M_STAT_NUMBER+ M_TYPE_MIL_INT32, &NumerOfPixels);
		m_pixelCount.SetValue(NumerOfPixels);
			break;
		}

	if (!Result)
	{
		rRunStatus.SetInvalid ();
	}
	return Result;
}

bool PixelAnalyzer::ResetObject(SvStl::MessageContainerVector *pErrorMessages)
{
	bool Result = __super::ResetObject(pErrorMessages);

	SvIe::SVImageClass* pImage = getInputImage();
	if (!pImage)
	{
		if (nullptr != pErrorMessages)
		{
			SvStl::MessageContainer Msg(SVMSG_SVO_103_REPLACE_ERROR_TRAP, SvStl::Tid_UnexpectedError, SvStl::SourceFileParams(StdMessageParams), 0, getObjectId());
			pErrorMessages->push_back(Msg);
		}
		Result = false;
	}

	if (M_NULL == m_contextID || M_NULL == m_ResultID)
	{
		if (nullptr != pErrorMessages)
		{
			SvStl::MessageContainer Msg(  SVMSG_SVO_103_REPLACE_ERROR_TRAP, SvStl::Tid_UnexpectedError, SvStl::SourceFileParams(StdMessageParams), 0, getObjectId() );
			pErrorMessages->push_back(Msg);
		}
		Result = false;
	}
	MimControl(m_contextID, M_STAT_NUMBER, M_ENABLE);
	MimControl(m_contextID, M_CONDITION, M_EQUAL);
	BYTE byIndex {0};
	m_pixelCountColor.GetValue(byIndex);
	MimControl(m_contextID, M_COND_LOW, byIndex);

	return Result;
}

void PixelAnalyzer::addParameterForMonitorList(SvStl::MessageContainerVector& rMessages, std::back_insert_iterator<SvOi::ParametersForML> inserter) const
{
	bool isNoError = true;

	inserter = SvOi::ParameterPairForML(m_pixelCount.GetCompleteName(), m_pixelCount.getObjectId());
	SvOp::SVRange* pRangeObject = dynamic_cast<SvOp::SVRange*>(getFirstObject(SvDef::SVObjectTypeInfoStruct(SvPb::SVObjectTypeEnum::SVRangeObjectType)));
	if (nullptr != pRangeObject)
	{
		pRangeObject->addEntriesToMonitorList(inserter);
	}
	else
	{
		isNoError = false;
	}

	if (!isNoError)
	{
		SvStl::MessageContainer Msg(SVMSG_SVO_92_GENERAL_ERROR, SvStl::Tid_SetParameterToMonitorListFailed, SvStl::SourceFileParams(StdMessageParams), 0, getObjectId());
		rMessages.push_back(Msg);
	}
}

} //namespace SvAo
