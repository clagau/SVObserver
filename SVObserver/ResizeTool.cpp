//*****************************************************************************
/// \copyright COPYRIGHT (c) 2015 by Seidenader Maschinenbau GmbH
/// All Rights Reserved 
/// \Author	Jim Brown
//*****************************************************************************

#pragma region Includes
#include "StdAfx.h"
#include "ResizeTool.h"
#include "SVImageLibrary/SVImageBufferHandleImage.h"
#include "ToolSizeAdjustTask.h"
#include "TextDefinesSvO.h"
#include "SVUtilityLibrary/SVString.h"
#pragma endregion Includes


SV_IMPLEMENT_CLASS( ResizeTool, SVResizeToolGuid );

const long ResizeTool::MinScaleFactorThreshold = 0; // Scale Factor may not 
													// be less than or equal 
													// to 0.
const long ResizeTool::MaxScaleFactor = 1000;       // Maximum allowed Scale Factor. 


ResizeTool::ResizeTool( BOOL bCreateDefaultTaskList, SVObjectClass* pOwner, int stringResourceID )
: SVToolClass( bCreateDefaultTaskList, pOwner, stringResourceID )
{
	LocalInitialize();
}

void ResizeTool::LocalInitialize()
{
	//  Start of initialize class members.
	InitializeInterpolationModeMember();
	InitializeOverscanMember();
	InitializePerformanceMember();
	//  End of initialize class members.

	BuildInputObjectList();

	// The output image is referenced in the embedded list.
	m_OutputImage.InitializeImage(SVImageTypePhysical);
	// This logical ROI image is also referenced in the embedded list, but 
	//  will be hidden from most exposure (within ResizeTool::Create).
	m_LogicalROIImage.InitializeImage(SVImageTypeLogical);
	
	BuildEmbeddedObjectList();

	ToolSizeAdjustTask::AddToFriendlist(this, true, true, true);

	// Set up your type... in this case this will reference that this tool is a 
	// Resize Tool.
	m_outObjectInfo.ObjectTypeInfo.ObjectType = SVToolObjectType;
	m_outObjectInfo.ObjectTypeInfo.SubType    = SVResizeToolObjectType;
}

HRESULT	ResizeTool::InitializeInterpolationModeMember()
{
	HRESULT	hr = S_OK;

	SVEnumerateVector interpolationModeOptions;

	// push_back(s) will throw exceptions if they fail to allocate.
	interpolationModeOptions.push_back( SVEnumeratePair(SvO::Auto, SVInterpolationModeOptions::InterpolationModeAuto ) );
	interpolationModeOptions.push_back( SVEnumeratePair(SvO::Bicubic, SVInterpolationModeOptions::InterpolationModeBicubic ) );
	interpolationModeOptions.push_back( SVEnumeratePair(SvO::Bilinear, SVInterpolationModeOptions::InterpolationModeBilinear ) );
	interpolationModeOptions.push_back( SVEnumeratePair(SvO::NearestNeighbor, SVInterpolationModeOptions::InterpolationModeNearestNeighbor ) );

	m_ResizeInterpolationMode.SetEnumTypes(interpolationModeOptions);
	m_ResizeInterpolationMode.SetDefaultValue(SVInterpolationModeOptions::InterpolationModeAuto, true);

	return hr;
}

HRESULT	ResizeTool::InitializeOverscanMember()
{
	HRESULT	hr = S_OK;

	SVEnumerateVector overscanOptions;
	overscanOptions.push_back( SVEnumeratePair(SvO::Enabled, SVOverscanOptions::OverscanEnable ) );
	overscanOptions.push_back( SVEnumeratePair(SvO::Disabled, SVOverscanOptions::OverscanDisable ) );

	m_ResizeOverscan.SetEnumTypes(overscanOptions);
	m_ResizeOverscan.SetDefaultValue(SVOverscanOptions::OverscanEnable, true);

	return hr;
}

