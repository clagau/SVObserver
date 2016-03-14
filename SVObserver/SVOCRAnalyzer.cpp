//******************************************************************************
//* COPYRIGHT (c) 2003 by SVResearch, Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVOCRAnalyzer
//* .File Name       : $Workfile:   SVOCRAnalyzer.cpp  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.0  $
//* .Check In Date   : $Date:   23 Apr 2013 12:50:38  $
//******************************************************************************

#include "stdafx.h"
#include "SVOCRAnalyzerResult.h"
#include "SVOCRAnalyzer.h"
#include "SVOCRDialog.h"
#include "SVInspectionProcess.h"
#include "SVToolSet.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif



SV_IMPLEMENT_CLASS( SVOCRAnalyzerClass, SVOCRAnalyzerClassGuid );

SVOCRAnalyzerClass::SVOCRAnalyzerClass( 
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
void SVOCRAnalyzerClass::init()
{

	outObjectInfo.ObjectTypeInfo.ObjectType = SVAnalyzerObjectType;
	outObjectInfo.ObjectTypeInfo.SubType = SVOCRAnalyzerObjectType;

   SVOCRAnalyzeResultClass* pAnalyzerResult = new SVOCRAnalyzeResultClass;

	// Children list defaults:
	if( pAnalyzerResult )
	{
		Add( pAnalyzerResult );
	}

}


SVOCRAnalyzerClass::~SVOCRAnalyzerClass()
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
BOOL SVOCRAnalyzerClass::CreateObject( 
	SVObjectLevelCreateStruct* PCreateStructure )
{
	isCreated = SVImageAnalyzerClass::CreateObject( PCreateStructure );

	return isCreated;
}



////////////////////////////////////////////////////////////////////////////////
//
//
//
SVResultClass* SVOCRAnalyzerClass::GetResultObject()
{
	SVResultClass* pAnalyzerResult = NULL;

	// Find the result Object in Our List
	pAnalyzerResult = (SVResultClass *)GetAt(0);

	//ASSERT( GetTool() != NULL );
	//if( pAnalyzerResult == NULL )
	//{
	//	if( pAnalyzerResult	= new SVWhitePixelAnalyzeResultClass( this ) )
	//	{
	//		if( ! pAnalyzerResult->Create( this, GetTool() ) )
	//		{
	//			AfxMessageBox( "Unable to create white pixel Analyze result!" );
	//			pAnalyzerResult->Destroy();
	//			pAnalyzerResult = NULL;
	//		}
	//	}
	//}
	return( pAnalyzerResult );
}

/////////////////////////////////////////////////////////////////////////////
//
//
//
//
BOOL SVOCRAnalyzerClass::OnValidate()
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
BOOL SVOCRAnalyzerClass::onRun( SVRunStatusClass& RRunStatus )
{
	if( SVImageAnalyzerClass::onRun( RRunStatus ) )
	{
		return TRUE;

	}
	SetInvalid();
	RRunStatus.SetInvalid();

	return FALSE;
}

