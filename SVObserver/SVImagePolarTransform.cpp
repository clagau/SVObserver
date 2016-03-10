//******************************************************************************
//* COPYRIGHT (c) 2003 by SVResearch, Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVImagePolarTransform
//* .File Name       : $Workfile:   SVImagePolarTransform.cpp  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.4  $
//* .Check In Date   : $Date:   20 Nov 2014 07:02:58  $
//******************************************************************************

#include "stdafx.h"
#include <float.h>
#include "SVImagePolarTransform.h"

#include "SVImageLibrary/SVDrawContext.h"
#include "SVImageLibrary/SVImageBufferHandleImage.h"
#include "SVMatroxLibrary/SVMatroxLibrary.h"
#include "SVObjectLibrary/SVToolLevelCreateStruct.h"

#include "SVEquation.h"
#include "SVEvaluate.h"
#include "SVGlobal.h"
#include "SVImageProcessingClass.h"
#include "SVTool.h"
#include "SVPolarTransformationTool.h"

double SVImagePolarTransformClass::g_dMaxAngularDistance  = 720.0;

SV_IMPLEMENT_CLASS( SVImagePolarTransformClass, SVImagePolarTransformClassGuid )

SVImagePolarTransformClass::SVImagePolarTransformClass( SVObjectClass* POwner, int StringResourceID )
						   :SVPolarTransformClass( POwner, StringResourceID ) 
{

	// Identify yourself
	outObjectInfo.ObjectTypeInfo.SubType = SVImagePolarTransformObjectType;

	// Identify our input type needs...
	// Image
	inputImageObjectInfo.SetInputObjectType( SVImageObjectType );
	inputImageObjectInfo.SetObject( GetObjectInfo() );
	RegisterInputObject( &inputImageObjectInfo, _T( "ImagePolarTransformImage" ) );


	// Register Embedded Objects
	RegisterEmbeddedObject( &centerX, SVOutputCenterXObjectGuid, IDS_OBJECTNAME_CENTER_X, false, SVResetItemNone );
	RegisterEmbeddedObject( &centerY, SVOutputCenterYObjectGuid, IDS_OBJECTNAME_CENTER_Y, false, SVResetItemNone );
	RegisterEmbeddedObject( &startRadius, SVOutputStartRadiusObjectGuid, IDS_OBJECTNAME_START_RADIUS, false, SVResetItemNone );
	RegisterEmbeddedObject( &endRadius, SVOutputEndRadiusObjectGuid, IDS_OBJECTNAME_END_RADIUS, false, SVResetItemNone );
	RegisterEmbeddedObject( &startAngle, SVOutputStartAngleObjectGuid, IDS_OBJECTNAME_START_ANGLE, false, SVResetItemNone );
	RegisterEmbeddedObject( &endAngle, SVOutputEndAngleObjectGuid, IDS_OBJECTNAME_END_ANGLE, false, SVResetItemNone );
	RegisterEmbeddedObject( &interpolationMode, SVOutputInterpolationModeObjectGuid, IDS_OBJECTNAME_INTERPOLATION_MODE, false, SVResetItemNone );

	RegisterEmbeddedObject( &useFormulaInput, SVOutputUseFormulaObjectGuid, IDS_OBJECTNAME_USE_FORMULA, false, SVResetItemTool );

	RegisterEmbeddedObject( &outputImageObject, SVOutputImageObjectGuid, IDS_OBJECTNAME_IMAGE1 );
	
	// Prepare known interpolation types...
	CString strEnable;
	strEnable.LoadString( IDS_ENABLE_STRING );

	CString strDisable;
	strDisable.LoadString( IDS_DISABLE_STRING );

	CString strOverscan;
	strOverscan.LoadString( IDS_OVERSCAN_STRING );

	CString strOverscanClear;
	strOverscanClear.LoadString( IDS_OVERSCAN_CLEAR_STRING );

	CString strMode;
	CString strPrepare;
	CString strEnumTypes;

	// M_NEAREST_NEIGHBOR + M_OVERSCAN_ENABLE
	strMode.LoadString( IDS_NEAREST_NEIGHBOR_STRING );
	strPrepare.Format( _T( "%s+%s %s=%d," ), strMode, strOverscan, strEnable, SVNearestNeighOverScanEnable ); // M_NEAREST_NEIGHBOR + M_OVERSCAN_ENABLE );
	strEnumTypes += strPrepare;
	// M_NEAREST_NEIGHBOR + M_OVERSCAN_DISABLE
	strPrepare.Format( _T( "%s+%s %s=%d," ), strMode, strOverscan, strDisable, SVNearestNeighOverScanDisable ); // M_NEAREST_NEIGHBOR + M_OVERSCAN_DISABLE );
	strEnumTypes += strPrepare;
	// M_NEAREST_NEIGHBOR + M_OVERSCAN_CLEAR
	strPrepare.Format( _T( "%s+%s=%d," ), strMode, strOverscanClear, SVNearestNeighOverScanClear ); // M_NEAREST_NEIGHBOR + M_OVERSCAN_CLEAR );
	strEnumTypes += strPrepare;

	// M_BILINEAR + M_OVERSCAN_ENABLE
	strMode.LoadString( IDS_BILINEAR_STRING );
	strPrepare.Format( _T( "%s+%s %s=%d," ), strMode, strOverscan, strEnable, SVBilinearOverScanEnable ); // M_BILINEAR + M_OVERSCAN_ENABLE );
	strEnumTypes += strPrepare;
	// M_BILINEAR + M_OVERSCAN_DISABLE
	strPrepare.Format( _T( "%s+%s %s=%d," ), strMode, strOverscan, strDisable, SVBilinearOverScanClear ); // M_BILINEAR + M_OVERSCAN_DISABLE );
	strEnumTypes += strPrepare;
	// M_BILINEAR + M_OVERSCAN_CLEAR
	strPrepare.Format( _T( "%s+%s=%d," ), strMode, strOverscanClear, SVBilinearOverScanClear ); // M_BILINEAR + M_OVERSCAN_CLEAR );
	strEnumTypes += strPrepare;

	// M_BICUBIC + M_OVERSCAN_ENABLE
	strMode.LoadString( IDS_BICUBIC_STRING );
	strPrepare.Format( _T( "%s+%s %s=%d," ), strMode, strOverscan, strEnable, SVBiCubicOverScanEnable ); // M_BICUBIC + M_OVERSCAN_ENABLE );
	strEnumTypes += strPrepare;
	// M_BICUBIC + M_OVERSCAN_DISABLE
	strPrepare.Format( _T( "%s+%s %s=%d," ), strMode, strOverscan, strDisable, SVBiCubicOverScanDisable ); // M_BICUBIC + M_OVERSCAN_DISABLE );
	strEnumTypes += strPrepare;
	// M_BICUBIC + M_OVERSCAN_CLEAR
	strPrepare.Format( _T( "%s+%s=%d," ), strMode, strOverscanClear, SVBiCubicOverScanClear ); // M_BICUBIC + M_OVERSCAN_CLEAR );
	strEnumTypes += strPrepare;

	// And now set enum types...
	interpolationMode.SetEnumTypes( strEnumTypes );


	// Set Embedded defaults
	interpolationMode.SetDefaultValue( SVNearestNeighOverScanEnable, TRUE );	// Refer to MIL...
	centerX.SetDefaultValue( ( double ) SV_DEFAULT_POLARTRANSFORM_CENTER_X, TRUE );
	centerY.SetDefaultValue( ( double ) SV_DEFAULT_POLARTRANSFORM_CENTER_Y, TRUE );
	startRadius.SetDefaultValue( ( double ) SV_DEFAULT_POLARTRANSFORM_START_RADIUS, TRUE );
	endRadius.SetDefaultValue( ( double ) SV_DEFAULT_POLARTRANSFORM_END_RADIUS, TRUE );
	startAngle.SetDefaultValue( ( double ) SV_DEFAULT_POLARTRANSFORM_START_ANGLE, TRUE );
	endAngle.SetDefaultValue( ( double ) SV_DEFAULT_POLARTRANSFORM_END_ANGLE, TRUE );

	outputImageObject.InitializeImage( SVImageTypePhysical );

	useFormulaInput.SetDefaultValue( FALSE, TRUE );	// Default: Don't use formulas...


	// Add Default Inputs and Outputs
	addDefaultInputObjects();
}

