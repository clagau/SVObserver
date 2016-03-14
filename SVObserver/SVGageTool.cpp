//******************************************************************************
//* COPYRIGHT (c) 2003 by SVResearch, Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVGageTool
//* .File Name       : $Workfile:   SVGageTool.cpp  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.2  $
//* .Check In Date   : $Date:   15 May 2013 19:45:48  $
//******************************************************************************

#include "stdafx.h"
#include "SVGageTool.h"
#include "SVLineROI.h"
#include "SVLineAnalyzer.h"
#include "SVInspectionProcess.h"

SV_IMPLEMENT_CLASS( SVGageToolClass, SVGageToolClassGuid );

SVGageToolClass::SVGageToolClass( BOOL BCreateDefaultTaskList, SVObjectClass* POwner, int StringResourceID )
				:SVToolClass( BCreateDefaultTaskList, POwner, StringResourceID )
{
	init();
}

////////////////////////////////////////////////////////////////////////////////
// .Title       : Initialization of class SVGageToolClass
// -----------------------------------------------------------------------------
// .Description : Initialization of newly Instantiated Object
////////////////////////////////////////////////////////////////////////////////
// .History
//	 Date		Author		Comment                                       
//  :20.08.1999 RO          First Implementation
////////////////////////////////////////////////////////////////////////////////
void SVGageToolClass::init()
{
	m_bToolRegistered = false;

	// Set up your type...
	outObjectInfo.ObjectTypeInfo.ObjectType = SVToolObjectType;
	outObjectInfo.ObjectTypeInfo.SubType    = SVGageToolObjectType;

	// Identify our input type needs
	inputImageObjectInfo.SetInputObjectType( SVImageObjectType );
	inputImageObjectInfo.SetObject( GetObjectInfo() );
	RegisterInputObject( &inputImageObjectInfo, _T( "GageToolImage" ) );

	m_svToolExtent.SetImageType( SVImageTypeVirtual );
	m_svToolExtent.SetTranslation( SVExtentTranslationNone );

	// Remove Embedded Extents
	removeEmbeddedExtents( false );

	// Register Embedded Objects
	RegisterEmbeddedObject( &gageImage, SVOutputImageObjectGuid, IDS_OBJECTNAME_IMAGE1 );

	gageImage.InitializeImage( SVImageTypeVirtual );

	// Default taskObjectList items:

	// Populate the available analyzer list
	SVClassInfoStruct analyzerClassInfo;
	
	// Add the Line Pixel Count Analyzer...
	analyzerClassInfo.ObjectTypeInfo.ObjectType = SVAnalyzerObjectType;
	analyzerClassInfo.ObjectTypeInfo.SubType	= SVLinePixelCountingAnalyzerObjectType;
	analyzerClassInfo.ClassId = SVPixelCountingLineAnalyzerClassGuid;
	analyzerClassInfo.ClassName.LoadString( IDS_CLASSNAME_SVPIXELCOUNTINGLINEANALYZER );
	availableChildren.Add( analyzerClassInfo );

	// Add the Line Edge Count Analyzer...
	analyzerClassInfo.ObjectTypeInfo.ObjectType = SVAnalyzerObjectType;
	analyzerClassInfo.ObjectTypeInfo.SubType	= SVLineEdgeCountingAnalyzerObjectType;
	analyzerClassInfo.ClassId = SVEdgeCountingLineAnalyzerClassGuid;
	analyzerClassInfo.ClassName.LoadString( IDS_CLASSNAME_SVEDGECOUNTINGLINEANALYZER );
	availableChildren.Add(analyzerClassInfo);
	
	// Add the Line Edge Position Analyzer...
	analyzerClassInfo.ObjectTypeInfo.ObjectType = SVAnalyzerObjectType;
	analyzerClassInfo.ObjectTypeInfo.SubType	= SVLineEdgePositionAnalyzerObjectType;
	analyzerClassInfo.ClassId = SVEdgePositionLineAnalyzerClassGuid;
	analyzerClassInfo.ClassName.LoadString( IDS_CLASSNAME_SVEDGEPOSITIONLINEANALYZER );
	availableChildren.Add(analyzerClassInfo);

	// Add the Line Linear Measurement Analyzer...
	analyzerClassInfo.ObjectTypeInfo.ObjectType = SVAnalyzerObjectType;
	analyzerClassInfo.ObjectTypeInfo.SubType	= SVLineLinearMeasurementAnalyzerObjectType;
	analyzerClassInfo.ClassId = SVLinearMeasurementLineAnalyzerClassGuid;
	analyzerClassInfo.ClassName.LoadString( IDS_CLASSNAME_SVLINEARMEASUREMENTANALYZER );
	availableChildren.Add(analyzerClassInfo);

	// Add the Line Maximum Foreground Object Analyzer...
	analyzerClassInfo.ObjectTypeInfo.ObjectType = SVAnalyzerObjectType;
	analyzerClassInfo.ObjectTypeInfo.SubType	= SVLineMaximumForegroundObjectAnalyzerObjectType;
	analyzerClassInfo.ClassId = SVMaximumForegroundObjectLineAnalyzerClassGuid;
	analyzerClassInfo.ClassName.LoadString( IDS_CLASSNAME_SVMAXIMUMFOREGROUNDOBJECTLINEANALYZER );
	availableChildren.Add(analyzerClassInfo);

	// Add the Line Maximum Background Object Analyzer...
	analyzerClassInfo.ObjectTypeInfo.ObjectType = SVAnalyzerObjectType;
	analyzerClassInfo.ObjectTypeInfo.SubType	= SVLineMaximumBackgroundObjectAnalyzerObjectType;
	analyzerClassInfo.ClassId = SVMaximumBackgroundObjectLineAnalyzerClassGuid;
	analyzerClassInfo.ClassName.LoadString( IDS_CLASSNAME_SVMAXIMUMBACKGROUNDOBJECTLINEANALYZER );
	availableChildren.Add(analyzerClassInfo);

	// Add the Line Maximum Object Analyzer...
	analyzerClassInfo.ObjectTypeInfo.ObjectType = SVAnalyzerObjectType;
	analyzerClassInfo.ObjectTypeInfo.SubType	= SVLineMaximumObjectAnalyzerObjectType;
	analyzerClassInfo.ClassId = SVMaximumObjectLineAnalyzerClassGuid;
	analyzerClassInfo.ClassName.LoadString( IDS_CLASSNAME_SVMAXIMUMOBJECTLINEANALYZER );
	availableChildren.Add(analyzerClassInfo);

	// Add Default Inputs and Outputs
	addDefaultInputObjects();
}


