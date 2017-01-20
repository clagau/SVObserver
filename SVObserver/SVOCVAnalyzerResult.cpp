//******************************************************************************
//* COPYRIGHT (c) 2003 by SVResearch, Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVOCVAnalyzerResult
//* .File Name       : $Workfile:   SVOCVAnalyzerResult.cpp  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.5  $
//* .Check In Date   : $Date:   15 May 2014 11:07:30  $
//******************************************************************************

#pragma region Includes
#include "stdafx.h"
#include "SVOCore/SVImageProcessingClass.h"
#include "SVMessage/SVMessage.h"
#include "SVImageLibrary/SVImageBufferHandleImage.h"
#include "SVImageLibrary/SVImageBufferHandleInterface.h"
#include "SVRunControlLibrary/SVRunControlLibrary.h"
#include "SVUtilityLibrary/SVUtilityGlobals.h"
#include "SVOCore/SVImageClass.h"
#include "SVOCVAnalyzer.h"
#include "SVOCVAnalyzerResult.h"
#include "SVTool.h"
#include "SVOLicenseManager/SVOLicenseManager.h"
#include "SVStatusLibrary\MessageManager.h"
#include "TextDefinesSvO.h"
#include "ObjectInterfaces\ErrorNumbers.h"
#include "SVStatusLibrary\MessageManager.h"
#include "CameraLibrary/SVDeviceParams.h" //Arvid added to avoid VS2015 compile Error

#pragma endregion Includes

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

#define CAR_RETURN 0x0D
#define OCV_MAX_RESULTS 32L

SV_IMPLEMENT_CLASS( SVOCVAnalyzeResultClass, SVOCVAnalyzerResultClassGuid );

#define SV_ARC_ERROR 0xc0000000   //SV Analyzer Result Class Error.

SVOCVAnalyzeResultClass::SVOCVAnalyzeResultClass( SVObjectClass* POwner, int StringResourceID  )
						:SVResultClass(FALSE, POwner, StringResourceID )
{
	m_lCurrentFoundStringLength = 0;

	if ( TheSVOLicenseManager().HasMatroxIdentificationLicense() )
	{
		m_bHasLicenseError = false;
	}
	else
	{
		m_bHasLicenseError = true;
	}	
	
	clearAll();
	m_bUseOverlays = true;
}


void SVOCVAnalyzeResultClass::clearAll()
{	
	// Identify yourself
	m_outObjectInfo.ObjectTypeInfo.ObjectType = SVResultObjectType;
	m_outObjectInfo.ObjectTypeInfo.SubType = SVResultOCVObjectType;

	// Identify our input type needs
	inputImageObjectInfo.SetInputObjectType( SVImageObjectType );
	inputImageObjectInfo.SetObject( GetObjectInfo() );
	RegisterInputObject( &inputImageObjectInfo, _T( "OCVAnalyzerResultImage" ) );

	// Register Embedded Objects
	RegisterEmbeddedObject(
		&m_svoMatchString, 
		SVOCVMatchStringGuid,
		IDS_OBJECTNAME_OCV_MATCH_STRING,
		false, SVResetItemNone );
	
	RegisterEmbeddedObject( 
		&m_svoFoundString, 
		SVOCVFoundStringGuid,
		IDS_OBJECTNAME_OCV_FOUND_STRING,
		false, SVResetItemNone );
	
	RegisterEmbeddedObject( 
		&m_bvoPerformOCR,
		SVOCVPerformOCRGuid,
		IDS_OBJECTNAME_OCV_PERFORM_OCR,
		false, SVResetItemNone );
	
	RegisterEmbeddedObject( 
		&m_lvoMatchLineNumber,
		SVOCVMatchLineNumberGuid,
		IDS_OBJECTNAME_OCV_MATCH_LINE_NUMBER,
		false, SVResetItemNone );
	
	// Exposing OCV Match Scores
	RegisterEmbeddedObject( 
		&m_dvoHighestMatchScore,
		SVOCVHighestMatchScoreGuid,
		IDS_OBJECTNAME_OCV_HIGHEST_MATCH_SCORE,
		false, SVResetItemNone );
	
	RegisterEmbeddedObject( 
		&m_dvoLowestMatchScore,
		SVOCVLowestMatchScoreGuid,
		IDS_OBJECTNAME_OCV_LOWEST_MATCH_SCORE,
		false, SVResetItemNone );
	
	RegisterEmbeddedObject( 
		&m_dvoAverageMatchScore,
		SVOCVAverageMatchScoreGuid,
		IDS_OBJECTNAME_OCV_AVERAGE_MATCH_SCORE,
		false, SVResetItemNone );
	
	// OCV File Names
	RegisterEmbeddedObject( 
		&m_fnvoFontFileName,
		SVOCVFontFileNameGuid,
		IDS_OBJECTNAME_OCV_FONT_FILE_NAME,
		false, SVResetItemOwner );
	
	RegisterEmbeddedObject( 
		&m_fnvoConstraintsFileName,
		SVOCVConstraintsFileNameGuid,
		IDS_OBJECTNAME_OCV_CONSTRAINTS_FILE_NAME,
		false, SVResetItemOwner );
	
	RegisterEmbeddedObject( 
		&m_fnvoControlsFileName,
		SVOCVControlsFileNameGuid,
		IDS_OBJECTNAME_OCV_CONTROLS_FILE_NAME,
		false, SVResetItemOwner );
	
	RegisterEmbeddedObject(
		&m_fnvoMatchStringFileName,
		SVOCVMatchStringFileNameGuid,
		IDS_OBJECTNAME_OCV_MATCH_STRING_FILE_NAME,
		false, SVResetItemOwner );
	
	RegisterEmbeddedObject( 
		&m_bvoUseMatchFile,
		SVOCVUseMatchFileGuid,
		IDS_OBJECTNAME_OCV_USE_MATCH_FILE,
		false, SVResetItemOwner );
	
	for( long l = 0; l < OCV_MAX_RESULTS; l++ )
	{
		SVOCVCharacterResultClass *pResult = new SVOCVCharacterResultClass( this );
		// Add it to our task object list
		Add( pResult );

		SVString Name = SvUl_SF::Format( "Character %d Result", l + 1 );
		pResult->SetName( Name.c_str() );
	}// end for

	// Set Embedded defaults
	m_svoMatchString.SetDefaultValue( _T( "" ), TRUE );
	m_svoFoundString.SetDefaultValue( _T( "" ), TRUE );

	m_lvoMatchLineNumber.SetDefaultValue ( (long) -1, TRUE );
	m_bvoPerformOCR.SetDefaultValue ( (long) 1, TRUE );
	m_bvoUseMatchFile.SetDefaultValue( FALSE, TRUE );

	// Setting exposed OCV Match Score defaults
	m_dvoHighestMatchScore.SetDefaultValue( (double) -1.0, TRUE );
	m_dvoLowestMatchScore.SetDefaultValue( (double) -1.0, TRUE );
	m_dvoAverageMatchScore.SetDefaultValue( (double) -1.0, TRUE );

	// Setting exposed OCV File Name defaults
	m_fnvoFontFileName.SetDefaultValue( _T(""), TRUE );
	m_fnvoConstraintsFileName.SetDefaultValue( _T(""), TRUE );
	m_fnvoControlsFileName.SetDefaultValue( _T(""), TRUE );
	m_fnvoMatchStringFileName.SetDefaultValue( _T(""), TRUE );
	m_bvoUseMatchFile.SetDefaultValue( FALSE, TRUE );

	// Add Default Inputs and Outputs
	addDefaultInputObjects();

	//
	// Initialize OCV objects
	//
	m_lFontStringLength = 0;
	m_lFontStringLengthMax = 0;
	m_lMatchStringLength = 0;

	m_pBuffer = nullptr;
	m_pIndexTable = nullptr;
}

