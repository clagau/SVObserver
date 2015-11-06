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
	outObjectInfo.ObjectTypeInfo.ObjectType = SVToolObjectType;
	outObjectInfo.ObjectTypeInfo.SubType    = SVResizeToolObjectType;
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
	BOOL bRetVal = TRUE;

	if ( isCreated )
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

	SVString	inputImageName = inputImage->GetCompleteName();

	bOk &= (S_OK == m_svSourceImageName.SetValue(1, inputImageName.c_str()));

	isCreated = bOk;

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

	if( l_hrOk == S_OK )
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

HRESULT ResizeTool::GetInputImageNames( SVStringValueObjectClass*& p_pSourceNames )
{
	p_pSourceNames = &m_svSourceImageName;
	return S_OK;
}


HRESULT ResizeTool::ResetObject()
{
	HRESULT	hr  = S_OK;

	hr = OnValidate(RemotelyAndInspectionSettable);

	SVImageClass* inputImage = nullptr;
	if (SUCCEEDED (hr))
	{
		inputImage = getInputImage();

		if (nullptr == inputImage)
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

	for (SVTaskObjectPtrVector::iterator it = m_aTaskObjects.begin(); it != m_aTaskObjects.end(); ++it)
	{
		SVTaskObjectClass* pTask = *it;
		bValid &= pTask->OnValidate();
	}
	return SVToolClass::IsValid() && bValid;
}


BOOL ResizeTool::Validate()
{
	BOOL	br = TRUE; // BOOL Result

	// base class Validate MUST be called before using m_ValidationErrorCode.
	// That's where m_ValidationErrorCode gets initialized for this tool.
	br = SVToolClass::Validate();

	if ((FALSE == br) && SUCCEEDED (m_ValidationError.m_MessageCode))
	{
		m_ValidationError.m_MessageCode = SVMSG_SVO_5072_INCONSISTENTDATA;
	}

	if (SUCCEEDED (m_ValidationError.m_MessageCode))
	{
		m_ValidationError.m_MessageCode = ValidateOfflineParameters();
	}

	if (SUCCEEDED (m_ValidationError.m_MessageCode))
	{
		m_ValidationError.m_MessageCode = ValidateRemotelySettableParameters();
	}

	if (!SUCCEEDED(m_ValidationError.m_MessageCode))
	{
		br = FALSE;
	}

	return br;
}


BOOL ResizeTool::OnValidate()
{
	HRESULT hr = E_FAIL;
	BOOL	br = FALSE; // BOOL Result

	if (SUCCEEDED (m_ValidationError.m_MessageCode))
	{
		br = SVToolClass::OnValidate();
		if (SUCCEEDED (m_ValidationError.m_MessageCode) && (false == br))
		{
			m_ValidationError.m_MessageCode = SVMSG_SVO_5074_BASECLASSONVALIDATEFAILED; 
		}
	}

	if (SUCCEEDED (m_ValidationError.m_MessageCode))
	{
		// The setting of hr should not matter.  m_ValidationErrorCode should 
		// contain the same error.  In the case where OnValidate might 
		// contain a subsequent error, we want the first error which is 
		// assumed to be in m_ValidationErrorCode.
		hr = OnValidate (InspectionSettable);
		if (SUCCEEDED (hr) != SUCCEEDED (m_ValidationError.m_MessageCode))
		{
			m_ValidationError.m_MessageCode = SVMSG_SVO_5073_INCONSISTENTDATA;
		}
	}

	br = SUCCEEDED (m_ValidationError.m_MessageCode);

	return br;
}


HRESULT	ResizeTool::OnValidate (ValidationLevelEnum validationLevel)
{
	if (SUCCEEDED (m_ValidationError.m_MessageCode))
	{
		m_ValidationError.m_MessageCode = ValidateInspectionSettableParameters ();
	}

	if (SUCCEEDED (m_ValidationError.m_MessageCode) && 
		((RemotelyAndInspectionSettable == validationLevel) ||
		 (AllParameters == validationLevel)))
	{
		m_ValidationError.m_MessageCode = ValidateRemotelySettableParameters ();			
	}

	if (SUCCEEDED (m_ValidationError.m_MessageCode) && 
	    (AllParameters == validationLevel))
	{
		m_ValidationError.m_MessageCode = ValidateOfflineParameters ();
	}

	return m_ValidationError.m_MessageCode;
}


HRESULT	ResizeTool::ValidateInspectionSettableParameters ()
{
	HRESULT	hr = S_OK;
	// Possibly should include ROI extents (top, bottom, height, width), but 
	// I think Extents are checked elsewhere.
	return hr;
}


HRESULT	ResizeTool::ValidateRemotelySettableParameters ()
{
	HRESULT	hr = S_OK;

	SVImageExtentClass toolImageExtents;

	hr = GetImageExtent(toolImageExtents);
	if (S_FALSE == hr)
	{
		hr = SVMSG_SVO_5071_CAPTUREDSFALSE;
	}

	if (SUCCEEDED(hr))
	{
		double newWidthScaleFactor = 0.0;
		toolImageExtents.GetExtentProperty(SVExtentPropertyWidthScaleFactor, newWidthScaleFactor);
		hr = ValidateScaleFactor(newWidthScaleFactor);
	}

	if (SUCCEEDED(hr))
	{
		double newHeightScaleFactor = 0.0;
		toolImageExtents.GetExtentProperty(SVExtentPropertyHeightScaleFactor, newHeightScaleFactor);
		hr = ValidateScaleFactor(newHeightScaleFactor);
	}

	return hr;
}


HRESULT	ResizeTool::ValidateOfflineParameters ()
{
	HRESULT	hr = S_OK;

	SVInterpolationModeOptions::SVInterpolationModeOptionsEnum interpolationValue;
	SVEnumerateValueObjectClass* interpolationMode = getInterpolationMode ();
	interpolationMode->GetValue (*(reinterpret_cast <long*> (&interpolationValue)));
	hr = ValidateInterpolation(interpolationValue);
	if (SUCCEEDED(hr))
	{
		SVOverscanOptions::SVOverscanOptionsEnum overscanValue;
		SVEnumerateValueObjectClass* overscan = getOverscan ();
		overscan->GetValue (*(reinterpret_cast <long*> (&overscanValue)));
		hr = ValidateOverscan(overscanValue);
	}

	if (SUCCEEDED(hr))
	{
		SVPerformanceOptions::SVPerformanceOptionsEnum performanceValue;
		SVEnumerateValueObjectClass* performance = getPerformance ();
		performance->GetValue (*(reinterpret_cast <long*> (&performanceValue)));
		hr = ValidatePerformance(performanceValue);
	}

	return hr;
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
	BOOL returnValue =	false;

	if (SUCCEEDED (m_RunError.m_MessageCode))
	{ 
		returnValue = SVToolClass::onRun( RRunStatus );
		if (SUCCEEDED (m_RunError.m_MessageCode) && (TRUE != returnValue))
		{
			m_RunError.m_MessageCode = SVMSG_SVO_5020_BASECLASSONRUNFAILED;
		}
	}
	
	if (SUCCEEDED (m_RunError.m_MessageCode))
	{
		// Update the index for the Output Image
		returnValue = m_OutputImage.SetImageHandleIndex(RRunStatus.Images);
		if (!returnValue)
		{
			m_RunError.m_MessageCode = SVMSG_SVO_5021_SETIMAGEHANDLEFAILED;
		}
	}

	SVSmartHandlePointer	roiImageHandle;

	if (SUCCEEDED (m_RunError.m_MessageCode))
	{
		// The following logic was extrapolated from the StdImageOperatorList Run method.
		// It corrects an issue where the output image is black while running when using the toolset image.
		if (m_LogicalROIImage.GetLastResetTimeStamp() <= getInputImage()->GetLastResetTimeStamp())
		{
			m_RunError.m_MessageCode = UpdateImageWithExtent(RRunStatus.m_lResultDataIndex);

			if (!SUCCEEDED (m_RunError.m_MessageCode))
			{
				m_RunError.m_MessageCode = SVMSG_SVO_5022_UPDATEIMAGEEXTENTSFAILED;
			}
		}
	}
		
	//-----	Execute this objects run functionality. -----------------------------
	if (SUCCEEDED (m_RunError.m_MessageCode))
	{
		m_LogicalROIImage.GetImageHandle(roiImageHandle);
		if (roiImageHandle.empty())
		{
			m_RunError.m_MessageCode = SVMSG_SVO_5023_ROIGETIMAGEHANDLEFAILED;
		}
	}

	SVImageBufferHandleImage roiMilHandle;

	if (SUCCEEDED (m_RunError.m_MessageCode))
	{
		roiImageHandle->GetData(roiMilHandle);
		if (roiMilHandle.empty())
		{
			m_RunError.m_MessageCode = SVMSG_SVO_5024_ROIGETDATAFAILED;
		}
	}

	if (SUCCEEDED (m_RunError.m_MessageCode))
	{
		const SVMatroxBuffer& roiMilBuffer = roiMilHandle.GetBuffer();

		if (roiMilBuffer.empty())
		{
			m_RunError.m_MessageCode = SVMSG_SVO_5025_ROIGETBUFFERFAILED;
		}

		SVSmartHandlePointer outputImageHandle;

		if (SUCCEEDED (m_RunError.m_MessageCode))
		{
			m_OutputImage.GetImageHandle(outputImageHandle);
			if (outputImageHandle.empty())
			{
				m_RunError.m_MessageCode = SVMSG_SVO_5026_OUTPUTGETIMAGEHANDLEFAILED;
			}
		}

		SVImageBufferHandleImage outputMilHandle;

		if (SUCCEEDED (m_RunError.m_MessageCode))
		{
			outputImageHandle->GetData(outputMilHandle);

			if (outputMilHandle.empty())
			{
				m_RunError.m_MessageCode = SVMSG_SVO_5027_OUTPUTGETDATAFAILED;
			}

			if (SUCCEEDED (m_RunError.m_MessageCode))
			{
				const SVMatroxBuffer& outputMilBuffer = outputMilHandle.GetBuffer();

				if (outputMilBuffer.empty())
				{
					m_RunError.m_MessageCode = SVMSG_SVO_5028_OUTPUTGETBUFFERFAILED;
				}

				SVImageExtentClass toolImageExtents;

				if (SUCCEEDED (m_RunError.m_MessageCode))
				{
					m_RunError.m_MessageCode = GetImageExtent(toolImageExtents);

					if (!SUCCEEDED  (m_RunError.m_MessageCode))
					{
						m_RunError.m_MessageCode = SVMSG_SVO_5029_GETEXTENTSFAILED;
					}
				}

				double heightScaleFactor = 0.0;
				
				if (SUCCEEDED (m_RunError.m_MessageCode))
				{
					m_RunError.m_MessageCode = toolImageExtents.GetExtentProperty(SVExtentPropertyHeightScaleFactor, heightScaleFactor);

					if (!SUCCEEDED  (m_RunError.m_MessageCode))
					{
						m_RunError.m_MessageCode = SVMSG_SVO_5030_GETHEIGHTSFFAILED;
					}
				}

				double widthScaleFactor = 0.0;

				if (SUCCEEDED (m_RunError.m_MessageCode))
				{
					m_RunError.m_MessageCode = toolImageExtents.GetExtentProperty(SVExtentPropertyWidthScaleFactor, widthScaleFactor);

					if (!SUCCEEDED  (m_RunError.m_MessageCode))
					{
						m_RunError.m_MessageCode = SVMSG_SVO_5031_GETWIDTHSFFAILED;
					}
				}

				long interpolationMode = 0;

				if (SUCCEEDED (m_RunError.m_MessageCode))
				{
					m_RunError.m_MessageCode = m_ResizeInterpolationMode.GetValue(interpolationMode);

					if (!SUCCEEDED  (m_RunError.m_MessageCode))
					{
						m_RunError.m_MessageCode = SVMSG_SVO_5032_GETINTERPOLATIONMODEFAILED;
					}
				}
				
				long overscan = 0;

				if (SUCCEEDED (m_RunError.m_MessageCode))
				{
					m_RunError.m_MessageCode = m_ResizeOverscan.GetValue(overscan);

					if (!SUCCEEDED  (m_RunError.m_MessageCode))
					{
						m_RunError.m_MessageCode = SVMSG_SVO_5033_GETOVERSCANFAILED;
					}
				}

				long performance = 0;

				if (SUCCEEDED (m_RunError.m_MessageCode))
				{
					m_RunError.m_MessageCode = m_ResizePerformance.GetValue(performance);

					if (!SUCCEEDED  (m_RunError.m_MessageCode))
					{
						m_RunError.m_MessageCode = SVMSG_SVO_5034_GETPERFORMANCEFAILED;
					}
				}

				if (SUCCEEDED (m_RunError.m_MessageCode))
				{
					m_RunError.m_MessageCode = SVMatroxImageInterface::Resize (outputMilBuffer,
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

	if (returnValue && !SUCCEEDED (m_RunError.m_MessageCode))
	{
		returnValue = false;
	}

	if (!returnValue)
	{
		RRunStatus.SetInvalid();
	}

	return returnValue;
}
