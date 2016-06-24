//*****************************************************************************
/// \copyright (c) 2015 by Seidenader Maschinenbau GmbH
/// All Rights Reserved 
/// \Author	Marc Ziegler
//*****************************************************************************
/// Class of the RingBuffer-tool
//*****************************************************************************
#pragma region Includes
#include "StdAfx.h"
#include "RingBufferTool.h"
#include "SVAnalyzer.h"
#include "SVMatroxImageProcessingClass.h"
#include "SVImageLibrary\SVImageBufferHandleImage.h"
#include "ObjectInterfaces\ErrorNumbers.h"
#include "SVStatusLibrary\MessageManager.h"
#include "TextDefinesSvO.h"
#pragma endregion Includes

#pragma region Declarations
static const int cDefaultRingBufferDepth = 8;
static const long cDefaultIndexValue[] = {1, 2};
static const VARTYPE cVarType_imageIndex = VT_I4;

SV_IMPLEMENT_CLASS( RingBufferTool, RingBufferToolGuid );
#pragma endregion Declarations

#pragma region Constructor
RingBufferTool::RingBufferTool( BOOL bCreateDefaultTaskList, SVObjectClass* pOwner, int stringResourceID )
	:SVToolClass( bCreateDefaultTaskList, pOwner, stringResourceID )
	, m_isBufferFull(false)
	, m_nextBufferPos(0)
{
	LocalInitialize ();
}

RingBufferTool::~RingBufferTool(void)
{
}
#pragma endregion Constructor

#pragma region Public Methods
BOOL RingBufferTool::CreateObject( SVObjectLevelCreateStruct* PCreateStructure )
{
	BOOL bOk = SVToolClass::CreateObject( PCreateStructure ); //  TRUE/FALSE

	SVImageClass* inputImage = getInputImage ();
	bOk &= (nullptr != inputImage);

	bOk &= (S_OK == m_svToolExtent.SetTranslation( SVExtentTranslationNone ));
	SetToolROIExtentToFullInputImage ();

	m_BufferDepth.ObjectAttributesAllowedRef() = SV_REMOTELY_SETABLE | SV_VIEWABLE | SV_PRINTABLE | SV_SELECTABLE_FOR_EQUATION;
	if (bOk)
	{
		for (int i=0; i < SvOi::cRingBufferNumberOutputImages; i++)
		{
			bOk &= (S_OK == m_OutputImages[i].InitializeImage( inputImage ));
			m_ImageIndexManager[i].ObjectAttributesAllowedRef() = SV_REMOTELY_SETABLE | SV_VIEWABLE | SV_PRINTABLE | SV_SELECTABLE_FOR_EQUATION;
		}
	}

	bOk &= (nullptr != GetTool());
	bOk &= (nullptr != GetInspection());

	m_svSourceImageName.ObjectAttributesAllowedRef() &=~SV_REMOTELY_SETABLE & ~SV_SETABLE_ONLINE;

	// Override base class exposure of the draw flag
	// This value will not be exposed for the RingBuffer Tool.
	drawToolFlag.ObjectAttributesAllowedRef() = SV_HIDDEN;

	// Override base class exposure of the auxillary extent variables
	// These values will not be exposed for the RingBuffer Tool.
	m_svUpdateAuxiliaryExtents.ObjectAttributesAllowedRef() = SV_HIDDEN;
	m_svAuxiliarySourceX.ObjectAttributesAllowedRef() = SV_HIDDEN;
	m_svAuxiliarySourceY.ObjectAttributesAllowedRef() = SV_HIDDEN;
	m_svAuxiliarySourceAngle.ObjectAttributesAllowedRef() = SV_HIDDEN;
	m_svAuxiliarySourceImageName.ObjectAttributesAllowedRef() = SV_HIDDEN;
	m_svAuxiliaryDrawType.ObjectAttributesAllowedRef() = SV_HIDDEN;

	m_isCreated = bOk;

	return bOk;
}

bool RingBufferTool::DoesObjectHaveExtents() const
{
	return false;
}

SVStaticStringValueObjectClass* RingBufferTool::GetInputImageNames()
{
	return &m_svSourceImageName;
}