SVOCVAnalyzeResultClass::~SVOCVAnalyzeResultClass()
{
	// First destroy the MIL font and result buffer

	if ( !m_bHasLicenseError )
	{
		SVMatroxOcrInterface ::SVStatusCode l_Code;

		if( !m_milFontID.empty() )
		{
			l_Code = SVMatroxOcrInterface::Destroy( m_milFontID );
		}// end if

		if( !m_milResultID.empty() )
		{
			l_Code = SVMatroxOcrInterface::Destroy( m_milResultID );
		}// end if

		if( m_pBuffer )
		{
			delete [] m_pBuffer;
		}

		if( m_pIndexTable )
		{
			delete [] m_pIndexTable;
		}
	}
}


SVImageClass* SVOCVAnalyzeResultClass::getInputImage()
{
	if( inputImageObjectInfo.IsConnected() && 
		inputImageObjectInfo.GetInputObjectInfo().PObject )
	{
		return (SVImageClass*) inputImageObjectInfo.GetInputObjectInfo().PObject;
	}
	return nullptr;
}

BOOL SVOCVAnalyzeResultClass::CreateObject(	SVObjectLevelCreateStruct* PCreateStructure )
{
	BOOL bOk = SVResultClass::CreateObject( PCreateStructure );

	if ( bOk )
	{
		bOk = nullptr != getInputImage();
	}
	
	if ( bOk && !m_bHasLicenseError )
	{
		// Load Match String, if necessary...
		LoadMatchString();

		// Generate Font Model, if necessary...
		GenerateFontModel();
	}

	// Set / Reset Printable Flag
	m_svoMatchString.ObjectAttributesAllowedRef() |= SV_PRINTABLE | SV_SETABLE_ONLINE | SV_REMOTELY_SETABLE;
	m_svoFoundString.ObjectAttributesAllowedRef() &= ~SV_PRINTABLE;
	m_lvoMatchLineNumber.ObjectAttributesAllowedRef() &= ~SV_PRINTABLE;
	m_bvoPerformOCR.ObjectAttributesAllowedRef() = SV_PRINTABLE | SV_SETABLE_ONLINE | SV_REMOTELY_SETABLE;

	m_dvoHighestMatchScore.ObjectAttributesAllowedRef() &= ~SV_PRINTABLE;
	m_dvoLowestMatchScore.ObjectAttributesAllowedRef() &= ~SV_PRINTABLE;
	m_dvoAverageMatchScore.ObjectAttributesAllowedRef() &= ~SV_PRINTABLE;

	m_fnvoFontFileName.ObjectAttributesAllowedRef() |= SV_PRINTABLE | SV_SETABLE_ONLINE | SV_REMOTELY_SETABLE;
	m_fnvoConstraintsFileName.ObjectAttributesAllowedRef() |= SV_PRINTABLE | SV_SETABLE_ONLINE | SV_REMOTELY_SETABLE;
	m_fnvoControlsFileName.ObjectAttributesAllowedRef() |= SV_PRINTABLE | SV_SETABLE_ONLINE | SV_REMOTELY_SETABLE;
	m_fnvoMatchStringFileName.ObjectAttributesAllowedRef() |= SV_PRINTABLE | SV_SETABLE_ONLINE | SV_REMOTELY_SETABLE;
	m_bvoUseMatchFile.ObjectAttributesAllowedRef() = SV_PRINTABLE | SV_SETABLE_ONLINE | SV_REMOTELY_SETABLE;

	HideResults();

	m_isCreated = bOk;

	return bOk;
}

void SVOCVAnalyzeResultClass::HideResults()
{
	if ( !m_bHasLicenseError )
	{
		// Clear our results array
		arrayOCVCharacterResults.RemoveAll();

		for( long l = 0; l < OCV_MAX_RESULTS; l++ )
		{
			SVOCVCharacterResultClass *pResult = (SVOCVCharacterResultClass*) GetAt( l );
			pResult->ObjectAttributesAllowedRef() = SV_EMBEDABLE;

			if( l < m_lFontStringLength )
			{
				pResult->UnhideResults();
			}// end if
			else
			{
				pResult->HideResults();
			}// end else

			// Add it to our results array
			arrayOCVCharacterResults.Add( pResult );
		}// end for
	}// end for
}

/////////////////////////////////////////////////////////////////////////////
//
//
//
//
BOOL SVOCVAnalyzeResultClass::CloseObject()
{
	BOOL bOk = FALSE;

	if( SVResultClass::CloseObject() )
	{
		bOk = TRUE;
		
		SVMatroxOcrInterface ::SVStatusCode l_Code;

		// First destroy the MIL font and result buffer
		if( !m_milFontID.empty() )
		{
			l_Code = SVMatroxOcrInterface::Destroy( m_milFontID );
		}// end if

		if( !m_milResultID.empty() )
		{
			l_Code = SVMatroxOcrInterface::Destroy( m_milResultID );
		}// end if
	}

	return bOk;
}

//******************************************************************************
// Operation(s) Of Reading Access:
//******************************************************************************

void SVOCVAnalyzeResultClass::GetOCVResultString( SVString& rResult )
{
   BOOL bResult = ( S_OK == m_svoFoundString.GetValue( rResult ) );
   ASSERT(bResult);
}


