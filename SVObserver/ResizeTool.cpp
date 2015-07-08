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

ResizeTool::ResizeTool( BOOL bCreateDefaultTaskList, SVObjectClass* pOwner, int stringResourceID )
: SVToolClass( bCreateDefaultTaskList, pOwner, stringResourceID )
{
	LocalInitialize();
}

void ResizeTool::LocalInitialize()
{
	InitializeInterpolationModeMember();
	InitializeOverscanMember();
	InitializePerformanceMember();

	BuildInputObjectList();

	// The output image is referenced in the embedded list.
	m_OutputImage.InitializeImage(SVImageTypePhysical);
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
}

BOOL ResizeTool::CreateObject( SVObjectLevelCreateStruct* pCreateStructure )
{
	BOOL bOk = SVToolClass::CreateObject( pCreateStructure ); //  TRUE/FALSE

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

// Set String value object for Source Image Names
HRESULT ResizeTool::CollectInputImageNames(SVRunStatusClass& RRunStatus)
{
	HRESULT hr = S_FALSE;
	SVImageClass* pInputImage = getInputImage();
	if (pInputImage)
	{
		CString strName = pInputImage->GetCompleteObjectName();
		m_svSourceImageName.SetValue(RRunStatus.m_lResultDataIndex, 0, strName);
		hr = S_OK;
	}
	return hr;
}

HRESULT ResizeTool::DoesObjectHaveExtents() const
{
	return S_OK;
}

HRESULT ResizeTool::GetInputImageNames( SVStringValueObjectClass*& p_pSourceNames )
{
	p_pSourceNames = &m_svSourceImageName;
	return S_OK;
}

HRESULT ResizeTool::ResetObject()
{
	HRESULT	hr  = m_LogicalROIImage.InitializeImage( getInputImage() );
	if (SUCCEEDED(hr))
	{
		hr = SVToolClass::ResetObject();
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

BOOL ResizeTool::onRun( SVRunStatusClass& RRunStatus )
{
	BOOL returnValue =	false;
	HRESULT	hr = S_OK;

	if (SUCCEEDED (hr))
	{
		hr = CollectInputImageNames(RRunStatus); // So that source image name value object gets populated
		if (hr != S_OK)
		{
			// CollectInputImageNames only returns an S_FALSE on a failure.
			hr = SVMSG_SVO_5019_COULDNOTCOLLECTINPUTIMAGENAMES;
		}
	}
	
	if (SUCCEEDED (hr))
	{ 
		returnValue = SVToolClass::onRun( RRunStatus );
		if (TRUE != returnValue)
		{
			hr = SVMSG_SVO_5020_BASECLASSONRUNFAILED;
		}
	}
	
	if (SUCCEEDED (hr))
	{
		// Update the index for the Output Image
		returnValue = m_OutputImage.SetImageHandleIndex(RRunStatus.Images);
		if (!returnValue)
		{
			hr = SVMSG_SVO_5021_SETIMAGEHANDLEFAILED;
		}
	}

	SVSmartHandlePointer	roiImageHandle;

	if (SUCCEEDED (hr))
	{
		// The following logic was extrapolated from the StdImageOperatorList Run method.
		// It corrects an issue where the output image is black while running when using the toolset image.
		if (m_LogicalROIImage.GetLastResetTimeStamp() <= getInputImage()->GetLastResetTimeStamp())
		{
			hr = UpdateImageWithExtent(RRunStatus.m_lResultDataIndex);

			if (!SUCCEEDED (hr))
			{
				hr = SVMSG_SVO_5022_UPDATEIMAGEEXTENTSFAILED;
			}
		}
	}
		
	//-----	Execute this objects run functionality. -----------------------------
	if (SUCCEEDED (hr))
	{
		m_LogicalROIImage.GetImageHandle(roiImageHandle);
		if (roiImageHandle.empty())
		{
			hr = SVMSG_SVO_5023_ROIGETIMAGEHANDLEFAILED;
		}
	}

	SVImageBufferHandleImage roiMilHandle;

	if (SUCCEEDED (hr))
	{
		roiImageHandle->GetData(roiMilHandle);
		if (roiMilHandle.empty())
		{
			hr = SVMSG_SVO_5024_ROIGETDATAFAILED;
		}
	}

	if (SUCCEEDED (hr))
	{
		const SVMatroxBuffer& roiMilBuffer = roiMilHandle.GetBuffer();

		if (roiMilBuffer.empty())
		{
			hr = SVMSG_SVO_5025_ROIGETBUFFERFAILED;
		}

		SVSmartHandlePointer outputImageHandle;

		if (SUCCEEDED (hr))
		{
			m_OutputImage.GetImageHandle(outputImageHandle);
			if (outputImageHandle.empty())
			{
				hr = SVMSG_SVO_5026_OUTPUTGETIMAGEHANDLEFAILED;
			}
		}

		SVImageBufferHandleImage outputMilHandle;

		if (SUCCEEDED (hr))
		{
			outputImageHandle->GetData(outputMilHandle);

			if (outputMilHandle.empty())
			{
				hr = SVMSG_SVO_5027_OUTPUTGETDATAFAILED;
			}

			if (SUCCEEDED (hr))
			{
				const SVMatroxBuffer& outputMilBuffer = outputMilHandle.GetBuffer();

				if (outputMilBuffer.empty())
				{
					hr = SVMSG_SVO_5028_OUTPUTGETBUFFERFAILED;
				}

				SVImageExtentClass toolImageExtents;

				if (SUCCEEDED (hr))
				{
					hr = GetImageExtent(toolImageExtents);

					if (!SUCCEEDED  (hr))
					{
						hr = SVMSG_SVO_5029_GETEXTENTSFAILED;
					}
				}

				double heightScaleFactor = 0.0;
				
				if (SUCCEEDED (hr))
				{
					hr = toolImageExtents.GetExtentProperty(SVExtentPropertyHeightScaleFactor, heightScaleFactor);

					if (!SUCCEEDED  (hr))
					{
						hr = SVMSG_SVO_5030_GETHEIGHTSFFAILED;
					}
				}

				double widthScaleFactor = 0.0;

				if (SUCCEEDED (hr))
				{
					hr = toolImageExtents.GetExtentProperty(SVExtentPropertyWidthScaleFactor, widthScaleFactor);

					if (!SUCCEEDED  (hr))
					{
						hr = SVMSG_SVO_5031_GETWIDTHSFFAILED;
					}
				}

				long interpolationMode = 0;

				if (SUCCEEDED (hr))
				{
					hr = m_ResizeInterpolationMode.GetValue(interpolationMode);

					if (!SUCCEEDED  (hr))
					{
						hr = SVMSG_SVO_5032_GETINTERPOLATIONMODEFAILED;
					}
				}
				
				long overscan = 0;

				if (SUCCEEDED (hr))
				{
					hr = m_ResizeOverscan.GetValue(overscan);

					if (!SUCCEEDED  (hr))
					{
						hr = SVMSG_SVO_5033_GETOVERSCANFAILED;
					}
				}

				long performance = 0;

				if (SUCCEEDED (hr))
				{
					hr = m_ResizePerformance.GetValue(performance);

					if (!SUCCEEDED  (hr))
					{
						hr = SVMSG_SVO_5034_GETPERFORMANCEFAILED;
					}
				}

				if (SUCCEEDED (hr))
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

	if (returnValue && !SUCCEEDED (hr))
	{
		returnValue = false;
	}
	if (!returnValue)
	{
		RRunStatus.SetInvalid();
	}
	return returnValue;
}
