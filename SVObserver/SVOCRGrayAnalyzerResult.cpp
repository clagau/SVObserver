//******************************************************************************
//* COPYRIGHT (c) 2003 by SVResearch, Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVOCRGrayAnalyzerResult
//* .File Name       : $Workfile:   SVOCRGrayAnalyzerResult.cpp  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.4  $
//* .Check In Date   : $Date:   15 May 2014 11:02:52  $
//******************************************************************************

#include "stdafx.h"
#include "SVOCRGrayAnalyzerResult.h"
#include "SVImageLibrary/SVImageBufferHandleImage.h"
#include "SVImageLibrary/SVImageBufferHandleInterface.h"
#include "SVRunControlLibrary/SVRunControlLibrary.h"
#include "SVGlobal.h"
#include "SVImageClass.h"
#include "SVTool.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

#define  CAR_RETURN        0x0D

SV_IMPLEMENT_CLASS( SVOCRGrayAnalyzeResultClass, SVOCRGrayAnalyzerResultClassGuid );

SVOCRGrayAnalyzeResultClass::SVOCRGrayAnalyzeResultClass( SVObjectClass* POwner, 
	                                                        int StringResourceID /* = IDS_CLASSNAME_SVWHITEPIXELANALYZERESULT */ )
												    :SVResultClass(FALSE, POwner, StringResourceID )   // TBD - 16 Jul 1999 - frb. FALSE???
{
	ocrParameters.psvOCRResult = this;

	clearAll();
}

// Should only be called from constructor!!!
void SVOCRGrayAnalyzeResultClass::clearAll()
{	
	// Identify yourself
	outObjectInfo.ObjectTypeInfo.ObjectType = SVResultObjectType;
	outObjectInfo.ObjectTypeInfo.SubType = SVResultOCRGrayObjectType;

	// Identify our input type needs
	inputImageObjectInfo.SetInputObjectType( SVImageObjectType );
	inputImageObjectInfo.SetObject( GetObjectInfo() );
	RegisterInputObject( &inputImageObjectInfo, _T( "OCRGrayAnalyzerResultImage" ) );

	// Register Embedded Objects
	RegisterEmbeddedObject( 
		&matchString, 
		SVMatchStringObjectGuid,
		IDS_OBJECTNAME_MATCH_STRING,
		false, SVResetItemNone );
	
	RegisterEmbeddedObject( 
		&foundString, 
		SVFoundStringObjectGuid,
		IDS_OBJECTNAME_FOUND_STRING,
		false, SVResetItemNone );
	
	RegisterEmbeddedObject( 
		&StrOcrParameters, 
		SVOCRGrayParametersObjectGuid,
		IDS_OBJECT_NAME_OCR_GRAY_PARAMETERS_STRING,
		false, SVResetItemNone );
	
	RegisterEmbeddedObject( 
		&valueBlobSortDirection,
		SVOCRBlobSortDirectionGuid,
		IDS_OBJECTNAME_OCR_BLOB_SORT_DIRECTION,
		false, SVResetItemNone );
	
	RegisterEmbeddedObject( 
		&valuePixelsForASpace,
		SVOCRPixelsForASpaceGuid,
		IDS_OBJECTNAME_OCR_PIXELS_FOR_A_SPACE,
		false, SVResetItemNone );
	
	RegisterEmbeddedObject( 
		&msvlMatchLineNumber,
		SVOCRGrayMatchLineNumberGuid,
		IDS_OBJECTNAME_OCR_MATCH_LINE_NUMBER,
		false, SVResetItemNone );
	
	// Exposing OCR Match Scores
	RegisterEmbeddedObject( 
		&msvdvocHighestMatchScore,
		SVOCRGrayHighestMatchScoreGuid,
		IDS_OBJECTNAME_OCR_HIGHEST_MATCH_SCORE,
		false, SVResetItemNone );
	
	RegisterEmbeddedObject( 
		&msvdvocLowestMatchScore,
		SVOCRGrayLowestMatchScoreGuid,
		IDS_OBJECTNAME_OCR_LOWEST_MATCH_SCORE,
		false, SVResetItemNone );
	
	RegisterEmbeddedObject( 
		&msvdvocAverageMatchScore,
		SVOCRGrayAverageMatchScoreGuid,
		IDS_OBJECTNAME_OCR_AVERAGE_MATCH_SCORE,
		false, SVResetItemNone );
	
	// OCR File Names
	RegisterEmbeddedObject( 
		&msvfnvocGrayFontFileName,
		SVOCRGrayFontFileNameGuid,
		IDS_OBJECTNAME_OCR_GRAY_FONT_FILE_NAME,
		false, SVResetItemOwner );
	
	RegisterEmbeddedObject( 
		&msvfnvocGrayMatchStringFileName,
		SVOCRGrayMatchStringFileNameGuid,
		IDS_OBJECTNAME_OCR_GRAY_MATCH_STRING_FILE_NAME,
		false, SVResetItemOwner );
	
	// Converting all OCR parameters from String to their own Value Objects
	// Will be used when Task Objects are redone
	/*
	RegisterEmbeddedObject( &m_voStrNoMatchLabel,
		                      SVOCRGrayNoMatchStringGuid,
		                      IDS_OBJECTNAME_OCR_GRAY_NO_MATCH_STRING );
	RegisterEmbeddedObject( &m_voStrMatchString,
		                      SVOCRGrayMatchStringGuid,
		                      IDS_OBJECTNAME_OCR_GRAY_MATCH_STRING );
	
	RegisterEmbeddedObject( &m_voUseMatchFile,
		                      SVOCRGrayUseMatchFileGuid,
		                      IDS_OBJECTNAME_OCR_GRAY_USE_MATCH_FILE );
	RegisterEmbeddedObject( &m_voMaxMatches,
		                      SVOCRGrayMaxMatchesGuid,
		                      IDS_OBJECTNAME_OCR_GRAY_MAX_MATCHES );
	RegisterEmbeddedObject( &m_voXVicinity,
		                      SVOCRGrayXVicinityGuid,
		                      IDS_OBJECTNAME_OCR_GRAY_X_VICINITY );
	RegisterEmbeddedObject( &m_voYVicinity,
		                      SVOCRGrayYVicinityGuid,
		                      IDS_OBJECTNAME_OCR_GRAY_Y_VICINITY );
	RegisterEmbeddedObject( &m_voScale,
		                      SVOCRGrayScaleGuid,
		                      IDS_OBJECTNAME_OCR_GRAY_SCALE );
	RegisterEmbeddedObject( &m_voMaxCandidates,
		                      SVOCRGrayMaxCandidatesGuid,
		                      IDS_OBJECTNAME_OCR_GRAY_MAX_CANDIDATES );
	RegisterEmbeddedObject( &m_voCandsInVicinity,
		                      SVOCRGrayCandsInVicinityGuid,
		                      IDS_OBJECTNAME_OCR_GRAY_CANDS_IN_VICINITY );
	RegisterEmbeddedObject( &m_voOutput,
		                      SVOCRGrayOutputGuid,
		                      IDS_OBJECTNAME_OCR_GRAY_OUTPUT );

	RegisterEmbeddedObject( &m_voRejectThresh,
		                      SVOCRGrayRejectThreshGuid,
		                      IDS_OBJECTNAME_OCR_GRAY_REJECT_THRESH );
	RegisterEmbeddedObject( &m_voAcceptThresh,
		                      SVOCRGrayMatchAcceptThreshGuid,
		                      IDS_OBJECTNAME_OCR_GRAY_ACCEPT_THRESH );
	RegisterEmbeddedObject( &m_voRelThresh,
		                      SVOCRGrayRelThreshGuid,
		                      IDS_OBJECTNAME_OCR_GRAY_REL_THRESH );
	RegisterEmbeddedObject( &m_voMinContrast,
		                      SVOCRGrayMinContrastGuid,
		                      IDS_OBJECTNAME_OCR_GRAY_MIN_CONTRAST );
	RegisterEmbeddedObject( &m_voCandidateThresh,
		                      SVOCRGrayCandidateThreshGuid,
		                      IDS_OBJECTNAME_OCR_GRAY_CANDIDATE_THRESH );
	RegisterEmbeddedObject( &m_voCandidateRelThresh,
		                      SVOCRGrayCandidateRelThreshGuid,
		                      IDS_OBJECTNAME_OCR_GRAY_CANDIDATE_REL_THRESH );
	//*/

	// Set Embedded defaults
	matchString.SetDefaultValue( _T( "" ), TRUE );
	foundString.SetDefaultValue( _T( "" ), TRUE );

	valueBlobSortDirection.SetDefaultValue( (long)SORT_BLOBS_RIGHT, TRUE );
	valuePixelsForASpace.SetDefaultValue( (long)-1, TRUE );
	msvlMatchLineNumber.SetDefaultValue ( (long) -1, TRUE );

	valueBlobSortDirection.ObjectAttributesAllowedRef() = SV_NO_ATTRIBUTES;
	valuePixelsForASpace.ObjectAttributesAllowedRef() = SV_NO_ATTRIBUTES;
	//
	// Set defaults in the OCR 'string' value..  This is NOT a 
	// publishable, viewable, etc. object.  It is saved and restored
	// using scripting.
	//
	CString csOCRDefaults = ocrParameters.ConvertParamsToString();
	StrOcrParameters.ObjectAttributesAllowedRef() = SV_EMBEDABLE;
	StrOcrParameters.SetDefaultValue( (LPTSTR)(LPCTSTR)csOCRDefaults, TRUE );

	// Setting exposed OCR Match Score defaults
	msvdvocHighestMatchScore.SetDefaultValue( (double) -1.0, TRUE );
	msvdvocLowestMatchScore.SetDefaultValue( (double) -1.0, TRUE );
	msvdvocAverageMatchScore.SetDefaultValue( (double) -1.0, TRUE );

	// Setting exposed OCR File Name defaults
	msvfnvocGrayFontFileName.SetDefaultValue( _T(""), TRUE );
	msvfnvocGrayMatchStringFileName.SetDefaultValue( _T(""), TRUE );

	// Add Default Inputs and Outputs
	addDefaultInputObjects();

  //
  // Initialize Wit objects
  //
	pOCRFontModelGray = NULL;

	presultStringVector = (CorStringVector *)svlvFastOcr.CreateLVObjectData("CorVector");
	plocationVector = (CorFpointVector *)svlvFastOcr.CreateLVObjectData("CorVector");
	plvcfvMatchScoreVector = (CorFloatVector *)svlvFastOcr.CreateLVObjectData("CorVector");
	pROIObj = (CorObj*) svlvFastOcr.CreateLVObject(COR_OBJ_GENERIC);

	m_pBuffer = NULL;
	m_pIndexTable = NULL;
}