SVImagePolarTransformClass::~SVImagePolarTransformClass()
{
	CloseObject();
}

BOOL SVImagePolarTransformClass::CreateObject( SVObjectLevelCreateStruct* PCreateStructure )
{
	BOOL l_bOk = SVPolarTransformClass::CreateObject( PCreateStructure );

	l_bOk = l_bOk && GetTool() != NULL;

	l_bOk = l_bOk && GetTool()->SetImageExtentProperty( SVExtentPropertyPositionPointX, &centerX ) == S_OK;
	l_bOk = l_bOk && GetTool()->SetImageExtentProperty( SVExtentPropertyPositionPointY, &centerY ) == S_OK;
	l_bOk = l_bOk && GetTool()->SetImageExtentProperty( SVExtentPropertyInnerRadius, &endRadius ) == S_OK;
	l_bOk = l_bOk && GetTool()->SetImageExtentProperty( SVExtentPropertyOuterRadius, &startRadius ) == S_OK;
	l_bOk = l_bOk && GetTool()->SetImageExtentProperty( SVExtentPropertyStartAngle, &startAngle ) == S_OK;
	l_bOk = l_bOk && GetTool()->SetImageExtentProperty( SVExtentPropertyEndAngle, &endAngle ) == S_OK;

	l_bOk &= ( outputImageObject.InitializeImage( getInputImage() ) == S_OK );

	// Set / Reset Printable Flag
	centerX.ObjectAttributesAllowedRef() |= SV_PRINTABLE | SV_REMOTELY_SETABLE | SV_EXTENT_OBJECT | SV_SETABLE_ONLINE;
	centerY.ObjectAttributesAllowedRef() |= SV_PRINTABLE | SV_REMOTELY_SETABLE | SV_EXTENT_OBJECT | SV_SETABLE_ONLINE;
	startRadius.ObjectAttributesAllowedRef() |= SV_PRINTABLE | SV_REMOTELY_SETABLE | SV_EXTENT_OBJECT | SV_SETABLE_ONLINE;
	endRadius.ObjectAttributesAllowedRef() |= SV_PRINTABLE | SV_REMOTELY_SETABLE | SV_EXTENT_OBJECT | SV_SETABLE_ONLINE;
	startAngle.ObjectAttributesAllowedRef() |= SV_PRINTABLE | SV_REMOTELY_SETABLE | SV_EXTENT_OBJECT | SV_SETABLE_ONLINE;
	endAngle.ObjectAttributesAllowedRef() |= SV_PRINTABLE | SV_REMOTELY_SETABLE | SV_EXTENT_OBJECT | SV_SETABLE_ONLINE;
	interpolationMode.ObjectAttributesAllowedRef() |= SV_PRINTABLE;
	useFormulaInput.ObjectAttributesAllowedRef() |= SV_PRINTABLE;


	SetCalculatedPrintableFlags();


	isCreated = l_bOk;

	return isCreated;
}

HRESULT SVImagePolarTransformClass::IsInputImage( SVImageClass *p_psvImage )
{
	HRESULT l_hrOk = S_FALSE;

	if ( p_psvImage != NULL && p_psvImage == getInputImage() )
	{
		l_hrOk = S_OK;
	}

	return l_hrOk;
}

SVImageClass* SVImagePolarTransformClass::getInputImage()
{
	if( inputImageObjectInfo.IsConnected() && inputImageObjectInfo.GetInputObjectInfo().PObject )
		return ( SVImageClass* ) inputImageObjectInfo.GetInputObjectInfo().PObject;

	return NULL;
}

SVImageClass* SVImagePolarTransformClass::GetOutputImage()
{
	return &outputImageObject;
}

