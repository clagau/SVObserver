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

#include "stdafx.h"
#include "SVLUTOperator.h"
#include "SVImageLibrary/SVImageBufferHandleImage.h"
#include "SVImageClass.h"
#include "SVImageProcessingClass.h" 
#include "SVLUTEquation.h"

//******************************************************************************
//* DEFINITIONS OF MODULE-LOCAL VARIABLES:
//******************************************************************************

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif


//******************************************************************************
//* CLASS METHOD IMPLEMENTATION(S):
//******************************************************************************


//*/*\*/*\*/*\*/*\*/*\*/*\*/*\*/*\*/*\*/*\*/*\*/*\*/*\*/*\*/*\*/*\*/*\*/*\*/*\*/
//* Class Name : SVLUTOperatorClass
//* Note(s)    : 
//*/*\*/*\*/*\*/*\*/*\*/*\*/*\*/*\*/*\*/*\*/*\*/*\*/*\*/*\*/*\*/*\*/*\*/*\*/*\*/

//******************************************************************************
// Adjustments
//******************************************************************************
SV_IMPLEMENT_CLASS( SVLUTOperatorClass, SVLUTOperatorClassGuid )



//******************************************************************************
// Constructor(s):
//******************************************************************************

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
// .History
//	 Date		Author		Comment                                       
//  :16.03.2000 RO			First Implementation
////////////////////////////////////////////////////////////////////////////////
void SVLUTOperatorClass::init()
{
	// Identify our output type
	outObjectInfo.ObjectTypeInfo.ObjectType = SVUnaryImageOperatorObjectType;
	outObjectInfo.ObjectTypeInfo.SubType = SVLUTOperatorObjectType;

	// Register Embedded Object(s)
	RegisterEmbeddedObject( &m_lutVector, SVOutputLUTVectorObjectGuid, IDS_OBJECTNAME_LUTVECTOR, false, SVResetItemNone );
	RegisterEmbeddedObject( &m_useLUT, SVUseLUTObjectGuid, IDS_OBJECTNAME_USE_LUT, false, SVResetItemOwner  );
	RegisterEmbeddedObject( &m_continuousRecalcLUT, SVContinuousRecalcLUTObjectGuid, IDS_OBJECTNAME_CONTINUOUS_RECALC_LUT, false, SVResetItemOwner  );
	RegisterEmbeddedObject( &m_lutMode, SVLUTModeObjectGuid, IDS_OBJECTNAME_LUT_MODE, false, SVResetItemOwner  );
	RegisterEmbeddedObject( &m_upperClip, SVLUTUpperClipObjectGuid, IDS_OBJECTNAME_LUT_UPPER_CLIP, false, SVResetItemOwner  );
	RegisterEmbeddedObject( &m_lowerClip, SVLUTLowerClipObjectGuid, IDS_OBJECTNAME_LUT_LOWER_CLIP, false, SVResetItemOwner  );

	// Set Embedded defaults...

	// LUT Vector...
	m_lutVector.SetLegacyVectorObjectCompatibility();
	m_lutVector.SetDefaultValue( 0, TRUE );
	// NOTE: Vector Size Setting...
	// Set default vector size in CreateObject, since objectDepth must be set before!!!
	//m_lutVector.SetSize( 256, TRUE );
	// Place another check/set vector size at the position you want to use the vector
	// i.e. in onRun(...) : 
	//	if( vector.GetSize() != targetSize  ) { vector.SetSize( targetSize ); }

	// Use LUT...
	// Default must be FALSE, since we have to be compatible to 
	//	older versions, which had no LUT yet.
	m_useLUT.SetDefaultValue( FALSE, TRUE ); 

	// Continuous Recalc LUT...
	// Default is FALSE.
	m_continuousRecalcLUT.SetDefaultValue( FALSE, TRUE ); 

	// LUT Mode...
	// Enumerate Value Object...
	m_lutMode.SetEnumTypes( IDS_LUTMODE_ENUMOBJECT_LIST );
	// Default should be Identity...
	m_lutMode.SetDefaultValue( ( const long ) 0 , TRUE );

	// Upper Clip Value for Clip Mode...
	m_upperClip.SetDefaultValue( 128, TRUE );

	// Lower Clip Value for Clip Mode...
	m_lowerClip.SetDefaultValue( 0, TRUE );

	// Trivial mil lut buffer...
	m_lutElementNumber = 0;

	// Add equation friends...
	SVLUTEquationClass* pLUTEquationFriend = new SVLUTEquationClass( this );
	if( pLUTEquationFriend )
		AddFriend( pLUTEquationFriend->GetUniqueObjectID() );


	// Identify our input type needs...

	// input of lut equation...
	m_inputLUTVectorResult.SetInputObjectType( SVLUTEquationResultObjectGuid, SVByteValueObjectType );
	m_inputLUTVectorResult.SetObject( GetObjectInfo() );
	RegisterInputObject( &m_inputLUTVectorResult, _T( "LUTOperator" ) );

	addDefaultInputObjects();

	m_bForceLUTRecalc = true;
}


