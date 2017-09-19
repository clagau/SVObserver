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
#include "SVOCore/SVImageTransform.h"
#include "SVImageLibrary/SVImageBufferHandleImage.h"
#include "SVMatroxLibrary/SVMatroxLibrary.h"
#include "SVObjectLibrary/SVObjectLevelCreateStruct.h"
#include "ObjectInterfaces/GlobalConst.h"
#include "SVTool.h"
#include "SVTransformationTool.h"
#include "SVUtilityLibrary/SVString.h"
#include "SVObjectLibrary/SVClsIds.h"
#pragma endregion Includes

SV_IMPLEMENT_CLASS( SVImageTransformClass, SVImageTransformClassGuid )

#pragma region Constructor
SVImageTransformClass::SVImageTransformClass( SVObjectClass* POwner, int StringResourceID )
	:SVTransformClass( POwner, StringResourceID ) 
{

	// Identify yourself
	m_outObjectInfo.m_ObjectTypeInfo.SubType = SVImageTransformObjectType;

	// Identify our input type needs...
	// Image
	m_inputImageObjectInfo.SetInputObjectType( SVImageObjectType );
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
	SVString Mode;
	SVString Text;
	SVString EnumTypes;
	// M_NEAREST_NEIGHBOR 
	Mode = SvUl_SF::LoadSVString( IDS_NEAREST_NEIGHBOR_STRING );
	Text = SvUl_SF::Format( _T("%s=%d,"), Mode.c_str(), SVNearestNeighOverScanClear); // M_NEAREST_NEIGHBOR);
	EnumTypes += Text;
	// M_BILINEAR
	Mode = SvUl_SF::LoadSVString( IDS_BILINEAR_STRING );
	Text = SvUl_SF::Format( _T("%s=%d,"), Mode.c_str(), SVBilinear);		// M_BILINEAR );
	EnumTypes += Text;
	// M_BICUBIC
	Mode = SvUl_SF::LoadSVString( IDS_BICUBIC_STRING );
	Text = SvUl_SF::Format( _T("%s=%d,"), Mode.c_str(), SVBiCubic);			// M_BICUBIC );
	EnumTypes += Text;
	// And now set enum types...
	m_interpolationMode.SetEnumTypes( EnumTypes.c_str() );
	m_interpolationMode.SetDefaultValue( SVNearestNeighOverScanClear );	// Refer to MIL...
	RegisterEmbeddedObject( &m_interpolationMode, SVOutputInterpolationModeObjectGuid, IDS_OBJECTNAME_INTERPOLATION_MODE, false, SvOi::SVResetItemNone );

	// Set Embedded defaults
	m_useExtentsOnly.SetDefaultValue( BOOL(false) );

	m_extentWidth.SetDefaultValue( SvOi::cDefaultWindowToolWidth );
	m_extentHeight.SetDefaultValue( SvOi::cDefaultWindowToolHeight );

	m_outputImageObject.InitializeImage( SvOi::SVImageTypeEnum::SVImageTypePhysical );

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

	l_bOk = l_bOk && S_OK == pTool->SetImageExtentProperty( SVExtentPropertyWidth, &m_extentWidth );
	l_bOk = l_bOk && S_OK == pTool->SetImageExtentProperty( SVExtentPropertyHeight, &m_extentHeight );
	l_bOk = l_bOk && S_OK == pTool->SetImageExtentProperty( SVExtentPropertyTranslationOffsetX, &m_extentDisplacementX );
	l_bOk = l_bOk && S_OK == pTool->SetImageExtentProperty( SVExtentPropertyTranslationOffsetY, &m_extentDisplacementY );
	l_bOk = l_bOk && S_OK == pTool->SetImageExtentProperty( SVExtentPropertyPositionPointX, &m_extentSourceX );
	l_bOk = l_bOk && S_OK == pTool->SetImageExtentProperty( SVExtentPropertyPositionPointY, &m_extentSourceY );
	l_bOk = l_bOk && S_OK == pTool->SetImageExtentProperty( SVExtentPropertyRotationAngle, &m_extentRotationAngle );

	l_bOk = l_bOk && S_OK == UpdateTransformData();

	m_isCreated = l_bOk;

	// Set / Reset Printable Flags
	m_useExtentsOnly.SetObjectAttributesAllowed( SvOi::SV_PRINTABLE, SvOi::SetAttributeType::AddAttribute );
	m_extentWidth.SetObjectAttributesAllowed( SvOi::SV_PRINTABLE, SvOi::SetAttributeType::RemoveAttribute );
	m_extentHeight.SetObjectAttributesAllowed( SvOi::SV_PRINTABLE, SvOi::SetAttributeType::RemoveAttribute );
	m_extentDisplacementX.SetObjectAttributesAllowed( SvOi::SV_PRINTABLE, SvOi::SetAttributeType::RemoveAttribute );
	m_extentDisplacementY.SetObjectAttributesAllowed( SvOi::SV_PRINTABLE, SvOi::SetAttributeType::RemoveAttribute );
	m_extentSourceX.SetObjectAttributesAllowed( SvOi::SV_PRINTABLE, SvOi::SetAttributeType::RemoveAttribute );
	m_extentSourceY.SetObjectAttributesAllowed( SvOi::SV_PRINTABLE, SvOi::SetAttributeType::RemoveAttribute );
	m_extentRotationAngle.SetObjectAttributesAllowed( SvOi::SV_PRINTABLE, SvOi::SetAttributeType::RemoveAttribute );

	return l_bOk;
}

