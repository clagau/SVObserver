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
#include "SVAnalyzerResultDlg.h"
#include "SVOCVAnalyzerResult.h"
#include "SVOCVAnalyzer.h"
#include "SVOCVDialog.h"
#include "SVInspectionProcess.h"
#include "SVToolSet.h"
#include "SVOCore/SVTool.h"
#include "SVOLicenseManager/SVOLicenseManager.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif



SV_IMPLEMENT_CLASS( SVOCVAnalyzerClass, SVOCVAnalyzerClassGuid );

/////////////////////////////////////////////////////////////////////////////
SVOCVAnalyzerClass::SVOCVAnalyzerClass( 
	BOOL BCreateDefaultTaskList, 
	SVObjectClass* POwner, 
	int StringResourceID )
		:SVImageAnalyzerClass( BCreateDefaultTaskList, POwner, StringResourceID ) 
{
	init();

	if ( TheSVOLicenseManager().HasMatroxIdentificationLicense() )
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
	m_outObjectInfo.m_ObjectTypeInfo.ObjectType = SVAnalyzerObjectType;
	m_outObjectInfo.m_ObjectTypeInfo.SubType = SVOCVAnalyzerObjectType;

	SVOCVAnalyzeResultClass* pAnalyzerResult = new SVOCVAnalyzeResultClass( this );

	// Children list defaults:
	if( pAnalyzerResult )
	{
		Add( pAnalyzerResult );
	}

}


SVOCVAnalyzerClass::~SVOCVAnalyzerClass()
{
	if ( m_bHasLicenseError )
	{
		m_bHasLicenseError = false;
		TheSVOLicenseManager().RemoveLicenseErrorFromList(GetUniqueObjectID());
	}
		//remove error from LicenseManager
	//delete( pAnalyzerResult ); 
	//Error!!!! Must be ONLY killed by SVResultListClass or PreDestroy()
}



////////////////////////////////////////////////////////////////////////////////
BOOL SVOCVAnalyzerClass::CreateObject( SVObjectLevelCreateStruct* PCreateStructure )
{
	m_isCreated = SVImageAnalyzerClass::CreateObject( PCreateStructure );
	
	if ( m_bHasLicenseError )
	{
		TheSVOLicenseManager().AddLicenseErrorToList(GetUniqueObjectID());
	}

	return m_isCreated;
}

////////////////////////////////////////////////////////////////////////////////
SvOi::IObjectClass* SVOCVAnalyzerClass::GetResultObject()
{
	SVResultClass* pAnalyzerResult = nullptr;

	// Find the result Object in Our List
	pAnalyzerResult = (SVResultClass *) GetAt(0);

	return( pAnalyzerResult );
}

bool SVOCVAnalyzerClass::ResetObject(SvStl::MessageContainerVector *pErrorMessages)
{
	bool Valid = __super::ResetObject(pErrorMessages);
	if( m_bHasLicenseError )
	{
		if (nullptr != pErrorMessages)
		{
			SvStl::MessageContainer Msg( SVMSG_SVO_92_GENERAL_ERROR, SvStl::Tid_SVObserver_MatroxLicenseNotFound, SvStl::SourceFileParams(StdMessageParams), SvStl::Err_16151, GetUniqueObjectID() );
			pErrorMessages->push_back(Msg);
		}
		Valid = false;
	}

	return Valid;
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

void SVOCVAnalyzerClass::DisplayAnalyzerResult()
{
	SVOCVAnalyzeResultClass* pOCVResult = ( SVOCVAnalyzeResultClass* ) GetResultObject();

	SVAnalyzerResultDlg dlg;
	dlg.psvocvResultArray = &pOCVResult->arrayOCVCharacterResults;
	dlg.DoModal();
}