SVLUTOperatorClass::~SVLUTOperatorClass()
{
	CloseObject();
}

//******************************************************************************
// Operator(s):
//******************************************************************************

////////////////////////////////////////////////////////////////////////////////
// .Title       : CreateObject
// -----------------------------------------------------------------------------
// .Description : ...
//              :
////////////////////////////////////////////////////////////////////////////////
// .History
//	 Date		Author		Comment                                       
//  :16.03.2000 RO			First Implementation
////////////////////////////////////////////////////////////////////////////////
BOOL SVLUTOperatorClass::CreateObject( SVObjectLevelCreateStruct* PCreateStructure )
{
	BOOL bOk = SVUnaryImageOperatorClass::CreateObject( PCreateStructure );

	// Set / Reset Printable Flag
	m_lutVector.ObjectAttributesAllowedRef() |= SV_PRINTABLE;
	m_lutVector.ObjectAttributesAllowedRef() &= ~SV_VIEWABLE;
	
	m_useLUT.ObjectAttributesAllowedRef() |= SV_PRINTABLE;
	m_continuousRecalcLUT.ObjectAttributesAllowedRef() |= SV_PRINTABLE;
	m_lutMode.ObjectAttributesAllowedRef() |= SV_PRINTABLE;
	m_upperClip.ObjectAttributesAllowedRef() |= SV_PRINTABLE;
	m_lowerClip.ObjectAttributesAllowedRef() |= SV_PRINTABLE;

	isCreated = bOk;

	return bOk;
}


DWORD_PTR SVLUTOperatorClass::processMessage( DWORD DwMessageID, DWORD_PTR DwMessageValue, DWORD_PTR DwMessageContext )
{
	DWORD_PTR DwResult = NULL;
	// Try to process message by yourself...
	DWORD dwPureMessageID = DwMessageID & SVM_PURE_MESSAGE;
	BOOL bUseSilent = TRUE;
	switch( dwPureMessageID )
	{
		case SVMSGID_RESET_ALL_OBJECTS:
		{
			HRESULT ResetStatus = ResetObject();
			if( ResetStatus != S_OK )
			{
				DwResult = SVMR_NO_SUCCESS;
			}
			else
			{
				DwResult = SVMR_SUCCESS;
			}
			bool bLutActive = false;
			m_useLUT.GetValue(bLutActive);

			long lLutMode = 0;
			m_lutMode.GetValue(lLutMode);

			if (!bLutActive || (bLutActive && (lLutMode != 4)))
			{
				//Set DwMessageValue to be silent if you are not using a formual
				DwMessageValue = bUseSilent;
			}
			break;
		}
	}
	return( SVTaskObjectClass::processMessage( DwMessageID, DwMessageValue, DwMessageContext ) | DwResult );
}