bool SVImageTransformClass::ResetObject( SvStl::MessageContainerVector *pErrorMessages )
{
	bool Result = true;
	
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
		Result = m_outputImageObject.ResetObject(pErrorMessages);
	}

	Result = __super::ResetObject(pErrorMessages) && Result;

	CollectInputImageNames();

	return Result;
}
#pragma endregion

SVImageClass* SVImageTransformClass::getInputImage() const
{
	if (m_inputImageObjectInfo.IsConnected() && nullptr != m_inputImageObjectInfo.GetInputObjectInfo().m_pObject)
	{
		//! Use static_cast to avoid time penalty in run mode for dynamic_cast
		//! We are sure that when m_pObject is not nullptr then it is a SVImageClass
		return static_cast<SVImageClass*> (m_inputImageObjectInfo.GetInputObjectInfo().m_pObject);
	}

	return nullptr;
}

SVImageClass* SVImageTransformClass::getOutputImage()
{
	return &m_outputImageObject;
}
#pragma endregion

#pragma region Protected Methods
bool SVImageTransformClass::isInputImage(const SVGUID& rImageGuid) const
{
	bool Result(false);

	SVImageClass* pImage = getInputImage();
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
	double width = 0.0;
	double height = 0.0;

	SVImageClass* l_psvInputImage = getInputImage();

	if ( nullptr == l_psvInputImage )
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
		(S_OK != m_useExtentsOnly.GetValue( bUseExtentsOnly ) || S_OK != performTranslation.GetValue( bTranslationEnabled ) || S_OK != performRotation.GetValue( bRotationEnabled )))
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
			bRetVal = bRetVal && S_OK == l_svExtents.GetExtentProperty( SVExtentPropertyTranslationOffsetX, xDisplacement );
			bRetVal = bRetVal && S_OK == l_svExtents.GetExtentProperty( SVExtentPropertyTranslationOffsetY, yDisplacement );
		}

		if( bRotationEnabled )
		{
			bRetVal = bRetVal && S_OK == l_svExtents.GetExtentProperty( SVExtentPropertyPositionPointX, srcX );
			bRetVal = bRetVal && S_OK == l_svExtents.GetExtentProperty( SVExtentPropertyPositionPointY, srcY );
			bRetVal = bRetVal && S_OK == l_svExtents.GetExtentProperty( SVExtentPropertyRotationAngle, angle );
		}

		bRetVal = bRetVal && S_OK == l_svExtents.GetExtentProperty( SVExtentPropertyWidth, width );
		bRetVal = bRetVal && S_OK == l_svExtents.GetExtentProperty( SVExtentPropertyHeight, height );
		bRetVal = bRetVal && S_OK == l_svExtents.GetExtentProperty( SVExtentPropertyOutputPositionPointX, dstX );
		bRetVal = bRetVal && S_OK == l_svExtents.GetExtentProperty( SVExtentPropertyOutputPositionPointY, dstY );

		if (!bRetVal && nullptr != pErrorMessages)
		{
			SvStl::MessageContainer Msg( SVMSG_SVO_92_GENERAL_ERROR, SvStl::Tid_ErrorGettingInputs, SvStl::SourceFileParams(StdMessageParams), 0, GetUniqueObjectID() );
			pErrorMessages->push_back(Msg);
	}
	}

	if( bRetVal )
	{
		SVSmartHandlePointer InImageHandle;
		SVSmartHandlePointer OutImageHandle;

		if ( !l_psvInputImage->GetImageHandle( InImageHandle ) || InImageHandle.empty() )
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
			SVImageInfoClass InImageInfo = l_psvInputImage->GetImageInfo();

			if( S_OK == InImageInfo.GetExtentProperty( SVExtentPropertyPositionPoint, l_oPoint ) )
			{
				// Get Root Image from our Input Image
				SVImageClass* l_psvRootImage = dynamic_cast<SVImageClass*>( l_psvInputImage->GetRootImage() );
				if( nullptr != l_psvRootImage )
				{
					interpolationType = SVImageDefault; // M_DEFAULT;
				}
			}
		}

		if (bRetVal && (!m_outputImageObject.SetImageHandleIndex( runStatus.Images ) || !m_outputImageObject.GetImageHandle( OutImageHandle ) || OutImageHandle.empty() ))
		{
			bRetVal = false;
			if (nullptr != pErrorMessages)
			{
				SvStl::MessageContainer Msg( SVMSG_SVO_92_GENERAL_ERROR, SvStl::Tid_ErrorGettingInputs, SvStl::SourceFileParams(StdMessageParams), 0, GetUniqueObjectID() );
				pErrorMessages->push_back(Msg);
			}
		}

		if( bRetVal && nullptr != OutImageHandle->GetBufferAddress() )
		{
			memset( OutImageHandle->GetBufferAddress(), 0, static_cast<size_t>(width * height) );
		}

		SVImageBufferHandleImage l_InMilHandle;
		SVImageBufferHandleImage l_OutMilHandle;

		if( bRetVal && ( S_OK != InImageHandle->GetData( l_InMilHandle ) || l_InMilHandle.empty() || 
							S_OK != OutImageHandle->GetData( l_OutMilHandle ) || l_OutMilHandle.empty() ))
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
			SVMatroxImageRotateStruct l_Rotate(l_InMilHandle.GetBuffer());
			l_Rotate.m_dAngle = angle;
			l_Rotate.m_dSrcCenX = srcX + xDisplacement;
			l_Rotate.m_dSrcCenY = srcY + yDisplacement;
			l_Rotate.m_dDstCenX = dstX;
			l_Rotate.m_dDstCenY = dstY;
			l_Rotate.m_eInterpolation = interpolationType;

			// Use MimRotate to Rotate, Translate or Copy buffer
			// Rotate...( and translate image )
			HRESULT MatroxCode = SVMatroxImageInterface::Rotate( l_OutMilHandle.GetBuffer(), l_Rotate );

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

	SVImageClass* pInputImage = getInputImage();
	SVToolClass* pTool = dynamic_cast<SVToolClass*>(GetTool());

	if( nullptr != pInputImage && nullptr != pTool )
	{
		POINT UseExtentsOnlyPoint;

		SVImageExtentClass Extents = pInputImage->GetImageExtents();

		SVDoubleValueObjectClass* l_pTranslationXResult = getInputTranslationXResult();
		SVDoubleValueObjectClass* l_pTranslationYResult = getInputTranslationYResult();
		SVDoubleValueObjectClass* l_pRotationXResult = getInputRotationXResult();
		SVDoubleValueObjectClass* l_pRotationYResult = getInputRotationYResult();
		SVDoubleValueObjectClass* l_pRotationAngleResult = getInputRotationAngleResult();

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

		::KeepPrevError( l_hrOk, performTranslation.GetValue( bTranslationEnabled ) );
		::KeepPrevError( l_hrOk, performRotation.GetValue( bRotationEnabled ) );
		::KeepPrevError( l_hrOk, m_useExtentsOnly.GetValue( bUseExtentsOnly ) );

		::KeepPrevError( l_hrOk, Extents.GetExtentProperty( SVExtentPropertyWidth, dWidth ) );
		::KeepPrevError( l_hrOk, Extents.GetExtentProperty( SVExtentPropertyHeight, dHeight ) );

		bool l_bAlwaysUpdate = false;

		if( S_OK == l_hrOk && bTranslationEnabled )
		{
			l_bAlwaysUpdate = true;

			::KeepPrevError( l_hrOk, l_pTranslationXResult->GetValue( dTranslationXResult ) );
			::KeepPrevError( l_hrOk, l_pTranslationYResult->GetValue( dTranslationYResult ) );

			::KeepPrevError( l_hrOk, learnedTranslationX.GetValue( dLearnedTranslationXValue ) );
			::KeepPrevError( l_hrOk, learnedTranslationY.GetValue( dLearnedTranslationYValue ) );
		}

		if( S_OK == l_hrOk && bRotationEnabled )
		{
			l_bAlwaysUpdate = true;

			::KeepPrevError( l_hrOk, l_pRotationXResult->GetValue( dRotationXResult ) );
			::KeepPrevError( l_hrOk, l_pRotationYResult->GetValue( dRotationYResult ) );
			::KeepPrevError( l_hrOk, l_pRotationAngleResult->GetValue( dRotationAngleResult ) );

			::KeepPrevError( l_hrOk, learnedRotationX.GetValue( dLearnedRotationXValue ) );
			::KeepPrevError( l_hrOk, learnedRotationY.GetValue( dLearnedRotationYValue ) );
			::KeepPrevError( l_hrOk, learnedRotationAngle.GetValue( dLearnedRotationAngleValue ) );
		}

		pTool->SetAlwaysUpdate( l_bAlwaysUpdate );

		if( S_OK == l_hrOk && bUseExtentsOnly )
		{
			::KeepPrevError( l_hrOk, Extents.GetExtentProperty( SVExtentPropertyPositionPoint, UseExtentsOnlyPoint ) );
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

				l_hrOk = Extents.SetTranslation( SVExtentTranslationTransformRotate );
			}
			else
			{
				l_hrOk = Extents.SetTranslation( SVExtentTranslationTransformShift );
			}

			// Check if using Source Image for Extents only
			if( bUseExtentsOnly )
			{
				xDisplacement += UseExtentsOnlyPoint.x;
				yDisplacement += UseExtentsOnlyPoint.y;
			}

			::KeepPrevError( l_hrOk, Extents.SetExtentProperty( SVExtentPropertyWidth, dWidth ) );
			::KeepPrevError( l_hrOk, Extents.SetExtentProperty( SVExtentPropertyHeight, dHeight ) );
			::KeepPrevError( l_hrOk, Extents.SetExtentProperty( SVExtentPropertyTranslationOffsetX, xDisplacement ) );
			::KeepPrevError( l_hrOk, Extents.SetExtentProperty( SVExtentPropertyTranslationOffsetY, yDisplacement ) );
			::KeepPrevError( l_hrOk, Extents.SetExtentProperty( SVExtentPropertyPositionPointX, srcX ) );
			::KeepPrevError( l_hrOk, Extents.SetExtentProperty( SVExtentPropertyPositionPointY, srcY ) );
			::KeepPrevError( l_hrOk, Extents.SetExtentProperty( SVExtentPropertyRotationAngle, angle ) );
		}

		::KeepPrevError( l_hrOk, pTool->SetImageExtent( Extents ) );

		::KeepPrevError( l_hrOk, m_outputImageObject.InitializeImage( getInputImage() ) );

		// Return code for UpdateImageWithExtend not being checked because it may not be valid the first time.
		pTool->UpdateImageWithExtent();
	}
	else
	{
		l_hrOk = S_FALSE;
	}

	return l_hrOk;
}
#pragma endregion

#pragma region Private Methods
// Set String value object for Source Image Names
HRESULT SVImageTransformClass::CollectInputImageNames()
{
	HRESULT l_hr = S_FALSE;
	SVImageClass* pInputImage = getInputImage();
	SVTransformationToolClass* pTool = dynamic_cast<SVTransformationToolClass*>(GetTool());
	if( nullptr != pInputImage && nullptr != pTool )
	{
		SVString Name = pInputImage->GetCompleteName();

		pTool->GetInputImageNames()->SetValue(Name);

		l_hr = S_OK;
	}
	return l_hr;
}
#pragma endregion

