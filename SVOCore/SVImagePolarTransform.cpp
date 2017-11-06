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
#include "Definitions/GlobalConst.h"
#include "SVImageProcessingClass.h"
#include "SVPolarTransformationTool.h"
#include "SVObjectLibrary/SVClsIds.h"
#pragma endregion Includes

double SVImagePolarTransformClass::g_dMaxAngularDistance  = 720.0;

SV_IMPLEMENT_CLASS( SVImagePolarTransformClass, SVImagePolarTransformClassGuid )

SVImagePolarTransformClass::SVImagePolarTransformClass( SVObjectClass* POwner, int StringResourceID )
						   :SVPolarTransformClass( POwner, StringResourceID ) 
{
	// Identify yourself
	m_outObjectInfo.m_ObjectTypeInfo.SubType = SVImagePolarTransformObjectType;

	// Identify our input type needs...
	// Image
	m_inputImageObjectInfo.SetInputObjectType( SVImageObjectType );
	m_inputImageObjectInfo.SetObject( GetObjectInfo() );
	RegisterInputObject( &m_inputImageObjectInfo, _T( "ImagePolarTransformImage" ) );

	// Register Embedded Objects
	RegisterEmbeddedObject( &m_centerX, SVOutputCenterXObjectGuid, IDS_OBJECTNAME_CENTER_X, false, SvOi::SVResetItemNone );
	RegisterEmbeddedObject( &m_centerY, SVOutputCenterYObjectGuid, IDS_OBJECTNAME_CENTER_Y, false, SvOi::SVResetItemNone );
	RegisterEmbeddedObject( &m_startRadius, SVOutputStartRadiusObjectGuid, IDS_OBJECTNAME_START_RADIUS, false, SvOi::SVResetItemNone );
	RegisterEmbeddedObject( &m_endRadius, SVOutputEndRadiusObjectGuid, IDS_OBJECTNAME_END_RADIUS, false, SvOi::SVResetItemNone );
	RegisterEmbeddedObject( &m_startAngle, SVOutputStartAngleObjectGuid, IDS_OBJECTNAME_START_ANGLE, false, SvOi::SVResetItemNone );
	RegisterEmbeddedObject( &m_endAngle, SVOutputEndAngleObjectGuid, IDS_OBJECTNAME_END_ANGLE, false, SvOi::SVResetItemNone );
	RegisterEmbeddedObject( &m_interpolationMode, SVOutputInterpolationModeObjectGuid, IDS_OBJECTNAME_INTERPOLATION_MODE, false, SvOi::SVResetItemNone );

	RegisterEmbeddedObject( &m_useFormulaInput, SVOutputUseFormulaObjectGuid, IDS_OBJECTNAME_USE_FORMULA, false, SvOi::SVResetItemTool );

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
	m_interpolationMode.SetEnumTypes( EnumTypes.c_str() );


	// Set Embedded defaults
	m_interpolationMode.SetDefaultValue( SVNearestNeighOverScanEnable );	// Refer to MIL...
	m_centerX.SetDefaultValue( ( double ) SvDef::cDefaultPolarTransformCenterX );
	m_centerY.SetDefaultValue( ( double ) SvDef::cDefaultPolarTransformCenterY );
	m_startRadius.SetDefaultValue( ( double ) SvDef::cDefaultPolarTransformStartRadius );
	m_endRadius.SetDefaultValue( ( double ) SvDef::cDefaultPolarTransformEndRadius );
	m_startAngle.SetDefaultValue( ( double ) SvDef::cDefaultPolarTransformStartAngle );
	m_endAngle.SetDefaultValue( ( double ) SvDef::cDefaultPolarTransformEndAngle );

	outputImageObject.InitializeImage( SvDef::SVImageTypeEnum::SVImageTypePhysical );

	m_useFormulaInput.SetDefaultValue( BOOL(false) );	// Default: Don't use formulas...


	// Add Default Inputs and Outputs
	addDefaultInputObjects();
}

SVImagePolarTransformClass::~SVImagePolarTransformClass()
{
	CloseObject();
}