HRESULT	ResizeTool::InitializePerformanceMember()
{
	HRESULT	hr = S_OK;

	SVEnumerateVector performanceOptions;
	performanceOptions.push_back( SVEnumeratePair(SvO::Fast, SVPerformanceOptions::PerformanceFast ) );
	performanceOptions.push_back( SVEnumeratePair(SvO::Precise, SVPerformanceOptions::PerformancePresice ) );

	m_ResizePerformance.SetEnumTypes(performanceOptions);
	m_ResizePerformance.SetDefaultValue(SVPerformanceOptions::PerformancePresice, TRUE);

	return hr;
}

void ResizeTool::BuildInputObjectList()
{
	// Source Image
	m_InputImageObjectInfo.SetInputObjectType( SVImageObjectType );
	m_InputImageObjectInfo.SetObject( GetObjectInfo() );
	RegisterInputObject( &m_InputImageObjectInfo, SvO::ResizeImage );

	addDefaultInputObjects();
}

void ResizeTool::BuildEmbeddedObjectList()
{
	RegisterEmbeddedObject( &m_svSourceImageName, 
							SVSourceImageNamesGuid, 
							IDS_OBJECTNAME_SOURCE_IMAGE_NAMES, 
							false, 
							SVResetItemTool );

	RegisterEmbeddedObject( &m_ResizeInterpolationMode, 
							SVResizeInterpolationModeGuid, 
							IDS_OBJECTNAME_RESIZE_INTERPOLATIONMODE, 
							false, 
							SVResetItemTool );

	RegisterEmbeddedObject( &m_ResizeOverscan, 
							SVResizeOverscanGuid, 
							IDS_OBJECTNAME_RESIZE_OVERSCAN, 
							false, 
							SVResetItemTool );

	RegisterEmbeddedObject( &m_ResizePerformance, 
							SVResizePerformanceGuid, 
							IDS_OBJECTNAME_RESIZE_PERFORMANCE, 
							false, 
							SVResetItemTool );

	RegisterEmbeddedObject( &m_OutputImage, 
							SVOutputImageObjectGuid, 
							IDS_OBJECTNAME_IMAGE1 );

	RegisterEmbeddedObject( &m_LogicalROIImage, 
							SVLogicalROIImageGuid, 
							IDS_OBJECTNAME_IMAGE2 );
}


ResizeTool::~ResizeTool(void)
{
	CloseObject();
}


BOOL ResizeTool::CloseObject()
{
	BOOL bRetVal = true;

	if ( m_isCreated )
	{
//		This statement should not be necessary because it should be set within 
//		the base class.  
//		isCreated = FALSE;

		bRetVal = m_LogicalROIImage.CloseObject();

		bRetVal = m_OutputImage.CloseObject() && bRetVal;

		// This should end up setting isCreated to FALSE within 
		// SVObjectClass::CloseObject().
		bRetVal = SVToolClass::CloseObject() && bRetVal;
	}

	return bRetVal;
}


