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
	extentWidthScaleFactor.ObjectAttributesAllowedRef() = 
		(SV_DEFAULT_VALUE_OBJECT_ATTRIBUTES | 
		 SV_REMOTELY_SETABLE | 
		 SV_EXTENT_OBJECT | 
		 SV_SETABLE_ONLINE) & 
		~SV_EMBEDABLE;				// Since this value object is already 
									// exposed as an extent, we do not want 
									// it to be embeddable.
	extentHeightScaleFactor.ObjectAttributesAllowedRef() = 
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

HRESULT ResizeTool::ResetObject()
{
	HRESULT	hr  = S_OK;

	if (!OnValidateParameter(RemotelyAndInspectionSettable))
	{
		hr = getFirstTaskMessage().getMessage().m_MessageCode;
	}

	SVImageClass* inputImage = nullptr;
	if (SUCCEEDED (hr))
	{
		inputImage = getInputImage();

		if (nullptr != inputImage)
		{
			//Set input name to source image name to display it in result picker
			m_svSourceImageName.SetValue( 0/*Static value, this parameter will not used*/, inputImage->GetCompleteObjectName() );
		}
		else
		{
			hr = SVMSG_SVO_5047_GETINPUTIMAGEFAILED;
		}
	}
		
	if (SUCCEEDED(hr))		
	{
		// required within ResetObject in order to correctly reallocate
		// buffers when source image is changed within GUI.
		hr = m_LogicalROIImage.InitializeImage( inputImage );
		//@WARNING [Jim] Several functions within InitializeImage can return 
		// S_FALSE.  This is not helpful in attempting to debug the error,
		// and does not adhere to HRESULT standards by setting the fail bit.
		// At some point, all S_FALSE return codes should be 
		// removed.  
		if (S_FALSE == hr)
		{
			hr = SVMSG_SVO_5048_INITIALIZEROIIMAGEFAILED;
		}
	}

	if (SUCCEEDED(hr))
	{
		// required within ResetObject in order to correctly reallocate
		// buffers when source image is changed within GUI.
		hr = m_OutputImage.InitializeImage( inputImage );
		if (S_FALSE == hr)
		{
			hr = SVMSG_SVO_5049_INITIALIZEOUTPUTIMAGEFAILED;
		}
	}
		
	if (SUCCEEDED(hr))
	{
		hr = SVToolClass::ResetObject();
		if (S_FALSE == hr)
		{
			hr = SVMSG_SVO_5050_BASECLASSFAILED;
		}
	}

	return hr;
}

BOOL ResizeTool::IsValid()
{
	BOOL bValid = true;
	
	ToolSizeAdjustTask* pToolSizeAdjustTask = ToolSizeAdjustTask::GetToolSizeAdjustTask(this);
	if (nullptr != pToolSizeAdjustTask)
	{
		bValid = pToolSizeAdjustTask->OnValidate();
	}

	return SVToolClass::IsValid() && bValid;
}

bool ResizeTool::ValidateRemotelySettableParameters ()
{
	bool Result = SVToolClass::ValidateRemotelySettableParameters();

	SVImageExtentClass toolImageExtents;

	HRESULT hr = GetImageExtent(toolImageExtents);
	if (S_OK != hr)
	{
		SvStl::MessageContainer message;
		message.setMessage( SVMSG_SVO_5071_CAPTUREDSFALSE, SvOi::Tid_Empty, SvStl::SourceFileParams(StdMessageParams) );
		addTaskMessage( message );
		Result = false;
	}

	if (Result)
	{
		double newWidthScaleFactor = 0.0;
		toolImageExtents.GetExtentProperty(SVExtentPropertyWidthScaleFactor, newWidthScaleFactor);
		Result = (S_OK == ValidateScaleFactor(newWidthScaleFactor));
	}

	if (Result)
	{
		double newHeightScaleFactor = 0.0;
		toolImageExtents.GetExtentProperty(SVExtentPropertyHeightScaleFactor, newHeightScaleFactor);
		Result = (S_OK == ValidateScaleFactor(newHeightScaleFactor));
	}

	return Result;
}