BOOL SVOCRGrayAnalyzeResultClass::isWitCompleteLoaded()
{
	return svlvFastOcr.IsLVFastOCRLoaded();
}



////////////////////////////////////////////////////////////////////////////////
// .Title       : // e.g. Standard destructor of class ...
// -----------------------------------------------------------------------------
// .Description : This destructor deletes ...
//              :
////////////////////////////////////////////////////////////////////////////////
// .History
//	 Date		Author		Comment                                       
//  :27.05.1997 RO			First Implementation
//	:
////////////////////////////////////////////////////////////////////////////////
SVOCRGrayAnalyzeResultClass::~SVOCRGrayAnalyzeResultClass()
{
	if ( presultStringVector )
	{
		svlvFastOcr.DestroyLVObjectData((void **)&presultStringVector, "CorVector", NULL);
	}

	if ( plocationVector )
	{
		svlvFastOcr.DestroyLVObjectData((void **)&plocationVector, "CorVector", NULL);
	}

	if ( plvcfvMatchScoreVector )
	{
		svlvFastOcr.DestroyLVObjectData((void **)&plvcfvMatchScoreVector, "CorVector", NULL);
	}

	if ( pROIObj )
	{
		svlvFastOcr.DestroyLVObject(&pROIObj);
	}

	if( pOCRFontModelGray ) 
	{
		svlvFastOcr.DestroyLVObjectData((void **)&pOCRFontModelGray, "CorFontModelGray", NULL);
	}

	if( m_pBuffer )
	{
		delete [] m_pBuffer;
	}
}

BOOL SVOCRGrayAnalyzeResultClass::GetFontFileName( CString & csName )
{
	BOOL bOk = FALSE;

	bOk = ( msvfnvocGrayFontFileName.GetValue( csName ) == S_OK );

	return bOk;
}