BOOL ResizeTool::CreateObject( SVObjectLevelCreateStruct* pCreateStructure )
{
	BOOL bOk = SVToolClass::CreateObject( pCreateStructure ); 

	// Override base class hiding of Scale Factors.  These values will be 
	// exposed for the Resize Tool.
	m_ExtentWidthScaleFactor.ObjectAttributesAllowedRef() = 
		(SV_DEFAULT_VALUE_OBJECT_ATTRIBUTES | 
		 SV_REMOTELY_SETABLE | 
		 SV_EXTENT_OBJECT | 
		 SV_SETABLE_ONLINE) & 
		~SV_EMBEDABLE;				// Since this value object is already 
									// exposed as an extent, we do not want 
									// it to be embeddable.
	m_ExtentHeightScaleFactor.ObjectAttributesAllowedRef() = 
		(SV_DEFAULT_VALUE_OBJECT_ATTRIBUTES | 
		SV_REMOTELY_SETABLE | 
		SV_EXTENT_OBJECT | 
		SV_SETABLE_ONLINE) & 
		~SV_EMBEDABLE;				// Since this value object is already 
									// exposed as an extent, we do not want 
									// it to be embeddable.
	SVImageClass* inputImage = getInputImage();
	bOk &= (nullptr != inputImage);

	bOk &= (S_OK == m_LogicalROIImage.InitializeImage(inputImage));

	// We do not want the ROI image showing up as an output image.
	m_LogicalROIImage.ObjectAttributesAllowedRef() |= SV_HIDDEN;

	bOk &= (S_OK == m_svToolExtent.SetTranslation( SVExtentTranslationResize ));

	bOk &= (S_OK == m_OutputImage.InitializeImage( inputImage ) );

	bOk &= (nullptr != GetTool());

	bOk &= (nullptr != GetInspection());

	bOk &= (S_OK == ToolSizeAdjustTask::EnsureInFriendList(this, true, true, true)); 

	m_svSourceImageName.ObjectAttributesAllowedRef() &= ~SV_REMOTELY_SETABLE & ~SV_SETABLE_ONLINE;

	m_isCreated = bOk;

	return bOk;
}

HRESULT ResizeTool::IsInputImage( SVImageClass *p_psvImage )
{
	HRESULT l_hrOk = S_FALSE;

	const SVImageClass* inputImage = getInputImage();

	if ( nullptr != p_psvImage && p_psvImage == inputImage )
	{
		l_hrOk = S_OK;
	}
	return l_hrOk;
}	

SVImageClass* ResizeTool::getInputImage()
{
	if( m_InputImageObjectInfo.IsConnected() && 
		m_InputImageObjectInfo.GetInputObjectInfo().PObject )
		return dynamic_cast<SVImageClass*>(m_InputImageObjectInfo.GetInputObjectInfo().PObject);

	return nullptr;
}

SVImageClass* ResizeTool::getOutputImage()
{
	return static_cast <SVImageClass*> (&m_OutputImage);
}

SVImageClass* ResizeTool::getLogicalROIImage()
{
	return static_cast <SVImageClass*> (&m_LogicalROIImage);
}

SVEnumerateValueObjectClass* ResizeTool::getInterpolationMode()
{
	return &m_ResizeInterpolationMode;
}

SVEnumerateValueObjectClass* ResizeTool::getOverscan()
{
	return &m_ResizeOverscan;
}

SVEnumerateValueObjectClass* ResizeTool::getPerformance()
{
	return &m_ResizePerformance;
}

HRESULT ResizeTool::SetImageExtentToParent(unsigned long p_ulIndex)
{
	HRESULT l_hrOk = S_OK;
	SVImageExtentClass l_NewExtent;

	l_hrOk = m_svToolExtent.UpdateExtentToParentExtents( p_ulIndex, l_NewExtent ); // 1 for testing only; should be last index or next index

	if( S_OK == l_hrOk )
	{
		//@WARNING [Jim][8 July 2015] - Research this.  All tools are calling 
		// base class which does not validate.  Should probably call the 
		// derived tool classes explicit version.
		l_hrOk = SVToolClass::SetImageExtent( p_ulIndex, l_NewExtent );
	}
	return l_hrOk;
}

HRESULT ResizeTool::SetImageExtent( unsigned long p_ulIndex, SVImageExtentClass p_svImageExtent )
{
	HRESULT l_hrOk = m_svToolExtent.ValidExtentAgainstParentImage( p_svImageExtent );

	if( S_OK == l_hrOk )
	{
		l_hrOk = SVToolClass::SetImageExtent( p_ulIndex, p_svImageExtent );
	}

	return l_hrOk;
}