////////////////////////////////////////////////////////////////////////////////
// .Title       : GenerateFontModel ()
// -----------------------------------------------------------------------------
// .Description : Generates the mil font model from the Font file and the
//				  entered sampling rate.
////////////////////////////////////////////////////////////////////////////////
BOOL SVOCVAnalyzeResultClass::GenerateFontModel()
{
	BOOL bOk = TRUE;
	long	l_lIsFontPreprocessed = 0;

	if ( bOk )
	{
		SVMatroxOcrInterface ::SVStatusCode l_Code;
		// First destroy the MIL font and result buffer
		if( !m_milFontID.empty() )
		{
			l_Code = SVMatroxOcrInterface::Destroy( m_milFontID );
		}// end if

		if( !m_milResultID.empty() )
		{
			l_Code = SVMatroxOcrInterface::Destroy( m_milResultID );
		}// end if

	}// end if

	if ( bOk )
	{
		// Now recreate the MIL font and result buffer
		CFileStatus rStatus;
		SVString FontFileName;
		SVString ConstraintsFileName;
		SVString ControlsFileName;

		m_fnvoFontFileName.GetValue( FontFileName );
		bOk = CFile::GetStatus( FontFileName.c_str(), rStatus );
		if ( bOk )
		{
			bOk = 0L < rStatus.m_size;
		}// end if

		if( bOk )
		{
			m_fnvoControlsFileName.GetValue( ControlsFileName );
			if( !ControlsFileName.empty() )
			{
				bOk = CFile::GetStatus( ControlsFileName.c_str(), rStatus );
				if ( bOk )
				{
					bOk = 0L < rStatus.m_size;
				}// end if

			}// end if

		}// end if

		if( bOk )
		{
			m_fnvoConstraintsFileName.GetValue( ConstraintsFileName );
			if( !ConstraintsFileName.empty() )
			{
				bOk = CFile::GetStatus( ConstraintsFileName.c_str(), rStatus );
				if ( bOk )
				{
					bOk = 0L < rStatus.m_size;
				}// end if

			}// end if

		}// end if

		if ( bOk )
		{
			// Allocate a MIL kernel.
			
			
			SVMatroxOcrInterface ::SVStatusCode l_Code;

			SVString Path;
			Path = FontFileName;

			l_Code = SVMatroxOcrInterface::RestoreFont( m_milFontID, Path, SVOcrRestore );
			if( m_milFontID.empty() )
			{
				SvStl::MessageMgrStd MesMan( SvStl::LogOnly );
				MesMan.setMessage( SVMSG_SVO_103_REPLACE_ERROR_TRAP, SvOi::Tid_UnexpectedError, SvStl::SourceFileParams(StdMessageParams), SvOi::Err_16126);
			
			}// end if
			
			if( !ControlsFileName.empty() )
			{
				Path = ControlsFileName;
				l_Code = SVMatroxOcrInterface::RestoreFont( m_milFontID, Path, SVOcrLoadControl );
				if( m_milFontID.empty() )
				{
					SvStl::MessageMgrStd MesMan( SvStl::LogOnly );
					MesMan.setMessage( SVMSG_SVO_103_REPLACE_ERROR_TRAP, SvOi::Tid_UnexpectedError, SvStl::SourceFileParams(StdMessageParams), SvOi::Err_16127);
				}// end if

			}// end if

			if( !ConstraintsFileName.empty() )
			{
				Path = ConstraintsFileName;
				l_Code = SVMatroxOcrInterface::RestoreFont( m_milFontID, Path, SVOcrLoadConstraint );

				if( m_milFontID.empty() )
				{
					SvStl::MessageMgrStd MesMan( SvStl::LogOnly );
					MesMan.setMessage( SVMSG_SVO_103_REPLACE_ERROR_TRAP, SvOi::Tid_UnexpectedError, SvStl::SourceFileParams(StdMessageParams), SvOi::Err_16128);

				}// end if

			}// end if

//-			JAB111208
			l_Code = SVMatroxOcrInterface::Get (m_milFontID, 
												SVOcrIsFontPreprocessed,
												l_lIsFontPreprocessed);
			if (l_Code != SVMEE_STATUS_OK)
			{
				SvStl::MessageMgrStd MesMan( SvStl::LogOnly );
				MesMan.setMessage( SVMSG_SVO_103_REPLACE_ERROR_TRAP, SvOi::Tid_UnexpectedError, SvStl::SourceFileParams(StdMessageParams), SvOi::Err_16129);
			}

			if (l_lIsFontPreprocessed == 0)
			{
				l_Code = SVMatroxOcrInterface::Preprocess( m_milFontID );

				if (l_Code != SVMEE_STATUS_OK)
				{
					SvStl::MessageMgrStd MesMan( SvStl::LogOnly );
					MesMan.setMessage( SVMSG_SVO_103_REPLACE_ERROR_TRAP, SvOi::Tid_UnexpectedError, SvStl::SourceFileParams(StdMessageParams), SvOi::Err_16130);
				}
			}

			l_Code = SVMatroxOcrInterface::Create( m_milResultID );
			if( m_milResultID.empty() )
			{
				SvStl::MessageMgrStd MesMan( SvStl::LogOnly );
				MesMan.setMessage( SVMSG_SVO_103_REPLACE_ERROR_TRAP, SvOi::Tid_UnexpectedError, SvStl::SourceFileParams(StdMessageParams), SvOi::Err_16131);
			}// end if

				l_Code = SVMatroxOcrInterface::Get( m_milFontID, SVOcrStringSize, m_lFontStringLength );
				if( m_lFontStringLength < 1 )
				{
					SvStl::MessageMgrStd MesMan( SvStl::LogOnly );
					MesMan.setMessage( SVMSG_SVO_103_REPLACE_ERROR_TRAP, SvOi::Tid_UnexpectedError, SvStl::SourceFileParams(StdMessageParams), SvOi::Err_16132);
				}// end if

				l_Code = SVMatroxOcrInterface::Get( m_milFontID, SVOcrStringSizeMax, m_lFontStringLengthMax );
				if( m_lFontStringLengthMax < 1 )
				{
					SvStl::MessageMgrStd MesMan( SvStl::LogOnly );
					MesMan.setMessage( SVMSG_SVO_103_REPLACE_ERROR_TRAP, SvOi::Tid_UnexpectedError, SvStl::SourceFileParams(StdMessageParams), SvOi::Err_16133);
				}// end if

			if( m_lFontStringLength == SVValueAny )
			{
				m_lFontStringLength = m_lFontStringLengthMax;
			}// end if

			if ( 0 < m_lMatchStringLength &&
				   m_lMatchStringLength != m_lFontStringLength && 
			     m_lMatchStringLength <= m_lFontStringLengthMax )
			{
					l_Code = SVMatroxOcrInterface::Set( m_milFontID, SVOcrStringSize, m_lMatchStringLength);

					l_Code = SVMatroxOcrInterface::Get( m_milFontID, SVOcrStringSize, m_lFontStringLength );
					if( m_lFontStringLength < 1 )
					{
						SvStl::MessageMgrStd MesMan( SvStl::LogOnly );
						MesMan.setMessage( SVMSG_SVO_103_REPLACE_ERROR_TRAP, SvOi::Tid_UnexpectedError, SvStl::SourceFileParams(StdMessageParams), SvOi::Err_16134);
					}// end if
			}

		}// end if

	}// end if

	HideResults();
	return bOk;
}

