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

#include "SVObjectLibrary/SVClsIds.h"
#include "SVImageLibrary/SVImageBufferHandleImage.h"
#include "SVEvaluateCenterXClass.h"
#include "SVEvaluateCenterYClass.h"
#include "SVEvaluateStartRadiusClass.h"
#include "SVEvaluateEndRadiusClass.h"
#include "SVEvaluateStartAngleClass.h"
#include "SVEvaluateEndAngleClass.h"
#include "SVEquation.h"
#include "Definitions/GlobalConst.h"
#include "InspectionEngine/SVImageProcessingClass.h"
#include "Tools/SVTool.h"
#include "Tools/SVPolarTransformationTool.h"
#include "SVUtilityLibrary/StringHelper.h"
#pragma endregion Includes

namespace SvOp
{

#pragma region Declarations
#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

double SVImagePolarTransformClass::g_dMaxAngularDistance  = 720.0;
#pragma endregion Declarations


SV_IMPLEMENT_CLASS( SVImagePolarTransformClass, SVImagePolarTransformClassGuid )

SVImagePolarTransformClass::SVImagePolarTransformClass( SVObjectClass* POwner, int StringResourceID )
						   :SVPolarTransformClass( POwner, StringResourceID ) 
{
	// Identify yourself
	m_outObjectInfo.m_ObjectTypeInfo.SubType = SvPb::SVImagePolarTransformObjectType;

	// Identify our input type needs...
	// Image
	m_inputImageObjectInfo.SetInputObjectType(SvPb::SVImageObjectType, SvPb::SVImageMonoType);
	m_inputImageObjectInfo.SetObject( GetObjectInfo() );
	RegisterInputObject( &m_inputImageObjectInfo, _T( "ImagePolarTransformImage" ) );

	// Register Embedded Objects
	RegisterEmbeddedObject( &m_centerX, SVOutputCenterXObjectGuid, IDS_OBJECTNAME_CENTER_X, false, SvOi::SVResetItemTool);
	RegisterEmbeddedObject( &m_centerY, SVOutputCenterYObjectGuid, IDS_OBJECTNAME_CENTER_Y, false, SvOi::SVResetItemTool);
	RegisterEmbeddedObject( &m_startRadius, SVOutputStartRadiusObjectGuid, IDS_OBJECTNAME_START_RADIUS, false, SvOi::SVResetItemTool);
	RegisterEmbeddedObject( &m_endRadius, SVOutputEndRadiusObjectGuid, IDS_OBJECTNAME_END_RADIUS, false, SvOi::SVResetItemTool);
	RegisterEmbeddedObject( &m_startAngle, SVOutputStartAngleObjectGuid, IDS_OBJECTNAME_START_ANGLE, false, SvOi::SVResetItemTool );
	RegisterEmbeddedObject( &m_endAngle, SVOutputEndAngleObjectGuid, IDS_OBJECTNAME_END_ANGLE, false, SvOi::SVResetItemTool);
	RegisterEmbeddedObject( &m_interpolationMode, SVOutputInterpolationModeObjectGuid, IDS_OBJECTNAME_INTERPOLATION_MODE, false, SvOi::SVResetItemNone );

	RegisterEmbeddedObject( &m_useFormulaInput, SVOutputUseFormulaObjectGuid, IDS_OBJECTNAME_USE_FORMULA, false, SvOi::SVResetItemTool );

	RegisterEmbeddedObject( &m_outputImage, SVOutputImageObjectGuid, IDS_OBJECTNAME_IMAGE1 );
	
	// Prepare known interpolation types...
	std::string Enable = SvUl::LoadStdString( IDS_ENABLE_STRING );
	std::string Disable = SvUl::LoadStdString( IDS_DISABLE_STRING );
	std::string Overscan = SvUl::LoadStdString( IDS_OVERSCAN_STRING );
	std::string OverscanClear = SvUl::LoadStdString( IDS_OVERSCAN_CLEAR_STRING );

	std::string Mode;
	std::string Text;
	std::string EnumTypes;

	// M_NEAREST_NEIGHBOR + M_OVERSCAN_ENABLE
	Mode = SvUl::LoadStdString( IDS_NEAREST_NEIGHBOR_STRING );
	Text = SvUl::Format( _T("%s+%s %s=%d,"), Mode.c_str(), Overscan.c_str(), Enable.c_str(), SVNearestNeighOverScanEnable ); // M_NEAREST_NEIGHBOR + M_OVERSCAN_ENABLE );
	EnumTypes += Text;
	// M_NEAREST_NEIGHBOR + M_OVERSCAN_DISABLE
	Text = SvUl::Format( _T("%s+%s %s=%d,"), Mode.c_str(), Overscan.c_str(), Disable.c_str(), SVNearestNeighOverScanDisable ); // M_NEAREST_NEIGHBOR + M_OVERSCAN_DISABLE );
	EnumTypes += Text;
	// M_NEAREST_NEIGHBOR + M_OVERSCAN_CLEAR
	Text = SvUl::Format( _T("%s+%s=%d,"), Mode.c_str(), OverscanClear.c_str(), SVNearestNeighOverScanClear ); // M_NEAREST_NEIGHBOR + M_OVERSCAN_CLEAR );
	EnumTypes += Text;

	// M_BILINEAR + M_OVERSCAN_ENABLE
	Mode = SvUl::LoadStdString( IDS_BILINEAR_STRING );
	Text = SvUl::Format( _T("%s+%s %s=%d,"), Mode.c_str(), Overscan.c_str(), Enable.c_str(), SVBilinearOverScanEnable ); // M_BILINEAR + M_OVERSCAN_ENABLE );
	EnumTypes += Text;
	// M_BILINEAR + M_OVERSCAN_DISABLE
	Text = SvUl::Format( _T("%s+%s %s=%d,"), Mode.c_str(), Overscan.c_str(), Disable.c_str(), SVBilinearOverScanClear ); // M_BILINEAR + M_OVERSCAN_DISABLE );
	EnumTypes += Text;
	// M_BILINEAR + M_OVERSCAN_CLEAR
	Text = SvUl::Format( _T("%s+%s=%d,"), Mode.c_str(), OverscanClear.c_str(), SVBilinearOverScanClear ); // M_BILINEAR + M_OVERSCAN_CLEAR );
	EnumTypes += Text;

	// M_BICUBIC + M_OVERSCAN_ENABLE
	Mode = SvUl::LoadStdString( IDS_BICUBIC_STRING );
	Text = SvUl::Format( _T("%s+%s %s=%d,"), Mode.c_str(), Overscan.c_str(), Enable.c_str(), SVBiCubicOverScanEnable ); // M_BICUBIC + M_OVERSCAN_ENABLE );
	EnumTypes += Text;
	// M_BICUBIC + M_OVERSCAN_DISABLE
	Text = SvUl::Format( _T("%s+%s %s=%d,"), Mode.c_str(), Overscan.c_str(), Disable.c_str(), SVBiCubicOverScanDisable ); // M_BICUBIC + M_OVERSCAN_DISABLE );
	EnumTypes += Text;
	// M_BICUBIC + M_OVERSCAN_CLEAR
	Text = SvUl::Format( _T("%s+%s=%d,"), Mode.c_str(), OverscanClear.c_str(), SVBiCubicOverScanClear ); // M_BICUBIC + M_OVERSCAN_CLEAR );
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

	m_outputImage.InitializeImage( SvDef::SVImageTypeEnum::SVImageTypePhysical );

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
	bool result = SVPolarTransformClass::CreateObject(rCreateStructure);
	SvTo::SVToolClass* pTool = dynamic_cast<SvTo::SVToolClass*> (GetTool());
	result = result && nullptr != pTool;

	if(result)
	{
		pTool->SetImageExtentProperty( SvDef::SVExtentPropertyPositionPointX, &m_centerX );
		pTool->SetImageExtentProperty( SvDef::SVExtentPropertyPositionPointY, &m_centerY );
		pTool->SetImageExtentProperty( SvDef::SVExtentPropertyInnerRadius, &m_endRadius );
		pTool->SetImageExtentProperty( SvDef::SVExtentPropertyOuterRadius, &m_startRadius );
		pTool->SetImageExtentProperty( SvDef::SVExtentPropertyStartAngle, &m_startAngle );
		pTool->SetImageExtentProperty( SvDef::SVExtentPropertyEndAngle, &m_endAngle );
	}

	result &= S_OK == ( m_outputImage.InitializeImage(SvOl::getInput<SvIe::SVImageClass>(m_inputImageObjectInfo)));

	// Set / Reset Printable Flag
	const UINT cAttributes = SvPb::printable | SvPb::remotelySetable | SvPb::extentObject | SvPb::setableOnline;
	m_centerX.SetObjectAttributesAllowed( cAttributes, SvOi::SetAttributeType::AddAttribute );
	m_centerY.SetObjectAttributesAllowed( cAttributes, SvOi::SetAttributeType::AddAttribute );
	m_startRadius.SetObjectAttributesAllowed( cAttributes, SvOi::SetAttributeType::AddAttribute );
	m_endRadius.SetObjectAttributesAllowed( cAttributes, SvOi::SetAttributeType::AddAttribute );
	m_startAngle.SetObjectAttributesAllowed( cAttributes, SvOi::SetAttributeType::AddAttribute );
	m_endAngle.SetObjectAttributesAllowed( cAttributes, SvOi::SetAttributeType::AddAttribute );
	m_interpolationMode.SetObjectAttributesAllowed( SvPb::printable, SvOi::SetAttributeType::AddAttribute );
	m_useFormulaInput.SetObjectAttributesAllowed( SvPb::printable, SvOi::SetAttributeType::AddAttribute );

	SetCalculatedPrintableFlags();

	m_isCreated = result;

	return m_isCreated;
}

bool SVImagePolarTransformClass::isInputImage(const SVGUID& rImageGuid) const
{
	bool Result(false);

	SvIe::SVImageClass* pImage = SvOl::getInput<SvIe::SVImageClass>(m_inputImageObjectInfo);
	if ( nullptr != pImage && rImageGuid == pImage->GetUniqueObjectID())
	{
		Result = true;
	}

	return Result;
}

SvIe::SVImageClass* SVImagePolarTransformClass::GetOutputImage()
{
	return &m_outputImage;
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
	SvDef::SVObjectTypeInfoStruct objectInfo;
	SvDef::SVObjectTypeInfoStruct equationObjectInfo;
	equationObjectInfo.ObjectType = SvPb::SVEquationObjectType;
	equationObjectInfo.SubType    = SvPb::SVMathEquationObjectType;
	
	// Find the evaluation center x object...
	bOk = false;
	objectInfo.SubType = SvPb::SVEvaluateCenterXObjectType;
	SVEvaluateCenterXClass* pEvaluateCenterX = dynamic_cast<SVEvaluateCenterXClass*>( GetTool()->getFirstObject(objectInfo) );
	if( pEvaluateCenterX )
	{
		// Find equation object...
		SVEquationClass* pEquation = dynamic_cast<SVEquationClass*>(pEvaluateCenterX->getFirstObject(equationObjectInfo));
		std::string Name = m_centerX.GetObjectNameToObjectType();
		bOk = SetDefaultEquation( pEquation, Name, pErrorMessages );
	}
	if( ! bOk )
	{
		bRetVal = false;
	}

	// Find the evaluation center y object...
	bOk = false;
	objectInfo.SubType = SvPb::SVEvaluateCenterYObjectType;
	SVEvaluateCenterYClass* pEvaluateCenterY = dynamic_cast<SVEvaluateCenterYClass*>(GetTool()->getFirstObject(objectInfo));
	if( pEvaluateCenterY )
	{
		// Find equation object...
		SVEquationClass* pEquation = dynamic_cast<SVEquationClass*>(pEvaluateCenterY->getFirstObject(equationObjectInfo));
		std::string Name = m_centerY.GetObjectNameToObjectType();
		bOk = SetDefaultEquation( pEquation, Name, pErrorMessages );
	}
	if( ! bOk )
	{
		bRetVal = false;
	}

	// Find the evaluation start radius object...
	bOk = false;
	objectInfo.SubType = SvPb::SVEvaluateStartRadiusObjectType;
	SVEvaluateStartRadiusClass* pEvaluateStartRadius = dynamic_cast<SVEvaluateStartRadiusClass*>(GetTool()->getFirstObject(objectInfo));
	if( pEvaluateStartRadius )
	{
		// Find equation object...
		SVEquationClass* pEquation = dynamic_cast<SVEquationClass*>(pEvaluateStartRadius->getFirstObject(equationObjectInfo));
		std::string Name = m_startRadius.GetObjectNameToObjectType();
		bOk = SetDefaultEquation( pEquation, Name, pErrorMessages );
	}
	if( ! bOk )
	{
		bRetVal = false;
	}

	// Find the evaluation end radius object...
	bOk = false;
	objectInfo.SubType = SvPb::SVEvaluateEndRadiusObjectType;
	SVEvaluateEndRadiusClass* pEvaluateEndRadius = dynamic_cast<SVEvaluateEndRadiusClass*>(GetTool()->getFirstObject(objectInfo));
	if( pEvaluateEndRadius )
	{
		// Find equation object...
		SVEquationClass* pEquation = dynamic_cast<SVEquationClass*>(pEvaluateEndRadius->getFirstObject(equationObjectInfo));
		std::string Name = m_endRadius.GetObjectNameToObjectType();
		bOk = SetDefaultEquation( pEquation, Name, pErrorMessages );
	}
	if( ! bOk )
	{
		bRetVal = false;
	}

	// Find the evaluation start angle object...
	bOk = false;
	objectInfo.SubType = SvPb::SVEvaluateStartAngleObjectType;
	SVEvaluateStartAngleClass* pEvaluateStartAngle = dynamic_cast<SVEvaluateStartAngleClass*>(GetTool()->getFirstObject(objectInfo));
	if( pEvaluateStartAngle )
	{
		// Find equation object...
		SVEquationClass* pEquation = dynamic_cast<SVEquationClass*>(pEvaluateStartAngle->getFirstObject(equationObjectInfo));
		std::string Name = m_startAngle.GetObjectNameToObjectType();
		bOk = SetDefaultEquation( pEquation, Name, pErrorMessages );
	}
	if( ! bOk )
	{
		bRetVal = false;
	}

	// Find the evaluation end angle object...
	bOk = false;
	objectInfo.SubType = SvPb::SVEvaluateEndAngleObjectType;
	SVEvaluateEndAngleClass* pEvaluateEndAngle = dynamic_cast<SVEvaluateEndAngleClass*>(GetTool()->getFirstObject(objectInfo));
	if( pEvaluateEndAngle )
	{
		// Find equation object...
		SVEquationClass* pEquation = dynamic_cast<SVEquationClass*>(pEvaluateEndAngle->getFirstObject(equationObjectInfo));
		std::string Name = m_endAngle.GetObjectNameToObjectType();
		bOk = SetDefaultEquation( pEquation, Name, pErrorMessages );
	}
	if( ! bOk )
	{
		bRetVal = false;
	}

	return bRetVal;
}

bool SVImagePolarTransformClass::SetDefaultEquation( SVEquationClass* pEquation, const std::string& rName, SvStl::MessageContainerVector *pErrorMessages )
{
	bool Result( false );

	if( nullptr != pEquation )
	{
		// Get current name of embedded CenterX...
		if( ! rName.empty() )
		{
			// Set equation...
			pEquation->SetEquationText( SvUl::Format( _T( "\"%s\"" ), rName.c_str() ) );
			// Update symbol table and test...
			if( ! ( Result = pEquation->Test(pErrorMessages).bPassed ) )
			{
				// something is wrong...
				pEquation->SetEquationText( std::string() );
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

	bool result = __super::onRun( rRunStatus, pErrorMessages ) && ValidateLocal(pErrorMessages);

	if (S_OK != m_useFormulaInput.GetValue( bUseFormula ))
	{
		result = false;
		if (nullptr != pErrorMessages)
		{
			SvStl::MessageContainer Msg( SVMSG_SVO_92_GENERAL_ERROR, SvStl::Tid_ErrorGettingInputs, SvStl::SourceFileParams(StdMessageParams), 0, GetUniqueObjectID() );
			pErrorMessages->push_back(Msg);
		}
	}

	SvTo::SVPolarTransformationToolClass* pTool = dynamic_cast<SvTo::SVPolarTransformationToolClass*>( GetTool() );

	if( pTool )
	{
		if( S_OK != pTool->m_svAngularMethod.GetValue( AngularMethod ) )
		{
			result = false;
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
		result = false;
	}



	if ( result )
	{
		double dStartAngle = 0.0;
		double dEndAngle = 0.0;
		double dCenterX = 0.0;
		double dCenterY = 0.0;
		double dStartRadius = 0.0;
		double dEndRadius = 0.0;
		
		if ( bUseFormula )
		{
			SvVol::SVDoubleValueObjectClass* pValue = SvOl::getInput<SvVol::SVDoubleValueObjectClass>(m_inputStartAngleResult, true);
			result = result && nullptr != pValue && S_OK == (pValue->GetValue(dStartAngle));
			pValue = SvOl::getInput<SvVol::SVDoubleValueObjectClass>(m_inputEndAngleResult, true);
			result = result && nullptr != pValue && S_OK == (pValue->GetValue(dEndAngle));
			pValue = SvOl::getInput<SvVol::SVDoubleValueObjectClass>(m_inputCenterXResult, true);
			result = result && nullptr != pValue && S_OK == (pValue->GetValue(dCenterX));
			pValue = SvOl::getInput<SvVol::SVDoubleValueObjectClass>(m_inputCenterYResult, true);
			result = result && nullptr != pValue && S_OK == (pValue->GetValue(dCenterY));
			pValue = SvOl::getInput<SvVol::SVDoubleValueObjectClass>(m_inputStartRadiusResult, true);
			result = result && nullptr != pValue && ( S_OK == pValue->GetValue(dStartRadius));
			pValue = SvOl::getInput<SvVol::SVDoubleValueObjectClass>(m_inputEndRadiusResult, true);
			result = result && nullptr != pValue && ( S_OK == pValue->GetValue(dEndRadius));

			if (!result && nullptr != pErrorMessages)
			{
				SvStl::MessageContainer Msg( SVMSG_SVO_92_GENERAL_ERROR, SvStl::Tid_Error_NoResultObject, SvStl::SourceFileParams(StdMessageParams), 0, GetUniqueObjectID() );
				pErrorMessages->push_back(Msg);
			}

			if ( result )
			{
				result = result && ( S_OK == m_centerX.SetValue(dCenterX) );
				result = result && ( S_OK == m_centerY.SetValue(dCenterY) );
				result = result && ( S_OK == m_startRadius.SetValue(dStartRadius) );
				result = result && ( S_OK == m_endRadius.SetValue(dEndRadius) );
				if (!result && nullptr != pErrorMessages)
				{
					SvStl::MessageContainer Msg( SVMSG_SVO_92_GENERAL_ERROR, SvStl::Tid_SetValueFailed, SvStl::SourceFileParams(StdMessageParams), 0, GetUniqueObjectID() );
					pErrorMessages->push_back(Msg);
				}
			}
		}
		else
		{
			result = result && ( S_OK == m_startAngle.GetValue( dStartAngle ) );
			result = result && ( S_OK == m_endAngle.GetValue( dEndAngle ) );
			if (!result && nullptr != pErrorMessages)
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
			result = false;
			if (nullptr != pErrorMessages)
			{
				SvStl::MessageContainer Msg( SVMSG_SVO_92_GENERAL_ERROR, SvStl::Tid_SetValueFailed, SvStl::SourceFileParams(StdMessageParams), 0, GetUniqueObjectID() );
				pErrorMessages->push_back(Msg);
			}
		}
	}

	if ( result )
	{
		//This update call is required for the image extents which may have changed above
		pTool->updateImageExtent();
		
		const SVImageExtentClass& rImageToolExtents = pTool->GetImageExtent();

		//added "true" to the call of UpdateImage - if the tool only changes position the output image does not need to be re-created
		result = result && S_OK == m_outputImage.UpdateImage(rImageToolExtents, true);

		SvTrc::IImagePtr pOutputImageBuffer = m_outputImage.getImageToWrite(rRunStatus.m_triggerRecord);
		result = result && nullptr != pOutputImageBuffer && !pOutputImageBuffer->isEmpty();

		const SVImageExtentClass& rExtents = m_outputImage.GetImageExtents();

		SvTrc::IImagePtr pInputImageBuffer = SvOl::getInput<SvIe::SVImageClass>(m_inputImageObjectInfo, true)->getImageReadOnly(rRunStatus.m_triggerRecord);
		result = result && nullptr != pInputImageBuffer && !pInputImageBuffer->isEmpty();

		if (!result && nullptr != pErrorMessages)
		{
			SvStl::MessageContainer Msg( SVMSG_SVO_92_GENERAL_ERROR, SvStl::Tid_RunImagePolarFailed, SvStl::SourceFileParams(StdMessageParams), 0, GetUniqueObjectID() );
			pErrorMessages->push_back(Msg);
		}

		if ( result )
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

			result = result && S_OK == rExtents.GetExtentProperty( SvDef::SVExtentPropertyPositionPointX, dCenterX );
			result = result && S_OK == rExtents.GetExtentProperty( SvDef::SVExtentPropertyPositionPointY, dCenterY );
			result = result && S_OK == rExtents.GetExtentProperty( SvDef::SVExtentPropertyInnerRadius, dStartRadius );
			result = result && S_OK == rExtents.GetExtentProperty( SvDef::SVExtentPropertyOuterRadius, dEndRadius );
			result = result && S_OK == rExtents.GetExtentProperty( SvDef::SVExtentPropertyStartAngle, dStartAngle );
			result = result && S_OK == rExtents.GetExtentProperty( SvDef::SVExtentPropertyEndAngle, dEndAngle );
			result = result && S_OK == rExtents.GetExtentProperty( SvDef::SVExtentPropertyOutputWidth, dOutputWidth );
			result = result && S_OK == rExtents.GetExtentProperty( SvDef::SVExtentPropertyOutputHeight, dOutputHeight );

			result = result && ( S_OK == m_interpolationMode.GetValue( l_lInterpolationMode ) );

			if ( !result )
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


					SVMatroxPolarTransformStruct l_Polar(pInputImageBuffer->getHandle()->GetBuffer());
					l_Polar.m_dCenterX = dCenterX;
					l_Polar.m_dCenterY = dCenterY;
					l_Polar.m_dStartRadius = dStartRadius;
					l_Polar.m_dEndRadius = dEndRadius;
					l_Polar.m_dStartAngle = l_dTmpStartAngle;
					l_Polar.m_dEndAngle = l_dTmpEndAngle;
					l_Polar.m_eInterpMode = static_cast<SVImageOperationTypeEnum>(l_lInterpolationMode);
					l_Polar.m_eOpMode = SVRectToPolar;

					MatroxCode = SVMatroxImageInterface::PolarTransform(pOutputImageBuffer->getHandle()->GetBuffer(), l_Polar );



					if( l_dAngularDistance > 360 )
					{
						//  Small Part 
						double l_dPartialWidth = dOutputWidth - floor( l_Polar.m_dDestSizeX ); 

						if( l_dPartialWidth >= 1.0 )
						{
							MatroxCode = SVMatroxBufferInterface::CopyBuffer(pOutputImageBuffer->getHandle()->GetBuffer(), pOutputImageBuffer->getHandle()->GetBuffer(), (long) l_Polar.m_dDestSizeX,  0L);
						}
					}


					if (S_OK != MatroxCode)
					{
						result = false;
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

					result = false;
					if (nullptr != pErrorMessages)
					{
						SvStl::MessageContainer Msg( SVMSG_SVO_92_GENERAL_ERROR, SvStl::Tid_UnknownException, SvStl::SourceFileParams(StdMessageParams), 0, GetUniqueObjectID() );
						pErrorMessages->push_back(Msg);
				}
				}

				if( result )
				{
					rRunStatus.SetPassed();
				}
			}
		}
	}

	if ( ! result )
	{
		SetInvalid();
		rRunStatus.SetInvalid();
	}

	return result;
}

////////////////////////////////////////////////////////////////////////////////
// is... Operator(s)
////////////////////////////////////////////////////////////////////////////////

void SVImagePolarTransformClass::SetCalculatedPrintableFlags()
{
	const UINT cAttributes = SvPb::printable | SvPb::remotelySetable | SvPb::setableOnline;
	BOOL bSetValue( false );
	m_useFormulaInput.GetValue(bSetValue);

	SvOi::SetAttributeType AllowedAttribute = bSetValue ? SvOi::SetAttributeType::RemoveAttribute : SvOi::SetAttributeType::AddAttribute;

	m_centerX.SetObjectAttributesAllowed(cAttributes, AllowedAttribute);
	m_centerY.SetObjectAttributesAllowed(cAttributes, AllowedAttribute);
	m_startRadius.SetObjectAttributesAllowed(cAttributes, AllowedAttribute);
	m_endRadius.SetObjectAttributesAllowed(cAttributes, AllowedAttribute);
	m_startAngle.SetObjectAttributesAllowed(cAttributes, AllowedAttribute);
	m_endAngle.SetObjectAttributesAllowed(cAttributes, AllowedAttribute);

}

bool SVImagePolarTransformClass::ResetObject(SvStl::MessageContainerVector *pErrorMessages)
{
	bool Result = __super::ResetObject(pErrorMessages);
	SvOl::ValidateInput(m_inputImageObjectInfo);

	SvTo::SVToolClass* pTool = dynamic_cast<SvTo::SVToolClass*> ( GetTool() );

	HRESULT hr;
	BOOL Value( false );
	Result = Result && ( S_OK == m_useFormulaInput.GetValue( Value ) );	// one formula option for all values?
	bool bUseFormula = Value ? true : false;
	SvIe::SVExtentPropertyInfoStruct info;

	hr = pTool->getToolExtent().GetExtentPropertyInfo( SvDef::SVExtentPropertyPositionPointX, info );
	info.bFormula = bUseFormula;
	hr = pTool->getToolExtent().SetExtentPropertyInfo( SvDef::SVExtentPropertyPositionPointX, info );

	hr = pTool->getToolExtent().GetExtentPropertyInfo( SvDef::SVExtentPropertyPositionPointY, info );
	info.bFormula = bUseFormula;
	hr = pTool->getToolExtent().SetExtentPropertyInfo( SvDef::SVExtentPropertyPositionPointY, info );

	hr = pTool->getToolExtent().GetExtentPropertyInfo( SvDef::SVExtentPropertyInnerRadius, info );
	info.bFormula = bUseFormula;
	hr = pTool->getToolExtent().SetExtentPropertyInfo( SvDef::SVExtentPropertyInnerRadius, info );

	hr = pTool->getToolExtent().GetExtentPropertyInfo( SvDef::SVExtentPropertyOuterRadius, info );
	info.bFormula = bUseFormula;
	hr = pTool->getToolExtent().SetExtentPropertyInfo( SvDef::SVExtentPropertyOuterRadius, info );

	hr = pTool->getToolExtent().GetExtentPropertyInfo( SvDef::SVExtentPropertyStartAngle, info );
	info.bFormula = bUseFormula;
	hr = pTool->getToolExtent().SetExtentPropertyInfo( SvDef::SVExtentPropertyStartAngle, info );

	hr = pTool->getToolExtent().GetExtentPropertyInfo( SvDef::SVExtentPropertyEndAngle, info );
	info.bFormula = bUseFormula;
	hr = pTool->getToolExtent().SetExtentPropertyInfo( SvDef::SVExtentPropertyEndAngle, info );

	hr = pTool->getToolExtent().GetExtentPropertyInfo( SvDef::SVExtentPropertyRotationAngle, info );
	info.bFormula = bUseFormula;
	hr = pTool->getToolExtent().SetExtentPropertyInfo( SvDef::SVExtentPropertyRotationAngle, info );

	if (S_OK != m_outputImage.InitializeImage(SvOl::getInput<SvIe::SVImageClass>(m_inputImageObjectInfo)))
	{
		Result = false;
		if (nullptr != pErrorMessages)
		{
			SvStl::MessageContainer Msg( SVMSG_SVO_92_GENERAL_ERROR, SvStl::Tid_InitImageFailed, SvStl::SourceFileParams(StdMessageParams), 0, GetUniqueObjectID() );
			pErrorMessages->push_back(Msg);
		}
	}

	SetCalculatedPrintableFlags();

	CollectInputImageNames();

	return ValidateLocal(pErrorMessages) && Result;
}

// Set String value object for Source Image Names
HRESULT SVImagePolarTransformClass::CollectInputImageNames()
{
	HRESULT l_hr = S_FALSE;
	SvIe::SVImageClass* pInputImage = SvOl::getInput<SvIe::SVImageClass>(m_inputImageObjectInfo);
	SvTo::SVToolClass* pTool = dynamic_cast<SvTo::SVToolClass*> (GetTool());
	if(nullptr != pInputImage && nullptr != pTool && nullptr != pTool->GetInputImageNames())
	{
		std::string Name = pInputImage->GetCompleteName();

		pTool->GetInputImageNames()->SetValue(Name);

		l_hr = S_OK;
	}
	return l_hr;
}

bool SVImagePolarTransformClass::ValidateLocal(SvStl::MessageContainerVector *pErrorMessages) const
{
	if(nullptr == SvOl::getInput<SvIe::SVImageClass>(m_inputImageObjectInfo))
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

} //namespace SvOp
