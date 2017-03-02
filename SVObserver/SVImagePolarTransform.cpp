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

#pragma region Includes
#include "stdafx.h"
//Moved to precompiled header: #include <float.h>
#include "SVImagePolarTransform.h"

#include "SVImageLibrary/SVImageBufferHandleImage.h"
#include "SVEvaluateCenterXClass.h"
#include "SVEvaluateCenterYClass.h"
#include "SVEvaluateStartRadiusClass.h"
#include "SVEvaluateEndRadiusClass.h"
#include "SVEvaluateStartAngleClass.h"
#include "SVEvaluateEndAngleClass.h"
#include "SVEquation.h"
#include "ObjectInterfaces/GlobalConst.h"
#include "SVOCore/SVImageProcessingClass.h"
#include "SVPolarTransformationTool.h"
#include "CameraLibrary/SVDeviceParams.h" //Arvid added to avoid VS2015 compile Error

#pragma endregion Includes

double SVImagePolarTransformClass::g_dMaxAngularDistance  = 720.0;

SV_IMPLEMENT_CLASS( SVImagePolarTransformClass, SVImagePolarTransformClassGuid )

SVImagePolarTransformClass::SVImagePolarTransformClass( SVObjectClass* POwner, int StringResourceID )
						   :SVPolarTransformClass( POwner, StringResourceID ) 
{
	// Identify yourself
	m_outObjectInfo.ObjectTypeInfo.SubType = SVImagePolarTransformObjectType;

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
	SVString Enable = SvUl_SF::LoadSVString( IDS_ENABLE_STRING );
	SVString Disable = SvUl_SF::LoadSVString( IDS_DISABLE_STRING );
	SVString Overscan = SvUl_SF::LoadSVString( IDS_OVERSCAN_STRING );
	SVString OverscanClear = SvUl_SF::LoadSVString( IDS_OVERSCAN_CLEAR_STRING );

	SVString Mode;
	SVString Text;
	SVString EnumTypes;

	// M_NEAREST_NEIGHBOR + M_OVERSCAN_ENABLE
	Mode = SvUl_SF::LoadSVString( IDS_NEAREST_NEIGHBOR_STRING );
	Text = SvUl_SF::Format( _T("%s+%s %s=%d,"), Mode.c_str(), Overscan.c_str(), Enable.c_str(), SVNearestNeighOverScanEnable ); // M_NEAREST_NEIGHBOR + M_OVERSCAN_ENABLE );
	EnumTypes += Text;
	// M_NEAREST_NEIGHBOR + M_OVERSCAN_DISABLE
	Text = SvUl_SF::Format( _T("%s+%s %s=%d,"), Mode.c_str(), Overscan.c_str(), Disable.c_str(), SVNearestNeighOverScanDisable ); // M_NEAREST_NEIGHBOR + M_OVERSCAN_DISABLE );
	EnumTypes += Text;
	// M_NEAREST_NEIGHBOR + M_OVERSCAN_CLEAR
	Text = SvUl_SF::Format( _T("%s+%s=%d,"), Mode.c_str(), OverscanClear.c_str(), SVNearestNeighOverScanClear ); // M_NEAREST_NEIGHBOR + M_OVERSCAN_CLEAR );
	EnumTypes += Text;

	// M_BILINEAR + M_OVERSCAN_ENABLE
	Mode = SvUl_SF::LoadSVString( IDS_BILINEAR_STRING );
	Text = SvUl_SF::Format( _T("%s+%s %s=%d,"), Mode.c_str(), Overscan.c_str(), Enable.c_str(), SVBilinearOverScanEnable ); // M_BILINEAR + M_OVERSCAN_ENABLE );
	EnumTypes += Text;
	// M_BILINEAR + M_OVERSCAN_DISABLE
	Text = SvUl_SF::Format( _T("%s+%s %s=%d,"), Mode.c_str(), Overscan.c_str(), Disable.c_str(), SVBilinearOverScanClear ); // M_BILINEAR + M_OVERSCAN_DISABLE );
	EnumTypes += Text;
	// M_BILINEAR + M_OVERSCAN_CLEAR
	Text = SvUl_SF::Format( _T("%s+%s=%d,"), Mode.c_str(), OverscanClear.c_str(), SVBilinearOverScanClear ); // M_BILINEAR + M_OVERSCAN_CLEAR );
	EnumTypes += Text;

	// M_BICUBIC + M_OVERSCAN_ENABLE
	Mode = SvUl_SF::LoadSVString( IDS_BICUBIC_STRING );
	Text = SvUl_SF::Format( _T("%s+%s %s=%d,"), Mode.c_str(), Overscan.c_str(), Enable.c_str(), SVBiCubicOverScanEnable ); // M_BICUBIC + M_OVERSCAN_ENABLE );
	EnumTypes += Text;
	// M_BICUBIC + M_OVERSCAN_DISABLE
	Text = SvUl_SF::Format( _T("%s+%s %s=%d,"), Mode.c_str(), Overscan.c_str(), Disable.c_str(), SVBiCubicOverScanDisable ); // M_BICUBIC + M_OVERSCAN_DISABLE );
	EnumTypes += Text;
	// M_BICUBIC + M_OVERSCAN_CLEAR
	Text = SvUl_SF::Format( _T("%s+%s=%d,"), Mode.c_str(), OverscanClear.c_str(), SVBiCubicOverScanClear ); // M_BICUBIC + M_OVERSCAN_CLEAR );
	EnumTypes += Text;

	// And now set enum types...
	interpolationMode.SetEnumTypes( EnumTypes.c_str() );


	// Set Embedded defaults
	interpolationMode.SetDefaultValue( SVNearestNeighOverScanEnable, TRUE );	// Refer to MIL...
	centerX.SetDefaultValue( ( double ) SvOi::cDefaultPolarTransformCenterX, TRUE );
	centerY.SetDefaultValue( ( double ) SvOi::cDefaultPolarTransformCenterY, TRUE );
	startRadius.SetDefaultValue( ( double ) SvOi::cDefaultPolarTransformStartRadius, TRUE );
	endRadius.SetDefaultValue( ( double ) SvOi::cDefaultPolarTransformEndRadius, TRUE );
	startAngle.SetDefaultValue( ( double ) SvOi::cDefaultPolarTransformStartAngle, TRUE );
	endAngle.SetDefaultValue( ( double ) SvOi::cDefaultPolarTransformEndAngle, TRUE );

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
	SVToolClass* pTool = dynamic_cast<SVToolClass*>(GetTool());
	l_bOk = l_bOk && nullptr != pTool;

	l_bOk = l_bOk && S_OK == pTool->SetImageExtentProperty( SVExtentPropertyPositionPointX, &centerX );
	l_bOk = l_bOk && S_OK == pTool->SetImageExtentProperty( SVExtentPropertyPositionPointY, &centerY );
	l_bOk = l_bOk && S_OK == pTool->SetImageExtentProperty( SVExtentPropertyInnerRadius, &endRadius );
	l_bOk = l_bOk && S_OK == pTool->SetImageExtentProperty( SVExtentPropertyOuterRadius, &startRadius );
	l_bOk = l_bOk && S_OK == pTool->SetImageExtentProperty( SVExtentPropertyStartAngle, &startAngle );
	l_bOk = l_bOk && S_OK == pTool->SetImageExtentProperty( SVExtentPropertyEndAngle, &endAngle );

	l_bOk &= S_OK == ( outputImageObject.InitializeImage( getInputImage() ) );

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

	m_isCreated = l_bOk;

	return m_isCreated;
}