HRESULT SVOCVAnalyzeResultClass::LoadMatchString()
{
	//
	// Check to see if match string is to read from a file.
	//
	HRESULT hrRet = S_OK;
	BOOL bUseFile;
	
	DWORD LastError(0);
	
	m_lMatchStringLength = 0;
	
	while (1)
	{
	   m_nTotalCount = 0;
		 
		 m_bvoUseMatchFile.GetValue( bUseFile );
		 if( bUseFile == TRUE )
		 {		   
			 //
			 // Check to see if the file exists..
			 //
			 SVString MatchStringFileName;
			 m_fnvoMatchStringFileName.GetValue( MatchStringFileName  );
			 if( ! ::SVFileExists( MatchStringFileName.c_str() ) )
			 {
				 //
				 // The file does not exist.  Set error and set defaults.
				 //
				 hrRet = S_FALSE;
				 SvStl::MessageMgrStd MesMan( SvStl::LogOnly );
				 MesMan.setMessage( SVMSG_SVO_103_REPLACE_ERROR_TRAP, SvOi::Tid_UnexpectedError, SvStl::SourceFileParams(StdMessageParams), SvOi::Err_16194);
				 LastError = -SvOi::Err_16194;
				 break;
			 }
			 
			 //
			 // Open and read the OCV match string from the file.
			 //
			 CFile matchFile;
			 if( !matchFile.Open( MatchStringFileName.c_str(), CFile::modeRead | CFile::shareDenyNone ) )
			 {
				 hrRet = S_FALSE;
				 SvStl::MessageMgrStd MesMan( SvStl::LogOnly );
				 MesMan.setMessage( SVMSG_SVO_103_REPLACE_ERROR_TRAP, SvOi::Tid_UnexpectedError, SvStl::SourceFileParams(StdMessageParams), SvOi::Err_16195);
				 LastError = SvOi::Err_16195;
				 break;
			 }
			 
			 
			unsigned __int64 fileLen = matchFile.GetLength();
			DWORD dwByteNumber = static_cast<unsigned long>(fileLen);

			if(m_pBuffer)
				 delete m_pBuffer;
			 m_pBuffer = new char[dwByteNumber+1];
			 if(!m_pBuffer)
			 {
				 hrRet = S_FALSE;
				 SvStl::MessageMgrStd MesMan( SvStl::LogOnly );
				 MesMan.setMessage( SVMSG_SVO_103_REPLACE_ERROR_TRAP, SvOi::Tid_UnexpectedError, SvStl::SourceFileParams(StdMessageParams), SvOi::Err_16196);
				 LastError = SvOi::Err_16196;
				 break;
			 }
			 
			 // Copy entire file into the buffer.
			 m_lTotalBytes = matchFile.Read( m_pBuffer, (UINT)dwByteNumber );
			 matchFile.Close();
			 m_pBuffer[m_lTotalBytes] = 0;
			 
			 if (0 != LastError)
			 {
				 m_svoMatchString.SetValue( 1, _T( "" ) );
			 }
			 
			 if ( dwByteNumber > 0 )
			 {
				 if( !BuildHashTable( m_pBuffer ) )
				 {
					 m_nTotalCount = 0;
					 SvStl::MessageMgrStd MesMan( SvStl::LogOnly );
					 MesMan.setMessage( SVMSG_SVO_103_REPLACE_ERROR_TRAP, SvOi::Tid_UnexpectedError, SvStl::SourceFileParams(StdMessageParams), SvOi::Err_16197);
					 LastError = SvOi::Err_16197;
					 break;
				 }
			 
				m_lMatchStringLength = static_cast< long >( strlen( m_pDataArr[ 0 ] ) );
			 }
			 else
			 {
				 hrRet = S_FALSE;
			 }
			 
			 break;
		 }// end if
		 else
		 {
			 SVString MatchString;
			 
			 m_svoMatchString.GetValue( MatchString );
			 
			 m_lMatchStringLength = static_cast<long> (MatchString.size());
		 }
		 
		 break;
	} // while (1)
	
	if (0 != LastError)
	{
	   m_svoMatchString.SetValue( 1, _T( "" ) );
	}

	return hrRet;
}


BOOL SVOCVAnalyzeResultClass::OnValidate()
{
	BOOL bRetVal = true;

	bRetVal = bRetVal && inputImageObjectInfo.IsConnected();
	bRetVal = bRetVal && nullptr != inputImageObjectInfo.GetInputObjectInfo().PObject;
	bRetVal = bRetVal && m_lMatchStringLength == m_lFontStringLength;
	bRetVal = bRetVal && m_lMatchStringLength <= m_lFontStringLengthMax;
	bRetVal = bRetVal && !m_milFontID.empty();
	bRetVal = bRetVal && SVResultClass::OnValidate();

	// Note: Make sure this is called when Validate fails !!!
	if( ! bRetVal )
		SetInvalid();

	return bRetVal;
}

/////////////////////////////////////////////////////////////////////////////
//
//
//
bool SVOCVAnalyzeResultClass::resetAllObjects( bool shouldNotifyFriends, bool silentReset )
{
	bool Result = ( S_OK == ResetObject() );
	ASSERT( Result );

	return Result;
}

HRESULT SVOCVAnalyzeResultClass::ResetObject()
{
	HRESULT l_hrOk = SVResultClass::ResetObject();

	if ( !m_bHasLicenseError )
	{
		//
		// Read the match string from a possible match string file.
		//
		LoadMatchString();

		//
		// Regenerate the Font Model.
		//
		GenerateFontModel();
	}
	return l_hrOk;
}

