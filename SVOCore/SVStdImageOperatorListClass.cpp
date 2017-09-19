//******************************************************************************
/// COPYRIGHT (c) 2016 by Seidenader Maschinenbau GmbH
/// All Rights Reserved
//******************************************************************************
/// Class for the StdImageOperator List
//******************************************************************************

#pragma region Includes
#include "stdafx.h"
#include "SVStdImageOperatorListClass.h"

#include "SVImageLibrary/SVImageBufferHandleImage.h"
#include "SVObjectLibrary/SVObjectLevelCreateStruct.h"

#include "SVImageClass.h"
#include "SVImageProcessingClass.h"
#include "SVUnaryImageOperatorClass.h"
#include "ObjectInterfaces/ITool.h"
#include "SVObjectLibrary/SVClsids.h"
#include "SVUtilityLibrary/SVString.h"
#pragma endregion Includes

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

SV_IMPLEMENT_CLASS( SVStdImageOperatorListClass, SVStdImageOperatorListClassGuid )

SVStdImageOperatorListClass::SVStdImageOperatorListClass( SVObjectClass* POwner, int StringResourceID )
							:SVUnaryImageOperatorListClass( POwner, StringResourceID ) 
{
	init();
}

void SVStdImageOperatorListClass::init()
{
	m_bUseOverlays = false;

	// Identify our output type
	m_outObjectInfo.m_ObjectTypeInfo.ObjectType = SVUnaryImageOperatorListObjectType;
	m_outObjectInfo.m_ObjectTypeInfo.SubType	= SVStdImageOperatorListObjectType;

	// SetObjectDepth() already called in SVObjectClass Ctor

	// Register Embedded Objects
	RegisterEmbeddedObject( &outputImageObject, SVOutputImageObjectGuid, IDS_OBJECTNAME_IMAGE1 );

	// Set Embedded defaults
	outputImageObject.InitializeImage( SvOi::SVImageTypeEnum::SVImageTypeLogicalAndPhysical );

	// Set default inputs and outputs
	addDefaultInputObjects();
}

SVStdImageOperatorListClass::~SVStdImageOperatorListClass() 
{ 
	CloseObject();
}

bool SVStdImageOperatorListClass::CreateObject( const SVObjectLevelCreateStruct& rCreateStructure )
{
	// Image input must already exist, and must be created!!!

	// Embedded Image output must already exist!!!
	m_isCreated = __super::CreateObject(rCreateStructure) && ( S_OK == outputImageObject.InitializeImage( getInputImage() ) );

	return m_isCreated;
}

bool SVStdImageOperatorListClass::CloseObject()
{
	bool bRetVal = true;
	
	if( getOutputImage() )
	{
		bRetVal = getOutputImage()->CloseObject() && bRetVal;
	}

	if ( m_isCreated )
	{
		m_isCreated = false;
		
		bRetVal = __super::CloseObject() && bRetVal;
	}

	return bRetVal;
}

bool SVStdImageOperatorListClass::ResetObject(SvStl::MessageContainerVector *pErrorMessages)
{
	bool Result = true;
	if (outputImageObject.InitializeImage( getInputImage() ))
	{
		Result = false;
		if (nullptr != pErrorMessages)
		{
			SvStl::MessageContainer Msg( SVMSG_SVO_92_GENERAL_ERROR, SvStl::Tid_InitImageFailed, SvStl::SourceFileParams(StdMessageParams), 0, GetUniqueObjectID() );
			pErrorMessages->push_back(Msg);
		}
	}

	Result = __super::ResetObject(pErrorMessages) && Result;

	CollectInputImageNames();

	//create tmp mil buffer for operator
	SVImageClass* pImage = getOutputImage();
	if (pImage)
	{
		SVImageInfoClass imageInfo = pImage->GetImageInfo();
		imageInfo.setDibBufferFlag(false);
		SVImageProcessingClass::CreateImageBuffer(imageInfo, m_milTmpImageObjectInfo1);
		SVImageProcessingClass::CreateImageBuffer(imageInfo, m_milTmpImageObjectInfo2);
	}

	return Result;
}

////////////////////////////////////////////////////////////////////////////////
// .Title       : getOutputImage
// -----------------------------------------------------------------------------
// .Description : Returns Output Image Attribute of this class
////////////////////////////////////////////////////////////////////////////////
SVImageClass* SVStdImageOperatorListClass::getOutputImage()
{
	return &outputImageObject;
}