HRESULT SVLUTOperatorClass::ResetObject()
{
	HRESULT l_hrOk = SVUnaryImageOperatorClass::ResetObject();

	m_bForceLUTRecalc = m_bForceLUTRecalc || l_hrOk == S_OK;

	return l_hrOk;
}
////////////////////////////////////////////////////////////////////////////////
// .Title       : CloseObject
// -----------------------------------------------------------------------------
// .Description : ...
//              :
////////////////////////////////////////////////////////////////////////////////
// .History
//	 Date		Author		Comment                                       
//  :16.03.2000 RO			First Implementation
////////////////////////////////////////////////////////////////////////////////
BOOL SVLUTOperatorClass::CloseObject()
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
//				: 
////////////////////////////////////////////////////////////////////////////////
// .History
//	 Date		Author		Comment                                       
//  :19.03.2000 RO			First Implementation
////////////////////////////////////////////////////////////////////////////////
BOOL SVLUTOperatorClass::RecalcLUT( SVRunStatusClass& RRunStatus )
{
	long lLutMode = 0;

	if( getReferenceImage() )
	{
		long pixelDepth = getReferenceImage()->getPixelDepth();

		SVMatroxBufferInterface::SVStatusCode l_Code;
		
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
				return FALSE;
			}
		}

		// Get current LUT Operator Mode...
		if( S_OK != m_lutMode.GetValue( lLutMode ) )
			return FALSE;

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
				if( S_OK != m_lutVector.SetArrayValues( RRunStatus.m_lResultDataIndex, byteVec ) )
				{
					return FALSE;
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

				if( S_OK != m_lutVector.SetArrayValues( RRunStatus.m_lResultDataIndex, byteVec ) )
				{
					return FALSE;
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

				if( S_OK != m_lutVector.SetArrayValues( RRunStatus.m_lResultDataIndex, byteVec ) )
				{
					return FALSE;
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
					return FALSE;
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

				if( S_OK != m_lutVector.SetArrayValues( RRunStatus.m_lResultDataIndex, byteVec ) )
				{
					return FALSE;
				}
			}
			break;

			case 4: // Formula...
			{
				BOOL l_bOk = runFriends( RRunStatus );

				// Get LUT Vector from equation and stick it inside m_lutVector...
				std::vector<BYTE> byteVector;
				SVByteValueObjectClass* pLUTResult = getInputLUTVectorResult();

				l_bOk = l_bOk && pLUTResult != NULL;
				l_bOk = l_bOk && S_OK == pLUTResult->GetArrayValues( byteVector );
				l_bOk = l_bOk && S_OK == m_lutVector.SetArrayValues( RRunStatus.m_lResultDataIndex, byteVector );

				if( ! l_bOk )
				{
					return FALSE;
				}
			}
			break;

			case 5: // Free Form...
				// Do nothing !
				break;

			default:
				// Unknown Mode...
				return FALSE;
		}

		// Check LUT Vector size...
		if( m_lutVector.GetArraySize() != m_lutElementNumber )
		{
			m_lutVector.SetArraySize( m_lutElementNumber );
		}

		// Fill Mil Lut Buffer...
		if( !m_lutBufID.empty() )
		{
			if( m_lutElementNumber == 256 )	// 8 Bit LUT
			{
				std::vector<BYTE> byteVec;
				if( m_lutVector.GetArrayValues( byteVec ) == S_OK )
				{
					ASSERT( byteVec.size() == m_lutElementNumber );
					if ( byteVec.size() == m_lutElementNumber )
					{
						l_Code = SVMatroxBufferInterface::PutLine( m_lutBufID, m_lutElementNumber, reinterpret_cast<unsigned char*>(&(byteVec[0])));
					}

					// Success...
					return TRUE;
				}
			}

			if( m_lutElementNumber == 65536 )	// 16 Bit LUT
			{
				// Other Depths are not yet supported...
			}

		}
	}
	return FALSE;
}


////////////////////////////////////////////////////////////////////////////////
// .Title       : getInputLUTVectorResult
// -----------------------------------------------------------------------------
// .Description : Returns input LUT Vector. Is comming from LUTEquation friend.
////////////////////////////////////////////////////////////////////////////////
// .History
//	 Date		Author		Comment                                       
//  :19.03.2000 RO			First Implementation
////////////////////////////////////////////////////////////////////////////////
SVByteValueObjectClass* SVLUTOperatorClass::getInputLUTVectorResult()
{
	if( m_inputLUTVectorResult.IsConnected() && m_inputLUTVectorResult.GetInputObjectInfo().PObject )
		return dynamic_cast <SVByteValueObjectClass*> (m_inputLUTVectorResult.GetInputObjectInfo().PObject);

	return NULL;
}