HRESULT SVImagePolarTransformClass::IsInputImage( SVImageClass *p_psvImage )
{
	HRESULT l_hrOk = S_FALSE;

	if ( nullptr != p_psvImage && p_psvImage == getInputImage() )
	{
		l_hrOk = S_OK;
	}

	return l_hrOk;
}

SVImageClass* SVImagePolarTransformClass::getInputImage() const
{
	if( inputImageObjectInfo.IsConnected() && inputImageObjectInfo.GetInputObjectInfo().PObject )
		return ( SVImageClass* ) inputImageObjectInfo.GetInputObjectInfo().PObject;

	return nullptr;
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
// Another possibility to call this function is in the setup
// dialog, if a kind of reset formula button exists.
bool SVImagePolarTransformClass::SetDefaultFormulas(SvStl::MessageContainerVector *pErrorMessages)
{
	bool bRetVal = true;
	bool bOk;
	SVObjectTypeInfoStruct objectInfo;
	SVObjectTypeInfoStruct equationObjectInfo;
	equationObjectInfo.ObjectType = SVEquationObjectType;
	equationObjectInfo.SubType    = SVMathEquationObjectType;
	
	// Find the evaluation center x object...
	bOk = false;
	objectInfo.SubType = SVEvaluateCenterXObjectType;
	SVEvaluateCenterXClass* pEvaluateCenterX = dynamic_cast<SVEvaluateCenterXClass*>( GetTool()->getFirstObject(objectInfo) );
	if( pEvaluateCenterX )
	{
		// Find equation object...
		SVEquationClass* pEquation = dynamic_cast<SVEquationClass*>(pEvaluateCenterX->getFirstObject(equationObjectInfo));
		SVString Name = centerX.GetCompleteObjectNameToObjectType( nullptr, SVToolSetObjectType );
		bOk = SetDefaultEquation( pEquation, Name, pErrorMessages );
	}
	if( ! bOk )
	{
		bRetVal = false;
	}

	// Find the evaluation center y object...
	bOk = false;
	objectInfo.SubType = SVEvaluateCenterYObjectType;
	SVEvaluateCenterYClass* pEvaluateCenterY = dynamic_cast<SVEvaluateCenterYClass*>(GetTool()->getFirstObject(objectInfo));
	if( pEvaluateCenterY )
	{
		// Find equation object...
		SVEquationClass* pEquation = dynamic_cast<SVEquationClass*>(pEvaluateCenterY->getFirstObject(equationObjectInfo));
		SVString Name = centerY.GetCompleteObjectNameToObjectType( nullptr, SVToolSetObjectType );
		bOk = SetDefaultEquation( pEquation, Name, pErrorMessages );
	}
	if( ! bOk )
	{
		bRetVal = false;
	}

	// Find the evaluation start radius object...
	bOk = false;
	objectInfo.SubType = SVEvaluateStartRadiusObjectType;
	SVEvaluateStartRadiusClass* pEvaluateStartRadius = dynamic_cast<SVEvaluateStartRadiusClass*>(GetTool()->getFirstObject(objectInfo));
	if( pEvaluateStartRadius )
	{
		// Find equation object...
		SVEquationClass* pEquation = dynamic_cast<SVEquationClass*>(pEvaluateStartRadius->getFirstObject(equationObjectInfo));
		SVString Name = startRadius.GetCompleteObjectNameToObjectType( nullptr, SVToolSetObjectType );
		bOk = SetDefaultEquation( pEquation, Name, pErrorMessages );
	}
	if( ! bOk )
	{
		bRetVal = false;
	}

	// Find the evaluation end radius object...
	bOk = false;
	objectInfo.SubType = SVEvaluateEndRadiusObjectType;
	SVEvaluateEndRadiusClass* pEvaluateEndRadius = dynamic_cast<SVEvaluateEndRadiusClass*>(GetTool()->getFirstObject(objectInfo));
	if( pEvaluateEndRadius )
	{
		// Find equation object...
		SVEquationClass* pEquation = dynamic_cast<SVEquationClass*>(pEvaluateEndRadius->getFirstObject(equationObjectInfo));
		SVString Name = endRadius.GetCompleteObjectNameToObjectType( nullptr, SVToolSetObjectType );
		bOk = SetDefaultEquation( pEquation, Name, pErrorMessages );
	}
	if( ! bOk )
	{
		bRetVal = false;
	}

	// Find the evaluation start angle object...
	bOk = false;
	objectInfo.SubType = SVEvaluateStartAngleObjectType;
	SVEvaluateStartAngleClass* pEvaluateStartAngle = dynamic_cast<SVEvaluateStartAngleClass*>(GetTool()->getFirstObject(objectInfo));
	if( pEvaluateStartAngle )
	{
		// Find equation object...
		SVEquationClass* pEquation = dynamic_cast<SVEquationClass*>(pEvaluateStartAngle->getFirstObject(equationObjectInfo));
		SVString Name = startAngle.GetCompleteObjectNameToObjectType( nullptr, SVToolSetObjectType );
		bOk = SetDefaultEquation( pEquation, Name, pErrorMessages );
	}
	if( ! bOk )
	{
		bRetVal = false;
	}

	// Find the evaluation end angle object...
	bOk = false;
	objectInfo.SubType = SVEvaluateEndAngleObjectType;
	SVEvaluateEndAngleClass* pEvaluateEndAngle = dynamic_cast<SVEvaluateEndAngleClass*>(GetTool()->getFirstObject(objectInfo));
	if( pEvaluateEndAngle )
	{
		// Find equation object...
		SVEquationClass* pEquation = dynamic_cast<SVEquationClass*>(pEvaluateEndAngle->getFirstObject(equationObjectInfo));
		SVString Name = endAngle.GetCompleteObjectNameToObjectType( nullptr, SVToolSetObjectType );
		bOk = SetDefaultEquation( pEquation, Name, pErrorMessages );
	}
	if( ! bOk )
	{
		bRetVal = false;
	}

	return bRetVal;
}

bool SVImagePolarTransformClass::SetDefaultEquation( SVEquationClass* pEquation, const SVString& rName, SvStl::MessageContainerVector *pErrorMessages )
{
	bool Result( false );

	if( nullptr != pEquation )
	{
		// Get current name of embedded CenterX...
		if( ! rName.empty() )
		{
			// Set equation...
			pEquation->SetEquationText( SvUl_SF::Format( _T( "\"%s\"" ), rName.c_str() ) );
			// Update symbol table and test...
			if( ! ( Result = pEquation->Test(pErrorMessages).bPassed ) )
			{
				// something is wrong...
				pEquation->SetEquationText( SVString() );
			}
		}
	}

	return Result;
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

bool SVImagePolarTransformClass::onRun( SVRunStatusClass& RRunStatus, SvStl::MessageContainerVector *pErrorMessages )
{
	BOOL l_bUseFormula = FALSE;
	long l_lAngularMethod = 0;

	bool l_bOk = __super::onRun( RRunStatus, pErrorMessages ) && ValidateLocal(pErrorMessages);

	if (S_OK != useFormulaInput.GetValue( l_bUseFormula ))
	{
		l_bOk = false;
		if (nullptr != pErrorMessages)
		{
			SvStl::MessageContainer Msg( SVMSG_SVO_92_GENERAL_ERROR, SvOi::Tid_ErrorGettingInputs, SvStl::SourceFileParams(StdMessageParams), 0, GetUniqueObjectID() );
			pErrorMessages->push_back(Msg);
		}
	}

	SVPolarTransformationToolClass* pTool = dynamic_cast<SVPolarTransformationToolClass*>( GetTool() );

	if( pTool )
	{
		if( S_OK != pTool->m_svAngularMethod.GetValue( l_lAngularMethod ) )
		{
			l_bOk = false;
			if (nullptr != pErrorMessages)
			{
				SvStl::MessageContainer Msg( SVMSG_SVO_92_GENERAL_ERROR, SvOi::Tid_ErrorGettingInputs, SvStl::SourceFileParams(StdMessageParams), 0, GetUniqueObjectID() );
				pErrorMessages->push_back(Msg);
			}
		}
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
			l_bOk = l_bOk && nullptr != getInputStartAngleResult() && S_OK == ( getInputStartAngleResult()->GetValue( l_dStartAngle ) );
			l_bOk = l_bOk && nullptr != getInputEndAngleResult() && S_OK == ( getInputEndAngleResult()->GetValue( l_dEndAngle ) );
			l_bOk = l_bOk && nullptr != getInputCenterXResult() && S_OK == ( getInputCenterXResult()->GetValue( l_dCenterX ) );
			l_bOk = l_bOk && nullptr != getInputCenterYResult() && S_OK == ( getInputCenterYResult()->GetValue( l_dCenterY ) );
			l_bOk = l_bOk && nullptr != getInputStartRadiusResult() && ( S_OK == getInputStartRadiusResult()->GetValue( l_dStartRadius ) );
			l_bOk = l_bOk && nullptr != getInputEndRadiusResult() && ( S_OK == getInputEndRadiusResult()->GetValue( l_dEndRadius ) );

			if (!l_bOk && nullptr != pErrorMessages)
			{
				SvStl::MessageContainer Msg( SVMSG_SVO_92_GENERAL_ERROR, SvOi::Tid_Error_NoResultObject, SvStl::SourceFileParams(StdMessageParams), 0, GetUniqueObjectID() );
				pErrorMessages->push_back(Msg);
			}

			if ( l_bOk )
			{
				l_bOk = l_bOk && ( S_OK == centerX.SetValue( RRunStatus.m_lResultDataIndex, l_dCenterX ) );
				l_bOk = l_bOk && ( S_OK == centerY.SetValue( RRunStatus.m_lResultDataIndex, l_dCenterY ) );
				l_bOk = l_bOk && ( S_OK == startRadius.SetValue( RRunStatus.m_lResultDataIndex, l_dStartRadius ) );
				l_bOk = l_bOk && ( S_OK == endRadius.SetValue( RRunStatus.m_lResultDataIndex, l_dEndRadius ) );
				if (!l_bOk && nullptr != pErrorMessages)
				{
					SvStl::MessageContainer Msg( SVMSG_SVO_92_GENERAL_ERROR, SvOi::Tid_SetValueFailed, SvStl::SourceFileParams(StdMessageParams), 0, GetUniqueObjectID() );
					pErrorMessages->push_back(Msg);
				}
			}
		}
		else
		{
			l_bOk = l_bOk && ( S_OK == startAngle.GetValue( l_dStartAngle ) );
			l_bOk = l_bOk && ( S_OK == endAngle.GetValue( l_dEndAngle ) );
			if (!l_bOk && nullptr != pErrorMessages)
			{
				SvStl::MessageContainer Msg( SVMSG_SVO_92_GENERAL_ERROR, SvOi::Tid_Error_NoResultObject, SvStl::SourceFileParams(StdMessageParams), 0, GetUniqueObjectID() );
				pErrorMessages->push_back(Msg);
			}
		}

		// Correct start and end angle...
		// ]-360.0...360.0[
		// start angle must always be lower than end angle, so that
		// MIL works all the time counter clockwise...
		if( 1 == l_lAngularMethod  )
		{
			NewCorrectAngles( l_dStartAngle, l_dEndAngle );
		}
		else
		{
			correctAngles(  l_dStartAngle, l_dEndAngle );
		}


		// Write back modified angles...
		if ( S_OK != startAngle.SetValue( RRunStatus.m_lResultDataIndex, l_dStartAngle ) || S_OK != endAngle.SetValue( RRunStatus.m_lResultDataIndex, l_dEndAngle ) )
		{
			l_bOk = false;
			if (nullptr != pErrorMessages)
			{
				SvStl::MessageContainer Msg( SVMSG_SVO_92_GENERAL_ERROR, SvOi::Tid_SetValueFailed, SvStl::SourceFileParams(StdMessageParams), 0, GetUniqueObjectID() );
				pErrorMessages->push_back(Msg);
			}
		}
	}

	if ( l_bOk )
	{
		SVImageExtentClass l_svExtents;

		SVSmartHandlePointer l_svInputHandle;
		SVSmartHandlePointer l_svOutputHandle;

		SVToolClass* pTool = dynamic_cast<SVToolClass*>(GetTool());
		l_bOk = l_bOk && (nullptr != pTool) && (S_OK == pTool->GetImageExtent( l_svExtents ));

		l_bOk = l_bOk && outputImageObject.SetImageHandleIndex( RRunStatus.Images );

		//added "true" to the call of UpdateImage - if the tool only changes position the output image does not need to be re-created
		l_bOk = l_bOk && S_OK == outputImageObject.UpdateImage( l_svExtents, true );

		l_svExtents = outputImageObject.GetImageExtents( );

		l_bOk = l_bOk && outputImageObject.GetImageHandle( l_svOutputHandle ) && !( l_svOutputHandle.empty() );

		l_bOk = l_bOk && getInputImage()->GetImageHandle( l_svInputHandle ) && !( l_svInputHandle.empty() );

		SVImageBufferHandleImage l_InMilHandle;
		l_bOk = l_bOk && ( S_OK == l_svInputHandle->GetData( l_InMilHandle ) );

		SVImageBufferHandleImage l_OutMilHandle;
		l_bOk = l_bOk && ( S_OK == l_svOutputHandle->GetData( l_OutMilHandle ) );

		if (!l_bOk && nullptr != pErrorMessages)
		{
			SvStl::MessageContainer Msg( SVMSG_SVO_92_GENERAL_ERROR, SvOi::Tid_RunImagePolarFailed, SvStl::SourceFileParams(StdMessageParams), 0, GetUniqueObjectID() );
			pErrorMessages->push_back(Msg);
		}

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

			l_bOk = l_bOk && S_OK == l_svExtents.GetExtentProperty( SVExtentPropertyPositionPointX, l_dCenterX );
			l_bOk = l_bOk && S_OK == l_svExtents.GetExtentProperty( SVExtentPropertyPositionPointY, l_dCenterY );
			l_bOk = l_bOk && S_OK == l_svExtents.GetExtentProperty( SVExtentPropertyInnerRadius, l_dStartRadius );
			l_bOk = l_bOk && S_OK == l_svExtents.GetExtentProperty( SVExtentPropertyOuterRadius, l_dEndRadius );
			l_bOk = l_bOk && S_OK == l_svExtents.GetExtentProperty( SVExtentPropertyStartAngle, l_dStartAngle );
			l_bOk = l_bOk && S_OK == l_svExtents.GetExtentProperty( SVExtentPropertyEndAngle, l_dEndAngle );
			l_bOk = l_bOk && S_OK == l_svExtents.GetExtentProperty( SVExtentPropertyOutputWidth, l_dOutputWidth );
			l_bOk = l_bOk && S_OK == l_svExtents.GetExtentProperty( SVExtentPropertyOutputHeight, l_dOutputHeight );

			l_bOk = l_bOk && ( S_OK == interpolationMode.GetValue( l_lInterpolationMode ) );

			if ( !l_bOk )
			{
				if (nullptr != pErrorMessages)
				{
					SvStl::MessageContainer Msg( SVMSG_SVO_92_GENERAL_ERROR, SvOi::Tid_ErrorGettingInputs, SvStl::SourceFileParams(StdMessageParams), 0, GetUniqueObjectID() );
					pErrorMessages->push_back(Msg);
				}
			}
			else
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
						l_bOk = false;
						if (nullptr != pErrorMessages)
						{
							SvStl::MessageContainer Msg( SVMSG_SVO_92_GENERAL_ERROR, SvOi::Tid_RunImagePolarFailed, SvStl::SourceFileParams(StdMessageParams), 0, GetUniqueObjectID() );
							pErrorMessages->push_back(Msg);
						}
					}


					// Reset it back
					_controlfp( cw, MCW_EM );
				}
				catch( ... )
				{
					_clearfp();

					l_bOk = false;
					if (nullptr != pErrorMessages)
					{
						SvStl::MessageContainer Msg( SVMSG_SVO_92_GENERAL_ERROR, SvOi::Tid_UnknownException, SvStl::SourceFileParams(StdMessageParams), 0, GetUniqueObjectID() );
						pErrorMessages->push_back(Msg);
					}
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

////////////////////////////////////////////////////////////////////////////////
// is... Operator(s)
////////////////////////////////////////////////////////////////////////////////

void SVImagePolarTransformClass::SetCalculatedPrintableFlags()
{
	BOOL l_bSetValue;

	if ( ( S_OK == useFormulaInput.GetValue( l_bSetValue ) ) && l_bSetValue )
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

bool SVImagePolarTransformClass::ResetObject(SvStl::MessageContainerVector *pErrorMessages)
{
	SVPolarTransformationToolClass* pTool = dynamic_cast<SVPolarTransformationToolClass*> ( GetOwner() );
	ASSERT( pTool );

	HRESULT hr;
	bool bUseFormula = false;
	BOOL bOk = ( S_OK == useFormulaInput.GetValue( bUseFormula ) );	// one formula option for all values?
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

	bool Result = true;
	if (S_OK != outputImageObject.InitializeImage( getInputImage() ))
	{
		Result = false;
		if (nullptr != pErrorMessages)
		{
			SvStl::MessageContainer Msg( SVMSG_SVO_92_GENERAL_ERROR, SvOi::Tid_InitImageFailed, SvStl::SourceFileParams(StdMessageParams), 0, GetUniqueObjectID() );
			pErrorMessages->push_back(Msg);
		}
	}

	Result = __super::ResetObject(pErrorMessages) && Result;

	SetCalculatedPrintableFlags();

	CollectInputImageNames();

	return ValidateLocal(pErrorMessages) && Result;
}

// Set String value object for Source Image Names
HRESULT SVImagePolarTransformClass::CollectInputImageNames()
{
	HRESULT l_hr = S_FALSE;
	SVImageClass* l_pInputImage = getInputImage();
	SVPolarTransformationToolClass* l_pTool = dynamic_cast<SVPolarTransformationToolClass*>(GetTool());
	if( l_pInputImage && l_pTool )
	{
		SVString Name = l_pInputImage->GetCompleteName();

		l_pTool->m_svSourceImageNames.SetValue( 0, Name );

		l_hr = S_OK;
	}
	return l_hr;
}

bool SVImagePolarTransformClass::ValidateLocal(SvStl::MessageContainerVector *pErrorMessages) const
{
	if( nullptr == getInputImage() )
	{
		if (nullptr != pErrorMessages)
		{
			SvStl::MessageContainer Msg( SVMSG_SVO_92_GENERAL_ERROR, SvOi::Tid_NoSourceImage, SvStl::SourceFileParams(StdMessageParams), 0, GetUniqueObjectID() );
			pErrorMessages->push_back(Msg);
		}
		return false;
	}

	return true;
}
