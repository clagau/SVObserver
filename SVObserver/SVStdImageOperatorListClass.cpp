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
#include "SVObjectLibrary/SVAnalyzerLevelCreateStruct.h"

#include "SVOCore/SVImageClass.h"
#include "SVOCore/SVImageProcessingClass.h"
#include "SVUnaryImageOperatorClass.h"
#include "SVTool.h"
#include "CameraLibrary/SVDeviceParams.h" //Arvid added to avoid VS2015 compile Error

#pragma endregion Includes

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

SV_IMPLEMENT_CLASS( SVStdImageOperatorListClass, SVStdImageOperatorListClassGuid )

SVStdImageOperatorListClass::SVStdImageOperatorListClass( BOOL BCreateDefaultTaskList, SVObjectClass* POwner, int StringResourceID )
							:SVUnaryImageOperatorListClass( BCreateDefaultTaskList, POwner, StringResourceID ) 
{
	init();
}

void SVStdImageOperatorListClass::init()
{
	m_bUseOverlays = false;

	// Identify our output type
	m_outObjectInfo.ObjectTypeInfo.ObjectType = SVUnaryImageOperatorListObjectType;
	m_outObjectInfo.ObjectTypeInfo.SubType	= SVStdImageOperatorListObjectType;

	// SetObjectDepth() already called in SVObjectClass Ctor

	// Register Embedded Objects
	RegisterEmbeddedObject( &outputImageObject, SVOutputImageObjectGuid, IDS_OBJECTNAME_IMAGE1 );

	// Set Embedded defaults
	outputImageObject.InitializeImage( SVImageTypeLogicalAndPhysical );

	// Set default inputs and outputs
	addDefaultInputObjects();
}

SVStdImageOperatorListClass::~SVStdImageOperatorListClass() 
{ 
	CloseObject();
}

BOOL SVStdImageOperatorListClass::CreateObject( SVObjectLevelCreateStruct* PCreateStructure )
{
	BOOL l_bOk = __super::CreateObject( PCreateStructure );

	// Image input must already exist, and must be created!!!
	
	// Embedded Image output must already exist!!!
	l_bOk &= ( S_OK == outputImageObject.InitializeImage( getInputImage() ) );

	m_isCreated = l_bOk;

	return l_bOk;
}