// Sets the formulas of the evaluation inputs to the standard
// outputs of this image polar transform object. 
// This is very useful, if you want that mouse movings will 
// work also if use formula is selected.
// So this should be called if the user adds a new blank polar
// unwrap tool for easier handling. If he wants he can then  
// change the formulas.
// Another possibilty to call this function is in the setup
// dialog, if a kind of reset formula button exists.
BOOL SVImagePolarTransformClass::SetDefaultFormulas()
{
	BOOL bRetVal = TRUE;
	BOOL bOk;
	CString strEmpty;
	SVObjectTypeInfoStruct objectInfo;
	SVObjectTypeInfoStruct equationObjectInfo;
	equationObjectInfo.ObjectType = SVEquationObjectType;
	equationObjectInfo.SubType    = SVMathEquationObjectType;
	
	// Find the evaluation center x object...
	bOk = FALSE;
	objectInfo.SubType = SVEvaluateCenterXObjectType;
	SVEvaluateCenterXClass* pEvaluateCenterX = reinterpret_cast<SVEvaluateCenterXClass*>(::SVSendMessage( GetTool(), SVM_GETFIRST_OBJECT, NULL, reinterpret_cast<DWORD_PTR>(&objectInfo)) );
	if( pEvaluateCenterX )
	{
		// Find equation object...
		SVEquationClass* pEquation = reinterpret_cast<SVEquationClass*>(::SVSendMessage( pEvaluateCenterX, SVM_GETFIRST_OBJECT, NULL, reinterpret_cast<DWORD_PTR>(&equationObjectInfo)) );
		if( pEquation )
		{
			// Get current name of embedded CenterX...
			CString strName = centerX.GetCompleteObjectNameToObjectType( NULL, SVToolSetObjectType );
			if( ! strName.IsEmpty() )
			{
				// Set equation in quotes...
				CString strEquation;
				strEquation.Format( _T( "\"%s\"" ), strName );

				// Set equation...
				pEquation->SetEquationText( strEquation );
				// Update symbol table and test...
				if( ! ( bOk = pEquation->Test().bPassed ) )
				{
					// something is wrong...
					pEquation->SetEquationText( strEmpty );
				}
			}
		}
	}
	if( ! bOk )
		bRetVal = FALSE;

	// Find the evaluation center y object...
	bOk = FALSE;
	objectInfo.SubType = SVEvaluateCenterYObjectType;
	SVEvaluateCenterYClass* pEvaluateCenterY = reinterpret_cast<SVEvaluateCenterYClass*>(::SVSendMessage( GetTool(), SVM_GETFIRST_OBJECT, NULL, reinterpret_cast<DWORD_PTR>(&objectInfo)) );
	if( pEvaluateCenterY )
	{
		// Find equation object...
		SVEquationClass* pEquation = reinterpret_cast<SVEquationClass*>(::SVSendMessage( pEvaluateCenterY, SVM_GETFIRST_OBJECT, NULL, reinterpret_cast<DWORD_PTR>(&equationObjectInfo)) );
		if( pEquation )
		{
			// Get current name of embedded CenterY...
			CString strName = centerY.GetCompleteObjectNameToObjectType( NULL, SVToolSetObjectType );
			if( ! strName.IsEmpty() )
			{
				// Set equation in quotes...
				CString strEquation;
				strEquation.Format( _T( "\"%s\"" ), strName );

				// Set equation...
				pEquation->SetEquationText( strEquation );
				// Update symbol table and test...
				if( ! ( bOk = pEquation->Test().bPassed ) )
				{
					// something is wrong...
					pEquation->SetEquationText( strEmpty );
				}
			}
		}
	}
	if( ! bOk )
		bRetVal = FALSE;

	// Find the evaluation start radius object...
	bOk = FALSE;
	objectInfo.SubType = SVEvaluateStartRadiusObjectType;
	SVEvaluateStartRadiusClass* pEvaluateStartRadius = reinterpret_cast<SVEvaluateStartRadiusClass*>(::SVSendMessage( GetTool(), SVM_GETFIRST_OBJECT, NULL, reinterpret_cast<DWORD_PTR>(&objectInfo)) );
	if( pEvaluateStartRadius )
	{
		// Find equation object...
		SVEquationClass* pEquation = reinterpret_cast<SVEquationClass*>(::SVSendMessage( pEvaluateStartRadius, SVM_GETFIRST_OBJECT, NULL, reinterpret_cast<DWORD_PTR>(&equationObjectInfo)) );
		if( pEquation )
		{
			// Get current name of embedded StartRadius...
			CString strName = startRadius.GetCompleteObjectNameToObjectType( NULL, SVToolSetObjectType );
			if( ! strName.IsEmpty() )
			{
				// Set equation in quotes...
				CString strEquation;
				strEquation.Format( _T( "\"%s\"" ), strName );

				// Set equation...
				pEquation->SetEquationText( strEquation );
				// Update symbol table and test...
				if( ! ( bOk = pEquation->Test().bPassed ) )
				{
					// something is wrong...
					pEquation->SetEquationText( strEmpty );
				}
			}
		}
	}
	if( ! bOk )
		bRetVal = FALSE;

	// Find the evaluation end radius object...
	bOk = FALSE;
	objectInfo.SubType = SVEvaluateEndRadiusObjectType;
	SVEvaluateEndRadiusClass* pEvaluateEndRadius = reinterpret_cast<SVEvaluateEndRadiusClass*>(::SVSendMessage( GetTool(), SVM_GETFIRST_OBJECT, NULL, reinterpret_cast<DWORD_PTR>(&objectInfo)) );
	if( pEvaluateEndRadius )
	{
		// Find equation object...
		SVEquationClass* pEquation = reinterpret_cast<SVEquationClass*>(::SVSendMessage( pEvaluateEndRadius, SVM_GETFIRST_OBJECT, NULL, reinterpret_cast<DWORD_PTR>(&equationObjectInfo)) );
		if( pEquation )
		{
			// Get current name of embedded EndRadius...
			CString strName = endRadius.GetCompleteObjectNameToObjectType( NULL, SVToolSetObjectType );
			if( ! strName.IsEmpty() )
			{
				// Set equation in quotes...
				CString strEquation;
				strEquation.Format( _T( "\"%s\"" ), strName );

				// Set equation...
				pEquation->SetEquationText( strEquation );
				// Update symbol table and test...
				if( ! ( bOk = pEquation->Test().bPassed ) )
				{
					// something is wrong...
					pEquation->SetEquationText( strEmpty );
				}
			}
		}
	}
	if( ! bOk )
		bRetVal = FALSE;

	// Find the evaluation start angle object...
	bOk = FALSE;
	objectInfo.SubType = SVEvaluateStartAngleObjectType;
	SVEvaluateStartAngleClass* pEvaluateStartAngle = reinterpret_cast<SVEvaluateStartAngleClass*>(::SVSendMessage( GetTool(), SVM_GETFIRST_OBJECT, NULL, reinterpret_cast<DWORD_PTR>(&objectInfo)) );
	if( pEvaluateStartAngle )
	{
		// Find equation object...
		SVEquationClass* pEquation = reinterpret_cast<SVEquationClass*>(::SVSendMessage( pEvaluateStartAngle, SVM_GETFIRST_OBJECT, NULL, reinterpret_cast<DWORD_PTR>(&equationObjectInfo)) );
		if( pEquation )
		{
			// Get current name of embedded StartAngle...
			CString strName = startAngle.GetCompleteObjectNameToObjectType( NULL, SVToolSetObjectType );
			if( ! strName.IsEmpty() )
			{
				// Set equation in quotes...
				CString strEquation;
				strEquation.Format( _T( "\"%s\"" ), strName );

				// Set equation...
				pEquation->SetEquationText( strEquation );
				// Update symbol table and test...
				if( ! ( bOk = pEquation->Test().bPassed ) )
				{
					// something is wrong...
					pEquation->SetEquationText( strEmpty );
				}
			}
		}
	}
	if( ! bOk )
		bRetVal = FALSE;

	// Find the evaluation end angle object...
	bOk = FALSE;
	objectInfo.SubType = SVEvaluateEndAngleObjectType;
	SVEvaluateEndAngleClass* pEvaluateEndAngle = reinterpret_cast<SVEvaluateEndAngleClass*>(::SVSendMessage( GetTool(), SVM_GETFIRST_OBJECT, NULL, reinterpret_cast<DWORD_PTR>(&objectInfo)) );
	if( pEvaluateEndAngle )
	{
		// Find equation object...
		SVEquationClass* pEquation = reinterpret_cast<SVEquationClass*>(::SVSendMessage( pEvaluateEndAngle, SVM_GETFIRST_OBJECT, NULL, reinterpret_cast<DWORD_PTR>(&equationObjectInfo)) );
		if( pEquation )
		{
			// Get current name of embedded EndAngle...
			CString strName = endAngle.GetCompleteObjectNameToObjectType( NULL, SVToolSetObjectType );
			if( ! strName.IsEmpty() )
			{
				// Set equation in quotes...
				CString strEquation;
				strEquation.Format( _T( "\"%s\"" ), strName );

				// Set equation...
				pEquation->SetEquationText( strEquation );
				// Update symbol table and test...
				if( ! ( bOk = pEquation->Test().bPassed ) )
				{
					// something is wrong...
					pEquation->SetEquationText( strEmpty );
				}
			}
		}
	}
	if( ! bOk )
		bRetVal = FALSE;


	return bRetVal;
}