bool ResizeTool::ValidateOfflineParameters ()
{
	bool Result = SVToolClass::ValidateOfflineParameters();
	if (Result)
	{
		SVInterpolationModeOptions::SVInterpolationModeOptionsEnum interpolationValue;
		SVEnumerateValueObjectClass* interpolationMode = getInterpolationMode ();
		interpolationMode->GetValue (*(reinterpret_cast <long*> (&interpolationValue)));
		Result = (S_OK == ValidateInterpolation(interpolationValue));
	}

	if (Result)
	{
		SVOverscanOptions::SVOverscanOptionsEnum overscanValue;
		SVEnumerateValueObjectClass* overscan = getOverscan ();
		overscan->GetValue (*(reinterpret_cast <long*> (&overscanValue)));
		Result = (S_OK == ValidateOverscan(overscanValue));
	}

	if (Result)
	{
		SVPerformanceOptions::SVPerformanceOptionsEnum performanceValue;
		SVEnumerateValueObjectClass* performance = getPerformance ();
		performance->GetValue (*(reinterpret_cast <long*> (&performanceValue)));
		Result = (S_OK == ValidatePerformance(performanceValue));
	}

	return Result;
}

HRESULT	ResizeTool::ValidateInterpolation (SVInterpolationModeOptions::SVInterpolationModeOptionsEnum interpolationMode)
{
	HRESULT hr = S_OK;

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
			SvStl::MessageContainer message;
			message.setMessage( SVMSG_SVO_5044_INVALIDINTERPOLATIONMODE, SvOi::Tid_Empty, SvStl::SourceFileParams(StdMessageParams) );
			addTaskMessage( message );
			hr = SVMSG_SVO_5044_INVALIDINTERPOLATIONMODE;
			break;
		}
	}

	return hr;
}

HRESULT	ResizeTool::ValidateOverscan (const SVOverscanOptions::SVOverscanOptionsEnum overscan)
{
	HRESULT hr = S_OK;

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
			SvStl::MessageContainer message;
			message.setMessage( SVMSG_SVO_5045_INVALIDOVERSCAN, SvOi::Tid_Empty, SvStl::SourceFileParams(StdMessageParams) );
			addTaskMessage( message );
			hr = SVMSG_SVO_5045_INVALIDOVERSCAN;
			break;
		}
	} 

	return hr;
}

HRESULT	ResizeTool::ValidatePerformance (const SVPerformanceOptions::SVPerformanceOptionsEnum performance)
{
	HRESULT hr = S_OK;

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
			SvStl::MessageContainer message;
			message.setMessage( SVMSG_SVO_5046_INVALIDPERFORMANCE, SvOi::Tid_Empty, SvStl::SourceFileParams(StdMessageParams) );
			addTaskMessage( message );
			hr = SVMSG_SVO_5046_INVALIDPERFORMANCE;
			break;
		}
	}

	return hr;
}