BOOL SVOCRGrayAnalyzeResultClass::GetMatchStringFileName( CString & csName )
{
	BOOL bOk = FALSE;

	bOk = ( msvfnvocGrayMatchStringFileName.GetValue( csName ) == S_OK );

	return bOk;
}
SVImageClass* SVOCRGrayAnalyzeResultClass::getInputImage()
{
	if( inputImageObjectInfo.IsConnected() && 
		inputImageObjectInfo.GetInputObjectInfo().PObject )
	{
		return ( SVImageClass* ) inputImageObjectInfo.GetInputObjectInfo().PObject;
	}
	return NULL;
}



/////////////////////////////////////////////////////////////////////////////
//
//
//
//
BOOL SVOCRGrayAnalyzeResultClass::CreateObject(	SVObjectLevelCreateStruct* PCreateStructure )
{
	BOOL bOk = SVResultClass::CreateObject( PCreateStructure );

	if ( bOk )
	{
		bOk = getInputImage() != NULL;
	}

	if ( bOk )
	{
    bOk = isWitCompleteLoaded();

		if ( ! bOk )
		{
		   AfxMessageBox("ERROR: Failed to Load OCR Gray Library");   // 24 Jun 1999 - frb.
		}
	}

	if ( bOk )
	{
		//
		// The OCR parameters should be available now from the script..
		//
		CString csTemp;

		StrOcrParameters.GetValue( csTemp );
		ocrParameters.ConvertStringToParams( (LPCTSTR)csTemp );

		// Load Match String, if necessary...
		LoadMatchString();

		// Generate Font Model, if necessary...
    GenerateFontModel();
  }

	// Set / Reset Printable Flag
	matchString.ObjectAttributesAllowedRef() |= SV_PRINTABLE;
	foundString.ObjectAttributesAllowedRef() &= ~SV_PRINTABLE;
	StrOcrParameters.ObjectAttributesAllowedRef() |= SV_PRINTABLE;
	valueBlobSortDirection.ObjectAttributesAllowedRef() |= SV_PRINTABLE;
	valuePixelsForASpace.ObjectAttributesAllowedRef() |= SV_PRINTABLE;
	msvlMatchLineNumber.ObjectAttributesAllowedRef() &= ~SV_PRINTABLE;

	msvdvocHighestMatchScore.ObjectAttributesAllowedRef() &= ~SV_PRINTABLE;
	msvdvocLowestMatchScore.ObjectAttributesAllowedRef() &= ~SV_PRINTABLE;
	msvdvocAverageMatchScore.ObjectAttributesAllowedRef() &= ~SV_PRINTABLE;

	msvfnvocGrayFontFileName.ObjectAttributesAllowedRef() |= SV_PRINTABLE;
	msvfnvocGrayMatchStringFileName.ObjectAttributesAllowedRef() |= SV_PRINTABLE;

	m_voStrNoMatchLabel.ObjectAttributesAllowedRef() &= ~SV_PRINTABLE;
	m_voStrMatchString.ObjectAttributesAllowedRef() &= ~SV_PRINTABLE;
	
	m_voUseMatchFile.ObjectAttributesAllowedRef() &= ~SV_PRINTABLE;
	m_voMaxMatches.ObjectAttributesAllowedRef() &= ~SV_PRINTABLE;
	m_voXVicinity.ObjectAttributesAllowedRef() &= ~SV_PRINTABLE;
	m_voYVicinity.ObjectAttributesAllowedRef() &= ~SV_PRINTABLE;
	m_voScale.ObjectAttributesAllowedRef() &= ~SV_PRINTABLE;
	m_voMaxCandidates.ObjectAttributesAllowedRef() &= ~SV_PRINTABLE;
	m_voCandsInVicinity.ObjectAttributesAllowedRef() &= ~SV_PRINTABLE;
	m_voOutput.ObjectAttributesAllowedRef() &= ~SV_PRINTABLE;

	m_voRejectThresh.ObjectAttributesAllowedRef() &= ~SV_PRINTABLE;
	m_voAcceptThresh.ObjectAttributesAllowedRef() &= ~SV_PRINTABLE;
	m_voRelThresh.ObjectAttributesAllowedRef() &= ~SV_PRINTABLE;
	m_voMinContrast.ObjectAttributesAllowedRef() &= ~SV_PRINTABLE;
	m_voCandidateThresh.ObjectAttributesAllowedRef() &= ~SV_PRINTABLE;
	m_voCandidateRelThresh.ObjectAttributesAllowedRef() &= ~SV_PRINTABLE;

	isCreated = bOk;

	return bOk;
}

/////////////////////////////////////////////////////////////////////////////
//
//
//
//
BOOL SVOCRGrayAnalyzeResultClass::CloseObject()
{
	BOOL bOk = FALSE;

	if( SVResultClass::CloseObject() )
	{
		bOk = TRUE;

	  if( pOCRFontModelGray ) 
		{
			bOk = svlvFastOcr.DestroyLVObjectData( (void **)(&pOCRFontModelGray), "CorFontModelGray", NULL ); 
		}
	}

	return bOk;
}

//******************************************************************************
// Operation(s) Of Reading Access:
//******************************************************************************

void SVOCRGrayAnalyzeResultClass::UpdateOCRScriptString()
{
	CString csTemp = ocrParameters.ConvertParamsToString();
	StrOcrParameters.SetValue( 1, (LPTSTR)(LPCTSTR)csTemp);
}

SVOCRGrayParamStruct& SVOCRGrayAnalyzeResultClass::GetOCRParameters()
{
	return ocrParameters;
}

void SVOCRGrayAnalyzeResultClass::GetOCRResultString( CString & csResult )
{
   //return (LPCTSTR)&resultString[0];
   BOOL bResult = ( foundString.GetValue( csResult ) == S_OK );
   ASSERT(bResult);
}