HRESULT RingBufferTool::ResetObject()
{
	HRESULT status = SVToolClass::ResetObject();

	SetToolROIExtentToFullInputImage ();

	if (OnValidate())
	{
		SVImageClass* inputImage = getInputImage ();
		if (nullptr != inputImage)
		{
			//Set input name to source image name to display it in result picker
			m_svSourceImageName.SetValue(0/*Static value, this parameter will not used*/, inputImage->GetCompleteObjectName() );

			SVImageInfoClass imageInfo = inputImage->GetImageInfo();

			if (S_OK == status)
			{
				//create ring buffer images
				long ringBufferDepth = 0;
				m_BufferDepth.GetValue(ringBufferDepth);
				m_ringBuffer.clear();
				m_ringBuffer.resize(ringBufferDepth);
				m_isBufferFull = false;
				m_nextBufferPos = 0;
				
				imageInfo.setDibBufferFlag(false);
				for (int i=0; i<ringBufferDepth; i++)
				{
					SVSmartHandlePointer imageHandle;
					SVMatroxImageProcessingClass::Instance().CreateImageBuffer(imageInfo, imageHandle);
					m_ringBuffer[i] = imageHandle;
				}
			}
		}
		else
		{
			status = SvOi::Err_10012_RingBuffer_NoInputImage;
		}
	}
	else
	{
		status = SvOi::Err_10011_RingBuffer_OnValidFailed;
	}

	return status;
}

BOOL RingBufferTool::OnValidate()
{
	bool bValid = (nullptr != getInputImage());

	long ringBufferDepth = 0;
	m_BufferDepth.GetValue(ringBufferDepth);
	if( SvOi::cRingBufferDepthMin > ringBufferDepth || SvOi::cRingBufferDepthMax < ringBufferDepth )
	{
		bValid = false;
		SvStl::MessageMgrNoDisplay Exception( SvStl::LogOnly );
		SVStringArray msgList;
		msgList.push_back(SvUl_SF::Format("%d", SvOi::cRingBufferDepthMin));
		msgList.push_back(SvUl_SF::Format("%d", SvOi::cRingBufferDepthMax));
		msgList.push_back(SvUl_SF::Format("%d", ringBufferDepth));
		Exception.setMessage( SVMSG_SVO_61_RINGBUFFER_ONVALIDATE_ERROR, SvOi::Tid_RingBuffer_Depth_Invalid_Value, msgList, SvStl::SourceFileParams(StdMessageParams), SvOi::Err_10013_RingBuffer_DepthValueInvalid );
	}
	for (int i=0; i< SvOi::cRingBufferNumberOutputImages; ++i)
	{
		bValid &= m_ImageIndexManager[i].IsValid() ? true : false;
	}	

	return SVToolClass::OnValidate() && bValid ;
}

SVImageClass* RingBufferTool::getInputImage()
{
	if( m_InputImageObjectInfo.IsConnected() && 
		m_InputImageObjectInfo.GetInputObjectInfo().PObject )
	{
		return static_cast< SVImageClass* >(m_InputImageObjectInfo.GetInputObjectInfo().PObject);
	}

	return nullptr;
}

SVImageClass* RingBufferTool::getOutputImage(int index)
{
	SVImageClass *retValue = nullptr;
	if (index >= 0 && index < SvOi::cRingBufferNumberOutputImages)
	{
		retValue = &m_OutputImages[index];
	}
	return retValue;
}
#pragma endregion Public Methods

#pragma region Protected Methods
HRESULT RingBufferTool::IsInputImage( SVImageClass *p_psvImage )
{
	HRESULT l_hrOk = S_FALSE;
	//@Hack: [MZA][24.04.15] This method is only used by CollectOverlays and the recursive functionality of IsInputImage. 
	// Always FALSE, optimizes the CollectOverlay (there is no overlay of this tool). 
	// In further implementation, it should be possible to return the right result and have nevertheless a optimized CollectOverlay method. 
	return l_hrOk;
}	