BOOL SVStdImageOperatorListClass::CloseObject()
{
	BOOL bRetVal = true;
	
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

bool SVStdImageOperatorListClass::resetAllObjects( bool shouldNotifyFriends, bool silentReset )
{
	bool Result = ( S_OK == ResetObject() );
	ASSERT( Result );

	return( __super::resetAllObjects( shouldNotifyFriends, silentReset ) );
}

HRESULT SVStdImageOperatorListClass::ResetObject()
{
	HRESULT l_hrOk = outputImageObject.InitializeImage( getInputImage() );
	
	if ( S_OK != __super::ResetObject() )
	{
		l_hrOk = S_FALSE;
	}

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

	return l_hrOk;
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
BOOL SVStdImageOperatorListClass::Run( SVRunStatusClass& RRunStatus )
{
	BOOL bRetVal = true;

	SVSmartHandlePointer input;
	SVSmartHandlePointer output;
	
	SVRunStatusClass ChildRunStatus;
	ChildRunStatus.m_lResultDataIndex  = RRunStatus.m_lResultDataIndex;
	ChildRunStatus.Images = RRunStatus.Images;
	ChildRunStatus.m_UpdateCounters = RRunStatus.m_UpdateCounters;

	SVDataManagerHandle dmHandleInput;
	
	// Run yourself...
	bRetVal = onRun( RRunStatus );

	SVImageClass* pInputImage = getInputImage();
	SVImageClass* pOutputImage = getOutputImage();
	
	bRetVal = bRetVal && ( nullptr != pInputImage );
	bRetVal = bRetVal && ( nullptr != pOutputImage );

	if( bRetVal )
	{
		bRetVal = pOutputImage->SetImageHandleIndex( RRunStatus.Images );

		if ( bRetVal )
		{
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
				SVToolClass* pTool = dynamic_cast<SVToolClass*>(GetTool());
				if( pTool && pOutputImage->GetLastResetTimeStamp() <= pInputImage->GetLastResetTimeStamp() )
				{
					pTool->UpdateImageWithExtent( RRunStatus.m_lResultDataIndex );
				}

				pOutputImage->GetParentImageHandle( input );
			}

			pOutputImage->GetImageHandle( output );

			bRetVal = bRetVal && !( input.empty() );
			bRetVal = bRetVal && !( output.empty() );

			if( input.empty() )
			{
				SVImageProcessingClass::InitBuffer( output );

				input = output;
			}

			//set tmp variable
			SVSmartHandlePointer sourceImage = m_milTmpImageObjectInfo1;
			SVSmartHandlePointer destinationImage = m_milTmpImageObjectInfo2;
			bRetVal = bRetVal && !( sourceImage->empty() );
			bRetVal = bRetVal && !( destinationImage->empty() );
			bRetVal = bRetVal && copyBuffer( input, sourceImage );

			if (bRetVal)
			{
				// Run children...
				for( int i = 0; i < GetSize(); i++ )
				{
					ChildRunStatus.ResetRunStateAndToolSetTimes();

					SVUnaryImageOperatorClass*  pOperator = ( SVUnaryImageOperatorClass* )GetAt( i );

					bRetVal &= ( nullptr != pOperator );

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

					// Update our Run Status
					if( ChildRunStatus.IsDisabled() )
						RRunStatus.SetDisabled();

					if( ChildRunStatus.IsDisabledByCondition() )
						RRunStatus.SetDisabledByCondition();

					if( ChildRunStatus.IsWarned() )
						RRunStatus.SetWarned();

					if( ChildRunStatus.IsFailed() )
						RRunStatus.SetFailed();

					if( ChildRunStatus.IsPassed() )
						RRunStatus.SetPassed();

					if( ChildRunStatus.IsCriticalFailure() )
						RRunStatus.SetCriticalFailure();
				}
			} // for( int i = 0; i < GetSize(); i++ )
			
			// 'no operator was running' check...
			// RO_22Mar2000
			
			if( bRetVal )
			{
				bRetVal = copyBuffer(sourceImage, output);
			} // if( bFirstFlag ) 
		}
	}
	
	if( ! bRetVal )
	{
		// Something was wrong...
		
		SetInvalid();
		RRunStatus.SetInvalid();
	}
	
	// Get Status Color...
	DWORD dwValue = RRunStatus.GetStatusColor();
	m_statusColor.SetValue( RRunStatus.m_lResultDataIndex, dwValue );
	
	// Get Status...
	dwValue = RRunStatus.GetState();
	m_statusTag.SetValue( RRunStatus.m_lResultDataIndex, dwValue );
	
	return bRetVal;
}

BOOL SVStdImageOperatorListClass::OnValidate()
{
	if( SVUnaryImageOperatorListClass::OnValidate() )
	{
		if( outputImageObject.OnValidate() ) 
		{
			return true;
		}
	}

	SetInvalid();
	return false;
}

// Set String value object for Source Image Names
HRESULT SVStdImageOperatorListClass::CollectInputImageNames()
{
	HRESULT l_hr = S_FALSE;
	SVImageClass* l_pInputImage = getInputImage();
	SVToolClass* l_pTool = dynamic_cast<SVToolClass*>( GetTool() );
	if( l_pInputImage && l_pTool )
	{
		CString l_strName = l_pInputImage->GetCompleteObjectName();

		SVStaticStringValueObjectClass* l_lstrVO = l_pTool->GetInputImageNames();
		if( l_lstrVO )
		{
			l_lstrVO->SetValue( 0/*Static value, this parameter will not used*/, l_strName );
		}

		l_hr = S_OK;
	}
	return l_hr;
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
		SVMatroxBufferInterface::SVStatusCode  l_Code = SVMatroxBufferInterface::CopyBuffer( destinationMilHandle.GetBuffer(), sourceMilHandle.GetBuffer() );
		bRetVal = (SVMEE_STATUS_OK == l_Code);
	}	
	
	return bRetVal;
}