// New Correct start and end angle...
// ]-360.0...360.0[
// start angle must always be lower than end angle, so that
// MIL works all the time counter clockwise...

void SVImagePolarTransformClass::NewCorrectAngles( double& RDStartAngle, double& RDEndAngle )
{
	double l_dDistance = RDEndAngle - RDStartAngle; 

	if ( l_dDistance < 0.0 )	// Rule : Start Angle must be less than End Angle
	{
		double l_dTmp = RDEndAngle;
		RDEndAngle = RDStartAngle;
		RDStartAngle = l_dTmp ;
		l_dDistance = -l_dDistance;
	}

	if( l_dDistance < 1.0 )		// Rule : Angular distance must be greater or equal to 1.0 degree.
	{
		RDEndAngle = RDStartAngle + 1.0;
	}

								// Rule : Start angle must be between -360 and 360
	if( RDStartAngle < -360.0 || 360.0 < RDStartAngle)
	{
		RDStartAngle = fmod( RDStartAngle, 360.0 );
		RDEndAngle = RDStartAngle +  l_dDistance ;
	}

	if( RDEndAngle > 720.0 || 0 > RDEndAngle)
	{							// Rule : End Angle Must be between 0 and 720
		RDEndAngle = fmod(RDEndAngle, 360.0 ) + 360.0;
		RDStartAngle = RDEndAngle - l_dDistance ;
	}

	if( l_dDistance > 360.0 )	// Rule : If distance is greater than 360 
	{							// then start angle must be negative.  
		if( RDStartAngle > 0 )
		{
			RDStartAngle -= 360.0;
			RDEndAngle -= 360.0;
		}
	}
	else						// Rule : If distance is less than  360 then 
	{							// Angles must be in the range of -360 to +360 
		if( RDEndAngle > 360.0 && RDStartAngle > 0.0)
		{
			RDEndAngle -= 360.0;
			RDStartAngle -= 360.0;
		}
	}
								// Rule : Distance must be limited to 720 degrees
	if( RDEndAngle - RDStartAngle > g_dMaxAngularDistance )
	{
		RDEndAngle = RDStartAngle + g_dMaxAngularDistance;
	}

	if( -360.0 > RDStartAngle ) // Never! never! let start angle go below -360 !
	{
		RDStartAngle = -360.0;
	}
}

// Correct start and end angle...
// Angles are corrected to be in the range of -360.0...360.0
// start angle must always be lower than end angle, so that
// MIL works all the time counter clockwise...
// This method limits the total angular distance to less than 360
// degrees.
void SVImagePolarTransformClass::correctAngles( double& RDStartAngle, double& RDEndAngle )
{
	RDStartAngle = fmod( RDStartAngle, 360.0 );
	RDEndAngle   = fmod( RDEndAngle, 360.0 );
	if( RDStartAngle > RDEndAngle )
	{
		// make start angle negative...
		RDStartAngle = RDStartAngle - 360.0;
		RDStartAngle = fmod( RDStartAngle, 360.0 );
	}
	else
	{
		if( RDEndAngle - RDStartAngle > 360.0 )
		{
			// reduce start angle, no overrun allowed ( max span 360.0 Degree !!! )
			RDStartAngle = RDEndAngle - 360.0;
			RDStartAngle = fmod( RDStartAngle, 360.0 );
		}
	}
}




BOOL SVImagePolarTransformClass::OnValidate()
{
	BOOL bRetVal = FALSE;
	SetCalculatedPrintableFlags();
	if( getInputImage() )
	{
		bRetVal = TRUE;
		bRetVal = SVPolarTransformClass::OnValidate() && bRetVal;
	}

	// Note: Make sure this is called when Validate fails !!!
	if( ! bRetVal )
		SetInvalid();

	return bRetVal;
}

void SVImagePolarTransformClass::AnglesTo360( double& p_dStart, double& p_dEnd)
{
	// Shifts start and end angles to be between -360 and +350 degrees.
	if( p_dEnd - p_dStart > 360.0 ) // 
	{
		p_dEnd = p_dStart + 360.0;
	}

	if( p_dEnd > 360.0 || p_dStart > 360.0 )
	{
		p_dEnd -= 360.0;
		p_dStart -= 360.0;
	}
	if( p_dEnd < -360.0 || p_dStart < -360.0 )
	{
		p_dEnd += 360.0;
		p_dStart += 360.0;
	}
}	