BOOL RingBufferTool::onRun( SVRunStatusClass& RRunStatus )
{
	BOOL returnValue = S_FALSE;

	returnValue = SVToolClass::onRun( RRunStatus );
	if (returnValue)
	{
		//-----	Execute this objects run functionality. -----------------------------
		int imageOutputFlag = 0;
		long ringBufferDepth = 0;
		m_BufferDepth.GetValue(ringBufferDepth);
		int maxIndexPos = ringBufferDepth -1;
		if (!m_isBufferFull)
		{
			maxIndexPos = m_nextBufferPos -1;
		}

		//set output image
		for (int i=0; i < SvOi::cRingBufferNumberOutputImages; ++i)
		{
			_variant_t depthVariant;
			m_ImageIndexManager[i].GetValue( depthVariant );
			if (cVarType_imageIndex == depthVariant.vt && 0 < depthVariant.lVal && ringBufferDepth >= depthVariant.lVal) //imageIndex invalid -> output image deactivated.
			{
				imageOutputFlag |= SetOutputImage(i, depthVariant.lVal, maxIndexPos, ringBufferDepth);
			}
		}

		//copy input image to ring buffer
		SVImageBufferHandleImage milHandleTo;
		SVImageBufferHandleImage milHandleFrom;
		if( static_cast<int>(m_ringBuffer.size()) > m_nextBufferPos && !( m_ringBuffer[m_nextBufferPos].empty() ) )
		{
			m_ringBuffer[m_nextBufferPos]->GetData( milHandleTo );
		}
		SVImageClass* inputImage = getInputImage ();
		if (nullptr != inputImage)
		{
			SVSmartHandlePointer inputImageBuffer;
			inputImage->GetImageHandle(inputImageBuffer);
			if( !( inputImageBuffer.empty() ) )
			{
				inputImageBuffer->GetData( milHandleFrom );
			}
		}
		if (!milHandleTo.empty() && !milHandleFrom.empty())
		{
			SVMatroxBufferInterface::CopyBuffer( milHandleTo.GetBuffer(), milHandleFrom.GetBuffer());
		}

		//calculate next image pos
		m_nextBufferPos++;
		if (ringBufferDepth <= m_nextBufferPos)
		{
			m_nextBufferPos=0;
			m_isBufferFull = true;
		}

		//set flag
		m_FlagOfOutputImage.SetValue(RRunStatus.m_lResultDataIndex, imageOutputFlag);
	}

	return returnValue;
}
#pragma endregion Protected Methods

#pragma region Private Methods
void RingBufferTool::LocalInitialize ()
{
	BuildInputObjectList ();
	BuildEmbeddedObjectList ();

	for (int i=0; i<SvOi::cRingBufferNumberOutputImages; i++)
	{
		// The output image is referenced in the embedded list.
		m_OutputImages[i].InitializeImage( SVImageTypePhysical );
	}

	// Set up your type... in this case this will reference that this tool is a 
	// Resize Tool.
	m_outObjectInfo.ObjectTypeInfo.ObjectType = SVToolObjectType;
	m_outObjectInfo.ObjectTypeInfo.SubType    = SVRingBufferToolObjectType;

	m_ringBuffer.clear();
}

void RingBufferTool::BuildInputObjectList ()
{
	// Source Image
	m_InputImageObjectInfo.SetInputObjectType( SVImageObjectType );
	m_InputImageObjectInfo.SetObject( GetObjectInfo() );
	RegisterInputObject( &m_InputImageObjectInfo, _T( "InputImage" ) );

	addDefaultInputObjects();
}


void RingBufferTool::BuildEmbeddedObjectList ()
{
	RegisterEmbeddedObject( &m_svSourceImageName, SVSourceImageNamesGuid, IDS_OBJECTNAME_SOURCE_IMAGE_NAMES, false, SVResetItemTool );
	
	RegisterEmbeddedObject( &m_BufferDepth, RingBuffer_DepthGuid, IDS_OBJECTNAME_RINGBUFFER_DEPTH, true, SVResetItemTool );
	m_BufferDepth.SetDefaultValue( cDefaultRingBufferDepth, true );

	RegisterEmbeddedObject( &m_FlagOfOutputImage, RingBuffer_FlagOfOutputImagesGuid, IDS_OBJECTNAME_RINGBUFFER_FLAG, false, SVResetItemNone );
	m_FlagOfOutputImage.SetDefaultValue( 0, true );

	int RingbufferIndexNames[SvOi::cRingBufferNumberOutputImages] = { IDS_OBJECTNAME_RINGBUFFER_INDEX1, IDS_OBJECTNAME_RINGBUFFER_INDEX2};
	int ImageNames[SvOi::cRingBufferNumberOutputImages] = { IDS_OBJECTNAME_IMAGE1, IDS_OBJECTNAME_IMAGE2};
	for( int i = 0; i < SvOi::cRingBufferNumberOutputImages; i++)
	{
		RegisterEmbeddedObject( &m_ImageIndexManager[i], RingBuffer_IndexGuid[i], RingbufferIndexNames[i], true, SVResetItemTool );
		_variant_t vtTemp;
		::VariantInit(&vtTemp);
		vtTemp.vt = cVarType_imageIndex;
		vtTemp.lVal = cDefaultIndexValue[i];
		m_ImageIndexManager[i].SetDefaultValue( vtTemp, true );

		CString ObjectName;
		ObjectName.LoadString( RingbufferIndexNames[i] );
		ObjectName +=  SvO::cLinkName;
		RegisterEmbeddedObject( &m_ImageIndexManager[i].getLinkedName(), RingBufferLink_IndexGuid[i], ObjectName, false, SVResetItemNone );
		m_ImageIndexManager[i].getLinkedName().SetDefaultValue( _T(""), false );

		RegisterEmbeddedObject( &m_OutputImages[i], aSVVariantResultImageObjectGuid[i], ImageNames[i] );
	}
}

