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

	bOk &= (m_svToolExtent.SetTranslation( SVExtentTranslationNone ) == S_OK);
	SetToolROIExtentToFullInputImage ();

	m_BufferDepth.ObjectAttributesAllowedRef() = SV_REMOTELY_SETABLE | SV_VIEWABLE | SV_PRINTABLE | SV_SELECTABLE_FOR_EQUATION;
	if (bOk)
	{
		for (int i=0; i < m_numberOfOutputImages; i++)
		{
			bOk &= (m_OutputImages[i].InitializeImage( inputImage ) == S_OK );
			m_ImageIndexManagers[i].init(GetInspection());
			m_ImageIndexManagers[i].SetAttributes(SV_REMOTELY_SETABLE | SV_VIEWABLE | SV_PRINTABLE | SV_SELECTABLE_FOR_EQUATION, true);
		}
	}

	bOk &= (nullptr != GetTool());
	bOk &= (nullptr != GetInspection());

	m_svSourceImageName.ObjectAttributesAllowedRef() &=~SV_REMOTELY_SETABLE & ~SV_SETABLE_ONLINE;

	isCreated = bOk;

	return bOk;
}

bool RingBufferTool::DoesObjectHaveExtents() const
{
	return false;
}

HRESULT RingBufferTool::GetInputImageNames( SVStringValueObjectClass*& p_pSourceNames )
{
	p_pSourceNames = &m_svSourceImageName;
	return S_OK;
}