/////////////////////////////////////////////////////////////////////////////
//
//
//
//
BOOL SVOCVAnalyzeResultClass::onRun( SVRunStatusClass& RRunStatus )
{
	BOOL bOk = FALSE;
	SVMatroxBuffer l_milImageID;
	BYTE* pMilBuffer = nullptr;
	BOOL l_bOperation;
	BOOL bUseFile;
	BOOL bStringPassed;
	BOOL bCharsPassed;

	long l_lLength = 0;

	double l_dHigh;
	double l_dLow;
	double l_dAvg;
	double l_dSum;
	double l_dValidString = 0.0f;
	double l_dNbrString = 0.0f;
	SVString l_strLabel;
	SVMatroxDoubleArray l_adScores;
	SVMatroxDoubleArray l_adXCoords;
	SVMatroxDoubleArray l_adYCoords;
	SVMatroxDoubleArray l_adValidChars;
	double l_dScore;
	double l_dLength = 0.0f;
	double l_dCharBoxSizeX;
	double l_dCharBoxSizeY;

	SVString FoundString;
	SVString MatchString;
	 
	SVMatroxOcrInterface ::SVStatusCode l_Code = SVMEE_STATUS_OK;
  
	bOk = SVResultClass::onRun( RRunStatus );

	if( bOk && !RRunStatus.IsDisabled() && !RRunStatus.IsDisabledByCondition() )
	{

		if( bOk )
		{
			SVImageClass* pImage = getInputImage();

			bOk = nullptr != pImage;
			
			if( !bOk )
			{    
				SetInvalid();
			}
			else
			{
				SVSmartHandlePointer ImageHandle;

				if( pImage->GetImageHandle( ImageHandle ) && !( ImageHandle.empty() ) )
				{
					SVImageBufferHandleImage l_MilHandle;
					ImageHandle->GetData( l_MilHandle );

					l_milImageID = l_MilHandle.GetBuffer();

					//
					// Get the Mil buffer host pointer ( address in the process address space )
					//
					
					SVMatroxBufferInterface::GetHostAddress( &pMilBuffer, l_milImageID );				
					bOk = nullptr != pMilBuffer;

					if( ! bOk )
					{
						//
						// No MIL image buffer handle..
						//
						SvStl::MessageMgrStd Msg( SvStl::LogAndDisplay );
						Msg.setMessage( SVMSG_SVO_93_GENERAL_WARNING, SvOi::Tid_Error_NoMilHostBuffer, SvStl::SourceFileParams(StdMessageParams), SvOi::Err_10169);
					}
					else
					{
						//
						// Check type of image buffer
						//
						long imageTypeMil = 0;
						SVMatroxBufferInterface::Get( l_milImageID, SVType, imageTypeMil );

						if ( imageTypeMil != 8)  // (8L + M_UNSIGNED) )
						{
							SVStringVector msgList;
							msgList.push_back(SvUl_SF::Format(_T("%x"), imageTypeMil));
							SvStl::MessageMgrStd Msg( SvStl::LogAndDisplay );
							Msg.setMessage( SVMSG_SVO_93_GENERAL_WARNING, SvOi::Tid_Error_MilImageTypeInvalid, msgList, SvStl::SourceFileParams(StdMessageParams), SvOi::Err_10170);
						}
					}
				}
			}
		}

		if( bOk )
		{
			//
			// Turn off mouse events for duration of this method.
			//
			bOk = lock.Lock( 1 );

			if( !bOk )
			{
				SetInvalid();
			}
			else
			{
				long k( 0 );
				// Reset the 'found' string to nothing.
				m_svoFoundString.SetValue( RRunStatus.m_lResultDataIndex, _T("") );
				m_lvoMatchLineNumber.SetValue( RRunStatus.m_lResultDataIndex, (const long) -1L );

				//
				// Preset for failure
				//
				failed.SetValue( RRunStatus.m_lResultDataIndex, TRUE );
				warned.SetValue( RRunStatus.m_lResultDataIndex, TRUE );
				passed.SetValue( RRunStatus.m_lResultDataIndex, FALSE );

				m_dvoHighestMatchScore.SetValue( RRunStatus.m_lResultDataIndex, (double) 0.0f );
				m_dvoLowestMatchScore.SetValue(  RRunStatus.m_lResultDataIndex, (double) 0.0f );
				m_dvoAverageMatchScore.SetValue( RRunStatus.m_lResultDataIndex, (double) 0.0f );

				//
				// Get some settings.
				//
				m_bvoPerformOCR.GetValue( l_bOperation );

				// Reset results
				
				SVString strFunctionName;
				int iProgramCode=-12382;

				try
				{
					while (1)
					{
						//
						// Run the OCV by calling MocrReadString();
						//
						bOk = !m_milFontID.empty();
						if (!bOk)
						{
							l_Code = -12395;
							break;
						}

						if( l_bOperation ) // read
						{
							m_milFontID.m_bVerify = false;
							strFunctionName = _T("MocrReadString"); iProgramCode = -12383;
							l_Code = SVMatroxOcrInterface::Execute( m_milResultID , m_milFontID, l_milImageID );

							if ((l_Code & 0xc0000000) != 0)
							{
								l_Code = -12396;
								break;
							}
						}// if( l_bOperation )
						else
						{ // verify
							m_bvoUseMatchFile.GetValue( bUseFile );
							if( bUseFile )
							{
								MatchString = m_pDataArr[0];
							}// end if
							else
							{
								m_svoMatchString.GetValue( MatchString );
							}// end else

							strFunctionName = _T("MocrVerifyString"); iProgramCode = -12384;
							m_milFontID.m_VerifyString = MatchString;
							m_milFontID.m_bVerify = true;
							l_Code = SVMatroxOcrInterface::Execute( m_milResultID , 
								                                    m_milFontID, 
																	l_milImageID );
							if ((l_Code & 0xc0000000) != 0)
							{
								l_Code = -12397;
								break;
							}
						}// end else //verify

						//
						// Process the OCV chars returned from MocrReadString();
						//
						l_dValidString = 0.0f;
						l_dNbrString = 0.0f;
						l_dLength = 0.0f;
						strFunctionName = _T("MocrGetResult");


						iProgramCode = -12386;


						// Before we try to get any additional results we MUST make sure that
						// we have a string to get results from. Otherwise Matrox gets flaky.
						l_Code = SVMatroxOcrInterface::GetResult(  m_milResultID, 
							                                      SVOcrNbrString, 
																  l_dNbrString );
						if( l_dNbrString < 1 )
						{
							// There is no reason to go on at this point 
							// and in fact going on will cause trouble.
							break;
						}

						l_Code = SVMatroxOcrInterface::GetResult( m_milResultID, 
							                                      SVOcrStringValidFlag, 
																  l_dValidString );
						if ((l_Code & ~0xcf000000) == 23002)
						{
//-							JAB111008 - This represents... Specified result 
//-							does not contain valid read or verify result 
//-							data, which we are currently considerring OK.
//-
//-							This seems to be occurring if we do not find a 
//-							match string... which is an exceptable condition.
//-							However, MIL 8 manual says no match string should
//-							give back an l_dValidString value of 0.  Why do 
//-							we get a MIL error?
							l_Code = 0;

//-							It is assumed that l_lStringFound still equals 
//-							false.
							break;
						}

						if ((l_Code & 0xc0000000) != 0)
						{
							break;
						}

//-						A string was found!

						iProgramCode = -12385;
						l_Code = SVMatroxOcrInterface::GetResult( m_milResultID, 
																  SVOcrResultStringSize, 
																  l_dLength );


						if ((l_Code & 0xc0000000) != 0)
						{
							l_Code = -12398;
							break;
						}

						l_lLength = (long) l_dLength;
						m_lCurrentFoundStringLength = l_lLength;
						if (l_lLength == 0)
						{
							break;
						}

						l_strLabel.resize( l_lLength );
						l_adScores.resize( l_lLength );
						l_adXCoords.resize( l_lLength );
						l_adYCoords.resize( l_lLength );
						l_adValidChars.resize( l_lLength );

						strFunctionName = _T("MocrGetResult");
						iProgramCode = -12387;
						l_Code = SVMatroxOcrInterface::GetResult( m_milResultID, SVOcrStringScore, l_dScore);
						if ((l_Code & 0xc0000000) != 0)
						{
							l_Code = -12399;
							break;
						}

						iProgramCode = -12388;
						l_Code = SVMatroxOcrInterface::GetResult( m_milResultID, SVOcrString, l_strLabel);
						if ((l_Code & 0xc0000000) != 0)
						{
							l_Code = -12400;
							break;
						}

						iProgramCode = -12389;
						l_Code = SVMatroxOcrInterface::GetResult( m_milResultID, SVOcrCharScore, l_adScores);

						if ((l_Code & 0xc0000000) != 0)
						{
							l_Code = -12401;
							break;
						}

						iProgramCode = -12390;
						l_Code = SVMatroxOcrInterface::GetResult( m_milResultID, SVOcrCharPosX, l_adXCoords);

						if ((l_Code & 0xc0000000) != 0)
						{
							l_Code = -12402;
							break;
						}

						iProgramCode = -12391;
						l_Code = SVMatroxOcrInterface::GetResult( m_milResultID, SVOcrCharPosY, l_adYCoords);

						if ((l_Code & 0xc0000000) != 0)
						{
							l_Code = -12403;
							break;
						}

						iProgramCode = -12392;
						l_Code = SVMatroxOcrInterface::GetResult( m_milResultID, SVOcrCharValidFlag, l_adValidChars);
						if ((l_Code & 0xc0000000) != 0)
						{
							l_Code = -12404;
							break;
						}

						strFunctionName = _T("MocrInquire");
						iProgramCode = -12393;
						l_Code = SVMatroxOcrInterface::Get( m_milFontID, SVCharCellSizeX, l_dCharBoxSizeX);
						if ((l_Code & 0xc0000000) != 0)
						{
							l_Code = -12405;
							break;
						}

						strFunctionName = _T("MocrInquire");
						iProgramCode = -12394;
						l_Code = SVMatroxOcrInterface::Get( m_milFontID, SVCharCellSizeY, l_dCharBoxSizeY);
						if ((l_Code & 0xc0000000) != 0)
						{
							l_Code = -12406;
							break;
						}

						break;
					} //while (1)

					if( l_lLength == 0 )
					{
						l_strLabel.empty();
					}

					l_dHigh = 0.0f;
					l_dLow  = 100.0f;
					l_dAvg  = 0.0f;
					l_dSum	= 0.0f;
					if( l_lLength > OCV_MAX_RESULTS )
						l_lLength = OCV_MAX_RESULTS;
				}// end try
				catch( ... ) // Access violation in : 5eec0a51 milpat!MpatFindMultipleModelUnpack
				{
					assert (0);
					// clear floating-point exception register; allow other downstream fp routines to work properly
					unsigned int iError = _clearfp() & _MCW_EM;

					if ( iError != 0 )
					{
						RRunStatus.SetInvalid();
						RRunStatus.SetFailed();
					}

					SvStl::MessageMgrStd Exception( SvStl::LogOnly );
					Exception.setMessage( SVMSG_SVO_30_EXCEPTION_IN_MIL, strFunctionName.c_str(), SvStl::SourceFileParams(StdMessageParams), iProgramCode );

					l_lLength = 0;
				}// end catch


				for( k = 0; k < l_lLength; k++ ) // For critical error cases 
					                             // the length must be 0 at 
												 // this point.
				{
					SVOCVCharacterResultClass *pResult = arrayOCVCharacterResults.GetAt( k );

					pResult->m_cvoLabelValue.SetValue( RRunStatus.m_lResultDataIndex, l_strLabel[k] );

					pResult->m_dvoOverlayLeft.SetValue( RRunStatus.m_lResultDataIndex, l_adXCoords[k] - ( l_dCharBoxSizeX / 2.0f ) );
					pResult->m_dvoOverlayTop.SetValue( RRunStatus.m_lResultDataIndex, l_adYCoords[k] - ( l_dCharBoxSizeY / 2.0f ) );
					pResult->m_dvoOverlayWidth.SetValue( RRunStatus.m_lResultDataIndex, l_dCharBoxSizeX );
					pResult->m_dvoOverlayHeight.SetValue( RRunStatus.m_lResultDataIndex, l_dCharBoxSizeY );

					pResult->m_dvoMatchScore.SetValue( RRunStatus.m_lResultDataIndex, l_adScores[k] );

					if( l_adScores[k] > l_dHigh )
						l_dHigh = l_adScores[k];

					if( l_adScores[k] < l_dLow )
						l_dLow = l_adScores[k];

					l_dSum += l_adScores[k];

				}// end for

//-				Set unused indexes.
				for( long l = l_lLength; l < OCV_MAX_RESULTS; l++ )
				{
					SVOCVCharacterResultClass *pResult = arrayOCVCharacterResults.GetAt( l );

					pResult->m_cvoLabelValue.SetValue( RRunStatus.m_lResultDataIndex, 0 );

					pResult->m_dvoOverlayLeft.SetValue( RRunStatus.m_lResultDataIndex, 0.0f );
					pResult->m_dvoOverlayTop.SetValue( RRunStatus.m_lResultDataIndex, 0.0f );
					pResult->m_dvoOverlayWidth.SetValue( RRunStatus.m_lResultDataIndex, 0.0f );
					pResult->m_dvoOverlayHeight.SetValue( RRunStatus.m_lResultDataIndex, 0.0f );

					pResult->m_dvoMatchScore.SetValue( RRunStatus.m_lResultDataIndex, -1.0f );
				}// end for

				if( l_lLength )
				{
					FoundString = l_strLabel.c_str();
					l_dAvg = l_dSum / l_lLength;
				}// end if
				else
				{
					FoundString = _T( "" );
					bOk = TRUE;
				}// end else

				// Determine MIL pass/fail status 
				bStringPassed = l_dValidString == SVValueTrue;
				bCharsPassed = TRUE;
				for( k = 0; k < l_lLength; k++ )
				{
					bCharsPassed &= l_adValidChars[k] == SVValueTrue;
				}// end for

				// Cleanup results

				m_dvoHighestMatchScore.SetValue( RRunStatus.m_lResultDataIndex, (double) l_dHigh );
				m_dvoLowestMatchScore.SetValue(  RRunStatus.m_lResultDataIndex, (double) l_dLow );
				m_dvoAverageMatchScore.SetValue( RRunStatus.m_lResultDataIndex, (double) l_dAvg );

				//
				// Copy OCV result to a storage array element.
				//
				m_svoFoundString.SetValue( RRunStatus.m_lResultDataIndex, FoundString.c_str() );

				m_svoMatchString.GetValue( MatchString );

				//
				// Is result == matchstring?
				//
				int nResultValue = 0;
				if( l_bOperation )
				{
					if( m_nTotalCount > 1 )
					{
						int nStringMatch;
						if( ( nStringMatch = CheckStringInTable( FoundString ) ) == -1 )
						{
							if( bStringPassed && bCharsPassed )
							{
								nResultValue = 2; // Warned
							}// end if
							else
							{
								nResultValue = 3; // Failed
							}// end else

						}// end if
						else
						{
							if( bStringPassed && bCharsPassed )
							{
								nResultValue = 1; // passed
							}// end if
							else
							{
								nResultValue = 2; // Warned
							}// end else

						}// end else

						m_lvoMatchLineNumber.SetValue( RRunStatus.m_lResultDataIndex, (long) nStringMatch );
					}// end if
					else // Handle the old way
					{
						if( FoundString == MatchString )
						{
							if( bStringPassed && bCharsPassed )
							{
								nResultValue = 1; // passed
							}// end if
							else
							{
								nResultValue = 2; // Warned
							}// end else
						
							if( m_nTotalCount == 1 )
							{
								m_lvoMatchLineNumber.SetValue( RRunStatus.m_lResultDataIndex, (const long) 1L );
							}// end if

						}// end if
						else
						{
							if( bStringPassed && bCharsPassed )
							{
								nResultValue = 2; // Warned
							}// end if
							else
							{
								nResultValue = 3; // Failed
							}// end else

						}// end else

					}// end else

				}// end if
				else
				{
					if( FoundString == MatchString )
					{
						if( bStringPassed && bCharsPassed )
						{
							nResultValue = 1; // passed
						}// end if
						else
						{
							nResultValue = 2; // Warned
						}// end else

					}// end if
					else
					{
						if( bStringPassed && bCharsPassed )
						{
							nResultValue = 2; // Warned
						}// end if
						else
						{
							nResultValue = 3; // Failed
						}// end else

					}// end else

				}// end else

				switch ( nResultValue )
				{
					case 1: // Passed
					{
						failed.SetValue( RRunStatus.m_lResultDataIndex, FALSE );
						warned.SetValue( RRunStatus.m_lResultDataIndex, FALSE );
						passed.SetValue( RRunStatus.m_lResultDataIndex, TRUE );

						RRunStatus.SetPassed();

						break;
					}
					case 2: // Warned
					{
						failed.SetValue( RRunStatus.m_lResultDataIndex, FALSE );
						warned.SetValue( RRunStatus.m_lResultDataIndex, TRUE );

						RRunStatus.SetWarned();

						break;
					}
					default: // Failed 
					{
						RRunStatus.SetFailed();

						break;
					}

				}// end switch

			}// end else

			lock.Unlock();
		}// end if

	}// end if

	if (l_Code & SV_ARC_ERROR)
	{
		assert (0);
		SetInvalid();
		RRunStatus.SetInvalid();

		SVStringVector msgList;
		msgList.push_back(_T("SVOCVAnalyzeResultClass::onRun"));

		SvStl::MessageMgrStd Exception( SvStl::LogOnly );
		Exception.setMessage( static_cast<DWORD> (l_Code), SvOi::Tid_ErrorIn, msgList, SvStl::SourceFileParams(StdMessageParams) );

		bOk = false;
	}

	return bOk;
}

