//******************************************************************************
//* COPYRIGHT (c) 2003 by SVResearch, Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVOCVAnalyzerResult
//* .File Name       : $Workfile:   SVOCVAnalyzerResult.cpp  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.2  $
//* .Check In Date   : $Date:   13 May 2013 12:36:56  $
//******************************************************************************

#include "stdafx.h"
#include "SVImageProcessingClass.h"
#include "SVMessage/SVMessage.h"
#include "SVImageLibrary/SVImageBufferHandleImage.h"
#include "SVImageLibrary/SVImageBufferHandleInterface.h"
#include "SVRunControlLibrary/SVRunControlLibrary.h"
#include "SVStatusLibrary/SVException.h"
#include "SVGlobal.h"
#include "SVImageClass.h"
#include "SVOCVAnalyzer.h"
#include "SVOCVAnalyzerResult.h"
#include "SVTool.h"
#include "SVOLicenseManager/SVOLicenseManager.h"

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
	outObjectInfo.ObjectTypeInfo.ObjectType = SVResultObjectType;
	outObjectInfo.ObjectTypeInfo.SubType = SVResultOCVObjectType;

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
		CString strName;
		SVOCVCharacterResultClass *pResult = new SVOCVCharacterResultClass( this );

		// Add it to our task object list
		Add( pResult );

		strName.Format( "Character %d Result", l + 1 );
		pResult->SetName( strName );
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

	m_pBuffer = NULL;
	m_pIndexTable = NULL;
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
	return NULL;
}


