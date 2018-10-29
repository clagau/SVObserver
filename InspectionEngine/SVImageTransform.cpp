//******************************************************************************
//* COPYRIGHT (c) 2003 by SVResearch, Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVImageTransform
//* .File Name       : $Workfile:   SVImageTransform.cpp  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.4  $
//* .Check In Date   : $Date:   15 May 2014 12:44:08  $
//******************************************************************************

#pragma region Includes
#include "stdafx.h"
#include "SVImageTransform.h"
#include "SVImageLibrary/SVImageBufferHandleImage.h"
#include "SVMatroxLibrary/SVMatroxLibrary.h"
#include "SVObjectLibrary/SVObjectLevelCreateStruct.h"
#include "Definitions/GlobalConst.h"
#include "SVTool.h"
#include "SVTransformationTool.h"
#include "SVUtilityLibrary/StringHelper.h"
#include "SVObjectLibrary/SVClsIds.h"
#pragma endregion Includes

SV_IMPLEMENT_CLASS( SVImageTransformClass, SVImageTransformClassGuid )

#pragma region Constructor
SVImageTransformClass::SVImageTransformClass( SVObjectClass* POwner, int StringResourceID )
	:SVTransformClass( POwner, StringResourceID ) 
{

	// Identify yourself
	m_outObjectInfo.m_ObjectTypeInfo.SubType = SvDef::SVImageTransformObjectType;

	// Identify our input type needs...
	// Image
	m_inputImageObjectInfo.SetInputObjectType(SvDef::SVImageObjectType, SvDef::SVImageMonoType);
	m_inputImageObjectInfo.SetObject( GetObjectInfo() );
	RegisterInputObject( &m_inputImageObjectInfo, _T("ImageTransformImage") );

	//Special type names for extents
	m_extentWidth.SetTypeName( _T("Extent Width") );
	m_extentHeight.SetTypeName( _T("Extent Height") );
	m_extentDisplacementX.SetTypeName( _T("Extent X") );
	m_extentDisplacementX.setSaveValueFlag(false);
	m_extentDisplacementY.SetTypeName( _T("Extent Y") );
	m_extentDisplacementY.setSaveValueFlag(false);
	m_extentSourceX.SetTypeName( _T("Extent X") );
	m_extentSourceY.SetTypeName( _T("Extent Y") );
	m_extentRotationAngle.SetTypeName( _T("Extent Angle") );
	// Register Embedded Objects
	RegisterEmbeddedObject( &m_outputImageObject, SVOutputImageObjectGuid, IDS_OBJECTNAME_IMAGE1 );
	RegisterEmbeddedObject( &m_useExtentsOnly, SVUseExtentsOnlyObjectGuid, IDS_OBJECTNAME_USE_EXTENTS_ONLY, false, SvOi::SVResetItemNone );
	RegisterEmbeddedObject( &m_extentWidth, SVExtentWidthObjectGuid, IDS_OBJECTNAME_EXTENT_WIDTH, false, SvOi::SVResetItemNone );
	RegisterEmbeddedObject( &m_extentHeight, SVExtentHeightObjectGuid, IDS_OBJECTNAME_EXTENT_HEIGHT, false, SvOi::SVResetItemNone );
	RegisterEmbeddedObject( &m_extentDisplacementX, SVImageTransformDisplacementXGuid, IDS_TRANSFORM_DISPLACEMENTX, false, SvOi::SVResetItemNone );
	RegisterEmbeddedObject( &m_extentDisplacementY, SVImageTransformDisplacementYGuid, IDS_TRANSFORM_DISPLACEMENTY, false, SvOi::SVResetItemNone );
	RegisterEmbeddedObject( &m_extentSourceX, SVImageTransformSourceXGuid, IDS_TRANSFORM_SOURCE_X, false, SvOi::SVResetItemNone );
	RegisterEmbeddedObject( &m_extentSourceY, SVImageTransformSourceYGuid, IDS_TRANSFORM_SOURCE_Y, false, SvOi::SVResetItemNone );
	RegisterEmbeddedObject( &m_extentRotationAngle, SVRotationAngleObjectGuid, IDS_OBJECTNAME_ROTATION_ANGLE, false, SvOi::SVResetItemNone );

	// Interpolation mode object
	// Set Default Interpolation Mode to use Nearest Neighbor
	std::string Mode;
	std::string Text;
	std::string EnumTypes;
	// M_NEAREST_NEIGHBOR 
	Mode = SvUl::LoadStdString( IDS_NEAREST_NEIGHBOR_STRING );
	Text = SvUl::Format( _T("%s=%d,"), Mode.c_str(), SVNearestNeighOverScanClear); // M_NEAREST_NEIGHBOR);
	EnumTypes += Text;
	// M_BILINEAR
	Mode = SvUl::LoadStdString( IDS_BILINEAR_STRING );
	Text = SvUl::Format( _T("%s=%d,"), Mode.c_str(), SVBilinear);		// M_BILINEAR );
	EnumTypes += Text;
	// M_BICUBIC
	Mode = SvUl::LoadStdString( IDS_BICUBIC_STRING );
	Text = SvUl::Format( _T("%s=%d,"), Mode.c_str(), SVBiCubic);			// M_BICUBIC );
	EnumTypes += Text;
	// And now set enum types...
	m_interpolationMode.SetEnumTypes( EnumTypes.c_str() );
	m_interpolationMode.SetDefaultValue( SVNearestNeighOverScanClear );	// Refer to MIL...
	RegisterEmbeddedObject( &m_interpolationMode, SVOutputInterpolationModeObjectGuid, IDS_OBJECTNAME_INTERPOLATION_MODE, false, SvOi::SVResetItemNone );

	// Set Embedded defaults
	m_useExtentsOnly.SetDefaultValue( BOOL(false) );

	m_extentWidth.SetDefaultValue( SvDef::cDefaultWindowToolWidth );
	m_extentHeight.SetDefaultValue( SvDef::cDefaultWindowToolHeight );

	m_outputImageObject.InitializeImage( SvDef::SVImageTypeEnum::SVImageTypePhysical );

	// Add Default Inputs and Outputs
	addDefaultInputObjects();
}

