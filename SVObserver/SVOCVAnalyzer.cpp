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
#include "SVTool.h"
#include "SVOLicenseManager/SVOLicenseManager.h"
#include "SVOMFCLibrary/SVDeviceParams.h"

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
	m_outObjectInfo.ObjectTypeInfo.ObjectType = SVAnalyzerObjectType;
	m_outObjectInfo.ObjectTypeInfo.SubType = SVOCVAnalyzerObjectType;

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

HRESULT SVOCVAnalyzerClass::ResetObject()
{
	HRESULT	l_hrOk = SVImageAnalyzerClass::ResetObject();

	return l_hrOk;
}

////////////////////////////////////////////////////////////////////////////////
SVResultClass* SVOCVAnalyzerClass::GetResultObject()
{
	SVResultClass* pAnalyzerResult = nullptr;

	// Find the result Object in Our List
	pAnalyzerResult = (SVResultClass *) GetAt(0);

	return( pAnalyzerResult );
}


/////////////////////////////////////////////////////////////////////////////
BOOL SVOCVAnalyzerClass::OnValidate()
{
	BOOL bRetVal = SVImageAnalyzerClass::OnValidate();

	// Note: Make sure this is called when Validate fails !!!
	if (! bRetVal)
	{
		SetInvalid();
		bRetVal = FALSE;
	}
	if( m_bHasLicenseError )
	{
		bRetVal = FALSE;
	}

	return bRetVal;
}

////////////////////////////////////////////////////////////////////////////////
//
//
//
BOOL SVOCVAnalyzerClass::onRun( SVRunStatusClass& RRunStatus )
{
	if ( !m_bHasLicenseError )
	{
		if( SVImageAnalyzerClass::onRun( RRunStatus ) )
		{
			return TRUE;
		}
	}
	SetInvalid();
	RRunStatus.SetInvalid();

	return FALSE;
}


BOOL SVOCVAnalyzerClass::IsPtOverResult( CPoint point )
{
	return TRUE;
}

void SVOCVAnalyzerClass::DisplayAnalyzerResult()
{
	SVOCVAnalyzeResultClass* pOCVResult = ( SVOCVAnalyzeResultClass* ) GetResultObject();

	SVAnalyzerResultDlg dlg;
	dlg.psvocvResultArray = &pOCVResult->arrayOCVCharacterResults;
	dlg.DoModal();
}

DWORD_PTR SVOCVAnalyzerClass::processMessage( DWORD DwMessageID, DWORD_PTR DwMessageValue, DWORD_PTR DwMessageContext )
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

	return( SVImageAnalyzerClass::processMessage( DwMessageID, DwMessageValue, DwMessageContext ) | DwResult );
}