////////////////////////////////////////////////////////////////////////////////
// .Title       : OnValidate
// -----------------------------------------------------------------------------
// .Description : ...
//              :
////////////////////////////////////////////////////////////////////////////////
// .History
//	 Date		Author		Comment                                       
//  :16.03.2000 RO			First Implementation
////////////////////////////////////////////////////////////////////////////////
BOOL SVLUTOperatorClass::OnValidate() 
{
	return SVUnaryImageOperatorClass::OnValidate();
}

////////////////////////////////////////////////////////////////////////////////
// .Title       : onRun
// -----------------------------------------------------------------------------
// .Description : Runs this operator.
//              : Returns FALSE, if operator cannot run ( may be deactivated ! )
////////////////////////////////////////////////////////////////////////////////
// .History
//	 Date		Author		Comment                                       
//  :16.03.2000 RO			First Implementation
////////////////////////////////////////////////////////////////////////////////
BOOL SVLUTOperatorClass::onRun( BOOL First, SVSmartHandlePointer RInputImageHandle, SVSmartHandlePointer ROutputImageHandle, SVRunStatusClass& RRunStatus )
{ 
	// Is doing special friend routing !!!
	// Don't call base class onRun(...).

	// Since we do special routing here, we have to validate by ourself...
	if( ! OnValidate() || RInputImageHandle.empty() || ROutputImageHandle.empty() )
	{
		// Signal that something was wrong...
		SetInvalid();
		RRunStatus.SetInvalid();
		return FALSE;
	}

	SVImageBufferHandleImage l_InMilHandle;
	SVImageBufferHandleImage l_OutMilHandle;

	RInputImageHandle->GetData( l_InMilHandle );
	ROutputImageHandle->GetData( l_OutMilHandle );

	BOOL bUseLUT  = FALSE;
	BOOL bContinuousRecalcLUT = FALSE;

	// Check, if LUT shall run...
	if( S_OK != m_useLUT.GetValue( bUseLUT ) )
	{
		// Signal that something was wrong...
		SetInvalid();
		RRunStatus.SetInvalid();
		return FALSE;
	}

	// LUT Operator is disabled...
	if( ! bUseLUT )
	{
		// Disable only this operator...
		SetDisabled();

		// Do not return TRUE here !!!
		//return TRUE;

		// Signal that this operator was not running...
		// But everything was ok!!!!
		return FALSE;
	}

	// Get shall LUT be continuous recalculated...
	if( S_OK != m_continuousRecalcLUT.GetValue( bContinuousRecalcLUT ) )
	{
		// Signal that something was wrong...
		SetInvalid();
		RRunStatus.SetInvalid();
		return FALSE;
	}

	if( m_bForceLUTRecalc || bContinuousRecalcLUT )
	{
		// Try to recalc LUT...
		if( ! RecalcLUT( RRunStatus ) )
		{
			// Signal that something was wrong...
			SetInvalid();
			RRunStatus.SetInvalid();
			return FALSE;
		}

		m_bForceLUTRecalc = false;
	}

	SVMatroxImageInterface::SVStatusCode l_Code;

	// Do LUT...
	l_Code = SVMatroxImageInterface::LutMap( l_OutMilHandle.GetBuffer(),
		( First == TRUE ) ? l_InMilHandle.GetBuffer() :	l_OutMilHandle.GetBuffer(), 
			m_lutBufID);

	if( l_Code != SVMEE_STATUS_OK )
	{
		// Signal that something was wrong...
		SetInvalid();
		RRunStatus.SetInvalid();
		return FALSE;
	}

	// Success...
	return TRUE;
}