/////////////////////////////////////////////////////////////////////////////
//
//
//
//
CRect SVOCRGrayAnalyzeResultClass::Draw( HDC DC, CRect R )
{
	CString text;
	
	::SetTextColor( DC, SV_DEFAULT_TEXT_COLOR ); 

	text = _T("OCR Analyzer Owner TBD");         // 16 Jul 1999 - frb.

	::TextOut( DC, R.left, R.top, text, text.GetLength() );
	R.top += SV_DEFAULT_TEXT_HEIGHT;

	text = _T( "Match String:" );
	::TextOut( DC, R.left, R.top, text, text.GetLength() );
	CString csResult;
	matchString.GetValue( csResult );
	text.Format( _T( "%s" ), (LPCTSTR)csResult );
	::TextOut( DC, R.left + 100, R.top, text, text.GetLength() );
	R.top += SV_DEFAULT_TEXT_HEIGHT + 2;

    DWORD dwColor = GetObjectColor();
	//resultColor.GetValue(dwColor);

	//::SetTextColor( DC, resultColor ); 
	::SetTextColor(DC,dwColor);

	text = _T( "OCR String:" );
	::TextOut( DC, R.left, R.top, text, text.GetLength() );
	foundString.GetValue( csResult );
	text.Format( _T( "%s" ), (LPCTSTR)csResult );
	::TextOut( DC, R.left + 100, R.top, text, text.GetLength() );
	R.top += SV_DEFAULT_TEXT_HEIGHT + 2;

	return R;
}


////////////////////////////////////////////////////////////////////////////////
// .Title       : GenerateFontModel ()
// -----------------------------------------------------------------------------
// .Description : Generates the Wit font model from the Font file and the
//				  entered sampling rate.
//              :
////////////////////////////////////////////////////////////////////////////////
// .History
//	 Date		Author		Comment
//  :19.03.1999 JB			First Implementation
//	:
////////////////////////////////////////////////////////////////////////////////
BOOL SVOCRGrayAnalyzeResultClass::GenerateFontModel()
{
	BOOL bOk = isWitCompleteLoaded();

	if ( bOk )
	{
		if (pOCRFontModelGray)
		{
			bOk = svlvFastOcr.DestroyLVObjectData( (void **)(&pOCRFontModelGray), "CorFontModelGray", NULL );
		}
	}

	if ( bOk )
	{
		CString csTemp;
		CFileStatus rStatus;

		bOk = GetFontFileName( csTemp );
		if ( bOk )
		{
			bOk = CFile::GetStatus( csTemp, rStatus );
			if ( bOk )
			{
				bOk = 0L < rStatus.m_size;
			}
		}
	}

	if ( bOk )
	{
		//--- File must exist and contain data in order to get here.
		CorObj *pOcrFontObject = svlvFastOcr.CreateLVObject(COR_OBJ_GENERIC);
		bOk = pOcrFontObject != NULL;
		if ( bOk )
		{
			CString csTemp;

			bOk = GetFontFileName( csTemp );
			if ( bOk )
			{
				//
				// Read the font parameters from a file into the OcrFont object.
				//
				bOk = svlvFastOcr.ReadLVObjectFromFile( pOcrFontObject, csTemp );
				if ( bOk )
				{
					bOk = svlvFastOcr.InitializeLVOCRGray( (CorOcrFont *)CorObj_OcrFont(pOcrFontObject),
																					       &pOCRFontModelGray );
				}
			}

			//
			// Free memory allocated by rdObj for fontParamObj
			//
			bOk = svlvFastOcr.DestroyLVObject(&pOcrFontObject) && bOk;
		}
	}

	if ( ! bOk && pOCRFontModelGray != NULL )
	{
		svlvFastOcr.DestroyLVObjectData( (void **)(&pOCRFontModelGray), "CorFontModelGray", NULL );
	}

	return bOk;
}

/////////////////////////////////////////////////////////////////////////////
//
//
//
//
void  SVOCRGrayAnalyzeResultClass::WriteToArchive(CFile & fileArchive)
{
    //
    // Write the match string to the archive.
    //
	CString s;
	CString csResult;
	foundString.GetValue( csResult );
	s.Format("OCR/OCV result: %s\r\n", (LPCTSTR)csResult );

    TRY
	{
	    fileArchive.Write((LPCTSTR)s,s.GetLength());
	}
	CATCH_ALL(e)
	{
	} 
	END_CATCH_ALL;
}


/////////////////////////////////////////////////////////////////////////////
//
//
// Load OCR match string from OCR parameters or from a file.f
//
//
HRESULT SVOCRGrayAnalyzeResultClass::LoadMatchString()
{
	//
	// Check to see if match string is to read from a file.
	//
	HRESULT hrRet = S_OK;
	CString strMatchString;
	
	msvError.ClearLastErrorCd ();
	msvError.msvlErrorCd = 0;
	
	while (1)
	{
		m_nTotalCount = 0;
		
		if( ocrParameters.useMatchFile != 1 )
		{
			//
			// File is not being used. Use local declared match string.
			// ocrParameters.matchStrings[] are TCHAR* !!!!
			//
			strMatchString = ocrParameters.STRmatchString;
			matchString.SetValue( 1, (LPTSTR)(LPCTSTR)strMatchString );
			break;
		}
		
		//
		// Check to see if the file exists..
		//
		CString strMatchStringFileName;
		GetMatchStringFileName( strMatchStringFileName );
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
		// Open and read the OCR match string from the file.
		//
		CFile matchFile;
		if(!matchFile.Open(strMatchStringFileName, 
			CFile::modeRead | CFile::shareDenyNone))
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
			msvError.msvlErrorCd = -25051;
			SV_TRAP_ERROR_BRK_TSTFIRST (msvError, 25051);
		}
		// Copy entire file into the buffer.
		m_lTotalBytes = matchFile.Read( m_pBuffer, (UINT)dwByteNumber );
		matchFile.Close();
		m_pBuffer[m_lTotalBytes] = 0;
		
		if (msvError.GetLastErrorCd () & SV_ERROR_CONDITION)
		{
			strMatchString.Format ("");
			matchString.SetValue( 1, (LPTSTR)(LPCTSTR)strMatchString );
		}
		
		if ( dwByteNumber > 0 )
		{
			if(!BuildHashTable(m_pBuffer))
			{
				m_nTotalCount = 0;
				msvError.msvlErrorCd = -25052;
				SV_TRAP_ERROR_BRK_TSTFIRST (msvError, 25052);
			}
		}
		else
		{
			hrRet = S_FALSE;
			SVToolClass *l_pTool = (SVToolClass*) GetAncestor(SVToolObjectType);
			if ( l_pTool )
			{
				l_pTool->SetInvalid();
			}
		}
		break;
	} // while (1)
	
	
	if (msvError.GetLastErrorCd () & SV_ERROR_CONDITION)
	{
		strMatchString.Format ("");
		matchString.SetValue( 1, (LPTSTR)(LPCTSTR)strMatchString );
	}
	return hrRet;
}


