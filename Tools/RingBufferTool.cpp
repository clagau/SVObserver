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
#include "SVObjectLibrary/SVClsIds.h"
#include "InspectionEngine/SVImageProcessingClass.h"
#include "SVImageLibrary/SVImageBufferHandleImage.h"
#include "SVStatusLibrary/ErrorNumbers.h"
#include "SVStatusLibrary/MessageManager.h"
#include "Definitions/StringTypeDef.h"
#include "Definitions/TextDefineSvDef.h"
#include "SVUtilityLibrary/StringHelper.h"
#pragma endregion Includes

namespace SvTo
{
#pragma region Declarations
#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

constexpr int cDefaultRingBufferDepth = 8;
constexpr long cDefaultIndexValue[] = {1, 2};
constexpr VARTYPE cVarType_imageIndex = VT_I4;
#pragma endregion Declarations

SV_IMPLEMENT_CLASS( RingBufferTool, RingBufferToolGuid );

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

	SvIe::SVImageClass* pInputImage = SvOl::getInput<SvIe::SVImageClass>(m_InputImageObjectInfo);
	bOk &= (nullptr != pInputImage);

	m_toolExtent.SetTranslation(SvPb::SVExtentTranslationNone);
	SetToolROIExtentToFullInputImage ();

	m_BufferDepth.SetObjectAttributesAllowed( SvPb::remotelySetable, SvOi::SetAttributeType::AddAttribute );
	if (bOk)
	{
		for (int i=0; i < SvDef::cRingBufferNumberOutputImages; i++)
		{
			bOk &= (S_OK == m_OutputImages[i].InitializeImage( pInputImage ));
			m_ImageIndexManager[i].SetObjectAttributesAllowed( SvPb::remotelySetable, SvOi::SetAttributeType::AddAttribute );
		}
	}

	bOk &= (nullptr != GetTool());
	bOk &= (nullptr != GetInspection());

	m_SourceImageNames.setSaveValueFlag(false);
	m_SourceImageNames.SetObjectAttributesAllowed( SvPb::remotelySetable | SvPb::setableOnline, SvOi::SetAttributeType::RemoveAttribute );

	// These values will not be exposed for the this Tool.
	constexpr UINT cAttribute {SvDef::selectableAttributes | SvPb::printable};
	m_drawToolFlag.SetObjectAttributesAllowed(cAttribute, SvOi::SetAttributeType::RemoveAttribute);

	m_isCreated = bOk;

	return bOk;
}

bool RingBufferTool::DoesObjectHaveExtents() const
{
	return false;
}

SvVol::SVStringValueObjectClass* RingBufferTool::GetInputImageNames()
{
	return &m_SourceImageNames;
}

bool RingBufferTool::ResetObject(SvStl::MessageContainerVector *pErrorMessages)
{
	bool Result = SVToolClass::ResetObject(pErrorMessages);

	SvOl::ValidateInput(m_InputImageObjectInfo);

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
		SvIe::SVImageClass* pInputImage = SvOl::getInput<SvIe::SVImageClass>(m_InputImageObjectInfo);
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
				SvIe::SVImageProcessingClass::CreateImageBuffer(ImageInfo, imageHandle);
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

SvIe::SVImageClass* RingBufferTool::getOutputImage(int index)
{
	SvIe::SVImageClass *retValue = nullptr;
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
				imageOutputFlag |= SetOutputImage(i, depthVariant.lVal, maxIndexPos, ringBufferDepth, rRunStatus.m_triggerRecord);
			}
		}

		//copy input image to ring buffer
		SvOi::SVImageBufferHandlePtr milHandleTo;
		if( static_cast<int>(m_ringBuffer.size()) > m_nextBufferPos && nullptr != m_ringBuffer[m_nextBufferPos] )
		{
			milHandleTo = m_ringBuffer[m_nextBufferPos];
		}
		SvIe::SVImageClass* pInputImage = SvOl::getInput<SvIe::SVImageClass>(m_InputImageObjectInfo, true);
		SvTrc::IImagePtr pInputImageBuffer;
		if (nullptr != pInputImage)
		{
			pInputImageBuffer = pInputImage->getImageReadOnly(rRunStatus.m_triggerRecord.get());
		}
		if (nullptr != milHandleTo.get() && !milHandleTo->empty() && nullptr != pInputImageBuffer && !pInputImageBuffer->isEmpty())
		{
			SVMatroxBufferInterface::CopyBuffer( milHandleTo->GetBuffer(), pInputImageBuffer->getHandle()->GetBuffer());
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
	// Override base class exposure of the auxiliary extent variables
	// These values will not be exposed for this Tool.
	m_hasToolExtents = false;

	BuildInputObjectList ();
	BuildEmbeddedObjectList ();

	for (int i=0; i<SvDef::cRingBufferNumberOutputImages; i++)
	{
		// The output image is referenced in the embedded list.
		m_OutputImages[i].InitializeImage( SvDef::SVImageTypeEnum::SVImageTypePhysical );
	}

	// Set up your type... in this case this will reference that this tool is a 
	// Resize Tool.
	m_outObjectInfo.m_ObjectTypeInfo.ObjectType = SvPb::SVToolObjectType;
	m_outObjectInfo.m_ObjectTypeInfo.SubType    = SvPb::SVRingBufferToolObjectType;

	m_ringBuffer.clear();
}