////////////////////////////////////////////////////////////////////////////////
// .Title       : Run
// -----------------------------------------------------------------------------
// .Description : Special routing here.
////////////////////////////////////////////////////////////////////////////////
bool SVStdImageOperatorListClass::Run( SVRunStatusClass& rRunStatus, SvStl::MessageContainerVector *pErrorMessages )
{
	bool bRetVal = true;
	clearRunErrorMessages();
	
	SVRunStatusClass ChildRunStatus;
	ChildRunStatus.m_lResultDataIndex  = rRunStatus.m_lResultDataIndex;
	ChildRunStatus.Images = rRunStatus.Images;
	ChildRunStatus.m_UpdateCounters = rRunStatus.m_UpdateCounters;

	SVDataManagerHandle dmHandleInput;
	
	// Run yourself...
	bRetVal = onRun( rRunStatus, &m_RunErrorMessages );

	SVImageClass* pInputImage = getInputImage();
	SVImageClass* pOutputImage = getOutputImage();
	
	if ( nullptr == pInputImage || nullptr == pOutputImage )
	{
		bRetVal = false;
		SvStl::MessageContainer Msg( SVMSG_SVO_92_GENERAL_ERROR, SvStl::Tid_ErrorGettingInputs, SvStl::SourceFileParams(StdMessageParams), 0, GetUniqueObjectID() );
		m_RunErrorMessages.push_back(Msg);
	}

	if( bRetVal )
	{
		if ( pOutputImage->SetImageHandleIndex( rRunStatus.Images ) )
		{
			SVSmartHandlePointer input;
			SVSmartHandlePointer output;

			// Check for new image type...
			if( nullptr == pOutputImage->GetParentImage() )
			{
				// Use just the image input of this unary image operator list...
				pInputImage->GetImageHandle( input );
			}
			else
			{
				// Old image types...( i.e. 'S', used in Window Tool's Unary Image Operator List )
				
				// NOTE: 
				// The old image type 'S' provided also a child layer in his derived image info!!!
				
				// Use the Child layer on Input Image as our input image

				SvOi::ITool* pTool = GetToolInterface();
				if( nullptr != pTool && pOutputImage->GetLastResetTimeStamp() <= pInputImage->GetLastResetTimeStamp() )
				{
					pTool->UpdateImageWithExtent();
				}

				pOutputImage->GetParentImageHandle( input );
			}

			pOutputImage->GetImageHandle( output );

			if ( input.empty() || output.empty() )
			{
				bRetVal = false;
				SvStl::MessageContainer Msg( SVMSG_SVO_92_GENERAL_ERROR, SvStl::Tid_ErrorGettingInputs, SvStl::SourceFileParams(StdMessageParams), 0, GetUniqueObjectID() );
				m_RunErrorMessages.push_back(Msg);
				if( input.empty() )
				{
					SVImageProcessingClass::InitBuffer( output );

					input = output;
				}
			}

			//set tmp variable
			SVSmartHandlePointer sourceImage = m_milTmpImageObjectInfo1;
			SVSmartHandlePointer destinationImage = m_milTmpImageObjectInfo2;
			if ( sourceImage->empty() || destinationImage->empty() || !copyBuffer( input, sourceImage ) )
			{
				bRetVal = false;
				SvStl::MessageContainer Msg( SVMSG_SVO_92_GENERAL_ERROR, SvStl::Tid_CopyImagesFailed, SvStl::SourceFileParams(StdMessageParams), 0, GetUniqueObjectID() );
				m_RunErrorMessages.push_back(Msg);
			}

			if (bRetVal)
			{
				// Run children...
				for( int i = 0; i < GetSize(); i++ )
				{
					ChildRunStatus.ResetRunStateAndToolSetTimes();

					SVUnaryImageOperatorClass*  pOperator = ( SVUnaryImageOperatorClass* )GetAt( i );
					if( nullptr != pOperator )
					{
						if( pOperator->Run( true, sourceImage, destinationImage, ChildRunStatus ) )
						{
							//switch image buffer for next run
							SVSmartHandlePointer tmpImage = sourceImage;
							sourceImage = destinationImage;
							destinationImage = tmpImage;
						}
						// NOTE:
						// If operator returns FALSE, he was not running ( may be deactivated )
						// So, don't switch first flag off, so that a following operator knows
						// he is the first one or the 'no operator was running' check can do his job!!!
						// RO_22Mar2000

						// WARNING:
						// Do not set bRetVal automatically to FALSE, if operator was not running !!!
						// ChildRunStatus keeps information about, if an error occurred while running !!!
					}
					else
					{
						bRetVal = false;
						SvStl::MessageContainer Msg( SVMSG_SVO_92_GENERAL_ERROR, SvStl::Tid_ErrorGettingInputs, SvStl::SourceFileParams(StdMessageParams), 0, GetUniqueObjectID() );
						m_RunErrorMessages.push_back(Msg);
					}

					// Update our Run Status
					if( ChildRunStatus.IsDisabled() )
						rRunStatus.SetDisabled();

					if( ChildRunStatus.IsDisabledByCondition() )
						rRunStatus.SetDisabledByCondition();

					if( ChildRunStatus.IsWarned() )
						rRunStatus.SetWarned();

					if( ChildRunStatus.IsFailed() )
						rRunStatus.SetFailed();

					if( ChildRunStatus.IsPassed() )
						rRunStatus.SetPassed();

					if( ChildRunStatus.IsCriticalFailure() )
						rRunStatus.SetCriticalFailure();
				}
			} // for( int i = 0; i < GetSize(); i++ )
			
			// 'no operator was running' check...
			// RO_22Mar2000
			
			if( bRetVal )
			{
				if (!copyBuffer(sourceImage, output))
				{
					bRetVal = false;
					SvStl::MessageContainer Msg( SVMSG_SVO_92_GENERAL_ERROR, SvStl::Tid_CopyImagesFailed, SvStl::SourceFileParams(StdMessageParams), 0, GetUniqueObjectID() );
					m_RunErrorMessages.push_back(Msg);
				}
			} // if( bFirstFlag ) 
		}  // if ( pOutputImage->SetImageHandleIndex( rRunStatus.Images ) )
		else
		{
			bRetVal = false;
			SvStl::MessageContainer Msg( SVMSG_SVO_92_GENERAL_ERROR, SvStl::Tid_SetImageHandleIndexFailed, SvStl::SourceFileParams(StdMessageParams), 0, GetUniqueObjectID() );
			m_RunErrorMessages.push_back(Msg);
		}
	}
	
	if( ! bRetVal )
	{
		// Something was wrong...
		
		SetInvalid();
		rRunStatus.SetInvalid();
	}
	
	// Get Status Color...
	DWORD dwValue = rRunStatus.GetStatusColor();
	m_statusColor.SetValue(dwValue);
	
	// Get Status...
	dwValue = rRunStatus.GetState();
	m_statusTag.SetValue(dwValue);

	if (nullptr != pErrorMessages && !m_RunErrorMessages.empty())
	{
		pErrorMessages->insert(pErrorMessages->end(), m_RunErrorMessages.begin(), m_RunErrorMessages.end());
	}
	
	return bRetVal;
}