/////////////////////////////////////////////////////////////////////////////
//
//
//
//
BOOL SVOCRGrayAnalyzeResultClass::OnValidate()
{
	BOOL bRetVal = FALSE;
	if( inputImageObjectInfo.IsConnected() &&
		inputImageObjectInfo.GetInputObjectInfo().PObject )
	{
		if ( ocrParameters.useMatchFile == 1)
		{
			bRetVal = m_lTotalBytes > 0;
		}
		else
		{
			bRetVal = TRUE;
		}

		bRetVal = pOCRFontModelGray != NULL && bRetVal;

		bRetVal = SVResultClass::OnValidate() && bRetVal;
	}

	// Note: Make sure this is called when Validate fails !!!
	if( ! bRetVal )
		SetInvalid();

	return bRetVal;
}

/////////////////////////////////////////////////////////////////////////////
//
//
//
DWORD_PTR SVOCRGrayAnalyzeResultClass::processMessage( DWORD DwMessageID, DWORD_PTR DwMessageValue, DWORD_PTR DwMessageContext )
{
	DWORD_PTR DwResult = NULL;

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

HRESULT SVOCRGrayAnalyzeResultClass::ResetObject()
{
	HRESULT l_hrOk = SVResultClass::ResetObject();

	//
	// Read the match string from a possible match string file.
  //
	LoadMatchString();

	//
	// Regenerate the Font Model.
  //
	GenerateFontModel();

	return l_hrOk;
}

/////////////////////////////////////////////////////////////////////////////
//
//
//
//
BOOL SVOCRGrayAnalyzeResultClass::onRun( SVRunStatusClass& RRunStatus )
{
	BOOL bOk = FALSE;
    
	bOk = SVResultClass::onRun( RRunStatus );

	if ( bOk && !RRunStatus.IsDisabled() && !RRunStatus.IsDisabledByCondition() )
	{
		long imagePitchMil = 0;
		long imageSizeYMil = 0;

		BYTE* pMilBuffer = NULL;

		if ( bOk )
		{
			bOk = pOCRFontModelGray != NULL;

			if ( ! bOk )
			{
				SetInvalid();
			}
		}

		if ( bOk )
		{
			SVImageClass* pImage = getInputImage();

			bOk = pImage != NULL;
			
			if ( ! bOk )
			{    
				SetInvalid();
			}
			else
			{
				SVSmartHandlePointer ImageHandle;

				if ( pImage->GetImageHandle( ImageHandle ) && !( ImageHandle.empty() ) )
				{
					SVImageBufferHandleImage l_MilHandle;
					ImageHandle->GetData( l_MilHandle );

					
					SVMatroxBufferInterface::SVStatusCode l_Code;
					
					SVMatroxBuffer milBuffer = l_MilHandle.GetBuffer();

					//
					// Get the Mil buffer host pointer ( address in the process address space )
					//
					SVMatroxBufferInterface::GetHostAddress( &pMilBuffer, milBuffer );

					bOk = pMilBuffer != NULL;

					if ( ! bOk )
					{
						//
						// No MIL image buffer handle..
						//
						AfxMessageBox( _T("ERROR: No Mil Host Buffer Pointer") );
					}
					else
					{
						long imageTypeMil = -1;
						l_Code = SVMatroxBufferInterface::Get( milBuffer, SVType, imageTypeMil );
						//long imageTypeMil = MbufInquire( milBuffer, M_TYPE, M_NULL );

						if ( imageTypeMil != 8 )
						{
							CString s;
							s.Format("ERROR: MIL Image Type Not 8 Bit Unsigned: %x",imageTypeMil);
							AfxMessageBox(s);
						}

						//
						// Check for pixel pitch and Y height.
						//
						l_Code = SVMatroxBufferInterface::Get( milBuffer, SVPitch, imagePitchMil );
						l_Code = SVMatroxBufferInterface::Get( milBuffer, SVSizeY, imageSizeYMil );

						bOk = imagePitchMil > 1 && imageSizeYMil > 1;

						if ( ! bOk )
						{
							// Mil image PITCH and Width error
							AfxMessageBox( _T("ERROR: Mil image Pitch and Width Error") );
						}
					}
				}
			}
		}

		if ( bOk )
		{
			//
			// Turn off mouse events for duration of this method.
			//
			bOk = lock.Lock( 1 );

			if ( ! bOk )
			{
				SetInvalid();
			}
			else
			{
				long lTemp;
				CString strTemp;

				// Reset the 'found' string to nothing.
				foundString.SetValue( RRunStatus.m_lResultDataIndex, _T("") );      // 04 Dec 1999 - frb.
				matchString.GetValue( strTemp );
				matchString.SetValue( RRunStatus.m_lResultDataIndex, strTemp );

				//
				// Preset for failure
				//
				failed.SetValue( RRunStatus.m_lResultDataIndex, TRUE );
				warned.SetValue( RRunStatus.m_lResultDataIndex, TRUE );
				passed.SetValue( RRunStatus.m_lResultDataIndex, FALSE );

				//
				// Get some settings.
				//

				valueBlobSortDirection.GetValue(lTemp);
				arrayOCRBlobRecords.enumSortDirection = (enumBlobSortDirection)lTemp;

				valuePixelsForASpace.GetValue(lTemp);
				arrayOCRBlobRecords.lPixelsForASpace = lTemp;

				// abb: the following only lets FastOCR go if the input image is 8bit unsigned. The FastOCR
				//      package actually handles other image types as well, so this can be generalized
				//      to handle those other image types.

				CorImage *plvImage = (CorImage*) svlvFastOcr.CreateLVObjectData("CorImage");
				bOk = plvImage != NULL;

				if ( ! bOk )
				{
					SetInvalid();
				}
				else
				{
					//  Wit wrapper for sourceImage.
					CorObj_mdType(plvImage)    = COR_OBJ_UBYTE;
					CorObj_width(plvImage)     = imagePitchMil; //imageSizeXMil;
					CorObj_height(plvImage)    = imageSizeYMil;
					CorObj_mdData(plvImage)    = (void *) pMilBuffer;

					//CorObj *pROIObj = (CorObj*) svlvFastOcr.CreateLVObject(COR_OBJ_GENERIC);
					//bOk = pROIObj != NULL;

					if ( ! bOk )
					{
						SetInvalid();
					}
					else
					{
						bOk = svlvFastOcr.RunLVOCRGray( plvImage, pROIObj, pOCRFontModelGray,
														(CorVector **)&presultStringVector,
														(CorVector **)&plvcfvMatchScoreVector,
														(CorVector **)&plocationVector,
														&ocrParameters );

						//svlvFastOcr.DestroyLVObject(&pROIObj);
					}

					CorObj_mdType(plvImage)    = COR_OBJ_GENERIC;
					CorObj_width(plvImage)     = 0;
					CorObj_height(plvImage)    = 0;
					CorObj_mdData(plvImage)    = NULL;

					svlvFastOcr.DestroyLVObjectData((void **)&plvImage, "CorImage", NULL);
				}

				//
				// Process the OCR chars returned from pfn_ocrExecute();
				//
				if ( bOk ) 
				{
					float fHigh = -1.0F;
					float fLow  = 1.1F;
					float fAvg  = 0.0F;

					CString resultStringFromOCR;

					msvlMatchLineNumber.SetValue( RRunStatus.m_lResultDataIndex, (const long) -1L );

					arrayOCRBlobRecords.ClearArray();

					bOk = arrayOCRBlobRecords.FillArray( presultStringVector,
					                                     plocationVector,					                                     
														 plvcfvMatchScoreVector);

					if ( presultStringVector && CorObj_mdData( presultStringVector ) )
					{
						svlvFastOcr.ReleaseLVVector( (CorVector *)presultStringVector );
					}
					if ( plocationVector && CorObj_mdData( plocationVector ) )
					{
						svlvFastOcr.ReleaseLVVector( (CorVector *)plocationVector );
					}
					if ( plvcfvMatchScoreVector && CorObj_mdData( plvcfvMatchScoreVector ) )
					{
						svlvFastOcr.ReleaseLVVector( (CorVector *)plvcfvMatchScoreVector );
					}

					if ( ! bOk )
					{
						//
						// Wit convert from wit object to char string failed.
						//
						AfxMessageBox( _T("OCR data convert failed") );
					}
					else
					{
						bOk = arrayOCRBlobRecords.AnalyzeBlobs( resultStringFromOCR,
						                                        &fHigh, &fLow, &fAvg );
						if ( ! bOk )
						{
							//
							// Data re-organizing failed.
							//
							AfxMessageBox( _T("OCR data formating failed") );
						}
					}

					if ( bOk )
					{
						msvdvocHighestMatchScore.SetValue( RRunStatus.m_lResultDataIndex, (double) fHigh );
						msvdvocLowestMatchScore.SetValue(  RRunStatus.m_lResultDataIndex, (double) fLow );
						msvdvocAverageMatchScore.SetValue( RRunStatus.m_lResultDataIndex, (double) fAvg );

						//
						// Copy OCR result to a storage array element.
						//
						foundString.SetValue( RRunStatus.m_lResultDataIndex, (LPCTSTR)resultStringFromOCR );

						//
						// Is result == matchstring?
						//
						CString csMatch;
						matchString.GetValue( csMatch );

						int nResultValue = 0;
						if ( m_nTotalCount > 1 )
						{
							int nStringMatch;
							if ( ( nStringMatch = CheckStringInTable( resultStringFromOCR ) ) == -1 )
							{
								nResultValue = 3; // Failed
							}
							else
							{
								nResultValue = 1; // passed
							}
							msvlMatchLineNumber.SetValue(RRunStatus.m_lResultDataIndex, (long)nStringMatch);
						}
						else // Handle the old way
						{
							if( csMatch == resultStringFromOCR )
							{
								nResultValue = 1; // passed
							
								if ( m_nTotalCount == 1 )
								{
									msvlMatchLineNumber.SetValue(RRunStatus.m_lResultDataIndex, (const long) 1L);
								}
							}
							else
							{
								// If the result has any number of chars other than length of the
								// match string  OR
								// The result has any 'no match' characters then we have a
								// 'warned' condition.
								//
								if( ( resultStringFromOCR.GetLength() != csMatch.GetLength() ) ||
										( resultStringFromOCR.Find( (LPCTSTR)ocrParameters.STRnoMatchLabel ) > -1 ) )
								{
									nResultValue = 2; // Warned
								}
								else
								{
									nResultValue = 3; // Failed
								}
							}
						}

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
						}
					}
				}
			}

			lock.Unlock();
		}
	}

	if ( ! bOk )
	{
		RRunStatus.SetInvalid();
	}

	return bOk;
}