SVImageTransformClass::~SVImageTransformClass()
{
	CloseObject();
}
#pragma endregion

#pragma region Public Methods
#pragma region virtual
bool SVImageTransformClass::CreateObject( const SVObjectLevelCreateStruct& rCreateStructure )
{
	bool l_bOk = SVTransformClass::CreateObject(rCreateStructure);

	SVToolClass* pTool = dynamic_cast<SVToolClass*>(GetTool());
	l_bOk = l_bOk && nullptr != pTool;

	l_bOk = l_bOk && S_OK == pTool->SetImageExtentProperty( SvDef::SVExtentPropertyWidth, &m_extentWidth );
	l_bOk = l_bOk && S_OK == pTool->SetImageExtentProperty( SvDef::SVExtentPropertyHeight, &m_extentHeight );
	l_bOk = l_bOk && S_OK == pTool->SetImageExtentProperty( SvDef::SVExtentPropertyTranslationOffsetX, &m_extentDisplacementX );
	l_bOk = l_bOk && S_OK == pTool->SetImageExtentProperty( SvDef::SVExtentPropertyTranslationOffsetY, &m_extentDisplacementY );
	l_bOk = l_bOk && S_OK == pTool->SetImageExtentProperty( SvDef::SVExtentPropertyPositionPointX, &m_extentSourceX );
	l_bOk = l_bOk && S_OK == pTool->SetImageExtentProperty( SvDef::SVExtentPropertyPositionPointY, &m_extentSourceY );
	l_bOk = l_bOk && S_OK == pTool->SetImageExtentProperty( SvDef::SVExtentPropertyRotationAngle, &m_extentRotationAngle );

	l_bOk = l_bOk && S_OK == UpdateTransformData();

	m_isCreated = l_bOk;

	// Set / Reset Printable Flags
	m_useExtentsOnly.SetObjectAttributesAllowed( SvDef::SV_PRINTABLE, SvOi::SetAttributeType::AddAttribute );
	m_extentWidth.SetObjectAttributesAllowed( SvDef::SV_PRINTABLE, SvOi::SetAttributeType::RemoveAttribute );
	m_extentHeight.SetObjectAttributesAllowed( SvDef::SV_PRINTABLE, SvOi::SetAttributeType::RemoveAttribute );
	m_extentDisplacementX.SetObjectAttributesAllowed( SvDef::SV_PRINTABLE, SvOi::SetAttributeType::RemoveAttribute );
	m_extentDisplacementY.SetObjectAttributesAllowed( SvDef::SV_PRINTABLE, SvOi::SetAttributeType::RemoveAttribute );
	m_extentSourceX.SetObjectAttributesAllowed( SvDef::SV_PRINTABLE, SvOi::SetAttributeType::RemoveAttribute );
	m_extentSourceY.SetObjectAttributesAllowed( SvDef::SV_PRINTABLE, SvOi::SetAttributeType::RemoveAttribute );
	m_extentRotationAngle.SetObjectAttributesAllowed( SvDef::SV_PRINTABLE, SvOi::SetAttributeType::RemoveAttribute );

	return l_bOk;
}