BOOL SVOCVAnalyzeResultClass::BuildHashTable( char *pBuffer )
{
	long lBufIndex = 0;
	BOOL  bRet = TRUE;
	
	for(int i = 0; i < OCV_MAX_ENTREES; i++)
		m_pDataArr[i] = 0;
	
	for(m_nTotalCount = 0; 
	(m_nTotalCount < OCV_MAX_ENTREES) && (lBufIndex < m_lTotalBytes);
	m_nTotalCount++)
	{
		int nCharCount = 0;
		long lIndexValue = 0;
		// Assign each line into a char pointer array.
		m_pDataArr[m_nTotalCount] = &pBuffer[lBufIndex];
		
		while(pBuffer[lBufIndex] != CAR_RETURN && (lBufIndex < m_lTotalBytes))
		{
			// The index value is calculated as follows.
			// nValue = ASCII value of char at position index - ASCII of first displayable character
			// lIndexValue = Square of nValue * Position of the char from left.
			nCharCount++;
			int   nValue = pBuffer[lBufIndex++] - 0x20;
			lIndexValue += nValue * nValue * nCharCount;
		}
		if(lBufIndex < m_lTotalBytes)
		{
			pBuffer[lBufIndex++] = 0; // Carriage Return
			lBufIndex++; // skip the Line feed
		}
		if(!nCharCount) // If there is a blank line skip that one.
		{
			m_nTotalCount--;
			continue;
		}
		// We are calculating m_lLowValue and m_lHighValue because the string corresponding to
		// m_lLowValue should go to index 0 and the string for m_lHighValue should have the max index
		if(!m_nTotalCount)
			m_lLowValue = m_lHighValue = lIndexValue;
		
		else if(lIndexValue < m_lLowValue)
			m_lLowValue = lIndexValue; 
		else if(lIndexValue > m_lHighValue)
			m_lHighValue = lIndexValue; 
	}
	
	// Allocate a table that is 10 times the size of m_nTotalCount
	while(1)
	{
		if(m_pIndexTable)
			delete m_pIndexTable;
		// If there is only one entry in the file, make it work the old way.   
		if(m_nTotalCount == 1)
		{
			m_svoMatchString.SetValue( 1, (LPTSTR)pBuffer );
			m_pIndexTable = nullptr;
			break; // No need to create hash table
		}
		
		m_pIndexTable = new short[m_nTotalCount * 10];
		if(!m_pIndexTable)
		{
			bRet = FALSE;
			break;
		}
		
		memset(m_pIndexTable, 0, (size_t)(sizeof(short) * m_nTotalCount * 10));
		
		// Now calculate the hash table index for each enrty in the m_pDataArr
		
		m_dFactor = ((double)(m_lHighValue - m_lLowValue)) / (double)m_nTotalCount;
		
		for(short Count = 0; Count < m_nTotalCount ; Count++)
		{
			char *pData = m_pDataArr[Count];
			int nCharCount = static_cast< int >( strlen( pData ) );
			long lIndexValue = 0;
			for (int i = 0; i < nCharCount; i++)
			{
				int   nValue = pData[i] - 0x20;
				lIndexValue += nValue * nValue * (i + 1);
			}
			// We are multiplying by 9.5 since we want to reserve some slots at the end of the table,
			// in case we get the max index for multiple strings.
			
			double dActualIndex = (((double)(lIndexValue - m_lLowValue)) / m_dFactor) * 9.5;
			
			// Line number will be count  + 1
			InsertValueToTable(Count + 1, (int) dActualIndex);
		}
		break;
	}
	return bRet;
}