// Set String value object for Source Image Names
HRESULT SVStdImageOperatorListClass::CollectInputImageNames()
{
	HRESULT hr = S_FALSE;
	SVImageClass* pInputImage = getInputImage();
	SvOi::ITool* pTool = GetToolInterface();
	if (nullptr != pInputImage && nullptr != pTool)
	{
		SVString Name = pInputImage->GetCompleteName();

		if( pTool->SetFirstInputImageName( Name.c_str() ) )
		{
			hr = S_OK;
		}
	}
	return hr;
}

bool SVStdImageOperatorListClass::copyBuffer( const SVSmartHandlePointer input, SVSmartHandlePointer output )
{
	bool bRetVal = true;
	SVImageBufferHandleImage sourceMilHandle;
	SVImageBufferHandleImage destinationMilHandle;

	input->GetData( sourceMilHandle );
	output->GetData( destinationMilHandle );

	bRetVal = bRetVal && !( sourceMilHandle.empty() );
	bRetVal = bRetVal && !( destinationMilHandle.empty() );

	if( bRetVal )
	{
		HRESULT  Code = SVMatroxBufferInterface::CopyBuffer( destinationMilHandle.GetBuffer(), sourceMilHandle.GetBuffer() );
		bRetVal = (S_OK == Code);
	}	
	
	return bRetVal;
}