HRESULT RingBufferTool::ResetObject()
{
	HRESULT status = SVToolClass::ResetObject();

	SetToolROIExtentToFullInputImage ();

	if (OnValidate())
	{
		for (int i=0; i < m_numberOfOutputImages; i++)
		{
			if (S_OK == status)
			{
				status = m_ImageIndexManagers[i].ResetObject();
			}
		}

		SVImageClass* inputImage = getInputImage ();
		if (nullptr != inputImage)
		{
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
	if (ringBufferDepth < m_minRingBufferDepth || ringBufferDepth > m_maxRingBufferDepth)
	{
		bValid = false;
		SvStl::MessageMgrNoDisplay Exception( SvStl::LogOnly );
		CString strText;
		strText.Format(SvO::RingBuffer_Depth_Invalid_Value, m_minRingBufferDepth, m_maxRingBufferDepth, ringBufferDepth);
		Exception.setMessage( SVMSG_SVO_61_RINGBUFFER_ONVALIDATE_ERROR, strText, StdMessageParams, SvOi::Err_10013_RingBuffer_DepthValueInvalid );
	}
	for (int i=0; i< m_numberOfOutputImages; ++i)
	{
		bValid &= m_ImageIndexManagers[i].IsValid();
	}	

	return SVToolClass::OnValidate() && bValid ;
}

void RingBufferTool::ResetPrivateInputInterface()
{
	const SVObjectInfoStruct& objectInfo = GetObjectInfo();
	for (int i=0; i < m_numberOfOutputImages; i++)
	{
		m_ImageIndexManagers[i].UpdateTaskInfo(objectInfo);
	}	

	SVToolClass::ResetPrivateInputInterface();
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
	if (index >= 0 && index < m_numberOfOutputImages)
	{
		retValue = &m_OutputImages[index];
	}
	return retValue;
}

#pragma region virtual methods (IRingBufferTool) 
HRESULT RingBufferTool::setRingDepth(SVString value)
{
	HRESULT retValue = S_OK;
	long depth = 0;
	bool isNumber = value.Convert2Number(depth, true);
	
	if (isNumber && m_minRingBufferDepth <= depth && m_maxRingBufferDepth >= depth)
	{
		retValue = m_taskObjectValueInterface.AddInputRequest( &m_BufferDepth, depth );
		if (S_OK == retValue)
		{
			retValue = m_taskObjectValueInterface.AddInputRequestMarker();
		}
	}
	else
	{
		retValue = SvOi::Err_10018_RingBuffer_InvalidRingDepth;
	}
	return retValue;
}

SVString RingBufferTool::getRingBufferDepthString() const
{
	SVString retValue = "";
	long depth = 0;
	HRESULT hValue = m_BufferDepth.GetValue(depth);
	if (S_OK == hValue)
	{
		retValue.Format("%d", depth);
	}

	return retValue;
}

HRESULT RingBufferTool::setImageIndex(int indexNumber, SVString valueString)
{
	HRESULT retValue = S_OK;
	if (0 <= indexNumber && m_numberOfOutputImages > indexNumber)
	{
		return m_ImageIndexManagers[indexNumber].SetInputValue(valueString);
	}
	else
	{
		retValue = SvOi::Err_10019_RingBuffer_InvalidImageIndex;
	}

	return retValue;
}

SVString RingBufferTool::getImageIndex(int indexNumber) const 
{
	SVString retValue = "";
	long index = 0;

	if (0 <= indexNumber && m_numberOfOutputImages > indexNumber)
	{
		retValue = m_ImageIndexManagers[indexNumber].GetInputValue();
	}

	return retValue;
}
#pragma endregion virtual methods (IRingBufferTool) 
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
		for (int i=0; i < m_numberOfOutputImages; ++i)
		{
			_variant_t depthVariant = m_ImageIndexManagers[i].GetValue();
			if (m_varType_imageIndex == depthVariant.vt && 0 < depthVariant.lVal && ringBufferDepth >= depthVariant.lVal) //imageIndex invalid -> output image deactivated.
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
			//Set input name to source image name to display it in result picker
			m_svSourceImageName.SetValue( RRunStatus.m_lResultDataIndex, 0, inputImage->GetCompleteObjectName() );

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

	for (int i=0; i<m_numberOfOutputImages; i++)
	{
		// The output image is referenced in the embedded list.
		m_OutputImages[i].InitializeImage( SVImageTypePhysical );
	}

	// Set up your type... in this case this will reference that this tool is a 
	// Resize Tool.
	outObjectInfo.ObjectTypeInfo.ObjectType = SVToolObjectType;
	outObjectInfo.ObjectTypeInfo.SubType    = SVRingBufferToolObjectType;

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
	RegisterEmbeddedObject( &m_svSourceImageName, 
							SVSourceImageNamesGuid, 
							IDS_OBJECTNAME_SOURCE_IMAGE_NAMES, 
							false, 
							SVResetItemTool );
	
	RegisterEmbeddedObject( &m_BufferDepth, 
		RingBuffer_DepthGuid, 
		IDS_OBJECTNAME_RINGBUFFER_DEPTH, 
		true, 
		SVResetItemTool );
	m_BufferDepth.SetDefaultValue( m_defaultRingBufferDepth, TRUE );

	m_ImageIndexManagers[0].RegisterObjects(*this, SvO::RingBuffer_ImageIndex1Connector, RingBuffer_Index1Guid, IDS_OBJECTNAME_RINGBUFFER_INDEX1, true, SVResetItemTool);
	_variant_t vtTemp;
	::VariantInit(&vtTemp);
	vtTemp.vt = m_varType_imageIndex;
	vtTemp.lVal = m_defaultIndex1Value;
	m_ImageIndexManagers[0].setDefaultValue( vtTemp, true );

	m_ImageIndexManagers[1].RegisterObjects(*this, SvO::RingBuffer_ImageIndex2Connector, RingBuffer_Index2Guid, IDS_OBJECTNAME_RINGBUFFER_INDEX2, true, SVResetItemTool);
	::VariantInit(&vtTemp);
	vtTemp.vt = m_varType_imageIndex;
	vtTemp.lVal = m_defaultIndex2Value;
	m_ImageIndexManagers[1].setDefaultValue( vtTemp, true );

	RegisterEmbeddedObject( &m_FlagOfOutputImage, 
							RingBuffer_FlagOfOutputImagesGuid, 
							IDS_OBJECTNAME_RINGBUFFER_FLAG, 
							false, 
							SVResetItemNone );
	m_FlagOfOutputImage.SetDefaultValue( 0, TRUE );

	int l_pImageNames[] = { IDS_OBJECTNAME_IMAGE1, IDS_OBJECTNAME_IMAGE2};
	for( int i = 0; i < m_numberOfOutputImages; i++)
	{
		RegisterEmbeddedObject( &m_OutputImages[i], aSVVariantResultImageObjectGuid[i], l_pImageNames[i] );
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

			for (int i=0; i < m_numberOfOutputImages; i++)
			{
				m_ImageIndexManagers[i].renameToolSetSymbol( pObject, OriginalName );
			}
			dwResult = SVMR_SUCCESS;
			break;
		}
	case SVMSGID_DISCONNECT_OBJECT_INPUT:
		{
			SVInObjectInfoStruct* pInInfo = reinterpret_cast <SVInObjectInfoStruct*> (DwMessageValue);
			SVObjectClass* pObject = pInInfo->GetInputObjectInfo().PObject;

			for (int i=0; i < m_numberOfOutputImages; i++)
			{
				m_ImageIndexManagers[i].DisconnectObject(pObject);
			}
			dwResult = SVMR_SUCCESS;
			break;
		}
	}

	dwResult = SVToolClass::processMessage( DwMessageID, DwMessageValue, DwMessageContext ) | dwResult;
	return dwResult;
}
#pragma endregion Private Methods