BOOL SVOCVAnalyzeResultClass::CreateObject(	SVObjectLevelCreateStruct* PCreateStructure )
{
	BOOL bOk = FALSE;
	

	bOk = SVResultClass::CreateObject( PCreateStructure );

	if ( bOk )
	{
		bOk = getInputImage() != NULL;
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

	isCreated = bOk;

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

void SVOCVAnalyzeResultClass::GetOCVResultString( CString & csResult )
{
   BOOL bResult = ( m_svoFoundString.GetValue( csResult ) == S_OK );
   ASSERT(bResult);
}

/////////////////////////////////////////////////////////////////////////////
//
//
//
//
CRect SVOCVAnalyzeResultClass::Draw( HDC DC, CRect R )
{
	CString text;
	
	::SetTextColor( DC, SV_DEFAULT_TEXT_COLOR ); 

	text = _T("OCV Analyzer Owner TBD");         // 16 Jul 1999 - frb.

	::TextOut( DC, R.left, R.top, text, text.GetLength() );
	R.top += SV_DEFAULT_TEXT_HEIGHT;

	text = _T( "Match String:" );
	::TextOut( DC, R.left, R.top, text, text.GetLength() );
	CString csResult;
	m_svoMatchString.GetValue( csResult );
	text.Format( _T( "%s" ), (LPCTSTR)csResult );
	::TextOut( DC, R.left + 100, R.top, text, text.GetLength() );
	R.top += SV_DEFAULT_TEXT_HEIGHT + 2;

    DWORD dwColor = GetObjectColor();
	//resultColor.GetValue(dwColor);

	//::SetTextColor( DC, resultColor ); 
	::SetTextColor(DC,dwColor);

	text = _T( "OCV String:" );
	::TextOut( DC, R.left, R.top, text, text.GetLength() );
	m_svoFoundString.GetValue( csResult );
	text.Format( _T( "%s" ), (LPCTSTR)csResult );
	::TextOut( DC, R.left + 100, R.top, text, text.GetLength() );
	R.top += SV_DEFAULT_TEXT_HEIGHT + 2;

	return R;
}


////////////////////////////////////////////////////////////////////////////////
// .Title       : GenerateFontModel ()
// -----------------------------------------------------------------------------
// .Description : Generates the mil font model from the Font file and the
//				  entered sampling rate.
//              :
////////////////////////////////////////////////////////////////////////////////
// .History
//	 Date		Author		Comment
//  :19.03.1999 JB			First Implementation
//	:
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
		CString strFontFileName;
		CString strConstraintsFileName;
		CString strControlsFileName;

		m_fnvoFontFileName.GetValue( strFontFileName );
		bOk = CFile::GetStatus( strFontFileName, rStatus );
		if ( bOk )
		{
			bOk = 0L < rStatus.m_size;
		}// end if

		if( bOk )
		{
			m_fnvoControlsFileName.GetValue( strControlsFileName );
			if( !strControlsFileName.IsEmpty() )
			{
				bOk = CFile::GetStatus( strControlsFileName, rStatus );
				if ( bOk )
				{
					bOk = 0L < rStatus.m_size;
				}// end if

			}// end if

		}// end if

		if( bOk )
		{
			m_fnvoConstraintsFileName.GetValue( strConstraintsFileName );
			if( !strConstraintsFileName.IsEmpty() )
			{
				bOk = CFile::GetStatus( strConstraintsFileName, rStatus );
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

			SVMatroxString l_strPath;
			l_strPath = strFontFileName;

			l_Code = SVMatroxOcrInterface::RestoreFont( m_milFontID, l_strPath, SVOcrRestore );
			if( m_milFontID.empty() )
			{
				msvError.msvlErrorCd = l_Code | SVMEE_MATROX_ERROR;
				SV_TRAP_ERROR (msvError, 33331);
			}// end if
			
			if( !strControlsFileName.IsEmpty() )
			{
				l_strPath = strControlsFileName;
				l_Code = SVMatroxOcrInterface::RestoreFont( m_milFontID, l_strPath, SVOcrLoadControl );
				if( m_milFontID.empty() )
				{
					msvError.msvlErrorCd = l_Code | SVMEE_MATROX_ERROR;
					SV_TRAP_ERROR (msvError, 33332);
				}// end if

			}// end if

			if( !strConstraintsFileName.IsEmpty() )
			{
				l_strPath = strConstraintsFileName;
				l_Code = SVMatroxOcrInterface::RestoreFont( m_milFontID, l_strPath, SVOcrLoadConstraint );

				if( m_milFontID.empty() )
				{
					msvError.msvlErrorCd = l_Code | SVMEE_MATROX_ERROR;
					SV_TRAP_ERROR (msvError, 33333);
				}// end if

			}// end if

//-			JAB111208
			l_Code = SVMatroxOcrInterface::Get (m_milFontID, 
												SVOcrIsFontPreprocessed,
												l_lIsFontPreprocessed);
			if (l_Code != SVMEE_STATUS_OK)
			{
				msvError.msvlErrorCd = l_Code | SVMEE_MATROX_ERROR;
				SV_TRAP_ERROR (msvError, 33338);
			}

			if (l_lIsFontPreprocessed == 0)
			{
				l_Code = SVMatroxOcrInterface::Preprocess( m_milFontID );

				if (l_Code != SVMEE_STATUS_OK)
				{
					msvError.msvlErrorCd = l_Code | SVMEE_MATROX_ERROR;
					SV_TRAP_ERROR (msvError, 33339);
				}
			}

			l_Code = SVMatroxOcrInterface::Create( m_milResultID );
			if( m_milResultID.empty() )
			{
				msvError.msvlErrorCd = l_Code | SVMEE_MATROX_ERROR;
				SV_TRAP_ERROR (msvError, 33334);
			}// end if

				l_Code = SVMatroxOcrInterface::Get( m_milFontID, SVOcrStringSize, m_lFontStringLength );
				if( m_lFontStringLength < 1 )
				{
					msvError.msvlErrorCd = l_Code | SVMEE_MATROX_ERROR;
					SV_TRAP_ERROR (msvError, 33335);
				}// end if

				l_Code = SVMatroxOcrInterface::Get( m_milFontID, SVOcrStringSizeMax, m_lFontStringLengthMax );
				if( m_lFontStringLengthMax < 1 )
				{
					msvError.msvlErrorCd = l_Code | SVMEE_MATROX_ERROR;
					SV_TRAP_ERROR (msvError, 33336);
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
						msvError.msvlErrorCd = l_Code | SVMEE_MATROX_ERROR;
						SV_TRAP_ERROR (msvError, 33337);
					}// end if
			}

		}// end if

	}// end if

	HideResults();
	return bOk;
}


