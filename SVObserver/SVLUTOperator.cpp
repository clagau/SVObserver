//******************************************************************************
//* COPYRIGHT (c) 2003 by SVResearch, Harrisburg
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
#include "SVLUTOperator.h"
#include "SVImageLibrary/SVImageBufferHandleImage.h"
#include "InspectionEngine/SVImageClass.h"
#include "InspectionEngine/SVImageProcessingClass.h"
#include "SVLUTEquation.h"
#pragma endregion Includes

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

SV_IMPLEMENT_CLASS( SVLUTOperatorClass, SVLUTOperatorClassGuid )

SVLUTOperatorClass::SVLUTOperatorClass( SVObjectClass* POwner, int StringResourceID )
				   :SVUnaryImageOperatorClass( POwner, StringResourceID ) 
{
	init();
}

////////////////////////////////////////////////////////////////////////////////
// .Title       : init
// -----------------------------------------------------------------------------
// .Description : Initialization of newly Instantiated Object
////////////////////////////////////////////////////////////////////////////////
void SVLUTOperatorClass::init()
{
	// Identify our output type
	m_outObjectInfo.m_ObjectTypeInfo.ObjectType = SvDef::SVUnaryImageOperatorObjectType;
	m_outObjectInfo.m_ObjectTypeInfo.SubType = SvDef::SVLUTOperatorObjectType;

	// Register Embedded Object(s)
	RegisterEmbeddedObject( &m_lutVector, SVOutputLUTVectorObjectGuid, IDS_OBJECTNAME_LUTVECTOR, false, SvOi::SVResetItemNone );
	RegisterEmbeddedObject( &m_useLUT, SVUseLUTObjectGuid, IDS_OBJECTNAME_USE_LUT, false, SvOi::SVResetItemOwner  );
	RegisterEmbeddedObject( &m_continuousRecalcLUT, SVContinuousRecalcLUTObjectGuid, IDS_OBJECTNAME_CONTINUOUS_RECALC_LUT, false, SvOi::SVResetItemOwner  );
	RegisterEmbeddedObject( &m_lutMode, SVLUTModeObjectGuid, IDS_OBJECTNAME_LUT_MODE, false, SvOi::SVResetItemOwner  );
	RegisterEmbeddedObject( &m_upperClip, SVLUTUpperClipObjectGuid, IDS_OBJECTNAME_LUT_UPPER_CLIP, false, SvOi::SVResetItemOwner  );
	RegisterEmbeddedObject( &m_lowerClip, SVLUTLowerClipObjectGuid, IDS_OBJECTNAME_LUT_LOWER_CLIP, false, SvOi::SVResetItemOwner  );

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

	// Trivial mil lut buffer...
	m_lutElementNumber = 0;

	// Add equation friends...
	SVLUTEquationClass* pLUTEquationFriend = new SVLUTEquationClass( this );
	if( pLUTEquationFriend )
		AddFriend( pLUTEquationFriend->GetUniqueObjectID() );


	// Identify our input type needs...

	// input of lut equation...
	m_inputLUTVectorResult.SetInputObjectType(SVLUTEquationResultObjectGuid, SvDef::SVValueObjectType, SvDef::SVByteValueObjectType);
	m_inputLUTVectorResult.SetObject( GetObjectInfo() );
	RegisterInputObject( &m_inputLUTVectorResult, _T( "LUTOperator" ) );

	addDefaultInputObjects();

	m_bForceLUTRecalc = true;
}

SVLUTOperatorClass::~SVLUTOperatorClass()
{
	CloseObject();
}

////////////////////////////////////////////////////////////////////////////////
// .Title       : CreateObject
// -----------------------------------------------------------------------------
// .Description : ...
////////////////////////////////////////////////////////////////////////////////
bool SVLUTOperatorClass::CreateObject( const SVObjectLevelCreateStruct& rCreateStructure )
{
	bool bOk = SVUnaryImageOperatorClass::CreateObject(rCreateStructure);

	// Set / Reset Printable Flag
	m_lutVector.SetObjectAttributesAllowed( SvDef::SV_PRINTABLE, SvOi::SetAttributeType::AddAttribute );
	m_lutVector.SetObjectAttributesAllowed( SvDef::SV_VIEWABLE, SvOi::SetAttributeType::RemoveAttribute );
	
	m_useLUT.SetObjectAttributesAllowed( SvDef::SV_PRINTABLE, SvOi::SetAttributeType::AddAttribute );
	m_continuousRecalcLUT.SetObjectAttributesAllowed( SvDef::SV_PRINTABLE, SvOi::SetAttributeType::AddAttribute );
	m_lutMode.SetObjectAttributesAllowed( SvDef::SV_PRINTABLE, SvOi::SetAttributeType::AddAttribute );
	m_upperClip.SetObjectAttributesAllowed( SvDef::SV_PRINTABLE, SvOi::SetAttributeType::AddAttribute );
	m_lowerClip.SetObjectAttributesAllowed( SvDef::SV_PRINTABLE, SvOi::SetAttributeType::AddAttribute );

	m_isCreated = bOk;

	return bOk;
}

