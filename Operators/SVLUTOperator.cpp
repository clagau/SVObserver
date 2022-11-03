//******************************************************************************
//* COPYRIGHT (c) 2003 by Körber Pharma Inspection GmbH. All Rights Reserved
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVLUTOperator
//* .File Name       : $Workfile:   SVLUTOperator.cpp  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.1  $
//* .Check In Date   : $Date:   01 Oct 2013 14:57:22  $
//******************************************************************************

#pragma region Includes
#include "stdafx.h"
#include "SVLUTEquation.h"
#include "SVLUTOperator.h"
#include "InspectionEngine/SVImageClass.h"
#include "InspectionEngine/SVImageProcessingClass.h"
#include "SVMatroxLibrary/SVMatroxBufferInterface.h"
#include "SVMatroxLibrary/SVMatroxBufferCreateLineStruct.h"
#include "SVMatroxLibrary/SVMatroxImageInterface.h"
#include "InspectionEngine/RunStatus.h"
#pragma endregion Includes

namespace SvOp
{

#pragma region Declarations
#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif
#pragma endregion Declarations

SV_IMPLEMENT_CLASS( SVLUTOperator, SvPb::LUTOperatorClassId)

SVLUTOperator::SVLUTOperator( SVObjectClass* POwner, int StringResourceID )
				   :SVUnaryImageOperatorClass( POwner, StringResourceID ) 
{
	init();
}

////////////////////////////////////////////////////////////////////////////////
// .Title       : init
// -----------------------------------------------------------------------------
// .Description : Initialization of newly Instantiated Object
////////////////////////////////////////////////////////////////////////////////
void SVLUTOperator::init()
{
	// Identify our output type
	m_ObjectTypeInfo.m_ObjectType = SvPb::SVUnaryImageOperatorObjectType;
	m_ObjectTypeInfo.m_SubType = SvPb::SVLUTOperatorObjectType;

	// Register Embedded Object(s)
	RegisterEmbeddedObject( &m_lutVector, SvPb::OutputLUTVectorEId, IDS_OBJECTNAME_LUTVECTOR, false, SvOi::SVResetItemNone, true);
	RegisterEmbeddedObject( &m_useLUT, SvPb::UseLUTEId, IDS_OBJECTNAME_USE_LUT, false, SvOi::SVResetItemOwner, true);
	RegisterEmbeddedObject( &m_continuousRecalcLUT, SvPb::ContinuousRecalcLUTEId, IDS_OBJECTNAME_CONTINUOUS_RECALC_LUT, false, SvOi::SVResetItemOwner, true);
	RegisterEmbeddedObject( &m_lutMode, SvPb::LUTModeEId, IDS_OBJECTNAME_LUT_MODE, false, SvOi::SVResetItemOwner, true);
	RegisterEmbeddedObject( &m_upperClip, SvPb::LUTUpperClipEId, IDS_OBJECTNAME_LUT_UPPER_CLIP, false, SvOi::SVResetItemOwner, true);
	RegisterEmbeddedObject( &m_lowerClip, SvPb::LUTLowerClipEId, IDS_OBJECTNAME_LUT_LOWER_CLIP, false, SvOi::SVResetItemOwner, true);
	RegisterEmbeddedObject(&m_minInput, SvPb::LUTMinInputEId, IDS_OBJECTNAME_LUT_MIN_INPUT, false, SvOi::SVResetItemOwner, true);
	RegisterEmbeddedObject(&m_maxInput, SvPb::LUTMaxInputEId, IDS_OBJECTNAME_LUT_MAX_INPUT, false, SvOi::SVResetItemOwner, true);
	RegisterEmbeddedObject(&m_minOutput, SvPb::LUTMinOutputEId, IDS_OBJECTNAME_LUT_MIN_OUTPUT, false, SvOi::SVResetItemOwner, true);
	RegisterEmbeddedObject(&m_maxOutput, SvPb::LUTMaxOutputEId, IDS_OBJECTNAME_LUT_MAX_OUTPUT, false, SvOi::SVResetItemOwner, true);

	// Set Embedded defaults...

	// LUT Vector...
	m_lutVector.SetLegacyVectorObjectCompatibility();
	m_lutVector.SetDefaultValue( 0, true );

	// Use LUT...
	// Default must be false, since we have to be compatible to 
	//	older versions, which had no LUT yet.
	m_useLUT.SetDefaultValue( BOOL(false), true);

	// Continuous Recalc LUT...
	// Default is false.
	m_continuousRecalcLUT.SetDefaultValue( BOOL(false), true);

	// LUT Mode...
	// Enumerate Value Object...
	m_lutMode.SetEnumTypes( IDS_LUTMODE_ENUMOBJECT_LIST );
	// Default should be Identity...
	m_lutMode.SetDefaultValue( ( const long ) 0 , true);

	// Upper Clip Value for Clip Mode...
	m_upperClip.SetDefaultValue( 128, true);

	// Lower Clip Value for Clip Mode...
	m_lowerClip.SetDefaultValue( 0, true);

	m_minInput.SetDefaultValue(0, true);
	m_maxInput.SetDefaultValue(255, true);
	m_minOutput.SetDefaultValue(0, true);
	m_maxOutput.SetDefaultValue(255, true);

	// Trivial mil lut buffer...
	m_lutElementNumber = 0;

	// Add equation friends...
	SVLUTEquation* pLUTEquationFriend = new SVLUTEquation( this );
	if( pLUTEquationFriend )
		AddFriend( pLUTEquationFriend );


	// Identify our input type needs...

	// input of lut equation...
	m_inputLUTVectorResult.SetInputObjectType(SvPb::SVValueObjectType, SvPb::SVByteValueObjectType, SvPb::LUTEquationResultEId);
	registerInputObject( &m_inputLUTVectorResult, _T( "LUTOperator" ), SvPb::LUTOperatorInputEId);
	m_inputLUTVectorResult.SetObjectAttributesAllowed(SvPb::noAttributes, SvOi::SetAttributeType::OverwriteAttribute);;

	m_bForceLUTRecalc = true;
}

SVLUTOperator::~SVLUTOperator()
{
	SVLUTOperator::CloseObject();
}

////////////////////////////////////////////////////////////////////////////////
// .Title       : CreateObject
// -----------------------------------------------------------------------------
// .Description : ...
////////////////////////////////////////////////////////////////////////////////
bool SVLUTOperator::CreateObject( const SVObjectLevelCreateStruct& rCreateStructure )
{
	bool bOk = SVUnaryImageOperatorClass::CreateObject(rCreateStructure);

	// Set / Reset Printable Flag
	m_lutVector.SetObjectAttributesAllowed( SvPb::audittrail, SvOi::SetAttributeType::AddAttribute );
	m_lutVector.SetObjectAttributesAllowed( SvPb::viewable, SvOi::SetAttributeType::RemoveAttribute );
	
	m_useLUT.SetObjectAttributesAllowed( SvPb::audittrail, SvOi::SetAttributeType::AddAttribute );
	m_continuousRecalcLUT.SetObjectAttributesAllowed( SvPb::audittrail, SvOi::SetAttributeType::AddAttribute );
	m_lutMode.SetObjectAttributesAllowed( SvPb::audittrail, SvOi::SetAttributeType::AddAttribute );
	m_upperClip.SetObjectAttributesAllowed( SvPb::audittrail, SvOi::SetAttributeType::AddAttribute );
	m_lowerClip.SetObjectAttributesAllowed( SvPb::audittrail, SvOi::SetAttributeType::AddAttribute );
	m_minInput.SetObjectAttributesAllowed(SvPb::audittrail, SvOi::SetAttributeType::AddAttribute);
	m_maxInput.SetObjectAttributesAllowed(SvPb::audittrail, SvOi::SetAttributeType::AddAttribute);
	m_minOutput.SetObjectAttributesAllowed(SvPb::audittrail, SvOi::SetAttributeType::AddAttribute);
	m_maxOutput.SetObjectAttributesAllowed(SvPb::audittrail, SvOi::SetAttributeType::AddAttribute);

	m_isCreated = bOk;

	return bOk;
}

bool SVLUTOperator::ResetObject(SvStl::MessageContainerVector *pErrorMessages)
{
	bool Result = SVUnaryImageOperatorClass::ResetObject(pErrorMessages);

	m_inputLUTVectorResult.validateInput();

	m_bForceLUTRecalc = m_bForceLUTRecalc || Result;

	long lLutMode = 0;
	if (S_OK == m_lutMode.GetValue(lLutMode))
	{
		bool isVectorPrint = (4 /*Formula*/ != lLutMode);
		m_lutVector.SetObjectAttributesAllowed(SvPb::audittrail, isVectorPrint ? SvOi::SetAttributeType::AddAttribute : SvOi::SetAttributeType::RemoveAttribute);
	}

	return Result;
}
////////////////////////////////////////////////////////////////////////////////
// .Title       : CloseObject
// -----------------------------------------------------------------------------
// .Description : ...
////////////////////////////////////////////////////////////////////////////////
bool SVLUTOperator::CloseObject()
{
	m_lutBufID.clear();

	m_lutElementNumber = 0;

	return SVUnaryImageOperatorClass::CloseObject();
}

////////////////////////////////////////////////////////////////////////////////
// .Title       : RecalcLUT
// -----------------------------------------------------------------------------
// .Description : Recalulates LUT any time it is called completely.
//              : If MIL LUT buffer is not yet allocated, it tries to do this, 
//				: also.
////////////////////////////////////////////////////////////////////////////////
bool SVLUTOperator::RecalcLUT( SvIe::RunStatus& rRunStatus )
{
	if( getOutputImage() )
	{
		long pixelDepth = getOutputImage()->getPixelDepth();

		
		// Check Pixel depth...
		m_lutElementNumber = 1 << (  pixelDepth & SVBufferSize );

		if( m_lutBufID.empty() )
		{
			// Allocated MIL Lut Buffer..
			// &&&
			SVMatroxBufferCreateLineStruct l_Create;
			l_Create.m_eAttribute = SVBufAttLut;
			l_Create.SetImageDepth( pixelDepth );
			l_Create.m_lSizeX = m_lutElementNumber;
			SVMatroxBufferInterface::Create( m_lutBufID, l_Create );
			if( m_lutBufID.empty() )
			{
				// Something is wrong, cannot allocate LUT buffer...
				return false;
			}
		}

		// Get current LUT Operator Mode...
		long lLutMode = 0;
		if (S_OK != m_lutMode.GetValue(lLutMode))
		{
			return false;
		}

		// Check LUT Vector size...
		if (m_lutVector.getArraySize() != m_lutElementNumber)
		{
			m_lutVector.SetArraySize(m_lutElementNumber);
		}

		switch( lLutMode )
		{
			case SvPb::LUTIdentity:	// Identity...
			{
				// Fill lut vector with [ 0 ... ( m_lutElementNumber - 1 ) ]
				std::vector<BYTE> byteVec;
				byteVec.resize( m_lutElementNumber );

				for( long i = 0; i < m_lutElementNumber; ++ i )
				{
					byteVec[i] = (BYTE) i;
				}

				if( S_OK != m_lutVector.SetArrayValues(byteVec) )
				{
					return false;
				}
			}
			break;

			case SvPb::LUTInversion: // Inversion...
			{
				// Fill lut vector with [ ( m_lutElementNumber - 1 ) ... 0 ]
				std::vector<BYTE> byteVec;
				byteVec.resize( m_lutElementNumber );
				long firstEntry = m_lutElementNumber - 1;
				for( long i = 0; i < m_lutElementNumber; ++ i )
				{
					byteVec[i] = (BYTE) (firstEntry - i);
				}

				if( S_OK != m_lutVector.SetArrayValues(byteVec) )
				{
					return false;
				}
			}
			break;

			case SvPb::LUTSign: // Sign...
			{
				// Fill lut vector with [ ( m_lutElementNumber / 2 + 0 ) ( m_lutElementNumber / 2 + 1 ) ... ( m_lutElementNumber / 2 + ( m_lutElementNumber - 1 ) ) ]
				std::vector<BYTE> byteVec;
				byteVec.resize( m_lutElementNumber );
				long firstEntry = m_lutElementNumber / 2;
				for( long i = 0; i < m_lutElementNumber; ++ i )
				{
					byteVec[i] = (BYTE) (firstEntry + i);
				}

				if( S_OK != m_lutVector.SetArrayValues(byteVec) )
				{
					return false;
				}
			}
			break;

			case SvPb::LUTClip: // Clip...
			{
				// Set all LUT entries lower  than lower clip to value of lower clip.
				// Set all LUT entries higher than upper clip to value of upper clip.

				// Get clip values...
				long lUpperClip = 0;
				long lLowerClip = 0;
				if( ( S_OK != m_upperClip.GetValue( lUpperClip ) ) || ( S_OK != m_lowerClip.GetValue( lLowerClip ) ) )
				{
					return false;
				}

				// Prepare lut vector...
				std::vector<BYTE> byteVec;
				byteVec.resize(m_lutElementNumber);

				// Clip...
				for( long i = 0; i < m_lutElementNumber; ++ i )
				{
					if( i < lLowerClip )
					{
						byteVec[i] = (BYTE) lLowerClip;
					}
					else if( i > lUpperClip )
					{
						byteVec[i] = (BYTE) lUpperClip;
					}
					else
					{
						byteVec[i] = (BYTE) i;
					}
				}

				if( S_OK != m_lutVector.SetArrayValues(byteVec) )
				{
					return false;
				}
			}
			break;

			case SvPb::LUTFormula: // Formula...
			{
				bool l_bOk = runFriends( rRunStatus );

				// Get LUT Vector from equation and stick it inside m_lutVector...
				std::vector<BYTE> byteVector;
				SvVol::SVByteValueObjectClass* pLUTResult = m_inputLUTVectorResult.getInput<SvVol::SVByteValueObjectClass>(true);

				l_bOk = l_bOk && nullptr != pLUTResult;
				l_bOk = l_bOk && S_OK == pLUTResult->GetArrayValues(byteVector);
				l_bOk = l_bOk && S_OK == m_lutVector.SetArrayValues(byteVector);

				if( ! l_bOk )
				{
					return false;
				}
			}
			break;

			case SvPb::LUTFreeForm: // Free Form...
				// Do nothing !
				break;

			case SvPb::LUTStretch: // Stretch...
			{
				// Prepare lut vector...
				long minInput = 0, maxInput = 0, minOutput = 0, maxOutput = 0;
				if ((S_OK != m_minInput.GetValue(minInput)) || (S_OK != m_maxInput.GetValue(maxInput)) ||
					(S_OK != m_minOutput.GetValue(minOutput)) || (S_OK != m_maxOutput.GetValue(maxOutput)))
				{
					return false;
				}
				std::vector<BYTE> byteVec;
				byteVec.resize(m_lutElementNumber);

				if (minInput != maxInput)
				{
					long aMin = std::min(minOutput, maxOutput);
					long aMax = std::max(minOutput, maxOutput);
					double factor = static_cast<double>(maxOutput - minOutput) / (maxInput - minInput);
					for (long i = 0; i < m_lutElementNumber; ++i)
					{
						long z = static_cast<long>((i - minInput) * factor + minOutput);
						byteVec[i] = static_cast<BYTE>(std::max(std::min(z, aMax), aMin));
					}
				}
				else
				{
					std::fill(byteVec.begin(), byteVec.end(), static_cast<BYTE>(minOutput));
				}

				if (S_OK != m_lutVector.SetArrayValues(byteVec))
				{
					return false;
				}
				break;
			}
			default:
				// Unknown Mode...
				return false;
		}

		// Fill Mil Lut Buffer...
		if( !m_lutBufID.empty() )
		{
			if( 256 == m_lutElementNumber )	// 8 Bit LUT
			{
				std::vector<BYTE> byteVec;
				if( S_OK == m_lutVector.GetArrayValues( byteVec )  )
				{
					assert( byteVec.size() == m_lutElementNumber );
					if ( byteVec.size() == m_lutElementNumber )
					{
						/*l_Code = */SVMatroxBufferInterface::PutLine( m_lutBufID, m_lutElementNumber, reinterpret_cast<unsigned char*>(&(byteVec[0])));
					}

					// Success...
					return true;
				}
			}

			if( 65536 == m_lutElementNumber )	// 16 Bit LUT
			{
				// Other Depths are not yet supported...
			}

		}
	}
	return false;
}

bool SVLUTOperator::onRun( bool First, SvOi::SVImageBufferHandlePtr rInputImageHandle, SvOi::SVImageBufferHandlePtr rOutputImageHandle, SvIe::RunStatus& rRunStatus, SvStl::MessageContainerVector *pErrorMessages )
{ 
	// Is doing special friend routing !!!
	// Don't call base class onRun(...).

	// Since we do special routing here, we have to validate by ourself...
	if(nullptr == rInputImageHandle || nullptr == rOutputImageHandle )
	{
		if (nullptr != pErrorMessages)
		{
			SvStl::MessageContainer Msg( SVMSG_SVO_92_GENERAL_ERROR, SvStl::Tid_ErrorGettingInputs, SvStl::SourceFileParams(StdMessageParams), getObjectId() );
			pErrorMessages->push_back(Msg);
		}
		// Signal that something was wrong...
		rRunStatus.SetInvalid();
		return false;
	}

	BOOL bUseLUT  = false;
	BOOL bContinuousRecalcLUT = false;

	// Check, if LUT shall run...
	if( S_OK != m_useLUT.GetValue( bUseLUT ) )
	{
		if (nullptr != pErrorMessages)
		{
			SvStl::MessageContainer Msg( SVMSG_SVO_92_GENERAL_ERROR, SvStl::Tid_ErrorGettingInputs, SvStl::SourceFileParams(StdMessageParams), getObjectId() );
			pErrorMessages->push_back(Msg);
		}
		// Signal that something was wrong...
		rRunStatus.SetInvalid();
		return false;
	}

	// LUT Operator is disabled...
	if( ! bUseLUT )
	{
		// Disable only this operator...
		SetDisabled();

		// Do not return true here !!!
		//return true;

		// Signal that this operator was not running...
		// But everything was ok!!!!
		return false;
	}

	// Get shall LUT be continuous recalculated...
	if( S_OK != m_continuousRecalcLUT.GetValue( bContinuousRecalcLUT ) )
	{
		if (nullptr != pErrorMessages)
		{
			SvStl::MessageContainer Msg( SVMSG_SVO_92_GENERAL_ERROR, SvStl::Tid_ErrorGettingInputs, SvStl::SourceFileParams(StdMessageParams), getObjectId() );
			pErrorMessages->push_back(Msg);
		}
		// Signal that something was wrong...
		rRunStatus.SetInvalid();
		return false;
	}

	if( m_bForceLUTRecalc || bContinuousRecalcLUT )
	{
		// Try to recalc LUT...
		if( ! RecalcLUT( rRunStatus ) )
		{
			if (nullptr != pErrorMessages)
			{
				SvStl::MessageContainer Msg( SVMSG_SVO_92_GENERAL_ERROR, SvStl::Tid_RecalcLUTFailed, SvStl::SourceFileParams(StdMessageParams), getObjectId() );
				pErrorMessages->push_back(Msg);
			}
			// Signal that something was wrong...
			rRunStatus.SetInvalid();
			return false;
		}

		m_bForceLUTRecalc = false;
	}

	// Do LUT...
	HRESULT MatroxCode = SVMatroxImageInterface::LutMap(rOutputImageHandle->GetBuffer(), First ? rInputImageHandle->GetBuffer() : rOutputImageHandle->GetBuffer(), m_lutBufID);

	if (S_OK != MatroxCode)
	{
		if (nullptr != pErrorMessages)
		{
			SvStl::MessageContainer Msg( SVMSG_SVO_92_GENERAL_ERROR, SvStl::Tid_RunLutFailed, SvStl::SourceFileParams(StdMessageParams), getObjectId() );
			pErrorMessages->push_back(Msg);
		}
		// Signal that something was wrong...
		rRunStatus.SetInvalid();
		return false;
	}

	// Success...
	return true;
}

} //namespace SvOp
