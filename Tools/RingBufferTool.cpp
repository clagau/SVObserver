//*****************************************************************************
/// \copyright (c) 2015 by K�rber Pharma Inspection GmbH. All Rights Reserved
/// All Rights Reserved 
/// \Author	Marc Ziegler
//*****************************************************************************
/// Class of the RingBuffer-tool
//*****************************************************************************
#pragma region Includes
#include "StdAfx.h"
#include "RingBufferTool.h"
#include "Definitions/StringTypeDef.h"
#include "Definitions/TextDefinesSvDef.h"
#include "InspectionEngine/SVImageProcessingClass.h"
#include "SVMatroxLibrary/SVMatroxBufferInterface.h"
#include "SVStatusLibrary/MessageManager.h"
#include "InspectionEngine/RunStatus.h"
#include "SVStatusLibrary/SvimState.h"
#include "ObjectInterfaces/ITriggerRecordControllerRW.h"
#include "SVMatroxLibrary/SVMatroxBufferCreateStruct.h"
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

SV_IMPLEMENT_CLASS( RingBufferTool, SvPb::RingBufferToolClassId );

#pragma region Constructor
RingBufferTool::RingBufferTool( SVObjectClass* pOwner, int stringResourceID )
	:SVToolClass(ToolExtType::None, pOwner, stringResourceID )
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

	SvIe::SVImageClass* pInputImage = m_InputImage.getInput<SvIe::SVImageClass>();

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
	m_SourceImageNames.SetObjectAttributesAllowed( SvPb::remotelySetable, SvOi::SetAttributeType::RemoveAttribute );

	// These values will not be exposed for the this Tool.
	m_drawToolFlag.SetObjectAttributesAllowed(SvPb::noAttributes, SvOi::SetAttributeType::OverwriteAttribute);

	m_isCreated = bOk;

	m_nextBufferPos = -1;

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

	m_InputImage.validateInput();

	SetToolROIExtentToFullInputImage ();

	_variant_t depthVariant;
	m_BufferDepth.GetValue(depthVariant);
	if (cVarType_imageIndex != depthVariant.vt || SvDef::cRingBufferDepthMin > depthVariant.lVal || SvDef::cRingBufferDepthMax < depthVariant.lVal)
	{
		Result = false;
		if (nullptr != pErrorMessages)
		{
			SvDef::StringVector msgList;
			msgList.push_back(std::format("{}", SvDef::cRingBufferDepthMin));
			msgList.push_back(std::format("{}", SvDef::cRingBufferDepthMax));
			msgList.push_back(std::format("{}", depthVariant.lVal));
			SvStl::MessageContainer message( SVMSG_SVO_61_RINGBUFFER_ERROR, SvStl::Tid_RingBuffer_Depth_Invalid_Value, msgList, SvStl::SourceFileParams(StdMessageParams), getObjectId() );
			pErrorMessages->push_back(message);
		}
	}
	else
	{
		m_ringBufferDepth = depthVariant.lVal;
	}

	if (Result)
	{
		SvIe::SVImageClass* pInputImage = m_InputImage.getInput<SvIe::SVImageClass>();
		if (nullptr != pInputImage)
		{
			SVImageInfoClass ImageInfo = pInputImage->GetImageInfo();

			for (int i = 0; i < SvDef::cRingBufferNumberOutputImages; i++)
			{
				m_OutputImages[i].UpdateImageSetParentAndImageInfo(pInputImage->getObjectId(), ImageInfo);
			}

			//Set input name to source image name to display it in result picker
			m_SourceImageNames.SetValue( pInputImage->GetCompleteName() );

			//create ring buffer images
			SVMatroxBufferCreateStruct bufferStruct;
			HRESULT hrOk = SvIe::SVImageProcessingClass::FillBufferStructFromInfo(ImageInfo, bufferStruct);
			if (S_OK == hrOk)
			{
				try
				{
					SvOi::getTriggerRecordControllerRWInstanceThrow().addImageBuffer(getObjectId(), bufferStruct, m_ringBufferDepth);
				}
				catch (const SvStl::MessageContainer& rExp)
				{
					Result = false;
					if (nullptr != pErrorMessages)
					{
						pErrorMessages->push_back(rExp);
					}
				}
				
			}
			else
			{
				Result = false;
				if (nullptr != pErrorMessages)
				{ 
					SvStl::MessageContainer Msg(SVMSG_SVO_92_GENERAL_ERROR, SvStl::Tid_InitImageFailed, SvStl::SourceFileParams(StdMessageParams), getObjectId());
					pErrorMessages->push_back(Msg);
				}
			}
			m_ringBuffer.clear();
			m_ringBuffer.resize(m_ringBufferDepth);
			m_isBufferFull = false;
			BOOL freezeFlag(false);
			m_editFreezeFlag.GetValue(freezeFlag);
			if (false == SvimState::CheckState(SV_STATE_REGRESSION) && FALSE == freezeFlag)
			{
				m_nextBufferPos = -1;
			}
			else
			{
				m_nextBufferPos = 0;
			}
		}
		else
		{
			Result = false;
			if (nullptr != pErrorMessages)
			{
				SvStl::MessageContainer Msg( SVMSG_SVO_92_GENERAL_ERROR, SvStl::Tid_NoSourceImage, SvStl::SourceFileParams(StdMessageParams), getObjectId() );
				pErrorMessages->push_back(Msg);
			}
		}
	}

	return Result;
}

