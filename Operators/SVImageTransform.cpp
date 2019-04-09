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
#include "Tools/SVTool.h"
#include "SVUtilityLibrary/StringHelper.h"
#include "SVObjectLibrary/SVClsIds.h"
#pragma endregion Includes

namespace SvOp
{

#pragma region Declarations
#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif
#pragma endregion Declarations

SV_IMPLEMENT_CLASS( SVImageTransformClass, SVImageTransformClassGuid )

#pragma region Constructor
SVImageTransformClass::SVImageTransformClass( SVObjectClass* POwner, int StringResourceID )
	:SVTransformClass( POwner, StringResourceID ) 
{

	// Identify yourself
	m_outObjectInfo.m_ObjectTypeInfo.SubType = SvPb::SVImageTransformObjectType;

	// Identify our input type needs...
	// Image
	m_inputImageObjectInfo.SetInputObjectType(SvPb::SVImageObjectType, SvPb::SVImageMonoType);
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
	RegisterEmbeddedObject( &m_outputImage, SVOutputImageObjectGuid, IDS_OBJECTNAME_IMAGE1 );
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

	m_outputImage.InitializeImage( SvDef::SVImageTypeEnum::SVImageTypePhysical );

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
	bool result = SVTransformClass::CreateObject(rCreateStructure);

	SvTo::SVToolClass* pTool = dynamic_cast<SvTo::SVToolClass*>(GetTool());
	result = result && nullptr != pTool;
	if(result)
	{
		pTool->SetImageExtentProperty( SvPb::SVExtentPropertyWidth, &m_extentWidth );
		pTool->SetImageExtentProperty( SvPb::SVExtentPropertyHeight, &m_extentHeight );
		pTool->SetImageExtentProperty( SvPb::SVExtentPropertyTranslationOffsetX, &m_extentDisplacementX );
		pTool->SetImageExtentProperty( SvPb::SVExtentPropertyTranslationOffsetY, &m_extentDisplacementY );
		pTool->SetImageExtentProperty( SvPb::SVExtentPropertyPositionPointX, &m_extentSourceX );
		pTool->SetImageExtentProperty( SvPb::SVExtentPropertyPositionPointY, &m_extentSourceY );
		pTool->SetImageExtentProperty( SvPb::SVExtentPropertyRotationAngle, &m_extentRotationAngle );
	}

	result = result && S_OK == UpdateTransformData();

	m_isCreated = result;

	// Set / Reset Printable Flags
	m_useExtentsOnly.SetObjectAttributesAllowed( SvPb::printable, SvOi::SetAttributeType::AddAttribute );
	m_extentWidth.SetObjectAttributesAllowed( SvPb::printable, SvOi::SetAttributeType::RemoveAttribute );
	m_extentHeight.SetObjectAttributesAllowed( SvPb::printable, SvOi::SetAttributeType::RemoveAttribute );
	m_extentDisplacementX.SetObjectAttributesAllowed( SvPb::printable, SvOi::SetAttributeType::RemoveAttribute );
	m_extentDisplacementY.SetObjectAttributesAllowed( SvPb::printable, SvOi::SetAttributeType::RemoveAttribute );
	m_extentSourceX.SetObjectAttributesAllowed( SvPb::printable, SvOi::SetAttributeType::RemoveAttribute );
	m_extentSourceY.SetObjectAttributesAllowed( SvPb::printable, SvOi::SetAttributeType::RemoveAttribute );
	m_extentRotationAngle.SetObjectAttributesAllowed( SvPb::printable, SvOi::SetAttributeType::RemoveAttribute );

	return result;
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
		Result = Result && m_outputImage.ResetObject(pErrorMessages);
	}

	CollectInputImageNames();

	return Result;
}
#pragma endregion

SvIe::SVImageClass* SVImageTransformClass::getOutputImage()
{
	return &m_outputImage;
}
#pragma endregion

#pragma region Protected Methods
bool SVImageTransformClass::isInputImage(const SVGUID& rImageGuid) const
{
	bool Result(false);

	SvIe::SVImageClass* pImage = SvOl::getInput<SvIe::SVImageClass>(m_inputImageObjectInfo);
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

	SvIe::SVImageClass* pInputImage = SvOl::getInput<SvIe::SVImageClass>(m_inputImageObjectInfo, true);

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
		const SVImageExtentClass& rExtents = m_outputImage.GetImageExtents();

		if( bTranslationEnabled )
		{
			bRetVal = bRetVal && S_OK == rExtents.GetExtentProperty( SvPb::SVExtentPropertyTranslationOffsetX, xDisplacement );
			bRetVal = bRetVal && S_OK == rExtents.GetExtentProperty( SvPb::SVExtentPropertyTranslationOffsetY, yDisplacement );
		}

		if( bRotationEnabled )
		{
			bRetVal = bRetVal && S_OK == rExtents.GetExtentProperty( SvPb::SVExtentPropertyPositionPointX, srcX );
			bRetVal = bRetVal && S_OK == rExtents.GetExtentProperty( SvPb::SVExtentPropertyPositionPointY, srcY );
			bRetVal = bRetVal && S_OK == rExtents.GetExtentProperty( SvPb::SVExtentPropertyRotationAngle, angle );
		}

		bRetVal = bRetVal && S_OK == rExtents.GetExtentProperty( SvPb::SVExtentPropertyOutputPositionPointX, dstX );
		bRetVal = bRetVal && S_OK == rExtents.GetExtentProperty( SvPb::SVExtentPropertyOutputPositionPointY, dstY );

		if (!bRetVal && nullptr != pErrorMessages)
		{
			SvStl::MessageContainer Msg( SVMSG_SVO_92_GENERAL_ERROR, SvStl::Tid_ErrorGettingInputs, SvStl::SourceFileParams(StdMessageParams), 0, GetUniqueObjectID() );
			pErrorMessages->push_back(Msg);
		}
	}

	if( bRetVal )
	{
		SvTrc::IImagePtr pInputImageBuffer = pInputImage->getImageReadOnly(runStatus.m_triggerRecord.get());
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

			if( S_OK == InImageInfo.GetExtentProperty( SvPb::SVExtentPropertyPositionPoint, l_oPoint ) )
			{
				// Get Root Image from our Input Image
				auto pRootImage = pInputImage->GetRootImage();
				if( nullptr != pRootImage)
				{
					interpolationType = SVImageDefault; // M_DEFAULT;
				}
			}
		}

		SvTrc::IImagePtr pOutputImageBuffer = m_outputImage.getImageToWrite(runStatus.m_triggerRecord);
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
	}

	return bRetVal;
}