BOOL SVImagePolarTransformClass::onRun( SVRunStatusClass& RRunStatus )
{
	BOOL l_bUseFormula = FALSE;
	long l_lAngularMethod = 0;

	BOOL l_bOk = SVPolarTransformClass::onRun( RRunStatus );

	l_bOk = l_bOk && (useFormulaInput.GetValue( l_bUseFormula ) == S_OK );

	SVPolarTransformationToolClass* pTool = dynamic_cast<SVPolarTransformationToolClass*>( GetTool() );

	if( pTool )
	{
		l_bOk = l_bOk && ( pTool->m_svAngularMethod.GetValue( l_lAngularMethod ) == S_OK );
	}
	else
		ASSERT( FALSE );


	double l_dStartAngle = 0.0;
	double l_dEndAngle = 0.0;
	double l_dCenterX = 0.0;
	double l_dCenterY = 0.0;
	double l_dStartRadius = 0.0;
	double l_dEndRadius = 0.0;

	if ( l_bOk )
	{
		if ( l_bUseFormula )
		{
			l_bOk = l_bOk && getInputStartAngleResult() != NULL && ( getInputStartAngleResult()->GetValue( l_dStartAngle ) == S_OK );

			l_bOk = l_bOk && getInputEndAngleResult() != NULL && ( getInputEndAngleResult()->GetValue( l_dEndAngle ) == S_OK );

			l_bOk = l_bOk && getInputCenterXResult() != NULL && ( getInputCenterXResult()->GetValue( l_dCenterX ) == S_OK );
			l_bOk = l_bOk && ( centerX.SetValue( RRunStatus.m_lResultDataIndex, l_dCenterX ) == S_OK );

			l_bOk = l_bOk && getInputCenterYResult() != NULL && ( getInputCenterYResult()->GetValue( l_dCenterY ) == S_OK );
			l_bOk = l_bOk && ( centerY.SetValue( RRunStatus.m_lResultDataIndex, l_dCenterY ) == S_OK );

			l_bOk = l_bOk && getInputStartRadiusResult() != NULL && ( getInputStartRadiusResult()->GetValue( l_dStartRadius ) == S_OK );
			l_bOk = l_bOk && ( startRadius.SetValue( RRunStatus.m_lResultDataIndex, l_dStartRadius ) == S_OK );

			l_bOk = l_bOk && getInputEndRadiusResult() != NULL && ( getInputEndRadiusResult()->GetValue( l_dEndRadius ) == S_OK );
			l_bOk = l_bOk && ( endRadius.SetValue( RRunStatus.m_lResultDataIndex, l_dEndRadius ) == S_OK );
		}
		else
		{
			l_bOk = l_bOk && ( startAngle.GetValue( l_dStartAngle ) == S_OK );
			l_bOk = l_bOk && ( endAngle.GetValue( l_dEndAngle ) == S_OK );
		}

		// Correct start and end angle...
		// ]-360.0...360.0[
		// start angle must always be lower than end angle, so that
		// MIL works all the time counter clockwise...
		if( l_lAngularMethod == 1 )
		{
			NewCorrectAngles( l_dStartAngle, l_dEndAngle );
		}
		else
		{
			correctAngles(  l_dStartAngle, l_dEndAngle );
		}


		// Write back modified angles...
		l_bOk = l_bOk && ( startAngle.SetValue( RRunStatus.m_lResultDataIndex, l_dStartAngle ) == S_OK );
		l_bOk = l_bOk && ( endAngle.SetValue( RRunStatus.m_lResultDataIndex, l_dEndAngle ) == S_OK );
	}

	if ( l_bOk )
	{
		SVImageExtentClass l_svExtents;

    SVSmartHandlePointer l_svInputHandle;
    SVSmartHandlePointer l_svOutputHandle;

		l_bOk = l_bOk && (GetTool() != NULL) && (GetTool()->GetImageExtent( l_svExtents ) == S_OK);

		l_bOk = l_bOk && outputImageObject.SetImageHandleIndex( RRunStatus.Images );

		//added "true" to the call of UpdateImage - if the tool only changes position the output image does not need to be re-created
		l_bOk = l_bOk && outputImageObject.UpdateImage( l_svExtents,true ) == S_OK;

		l_svExtents = outputImageObject.GetImageExtents( );

		l_bOk = l_bOk && outputImageObject.GetImageHandle( l_svOutputHandle ) && !( l_svOutputHandle.empty() );

		l_bOk = l_bOk && getInputImage() != NULL && getInputImage()->GetImageHandle( l_svInputHandle ) && !( l_svInputHandle.empty() );

		SVImageBufferHandleImage l_InMilHandle;
		l_bOk = l_bOk && ( l_svInputHandle->GetData( l_InMilHandle ) == S_OK );

		SVImageBufferHandleImage l_OutMilHandle;
		l_bOk = l_bOk && ( l_svOutputHandle->GetData( l_OutMilHandle ) == S_OK );

		if ( l_bOk )
		{
			long l_lInterpolationMode;

			double l_dCenterX = 0.0;
			double l_dCenterY = 0.0;
			double l_dStartRadius = 0.0;
			double l_dEndRadius = 0.0;
			double l_dStartAngle = 0.0;
			double l_dEndAngle = 0.0;
			double l_dOutputWidth = 0.0;
			double l_dOutputHeight = 0.0;

			l_bOk = l_bOk && l_svExtents.GetExtentProperty( SVExtentPropertyPositionPointX, l_dCenterX ) == S_OK;
			l_bOk = l_bOk && l_svExtents.GetExtentProperty( SVExtentPropertyPositionPointY, l_dCenterY ) == S_OK;
			l_bOk = l_bOk && l_svExtents.GetExtentProperty( SVExtentPropertyInnerRadius, l_dStartRadius ) == S_OK;
			l_bOk = l_bOk && l_svExtents.GetExtentProperty( SVExtentPropertyOuterRadius, l_dEndRadius ) == S_OK;
			l_bOk = l_bOk && l_svExtents.GetExtentProperty( SVExtentPropertyStartAngle, l_dStartAngle ) == S_OK;
			l_bOk = l_bOk && l_svExtents.GetExtentProperty( SVExtentPropertyEndAngle, l_dEndAngle ) == S_OK;
			l_bOk = l_bOk && l_svExtents.GetExtentProperty( SVExtentPropertyOutputWidth, l_dOutputWidth ) == S_OK;
			l_bOk = l_bOk && l_svExtents.GetExtentProperty( SVExtentPropertyOutputHeight, l_dOutputHeight ) == S_OK;

			l_bOk = l_bOk && ( interpolationMode.GetValue( l_lInterpolationMode ) == S_OK );

			if ( l_bOk )
			{
				try
				{
					
					SVMatroxImageInterface::SVStatusCode l_Code;

					// Get the default control word.   
					int cw = _controlfp( 0,0 );

					// This one turns off exception generation
					int newCw = cw | (EM_OVERFLOW|EM_UNDERFLOW|EM_INEXACT|EM_ZERODIVIDE|EM_DENORMAL);

					// Set the control word.   
					_controlfp( newCw, MCW_EM );
					double l_dTmpStartAngle = l_dStartAngle;
					double l_dTmpEndAngle =  l_dEndAngle;

					double l_dAngularDistance = l_dEndAngle - l_dStartAngle;
					if( l_dAngularDistance > 720.0 )
					{
						l_dAngularDistance = 720.0 ;
						l_dEndAngle = l_dStartAngle + 720.0;
					}

					// Do transform...
					AnglesTo360(l_dTmpStartAngle, l_dTmpEndAngle);


					SVMatroxPolarTransformStruct l_Polar(l_InMilHandle.GetBuffer());
					l_Polar.m_dCenterX = l_dCenterX;
					l_Polar.m_dCenterY = l_dCenterY;
					l_Polar.m_dStartRadius = l_dStartRadius;
					l_Polar.m_dEndRadius = l_dEndRadius;
					l_Polar.m_dStartAngle = l_dTmpStartAngle;
					l_Polar.m_dEndAngle = l_dTmpEndAngle;
					l_Polar.m_eInterpMode = static_cast<SVImageOperationTypeEnum>(l_lInterpolationMode);
					l_Polar.m_eOpMode = SVRectToPolar;

					l_Code = SVMatroxImageInterface::PolarTransform(l_OutMilHandle.GetBuffer(), l_Polar );



					if( l_dAngularDistance > 360 )
					{
						//  Small Part 
						double l_dPartialWidth = l_dOutputWidth - floor( l_Polar.m_dDestSizeX ); 

						if( l_dPartialWidth >= 1.0 )
						{
							l_Code = SVMatroxBufferInterface::CopyBuffer( l_OutMilHandle.GetBuffer(), l_OutMilHandle.GetBuffer(), (long) l_Polar.m_dDestSizeX,  0L);
						}
					}


					if( l_Code != SVMEE_STATUS_OK )
					{
						l_bOk = FALSE;
					}


					// Reset it back
					_controlfp( cw, MCW_EM );
				}
				catch( ... )
				{
					_clearfp();

					l_bOk = FALSE;
				}

				if( l_bOk )
				{
					RRunStatus.SetPassed();
				}
			}
		}
	}

	if ( ! l_bOk )
	{
		SetInvalid();
		RRunStatus.SetInvalid();
	}

	return l_bOk;
}