std::vector<std::string> RingBufferTool::getToolAdjustNameList() const
{
	constexpr std::array<LPCTSTR, 4> cToolAdjustNameList
	{
		_T("Image Source"),
		_T("Parameters"),
		_T("Conditional"),
		_T("Comment"),
	};
	return { cToolAdjustNameList.begin(), cToolAdjustNameList.end() };
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
bool RingBufferTool::isInputImage(uint32_t) const
{
	bool Result(false);
	//@Hack: [MZA][24.04.15] This method is only used by getOverlays and the recursive functionality of IsInputImage. 
	// Always false, optimizes the CollectOverlay (there is no overlay of this tool). 
	// In further implementation, it should be possible to return the right result and have nevertheless a optimized CollectOverlay method. 
	return Result;
}	

bool RingBufferTool::onRun( SvIe::RunStatus& rRunStatus, SvStl::MessageContainerVector *pErrorMessages )
{
	bool returnValue = SVToolClass::onRun( rRunStatus, pErrorMessages );
	if (returnValue)
	{
		//after reset, the first run is an init run, do not use this image
		if (m_nextBufferPos == -1)
		{
			m_nextBufferPos = 0;
			return true;
		}

		//-----	Execute this objects run functionality. -----------------------------
		int imageOutputFlag = 0;
		int maxIndexPos = m_ringBufferDepth -1;
		if (!m_isBufferFull)
		{
			maxIndexPos = m_nextBufferPos -1;
		}

		//set output image
		for (int i=0; i < SvDef::cRingBufferNumberOutputImages; ++i)
		{
			_variant_t indexVariant;
			m_ImageIndexManager[i].GetValue( indexVariant );
			if (cVarType_imageIndex == indexVariant.vt && 0 < indexVariant.lVal && m_ringBufferDepth >= indexVariant.lVal) //imageIndex invalid -> output image deactivated.
			{
				imageOutputFlag |= SetOutputImage(i, indexVariant.lVal, maxIndexPos, m_ringBufferDepth, rRunStatus.m_triggerRecord, pErrorMessages);
			}
		}

		//copy input image to ring buffer
		SvIe::SVImageClass* pInputImage = m_InputImage.getInput<SvIe::SVImageClass>(true);
		if (nullptr != pInputImage && static_cast<int>(m_ringBuffer.size()) > m_nextBufferPos)
		{
			SvOi::ITRCImagePtr pImageBuffer = nullptr;
			if (SvIe::SVImageClass::BufferType::TRCBuffer == pInputImage->getBufferType())
			{
				pImageBuffer = pInputImage->getImageReadOnly(rRunStatus.m_triggerRecord.get(), true);
			}
			else
			{
				auto pInputImageBuffer = pInputImage->getImageReadOnly(rRunStatus.m_triggerRecord.get());
				pImageBuffer = m_OutputImages->getTempImageBuffer(false);
				if (nullptr != pImageBuffer && !pImageBuffer->isEmpty() && nullptr != pInputImageBuffer && !pInputImageBuffer->isEmpty())
				{
					SVMatroxBufferInterface::CopyBuffer(pImageBuffer->getHandle()->GetBuffer(), pInputImageBuffer->getHandle()->GetBuffer());
				}
			}
			m_ringBuffer[m_nextBufferPos] = pImageBuffer;
		}

		//calculate next image pos
		m_nextBufferPos++;
		if (m_ringBufferDepth <= m_nextBufferPos)
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
		m_OutputImages[i].InitializeImage( SvPb::SVImageTypeEnum::SVImageTypePhysical );
	}

	// Set up your type... in this case this will reference that this tool is a 
	// Resize Tool.
	m_ObjectTypeInfo.m_ObjectType = SvPb::SVToolObjectType;
	m_ObjectTypeInfo.m_SubType    = SvPb::SVRingBufferToolObjectType;
	
	
	m_ringBuffer.clear();
}

void RingBufferTool::BuildInputObjectList ()
{
	// Source Image (SVNotSetSubObjectType means both mono and color type)
	m_InputImage.SetInputObjectType(SvPb::SVImageObjectType, SvPb::SVNotSetSubObjectType);
	registerInputObject( &m_InputImage, SvDef::SourceImageInputName, SvPb::ImageInputEId);
	m_InputImage.setAllowedMode(SvOi::InputAllowedMode::IsBeforeTool);
}


void RingBufferTool::BuildEmbeddedObjectList ()
{
	RegisterEmbeddedObject( &m_SourceImageNames, SvPb::SourceImageNamesEId, IDS_OBJECTNAME_SOURCE_IMAGE_NAMES, false, SvOi::SVResetItemToolAndDependent, false );
	
	RegisterEmbeddedObject( &m_BufferDepth, SvPb::RingBuffer_DepthEId, IDS_OBJECTNAME_RINGBUFFER_DEPTH, true, SvOi::SVResetItemToolAndDependent, true);
	_variant_t vtTemp;
	::VariantInit(&vtTemp);
	vtTemp.vt = cVarType_imageIndex;
	vtTemp.lVal = cDefaultRingBufferDepth;
	m_BufferDepth.SetDefaultValue(vtTemp, true);

	RegisterEmbeddedObject( &m_FlagOfOutputImage, SvPb::RingBuffer_FlagOfOutputImagesEId, IDS_OBJECTNAME_RINGBUFFER_FLAG, false, SvOi::SVResetItemNone, false );
	m_FlagOfOutputImage.SetObjectAttributesAllowed(SvPb::audittrail, SvOi::SetAttributeType::RemoveAttribute);
	m_FlagOfOutputImage.SetDefaultValue( 0, true );
	m_FlagOfOutputImage.setSaveValueFlag(false);

	int RingbufferIndexNames[SvDef::cRingBufferNumberOutputImages] = { IDS_OBJECTNAME_RINGBUFFER_INDEX1, IDS_OBJECTNAME_RINGBUFFER_INDEX2};
	int ImageNames[SvDef::cRingBufferNumberOutputImages] = { IDS_OBJECTNAME_IMAGE1, IDS_OBJECTNAME_IMAGE2};
	for( int i = 0; i < SvDef::cRingBufferNumberOutputImages; i++)
	{
		RegisterEmbeddedObject( &m_ImageIndexManager[i], SvPb::RingBufferIndexEId+i, RingbufferIndexNames[i], true, SvOi::SVResetItemToolAndDependent, true);
		::VariantInit(&vtTemp);
		vtTemp.vt = cVarType_imageIndex;
		vtTemp.lVal = cDefaultIndexValue[i];
		m_ImageIndexManager[i].SetDefaultValue( vtTemp, true );

		RegisterEmbeddedImage(&m_OutputImages[i], SvPb::OutputImageEId+i, ImageNames[i]);
	}
}

int RingBufferTool::SetOutputImage( int outputIndex, int imageIndex, int maxIndexPos, long ringBufferDepth, const SvOi::ITriggerRecordRWPtr& pTriggerRecord, SvStl::MessageContainerVector *pErrorMessages)
{
	int retValue = 0;
	int pos = (m_nextBufferPos + imageIndex - 1)%ringBufferDepth;
	
	if ( 0 <= pos && maxIndexPos >= pos)
	{
		if (static_cast<int>(m_ringBuffer.size()) > pos && nullptr != m_ringBuffer[pos] && !m_ringBuffer[pos]->isEmpty())
		{
			try
			{
				m_OutputImages[outputIndex].setImage(m_ringBuffer[pos], pTriggerRecord);
				retValue = 1 << outputIndex;
			}
			catch (const SvStl::MessageContainer& rExp)
			{
				SvStl::MessageManager  message(SvStl::MsgType::Log);
				message.setMessage(rExp.getMessage());
				if (nullptr != pErrorMessages)
				{
					pErrorMessages->push_back(message.getMessageContainer());
				}
			}
		}
	}
	return retValue;
}

void RingBufferTool::SetToolROIExtentToFullInputImage ()
{
	SvIe::SVImageClass* pInputImage = m_InputImage.getInput<SvIe::SVImageClass>();

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