BOOL SVOCRGrayAnalyzeResultClass::BuildHashTable ( char *pBuffer )
{
   long lBufIndex = 0;
   BOOL  bRet = TRUE;

   for(int i = 0; i < GRAY_MAX_ENTREES; i++)
      m_pDataArr[i] = 0;

   for(m_nTotalCount = 0; 
         (m_nTotalCount < GRAY_MAX_ENTREES) && (lBufIndex < m_lTotalBytes);
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
	      matchString.SetValue( 1, (LPTSTR)pBuffer );
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

      for(int Count = 0; Count < m_nTotalCount ; Count++)
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
void SVOCRGrayAnalyzeResultClass::InsertValueToTable ( short nValue, int nIndex )
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
int SVOCRGrayAnalyzeResultClass::CheckStringInTable(CString MatchString)
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

//EOF//

//******************************************************************************
//* LOG HISTORY:
//******************************************************************************
/*
$Log:   N:\PVCSarch65\ProjectFiles\archives\SVObserver_SRC\SVObserver\SVOCRGrayAnalyzerResult.cpp_v  $
 * 
 *    Rev 1.4   15 May 2014 11:02:52   tbair
 * Project:  SVObserver
 * Change Request (SCR) nbr:  852
 * SCR Title:  Add Multiple Platform Support to SVObserver's Visual Studio Solution
 * Checked in by:  tBair;  Tom Bair
 * Change Description:  
 *   Modified processMessage signature to use DWORD_PTR
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.3   01 Feb 2014 11:39:22   tbair
 * Project:  SVObserver
 * Change Request (SCR) nbr:  852
 * SCR Title:  Add Multiple Platform Support to SVObserver's Visual Studio Solution
 * Checked in by:  tBair;  Tom Bair
 * Change Description:  
 *   Changed SVSendmessage and processmessage to use LONG_PTR instead of DWORD.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.2   02 Oct 2013 06:48:20   tbair
 * Project:  SVObserver
 * Change Request (SCR) nbr:  852
 * SCR Title:  Add Multiple Platform Support to SVObserver's Visual Studio Solution
 * Checked in by:  tBair;  Tom Bair
 * Change Description:  
 *   Add x64 platform.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.1   13 May 2013 12:36:52   bWalter
 * Project:  SVObserver
 * Change Request (SCR) nbr:  814
 * SCR Title:  Upgrade SVObserver to Compile Using Visual Studio 2010
 * Checked in by:  bWalter;  Ben Walter
 * Change Description:  
 *   Merged with svo_src label SVO 6.10 Beta 014.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.36   02 May 2013 11:21:46   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  810
 * SCR Title:  Remove support for Legacy Tools
 * Checked in by:  rYoho;  Rob Yoho
 * Change Description:  
 *   Updated process message method to fix formatting issues and updated the reset object section to only assert when the HRESULT is a failed status.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.0   23 Apr 2013 12:55:42   bWalter
 * Project:  SVObserver
 * Change Request (SCR) nbr:  814
 * SCR Title:  Upgrade SVObserver to Compile Using Visual Studio 2010
 * Checked in by:  bWalter;  Ben Walter
 * Change Description:  
 *   Initial check in to SVObserver_src.  (Merged with svo_src label SVO 6.10 Beta 008.)
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.35   12 Jul 2012 15:03:18   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  769
 * SCR Title:  Fix Problems and Crashes with Inspection Document Display Updates
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated source code to add in an additional string identifier for the input and fixed clear functionality assoicated with input structure.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.34   19 Jun 2012 13:11:00   jspila
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
 *    Rev 1.33   10 Feb 2011 15:05:22   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  712
 * SCR Title:  Fix issues with black images when using command interface (SIAC)
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated source code to now use shared pointer instead of untracked structure.  This fixes destruction, tracking and rebuilding issues.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.32   27 Jan 2011 11:44:46   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  712
 * SCR Title:  Fix issues with black images when using command interface (SIAC)
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated source code to fix issues with Matrox Buffer Management.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.31   22 Jun 2010 10:00:02   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  693
 * SCR Title:  Fix Performance Issue with Inspection Process
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Fixed issues with registered inputs.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.30   01 Jun 2010 14:57:56   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  693
 * SCR Title:  Fix Performance Issue with Inspection Process
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated source code to improve ability to track performance.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.29   16 Dec 2009 12:16:22   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  677
 * SCR Title:  Fix problem in camera notify thread
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Fix issues with includes and comments.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.28   03 Sep 2009 10:35:10   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  665
 * SCR Title:  Fix unrecoverable failure when processing acquisitions at high speed
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated files to fix moved file includes.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.27   24 Jul 2007 15:17:34   tbair
 * Project:  SVObserver
 * Change Request (SCR) nbr:  597
 * SCR Title:  Upgrade Matrox Imaging Library to version 8.0
 * Checked in by:  tBair;  Tom Bair
 * Change Description:  
 *   Update MIL 8.0
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.26   22 Jun 2007 10:42:56   Joe
 * Project:  SVObserver
 * Change Request (SCR) nbr:  598
 * SCR Title:  Upgrade SVObserver to compile using vc++ in VS2005
 * Checked in by:  jSpila;  Joseph Spila
 * Change Description:  
 *   These changes include removal of all VC6 constraints, project files, and workspace files.  The new VC8 project and solution files will replace the VC6 files with the new tighter compilation rules.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.25   21 Jun 2007 14:34:00   Joe
 * Project:  SVObserver
 * Change Request (SCR) nbr:  598
 * SCR Title:  Upgrade SVObserver to compile using vc++ in VS2005
 * Checked in by:  jSpila;  Joseph Spila
 * Change Description:  
 *   These changes include modification based on fixing compiler-based and project-based differences between VC6 and VC8.  These changes mainly include casting issues, but some include type conversion and assignment of new compiler controlling defines.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.24   10 Aug 2005 12:40:34   Joe
 * Project:  SVObserver
 * Change Request (SCR) nbr:  500
 * SCR Title:  Reduce delay when adjusting tool parameters with a large toolset
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated methods to improve performance and reduce system delay.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.23   09 Aug 2005 07:35:42   Joe
 * Project:  SVObserver
 * Change Request (SCR) nbr:  500
 * SCR Title:  Reduce delay when adjusting tool parameters with a large toolset
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated objects and dialogs to use the ActiveX Interface to update inspection data.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.22   29 Jul 2005 13:08:04   Joe
 * Project:  SVObserver
 * Change Request (SCR) nbr:  500
 * SCR Title:  Reduce delay when adjusting tool parameters with a large toolset
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated object initialization to change the parameters for RegisterEmbeddedObject for a SVValueObjectClass.  The two additional parameters will inform the preperation process before toolset execution.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.21   21 Jun 2005 08:21:20   ebeyeler
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
 *    Rev 1.20   09 Mar 2005 07:06:20   Joe
 * Project:  SVObserver
 * Change Request (SCR) nbr:  456
 * SCR Title:  Update Image and Tool Objects to use the new Extent Classes
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Removed return code check within ResetObject method and moved validity check to OnValidate.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.19   17 Feb 2005 15:01:20   Joe
 * Project:  SVObserver
 * Change Request (SCR) nbr:  456
 * SCR Title:  Update Image and Tool Objects to use the new Extent Classes
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Update class with new methodologies based on new base object, extents and reset structure.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.18   08 Jul 2004 10:41:14   ryoho
 * Project:  SVObserver
 * Change Request (SCR) nbr:  431
 * SCR Title:  Fix problem with using match string files that are zero elements
 * Checked in by:  rYoho;  Rob Yoho
 * Change Description:  
 *   put in check to make sure that if using a match string file that the number of elements is above 0.  if not, the inspection will not be allowed to go online
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.17   26 Nov 2003 10:57:10   rschock
 * Project:  SVObserver
 * Change Request (SCR) nbr:  364
 * SCR Title:  OCR Analyzers show wrong result for "line number of match string" in results.
 * Checked in by:  rSchock;  Rosco Schock
 * Change Description:  
 *   Forced match string to be copied forward in the onRun.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.16   21 May 2003 17:51:16   rschock
 * Project:  SVObserver
 * Change Request (SCR) nbr:  364
 * SCR Title:  OCR Analyzers show wrong result for "line number of match string" in results.
 * Checked in by:  rSchock;  Rosco Schock
 * Change Description:  
 *   Fixed the Run functions to set the "Line number of match string" value object in the current result data index.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.15   30 Apr 2003 17:02:28   rschock
 * Project:  SVObserver
 * Change Request (SCR) nbr:  346
 * SCR Title:  Update SVObserver to Version 4.21 Release
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Fixed a couple of small memory leaks in the ::onRun.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.14   29 Apr 2003 10:51:40   rschock
 * Project:  SVObserver
 * Change Request (SCR) nbr:  346
 * SCR Title:  Update SVObserver to Version 4.21 Release
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Redid the #include defines and standardized the Tracker log headers and removed warning from release mode builds.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.13   22 Apr 2003 12:42:26   rschock
 * Project:  SVObserver
 * Change Request (SCR) nbr:  346
 * SCR Title:  Update SVObserver to Version 4.21 Release
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Redid the #include defines and standardized the Tracker log headers and removed warning from release mode builds.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.12   30 Jan 2003 15:20:16   joe
 * Project:  SVObserver
 * Change Request (SCR) nbr:  301
 * SCR Title:  Implement Result Image Circle Buffer
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated onRun and CreateObject methods to use the new image circle buffer methodology.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.11   09 Dec 2002 19:46:32   Joe
 * Project:  SVObserver
 * Change Request (SCR) nbr:  295
 * SCR Title:  Remove Result Data from Configuration Printout
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   CreateObject method was modified to properly update the isCreated flag and to set the printability of the internal value objects.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.10   20 Nov 2002 10:10:38   ryoho
 * Project:  SVObserver
 * Change Request (SCR) nbr:  272
 * SCR Title:  Outputs do not always get set correctly.
 * Checked in by:  rYoho;  Rob Yoho
 * Change Description:  
 *   Changed ValueObject semantics to work with buckets
 * Added value objects
 * 
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.9   04 Oct 2001 16:47:42   Joe
 * Project:  SVObserver
 * Change Request (SCR) nbr:  229
 * SCR Title:  Upgrade SVObserver to Version 3.34 Release
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Modified source code to remove compiler warning message.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.8   07 Aug 2001 16:39:42   Joe
 * Project:  SVObserver
 * Change Request (SCR) nbr:  209
 * SCR Title:  Permit multiple match strings in the Grayscale OCR match string file
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated constructor to register the embedded object msvlMatchLineNumber, set its default values, and initialized m_pBuffer and m_pIndexTable to NULL.
 * Updated addDefaultInputOutputObject to include msvlMatchLineNumber to output list.
 * Updated LoadMatchString to initialize m_nTotalCount, load match strings from file to m_pBuffer, update match string file name, and build hash table.
 * Added methods BuildHashTable, InsertValueToTable, CheckStringInTable.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.7   17 Jul 2001 20:45:20   Joe
 * Project:  SVObserver
 * Change Request (SCR) nbr:  214
 * SCR Title:  Fix OCR analyzer Dr. Watson problem when calculating match score information.
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Removed two class variables: pROIObj and plvImage.
 * 
 * Variables removed from the constructor and destructor as well as the OnRun method.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.6   13 Jun 2001 08:51:46   ed
 * Project:  SVObserver
 * Change Request (SCR) nbr:  200
 * SCR Title:  Update code to implement SVIM COM Server
 * Checked in by:  Ed;  Ed Chobanoff
 * Change Description:  
 *   Modified the following functions: SVOCRGrayAnalyzeResultClass, CreateObject, GenerateFontModel,
 * LoadMatchString, processMessage, onRun.
 * .
 * Added the following functions: GetFontFileName, GetMatchStringFileName, SetFontFileName,
 * SetMatchStringFileName.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.5   06 Feb 2001 13:29:10   cSchmittinger
 * Project:  SVObserver
 * Change Request (SCR) nbr:  191
 * SCR Title:  Restructure File Management
 * Checked in by:  cSchmittinger;  Carl Schmittinger
 * Change Description:  
 *   Modified source code to use new file management classes.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.4   19 Sep 2000 08:54:40   Joe
 * Project:  SVObserver
 * Change Request (SCR) nbr:  182
 * SCR Title:  Fix Intermitten Error in OCR DLLs when going On-line
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Source code changes required to clean-up and check the consistancy of the FastOCR functions.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.3   28 Aug 2000 16:05:46   Joe
 * Project:  SVObserver
 * Change Request (SCR) nbr:  177
 * SCR Title:  Fix Automatic Load Last SEC Problem
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Update FastOCR to prevent memory access violations.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.2   18 Jul 2000 15:48:52   Joe
 * Project:  SVObserver
 * Change Request (SCR) nbr:  169
 * SCR Title:  Add Match Score Information to OCR Tools
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   OCR Match Score Updates
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.1   15 Jun 2000 09:41:18   Joe
 * Project:  SVObserver
 * Change Request (SCR) nbr:  162
 * SCR Title:  Upgrade Coreco Sapera from Version 2.10 to Version 3.00
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Removed all instances of obsolete WiT data types.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.0   16 May 2000 14:44:46   Joe
 * Project:  SVObserver
 * Change Request (SCR) nbr:  133, 132
 * SCR Title:  Add OCR Gray Scale to SVObserver and Create OCR Gray Scale Font Training
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Upgrade Logical Vision WiT v5.3 to Logical Vision FastOCR v1.2.  Completely changed headers, libraries, and variable name types.  Functionality still the same as prior version.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
*/