DWORD_PTR SVImagePolarTransformClass::processMessage( DWORD DwMessageID, DWORD_PTR DwMessageValue, DWORD_PTR DwMessageContext )
{
	DWORD_PTR DwResult = NULL;

	DWORD dwPureMessageID = DwMessageID & SVM_PURE_MESSAGE;

	switch( dwPureMessageID )
	{
	case SVMSGID_RESET_ALL_OBJECTS:
		{
			HRESULT l_ResetStatus = ResetObject();
			if( l_ResetStatus != S_OK )
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

////////////////////////////////////////////////////////////////////////////////
// is... Operator(s)
////////////////////////////////////////////////////////////////////////////////

void SVImagePolarTransformClass::SetCalculatedPrintableFlags()
{
	BOOL l_bSetValue;

	if ( ( useFormulaInput.GetValue( l_bSetValue )  == S_OK ) && l_bSetValue )
	{
		//turn off the print flags
		//turn off setable remotely/online
		centerX.ObjectAttributesAllowedRef() &= ~SV_PRINTABLE & ~SV_REMOTELY_SETABLE  & ~SV_SETABLE_ONLINE;
		centerY.ObjectAttributesAllowedRef() &= ~SV_PRINTABLE & ~SV_REMOTELY_SETABLE  & ~SV_SETABLE_ONLINE;
		startRadius.ObjectAttributesAllowedRef() &= ~SV_PRINTABLE & ~SV_REMOTELY_SETABLE  & ~SV_SETABLE_ONLINE;
		endRadius.ObjectAttributesAllowedRef() &= ~SV_PRINTABLE & ~SV_REMOTELY_SETABLE  & ~SV_SETABLE_ONLINE;
		startAngle.ObjectAttributesAllowedRef() &= ~SV_PRINTABLE & ~SV_REMOTELY_SETABLE & ~SV_SETABLE_ONLINE;
		endAngle.ObjectAttributesAllowedRef() &= ~SV_PRINTABLE & ~SV_REMOTELY_SETABLE & ~SV_SETABLE_ONLINE;
	}
	else
	{
		//turn print flag ons
		//turn on setable remotely/online flags
		centerX.ObjectAttributesAllowedRef() |= SV_PRINTABLE | SV_REMOTELY_SETABLE  | SV_SETABLE_ONLINE;
		centerY.ObjectAttributesAllowedRef() |= SV_PRINTABLE | SV_REMOTELY_SETABLE  | SV_SETABLE_ONLINE;
		startRadius.ObjectAttributesAllowedRef() |= SV_PRINTABLE | SV_REMOTELY_SETABLE  | SV_SETABLE_ONLINE;
		endRadius.ObjectAttributesAllowedRef() |= SV_PRINTABLE | SV_REMOTELY_SETABLE | SV_SETABLE_ONLINE;
		startAngle.ObjectAttributesAllowedRef() |= SV_PRINTABLE | SV_REMOTELY_SETABLE  | SV_SETABLE_ONLINE;
		endAngle.ObjectAttributesAllowedRef() |= SV_PRINTABLE | SV_REMOTELY_SETABLE  | SV_SETABLE_ONLINE;
	}

}

HRESULT SVImagePolarTransformClass::ResetObject()
{
	SVPolarTransformationToolClass* pTool = dynamic_cast<SVPolarTransformationToolClass*> ( GetOwner() );
	ASSERT( pTool );

	HRESULT hr;
	bool bUseFormula = false;
	BOOL bOk = ( useFormulaInput.GetValue( bUseFormula ) == S_OK );	// one formula option for all values?
	SVExtentPropertyInfoStruct info;

	hr = pTool->m_svToolExtent.GetExtentPropertyInfo( SVExtentPropertyPositionPointX, info );
	info.bFormula = bUseFormula;
	hr = pTool->m_svToolExtent.SetExtentPropertyInfo( SVExtentPropertyPositionPointX, info );

	hr = pTool->m_svToolExtent.GetExtentPropertyInfo( SVExtentPropertyPositionPointY, info );
	info.bFormula = bUseFormula;
	hr = pTool->m_svToolExtent.SetExtentPropertyInfo( SVExtentPropertyPositionPointY, info );

	hr = pTool->m_svToolExtent.GetExtentPropertyInfo( SVExtentPropertyInnerRadius, info );
	info.bFormula = bUseFormula;
	hr = pTool->m_svToolExtent.SetExtentPropertyInfo( SVExtentPropertyInnerRadius, info );

	hr = pTool->m_svToolExtent.GetExtentPropertyInfo( SVExtentPropertyOuterRadius, info );
	info.bFormula = bUseFormula;
	hr = pTool->m_svToolExtent.SetExtentPropertyInfo( SVExtentPropertyOuterRadius, info );

	hr = pTool->m_svToolExtent.GetExtentPropertyInfo( SVExtentPropertyStartAngle, info );
	info.bFormula = bUseFormula;
	hr = pTool->m_svToolExtent.SetExtentPropertyInfo( SVExtentPropertyStartAngle, info );

	hr = pTool->m_svToolExtent.GetExtentPropertyInfo( SVExtentPropertyEndAngle, info );
	info.bFormula = bUseFormula;
	hr = pTool->m_svToolExtent.SetExtentPropertyInfo( SVExtentPropertyEndAngle, info );

	hr = pTool->m_svToolExtent.GetExtentPropertyInfo( SVExtentPropertyRotationAngle, info );
	info.bFormula = bUseFormula;
	hr = pTool->m_svToolExtent.SetExtentPropertyInfo( SVExtentPropertyRotationAngle, info );

	HRESULT l_hrOk = outputImageObject.InitializeImage( getInputImage() );

	if (SVPolarTransformClass::ResetObject() != S_OK )
	{
		l_hrOk = S_FALSE;
	}

	CollectInputImageNames();

	return l_hrOk;
}

// Set String value object for Source Image Names
HRESULT SVImagePolarTransformClass::CollectInputImageNames()
{
	HRESULT l_hr = S_FALSE;
	SVImageClass* l_pInputImage = getInputImage();
	SVPolarTransformationToolClass* l_pTool = dynamic_cast<SVPolarTransformationToolClass*>(GetTool());
	if( l_pInputImage && l_pTool )
	{
		CString l_strName = l_pInputImage->GetCompleteObjectName();

		l_pTool->m_svSourceImageNames.SetValue( 0, l_strName );

		l_hr = S_OK;
	}
	return l_hr;
}

//******************************************************************************
//* LOG HISTORY:
//******************************************************************************
/*
$Log:   N:\PVCSarch65\ProjectFiles\archives\SVObserver_SRC\SVObserver\SVImagePolarTransform.cpp_v  $
 * 
 *    Rev 1.4   20 Nov 2014 07:02:58   ryoho
 * Project:  SVObserver
 * Change Request (SCR) nbr:  971
 * SCR Title:  Reduce tool time for Polar Unwrap when using formula for position (SVO-317)
 * Checked in by:  rYoho;  Rob Yoho
 * Change Description:  
 *   Modified onRun - added 'true' to the call of UpdateImage.  This should not re-create the output buffer if only the position of the tool has changed.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.3   15 May 2014 12:47:34   sjones
 * Project:  SVObserver
 * Change Request (SCR) nbr:  852
 * SCR Title:  Add Multiple Platform Support to SVObserver's Visual Studio Solution
 * Checked in by:  tBair;  Tom Bair
 * Change Description:  
 *   Revised processMessage to use DWORD_PTR
 * Revised SVSendMessage to use DWORD_PTR
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.2   01 Feb 2014 10:42:32   tbair
 * Project:  SVObserver
 * Change Request (SCR) nbr:  852
 * SCR Title:  Add Multiple Platform Support to SVObserver's Visual Studio Solution
 * Checked in by:  tBair;  Tom Bair
 * Change Description:  
 *   Changed sendmessage to use LONG_PTR instead of DWORD.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.1   13 May 2013 12:16:10   bWalter
 * Project:  SVObserver
 * Change Request (SCR) nbr:  814
 * SCR Title:  Upgrade SVObserver to Compile Using Visual Studio 2010
 * Checked in by:  bWalter;  Ben Walter
 * Change Description:  
 *   Merged with svo_src label SVO 6.10 Beta 014.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.40   02 May 2013 11:05:48   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  810
 * SCR Title:  Remove support for Legacy Tools
 * Checked in by:  rYoho;  Rob Yoho
 * Change Description:  
 *   Updated process message method to fix formatting issues and updated the reset object section to only assert when the HRESULT is a failed status.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.0   23 Apr 2013 10:53:44   bWalter
 * Project:  SVObserver
 * Change Request (SCR) nbr:  814
 * SCR Title:  Upgrade SVObserver to Compile Using Visual Studio 2010
 * Checked in by:  bWalter;  Ben Walter
 * Change Description:  
 *   Initial check in to SVObserver_src.  (Merged with svo_src label SVO 6.10 Beta 008.)
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.39   12 Jul 2012 14:58:00   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  769
 * SCR Title:  Fix Problems and Crashes with Inspection Document Display Updates
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated source code to add in an additional string identifier for the input and fixed clear functionality assoicated with input structure.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.38   19 Jun 2012 12:47:04   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  744
 * SCR Title:  Add Shared Memory and Socket Functionality for Run Page Web Server
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Protected Object Information structure attributes.  This change will add accessor methods to get and set attributes.
 * Converted GUID to SVGUID.   This allows the use of standard operators for comparison and methods for conversion.
 * 
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.37   16 Mar 2011 13:35:54   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  712
 * SCR Title:  Fix issues with black images when using command interface (SIAC)
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated extent functionality and renamed extent methods to correct extent issues and add clarity to the logical flow.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.36   11 Mar 2011 14:23:02   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  712
 * SCR Title:  Fix issues with black images when using command interface (SIAC)
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated source code to fix extent and connection issues between image classes and tool classes.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.35   25 Feb 2011 12:11:04   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  712
 * SCR Title:  Fix issues with black images when using command interface (SIAC)
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated classes needed to consolidate SVImageClass and SVImageObjectClass updating calls to one method name.  Fixed updating problems with child buffers.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.34   10 Feb 2011 14:45:48   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  712
 * SCR Title:  Fix issues with black images when using command interface (SIAC)
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated source code to now use shared pointer instead of untracked structure.  This fixes destruction, tracking and rebuilding issues.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.33   27 Jan 2011 11:20:48   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  712
 * SCR Title:  Fix issues with black images when using command interface (SIAC)
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated source code to fix issues with Matrox Buffer Management.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.32   04 Nov 2010 13:39:40   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  707
 * SCR Title:  Change Inspection Display Functionality to Force Display of Last Inspected
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated source code to fix issues with image processing and display image processing classes and associated includes.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.31   01 Jun 2010 14:24:18   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  693
 * SCR Title:  Fix Performance Issue with Inspection Process
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated source code to improve ability to track performance.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.30   16 Dec 2009 11:00:46   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  677
 * SCR Title:  Fix problem in camera notify thread
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Fix issues with includes and comments.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.29   07 Mar 2008 11:54:58   tbair
 * Project:  SVObserver
 * Change Request (SCR) nbr:  616
 * SCR Title:  Add Server-side Configuration Data Def Lists to the SVObserver program
 * Checked in by:  tBair;  Tom Bair
 * Change Description:  
 *   Added support for Transfer Image Definition List Source Image Names.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.28   24 Jul 2007 13:31:26   tbair
 * Project:  SVObserver
 * Change Request (SCR) nbr:  597
 * SCR Title:  Upgrade Matrox Imaging Library to version 8.0
 * Checked in by:  tBair;  Tom Bair
 * Change Description:  
 *   Update to MIL 8.0
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.27   19 Feb 2007 16:36:02   Joe
 * Project:  SVObserver
 * Change Request (SCR) nbr:  594
 * SCR Title:  Remove issue with interpreting status response incorrectly
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated MIL error code methodology to use the new SVMatroxAppManager GetStatusCode operator.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.26   15 Feb 2007 11:46:12   Joe
 * Project:  SVObserver
 * Change Request (SCR) nbr:  594
 * SCR Title:  Remove issue with interpreting status response incorrectly
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated flags for the mappgeterror fucntion to remove the M_MESSAGE flag.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.25   02 Nov 2005 07:33:14   ebeyeler
 * Project:  SVObserver
 * Change Request (SCR) nbr:  512
 * SCR Title:  Add Published Result Images functionality
 * Checked in by:  eBeyeler;  Eric Beyeler
 * Change Description:  
 *   changed RunStatus.m_lResultImageIndex to .Images
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.24   08 Aug 2005 16:01:04   Joe
 * Project:  SVObserver
 * Change Request (SCR) nbr:  500
 * SCR Title:  Reduce delay when adjusting tool parameters with a large toolset
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated objects and dialogs to use the ActiveX Interface to update inspection data.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.23   29 Jul 2005 12:36:24   Joe
 * Project:  SVObserver
 * Change Request (SCR) nbr:  500
 * SCR Title:  Reduce delay when adjusting tool parameters with a large toolset
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated object initialization to change the parameters for RegisterEmbeddedObject for a SVValueObjectClass.  The two additional parameters will inform the preperation process before toolset execution.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.22   21 Jun 2005 08:08:54   ebeyeler
 * Project:  SVObserver
 * Change Request (SCR) nbr:  464
 * SCR Title:  Add array indexing for value objects
 * Checked in by:  eBeyeler;  Eric Beyeler
 * Change Description:  
 *   object attributes now use accessor methods
 * value object functions now use HRESULT
 * equation uses bPassed
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.21   24 Feb 2005 10:23:34   tbair
 * Project:  SVObserver
 * Change Request (SCR) nbr:  449
 * SCR Title:  Modify Polar Un-Wrap Tool
 * Checked in by:  tBair;  Tom Bair
 * Change Description:  
 *   Made changes to use a Angular Method combo instead of a use new method check box.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.20   18 Feb 2005 09:12:54   tbair
 * Project:  SVObserver
 * Change Request (SCR) nbr:  449
 * SCR Title:  Modify Polar Un-Wrap Tool
 * Checked in by:  tBair;  Tom Bair
 * Change Description:  
 *   Added newCorrectAngles( to allow PolarUnwrap to 720 degrees.
 * Modified OnRun to go beyond 360 degrees.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.19   17 Feb 2005 14:27:32   Joe
 * Project:  SVObserver
 * Change Request (SCR) nbr:  456
 * SCR Title:  Update Image and Tool Objects to use the new Extent Classes
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Update class with new methodologies based on new base object, extents and reset structure.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.18   16 Feb 2005 14:33:38   ryoho
 * Project:  SVObserver
 * Change Request (SCR) nbr:  440
 * SCR Title:  Create Internal Tool Object to Managing Tool Extents and Result Extents
 * Checked in by:  rYoho;  Rob Yoho
 * Change Description:  
 *   updated header files
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.17   27 May 2004 07:59:50   ryoho
 * Project:  SVObserver
 * Change Request (SCR) nbr:  423
 * SCR Title:  Do not Print Calculated Values from the Polar Unwrap Tool in Print Report
 * Checked in by:  rYoho;  Rob Yoho
 * Change Description:  
 *   added new method SetCalculatedPrintableFlags.  When use formula is checked, printable flags and remotely setable and online flags are turned off
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.16   20 Nov 2003 11:52:10   ryoho
 * Project:  SVObserver
 * Change Request (SCR) nbr:  385
 * SCR Title:  Expose additional parameters for SIAC
 * Checked in by:  rYoho;  Rob Yoho
 * Change Description:  
 *   changed attributes for the value objects to be setable online
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.15   08 Jul 2003 09:23:08   ebeyeler
 * Project:  SVObserver
 * Change Request (SCR) nbr:  322
 * SCR Title:  Add Additional Digital IO Resources to SVObserver
 * Checked in by:  eBeyeler;  Eric Beyeler
 * Change Description:  
 *   changed BOOL to HRESULT
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.14   22 Apr 2003 10:18:48   rschock
 * Project:  SVObserver
 * Change Request (SCR) nbr:  346
 * SCR Title:  Update SVObserver to Version 4.21 Release
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Redid the #include defines and standardized the Tracker log headers and removed warning from release mode builds.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.13   14 Mar 2003 17:36:42   rschock
 * Project:  SVObserver
 * Change Request (SCR) nbr:  282
 * SCR Title:  Dr.W if Blob Analyzer is closed using the System Menu Close Icon.
 * Checked in by:  rSchock;  Rosco Schock
 * Change Description:  
 *   Fixed existing bug I0001091.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.12   07 Mar 2003 14:36:52   rschock
 * Project:  SVObserver
 * Change Request (SCR) nbr:  301
 * SCR Title:  Implement Result Image Circle Buffer
 * Checked in by:  rYoho;  Rob Yoho
 * Change Description:  
 *   Change unwrap tool to use a temporary buffer to create its destination image so that if a source image isn't available it will still load and create.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.11   05 Mar 2003 12:13:32   rschock
 * Project:  SVObserver
 * Change Request (SCR) nbr:  307
 * SCR Title:  Implement the ability to remotely change Tool parameters on the SVIM
 * Checked in by:  tBair;  Tom Bair
 * Change Description:  
 *   Restructured and recoded the SetToolParameter code. It now uses a standard struct and each SVToolClass based class is responsible for moving itself based on the values passed in through the struct.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.10   29 Jan 2003 18:02:18   joe
 * Project:  SVObserver
 * Change Request (SCR) nbr:  301
 * SCR Title:  Implement Result Image Circle Buffer
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated CreateObject, calcPolarDestSize, checkOutputImageDimensions, onRun and processMessage with new image buffer methodology.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.9   10 Jan 2003 14:24:30   rschock
 * Project:  SVObserver
 * Change Request (SCR) nbr:  305
 * SCR Title:  Implement the ability to perform RunOnce from a SIAC client
 * Checked in by:  rSchock;  Rosco Schock
 * Change Description:  
 *   Added new function VerifyImageForOverlay to determine which object has overlays on which image.
 * Made changes related to changing the SVDrawObjectClass to work in a consistent manner with SVDrawObjectListClass.
 * Added method to return the overlay figure from this object.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.8   03 Jan 2003 15:24:22   ryoho
 * Project:  SVObserver
 * Change Request (SCR) nbr:  307
 * SCR Title:  Implement the ability to remotely change Tool parameters on the SVIM
 * Checked in by:  rYoho;  Rob Yoho
 * Change Description:  
 *   added attributes for set able online and extent object
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.7   09 Dec 2002 19:24:16   Joe
 * Project:  SVObserver
 * Change Request (SCR) nbr:  295
 * SCR Title:  Remove Result Data from Configuration Printout
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   CreateObject method was modified to properly update the isCreated flag and to set the printability of the internal value objects.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.6   19 Nov 2002 13:25:26   ryoho
 * Project:  SVObserver
 * Change Request (SCR) nbr:  226
 * SCR Title:  Monochrome SVIM configuration compatibility between ViperQUAD and ViperDUAL
 * Checked in by:  rYoho;  Rob Yoho
 * Change Description:  
 *   Removed references to System
 *         Changed ValueObject semantics to work with buckets
 * 
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.5   04 Oct 2001 15:34:40   Joe
 * Project:  SVObserver
 * Change Request (SCR) nbr:  229
 * SCR Title:  Upgrade SVObserver to Version 3.34 Release
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Modified source code to remove compiler warning message.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.4   19 Apr 2001 14:31:30   Steve
 * Project:  SVObserver
 * Change Request (SCR) nbr:  196
 * SCR Title:  Restructure Scripted Load/Save Procedures Functional Requirement
 * Checked in by:  Steve;  Stephen E. Steffan
 * Change Description:  
 *   Header file changes due to include file restructuring.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.3   03 Mar 2000 16:05:10   sjones
 * Project:  SVObserver
 * Change Request (SCR) nbr:  113
 * SCR Title:  Integrate new Polar Unwrap Tool
 * Checked in by:  Nick;  F Roland "Nick" Bjorklund
 * Change Description:  
 *   Introduced checking of resizing in size...(...) functions.
 * Added and introduced method to correct Angle overrun.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.2   03 Mar 2000 14:11:00   sjones
 * Project:  SVObserver
 * Change Request (SCR) nbr:  113
 * SCR Title:  Integrate new Polar Unwrap Tool
 * Checked in by:  Nick;  F Roland "Nick" Bjorklund
 * Change Description:  
 *   Added method to Set Default Formulas.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.1   03 Mar 2000 13:52:36   sjones
 * Project:  SVObserver
 * Change Request (SCR) nbr:  112
 * SCR Title:  Expose Extents of Images
 * Checked in by:  sJones;  Steve Jones
 * Change Description:  
 *   Revised to use SVImageInfoClass methdos to get/set image extnets.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.0   Mar 01 2000 12:06:18   Nick
 * Project:  SVObserver
 * Change Request (SCR) nbr:  113
 * SCR Title:  Integrate new Polar Unwrap Tool
 * Checked in by:  Nick;  F Roland "Nick" Bjorklund
 * Change Description:  
 *   NEW Module: Provides image polar transformation.
 * 
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
*/