bool SVImagePolarTransformClass::CreateObject( const SVObjectLevelCreateStruct& rCreateStructure )
{
	bool l_bOk = SVPolarTransformClass::CreateObject(rCreateStructure);
	SVToolClass* pTool = dynamic_cast<SVToolClass*>(GetTool());
	l_bOk = l_bOk && nullptr != pTool;

	l_bOk = l_bOk && S_OK == pTool->SetImageExtentProperty( SVExtentPropertyPositionPointX, &m_centerX );
	l_bOk = l_bOk && S_OK == pTool->SetImageExtentProperty( SVExtentPropertyPositionPointY, &m_centerY );
	l_bOk = l_bOk && S_OK == pTool->SetImageExtentProperty( SVExtentPropertyInnerRadius, &m_endRadius );
	l_bOk = l_bOk && S_OK == pTool->SetImageExtentProperty( SVExtentPropertyOuterRadius, &m_startRadius );
	l_bOk = l_bOk && S_OK == pTool->SetImageExtentProperty( SVExtentPropertyStartAngle, &m_startAngle );
	l_bOk = l_bOk && S_OK == pTool->SetImageExtentProperty( SVExtentPropertyEndAngle, &m_endAngle );

	l_bOk &= S_OK == ( outputImageObject.InitializeImage( getInputImage() ) );

	// Set / Reset Printable Flag
	const UINT cAttributes = SvDef::SV_PRINTABLE | SvDef::SV_REMOTELY_SETABLE | SvDef::SV_EXTENT_OBJECT | SvDef::SV_SETABLE_ONLINE;
	m_centerX.SetObjectAttributesAllowed( cAttributes, SvOi::SetAttributeType::AddAttribute );
	m_centerY.SetObjectAttributesAllowed( cAttributes, SvOi::SetAttributeType::AddAttribute );
	m_startRadius.SetObjectAttributesAllowed( cAttributes, SvOi::SetAttributeType::AddAttribute );
	m_endRadius.SetObjectAttributesAllowed( cAttributes, SvOi::SetAttributeType::AddAttribute );
	m_startAngle.SetObjectAttributesAllowed( cAttributes, SvOi::SetAttributeType::AddAttribute );
	m_endAngle.SetObjectAttributesAllowed( cAttributes, SvOi::SetAttributeType::AddAttribute );
	m_interpolationMode.SetObjectAttributesAllowed( SvDef::SV_PRINTABLE, SvOi::SetAttributeType::AddAttribute );
	m_useFormulaInput.SetObjectAttributesAllowed( SvDef::SV_PRINTABLE, SvOi::SetAttributeType::AddAttribute );

	SetCalculatedPrintableFlags();

	m_isCreated = l_bOk;

	return m_isCreated;
}

bool SVImagePolarTransformClass::isInputImage(const SVGUID& rImageGuid) const
{
	bool Result(false);

	SVImageClass* pImage = getInputImage();
	if ( nullptr != pImage && rImageGuid == pImage->GetUniqueObjectID())
	{
		Result = true;
	}

	return Result;
}

