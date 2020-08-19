//*****************************************************************************
/// \copyright (c) 2003,2018 by Seidenader Maschinenbau GmbH
/// \file SVPixelAnalyzer.cpp
/// All Rights Reserved 
//*****************************************************************************
/// This class is the TaskObjectList-Implementation of the PixelAnalyzer 
//*****************************************************************************
#pragma region Includes
#include "stdafx.h"
#include "SVPixelAnalyzer.h"
#include "InspectionEngine/SVImageClass.h"
#include "InspectionEngine/SVImageProcessingClass.h"
#include "Operators/SVRange.h"
#include "Operators/SVResultLong.h"
#include "SVMatroxLibrary/SVMatroxBuffer.h"
#include "SVMatroxLibrary/SVMatroxImageInterface.h"
#include "SVMatroxLibrary/SVMatroxResourceMonitor.h"
#include "SVStatusLibrary/MessageManager.h"
#include "SVStatusLibrary/SVRunStatus.h"
#pragma endregion Includes

namespace SvAo
{

#pragma region Declarations
#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif
#pragma endregion Declarations

SV_IMPLEMENT_CLASS( SVPixelAnalyzer, SvPb::PixelAnalyzerClassId);

SVPixelAnalyzer::SVPixelAnalyzer( LPCSTR ObjectName )
: SVImageAnalyzerClass( ObjectName )
{
	init();
}

SVPixelAnalyzer::SVPixelAnalyzer(SVObjectClass* POwner, int StringResourceID )
: SVImageAnalyzerClass( POwner, StringResourceID ) 
{
	init();
}

void SVPixelAnalyzer::init()
{
	// Identify our output type
	m_outObjectInfo.m_ObjectTypeInfo.m_SubType = SvPb::SVPixelAnalyzerObjectType;

	// Identify our input type needs

	// Register Embedded Objects
	RegisterEmbeddedObject(&m_pixelCount, SvPb::PixelCountEId, IDS_OBJECTNAME_PIXEL_COUNT, false, SvOi::SVResetItemNone);

	RegisterEmbeddedObject(&m_pixelCountColor, SvPb::PixelColorIndexEId,	IDS_OBJECTNAME_PIXEL_COLOR_INDEX, false, SvOi::SVResetItemNone);

	// Set Embedded defaults
	m_pixelCountColor. SetDefaultValue( 255, true ); // White
	m_pixelCount. SetDefaultValue( 0, true );
	m_pixelCount.setSaveValueFlag(false);

	// Set default inputs and outputs
	addDefaultInputObjects();

	// Instantiate Children
	SvOp::SVLongResultClass* pAnalyzerResult = new SvOp::SVLongResultClass(this, IDS_CLASSNAME_SVPIXELANALYZERESULT);

	if (nullptr != pAnalyzerResult)
	{
		Add(pAnalyzerResult);
	}
	else
	{
		SvStl::MessageMgrStd MesMan(SvStl::MsgType::Log );
		MesMan.setMessage( SVMSG_SVO_103_REPLACE_ERROR_TRAP, SvStl::Tid_UnexpectedError, SvStl::SourceFileParams(StdMessageParams), 0, getObjectId());
	}
}

SVPixelAnalyzer::~SVPixelAnalyzer()
{
	SVPixelAnalyzer::CloseObject();
}

bool SVPixelAnalyzer::CloseObject()
{
	SVMatroxImageInterface::Destroy(m_contextID);
	SVMatroxImageInterface::Destroy(m_ResultID);

	return SVImageAnalyzerClass::CloseObject();
}

bool SVPixelAnalyzer::CreateObject( const SVObjectLevelCreateStruct& rCreateStructure )
{
	bool isError(false);
    	
    while (1)
    {
        if (! SVImageAnalyzerClass::CreateObject(rCreateStructure) )
        {
			
			SvStl::MessageMgrStd MesMan(SvStl::MsgType::Log );
			MesMan.setMessage( SVMSG_SVO_103_REPLACE_ERROR_TRAP, SvStl::Tid_UnexpectedError, SvStl::SourceFileParams(StdMessageParams), 0, getObjectId());
			isError = true;
			break;
        }
		
		SvIe::SVImageClass* pImage = getInputImage ();
		
        if (!pImage)
        {
			SvStl::MessageMgrStd MesMan(SvStl::MsgType::Log );
			MesMan.setMessage( SVMSG_SVO_103_REPLACE_ERROR_TRAP, SvStl::Tid_UnexpectedError, SvStl::SourceFileParams(StdMessageParams), 0, getObjectId());
			isError = true;
			break;
        }

        // &&&
		m_contextID = MimAlloc(M_DEFAULT_HOST, M_STATISTICS_CONTEXT, M_DEFAULT, M_NULL);
		m_ResultID = MimAllocResult(M_DEFAULT_HOST, M_DEFAULT, M_STATISTICS_RESULT, M_NULL);
		
		if( M_NULL == m_contextID || M_NULL == m_ResultID )
		{
			SvStl::MessageMgrStd MesMan(SvStl::MsgType::Log );
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
	m_pixelCount.SetObjectAttributesAllowed( SvPb::printable, SvOi::SetAttributeType::RemoveAttribute );
	m_pixelCountColor.SetObjectAttributesAllowed( SvPb::printable, SvOi::SetAttributeType::AddAttribute );
	m_pixelCountColor.SetOutputFormat(SvVol::OutputFormat_int);
	
	return m_isCreated;
}

bool SVPixelAnalyzer::onRun(SVRunStatusClass &rRunStatus, SvStl::MessageContainerVector *pErrorMessages)
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

		SvTrc::IImagePtr pImageBuffer = pInputImage->getImageReadOnly(rRunStatus.m_triggerRecord.get());
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

bool SVPixelAnalyzer::ResetObject(SvStl::MessageContainerVector *pErrorMessages)
{
	bool Result = __super::ResetObject(pErrorMessages);

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

void SVPixelAnalyzer::addParameterForMonitorList(SvStl::MessageContainerVector& rMessages, std::back_insert_iterator<SvOi::ParametersForML> inserter) const
{
	bool isNoError = true;

	inserter = SvOi::ParameterPairForML(m_pixelCount.GetCompleteName(), m_pixelCount.getObjectId());
	SvOp::SVRangeClass* pRangeObject = dynamic_cast<SvOp::SVRangeClass*>(getFirstObject(SvDef::SVObjectTypeInfoStruct(SvPb::SVObjectTypeEnum::SVRangeObjectType)));
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