int RingBufferTool::SetOutputImage( int outputIndex, int imageIndex, int maxIndexPos, long ringBufferDepth )
{
	int retValue = 0;
	int pos = (m_nextBufferPos + imageIndex - 1)%ringBufferDepth;
	
	if ( 0 <= pos && maxIndexPos >= pos)
	{
		SVImageBufferHandleImage handleToIndex;
		SVImageBufferHandleImage handleToOutputImage;
		if( static_cast<int>(m_ringBuffer.size()) > pos && !( m_ringBuffer[pos].empty() ) )
		{
			m_ringBuffer[pos]->GetData( handleToIndex );
		}
		SVSmartHandlePointer outputImageBuffer;
		m_OutputImages[outputIndex].GetImageHandle(outputImageBuffer);
		if( !( outputImageBuffer.empty() ) )
		{
			outputImageBuffer->GetData( handleToOutputImage );
		}
		if (!handleToOutputImage.empty() && !handleToIndex.empty())
		{
			SVMatroxBufferInterface::SVStatusCode statusCode = SVMatroxBufferInterface::CopyBuffer( handleToOutputImage.GetBuffer(), handleToIndex.GetBuffer());
			if (S_OK == statusCode)
			{
				retValue = 1 << outputIndex;
			}
		}
	}
	return retValue;
}

void RingBufferTool::SetToolROIExtentToFullInputImage ()
{
	SVImageClass* inputImage = getInputImage ();

	if (nullptr != inputImage)
	{
		SVImageExtentClass inputImageExtents = inputImage->GetImageExtents ();

		double inputHeight = 0.0;
		inputImageExtents.GetExtentProperty (SVExtentPropertyHeight, inputHeight);
		double inputWidth = 0.0;
		inputImageExtents.GetExtentProperty (SVExtentPropertyWidth, inputWidth);

		long scratchpadIndex = 1;
		m_svToolExtent.SetExtentValue (SVExtentPropertyHeight, scratchpadIndex, inputHeight);
		m_svToolExtent.SetExtentValue (SVExtentPropertyWidth, scratchpadIndex, inputWidth);
		m_svToolExtent.SetExtentValue (SVExtentPropertyPositionPointX , scratchpadIndex, 0);
		m_svToolExtent.SetExtentValue (SVExtentPropertyPositionPointY , scratchpadIndex, 0);
	}
}

DWORD_PTR RingBufferTool::processMessage( DWORD DwMessageID, DWORD_PTR DwMessageValue, DWORD_PTR DwMessageContext )
{
	DWORD_PTR dwResult = SVMR_NOT_PROCESSED;

	// Try to process message by yourself...
	DWORD dwPureMessageID = DwMessageID & SVM_PURE_MESSAGE;
	switch( dwPureMessageID )
	{
		// handle renaming of toolset variables
	case SVMSGID_OBJECT_RENAMED:
		{
			SVObjectClass* pObject = reinterpret_cast <SVObjectClass*> (DwMessageValue); // Object with new name
			LPCTSTR OriginalName = reinterpret_cast<LPCTSTR> (DwMessageContext);

			for (int i=0; i < SvOi::cRingBufferNumberOutputImages; i++)
			{
				//This will refresh the dotted name using the Unique ID
				m_ImageIndexManager[i].UpdateLinkedName();
			}
			dwResult = SVMR_SUCCESS;
			break;
		}
	}

	dwResult = SVToolClass::processMessage( DwMessageID, DwMessageValue, DwMessageContext ) | dwResult;
	return dwResult;
}
#pragma endregion Private Methods