bool SVImageTransformClass::ResetObject( SvStl::MessageContainerVector *pErrorMessages )
{
	bool Result = __super::ResetObject(pErrorMessages);
	
	SvOl::ValidateInput(m_inputImageObjectInfo);

	if (S_OK != UpdateTransformData())
	{
		Result = false;
		if (nullptr != pErrorMessages)
		{
			SvStl::MessageContainer Msg( SVMSG_SVO_92_GENERAL_ERROR, SvStl::Tid_UpdateTransformDataFailed, SvStl::SourceFileParams(StdMessageParams), 0, GetUniqueObjectID() );
			pErrorMessages->push_back(Msg);
		}
	}
	else
	{
		Result = Result && m_outputImageObject.ResetObject(pErrorMessages);
	}

	CollectInputImageNames();

	return Result;
}
#pragma endregion

SVImageClass* SVImageTransformClass::getOutputImage()
{
	return &m_outputImageObject;
}
#pragma endregion

#pragma region Protected Methods
bool SVImageTransformClass::isInputImage(const SVGUID& rImageGuid) const
{
	bool Result(false);

	SVImageClass* pImage = SvOl::getInput<SVImageClass>(m_inputImageObjectInfo);
	if (nullptr != pImage && rImageGuid == pImage->GetUniqueObjectID())
	{
		Result = true;
	}

	return Result;
}