SVTaskObjectClass* ResizeTool::GetObjectAtPoint( const SVExtentPointStruct &p_rsvPoint )
{
	SVImageExtentClass l_svExtents;

	SVTaskObjectClass *l_psvObject = nullptr;

	if( S_OK == m_svToolExtent.GetImageExtent( l_svExtents ) &&
		l_svExtents.GetLocationPropertyAt( p_rsvPoint ) != SVExtentLocationPropertyUnknown )
	{
		l_psvObject = this;
	}
	return l_psvObject;
}

bool ResizeTool::DoesObjectHaveExtents() const
{
	return true;
}

SVStaticStringValueObjectClass* ResizeTool::GetInputImageNames()
{
	return &m_svSourceImageName;
}

bool ResizeTool::ResetObject(SvStl::MessageContainerVector *pErrorMessages)
{
	bool Result = ValidateParameters(pErrorMessages) && ValidateOfflineParameters(pErrorMessages);
	
	SVImageClass* inputImage = getInputImage();
	if (Result)
	{
		if (nullptr != inputImage)
		{
			//Set input name to source image name to display it in result picker
			m_svSourceImageName.SetValue( 0/*Static value, this parameter will not used*/, SVString( inputImage->GetCompleteName() ) );
		}
		else
		{
			Result = false;
			if (nullptr != pErrorMessages)
			{
				SvStl::MessageContainer Msg( SVMSG_SVO_5047_GETINPUTIMAGEFAILED, SvOi::Tid_Empty, SvStl::SourceFileParams(StdMessageParams), 0, GetUniqueObjectID() );
				pErrorMessages->push_back(Msg);
			}
		}
	}
		
	if (Result)
	{
		// required within ResetObject in order to correctly reallocate
		// buffers when source image is changed within GUI.
		if (S_FALSE == m_LogicalROIImage.InitializeImage( inputImage ))
		{
			Result = false;
			if (nullptr != pErrorMessages)
			{
				SvStl::MessageContainer Msg( SVMSG_SVO_5048_INITIALIZEROIIMAGEFAILED, SvOi::Tid_Empty, SvStl::SourceFileParams(StdMessageParams), 0, GetUniqueObjectID() );
				pErrorMessages->push_back(Msg);
			}
		}
	}

	if (Result)
	{
		// required within ResetObject in order to correctly reallocate
		// buffers when source image is changed within GUI.
		if (S_FALSE == m_OutputImage.InitializeImage( inputImage ))
		{
			Result = false;
			if (nullptr != pErrorMessages)
			{
				SvStl::MessageContainer Msg( SVMSG_SVO_5049_INITIALIZEOUTPUTIMAGEFAILED, SvOi::Tid_Empty, SvStl::SourceFileParams(StdMessageParams), 0, GetUniqueObjectID() );
				pErrorMessages->push_back(Msg);
			}
		}
	}
		
	Result = Result && SVToolClass::ResetObject(pErrorMessages);

	return Result;
}

bool ResizeTool::ValidateParameters (SvStl::MessageContainerVector *pErrorMessages)
{
	SVImageExtentClass toolImageExtents;
	bool Result = true;
	HRESULT hr = GetImageExtent(toolImageExtents);
	if (S_OK == hr)
	{
		double newWidthScaleFactor = 0.0;
		toolImageExtents.GetExtentProperty(SVExtentPropertyWidthScaleFactor, newWidthScaleFactor);
		Result = ValidateScaleFactor(newWidthScaleFactor, pErrorMessages) && Result;

		double newHeightScaleFactor = 0.0;
		toolImageExtents.GetExtentProperty(SVExtentPropertyHeightScaleFactor, newHeightScaleFactor);
		Result = ValidateScaleFactor(newHeightScaleFactor, pErrorMessages) && Result;
	}
	else
	{
		Result = false;
		if (nullptr != pErrorMessages)
		{
			SvStl::MessageContainer Msg( SVMSG_SVO_5071_CAPTUREDSFALSE, SvOi::Tid_Empty, SvStl::SourceFileParams(StdMessageParams), 0, GetUniqueObjectID() );
			pErrorMessages->push_back(Msg);
		}
	}
	return Result;
}