////////////////////////////////////////////////////////////////////////////////
// .Title       : InsertValueToTable(short nValue, int index)
// -----------------------------------------------------------------------------
// .Description : Inserts 'nValue' to the Hash Table at location 'nIndex'. If there is 
//              : a data at that location, 'nIndex' is incremented till we get an open spot.
//              : 'nValue' is the index of the string in the m_pDataArr, whose hash table index 
//              : is 'nIndex'.
//              : 
////////////////////////////////////////////////////////////////////////////////
// 
//	 Date		   Author		Comment
//  4-25-00    sri			First Implementation
//	:
////////////////////////////////////////////////////////////////////////////////
void SVOCVAnalyzeResultClass::InsertValueToTable ( short nValue, int nIndex )
{
	// Check whether the location contains a data
	
	if(m_pIndexTable[nIndex] == 0)
		m_pIndexTable[nIndex] = nValue;
	else
	{
		++nIndex;
		while(m_pIndexTable[nIndex] != 0)
		{
			nIndex++;
			// if we reach the end of the hash table, start from the begining( starting from 3rd position) 
			if(nIndex >= m_nTotalCount * 10 )
				nIndex = 2;
		}
		
		m_pIndexTable[nIndex] = nValue;
	}
} 

////////////////////////////////////////////////////////////////////////////////
// .Title       : CheckStringInTable(CString MatchString)
// -----------------------------------------------------------------------------
// .Description : Calculate Hash table index for 'MatchString'. Get the data from 
//              : hash table at that location. This will be the index of the string 
//              : in m_pDataArr. Check to see whether the two strings match.
//              : Returns the index of match string in m_pDataArr, which corresponds 
//              : to the line number in the file, and -1 if there is no match.
////////////////////////////////////////////////////////////////////////////////
// 
//	 Date		   Author		Comment
//  4-25-00    sri			First Implementation
//	:
////////////////////////////////////////////////////////////////////////////////
int SVOCVAnalyzeResultClass::CheckStringInTable(const SVString& rMatchString)
{
	int nReturnIndex = -1;
	
	long  lIndexValue = 0;
	size_t   nCharCount = rMatchString.size();
	
	for(size_t i = 0; i < nCharCount; i++)
	{
		long lValue = static_cast<LONG> (rMatchString[i] - 0x20);
		lIndexValue += lValue * lValue * static_cast<long> (i + 1);
	}
	
	// if Index value is out of range, definitely there won't be a match in the file.
	if(lIndexValue >= m_lLowValue && lIndexValue <= m_lHighValue)
	{
		int nActualIndex = (int)((((double)(lIndexValue - m_lLowValue)) / m_dFactor) * 9.5);
		// Check whether the string at this location is the matchString.
		
		while(m_pIndexTable[nActualIndex] != 0) 
		{
			TCHAR *pData = m_pDataArr[m_pIndexTable[nActualIndex] - 1];
			
			if( rMatchString == pData )
			{
				nReturnIndex = m_pIndexTable[nActualIndex];
				break;
			}
			nActualIndex++;
			
			// if we reach the end of the hash table, start from the begining( starting from 3rd position) 
			if(nActualIndex >= m_nTotalCount * 10 )
				nActualIndex = 2;
		}
	}
	return  nReturnIndex;
} 