bool SVImageTransformClass::onRun( SVRunStatusClass& runStatus, SvStl::MessageContainerVector *pErrorMessages )
{
	bool bRetVal = SVTransformClass::onRun( runStatus, pErrorMessages );

	BOOL bUseExtentsOnly( false );
	BOOL bTranslationEnabled( false );
	BOOL bRotationEnabled( false );

	double xDisplacement = 0.0;
	double yDisplacement = 0.0;
	double srcX = 0.0;
	double srcY = 0.0;
	double angle = 0.0;
	double dstX = 0.0;
	double dstY = 0.0;

	SVImageClass* pInputImage = SvOl::getInput<SVImageClass>(m_inputImageObjectInfo, true);

	if ( nullptr == pInputImage )
	{
		bRetVal = false;
		if (nullptr != pErrorMessages)
		{
			SvStl::MessageContainer Msg( SVMSG_SVO_92_GENERAL_ERROR, SvStl::Tid_ErrorGettingInputs, SvStl::SourceFileParams(StdMessageParams), 0, GetUniqueObjectID() );
			pErrorMessages->push_back(Msg);
		}
	}

	if (bRetVal && S_OK != UpdateTransformData())
	{
		bRetVal = false;
		if (nullptr != pErrorMessages)
		{
			SvStl::MessageContainer Msg( SVMSG_SVO_92_GENERAL_ERROR, SvStl::Tid_UpdateTranslationFailed, SvStl::SourceFileParams(StdMessageParams), 0, GetUniqueObjectID() );
			pErrorMessages->push_back(Msg);
		}
	}

	if (bRetVal && 
		(S_OK != m_useExtentsOnly.GetValue( bUseExtentsOnly ) || S_OK != m_performTranslation.GetValue( bTranslationEnabled ) || S_OK != m_performRotation.GetValue( bRotationEnabled )))
	{
		bRetVal = false;
		if (nullptr != pErrorMessages)
		{
			SvStl::MessageContainer Msg( SVMSG_SVO_92_GENERAL_ERROR, SvStl::Tid_ErrorGettingInputs, SvStl::SourceFileParams(StdMessageParams), 0, GetUniqueObjectID() );
			pErrorMessages->push_back(Msg);
		}
	}

	if( bRetVal )
	{
		SVImageExtentClass l_svExtents = m_outputImageObject.GetImageExtents();

		if( bTranslationEnabled )
		{
			bRetVal = bRetVal && S_OK == l_svExtents.GetExtentProperty( SvDef::SVExtentPropertyTranslationOffsetX, xDisplacement );
			bRetVal = bRetVal && S_OK == l_svExtents.GetExtentProperty( SvDef::SVExtentPropertyTranslationOffsetY, yDisplacement );
		}

		if( bRotationEnabled )
		{
			bRetVal = bRetVal && S_OK == l_svExtents.GetExtentProperty( SvDef::SVExtentPropertyPositionPointX, srcX );
			bRetVal = bRetVal && S_OK == l_svExtents.GetExtentProperty( SvDef::SVExtentPropertyPositionPointY, srcY );
			bRetVal = bRetVal && S_OK == l_svExtents.GetExtentProperty( SvDef::SVExtentPropertyRotationAngle, angle );
		}

		bRetVal = bRetVal && S_OK == l_svExtents.GetExtentProperty( SvDef::SVExtentPropertyOutputPositionPointX, dstX );
		bRetVal = bRetVal && S_OK == l_svExtents.GetExtentProperty( SvDef::SVExtentPropertyOutputPositionPointY, dstY );

		if (!bRetVal && nullptr != pErrorMessages)
		{
			SvStl::MessageContainer Msg( SVMSG_SVO_92_GENERAL_ERROR, SvStl::Tid_ErrorGettingInputs, SvStl::SourceFileParams(StdMessageParams), 0, GetUniqueObjectID() );
			pErrorMessages->push_back(Msg);
	}
	}

	if( bRetVal )
	{
		SvTrc::IImagePtr pInputImageBuffer = pInputImage->getImageReadOnly(runStatus.m_triggerRecord);
		if ( nullptr == pInputImageBuffer || pInputImageBuffer->isEmpty())
		{
			bRetVal = false;
			if (nullptr != pErrorMessages)
			{
				SvStl::MessageContainer Msg( SVMSG_SVO_92_GENERAL_ERROR, SvStl::Tid_ErrorGettingInputs, SvStl::SourceFileParams(StdMessageParams), 0, GetUniqueObjectID() );
				pErrorMessages->push_back(Msg);
			}
		}

		long InterpolationType = 0;
		m_interpolationMode.GetValue(InterpolationType);
		SVImageOperationTypeEnum interpolationType = static_cast<SVImageOperationTypeEnum>(InterpolationType);

		// Check if using Source Image for Extents only
		if( bUseExtentsOnly )
		{
			POINT l_oPoint;
			SVImageInfoClass InImageInfo = pInputImage->GetImageInfo();

			if( S_OK == InImageInfo.GetExtentProperty( SvDef::SVExtentPropertyPositionPoint, l_oPoint ) )
			{
				// Get Root Image from our Input Image
				auto pRootImage = pInputImage->GetRootImage();
				if( nullptr != pRootImage)
				{
					interpolationType = SVImageDefault; // M_DEFAULT;
				}
			}
		}

		SvTrc::IImagePtr pOutputImageBuffer = m_outputImageObject.getImageToWrite(runStatus.m_triggerRecord);
		if (bRetVal && (nullptr == pOutputImageBuffer || pOutputImageBuffer->isEmpty()))
		{
			bRetVal = false;
			if (nullptr != pErrorMessages)
			{
				SvStl::MessageContainer Msg( SVMSG_SVO_92_GENERAL_ERROR, SvStl::Tid_ErrorGettingInputs, SvStl::SourceFileParams(StdMessageParams), 0, GetUniqueObjectID() );
				pErrorMessages->push_back(Msg);
			}
		}

		if( bRetVal && nullptr != pOutputImageBuffer->getHandle()->GetBufferAddress() )
		{
			SVMatroxBufferInterface::ClearBuffer(pOutputImageBuffer->getHandle()->GetBuffer(), 0);
		}

		if( bRetVal )
		{
			SVMatroxImageRotateStruct l_Rotate(pInputImageBuffer->getHandle()->GetBuffer());
			l_Rotate.m_dAngle = angle;
			l_Rotate.m_dSrcCenX = srcX + xDisplacement;
			l_Rotate.m_dSrcCenY = srcY + yDisplacement;
			l_Rotate.m_dDstCenX = dstX;
			l_Rotate.m_dDstCenY = dstY;
			l_Rotate.m_eInterpolation = interpolationType;

			// Use MimRotate to Rotate, Translate or Copy buffer
			// Rotate...( and translate image )
			HRESULT MatroxCode = SVMatroxImageInterface::Rotate(pOutputImageBuffer->getHandle()->GetBuffer(), l_Rotate );

			if (S_OK != MatroxCode)
			{
				bRetVal = false;
				if (nullptr != pErrorMessages)
				{
					SvStl::MessageContainer Msg( SVMSG_SVO_92_GENERAL_ERROR, SvStl::Tid_RunRotateFailed, SvStl::SourceFileParams(StdMessageParams), 0, GetUniqueObjectID() );
					pErrorMessages->push_back(Msg);
				}
			}
		}
	}

	if( bRetVal )
	{
		runStatus.SetPassed();
	}
	else
	{
		runStatus.SetFailed();
		runStatus.SetInvalid();

		SetInvalid();
	}

	return bRetVal;
}