//******************************************************************************
// Destructor(s):
//******************************************************************************

////////////////////////////////////////////////////////////////////////////////
// .Title       : Standard Destructor of class SVGageToolClass
// -----------------------------------------------------------------------------
// .Description : Standard destructor
////////////////////////////////////////////////////////////////////////////////
// .History
//	 Date		Author		Comment                                       
//  :20.08.1999 RO          First Implementation
////////////////////////////////////////////////////////////////////////////////
SVGageToolClass::~SVGageToolClass()
{ 
	if( GetInspection() != NULL && m_bToolRegistered )
	{
		GetInspection()->GageToolCountDec();

		m_bToolRegistered = false;
	}
}

//******************************************************************************
// Operator(s):
//******************************************************************************


BOOL SVGageToolClass::CreateObject( SVObjectLevelCreateStruct* PCreateStructure )
{
	BOOL bOk = SVToolClass::CreateObject( PCreateStructure );

	if( GetInspection() != NULL && ! m_bToolRegistered )
	{
		GetInspection()->GageToolCountInc();

		m_bToolRegistered = true;
	}

	bOk &= ( gageImage.InitializeImage( getInputImage() ) == S_OK );

	// Return code for UpdateImageWithExtend not being checked because it may not be valid the first time.
	m_svToolExtent.UpdateImageWithExtent( 1, SVToolExtentClass::SVToolExtent );

	isCreated = bOk;

	RemoveEmbeddedObject( &m_svUpdateAuxilliaryExtents );

	// Add Default Inputs and Outputs
	addDefaultInputObjects();

	return bOk;
}