HRESULT SVOCVAnalyzeResultClass::onCollectOverlays(SVImageClass* p_pImage, SVExtentMultiLineStructCArray& p_rMultiLineArray )
{
	HRESULT l_hr = S_OK;

	SVToolClass* pTool = dynamic_cast<SVToolClass*>(GetTool());
	if (pTool && pTool->WasEnabled())
	{
		long lLeft, lTop, lWidth, lHeight;

		if( m_lCurrentFoundStringLength )
		{
			SVImageExtentClass l_svExtents;
			pTool->GetImageExtent( l_svExtents);

			for( long i = 0; i < m_lCurrentFoundStringLength; i++ )
			{
				SVExtentFigureStruct l_svFigure;

				SVOCVCharacterResultClass *pResult = arrayOCVCharacterResults.GetAt( i );

				pResult->m_dvoOverlayLeft.GetValue( lLeft );
				pResult->m_dvoOverlayTop.GetValue( lTop );
				pResult->m_dvoOverlayWidth.GetValue( lWidth );
				pResult->m_dvoOverlayHeight.GetValue( lHeight );

				CRect l_oRect(lLeft, lTop, lLeft + lWidth, lTop + lHeight);

				l_svFigure = l_oRect;

				l_svExtents.TranslateFromOutputSpace( l_svFigure, l_svFigure);

				SVExtentMultiLineStruct l_multiLine;

				l_multiLine.m_Color = SV_DEFAULT_SUB_FUNCTION_COLOR_1;
					
				l_multiLine.AssignExtentFigure( l_svFigure, SV_DEFAULT_SUB_FUNCTION_COLOR_1 );

				UpdateOverlayIDs( l_multiLine );

				p_rMultiLineArray.Add( l_multiLine );

			}// end for
		}
	}

	return l_hr;
}