HRESULT SVImageTransformClass::UpdateTransformData( )
{
	HRESULT l_hrOk = S_OK;

	SVImageClass* pInputImage = SvOl::getInput<SVImageClass>(m_inputImageObjectInfo, true);
	SVToolClass* pTool = dynamic_cast<SVToolClass*>(GetTool());

	if( nullptr != pInputImage && nullptr != pTool )
	{
		POINT UseExtentsOnlyPoint;

		SVImageExtentClass Extents = pInputImage->GetImageExtents();

		SVDoubleValueObjectClass* pTranslationXResult = SvOl::getInput<SVDoubleValueObjectClass>(m_inputTranslationXResult, true);
		SVDoubleValueObjectClass* pTranslationYResult = SvOl::getInput<SVDoubleValueObjectClass>(m_inputTranslationYResult, true);
		SVDoubleValueObjectClass* pRotationXResult = SvOl::getInput<SVDoubleValueObjectClass>(m_inputRotationXResult, true);
		SVDoubleValueObjectClass* pRotationYResult = SvOl::getInput<SVDoubleValueObjectClass>(m_inputRotationYResult, true);
		SVDoubleValueObjectClass* pRotationAngleResult = SvOl::getInput<SVDoubleValueObjectClass>(m_inputRotationAngleResult, true);

		BOOL bTranslationEnabled( false );
		BOOL bRotationEnabled( false );
		BOOL bUseExtentsOnly( false );

		double dTranslationXResult = 0.0;
		double dTranslationYResult = 0.0;

		double dRotationXResult = 0.0;
		double dRotationYResult = 0.0;
		double dRotationAngleResult = 0.0;

		double dLearnedTranslationXValue = 0.0;
		double dLearnedTranslationYValue = 0.0;
		double dLearnedRotationXValue = 0.0;
		double dLearnedRotationYValue = 0.0;
		double dLearnedRotationAngleValue = 0.0;

		double dWidth = 0.0;
		double dHeight = 0.0;

		::KeepPrevError( l_hrOk, m_performTranslation.GetValue( bTranslationEnabled ) );
		::KeepPrevError( l_hrOk, m_performRotation.GetValue( bRotationEnabled ) );
		::KeepPrevError( l_hrOk, m_useExtentsOnly.GetValue( bUseExtentsOnly ) );

		::KeepPrevError( l_hrOk, Extents.GetExtentProperty( SvDef::SVExtentPropertyWidth, dWidth ) );
		::KeepPrevError( l_hrOk, Extents.GetExtentProperty( SvDef::SVExtentPropertyHeight, dHeight ) );

		bool l_bAlwaysUpdate = false;

		if( S_OK == l_hrOk && bTranslationEnabled && nullptr != pTranslationXResult && nullptr != pTranslationYResult)
		{
			l_bAlwaysUpdate = true;

			::KeepPrevError( l_hrOk, pTranslationXResult->GetValue( dTranslationXResult ) );
			::KeepPrevError( l_hrOk, pTranslationYResult->GetValue( dTranslationYResult ) );

			::KeepPrevError( l_hrOk, m_learnedTranslationX.GetValue( dLearnedTranslationXValue ) );
			::KeepPrevError( l_hrOk, m_learnedTranslationY.GetValue( dLearnedTranslationYValue ) );
		}

		if( S_OK == l_hrOk && bRotationEnabled && nullptr != pRotationXResult && nullptr != pRotationYResult && nullptr != pRotationAngleResult)
		{
			l_bAlwaysUpdate = true;

			::KeepPrevError( l_hrOk, pRotationXResult->GetValue( dRotationXResult ) );
			::KeepPrevError( l_hrOk, pRotationYResult->GetValue( dRotationYResult ) );
			::KeepPrevError( l_hrOk, pRotationAngleResult->GetValue( dRotationAngleResult ) );

			::KeepPrevError( l_hrOk, m_learnedRotationX.GetValue( dLearnedRotationXValue ) );
			::KeepPrevError( l_hrOk, m_learnedRotationY.GetValue( dLearnedRotationYValue ) );
			::KeepPrevError( l_hrOk, m_learnedRotationAngle.GetValue( dLearnedRotationAngleValue ) );
		}

		pTool->SetAlwaysUpdate( l_bAlwaysUpdate );

		if( S_OK == l_hrOk && bUseExtentsOnly )
		{
			::KeepPrevError( l_hrOk, Extents.GetExtentProperty( SvDef::SVExtentPropertyPositionPoint, UseExtentsOnlyPoint ) );
		}

		if( S_OK == l_hrOk )
		{
			double xDisplacement = 0.0;
			double yDisplacement = 0.0;

			double srcX = 0.0;
			double srcY = 0.0;
			double angle = 0.0;

			if( bTranslationEnabled )
			{
				xDisplacement = ( dTranslationXResult - dLearnedTranslationXValue );
				yDisplacement = ( dTranslationYResult - dLearnedTranslationYValue );
			}

			if( bRotationEnabled )
			{
				srcX = ( dRotationXResult - dLearnedRotationXValue );
				srcY = ( dRotationYResult - dLearnedRotationYValue );
				angle = ( dRotationAngleResult - dLearnedRotationAngleValue );

				l_hrOk = Extents.SetTranslation( SvDef::SVExtentTranslationTransformRotate );
			}
			else
			{
				l_hrOk = Extents.SetTranslation( SvDef::SVExtentTranslationTransformShift );
			}

			// Check if using Source Image for Extents only
			if( bUseExtentsOnly )
			{
				xDisplacement += UseExtentsOnlyPoint.x;
				yDisplacement += UseExtentsOnlyPoint.y;
			}

			::KeepPrevError( l_hrOk, Extents.SetExtentProperty( SvDef::SVExtentPropertyWidth, dWidth ) );
			::KeepPrevError( l_hrOk, Extents.SetExtentProperty( SvDef::SVExtentPropertyHeight, dHeight ) );
			::KeepPrevError( l_hrOk, Extents.SetExtentProperty( SvDef::SVExtentPropertyTranslationOffsetX, xDisplacement ) );
			::KeepPrevError( l_hrOk, Extents.SetExtentProperty( SvDef::SVExtentPropertyTranslationOffsetY, yDisplacement ) );
			::KeepPrevError( l_hrOk, Extents.SetExtentProperty( SvDef::SVExtentPropertyPositionPointX, srcX ) );
			::KeepPrevError( l_hrOk, Extents.SetExtentProperty( SvDef::SVExtentPropertyPositionPointY, srcY ) );
			::KeepPrevError( l_hrOk, Extents.SetExtentProperty( SvDef::SVExtentPropertyRotationAngle, angle ) );
		}

		::KeepPrevError( l_hrOk, pTool->SetImageExtent( Extents ) );

		::KeepPrevError( l_hrOk, m_outputImageObject.InitializeImage(pInputImage) );

		// Return code for UpdateImageWithExtend not being checked because it may not be valid the first time.
		pTool->UpdateImageWithExtent(); 
	}
	else
	{
		l_hrOk = E_FAIL;
	}

	return l_hrOk;
}
#pragma endregion

#pragma region Private Methods
// Set String value object for Source Image Names
HRESULT SVImageTransformClass::CollectInputImageNames()
{
	HRESULT l_hr = S_FALSE;
	SVImageClass* pInputImage = SvOl::getInput<SVImageClass>(m_inputImageObjectInfo);
	SVTransformationToolClass* pTool = dynamic_cast<SVTransformationToolClass*>(GetTool());
	if( nullptr != pInputImage && nullptr != pTool )
	{
		std::string Name = pInputImage->GetCompleteName();

		pTool->GetInputImageNames()->SetValue(Name);

		l_hr = S_OK;
	}
	return l_hr;
}
#pragma endregion