bool SVLUTOperatorClass::ResetObject(SvStl::MessageContainerVector *pErrorMessages)
{
	bool Result = SVUnaryImageOperatorClass::ResetObject(pErrorMessages);

	m_bForceLUTRecalc = m_bForceLUTRecalc || Result;

	return Result;
}
////////////////////////////////////////////////////////////////////////////////
// .Title       : CloseObject
// -----------------------------------------------------------------------------
// .Description : ...
////////////////////////////////////////////////////////////////////////////////
bool SVLUTOperatorClass::CloseObject()
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
bool SVLUTOperatorClass::RecalcLUT( SVRunStatusClass& rRunStatus )
{
	long lLutMode = 0;

	if( getReferenceImage() )
	{
		long pixelDepth = getReferenceImage()->getPixelDepth();

		HRESULT l_Code;
		
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
			l_Code = SVMatroxBufferInterface::Create( m_lutBufID, l_Create );
			if( m_lutBufID.empty() )
			{
				// Something is wrong, cannot allocate LUT buffer...
				return false;
			}
		}

		// Get current LUT Operator Mode...
		if (S_OK != m_lutMode.GetValue(lLutMode))
		{
			return false;
		}

		switch( lLutMode )
		{
			case 0:	// Identity...
			{
				// Fill lut vector with [ 0 ... ( m_lutElementNumber - 1 ) ]
				std::vector<BYTE> byteVec;
				byteVec.resize( m_lutElementNumber );

				for( long i = 0; i < m_lutElementNumber; ++ i )
				{
					byteVec[i] = (BYTE) i;
				}

				m_lutVector.SetArraySize(static_cast<int>(byteVec.size()));
				if( S_OK != m_lutVector.SetArrayValues(byteVec) )
				{
					return false;
				}
			}
			break;

			case 1: // Inversion...
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

			case 2: // Sign...
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

			case 3: // Clip...
			{
				// Set all LUT entries lower  than lower clip to value of lower clip.
				// Set all LUT entries higher than upper clip to value of upper clip.

				// Prepare lut vector...
				std::vector<BYTE> byteVec;
				byteVec.resize( m_lutElementNumber );

				// Get clip values...
				long lUpperClip = 0;
				long lLowerClip = 0;
				if( ( S_OK != m_upperClip.GetValue( lUpperClip ) ) || ( S_OK != m_lowerClip.GetValue( lLowerClip ) ) )
				{
					return false;
				}

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

			case 4: // Formula...
			{
				bool l_bOk = runFriends( rRunStatus );

				// Get LUT Vector from equation and stick it inside m_lutVector...
				std::vector<BYTE> byteVector;
				SVByteValueObjectClass* pLUTResult = getInputLUTVectorResult();

				l_bOk = l_bOk && nullptr != pLUTResult;
				l_bOk = l_bOk && S_OK == pLUTResult->GetArrayValues(byteVector);
				l_bOk = l_bOk && S_OK == m_lutVector.SetArrayValues(byteVector);

				if( ! l_bOk )
				{
					return false;
				}
			}
			break;

			case 5: // Free Form...
				// Do nothing !
				break;

			default:
				// Unknown Mode...
				return false;
		}

		// Check LUT Vector size...
		if( m_lutVector.getArraySize() != m_lutElementNumber )
		{
			m_lutVector.SetArraySize( m_lutElementNumber );
		}

		// Fill Mil Lut Buffer...
		if( !m_lutBufID.empty() )
		{
			if( 256 == m_lutElementNumber )	// 8 Bit LUT
			{
				std::vector<BYTE> byteVec;
				if( S_OK == m_lutVector.GetArrayValues( byteVec )  )
				{
					ASSERT( byteVec.size() == m_lutElementNumber );
					if ( byteVec.size() == m_lutElementNumber )
					{
						l_Code = SVMatroxBufferInterface::PutLine( m_lutBufID, m_lutElementNumber, reinterpret_cast<unsigned char*>(&(byteVec[0])));
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


////////////////////////////////////////////////////////////////////////////////
// .Title       : getInputLUTVectorResult
// -----------------------------------------------------------------------------
// .Description : Returns input LUT Vector. Is comming from LUTEquation friend.
////////////////////////////////////////////////////////////////////////////////
SVByteValueObjectClass* SVLUTOperatorClass::getInputLUTVectorResult()
{
	if( m_inputLUTVectorResult.IsConnected() && m_inputLUTVectorResult.GetInputObjectInfo().m_pObject )
		return dynamic_cast <SVByteValueObjectClass*> (m_inputLUTVectorResult.GetInputObjectInfo().m_pObject);

	return nullptr;
}

////////////////////////////////////////////////////////////////////////////////
// .Title       : onRun
// -----------------------------------------------------------------------------
// .Description : Runs this operator.
//              : Returns false, if operator cannot run ( may be deactivated ! )
////////////////////////////////////////////////////////////////////////////////
bool SVLUTOperatorClass::onRun( bool First, SVImageBufferHandlePtr rInputImageHandle, SVImageBufferHandlePtr rOutputImageHandle, SVRunStatusClass& rRunStatus, SvStl::MessageContainerVector *pErrorMessages )
{ 
	// Is doing special friend routing !!!
	// Don't call base class onRun(...).

	// Since we do special routing here, we have to validate by ourself...
	if(nullptr == rInputImageHandle || nullptr == rOutputImageHandle )
	{
		if (nullptr != pErrorMessages)
		{
			SvStl::MessageContainer Msg( SVMSG_SVO_92_GENERAL_ERROR, SvStl::Tid_ErrorGettingInputs, SvStl::SourceFileParams(StdMessageParams), 0, GetUniqueObjectID() );
			pErrorMessages->push_back(Msg);
		}
		// Signal that something was wrong...
		SetInvalid();
		rRunStatus.SetInvalid();
		return false;
	}

	SVImageBufferHandleImage l_InMilHandle;
	SVImageBufferHandleImage l_OutMilHandle;

	rInputImageHandle->GetData( l_InMilHandle );
	rOutputImageHandle->GetData( l_OutMilHandle );

	BOOL bUseLUT  = false;
	BOOL bContinuousRecalcLUT = false;

	// Check, if LUT shall run...
	if( S_OK != m_useLUT.GetValue( bUseLUT ) )
	{
		if (nullptr != pErrorMessages)
		{
			SvStl::MessageContainer Msg( SVMSG_SVO_92_GENERAL_ERROR, SvStl::Tid_ErrorGettingInputs, SvStl::SourceFileParams(StdMessageParams), 0, GetUniqueObjectID() );
			pErrorMessages->push_back(Msg);
		}
		// Signal that something was wrong...
		SetInvalid();
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
			SvStl::MessageContainer Msg( SVMSG_SVO_92_GENERAL_ERROR, SvStl::Tid_ErrorGettingInputs, SvStl::SourceFileParams(StdMessageParams), 0, GetUniqueObjectID() );
			pErrorMessages->push_back(Msg);
		}
		// Signal that something was wrong...
		SetInvalid();
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
				SvStl::MessageContainer Msg( SVMSG_SVO_92_GENERAL_ERROR, SvStl::Tid_RecalcLUTFailed, SvStl::SourceFileParams(StdMessageParams), 0, GetUniqueObjectID() );
				pErrorMessages->push_back(Msg);
			}
			// Signal that something was wrong...
			SetInvalid();
			rRunStatus.SetInvalid();
			return false;
		}

		m_bForceLUTRecalc = false;
	}

	// Do LUT...
	HRESULT MatroxCode = SVMatroxImageInterface::LutMap( l_OutMilHandle.GetBuffer(), First ? l_InMilHandle.GetBuffer() : l_OutMilHandle.GetBuffer(), m_lutBufID);

	if (S_OK != MatroxCode)
	{
		if (nullptr != pErrorMessages)
		{
			SvStl::MessageContainer Msg( SVMSG_SVO_92_GENERAL_ERROR, SvStl::Tid_RunLutFailed, SvStl::SourceFileParams(StdMessageParams), 0, GetUniqueObjectID() );
			pErrorMessages->push_back(Msg);
		}
		// Signal that something was wrong...
		SetInvalid();
		rRunStatus.SetInvalid();
		return false;
	}

	// Success...
	return true;
}