//******************************************************************************
//* LOG HISTORY:
//******************************************************************************
/*
$Log:   N:\PVCSarch65\ProjectFiles\archives\SVObserver_SRC\SVObserver\SVLUTOperator.cpp_v  $
 * 
 *    Rev 1.1   01 Oct 2013 14:57:22   tbair
 * Project:  SVObserver
 * Change Request (SCR) nbr:  852
 * SCR Title:  Add Multiple Platform Support to SVObserver's Visual Studio Solution
 * Checked in by:  tBair;  Tom Bair
 * Change Description:  
 *   Add x64 platform.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.0   23 Apr 2013 12:17:08   bWalter
 * Project:  SVObserver
 * Change Request (SCR) nbr:  814
 * SCR Title:  Upgrade SVObserver to Compile Using Visual Studio 2010
 * Checked in by:  bWalter;  Ben Walter
 * Change Description:  
 *   Initial check in to SVObserver_src.  (Merged with svo_src label SVO 6.10 Beta 008.)
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.25   12 Jul 2012 14:58:04   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  769
 * SCR Title:  Fix Problems and Crashes with Inspection Document Display Updates
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated source code to add in an additional string identifier for the input and fixed clear functionality assoicated with input structure.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.24   19 Jun 2012 12:57:12   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  744
 * SCR Title:  Add Shared Memory and Socket Functionality for Run Page Web Server
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Protected Object Information structure attributes.  This change will add accessor methods to get and set attributes.
 * Converted GUID to SVGUID.   This allows the use of standard operators for comparison and methods for conversion.
 * 
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.23   10 Feb 2011 14:58:16   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  712
 * SCR Title:  Fix issues with black images when using command interface (SIAC)
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated source code to now use shared pointer instead of untracked structure.  This fixes destruction, tracking and rebuilding issues.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.22   27 Jan 2011 11:28:40   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  712
 * SCR Title:  Fix issues with black images when using command interface (SIAC)
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated source code to fix issues with Matrox Buffer Management.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.21   04 Nov 2010 13:43:32   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  707
 * SCR Title:  Change Inspection Display Functionality to Force Display of Last Inspected
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated source code to fix issues with image processing and display image processing classes and associated includes.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.20   01 Jun 2010 14:52:14   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  693
 * SCR Title:  Fix Performance Issue with Inspection Process
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated source code to improve ability to track performance.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.19   24 Jul 2007 14:27:10   tbair
 * Project:  SVObserver
 * Change Request (SCR) nbr:  597
 * SCR Title:  Upgrade Matrox Imaging Library to version 8.0
 * Checked in by:  tBair;  Tom Bair
 * Change Description:  
 *   Update to MIL 8.0
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.18   19 Feb 2007 16:37:00   Joe
 * Project:  SVObserver
 * Change Request (SCR) nbr:  594
 * SCR Title:  Remove issue with interpreting status response incorrectly
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated MIL error code methodology to use the new SVMatroxAppManager GetStatusCode operator.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.17   15 Feb 2007 11:47:32   Joe
 * Project:  SVObserver
 * Change Request (SCR) nbr:  594
 * SCR Title:  Remove issue with interpreting status response incorrectly
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated flags for the mappgeterror fucntion to remove the M_MESSAGE flag.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.16   15 Sep 2005 07:39:46   Joe
 * Project:  SVObserver
 * Change Request (SCR) nbr:  500
 * SCR Title:  Reduce delay when adjusting tool parameters with a large toolset
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated code to fix problems with formula recalc.  Added run friends to recalc functionality.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.15   14 Sep 2005 13:16:20   Joe
 * Project:  SVObserver
 * Change Request (SCR) nbr:  500
 * SCR Title:  Reduce delay when adjusting tool parameters with a large toolset
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated methodology to recalc lut during onRun method by setting a flag when the reset object method is run.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.14   08 Aug 2005 13:53:30   ebeyeler
 * Project:  SVObserver
 * Change Request (SCR) nbr:  464
 * SCR Title:  Add array indexing for value objects
 * Checked in by:  eBeyeler;  Eric Beyeler
 * Change Description:  
 *   turn off SV_VIEWABLE flag for LUT array
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.13   02 Aug 2005 09:37:04   ryoho
 * Project:  SVObserver
 * Change Request (SCR) nbr:  464
 * SCR Title:  Add array indexing for value objects
 * Checked in by:  eBeyeler;  Eric Beyeler
 * Change Description:  
 *   On RecalcLUT do a call to SetArraySize before doing a SetArrayValue
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.12   01 Aug 2005 12:00:14   ebeyeler
 * Project:  SVObserver
 * Change Request (SCR) nbr:  464
 * SCR Title:  Add array indexing for value objects
 * Checked in by:  eBeyeler;  Eric Beyeler
 * Change Description:  
 *   changed VectorObject to ValueObject
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.11   29 Jul 2005 13:07:08   Joe
 * Project:  SVObserver
 * Change Request (SCR) nbr:  500
 * SCR Title:  Reduce delay when adjusting tool parameters with a large toolset
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated object initialization to change the parameters for RegisterEmbeddedObject for a SVValueObjectClass.  The two additional parameters will inform the preperation process before toolset execution.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.10   22 Jun 2005 09:39:28   ebeyeler
 * Project:  SVObserver
 * Change Request (SCR) nbr:  464
 * SCR Title:  Add array indexing for value objects
 * Checked in by:  eBeyeler;  Eric Beyeler
 * Change Description:  
 *   object attributes use accessor functions
 * value object and vector object use SVHRESULT
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.9   19 May 2005 14:52:54   ebeyeler
 * Project:  SVObserver
 * Change Request (SCR) nbr:  482
 * SCR Title:  New vector based mask
 * Checked in by:  eBeyeler;  Eric Beyeler
 * Change Description:  
 *   added m_ prefix to member variables
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.8   17 Feb 2005 14:44:14   Joe
 * Project:  SVObserver
 * Change Request (SCR) nbr:  456
 * SCR Title:  Update Image and Tool Objects to use the new Extent Classes
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Update class with new methodologies based on new base object, extents and reset structure.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.7   11 Jul 2003 08:18:20   Joe
 * Project:  SVObserver
 * Change Request (SCR) nbr:  320
 * SCR Title:  Integrate Matrox Meteor II / 1394 Board and 1394 camera into SVObserver
 * Checked in by:  eBeyeler;  Eric Beyeler
 * Change Description:  
 *   Updated RecalcLUT method to use the Display system handle.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.6   22 Apr 2003 11:07:12   rschock
 * Project:  SVObserver
 * Change Request (SCR) nbr:  346
 * SCR Title:  Update SVObserver to Version 4.21 Release
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Redid the #include defines and standardized the Tracker log headers and removed warning from release mode builds.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.5   30 Jan 2003 13:10:22   joe
 * Project:  SVObserver
 * Change Request (SCR) nbr:  301
 * SCR Title:  Implement Result Image Circle Buffer
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated RecalcLUT method to use the new image circle buffer methodology.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.4   09 Dec 2002 19:37:46   Joe
 * Project:  SVObserver
 * Change Request (SCR) nbr:  295
 * SCR Title:  Remove Result Data from Configuration Printout
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   CreateObject method was modified to properly update the isCreated flag and to set the printability of the internal value objects.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.3   19 Nov 2002 15:29:26   ryoho
 * Project:  SVObserver
 * Change Request (SCR) nbr:  226
 * SCR Title:  Monochrome SVIM configuration compatibility between ViperQUAD and ViperDUAL
 * Checked in by:  rYoho;  Rob Yoho
 * Change Description:  
 *   Removed references to System
 * Changed ValueObject semantics to work with buckets
 *             Changed use of SVObjectByteArrayClass to SVByteValueObjectClass
 * 
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.2   04 Oct 2001 16:19:12   Joe
 * Project:  SVObserver
 * Change Request (SCR) nbr:  229
 * SCR Title:  Upgrade SVObserver to Version 3.34 Release
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Modified source code to remove compiler warning message.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.1   12 Jun 2001 15:55:56   ed
 * Project:  SVObserver
 * Change Request (SCR) nbr:  200
 * SCR Title:  Update code to implement SVIM COM Server
 * Checked in by:  Ed;  Ed Chobanoff
 * Change Description:  
 *   Rearranged #include statements.
 * Change all occurences of SVByteVector to SVObjectByteArrayClass.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.0   23 Mar 2000 14:51:18   sjones
 * Project:  SVObserver
 * Change Request (SCR) nbr:  118
 * SCR Title:  Integration of LUT Operator
 * Checked in by:  sJones;  Steve Jones
 * Change Description:  
 *   Initial checkin.
 * Provides Identity, Inversion,Sign,Clip,Formula, and Free Form LUT.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
*/
