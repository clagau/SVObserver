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
#include "SVOCore/SVAnalyzer.h"
#include "SVOCore/SVImageProcessingClass.h"
#include "SVImageLibrary\SVImageBufferHandleImage.h"
#include "SVStatusLibrary/ErrorNumbers.h"
#include "SVStatusLibrary\MessageManager.h"
#include "TextDefinesSvO.h"
#include "SVUtilityLibrary/SVString.h"
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

	m_BufferDepth.SetObjectAttributesAllowed( SvOi::SV_REMOTELY_SETABLE, SvOi::SetAttributeType::AddAttribute );
	if (bOk)
	{
		for (int i=0; i < SvOi::cRingBufferNumberOutputImages; i++)
		{
			bOk &= (S_OK == m_OutputImages[i].InitializeImage( inputImage ));
			m_ImageIndexManager[i].SetObjectAttributesAllowed( SvOi::SV_REMOTELY_SETABLE, SvOi::SetAttributeType::AddAttribute );
		}
	}

	bOk &= (nullptr != GetTool());
	bOk &= (nullptr != GetInspection());

	m_SourceImageNames.setStatic( true );
	m_SourceImageNames.setSaveValueFlag(false);
	m_SourceImageNames.SetObjectAttributesAllowed( SvOi::SV_REMOTELY_SETABLE | SvOi::SV_SETABLE_ONLINE, SvOi::SetAttributeType::RemoveAttribute );

	// Override base class exposure of the draw flag
	// This value will not be exposed for the RingBuffer Tool.
	drawToolFlag.SetObjectAttributesAllowed( SvOi::SV_HIDDEN, SvOi::SetAttributeType::OverwriteAttribute );

	// Override base class exposure of the auxillary extent variables
	// These values will not be exposed for the RingBuffer Tool.
	m_svUpdateAuxiliaryExtents.SetObjectAttributesAllowed( SvOi::SV_HIDDEN, SvOi::SetAttributeType::OverwriteAttribute );
	m_svAuxiliarySourceX.SetObjectAttributesAllowed( SvOi::SV_HIDDEN, SvOi::SetAttributeType::OverwriteAttribute );
	m_svAuxiliarySourceY.SetObjectAttributesAllowed( SvOi::SV_HIDDEN, SvOi::SetAttributeType::OverwriteAttribute );
	m_svAuxiliarySourceAngle.SetObjectAttributesAllowed( SvOi::SV_HIDDEN, SvOi::SetAttributeType::OverwriteAttribute );
	m_svAuxiliarySourceImageName.SetObjectAttributesAllowed( SvOi::SV_HIDDEN, SvOi::SetAttributeType::OverwriteAttribute );
	m_svAuxiliaryDrawType.SetObjectAttributesAllowed( SvOi::SV_HIDDEN, SvOi::SetAttributeType::OverwriteAttribute );

	m_isCreated = bOk;

	return bOk;
}

bool RingBufferTool::DoesObjectHaveExtents() const
{
	return false;
}

SVStringValueObjectClass* RingBufferTool::GetInputImageNames()
{
	return &m_SourceImageNames;
}

bool RingBufferTool::ResetObject(SvStl::MessageContainerVector *pErrorMessages)
{
	bool Result = SVToolClass::ResetObject(pErrorMessages);

	SetToolROIExtentToFullInputImage ();

	long ringBufferDepth = 0;
	m_BufferDepth.GetValue(ringBufferDepth);
	if( SvOi::cRingBufferDepthMin > ringBufferDepth || SvOi::cRingBufferDepthMax < ringBufferDepth )
	{
		Result = false;
		if (nullptr != pErrorMessages)
		{
			SVStringVector msgList;
			msgList.push_back(SvUl_SF::Format("%d", SvOi::cRingBufferDepthMin));
			msgList.push_back(SvUl_SF::Format("%d", SvOi::cRingBufferDepthMax));
			msgList.push_back(SvUl_SF::Format("%d", ringBufferDepth));
			SvStl::MessageContainer message( SVMSG_SVO_61_RINGBUFFER_ERROR, SvStl::Tid_RingBuffer_Depth_Invalid_Value, msgList, SvStl::SourceFileParams(StdMessageParams), SvStl::Err_10013_RingBuffer_DepthValueInvalid, GetUniqueObjectID() );
			pErrorMessages->push_back(message);
		}
	}

	if (Result)
	{
		SVImageClass* inputImage = getInputImage();
		if (nullptr != inputImage)
		{
			//Set input name to source image name to display it in result picker
			m_SourceImageNames.SetValue( inputImage->GetCompleteName() );

			SVImageInfoClass imageInfo = inputImage->GetImageInfo();

			//create ring buffer images
			m_ringBuffer.clear();
			m_ringBuffer.resize(ringBufferDepth);
			m_isBufferFull = false;
			m_nextBufferPos = 0;

			imageInfo.setDibBufferFlag(false);
			for (int i=0; i<ringBufferDepth; i++)
			{
				SVSmartHandlePointer imageHandle;
				SVImageProcessingClass::CreateImageBuffer(imageInfo, imageHandle);
				m_ringBuffer[i] = imageHandle;
			}
		}
		else
		{
			Result = false;
			if (nullptr != pErrorMessages)
			{
				SvStl::MessageContainer Msg( SVMSG_SVO_92_GENERAL_ERROR, SvStl::Tid_NoSourceImage, SvStl::SourceFileParams(StdMessageParams), 0, GetUniqueObjectID() );
				pErrorMessages->push_back(Msg);
			}
		}
	}

	return Result;
}