bool ResizeTool::ValidateOfflineParameters (SvStl::MessageContainerVector *pErrorMessages)
{
	SVInterpolationModeOptions::SVInterpolationModeOptionsEnum interpolationValue;
	SVEnumerateValueObjectClass* interpolationMode = getInterpolationMode ();
	interpolationMode->GetValue (*(reinterpret_cast <long*> (&interpolationValue)));
	bool Result = ValidateInterpolation(interpolationValue, pErrorMessages);

	SVOverscanOptions::SVOverscanOptionsEnum overscanValue;
	SVEnumerateValueObjectClass* overscan = getOverscan ();
	overscan->GetValue (*(reinterpret_cast <long*> (&overscanValue)));
	Result = ValidateOverscan(overscanValue) && Result;

	SVPerformanceOptions::SVPerformanceOptionsEnum performanceValue;
	SVEnumerateValueObjectClass* performance = getPerformance ();
	performance->GetValue (*(reinterpret_cast <long*> (&performanceValue)));
	Result = ValidatePerformance(performanceValue) && Result;

	return Result;
}

bool	ResizeTool::ValidateInterpolation (SVInterpolationModeOptions::SVInterpolationModeOptionsEnum interpolationMode, SvStl::MessageContainerVector *pErrorMessages)
{
	bool Result = true;

	switch (interpolationMode)
	{
	case	SVInterpolationModeOptions::InterpolationModeAuto:	
	case	SVInterpolationModeOptions::InterpolationModeBilinear:	
	case	SVInterpolationModeOptions::InterpolationModeBicubic:	
		// Average will only work with values less than 1.  Auto will 
		// already use Average for values less than 1.
	case	SVInterpolationModeOptions::InterpolationModeNearestNeighbor:
		{
			break;
		}
	default:
		{
			// invalid parameter
			Result = false;
			if (nullptr != pErrorMessages)
			{
				SvStl::MessageContainer Msg( SVMSG_SVO_5044_INVALIDINTERPOLATIONMODE, SvOi::Tid_Empty, SvStl::SourceFileParams(StdMessageParams), 0, GetUniqueObjectID() );
				pErrorMessages->push_back(Msg);
			}
			break;
		}
	}

	return Result;
}

bool	ResizeTool::ValidateOverscan (const SVOverscanOptions::SVOverscanOptionsEnum overscan, SvStl::MessageContainerVector *pErrorMessages)
{
	bool Result = true;

	switch (overscan)
	{
	case	SVOverscanOptions::OverscanEnable:	
	case	SVOverscanOptions::OverscanDisable:
		{
			break;
		}
	default:
		{
			// invalid parameter
			Result = false;
			if (nullptr != pErrorMessages)
			{
				SvStl::MessageContainer Msg( SVMSG_SVO_5045_INVALIDOVERSCAN, SvOi::Tid_Empty, SvStl::SourceFileParams(StdMessageParams), 0, GetUniqueObjectID() );
				pErrorMessages->push_back(Msg);
			}
			break;
		}
	} 

	return Result;
}

bool	ResizeTool::ValidatePerformance (const SVPerformanceOptions::SVPerformanceOptionsEnum performance, SvStl::MessageContainerVector *pErrorMessages)
{
	bool Result = true;

	switch (performance)
	{
	case	SVPerformanceOptions::PerformanceFast:	
	case	SVPerformanceOptions::PerformancePresice:
		{
			break;
		}
	default:
		{
			// invalid parameter
			Result = false;
			if (nullptr != pErrorMessages)
			{
				SvStl::MessageContainer Msg( SVMSG_SVO_5046_INVALIDPERFORMANCE, SvOi::Tid_Empty, SvStl::SourceFileParams(StdMessageParams), 0, GetUniqueObjectID() );
				pErrorMessages->push_back(Msg);
			}
			break;
		}
	}

	return Result;
}

