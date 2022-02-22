//******************************************************************************
//* COPYRIGHT (c) 2003 by Körber Pharma Inspection GmbH. All Rights Reserved
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : OCVAnalyzer
//* .File Name       : $Workfile: 
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.4  $
//* .Check In Date   : $Date:   15 May 2014 11:07:28  $
//******************************************************************************

#pragma region Includes
#include "stdafx.h"
#include "OCVAnalyzer.h"
#include "Operators/SVOCVAnalyzerResult.h"
#include "SVMatroxLibrary/SVOLicenseManager.h"
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

SV_IMPLEMENT_CLASS( OCVAnalyzer, SvPb::OCVAnalyzerClassId);

/////////////////////////////////////////////////////////////////////////////
OCVAnalyzer::OCVAnalyzer( SVObjectClass* POwner, int StringResourceID )
		:ImageAnalyzer( POwner, StringResourceID ) 
{
	init();

	if (SVOLicenseManager::Instance().HasMatroxIdentificationLicense() )
	{
		m_bHasLicenseError = false;
	}
	else
	{
		m_bHasLicenseError = true;
	}
}

////////////////////////////////////////////////////////////////////////////////
void OCVAnalyzer::init()
{
	m_ObjectTypeInfo.m_ObjectType = SvPb::SVAnalyzerObjectType;
	m_ObjectTypeInfo.m_SubType = SvPb::SVOCVAnalyzerObjectType;

	SvOp::SVOCVAnalyzerResult* pAnalyzerResult = new SvOp::SVOCVAnalyzerResult(this);

	// Children list defaults:
	if(nullptr != pAnalyzerResult )
	{
		Add(pAnalyzerResult);
	}

}


OCVAnalyzer::~OCVAnalyzer()
{
	if ( m_bHasLicenseError )
	{
		m_bHasLicenseError = false;
		SVOLicenseManager::Instance().RemoveLicenseErrorFromList(GetCompleteName());
	}
}



////////////////////////////////////////////////////////////////////////////////
bool OCVAnalyzer::CreateObject( const SVObjectLevelCreateStruct& rCreateStructure )
{
	m_isCreated = ImageAnalyzer::CreateObject(rCreateStructure);
	
	if ( m_bHasLicenseError )
	{
		SVOLicenseManager::Instance().AddLicenseErrorToList(GetCompleteName());
	}

	return m_isCreated;
}

bool OCVAnalyzer::ResetObject(SvStl::MessageContainerVector *pErrorMessages)
{
	bool Result = __super::ResetObject(pErrorMessages);

	if( m_bHasLicenseError )
	{
		if (nullptr != pErrorMessages)
		{
			SvStl::MessageContainer Msg( SVMSG_SVO_92_GENERAL_ERROR, SvStl::Tid_SVObserver_MatroxLicenseNotFound, SvStl::SourceFileParams(StdMessageParams), SvStl::Err_16151, getObjectId() );
			pErrorMessages->push_back(Msg);
		}
		Result = false;
	}

	return Result;
}

////////////////////////////////////////////////////////////////////////////////
//
//
//
bool OCVAnalyzer::onRun( SvIe::RunStatus& rRunStatus, SvStl::MessageContainerVector *pErrorMessages )
{
	if ( !m_bHasLicenseError )
	{
		if( __super::onRun( rRunStatus, pErrorMessages ) )
		{
			return true;
		}
	}
	else
	{
		if (nullptr != pErrorMessages)
		{
			SvStl::MessageContainer Msg( SVMSG_SVO_92_GENERAL_ERROR, SvStl::Tid_SVObserver_MatroxLicenseNotFound, SvStl::SourceFileParams(StdMessageParams), SvStl::Err_16151, getObjectId() );
			pErrorMessages->push_back(Msg);
		}
	}
	rRunStatus.SetInvalid();

	return false;
}


bool OCVAnalyzer::IsPtOverResult(const POINT&)
{
	return true;
}

SvDef::StringVector OCVAnalyzer::getAnalyzerResult()
{
	SvDef::StringVector result;

	SvOp::SVOCVAnalyzerResult* pOCVResult = dynamic_cast<SvOp::SVOCVAnalyzerResult*> (GetResultObject());

	if(nullptr != pOCVResult)
	{
		for (const auto pResult : pOCVResult->m_OCVCharacterResults)
		{
			if (nullptr != pResult)
			{
				char cValue;
				double dValue;
				pResult->m_cvoLabelValue.GetValue(cValue);
				pResult->m_dvoMatchScore.GetValue(dValue);

				if (dValue > -1.0f)
				{
					std::string item = SvUl::Format(_T("%c\t%f"), cValue, dValue);
					result.emplace_back(item);
				}
			}
		}
	}

	return result;


}

} //namespace SvAo