void RingBufferTool::BuildInputObjectList ()
{
	// Source Image
	m_InputImageObjectInfo.SetInputObjectType(SvPb::SVImageObjectType, SvPb::SVImageMonoType);
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
	m_FlagOfOutputImage.SetObjectAttributesAllowed(SvPb::printable, SvOi::SetAttributeType::RemoveAttribute);
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
		ObjectName +=  SvDef::cLinkName;
		RegisterEmbeddedObject( &m_ImageIndexManager[i].getLinkedName(), RingBufferLink_IndexGuid[i], ObjectName.c_str(), false, SvOi::SVResetItemNone );
		m_ImageIndexManager[i].getLinkedName().SetDefaultValue( _T(""), false );

		RegisterEmbeddedObject( &m_OutputImages[i], aSVVariantResultImageObjectGuid[i], ImageNames[i] );
	}
}

int RingBufferTool::SetOutputImage( int outputIndex, int imageIndex, int maxIndexPos, long ringBufferDepth, const SvTrc::ITriggerRecordRWPtr& pTriggerRecord)
{
	int retValue = 0;
	int pos = (m_nextBufferPos + imageIndex - 1)%ringBufferDepth;
	
	if ( 0 <= pos && maxIndexPos >= pos)
	{
		if (static_cast<int>(m_ringBuffer.size()) > pos && nullptr != m_ringBuffer[pos])
		{
			SvTrc::IImagePtr image = m_OutputImages[outputIndex].getImageToWrite(pTriggerRecord);
			if (nullptr != image && !image->isEmpty())
			{
				HRESULT statusCode = SVMatroxBufferInterface::CopyBuffer(image->getHandle()->GetBuffer(), m_ringBuffer[pos]->GetBuffer());
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
	SvIe::SVImageClass* pInputImage = SvOl::getInput<SvIe::SVImageClass>(m_InputImageObjectInfo);

	if (nullptr != pInputImage)
	{
		const SVImageExtentClass& rImageExtents = pInputImage->GetImageExtents ();

		double inputHeight( 0.0 );
		double inputWidth( 0.0 );
		rImageExtents.GetExtentProperty(SvPb::SVExtentPropertyHeight, inputHeight);
		rImageExtents.GetExtentProperty(SvPb::SVExtentPropertyWidth, inputWidth);

		m_toolExtent.SetExtentValue(SvPb::SVExtentPropertyHeight, inputHeight);
		m_toolExtent.SetExtentValue(SvPb::SVExtentPropertyWidth, inputWidth);
		m_toolExtent.SetExtentValue(SvPb::SVExtentPropertyPositionPointX, 0.0);
		m_toolExtent.SetExtentValue(SvPb::SVExtentPropertyPositionPointY, 0.0);
	}
}
#pragma endregion Private Methods

} //namespace SvTo