BOOL SVGageToolClass::CloseObject()
{
	if( SVToolClass::CloseObject() )
	{
		if( GetInspection() != NULL && m_bToolRegistered )
		{
			GetInspection()->GageToolCountDec();

			m_bToolRegistered = false;
		}

		BOOL bRetVal = gageImage.CloseObject();

		return bRetVal;
	}
	return FALSE;
}

BOOL SVGageToolClass::OnValidate()
{

	SetInvalid();
	return FALSE;
}

bool SVGageToolClass::DoesObjectHaveExtents() const
{
	return false;
}

BOOL SVGageToolClass::SetObjectDepth( int NewObjectDepth )
{
	BOOL l_Status = SVToolClass::SetObjectDepth( NewObjectDepth );

	extentLeft.SetObjectDepth( NewObjectDepth );
	extentTop.SetObjectDepth( NewObjectDepth );
	extentRight.SetObjectDepth( NewObjectDepth );
	extentBottom.SetObjectDepth( NewObjectDepth );
	extentWidth.SetObjectDepth( NewObjectDepth );
	extentHeight.SetObjectDepth( NewObjectDepth );

	return l_Status;
}

BOOL SVGageToolClass::SetObjectDepthWithIndex( int NewObjectDepth, int NewLastSetIndex )
{
	BOOL l_Status = SVToolClass::SetObjectDepthWithIndex( NewObjectDepth, NewLastSetIndex );

	extentLeft.SetObjectDepthWithIndex( NewObjectDepth, NewLastSetIndex );
	extentTop.SetObjectDepthWithIndex( NewObjectDepth, NewLastSetIndex );
	extentRight.SetObjectDepthWithIndex( NewObjectDepth, NewLastSetIndex );
	extentBottom.SetObjectDepthWithIndex( NewObjectDepth, NewLastSetIndex );
	extentWidth.SetObjectDepthWithIndex( NewObjectDepth, NewLastSetIndex );
	extentHeight.SetObjectDepthWithIndex( NewObjectDepth, NewLastSetIndex );

	return l_Status;
}

HRESULT SVGageToolClass::ResetObject()
{
	extentLeft.ResetObject();
	extentTop.ResetObject();
	extentRight.ResetObject();
	extentBottom.ResetObject();
	extentWidth.ResetObject();
	extentHeight.ResetObject();

	HRESULT l_hrOk = gageImage.InitializeImage( getInputImage() );

	HRESULT l_Temp = S_OK; //= m_svToolExtent.UpdateImageWithExtent( 1, SVToolExtentClass::SVToolExtent );
	m_svToolExtent.UpdateImageWithExtent( 1, SVToolExtentClass::SVToolExtent );

	if( l_hrOk == S_OK )
	{
		l_hrOk = l_Temp;
	}

//	l_Temp = m_svToolExtent.UpdateOffsetData( true );
	m_svToolExtent.UpdateOffsetData( true );

	if( l_hrOk == S_OK )
	{
		l_hrOk = l_Temp;
	}

	l_Temp = SVToolClass::ResetObject();

	if( l_hrOk == S_OK )
	{
		l_hrOk = l_Temp;
	}

	UpdateImageWithExtent( 1 );
	SetInvalid();
	return l_hrOk;
}

HRESULT SVGageToolClass::IsInputImage( SVImageClass *p_psvImage )
{
	HRESULT l_hrOk = S_FALSE;

	if ( p_psvImage != NULL )
	{
		if ( p_psvImage == getInputImage() )
		{
			l_hrOk = S_OK;
		}
		else
		{
			l_hrOk = SVToolClass::IsInputImage( p_psvImage );
		}
	}

	return l_hrOk;
}



//******************************************************************************
// Operation(s) Of Reading Access:
//******************************************************************************

SVImageClass* SVGageToolClass::getInputImage()
{
	if( inputImageObjectInfo.IsConnected() && inputImageObjectInfo.GetInputObjectInfo().PObject )
		return ( SVImageClass* ) inputImageObjectInfo.GetInputObjectInfo().PObject;

	return NULL;
}

BOOL SVGageToolClass::onRun( SVRunStatusClass& RRunStatus )
{
	SetInvalid();
	RRunStatus.SetInvalid();
	return FALSE;
}