SVImageClass* RingBufferTool::getInputImage()
{
	if( m_InputImageObjectInfo.IsConnected() && 
		m_InputImageObjectInfo.GetInputObjectInfo().m_pObject )
	{
		return static_cast< SVImageClass* >(m_InputImageObjectInfo.GetInputObjectInfo().m_pObject);
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

bool RingBufferTool::onRun( SVRunStatusClass& rRunStatus, SvStl::MessageContainerVector *pErrorMessages )
{
	bool returnValue = SVToolClass::onRun( rRunStatus, pErrorMessages );
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
		m_FlagOfOutputImage.SetValue( imageOutputFlag, rRunStatus.m_lResultDataIndex );
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
		m_OutputImages[i].InitializeImage( SvOi::SVImageTypeEnum::SVImageTypePhysical );
	}

	// Set up your type... in this case this will reference that this tool is a 
	// Resize Tool.
	m_outObjectInfo.m_ObjectTypeInfo.ObjectType = SVToolObjectType;
	m_outObjectInfo.m_ObjectTypeInfo.SubType    = SVRingBufferToolObjectType;

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
	RegisterEmbeddedObject( &m_SourceImageNames, SVSourceImageNamesGuid, IDS_OBJECTNAME_SOURCE_IMAGE_NAMES, false, SvOi::SVResetItemTool );
	
	RegisterEmbeddedObject( &m_BufferDepth, RingBuffer_DepthGuid, IDS_OBJECTNAME_RINGBUFFER_DEPTH, true, SvOi::SVResetItemTool );
	m_BufferDepth.SetDefaultValue( cDefaultRingBufferDepth, true );

	RegisterEmbeddedObject( &m_FlagOfOutputImage, RingBuffer_FlagOfOutputImagesGuid, IDS_OBJECTNAME_RINGBUFFER_FLAG, false, SvOi::SVResetItemNone );
	m_FlagOfOutputImage.SetDefaultValue( 0, true );
	m_FlagOfOutputImage.setSaveValueFlag(false);

	int RingbufferIndexNames[SvOi::cRingBufferNumberOutputImages] = { IDS_OBJECTNAME_RINGBUFFER_INDEX1, IDS_OBJECTNAME_RINGBUFFER_INDEX2};
	int ImageNames[SvOi::cRingBufferNumberOutputImages] = { IDS_OBJECTNAME_IMAGE1, IDS_OBJECTNAME_IMAGE2};
	for( int i = 0; i < SvOi::cRingBufferNumberOutputImages; i++)
	{
		RegisterEmbeddedObject( &m_ImageIndexManager[i], RingBuffer_IndexGuid[i], RingbufferIndexNames[i], true, SvOi::SVResetItemTool );
		_variant_t vtTemp;
		::VariantInit(&vtTemp);
		vtTemp.vt = cVarType_imageIndex;
		vtTemp.lVal = cDefaultIndexValue[i];
		m_ImageIndexManager[i].SetDefaultValue( vtTemp, true );

		SVString ObjectName = SvUl_SF::LoadSVString( RingbufferIndexNames[i] );
		ObjectName +=  SvO::cLinkName;
		RegisterEmbeddedObject( &m_ImageIndexManager[i].getLinkedName(), RingBufferLink_IndexGuid[i], ObjectName.c_str(), false, SvOi::SVResetItemNone );
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
			HRESULT statusCode = SVMatroxBufferInterface::CopyBuffer( handleToOutputImage.GetBuffer(), handleToIndex.GetBuffer());
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

		double inputHeight( 0.0 );
		double inputWidth( 0.0 );
		inputImageExtents.GetExtentProperty (SVExtentPropertyHeight, inputHeight);
		inputImageExtents.GetExtentProperty (SVExtentPropertyWidth, inputWidth);

		long scratchpadIndex = 1;
		m_svToolExtent.SetExtentValue (SVExtentPropertyHeight, scratchpadIndex, inputHeight);
		m_svToolExtent.SetExtentValue (SVExtentPropertyWidth, scratchpadIndex, inputWidth);
		m_svToolExtent.SetExtentValue (SVExtentPropertyPositionPointX , scratchpadIndex, 0.0);
		m_svToolExtent.SetExtentValue (SVExtentPropertyPositionPointY , scratchpadIndex, 0.0);
	}
}
#pragma endregion Private Methods