HRESULT SVImageTransformClass::UpdateTransformData( )
{
	HRESULT l_hrOk = S_OK;

	SvIe::SVImageClass* pInputImage = SvOl::getInput<SvIe::SVImageClass>(m_inputImageObjectInfo, true);
	SvTo::SVToolClass* pTool = dynamic_cast<SvTo::SVToolClass*>(GetTool());

	if( nullptr != pInputImage && nullptr != pTool )
	{
		POINT UseExtentsOnlyPoint;

		SvVol::SVDoubleValueObjectClass* pTranslationXResult = SvOl::getInput<SvVol::SVDoubleValueObjectClass>(m_inputTranslationXResult, true);
		SvVol::SVDoubleValueObjectClass* pTranslationYResult = SvOl::getInput<SvVol::SVDoubleValueObjectClass>(m_inputTranslationYResult, true);
		SvVol::SVDoubleValueObjectClass* pRotationXResult = SvOl::getInput<SvVol::SVDoubleValueObjectClass>(m_inputRotationXResult, true);
		SvVol::SVDoubleValueObjectClass* pRotationYResult = SvOl::getInput<SvVol::SVDoubleValueObjectClass>(m_inputRotationYResult, true);
		SvVol::SVDoubleValueObjectClass* pRotationAngleResult = SvOl::getInput<SvVol::SVDoubleValueObjectClass>(m_inputRotationAngleResult, true);

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

		SVImageExtentClass imageExtents = pInputImage->GetImageExtents();

		::KeepPrevError( l_hrOk, m_performTranslation.GetValue( bTranslationEnabled ) );
		::KeepPrevError( l_hrOk, m_performRotation.GetValue( bRotationEnabled ) );
		::KeepPrevError( l_hrOk, m_useExtentsOnly.GetValue( bUseExtentsOnly ) );

		::KeepPrevError( l_hrOk, imageExtents.GetExtentProperty( SvPb::SVExtentPropertyWidth, dWidth ) );
		::KeepPrevError( l_hrOk, imageExtents.GetExtentProperty( SvPb::SVExtentPropertyHeight, dHeight ) );

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
			::KeepPrevError( l_hrOk, imageExtents.GetExtentProperty( SvPb::SVExtentPropertyPositionPoint, UseExtentsOnlyPoint ) );
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

				imageExtents.SetTranslation(SvPb::SVExtentTranslationTransformRotate );
			}
			else
			{
				imageExtents.SetTranslation(SvPb::SVExtentTranslationTransformShift );
			}

			// Check if using Source Image for Extents only
			if( bUseExtentsOnly )
			{
				xDisplacement += UseExtentsOnlyPoint.x;
				yDisplacement += UseExtentsOnlyPoint.y;
			}

			::KeepPrevError( l_hrOk, imageExtents.SetExtentProperty( SvPb::SVExtentPropertyWidth, dWidth ) );
			::KeepPrevError( l_hrOk, imageExtents.SetExtentProperty( SvPb::SVExtentPropertyHeight, dHeight ) );
			::KeepPrevError( l_hrOk, imageExtents.SetExtentProperty( SvPb::SVExtentPropertyTranslationOffsetX, xDisplacement ) );
			::KeepPrevError( l_hrOk, imageExtents.SetExtentProperty( SvPb::SVExtentPropertyTranslationOffsetY, yDisplacement ) );
			::KeepPrevError( l_hrOk, imageExtents.SetExtentProperty( SvPb::SVExtentPropertyPositionPointX, srcX ) );
			::KeepPrevError( l_hrOk, imageExtents.SetExtentProperty( SvPb::SVExtentPropertyPositionPointY, srcY ) );
			::KeepPrevError( l_hrOk, imageExtents.SetExtentProperty( SvPb::SVExtentPropertyRotationAngle, angle ) );
		}

		::KeepPrevError( l_hrOk, pTool->SetImageExtent(imageExtents) );

		::KeepPrevError( l_hrOk, m_outputImage.InitializeImage(pInputImage) );

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
	SvIe::SVImageClass* pInputImage = SvOl::getInput<SvIe::SVImageClass>(m_inputImageObjectInfo);
	SvTo::SVToolClass* pTool = dynamic_cast<SvTo::SVToolClass*> (GetTool());
	if( nullptr != pInputImage && nullptr != pTool && nullptr != pTool->GetInputImageNames())
	{
		std::string Name = pInputImage->GetCompleteName();

		pTool->GetInputImageNames()->SetValue(Name);

		l_hr = S_OK;
	}
	return l_hr;
}
#pragma endregion

} //namespace SvOp