SVImageClass* SVImagePolarTransformClass::getInputImage() const
{
	if (m_inputImageObjectInfo.IsConnected() && nullptr != m_inputImageObjectInfo.GetInputObjectInfo().m_pObject)
	{
		//! Use static_cast to avoid time penalty in run mode for dynamic_cast
		//! We are sure that when m_pObject is not nullptr then it is a SVImageClass
		return static_cast<SVImageClass*> (m_inputImageObjectInfo.GetInputObjectInfo().m_pObject);
	}

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
		SVString Name = m_centerX.GetCompleteObjectNameToObjectType( nullptr, SVToolSetObjectType );
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
		SVString Name = m_centerY.GetCompleteObjectNameToObjectType( nullptr, SVToolSetObjectType );
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
		SVString Name = m_startRadius.GetCompleteObjectNameToObjectType( nullptr, SVToolSetObjectType );
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
		SVString Name = m_endRadius.GetCompleteObjectNameToObjectType( nullptr, SVToolSetObjectType );
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
		SVString Name = m_startAngle.GetCompleteObjectNameToObjectType( nullptr, SVToolSetObjectType );
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
		SVString Name = m_endAngle.GetCompleteObjectNameToObjectType( nullptr, SVToolSetObjectType );
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

bool SVImagePolarTransformClass::onRun( SVRunStatusClass& rRunStatus, SvStl::MessageContainerVector *pErrorMessages )
{
	BOOL bUseFormula( false );
	long AngularMethod = 0;

	bool l_bOk = __super::onRun( rRunStatus, pErrorMessages ) && ValidateLocal(pErrorMessages);

	if (S_OK != m_useFormulaInput.GetValue( bUseFormula ))
	{
		l_bOk = false;
		if (nullptr != pErrorMessages)
		{
			SvStl::MessageContainer Msg( SVMSG_SVO_92_GENERAL_ERROR, SvStl::Tid_ErrorGettingInputs, SvStl::SourceFileParams(StdMessageParams), 0, GetUniqueObjectID() );
			pErrorMessages->push_back(Msg);
		}
	}

	SVPolarTransformationToolClass* pTool = dynamic_cast<SVPolarTransformationToolClass*>( GetTool() );

	if( pTool )
	{
		if( S_OK != pTool->m_svAngularMethod.GetValue( AngularMethod ) )
		{
			l_bOk = false;
			if (nullptr != pErrorMessages)
			{
				SvStl::MessageContainer Msg( SVMSG_SVO_92_GENERAL_ERROR, SvStl::Tid_ErrorGettingInputs, SvStl::SourceFileParams(StdMessageParams), 0, GetUniqueObjectID() );
				pErrorMessages->push_back(Msg);
	}
		}
	}
	else
	{
		assert( false );
	}



	if ( l_bOk )
	{
		double dStartAngle = 0.0;
		double dEndAngle = 0.0;
		double dCenterX = 0.0;
		double dCenterY = 0.0;
		double dStartRadius = 0.0;
		double dEndRadius = 0.0;
		
		if ( bUseFormula )
		{
			l_bOk = l_bOk && nullptr != getInputStartAngleResult() && S_OK == ( getInputStartAngleResult()->GetValue( dStartAngle ) );
			l_bOk = l_bOk && nullptr != getInputEndAngleResult() && S_OK == ( getInputEndAngleResult()->GetValue( dEndAngle ) );
			l_bOk = l_bOk && nullptr != getInputCenterXResult() && S_OK == ( getInputCenterXResult()->GetValue( dCenterX ) );
			l_bOk = l_bOk && nullptr != getInputCenterYResult() && S_OK == ( getInputCenterYResult()->GetValue( dCenterY ) );
			l_bOk = l_bOk && nullptr != getInputStartRadiusResult() && ( S_OK == getInputStartRadiusResult()->GetValue( dStartRadius ) );
			l_bOk = l_bOk && nullptr != getInputEndRadiusResult() && ( S_OK == getInputEndRadiusResult()->GetValue( dEndRadius ) );

			if (!l_bOk && nullptr != pErrorMessages)
			{
				SvStl::MessageContainer Msg( SVMSG_SVO_92_GENERAL_ERROR, SvStl::Tid_Error_NoResultObject, SvStl::SourceFileParams(StdMessageParams), 0, GetUniqueObjectID() );
				pErrorMessages->push_back(Msg);
			}

			if ( l_bOk )
			{
				l_bOk = l_bOk && ( S_OK == m_centerX.SetValue(dCenterX) );
				l_bOk = l_bOk && ( S_OK == m_centerY.SetValue(dCenterY) );
				l_bOk = l_bOk && ( S_OK == m_startRadius.SetValue(dStartRadius) );
				l_bOk = l_bOk && ( S_OK == m_endRadius.SetValue(dEndRadius) );
				if (!l_bOk && nullptr != pErrorMessages)
				{
					SvStl::MessageContainer Msg( SVMSG_SVO_92_GENERAL_ERROR, SvStl::Tid_SetValueFailed, SvStl::SourceFileParams(StdMessageParams), 0, GetUniqueObjectID() );
					pErrorMessages->push_back(Msg);
				}
			}
		}
		else
		{
			l_bOk = l_bOk && ( S_OK == m_startAngle.GetValue( dStartAngle ) );
			l_bOk = l_bOk && ( S_OK == m_endAngle.GetValue( dEndAngle ) );
			if (!l_bOk && nullptr != pErrorMessages)
			{
				SvStl::MessageContainer Msg( SVMSG_SVO_92_GENERAL_ERROR, SvStl::Tid_Error_NoResultObject, SvStl::SourceFileParams(StdMessageParams), 0, GetUniqueObjectID() );
				pErrorMessages->push_back(Msg);
		}
		}

		// Correct start and end angle...
		// ]-360.0...360.0[
		// start angle must always be lower than end angle, so that
		// MIL works all the time counter clockwise...
		if( 1 == AngularMethod  )
		{
			NewCorrectAngles( dStartAngle, dEndAngle );
		}
		else
		{
			correctAngles(  dStartAngle, dEndAngle );
		}


		// Write back modified angles...
		if ( S_OK != m_startAngle.SetValue(dStartAngle) || S_OK != m_endAngle.SetValue(dEndAngle) )
		{
			l_bOk = false;
			if (nullptr != pErrorMessages)
			{
				SvStl::MessageContainer Msg( SVMSG_SVO_92_GENERAL_ERROR, SvStl::Tid_SetValueFailed, SvStl::SourceFileParams(StdMessageParams), 0, GetUniqueObjectID() );
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

		l_bOk = l_bOk && outputImageObject.SetImageHandleIndex( rRunStatus.Images );

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
			SvStl::MessageContainer Msg( SVMSG_SVO_92_GENERAL_ERROR, SvStl::Tid_RunImagePolarFailed, SvStl::SourceFileParams(StdMessageParams), 0, GetUniqueObjectID() );
			pErrorMessages->push_back(Msg);
		}

		if ( l_bOk )
		{
			long l_lInterpolationMode;

			double dCenterX = 0.0;
			double dCenterY = 0.0;
			double dStartRadius = 0.0;
			double dEndRadius = 0.0;
			double dStartAngle = 0.0;
			double dEndAngle = 0.0;
			double dOutputWidth = 0.0;
			double dOutputHeight = 0.0;

			l_bOk = l_bOk && S_OK == l_svExtents.GetExtentProperty( SVExtentPropertyPositionPointX, dCenterX );
			l_bOk = l_bOk && S_OK == l_svExtents.GetExtentProperty( SVExtentPropertyPositionPointY, dCenterY );
			l_bOk = l_bOk && S_OK == l_svExtents.GetExtentProperty( SVExtentPropertyInnerRadius, dStartRadius );
			l_bOk = l_bOk && S_OK == l_svExtents.GetExtentProperty( SVExtentPropertyOuterRadius, dEndRadius );
			l_bOk = l_bOk && S_OK == l_svExtents.GetExtentProperty( SVExtentPropertyStartAngle, dStartAngle );
			l_bOk = l_bOk && S_OK == l_svExtents.GetExtentProperty( SVExtentPropertyEndAngle, dEndAngle );
			l_bOk = l_bOk && S_OK == l_svExtents.GetExtentProperty( SVExtentPropertyOutputWidth, dOutputWidth );
			l_bOk = l_bOk && S_OK == l_svExtents.GetExtentProperty( SVExtentPropertyOutputHeight, dOutputHeight );

			l_bOk = l_bOk && ( S_OK == m_interpolationMode.GetValue( l_lInterpolationMode ) );

			if ( !l_bOk )
			{
				if (nullptr != pErrorMessages)
				{
					SvStl::MessageContainer Msg( SVMSG_SVO_92_GENERAL_ERROR, SvStl::Tid_ErrorGettingInputs, SvStl::SourceFileParams(StdMessageParams), 0, GetUniqueObjectID() );
					pErrorMessages->push_back(Msg);
				}
			}
			else
			{
				try
				{
					
					HRESULT MatroxCode;

					// Get the default control word.   
					int cw = _controlfp( 0,0 );

					// This one turns off exception generation
					int newCw = cw | (EM_OVERFLOW|EM_UNDERFLOW|EM_INEXACT|EM_ZERODIVIDE|EM_DENORMAL);

					// Set the control word.   
					_controlfp( newCw, MCW_EM );
					double l_dTmpStartAngle = dStartAngle;
					double l_dTmpEndAngle =  dEndAngle;

					double l_dAngularDistance = dEndAngle - dStartAngle;
					if( l_dAngularDistance > 720.0 )
					{
						l_dAngularDistance = 720.0 ;
						dEndAngle = dStartAngle + 720.0;
					}

					// Do transform...
					AnglesTo360(l_dTmpStartAngle, l_dTmpEndAngle);


					SVMatroxPolarTransformStruct l_Polar(l_InMilHandle.GetBuffer());
					l_Polar.m_dCenterX = dCenterX;
					l_Polar.m_dCenterY = dCenterY;
					l_Polar.m_dStartRadius = dStartRadius;
					l_Polar.m_dEndRadius = dEndRadius;
					l_Polar.m_dStartAngle = l_dTmpStartAngle;
					l_Polar.m_dEndAngle = l_dTmpEndAngle;
					l_Polar.m_eInterpMode = static_cast<SVImageOperationTypeEnum>(l_lInterpolationMode);
					l_Polar.m_eOpMode = SVRectToPolar;

					MatroxCode = SVMatroxImageInterface::PolarTransform(l_OutMilHandle.GetBuffer(), l_Polar );



					if( l_dAngularDistance > 360 )
					{
						//  Small Part 
						double l_dPartialWidth = dOutputWidth - floor( l_Polar.m_dDestSizeX ); 

						if( l_dPartialWidth >= 1.0 )
						{
							MatroxCode = SVMatroxBufferInterface::CopyBuffer( l_OutMilHandle.GetBuffer(), l_OutMilHandle.GetBuffer(), (long) l_Polar.m_dDestSizeX,  0L);
						}
					}


					if (S_OK != MatroxCode)
					{
						l_bOk = false;
						if (nullptr != pErrorMessages)
						{
							SvStl::MessageContainer Msg( SVMSG_SVO_92_GENERAL_ERROR, SvStl::Tid_RunImagePolarFailed, SvStl::SourceFileParams(StdMessageParams), 0, GetUniqueObjectID() );
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
						SvStl::MessageContainer Msg( SVMSG_SVO_92_GENERAL_ERROR, SvStl::Tid_UnknownException, SvStl::SourceFileParams(StdMessageParams), 0, GetUniqueObjectID() );
						pErrorMessages->push_back(Msg);
				}
				}

				if( l_bOk )
				{
					rRunStatus.SetPassed();
				}
			}
		}
	}

	if ( ! l_bOk )
	{
		SetInvalid();
		rRunStatus.SetInvalid();
	}

	return l_bOk;
}

////////////////////////////////////////////////////////////////////////////////
// is... Operator(s)
////////////////////////////////////////////////////////////////////////////////

void SVImagePolarTransformClass::SetCalculatedPrintableFlags()
{
	const UINT cAttributes = SvDef::SV_PRINTABLE | SvDef::SV_REMOTELY_SETABLE | SvDef::SV_SETABLE_ONLINE;
	BOOL bSetValue( false );

	if ( ( S_OK == m_useFormulaInput.GetValue( bSetValue ) ) && bSetValue )
	{
		//turn off the print flags
		//turn off setable remotely/online
		m_centerX.SetObjectAttributesAllowed( cAttributes, SvOi::SetAttributeType::RemoveAttribute );
		m_centerY.SetObjectAttributesAllowed( cAttributes, SvOi::SetAttributeType::RemoveAttribute );
		m_startRadius.SetObjectAttributesAllowed( cAttributes, SvOi::SetAttributeType::RemoveAttribute );
		m_endRadius.SetObjectAttributesAllowed( cAttributes, SvOi::SetAttributeType::RemoveAttribute );
		m_startAngle.SetObjectAttributesAllowed( cAttributes, SvOi::SetAttributeType::RemoveAttribute );
		m_endAngle.SetObjectAttributesAllowed( cAttributes, SvOi::SetAttributeType::RemoveAttribute );
	}
	else
	{
		//turn print flag ons
		//turn on setable remotely/online flags
		m_centerX.SetObjectAttributesAllowed( cAttributes, SvOi::SetAttributeType::AddAttribute );
		m_centerY.SetObjectAttributesAllowed( cAttributes, SvOi::SetAttributeType::AddAttribute );
		m_startRadius.SetObjectAttributesAllowed( cAttributes, SvOi::SetAttributeType::AddAttribute );
		m_endRadius.SetObjectAttributesAllowed( cAttributes, SvOi::SetAttributeType::AddAttribute );
		m_startAngle.SetObjectAttributesAllowed( cAttributes, SvOi::SetAttributeType::AddAttribute );
		m_endAngle.SetObjectAttributesAllowed( cAttributes, SvOi::SetAttributeType::AddAttribute );
	}

}

bool SVImagePolarTransformClass::ResetObject(SvStl::MessageContainerVector *pErrorMessages)
{
	SVPolarTransformationToolClass* pTool = dynamic_cast<SVPolarTransformationToolClass*> ( GetOwner() );

	HRESULT hr;
	BOOL Value( false );
	bool Result = ( S_OK == m_useFormulaInput.GetValue( Value ) );	// one formula option for all values?
	bool bUseFormula = Value ? true : false;
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

	if (S_OK != outputImageObject.InitializeImage( getInputImage() ))
	{
		Result = false;
		if (nullptr != pErrorMessages)
		{
			SvStl::MessageContainer Msg( SVMSG_SVO_92_GENERAL_ERROR, SvStl::Tid_InitImageFailed, SvStl::SourceFileParams(StdMessageParams), 0, GetUniqueObjectID() );
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
	SVPolarTransformationToolClass* pTool = dynamic_cast<SVPolarTransformationToolClass*>(GetTool());
	if( l_pInputImage && pTool )
	{
		SVString Name = l_pInputImage->GetCompleteName();

		pTool->m_SourceImageNames.SetValue(Name);

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
			SvStl::MessageContainer Msg( SVMSG_SVO_92_GENERAL_ERROR, SvStl::Tid_NoSourceImage, SvStl::SourceFileParams(StdMessageParams), 0, GetUniqueObjectID() );
			pErrorMessages->push_back(Msg);
		}
		return false;
	}

	return true;
}
