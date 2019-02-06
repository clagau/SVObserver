//******************************************************************************
//* COPYRIGHT (c) 2003 by SVResearch, Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVOCVAnalyzer
//* .File Name       : $Workfile: 
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.4  $
//* .Check In Date   : $Date:   15 May 2014 11:07:28  $
//******************************************************************************

#include "stdafx.h"
#include "SVOCVAnalyzer.h"
#include "SVObjectLibrary/SVClsIds.h"
#include "Operators/SVOCVAnalyzerResult.h"
#include "SVMatroxLibrary/SVOLicenseManager.h"

namespace SvAo
{

#pragma region Declarations
#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif
#pragma endregion Declarations

SV_IMPLEMENT_CLASS( SVOCVAnalyzerClass, SVOCVAnalyzerClassGuid );

/////////////////////////////////////////////////////////////////////////////
SVOCVAnalyzerClass::SVOCVAnalyzerClass( SVObjectClass* POwner, int StringResourceID )
		:SVImageAnalyzerClass( POwner, StringResourceID ) 
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
void SVOCVAnalyzerClass::init()
{
	m_outObjectInfo.m_ObjectTypeInfo.ObjectType = SvPb::SVAnalyzerObjectType;
	m_outObjectInfo.m_ObjectTypeInfo.SubType = SvPb::SVOCVAnalyzerObjectType;

	SvOp::SVOCVAnalyzeResultClass* pAnalyzerResult = new SvOp::SVOCVAnalyzeResultClass(this);

	// Children list defaults:
	if(nullptr != pAnalyzerResult )
	{
		Add(pAnalyzerResult);
	}

}


SVOCVAnalyzerClass::~SVOCVAnalyzerClass()
{
	if ( m_bHasLicenseError )
	{
		m_bHasLicenseError = false;
		SVOLicenseManager::Instance().RemoveLicenseErrorFromList(GetUniqueObjectID());
	}
}



////////////////////////////////////////////////////////////////////////////////
bool SVOCVAnalyzerClass::CreateObject( const SVObjectLevelCreateStruct& rCreateStructure )
{
	m_isCreated = SVImageAnalyzerClass::CreateObject(rCreateStructure);
	
	if ( m_bHasLicenseError )
	{
		SVOLicenseManager::Instance().AddLicenseErrorToList(GetUniqueObjectID());
	}

	return m_isCreated;
}

bool SVOCVAnalyzerClass::ResetObject(SvStl::MessageContainerVector *pErrorMessages)
{
	bool Result = __super::ResetObject(pErrorMessages);

	if( m_bHasLicenseError )
	{
		if (nullptr != pErrorMessages)
		{
			SvStl::MessageContainer Msg( SVMSG_SVO_92_GENERAL_ERROR, SvStl::Tid_SVObserver_MatroxLicenseNotFound, SvStl::SourceFileParams(StdMessageParams), SvStl::Err_16151, GetUniqueObjectID() );
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
bool SVOCVAnalyzerClass::onRun( SVRunStatusClass& rRunStatus, SvStl::MessageContainerVector *pErrorMessages )
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
			SvStl::MessageContainer Msg( SVMSG_SVO_92_GENERAL_ERROR, SvStl::Tid_SVObserver_MatroxLicenseNotFound, SvStl::SourceFileParams(StdMessageParams), SvStl::Err_16151, GetUniqueObjectID() );
			pErrorMessages->push_back(Msg);
		}
	}
	SetInvalid();
	rRunStatus.SetInvalid();

	return false;
}


bool SVOCVAnalyzerClass::IsPtOverResult( const POINT& rPoint )
{
	return true;
}

SvDef::StringVector SVOCVAnalyzerClass::getAnalyzerResult()
{
	SvDef::StringVector result;

	SvOp::SVOCVAnalyzeResultClass* pOCVResult = dynamic_cast<SvOp::SVOCVAnalyzeResultClass*> (GetResultObject());

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