bool ResizeTool::ValidateScaleFactor(const double value, SvStl::MessageContainerVector *pErrorMessages)
{
	bool Result = true;

	// Arbitrary high end scale factor limit of 1000.
	if ((value <= MinScaleFactorThreshold) ||
		(value > MaxScaleFactor))
	{
		Result = false;
		if (nullptr != pErrorMessages)
		{
			SVStringVector msgList;
			msgList.push_back(SvUl_SF::Format(_T("%d"), value));
			SvStl::MessageContainer Msg( SVMSG_SVO_5061_SFOUTSIDERANGE, SvOi::Tid_Default, msgList, SvStl::SourceFileParams(StdMessageParams), 0, GetUniqueObjectID() );
			pErrorMessages->push_back(Msg);
		}
	}

	return Result;
}

HRESULT	ResizeTool::BackupInspectionParameters ()
{
	SVImageExtentClass toolImageExtents;

	HRESULT hr = GetImageExtent(toolImageExtents);
	//  Embedded functions such as SVImageExtentClass::SetExtentProperty can return
	//  S_FALSE.  This violates the HRESULT standard.
	if (S_FALSE == hr)
	{
		hr = SVMSG_SVO_5063_ERRORGETTINGIMAGEEXTENTS;
	}

	if (SUCCEEDED (hr))
	{
		toolImageExtents.GetExtentProperty (SVExtentPropertyHeightScaleFactor, m_ResizeHeightSF_Backup);
		toolImageExtents.GetExtentProperty (SVExtentPropertyWidthScaleFactor, m_ResizeWidthSF_Backup);
		m_ResizeInterpolationMode.GetValue (m_ResizeInterpolationMode_Backup);
		m_ResizeOverscan.GetValue (m_ResizeOverscan_Backup);
		m_ResizePerformance.GetValue (m_ResizePerformance_Backup);
	}

	return hr;
}

HRESULT	ResizeTool::GetBackupInspectionParameters (	double*	oldHeightScaleFactor,
													double*	oldWidthScaleFactor,
													long*	oldInterpolationMode,
													long*	oldOverscan,
													long*	oldPerformance)
{
	HRESULT hr = S_OK;

	*oldHeightScaleFactor = m_ResizeHeightSF_Backup;
	*oldWidthScaleFactor = m_ResizeWidthSF_Backup;
	*oldInterpolationMode = m_ResizeInterpolationMode_Backup;
	*oldOverscan = m_ResizeOverscan_Backup;
	*oldPerformance = m_ResizePerformance_Backup;

	return hr;
}

