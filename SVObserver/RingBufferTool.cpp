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
#include "InspectionEngine/SVAnalyzer.h"
#include "InspectionEngine/SVImageProcessingClass.h"
#include "SVImageLibrary\SVImageBufferHandleImage.h"
#include "SVStatusLibrary/ErrorNumbers.h"
#include "SVStatusLibrary\MessageManager.h"
#include "TextDefinesSvO.h"
#include "Definitions/StringTypeDef.h"
#include "SVUtilityLibrary/StringHelper.h"
#pragma endregion Includes

#pragma region Declarations
static const int cDefaultRingBufferDepth = 8;
static const long cDefaultIndexValue[] = {1, 2};
static const VARTYPE cVarType_imageIndex = VT_I4;

SV_IMPLEMENT_CLASS( RingBufferTool, RingBufferToolGuid );
#pragma endregion Declarations

#pragma region Constructor
RingBufferTool::RingBufferTool( SVObjectClass* pOwner, int stringResourceID )
	:SVToolClass( pOwner, stringResourceID )
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
bool RingBufferTool::CreateObject( const SVObjectLevelCreateStruct& rCreateStructure )
{
	bool bOk = SVToolClass::CreateObject( rCreateStructure );

	SVImageClass* pInputImage = getInputImage();
	bOk &= (nullptr != pInputImage);

	bOk &= (S_OK == m_svToolExtent.SetTranslation( SvDef::SVExtentTranslationNone ));
	SetToolROIExtentToFullInputImage ();

	m_BufferDepth.SetObjectAttributesAllowed( SvDef::SV_REMOTELY_SETABLE, SvOi::SetAttributeType::AddAttribute );
	if (bOk)
	{
		for (int i=0; i < SvDef::cRingBufferNumberOutputImages; i++)
		{
			bOk &= (S_OK == m_OutputImages[i].InitializeImage( pInputImage ));
			m_ImageIndexManager[i].SetObjectAttributesAllowed( SvDef::SV_REMOTELY_SETABLE, SvOi::SetAttributeType::AddAttribute );
		}
	}

	bOk &= (nullptr != GetTool());
	bOk &= (nullptr != GetInspection());

	m_SourceImageNames.setStatic( true );
	m_SourceImageNames.setSaveValueFlag(false);
	m_SourceImageNames.SetObjectAttributesAllowed( SvDef::SV_REMOTELY_SETABLE | SvDef::SV_SETABLE_ONLINE, SvOi::SetAttributeType::RemoveAttribute );

	// Override base class exposure of the draw flag
	// This value will not be exposed for the RingBuffer Tool.
	drawToolFlag.SetObjectAttributesAllowed( SvDef::SV_HIDDEN, SvOi::SetAttributeType::OverwriteAttribute );

	// Override base class exposure of the auxiliary extent variables
	// These values will not be exposed for the RingBuffer Tool.
	m_svUpdateAuxiliaryExtents.SetObjectAttributesAllowed( SvDef::SV_HIDDEN, SvOi::SetAttributeType::OverwriteAttribute );
	m_svAuxiliarySourceX.SetObjectAttributesAllowed( SvDef::SV_HIDDEN, SvOi::SetAttributeType::OverwriteAttribute );
	m_svAuxiliarySourceY.SetObjectAttributesAllowed( SvDef::SV_HIDDEN, SvOi::SetAttributeType::OverwriteAttribute );
	m_svAuxiliarySourceAngle.SetObjectAttributesAllowed( SvDef::SV_HIDDEN, SvOi::SetAttributeType::OverwriteAttribute );
	m_svAuxiliarySourceImageName.SetObjectAttributesAllowed( SvDef::SV_HIDDEN, SvOi::SetAttributeType::OverwriteAttribute );
	m_svAuxiliaryDrawType.SetObjectAttributesAllowed( SvDef::SV_HIDDEN, SvOi::SetAttributeType::OverwriteAttribute );

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

	// Check if the input object is still valid otherwise the pointer is invalid
	if (m_InputImageObjectInfo.IsConnected() && !m_InputImageObjectInfo.GetInputObjectInfo().CheckExistence())
	{
		m_InputImageObjectInfo.SetInputObject(nullptr);
	}

	SetToolROIExtentToFullInputImage ();

	long ringBufferDepth = 0;
	m_BufferDepth.GetValue(ringBufferDepth);
	if( SvDef::cRingBufferDepthMin > ringBufferDepth || SvDef::cRingBufferDepthMax < ringBufferDepth )
	{
		Result = false;
		if (nullptr != pErrorMessages)
		{
			SvDef::StringVector msgList;
			msgList.push_back(SvUl::Format("%d", SvDef::cRingBufferDepthMin));
			msgList.push_back(SvUl::Format("%d", SvDef::cRingBufferDepthMax));
			msgList.push_back(SvUl::Format("%d", ringBufferDepth));
			SvStl::MessageContainer message( SVMSG_SVO_61_RINGBUFFER_ERROR, SvStl::Tid_RingBuffer_Depth_Invalid_Value, msgList, SvStl::SourceFileParams(StdMessageParams), SvStl::Err_10013_RingBuffer_DepthValueInvalid, GetUniqueObjectID() );
			pErrorMessages->push_back(message);
		}
	}

	if (Result)
	{
		SVImageClass* pInputImage = getInputImage();
		if (nullptr != pInputImage)
		{
			SVImageInfoClass ImageInfo = pInputImage->GetImageInfo();

			for (int i = 0; i < SvDef::cRingBufferNumberOutputImages; i++)
			{
				m_OutputImages[i].UpdateImage(pInputImage->GetUniqueObjectID(), ImageInfo);
			}

			//Set input name to source image name to display it in result picker
			m_SourceImageNames.SetValue( pInputImage->GetCompleteName() );

			//create ring buffer images
			m_ringBuffer.clear();
			m_ringBuffer.resize(ringBufferDepth);
			m_isBufferFull = false;
			m_nextBufferPos = 0;

			ImageInfo.setDibBufferFlag(false);
			for (int i=0; i<ringBufferDepth; i++)
			{
				SvOi::SVImageBufferHandlePtr imageHandle;
				SVImageProcessingClass::CreateImageBuffer(ImageInfo, imageHandle);
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

SVImageClass* RingBufferTool::getInputImage(bool bRunMode /*= false*/)
{
	if (m_InputImageObjectInfo.IsConnected() && nullptr != m_InputImageObjectInfo.GetInputObjectInfo().getObject())
	{
		SVObjectClass* pObject = m_InputImageObjectInfo.GetInputObjectInfo().getObject();
		//! Use static_cast to avoid time penalty in run mode for dynamic_cast
		//! We are sure that when getObject() is not nullptr that it is the correct type
		return bRunMode ? static_cast<SVImageClass*> (pObject) : dynamic_cast<SVImageClass*> (pObject);
	}

	return nullptr;
}

SVImageClass* RingBufferTool::getOutputImage(int index)
{
	SVImageClass *retValue = nullptr;
	if (index >= 0 && index < SvDef::cRingBufferNumberOutputImages)
	{
		retValue = &m_OutputImages[index];
	}
	return retValue;
}
#pragma endregion Public Methods

#pragma region Protected Methods
bool RingBufferTool::isInputImage(const SVGUID& rImageGuid) const
{
	bool Result(false);
	//@Hack: [MZA][24.04.15] This method is only used by CollectOverlays and the recursive functionality of IsInputImage. 
	// Always false, optimizes the CollectOverlay (there is no overlay of this tool). 
	// In further implementation, it should be possible to return the right result and have nevertheless a optimized CollectOverlay method. 
	return Result;
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
		for (int i=0; i < SvDef::cRingBufferNumberOutputImages; ++i)
		{
			_variant_t depthVariant;
			m_ImageIndexManager[i].GetValue( depthVariant );
			if (cVarType_imageIndex == depthVariant.vt && 0 < depthVariant.lVal && ringBufferDepth >= depthVariant.lVal) //imageIndex invalid -> output image deactivated.
			{
				imageOutputFlag |= SetOutputImage(i, depthVariant.lVal, maxIndexPos, ringBufferDepth);
			}
		}

		//copy input image to ring buffer
		SvOi::SVImageBufferHandlePtr milHandleTo;
		SvOi::SVImageBufferHandlePtr inputImageBuffer;
		if( static_cast<int>(m_ringBuffer.size()) > m_nextBufferPos && nullptr != m_ringBuffer[m_nextBufferPos] )
		{
			milHandleTo = m_ringBuffer[m_nextBufferPos];
		}
		SVImageClass* pInputImage = getInputImage(true);
		if (nullptr != pInputImage)
		{
			pInputImage->GetImageHandle(inputImageBuffer);
		}
		if (nullptr != milHandleTo.get() && !milHandleTo->empty() && nullptr != inputImageBuffer.get() && !inputImageBuffer->empty())
		{
			SVMatroxBufferInterface::CopyBuffer( milHandleTo->GetBuffer(), inputImageBuffer->GetBuffer());
		}

		//calculate next image pos
		m_nextBufferPos++;
		if (ringBufferDepth <= m_nextBufferPos)
		{
			m_nextBufferPos=0;
			m_isBufferFull = true;
		}

		//set flag
		m_FlagOfOutputImage.SetValue(imageOutputFlag);
	}

	return returnValue;
}
#pragma endregion Protected Methods

#pragma region Private Methods
void RingBufferTool::LocalInitialize ()
{
	BuildInputObjectList ();
	BuildEmbeddedObjectList ();

	for (int i=0; i<SvDef::cRingBufferNumberOutputImages; i++)
	{
		// The output image is referenced in the embedded list.
		m_OutputImages[i].InitializeImage( SvDef::SVImageTypeEnum::SVImageTypePhysical );
	}

	// Set up your type... in this case this will reference that this tool is a 
	// Resize Tool.
	m_outObjectInfo.m_ObjectTypeInfo.ObjectType = SvDef::SVToolObjectType;
	m_outObjectInfo.m_ObjectTypeInfo.SubType    = SvDef::SVRingBufferToolObjectType;

	m_ringBuffer.clear();
}

void RingBufferTool::BuildInputObjectList ()
{
	// Source Image
	m_InputImageObjectInfo.SetInputObjectType( SvDef::SVImageObjectType, SvDef::SVImageMonoType);
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

	int RingbufferIndexNames[SvDef::cRingBufferNumberOutputImages] = { IDS_OBJECTNAME_RINGBUFFER_INDEX1, IDS_OBJECTNAME_RINGBUFFER_INDEX2};
	int ImageNames[SvDef::cRingBufferNumberOutputImages] = { IDS_OBJECTNAME_IMAGE1, IDS_OBJECTNAME_IMAGE2};
	for( int i = 0; i < SvDef::cRingBufferNumberOutputImages; i++)
	{
		RegisterEmbeddedObject( &m_ImageIndexManager[i], RingBuffer_IndexGuid[i], RingbufferIndexNames[i], true, SvOi::SVResetItemTool );
		_variant_t vtTemp;
		::VariantInit(&vtTemp);
		vtTemp.vt = cVarType_imageIndex;
		vtTemp.lVal = cDefaultIndexValue[i];
		m_ImageIndexManager[i].SetDefaultValue( vtTemp, true );

		std::string ObjectName = SvUl::LoadStdString( RingbufferIndexNames[i] );
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
		if (static_cast<int>(m_ringBuffer.size()) > pos && nullptr != m_ringBuffer[pos])
		{
			SvOi::SVImageBufferHandlePtr outputImageBuffer;
			m_OutputImages[outputIndex].GetImageHandle(outputImageBuffer);
			if (nullptr != outputImageBuffer && !outputImageBuffer->empty() && !m_ringBuffer[pos]->empty())
			{
				HRESULT statusCode = SVMatroxBufferInterface::CopyBuffer(outputImageBuffer->GetBuffer(), m_ringBuffer[pos]->GetBuffer());
				if (S_OK == statusCode)
				{
					retValue = 1 << outputIndex;
				}
			}
		}
	}
	return retValue;
}

void RingBufferTool::SetToolROIExtentToFullInputImage ()
{
	SVImageClass* pInputImage = getInputImage ();

	if (nullptr != pInputImage)
	{
		SVImageExtentClass inputImageExtents = pInputImage->GetImageExtents ();

		double inputHeight( 0.0 );
		double inputWidth( 0.0 );
		inputImageExtents.GetExtentProperty (SvDef::SVExtentPropertyHeight, inputHeight);
		inputImageExtents.GetExtentProperty (SvDef::SVExtentPropertyWidth, inputWidth);

		m_svToolExtent.SetExtentValue (SvDef::SVExtentPropertyHeight, inputHeight);
		m_svToolExtent.SetExtentValue (SvDef::SVExtentPropertyWidth, inputWidth);
		m_svToolExtent.SetExtentValue (SvDef::SVExtentPropertyPositionPointX, 0.0);
		m_svToolExtent.SetExtentValue (SvDef::SVExtentPropertyPositionPointY, 0.0);
	}
}
#pragma endregion Private Methods