HRESULT ResizeTool::ValidateScaleFactor(const double value)
{
	HRESULT hr = S_OK;

	// Arbitrary high end scale factor limit of 1000.
	if ((value <= MinScaleFactorThreshold) ||
		(value > MaxScaleFactor))
	{
		hr = SVMSG_SVO_5061_SFOUTSIDERANGE;
		SvStl::MessageContainer message;
		SVStringArray msgList;
		msgList.push_back(SvUl_SF::Format(_T("%d"), value));
		message.setMessage( SVMSG_SVO_5061_SFOUTSIDERANGE, SvOi::Tid_Default, msgList, SvStl::SourceFileParams(StdMessageParams) );
		addTaskMessage( message );
	}

	return hr;
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

BOOL ResizeTool::onRun( SVRunStatusClass& RRunStatus )
{
	BOOL Result =	false;
	HRESULT hr( S_OK );

	hr = getFirstTaskMessage().getMessage().m_MessageCode;
	if( SUCCEEDED( hr ) )
	{ 
		Result = SVToolClass::onRun( RRunStatus );
		hr = getFirstTaskMessage().getMessage().m_MessageCode;
		if( SUCCEEDED(hr) && !Result )
		{
			hr = SVMSG_SVO_5020_BASECLASSONRUNFAILED;
		}
	}
	
	if( SUCCEEDED( hr ) )
	{
		// Update the index for the Output Image
		Result = m_OutputImage.SetImageHandleIndex(RRunStatus.Images);
		if( !Result )
		{
			hr = SVMSG_SVO_5021_SETIMAGEHANDLEFAILED;
		}
	}

	SVSmartHandlePointer	roiImageHandle;
	if( SUCCEEDED( hr ))
	{
		// The following logic was extrapolated from the StdImageOperatorList Run method.
		// It corrects an issue where the output image is black while running when using the toolset image.
		if (m_LogicalROIImage.GetLastResetTimeStamp() <= getInputImage()->GetLastResetTimeStamp())
		{
			hr = UpdateImageWithExtent(RRunStatus.m_lResultDataIndex);

			if ( !SUCCEEDED( hr ) )
			{
				hr = SVMSG_SVO_5022_UPDATEIMAGEEXTENTSFAILED;
			}
		}
	}
		
	//-----	Execute this objects run functionality. -----------------------------
	if( SUCCEEDED( hr ))
	{
		m_LogicalROIImage.GetImageHandle(roiImageHandle);
		if (roiImageHandle.empty())
		{
			hr = SVMSG_SVO_5023_ROIGETIMAGEHANDLEFAILED;
		}
	}

	SVImageBufferHandleImage roiMilHandle;

	if( SUCCEEDED( hr ) )
	{
		roiImageHandle->GetData(roiMilHandle);
		if (roiMilHandle.empty())
		{
			hr = SVMSG_SVO_5024_ROIGETDATAFAILED;
		}
	}

	if( SUCCEEDED( hr ))
	{
		const SVMatroxBuffer& roiMilBuffer = roiMilHandle.GetBuffer();

		if (roiMilBuffer.empty())
		{
			hr = SVMSG_SVO_5025_ROIGETBUFFERFAILED;
		}

		SVSmartHandlePointer outputImageHandle;

		if( SUCCEEDED( hr ))
		{
			m_OutputImage.GetImageHandle(outputImageHandle);
			if (outputImageHandle.empty())
			{
				hr = SVMSG_SVO_5026_OUTPUTGETIMAGEHANDLEFAILED;
			}
		}

		SVImageBufferHandleImage outputMilHandle;

		if( SUCCEEDED( hr ))
		{
			outputImageHandle->GetData(outputMilHandle);

			if (outputMilHandle.empty())
			{
				hr = SVMSG_SVO_5027_OUTPUTGETDATAFAILED;
			}

			if( SUCCEEDED( hr ))
			{
				const SVMatroxBuffer& outputMilBuffer = outputMilHandle.GetBuffer();

				if (outputMilBuffer.empty())
				{
					hr = SVMSG_SVO_5028_OUTPUTGETBUFFERFAILED;
				}

				SVImageExtentClass toolImageExtents;

				if( SUCCEEDED( hr ))
				{
					hr = GetImageExtent(toolImageExtents);

					if( !SUCCEEDED( hr ))
					{
						hr = SVMSG_SVO_5029_GETEXTENTSFAILED;
					}
				}

				double heightScaleFactor = 0.0;
				
				if( SUCCEEDED( hr ))
				{
					hr = toolImageExtents.GetExtentProperty(SVExtentPropertyHeightScaleFactor, heightScaleFactor);

					if( !SUCCEEDED( hr ))
					{
						hr = SVMSG_SVO_5030_GETHEIGHTSFFAILED;
					}
				}

				double widthScaleFactor = 0.0;

				if( SUCCEEDED( hr ))
				{
					hr = toolImageExtents.GetExtentProperty(SVExtentPropertyWidthScaleFactor, widthScaleFactor);

					if( !SUCCEEDED( hr ))
					{
						hr = SVMSG_SVO_5031_GETWIDTHSFFAILED;
					}
				}

				long interpolationMode = 0;

				if( SUCCEEDED( hr ))
				{
					hr = m_ResizeInterpolationMode.GetValue(interpolationMode);

					if( !SUCCEEDED( hr ))
					{
						hr = SVMSG_SVO_5032_GETINTERPOLATIONMODEFAILED;
					}
				}
				
				long overscan = 0;

				if( SUCCEEDED( hr ))
				{
					hr = m_ResizeOverscan.GetValue(overscan);

					if( !SUCCEEDED( hr ))
					{
						hr = SVMSG_SVO_5033_GETOVERSCANFAILED;
					}
				}

				long performance = 0;

				if( SUCCEEDED( hr ))
				{
					hr = m_ResizePerformance.GetValue(performance);

					if( !SUCCEEDED( hr ))
					{
						hr = SVMSG_SVO_5034_GETPERFORMANCEFAILED;
					}
				}

				if( SUCCEEDED(  hr ))
				{
					hr = SVMatroxImageInterface::Resize (outputMilBuffer,
						roiMilBuffer,
						widthScaleFactor,
						heightScaleFactor,
						static_cast <SVInterpolationModeOptions::SVInterpolationModeOptionsEnum>(interpolationMode),
						static_cast <SVOverscanOptions::SVOverscanOptionsEnum> (overscan),
						static_cast <SVPerformanceOptions::SVPerformanceOptionsEnum> (performance));
				}
			}
		}
	}

	if( Result && !SUCCEEDED( hr ) )
	{
		SvStl::MessageContainer message;
		message.setMessage( hr, SvOi::Tid_Empty, SvStl::SourceFileParams(StdMessageParams) );
		addTaskMessage( message );
		Result = false;
	}

	if (!Result)
	{
		RRunStatus.SetInvalid();
	}

	return Result;
}