bool ResizeTool::onRun( SVRunStatusClass& RRunStatus, SvStl::MessageContainerVector *pErrorMessages )
{
	bool Result = __super::onRun( RRunStatus, pErrorMessages );
	
	if( !m_OutputImage.SetImageHandleIndex(RRunStatus.Images) )
	{
		Result = false;
		if (nullptr != pErrorMessages)
		{
			SvStl::MessageContainer Msg( SVMSG_SVO_5021_SETIMAGEHANDLEFAILED, SvOi::Tid_Empty, SvStl::SourceFileParams(StdMessageParams), 0, GetUniqueObjectID() );
			pErrorMessages->push_back(Msg);
		}
	}

	SVSmartHandlePointer	roiImageHandle;
	if( Result )
	{
		// The following logic was extrapolated from the StdImageOperatorList Run method.
		// It corrects an issue where the output image is black while running when using the toolset image.
		if (m_LogicalROIImage.GetLastResetTimeStamp() <= getInputImage()->GetLastResetTimeStamp())
		{
			if ( !SUCCEEDED( UpdateImageWithExtent(RRunStatus.m_lResultDataIndex) ) )
			{
				Result = false;
				if (nullptr != pErrorMessages)
				{
					SvStl::MessageContainer Msg( SVMSG_SVO_5022_UPDATEIMAGEEXTENTSFAILED, SvOi::Tid_Empty, SvStl::SourceFileParams(StdMessageParams), 0, GetUniqueObjectID() );
					pErrorMessages->push_back(Msg);
				}
			}
		}
	}
		
	//-----	Execute this objects run functionality. -----------------------------
	if( Result )
	{
		m_LogicalROIImage.GetImageHandle(roiImageHandle);
		if (roiImageHandle.empty())
		{
			Result = false;
			if (nullptr != pErrorMessages)
			{
				SvStl::MessageContainer Msg( SVMSG_SVO_5023_ROIGETIMAGEHANDLEFAILED, SvOi::Tid_Empty, SvStl::SourceFileParams(StdMessageParams), 0, GetUniqueObjectID() );
				pErrorMessages->push_back(Msg);
			}
		}
	}

	SVImageBufferHandleImage roiMilHandle;
	if( Result )
	{
		roiImageHandle->GetData(roiMilHandle);
		if (roiMilHandle.empty())
		{
			Result = false;
			if (nullptr != pErrorMessages)
			{
				SvStl::MessageContainer Msg( SVMSG_SVO_5024_ROIGETDATAFAILED, SvOi::Tid_Empty, SvStl::SourceFileParams(StdMessageParams), 0, GetUniqueObjectID() );
				pErrorMessages->push_back(Msg);
			}
		}
	}

	if( Result )
	{
		const SVMatroxBuffer& roiMilBuffer = roiMilHandle.GetBuffer();

		if (roiMilBuffer.empty())
		{
			Result = false;
			if (nullptr != pErrorMessages)
			{
				SvStl::MessageContainer Msg( SVMSG_SVO_5025_ROIGETBUFFERFAILED, SvOi::Tid_Empty, SvStl::SourceFileParams(StdMessageParams), 0, GetUniqueObjectID() );
				pErrorMessages->push_back(Msg);
			}
		}

		SVSmartHandlePointer outputImageHandle;

		if( Result )
		{
			m_OutputImage.GetImageHandle(outputImageHandle);
			if (outputImageHandle.empty())
			{
				Result = false;
				if (nullptr != pErrorMessages)
				{
					SvStl::MessageContainer Msg( SVMSG_SVO_5026_OUTPUTGETIMAGEHANDLEFAILED, SvOi::Tid_Empty, SvStl::SourceFileParams(StdMessageParams), 0, GetUniqueObjectID() );
					pErrorMessages->push_back(Msg);
				}
			}
		}

		SVImageBufferHandleImage outputMilHandle;

		if( Result )
		{
			outputImageHandle->GetData(outputMilHandle);

			if (outputMilHandle.empty())
			{
				Result = false;
				if (nullptr != pErrorMessages)
				{
					SvStl::MessageContainer Msg( SVMSG_SVO_5027_OUTPUTGETDATAFAILED, SvOi::Tid_Empty, SvStl::SourceFileParams(StdMessageParams), 0, GetUniqueObjectID() );
					pErrorMessages->push_back(Msg);
				}
			}

			if( Result )
			{
				const SVMatroxBuffer& outputMilBuffer = outputMilHandle.GetBuffer();

				if (outputMilBuffer.empty())
				{
					Result = false;
					if (nullptr != pErrorMessages)
					{
						SvStl::MessageContainer Msg( SVMSG_SVO_5028_OUTPUTGETBUFFERFAILED, SvOi::Tid_Empty, SvStl::SourceFileParams(StdMessageParams), 0, GetUniqueObjectID() );
						pErrorMessages->push_back(Msg);
					}
				}

				SVImageExtentClass toolImageExtents;

				if( Result )
				{
					if( !SUCCEEDED( GetImageExtent(toolImageExtents) ))
					{
						Result = false;
						if (nullptr != pErrorMessages)
						{
							SvStl::MessageContainer Msg( SVMSG_SVO_5029_GETEXTENTSFAILED, SvOi::Tid_Empty, SvStl::SourceFileParams(StdMessageParams), 0, GetUniqueObjectID() );
							pErrorMessages->push_back(Msg);
						}
					}
				}

				double heightScaleFactor = 0.0;				
				if( Result )
				{
					if( !SUCCEEDED( toolImageExtents.GetExtentProperty(SVExtentPropertyHeightScaleFactor, heightScaleFactor) ))
					{
						Result = false;
						if (nullptr != pErrorMessages)
						{
							SvStl::MessageContainer Msg( SVMSG_SVO_5030_GETHEIGHTSFFAILED, SvOi::Tid_Empty, SvStl::SourceFileParams(StdMessageParams), 0, GetUniqueObjectID() );
							pErrorMessages->push_back(Msg);
						}
					}
				}

				double widthScaleFactor = 0.0;
				if( Result )
				{
					if( !SUCCEEDED( toolImageExtents.GetExtentProperty(SVExtentPropertyWidthScaleFactor, widthScaleFactor) ))
					{
						Result = false;
						if (nullptr != pErrorMessages)
						{
							SvStl::MessageContainer Msg( SVMSG_SVO_5031_GETWIDTHSFFAILED, SvOi::Tid_Empty, SvStl::SourceFileParams(StdMessageParams), 0, GetUniqueObjectID() );
							pErrorMessages->push_back(Msg);
						}
					}
				}

				long interpolationMode = 0;
				if( Result )
				{
					if( !SUCCEEDED( m_ResizeInterpolationMode.GetValue(interpolationMode) ))
					{
						Result = false;
						if (nullptr != pErrorMessages)
						{
							SvStl::MessageContainer Msg( SVMSG_SVO_5032_GETINTERPOLATIONMODEFAILED, SvOi::Tid_Empty, SvStl::SourceFileParams(StdMessageParams), 0, GetUniqueObjectID() );
							pErrorMessages->push_back(Msg);
						}
					}
				}
				
				long overscan = 0;
				if( Result )
				{
					if( !SUCCEEDED( m_ResizeOverscan.GetValue(overscan) ))
					{
						Result = false;
						if (nullptr != pErrorMessages)
						{
							SvStl::MessageContainer Msg( SVMSG_SVO_5033_GETOVERSCANFAILED, SvOi::Tid_Empty, SvStl::SourceFileParams(StdMessageParams), 0, GetUniqueObjectID() );
							pErrorMessages->push_back(Msg);
						}
					}
				}

				long performance = 0;
				if( Result )
				{
					if( !SUCCEEDED( m_ResizePerformance.GetValue(performance) ))
					{
						Result = false;
						if (nullptr != pErrorMessages)
						{
							SvStl::MessageContainer Msg( SVMSG_SVO_5034_GETPERFORMANCEFAILED, SvOi::Tid_Empty, SvStl::SourceFileParams(StdMessageParams), 0, GetUniqueObjectID() );
							pErrorMessages->push_back(Msg);
						}
					}
				}

				if( Result )
				{
					HRESULT hr = SVMatroxImageInterface::Resize (outputMilBuffer,
						roiMilBuffer,
						widthScaleFactor,
						heightScaleFactor,
						static_cast <SVInterpolationModeOptions::SVInterpolationModeOptionsEnum>(interpolationMode),
						static_cast <SVOverscanOptions::SVOverscanOptionsEnum> (overscan),
						static_cast <SVPerformanceOptions::SVPerformanceOptionsEnum> (performance));
					if( !SUCCEEDED( hr ) )
					{
						Result = false;
						if (nullptr != pErrorMessages)
						{
							SvStl::MessageContainer Msg( hr, SvOi::Tid_Empty, SvStl::SourceFileParams(StdMessageParams), 0, GetUniqueObjectID() );
							pErrorMessages->push_back(Msg);
						}
					}
				}
			}
		}
	}

	if (!Result)
	{
		RRunStatus.SetInvalid();
	}

	return Result;
}