void  SVOCVAnalyzeResultClass::WriteToArchive(CFile & fileArchive)
{
    //
    // Write the match string to the archive.
    //
	CString strTemp;
	CString csResult;

	m_svoFoundString.GetValue( csResult );
	strTemp.Format("OCV result: %s\r\n", (LPCTSTR)csResult );

    TRY
	{
	    fileArchive.Write((LPCTSTR)strTemp,strTemp.GetLength());
	}
	CATCH_ALL(e)
	{
	} 
	END_CATCH_ALL;
}


HRESULT SVOCVAnalyzeResultClass::LoadMatchString()
{
	//
	// Check to see if match string is to read from a file.
	//
	HRESULT hrRet = S_OK;
	CString strMatchString;
	BOOL bUseFile;
	
	msvError.ClearLastErrorCd ();
	msvError.msvlErrorCd = 0;
	
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
			 CString strMatchStringFileName;
			 m_fnvoMatchStringFileName.GetValue( strMatchStringFileName  );
			 if( ! ::SVFileExists( strMatchStringFileName ) )
			 {
				 //
				 // The file does not exist.  Set error and set defaults.
				 //
				 hrRet = S_FALSE;
				 msvError.msvlErrorCd = -1304;
				 SV_TRAP_ERROR_BRK_TSTFIRST (msvError, 1304);
			 }
			 
			 //
			 // Open and read the OCV match string from the file.
			 //
			 CFile matchFile;
			 if( !matchFile.Open( strMatchStringFileName, CFile::modeRead | CFile::shareDenyNone ) )
			 {
				 hrRet = S_FALSE;
				 msvError.msvlErrorCd = -1305;
				 SV_TRAP_ERROR_BRK_TSTFIRST (msvError, 1305);
			 }
			 
			 
			unsigned __int64 fileLen = matchFile.GetLength();
			DWORD dwByteNumber = static_cast<unsigned long>(fileLen);

			if(m_pBuffer)
				 delete m_pBuffer;
			 m_pBuffer = new char[dwByteNumber+1];
			 if(!m_pBuffer)
			 {
				 hrRet = S_FALSE;
				 msvError.msvlErrorCd = -25051;
				 SV_TRAP_ERROR_BRK_TSTFIRST (msvError, 25051);
			 }
			 
			 // Copy entire file into the buffer.
			 m_lTotalBytes = matchFile.Read( m_pBuffer, (UINT)dwByteNumber );
			 matchFile.Close();
			 m_pBuffer[m_lTotalBytes] = 0;
			 
			 if (msvError.GetLastErrorCd () & SV_ERROR_CONDITION)
			 {
				 m_svoMatchString.SetValue( 1, _T( "" ) );
			 }
			 
			 if ( dwByteNumber > 0 )
			 {
				 if( !BuildHashTable( m_pBuffer ) )
				 {
					 m_nTotalCount = 0;
					 msvError.msvlErrorCd = -25052;
					 SV_TRAP_ERROR_BRK_TSTFIRST (msvError, 25052);
				 }
			 
				m_lMatchStringLength = strlen( m_pDataArr[0] );
			 }
			 else
			 {
				 hrRet = S_FALSE;
			 }
			 
			 break;
		 }// end if
		 else
		 {
			 CString l_strMatch;
			 
			 m_svoMatchString.GetValue( l_strMatch );
			 
			 m_lMatchStringLength = l_strMatch.GetLength();
		 }
		 
		 break;
	} // while (1)
	
	if (msvError.GetLastErrorCd () & SV_ERROR_CONDITION)
	{
	   m_svoMatchString.SetValue( 1, _T( "" ) );
	}

	return hrRet;
}


