//******************************************************************************
//* COPYRIGHT (c) 2003 by SVResearch, Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVOCRGrayAnalyzer
//* .File Name       : $Workfile:   SVOCRGrayAnalyzer.cpp  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.0  $
//* .Check In Date   : $Date:   23 Apr 2013 12:55:20  $
//******************************************************************************

// 19 Jul 1999 - frb.
//
//

#include "stdafx.h"
#include "SVAnalyzerResultDlg.h"
#include "SVOCRGrayAnalyzerResult.h"
#include "SVOCRGrayAnalyzer.h"
#include "SVOCRGrayDialog.h"
#include "SVInspectionProcess.h"
#include "SVToolSet.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif



SV_IMPLEMENT_CLASS( SVOCRGrayAnalyzerClass, SVOCRGrayAnalyzerClassGuid );

/////////////////////////////////////////////////////////////////////////////
//
//
//
//
SVOCRGrayAnalyzerClass::SVOCRGrayAnalyzerClass( 
	BOOL BCreateDefaultTaskList, 
	SVObjectClass* POwner, 
	int StringResourceID )
		:SVImageAnalyzerClass( BCreateDefaultTaskList, POwner, StringResourceID ) 
{
	init();
}

////////////////////////////////////////////////////////////////////////////////
//
//
//
void SVOCRGrayAnalyzerClass::init()
{
	outObjectInfo.ObjectTypeInfo.ObjectType = SVAnalyzerObjectType;
	outObjectInfo.ObjectTypeInfo.SubType = SVOCRGrayAnalyzerObjectType;

   SVOCRGrayAnalyzeResultClass* pAnalyzerResult = new SVOCRGrayAnalyzeResultClass;

	// Children list defaults:
	if( pAnalyzerResult )
	{
		Add( pAnalyzerResult );
	}

}


SVOCRGrayAnalyzerClass::~SVOCRGrayAnalyzerClass()
{
	//delete( pAnalyzerResult ); 
	//Error!!!! Must be ONLY killed by SVResultListClass or PreDestroy()
}



////////////////////////////////////////////////////////////////////////////////
// .Title       : Create
// -----------------------------------------------------------------------------
// .Description : ...
//              :
// -----------------------------------------------------------------------------
// .Input(s)
//	 Type				Name				Description
//	: SVToolClass*		PTool 
//  :
// .Output(s)
//	:
//  :
// .Return Value
//	: FALSE
// -----------------------------------------------------------------------------
// .Import Function Reference(s)
//	:
// -----------------------------------------------------------------------------
// .Import Variable Reference(s)
//	:
////////////////////////////////////////////////////////////////////////////////
// .History
//	 Date		Author		Comment                                       
//  :25.08.1997 RO			First Implementation
//	:
////////////////////////////////////////////////////////////////////////////////
BOOL SVOCRGrayAnalyzerClass::CreateObject( SVObjectLevelCreateStruct* PCreateStructure )
{
	isCreated = SVImageAnalyzerClass::CreateObject( PCreateStructure );

	return isCreated;
}



////////////////////////////////////////////////////////////////////////////////
//
//
//
SVResultClass* SVOCRGrayAnalyzerClass::GetResultObject()
{
	SVResultClass* pAnalyzerResult = NULL;

	// Find the result Object in Our List
	pAnalyzerResult = (SVResultClass *)GetAt(0);

	return( pAnalyzerResult );
}

/////////////////////////////////////////////////////////////////////////////
//
//
//
//
BOOL SVOCRGrayAnalyzerClass::OnValidate()
{
	BOOL bRetVal = FALSE;
	//if( inputLongObjectInfo.IsConnected &&
	//	inputLongObjectInfo.InputObjectInfo.PObject )
	//{
		bRetVal = TRUE;
		bRetVal = SVImageAnalyzerClass::OnValidate() && bRetVal;
	//}

	// Note: Make sure this is called when Validate fails !!!
	if( ! bRetVal )
		SetInvalid();

	return bRetVal;
}

////////////////////////////////////////////////////////////////////////////////
//
//
//
BOOL SVOCRGrayAnalyzerClass::onRun( SVRunStatusClass& RRunStatus )
{
	if( SVImageAnalyzerClass::onRun( RRunStatus ) )
	{
		return TRUE;

	}
	SetInvalid();
	RRunStatus.SetInvalid();

	return FALSE;
}

BOOL SVOCRGrayAnalyzerClass::IsPtOverResult( CPoint point )
{
	return TRUE;
}

void SVOCRGrayAnalyzerClass::DisplayAnalyzerResult()
{
	SVOCRGrayAnalyzeResultClass* pOCRResult = ( SVOCRGrayAnalyzeResultClass* ) GetResultObject();
	if (pOCRResult->arrayOCRBlobRecords.GetSize() < 0)
		return;

	pOCRResult->arrayOCRBlobRecords.SortArray(SORT_BLOBS_SCORE);

	SVAnalyzerResultDlg dlg;
	dlg.svocrbraResults = &pOCRResult->arrayOCRBlobRecords;
	dlg.psvocvResultArray = NULL;
	dlg.DoModal();

	pOCRResult->arrayOCRBlobRecords.SortArray();
}