BOOL SVOCVAnalyzeResultClass::OnValidate()
{
	BOOL bRetVal = TRUE;

	bRetVal = bRetVal && inputImageObjectInfo.IsConnected();
	bRetVal = bRetVal && inputImageObjectInfo.GetInputObjectInfo().PObject != NULL;
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
DWORD SVOCVAnalyzeResultClass::processMessage( DWORD DwMessageID, DWORD DwMessageValue, DWORD DwMessageContext )
{
	DWORD DwResult = NULL;

	// Try to process message by yourself...
	DWORD dwPureMessageID = DwMessageID & SVM_PURE_MESSAGE;
	switch( dwPureMessageID )
	{
	case SVMSGID_RESET_ALL_OBJECTS:
		{
			HRESULT l_ResetStatus = ResetObject();
			if( l_ResetStatus != S_OK )
			{
				ASSERT( SUCCEEDED( l_ResetStatus ) );

				DwResult = SVMR_NO_SUCCESS;
			}
			else
			{
				DwResult = SVMR_SUCCESS;
			}
			break;
		}
	default:
		{
			DwResult = SVResultClass::processMessage( DwMessageID, DwMessageValue, DwMessageContext );
			break;
		}
	}

	return DwResult;
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
	BYTE* pMilBuffer = NULL;
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
	SVMatroxString l_strLabel;
	SVMatroxDoubleArray l_adScores;
	SVMatroxDoubleArray l_adXCoords;
	SVMatroxDoubleArray l_adYCoords;
	SVMatroxDoubleArray l_adValidChars;
	double l_dScore;
	double l_dLength = 0.0f;
	double l_dCharBoxSizeX;
	double l_dCharBoxSizeY;

	SVMatroxString l_strFound;
	SVMatroxString l_strMatch;
	 
	SVMatroxOcrInterface ::SVStatusCode l_Code = SVMEE_STATUS_OK;
  
	bOk = SVResultClass::onRun( RRunStatus );

	if( bOk && !RRunStatus.IsDisabled() && !RRunStatus.IsDisabledByCondition() )
	{

		if( bOk )
		{
			SVImageClass* pImage = getInputImage();

			bOk = pImage != NULL;
			
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
					bOk = pMilBuffer != NULL;

					if( ! bOk )
					{
						//
						// No MIL image buffer handle..
						//
						AfxMessageBox( _T("ERROR: No Mil Host Buffer Pointer") );
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
							CString s;
							s.Format("ERROR: MIL Image Type Not 8 Bit Unsigned: %x",imageTypeMil);
							AfxMessageBox(s);
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
				
				CString strFunctionName;
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
								l_strMatch = m_pDataArr[0];
							}// end if
							else
							{
								CString l_strTmp;
								m_svoMatchString.GetValue( l_strTmp );
								l_strMatch = l_strTmp;
							}// end else

							strFunctionName = _T("MocrVerifyString"); iProgramCode = -12384;
							m_milFontID.m_VerifyString = l_strMatch;
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

				/*
				// MILOCR 5ef91ee2 - 0xC0000090: Float Invalid Operation.
				// how to catch just fp exception???  use _controlfp/_clearfp to see if flag is set in (...)
				// or GetExceptionCode()???  --> only with __try / __except
				catch ( fpexception& e )
				{
					_clearfp();	// clear floating-point exception register; allow other downstream fp routines to work properly
				}
				//*/

				///*
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

					SVException e;
					SETEXCEPTION5( e, SVMSG_SVO_30_EXCEPTION_IN_MIL, iProgramCode, strFunctionName );
					e.LogException( strFunctionName );

					l_lLength = 0;
				}// end catch
				//*/

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
					l_strFound = l_strLabel.c_str();
					l_dAvg = l_dSum / l_lLength;
				}// end if
				else
				{
					l_strFound = _T( "" );
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
				m_svoFoundString.SetValue( RRunStatus.m_lResultDataIndex, l_strFound.c_str() );

				CString l_strTmp;
				m_svoMatchString.GetValue( l_strTmp );
				l_strMatch = l_strTmp;

				//
				// Is result == matchstring?
				//
				int nResultValue = 0;
				if( l_bOperation )
				{
					if( m_nTotalCount > 1 )
					{
						int nStringMatch;
						if( ( nStringMatch = CheckStringInTable( l_strFound.c_str() ) ) == -1 )
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
						if( l_strFound == l_strMatch )
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
					if( l_strFound == l_strMatch )
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

//	if( !bOk )
	if (l_Code & SV_ARC_ERROR)
	{
		SVException l_svLog;
		CString		l_csMessage;

		assert (0);
		SetInvalid();
		RRunStatus.SetInvalid();

		l_csMessage.Format ("Error in SVThresholdClass::onRun");
		SETEXCEPTION1( l_svLog, l_Code, l_csMessage);
		l_svLog.LogException();

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
			m_pIndexTable = NULL;
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
			int nCharCount = strlen(pData);
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
int SVOCVAnalyzeResultClass::CheckStringInTable(CString MatchString)
{
	int nReturnIndex = -1;
	//   m_dFactor = ((double)(m_lHighValue - m_lLowValue)) / (double)m_nTotalCount;
	
	long  lIndexValue = 0;
	int   nCharCount = MatchString.GetLength();
	
	for (int i = 0; i < nCharCount; i++)
	{
		int   nValue = MatchString.GetAt(i) - 0x20;
		lIndexValue += nValue * nValue * (i + 1);
	}
	
	// if Index value is out of range, definitely there won't be a match in the file.
	if(lIndexValue >= m_lLowValue && lIndexValue <= m_lHighValue)
	{
		int nActualIndex = (int)((((double)(lIndexValue - m_lLowValue)) / m_dFactor) * 9.5);
		// Check whether the string at this location is the matchString.
		
		while(m_pIndexTable[nActualIndex] != 0) 
		{
			char *pData = m_pDataArr[m_pIndexTable[nActualIndex] - 1];
			
			if(MatchString  == (CString)pData)
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

	if (GetTool()->WasEnabled())
	{
		long lLeft, lTop, lWidth, lHeight;

		if( m_lCurrentFoundStringLength )
		{
			SVImageExtentClass l_svExtents;
			SVToolClass* pTool = GetTool();

			if( pTool )
			{
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
	}

	return l_hr;
}

//******************************************************************************
//* LOG HISTORY:
//******************************************************************************
/*
$Log:   N:\PVCSarch65\ProjectFiles\archives\SVObserver_src\SVObserver\SVOCVAnalyzerResult.cpp_v  $
 * 
 *    Rev 1.2   13 May 2013 12:36:56   bWalter
 * Project:  SVObserver
 * Change Request (SCR) nbr:  814
 * SCR Title:  Upgrade SVObserver to Compile Using Visual Studio 2010
 * Checked in by:  bWalter;  Ben Walter
 * Change Description:  
 *   Merged with svo_src label SVO 6.10 Beta 014.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.38   02 May 2013 11:21:46   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  810
 * SCR Title:  Remove support for Legacy Tools
 * Checked in by:  rYoho;  Rob Yoho
 * Change Description:  
 *   Updated process message method to fix formatting issues and updated the reset object section to only assert when the HRESULT is a failed status.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.1   06 May 2013 14:38:32   bWalter
 * Project:  SVObserver
 * Change Request (SCR) nbr:  814
 * SCR Title:  Upgrade SVObserver to Compile Using Visual Studio 2010
 * Checked in by:  bWalter;  Ben Walter
 * Change Description:  
 *   Merged with svo_src label SVO 6.10 Beta 009.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.37   10 Apr 2013 14:54:24   ryoho
 * Project:  SVObserver
 * Change Request (SCR) nbr:  796
 * SCR Title:  Add a License Manager that will handle the Matrox Licenses
 * Checked in by:  rYoho;  Rob Yoho
 * Change Description:  
 *   CreateObject put the license check before calling LoadMatchString and GenerateFontModel
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.0   23 Apr 2013 13:00:52   bWalter
 * Project:  SVObserver
 * Change Request (SCR) nbr:  814
 * SCR Title:  Upgrade SVObserver to Compile Using Visual Studio 2010
 * Checked in by:  bWalter;  Ben Walter
 * Change Description:  
 *   Initial check in to SVObserver_src.  (Merged with svo_src label SVO 6.10 Beta 008.)
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.36   27 Feb 2013 11:26:12   ryoho
 * Project:  SVObserver
 * Change Request (SCR) nbr:  796
 * SCR Title:  Add a License Manager that will handle the Matrox Licenses
 * Checked in by:  rYoho;  Rob Yoho
 * Change Description:  
 *   Put in checks to see if it has licnese errors
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.35   02 Nov 2012 12:43:52   tbair
 * Project:  SVObserver
 * Change Request (SCR) nbr:  797
 * SCR Title:  Fix Analyzer Overlay Issues
 * Checked in by:  tBair;  Tom Bair
 * Change Description:  
 *   Added OnCollectOverlays Override.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.34   25 Jul 2012 14:39:50   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  769
 * SCR Title:  Fix Problems and Crashes with Inspection Document Display Updates
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Removal of dead code.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.33   18 Jul 2012 14:15:00   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  769
 * SCR Title:  Fix Problems and Crashes with Inspection Document Display Updates
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Removed obsolete methods assiciated with overlay drawling and fixed problem with copying data to byte buffer have wrong format.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.32   12 Jul 2012 15:03:18   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  769
 * SCR Title:  Fix Problems and Crashes with Inspection Document Display Updates
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated source code to add in an additional string identifier for the input and fixed clear functionality assoicated with input structure.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.31   19 Jun 2012 13:11:00   jspila
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
 *    Rev 1.30   10 Feb 2011 15:05:24   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  712
 * SCR Title:  Fix issues with black images when using command interface (SIAC)
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated source code to now use shared pointer instead of untracked structure.  This fixes destruction, tracking and rebuilding issues.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.29   27 Jan 2011 11:42:22   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  712
 * SCR Title:  Fix issues with black images when using command interface (SIAC)
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated source code to fix issues with Matrox Buffer Management.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.28   04 Nov 2010 14:08:06   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  707
 * SCR Title:  Change Inspection Display Functionality to Force Display of Last Inspected
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated source code to fix issues with image processing and display image processing classes and associated includes.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.27   29 Jun 2010 14:08:40   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  693
 * SCR Title:  Fix Performance Issue with Inspection Process
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated source to change object validity test and casting changes.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.26   01 Jun 2010 14:57:58   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  693
 * SCR Title:  Fix Performance Issue with Inspection Process
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated source code to improve ability to track performance.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.25   16 Dec 2009 12:16:24   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  677
 * SCR Title:  Fix problem in camera notify thread
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Fix issues with includes and comments.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.24   28 Oct 2009 14:42:14   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  633
 * SCR Title:  Upgrade Intek FireStack Driver and API to version 2.32
 * Checked in by:  jSpila;  Joseph Spila
 * Change Description:  
 *   Updated code to fix indexing published images and setting defaults.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.23   28 Oct 2009 14:36:40   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  633
 * SCR Title:  Upgrade Intek FireStack Driver and API to version 2.32
 * Checked in by:  jSpila;  Joseph Spila
 * Change Description:  
 *   Updated code to fix indexing published images and setting defaults.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.22   07 Oct 2009 15:08:18   tbair
 * Project:  SVObserver
 * Change Request (SCR) nbr:  671
 * SCR Title:  Fix Character Analyzer Result Bug
 * Checked in by:  tBair;  Tom Bair
 * Change Description:  
 *   Added an additional test to get the number of strings from matrox before trying to get string results in the On Run Function.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.21   03 Sep 2009 10:35:10   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  665
 * SCR Title:  Fix unrecoverable failure when processing acquisitions at high speed
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated files to fix moved file includes.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.20   20 Nov 2008 12:11:22   tbair
 * Project:  SVObserver
 * Change Request (SCR) nbr:  641
 * SCR Title:  BoundsChecker results
 * Checked in by:  JimAdmin;  James A. Brown
 * Change Description:  
 *   Fixed bug in error checking in OnRun.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.19   14 Nov 2008 10:27:22   jbrown
 * Project:  SVObserver
 * Change Request (SCR) nbr:  641
 * SCR Title:  BoundsChecker results
 * Checked in by:  JimAdmin;  James A. Brown
 * Change Description:  
 *   SVOCVAnalyzerResultClass::GenerateFontModel ()
 *   No real impact noticed.. added Preprocessed query and
 *   improved error handling of Preprocess () command.
 * 
 * SVOCVAnalyzerResultClass::onRun ()
 *   Added initialization of l_length.
 *   Changed scope of l_Code.
 *   Added comments.
 *   Added/Changed error handling.
 * 
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.18   24 Jul 2007 15:17:36   tbair
 * Project:  SVObserver
 * Change Request (SCR) nbr:  597
 * SCR Title:  Upgrade Matrox Imaging Library to version 8.0
 * Checked in by:  tBair;  Tom Bair
 * Change Description:  
 *   Update MIL 8.0
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.17   22 Jun 2007 10:42:58   Joe
 * Project:  SVObserver
 * Change Request (SCR) nbr:  598
 * SCR Title:  Upgrade SVObserver to compile using vc++ in VS2005
 * Checked in by:  jSpila;  Joseph Spila
 * Change Description:  
 *   These changes include removal of all VC6 constraints, project files, and workspace files.  The new VC8 project and solution files will replace the VC6 files with the new tighter compilation rules.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.16   21 Jun 2007 14:34:02   Joe
 * Project:  SVObserver
 * Change Request (SCR) nbr:  598
 * SCR Title:  Upgrade SVObserver to compile using vc++ in VS2005
 * Checked in by:  jSpila;  Joseph Spila
 * Change Description:  
 *   These changes include modification based on fixing compiler-based and project-based differences between VC6 and VC8.  These changes mainly include casting issues, but some include type conversion and assignment of new compiler controlling defines.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.15   19 Feb 2007 16:40:08   Joe
 * Project:  SVObserver
 * Change Request (SCR) nbr:  594
 * SCR Title:  Remove issue with interpreting status response incorrectly
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated MIL error code methodology to use the new SVMatroxAppManager GetStatusCode operator.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.14   13 Oct 2005 14:02:42   Joe
 * Project:  SVObserver
 * Change Request (SCR) nbr:  500
 * SCR Title:  Reduce delay when adjusting tool parameters with a large toolset
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Removed index 1 from GetValue calls.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.13   29 Jul 2005 13:08:04   Joe
 * Project:  SVObserver
 * Change Request (SCR) nbr:  500
 * SCR Title:  Reduce delay when adjusting tool parameters with a large toolset
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated object initialization to change the parameters for RegisterEmbeddedObject for a SVValueObjectClass.  The two additional parameters will inform the preperation process before toolset execution.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.12   21 Jun 2005 08:21:20   ebeyeler
 * Project:  SVObserver
 * Change Request (SCR) nbr:  464
 * SCR Title:  Add array indexing for value objects
 * Checked in by:  eBeyeler;  Eric Beyeler
 * Change Description:  
 *   object attributes now use accessor methods
 * value object functions now use HRESULT
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.11   09 Mar 2005 07:06:20   Joe
 * Project:  SVObserver
 * Change Request (SCR) nbr:  456
 * SCR Title:  Update Image and Tool Objects to use the new Extent Classes
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Removed return code check within ResetObject method and moved validity check to OnValidate.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.10   18 Feb 2005 07:33:24   Joe
 * Project:  SVObserver
 * Change Request (SCR) nbr:  456
 * SCR Title:  Update Image and Tool Objects to use the new Extent Classes
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Update class with new methodologies based on new base object, extents and reset structure.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.9   08 Jul 2004 10:39:24   ryoho
 * Project:  SVObserver
 * Change Request (SCR) nbr:  431
 * SCR Title:  Fix problem with using match string files that are zero elements
 * Checked in by:  rYoho;  Rob Yoho
 * Change Description:  
 *   put in check to make sure that if using a match string file that the number of elements is above 0.  if not, the inspection will not be allowed to go online
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.8   28 May 2004 09:19:54   ebeyeler
 * Project:  SVObserver
 * Change Request (SCR) nbr:  425
 * SCR Title:  Fix crash when using certain Matrox font files.
 * Checked in by:  eBeyeler;  Eric Beyeler
 * Change Description:  
 *   - Handled floating point exception properly
 * - added error logging
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.7   14 May 2004 11:27:30   Joe
 * Project:  SVObserver
 * Change Request (SCR) nbr:  420
 * SCR Title:  Allow SVObserver to update the string length for the Matrox OCR
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Added new member variables to monitor string length and matchstring length.
 * Updated OnValidate, LoadMatchString, HideResults, and the constructor to initialize and use the new member variables.
 * Updated GenerateFontModel to use new member variable and to set string length to match string length.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.6   22 Mar 2004 13:29:34   rschock
 * Project:  SVObserver
 * Change Request (SCR) nbr:  412
 * SCR Title:  Fix Character Analyzer to show correct number of character results
 * Checked in by:  rSchock;  Rosco Schock
 * Change Description:  
 *   Moved hiding/unhiding of result objects to happen everytime the font file is reloaded.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.5   16 Feb 2004 13:07:14   rschock
 * Project:  SVObserver
 * Change Request (SCR) nbr:  384
 * SCR Title:  Add new Matrox OCR/OCV analyzer to SVObserver
 * Checked in by:  rSchock;  Rosco Schock
 * Change Description:  
 *   Made changes to redo the pass/fail/warn rules of the Matrox OCR/OCV analyzer.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.4   12 Feb 2004 16:41:46   rschock
 * Project:  SVObserver
 * Change Request (SCR) nbr:  384
 * SCR Title:  Add new Matrox OCR/OCV analyzer to SVObserver
 * Checked in by:  rSchock;  Rosco Schock
 * Change Description:  
 *   Fixed bug when the read/verify function fails to return a string that passes the font file's string match percentage.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.3   24 Sep 2003 11:07:44   Joe
 * Project:  SVObserver
 * Change Request (SCR) nbr:  384
 * SCR Title:  Add new Matrox OCR/OCV analyzer to SVObserver
 * Checked in by:  rSchock;  Rosco Schock
 * Change Description:  
 *   Updated souce to allow code to compile under MIL 6.1.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.2   22 Sep 2003 13:44:20   rschock
 * Project:  SVObserver
 * Change Request (SCR) nbr:  384
 * SCR Title:  Add new Matrox OCR/OCV analyzer to SVObserver
 * Checked in by:  rSchock;  Rosco Schock
 * Change Description:  
 *   Removed a couple of unused variables.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.1   18 Sep 2003 13:49:52   rschock
 * Project:  SVObserver
 * Change Request (SCR) nbr:  384
 * SCR Title:  Add new Matrox OCR/OCV analyzer to SVObserver
 * Checked in by:  rSchock;  Rosco Schock
 * Change Description:  
 *   Added execption handling to protect against crash in MIL pattern code.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.0   17 Sep 2003 13:07:50   rschock
 * Project:  SVObserver
 * Change Request (SCR) nbr:  384
 * SCR Title:  Add new Matrox OCR/OCV analyzer to SVObserver
 * Checked in by:  rSchock;  Rosco Schock
 * Change Description:  
 *   Created and integrated a new Matrox based OCR/OCV analyzer to SVObserver.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
*/
