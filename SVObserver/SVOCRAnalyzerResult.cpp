//******************************************************************************
//* COPYRIGHT (c) 2003 by SVResearch, Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVOCRAnalyzerResult
//* .File Name       : $Workfile:   SVOCRAnalyzerResult.cpp  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.4  $
//* .Check In Date   : $Date:   01 Feb 2014 11:39:20  $
//******************************************************************************

#include "stdafx.h"
#include "SVOCRAnalyzerResult.h"
#include "SVRunControlLibrary/SVRunControlLibrary.h"
#include "SVImageLibrary/SVImageBufferHandleImage.h"
#include "SVImageLibrary/SVImageBufferHandleInterface.h"
#include "SVGlobal.h"
#include "SVImageClass.h"
#include "SVTool.h"
#include "SVOLicenseManager/SVOLicenseManager.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

SV_IMPLEMENT_CLASS( SVOCRAnalyzeResultClass, SVOCRAnalyzerResultClassGuid );

SVOCRAnalyzeResultClass::SVOCRAnalyzeResultClass( SVObjectClass* POwner, 
	                                                int StringResourceID /* = IDS_CLASSNAME_SVWHITEPIXELANALYZERESULT */ )
										    :SVResultClass( FALSE, POwner, StringResourceID )   // TBD - 16 Jul 1999 - frb. FALSE???
{
	ocrParameters.psvOCRResult = this;

	if ( TheSVOLicenseManager().HasFastOCRLicense() )
	{
		m_bHasLicenseError = false;
	}
	else
	{
		m_bHasLicenseError = true;
	}	
	
	clearAll();
}

// Should only be called from constructor!!!
void SVOCRAnalyzeResultClass::clearAll()
{
	// Identify yourself
	outObjectInfo.ObjectTypeInfo.ObjectType = SVResultObjectType;
	outObjectInfo.ObjectTypeInfo.SubType = SVResultOCRObjectType;

	// Identify our input type needs
	inputImageObjectInfo.SetInputObjectType( SVImageObjectType );
	inputImageObjectInfo.SetObject( GetObjectInfo() );
	RegisterInputObject( &inputImageObjectInfo, _T( "OCRAnalyzerResultImage" ) );

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
		SVOCRParametersObjectGuid,
		IDS_OBJECT_NAME_OCR_PARAMETERS_STRING,
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
		SVOCRMatchLineNumberGuid,
		IDS_OBJECTNAME_OCR_MATCH_LINE_NUMBER,
		false, SVResetItemNone );
	
	// Exposing OCR Match Scores
	RegisterEmbeddedObject( 
		&msvdvocHighestMatchScore,
		SVOCRHighestMatchScoreGuid,
		IDS_OBJECTNAME_OCR_HIGHEST_MATCH_SCORE,
		false, SVResetItemNone );
	
	RegisterEmbeddedObject( 
		&msvdvocLowestMatchScore,
		SVOCRLowestMatchScoreGuid,
		IDS_OBJECTNAME_OCR_LOWEST_MATCH_SCORE,
		false, SVResetItemNone );
	
	RegisterEmbeddedObject( 
		&msvdvocAverageMatchScore,
		SVOCRAverageMatchScoreGuid,
		IDS_OBJECTNAME_OCR_AVERAGE_MATCH_SCORE,
		false, SVResetItemNone );
	
	// OCR File Names
	RegisterEmbeddedObject( 
		&msvfnvocFontFileName,
		SVOCRFontFileNameGuid,
		IDS_OBJECTNAME_OCR_FONT_FILE_NAME,
		false, SVResetItemOwner );
	
	RegisterEmbeddedObject( 
		&msvfnvocMatchStringFileName,
		SVOCRMatchStringFileNameGuid,
		IDS_OBJECTNAME_OCR_MATCH_STRING_FILE_NAME,
		false, SVResetItemOwner );
	
	// Converting all OCR parameters from String to their own Value Objects
	// Will be used when Task Objects are redone
	/*
	RegisterEmbeddedObject( &m_voStrNoMatchLabel,
		                      SVOCRNoMatchStringGuid,
		                      IDS_OBJECTNAME_OCR_NO_MATCH_STRING );
	RegisterEmbeddedObject( &m_voStrMatchString,
		                      SVOCRMatchStringGuid,
		                      IDS_OBJECTNAME_OCR_MATCH_STRING );

	RegisterEmbeddedObject( &m_voRotation,
		                      SVOCRRotationGuid,
		                      IDS_OBJECTNAME_OCR_ROTATION );
	RegisterEmbeddedObject( &m_voScaleInvariant,
		                      SVOCRScaleInvariantGuid,
		                      IDS_OBJECTNAME_OCR_SCALE_INVARIANT );
	RegisterEmbeddedObject( &m_voIgnoreNoMatch,
		                      SVOCRIgnoreNoMatchGuid,
		                      IDS_OBJECTNAME_OCR_IGNORE_NO_MATCH );
	RegisterEmbeddedObject( &m_voSelection,
		                      SVOCRSelectionGuid,
		                      IDS_OBJECTNAME_OCR_SELECTION );
	RegisterEmbeddedObject( &m_voMinWidth,
		                      SVOCRMinWidthGuid,
		                      IDS_OBJECTNAME_OCR_MIN_WIDTH );
	RegisterEmbeddedObject( &m_voMaxWidth,
		                      SVOCRMaxWidthGuid,
		                      IDS_OBJECTNAME_OCR_MAX_WIDTH );
	RegisterEmbeddedObject( &m_voMinHeight,
		                      SVOCRMinHeightGuid,
		                      IDS_OBJECTNAME_OCR_MIN_HEIGHT );
	RegisterEmbeddedObject( &m_voMaxHeight,
		                      SVOCRMaxHeightGuid,
		                      IDS_OBJECTNAME_OCR_MAX_HEIGHT );
	RegisterEmbeddedObject( &m_voSamples,
		                      SVOCRSamplesGuid,
		                      IDS_OBJECTNAME_OCR_SAMPLES );
	RegisterEmbeddedObject( &m_voUseMatchFile,
		                      SVOCRUseMatchFileGuid,
		                      IDS_OBJECTNAME_OCR_USE_MATCH_FILE );
	RegisterEmbeddedObject( &m_voMinPixelsInBlob,
		                      SVOCRMinPixelsInBlobGuid,
		                      IDS_OBJECTNAME_OCR_MIN_PIXELS_IN_BLOB );
	RegisterEmbeddedObject( &m_voMaxPixelsInBlob,
		                      SVOCRMaxPixelsInBlobGuid,
		                      IDS_OBJECTNAME_OCR_MAX_PIXELS_IN_BLOB );

	RegisterEmbeddedObject( &m_voOrientationSensitivity,
		                      SVOCROrientationSensitivityGuid,
		                      IDS_OBJECTNAME_OCR_ORIENTATION_SENSITIVITY );
	RegisterEmbeddedObject( &m_voMinAngle,
		                      SVOCRMinAngleGuid,
		                      IDS_OBJECTNAME_OCR_MIN_ANGLE );
	RegisterEmbeddedObject( &m_voMaxAngle,
		                      SVOCRMaxAngleGuid,
		                      IDS_OBJECTNAME_OCR_MAX_ANGLE );
	RegisterEmbeddedObject( &m_voFeaturesUsed,
		                      SVOCRFeaturesUsedGuid,
		                      IDS_OBJECTNAME_OCR_FEATURES_USED );
	RegisterEmbeddedObject( &m_voMinScale,
		                      SVOCRMinScaleGuid,
		                      IDS_OBJECTNAME_OCR_MIN_SCALE );
	RegisterEmbeddedObject( &m_voMaxScale,
		                      SVOCRMaxScaleGuid,
		                      IDS_OBJECTNAME_OCR_MAX_SCALE );
	RegisterEmbeddedObject( &m_voThreshold,
		                      SVOCRThresholdGuid,
		                      IDS_OBJECTNAME_OCR_THRESHOLD );
	//*/

	// Set Embedded defaults
	matchString.SetDefaultValue( _T( "" ), TRUE );
	foundString.SetDefaultValue( _T( "" ), TRUE );

	valueBlobSortDirection.SetDefaultValue( (long)SORT_BLOBS_RIGHT, TRUE );
	valuePixelsForASpace.SetDefaultValue( (long)-1, TRUE );
	msvlMatchLineNumber.SetDefaultValue( (long)-1, TRUE );

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
	msvfnvocFontFileName.SetDefaultValue( _T(""), TRUE );
	msvfnvocMatchStringFileName.SetDefaultValue( _T(""), TRUE );

	// Add Default Inputs and Outputs
	addDefaultInputObjects();

  //
  // Initialize Wit objects
  //
	pOCRFontModel = NULL;

	presultStringVector = (CorStringVector *)svlvFastOcr.CreateLVObjectData("CorVector");
	plocationVector = (CorFpointVector *)svlvFastOcr.CreateLVObjectData("CorVector");
	plvcfvMatchScoreVector = (CorFloatVector *)svlvFastOcr.CreateLVObjectData("CorVector");
	pROIObj = (CorObj*) svlvFastOcr.CreateLVObject(COR_OBJ_GENERIC);

	m_pBuffer = NULL;
	m_pIndexTable = NULL;
}

BOOL SVOCRAnalyzeResultClass::isWitCompleteLoaded()
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
SVOCRAnalyzeResultClass::~SVOCRAnalyzeResultClass()
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

	if( pOCRFontModel ) 
	{
		svlvFastOcr.DestroyLVObjectData((void **)&pOCRFontModel, "CorFontModel", NULL);
	}

	if(m_pBuffer)
	{
		delete m_pBuffer;
	}

	if(m_pIndexTable)
	{
		delete m_pIndexTable;
	}
}

BOOL SVOCRAnalyzeResultClass::GetFontFileName( CString & csName )
{
	BOOL bOk = FALSE;

	bOk = ( msvfnvocFontFileName.GetValue( csName ) == S_OK );

	return bOk;
}

BOOL SVOCRAnalyzeResultClass::GetMatchStringFileName( CString & csName )
{
	BOOL bOk = FALSE;

	bOk = ( msvfnvocMatchStringFileName.GetValue( csName ) == S_OK );

	return bOk;
}

SVImageClass* SVOCRAnalyzeResultClass::getInputImage()
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
BOOL SVOCRAnalyzeResultClass::CreateObject(	SVObjectLevelCreateStruct* PCreateStructure )
{
	BOOL bOk = SVResultClass::CreateObject( PCreateStructure );

	if ( bOk )
	{
		bOk = getInputImage() != NULL;
	}

	if ( !m_bHasLicenseError )
	{
		if ( bOk )
		{
			bOk = isWitCompleteLoaded();
			
			if ( ! bOk )
			{
			   AfxMessageBox("ERROR: Failed to Load OCR Library");   // 24 Jun 1999 - frb.
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
	}//if (HasOCRLicense())
	if ( m_bHasLicenseError )
	{
	   AfxMessageBox("ERROR: Failed to Load OCR Library (Fast OCR not supported with current model number)");
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

	msvfnvocFontFileName.ObjectAttributesAllowedRef() |= SV_PRINTABLE;
	msvfnvocMatchStringFileName.ObjectAttributesAllowedRef() |= SV_PRINTABLE;

	m_voStrNoMatchLabel.ObjectAttributesAllowedRef() &= ~SV_PRINTABLE;
	m_voStrMatchString.ObjectAttributesAllowedRef() &= ~SV_PRINTABLE;

	m_voRotation.ObjectAttributesAllowedRef() &= ~SV_PRINTABLE;
	m_voScaleInvariant.ObjectAttributesAllowedRef() &= ~SV_PRINTABLE;
	m_voIgnoreNoMatch.ObjectAttributesAllowedRef() &= ~SV_PRINTABLE;
	m_voSelection.ObjectAttributesAllowedRef() &= ~SV_PRINTABLE;
	m_voMinWidth.ObjectAttributesAllowedRef() &= ~SV_PRINTABLE;
	m_voMaxWidth.ObjectAttributesAllowedRef() &= ~SV_PRINTABLE;
	m_voMinHeight.ObjectAttributesAllowedRef() &= ~SV_PRINTABLE;
	m_voMaxHeight.ObjectAttributesAllowedRef() &= ~SV_PRINTABLE;
	m_voSamples.ObjectAttributesAllowedRef() &= ~SV_PRINTABLE;
	m_voUseMatchFile.ObjectAttributesAllowedRef() &= ~SV_PRINTABLE;
	m_voMinPixelsInBlob.ObjectAttributesAllowedRef() &= ~SV_PRINTABLE;
	m_voMaxPixelsInBlob.ObjectAttributesAllowedRef() &= ~SV_PRINTABLE;

	m_voOrientationSensitivity.ObjectAttributesAllowedRef() &= ~SV_PRINTABLE;
	m_voMinAngle.ObjectAttributesAllowedRef() &= ~SV_PRINTABLE;
	m_voMaxAngle.ObjectAttributesAllowedRef() &= ~SV_PRINTABLE;
	m_voFeaturesUsed.ObjectAttributesAllowedRef() &= ~SV_PRINTABLE;
	m_voMinScale.ObjectAttributesAllowedRef() &= ~SV_PRINTABLE;
	m_voMaxScale.ObjectAttributesAllowedRef() &= ~SV_PRINTABLE;
	m_voThreshold.ObjectAttributesAllowedRef() &= ~SV_PRINTABLE;

	isCreated = bOk;

	return bOk;
}

/////////////////////////////////////////////////////////////////////////////
//
//
//
//
BOOL SVOCRAnalyzeResultClass::CloseObject()
{
	BOOL bOk = FALSE;

	if( SVResultClass::CloseObject() )
	{
		bOk = TRUE;

		if( pOCRFontModel ) 
		{
			bOk = svlvFastOcr.DestroyLVObjectData( (void **)(&pOCRFontModel), "CorFontModel", NULL );
		}
	}

	return bOk;
}

//******************************************************************************
// Operation(s) Of Reading Access:
//******************************************************************************

void SVOCRAnalyzeResultClass::UpdateOCRScriptString()
{
	CString csTemp = ocrParameters.ConvertParamsToString();
	StrOcrParameters.SetValue( 1, (LPTSTR)(LPCTSTR)csTemp);
}

SVOCRParamStruct& SVOCRAnalyzeResultClass::GetOCRParameters()
{
	return ocrParameters;
}

void SVOCRAnalyzeResultClass::GetOCRResultString( CString & csResult )
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
CRect SVOCRAnalyzeResultClass::Draw( HDC DC, CRect R )
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
BOOL SVOCRAnalyzeResultClass::GenerateFontModel()
{
	BOOL bOk = FALSE;
	if ( !m_bHasLicenseError )
	{
		bOk = isWitCompleteLoaded();
		if ( bOk )
		{
			if( pOCRFontModel ) 
			{
				bOk = svlvFastOcr.DestroyLVObjectData( (void **)(&pOCRFontModel), "CorFontModel", NULL );
			}
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
					bOk = svlvFastOcr.InitializeLVOCRPerim( (CorOcrFont *)CorObj_OcrFont(pOcrFontObject),
																									&pOCRFontModel, 
																									ocrParameters.samples );
				}
			}

			//
			// Free memory allocated by rdObj for fontParamObj
			//
			bOk = svlvFastOcr.DestroyLVObject(&pOcrFontObject) && bOk;
		}
	}

	if ( ! bOk && pOCRFontModel != NULL )
	{
		svlvFastOcr.DestroyLVObjectData( (void **)(&pOCRFontModel), "CorFontModel", NULL );
	}

	return bOk;
}

/////////////////////////////////////////////////////////////////////////////
//
//
//
//
void  SVOCRAnalyzeResultClass::WriteToArchive(CFile & fileArchive)
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
HRESULT SVOCRAnalyzeResultClass::LoadMatchString()
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

		if ( dwByteNumber > 0 )
		{
			// Sri 4-25-00
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

			// Set the matchString to be Empty. If the file contains only one entry, assign that
			// string to the matchString. This will be done inside 'BuildHashTable'.
			strMatchString.Format ("");
			matchString.SetValue( 1, (LPTSTR)(LPCTSTR)strMatchString );
		}
		else
		{
			m_lTotalBytes = 0;
		}

	   if ( m_lTotalBytes > 0 )
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
	   }
// End. Sri

      //CString strMatchString = pBuffer;
		//CString csMatch;
		//matchString.GetValue( csMatch );
		//sv_strncpy( (LPTSTR)(LPCTSTR)csMatch, strMatchString, 100 ); 
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
BOOL SVOCRAnalyzeResultClass::OnValidate()
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

		bRetVal = pOCRFontModel != NULL && bRetVal;

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
LONG_PTR SVOCRAnalyzeResultClass::processMessage( DWORD DwMessageID, LONG_PTR DwMessageValue, LONG_PTR DwMessageContext )
{
	LONG_PTR DwResult = NULL;

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

HRESULT SVOCRAnalyzeResultClass::ResetObject()
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
BOOL SVOCRAnalyzeResultClass::onRun( SVRunStatusClass& RRunStatus )
{
	BOOL bOk = FALSE;
	
	if (m_bHasLicenseError)
	{
		SetInvalid();
		RRunStatus.SetInvalid();
		return bOk;
	}
    
	bOk = SVResultClass::onRun( RRunStatus );

	if ( bOk && !RRunStatus.IsDisabled() && !RRunStatus.IsDisabledByCondition() )
	{
		long imagePitchMil = 0;
		long imageSizeYMil = 0;

		BYTE* pMilBuffer = NULL;

		if ( bOk )
		{
			bOk = pOCRFontModel != NULL;

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
					l_Code = SVMatroxBufferInterface::GetHostAddress( &pMilBuffer, milBuffer );

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
						//
						// Check type of image buffer
						//
						long imageTypeMil = -1;
						l_Code = SVMatroxBufferInterface::Get( milBuffer, SVType, imageTypeMil );

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

				CorImage *plvImage = (CorImage *)svlvFastOcr.CreateLVObjectData("CorImage");
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

					//CorObj *pROIObj = svlvFastOcr.CreateLVObject(COR_OBJ_GENERIC);
					//bOk = pROIObj != NULL;

					if ( ! bOk )
					{
						SetInvalid();
					}
					else
					{
						bOk = svlvFastOcr.RunLVOCRPerim( plvImage, pROIObj, pOCRFontModel,
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

					msvlMatchLineNumber.SetValue(RRunStatus.m_lResultDataIndex, (const long) -1L);

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

						int nResultValue;
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

////////////////////////////////////////////////////////////////////////////////
// .Title       : BuildHashTable ()
// -----------------------------------------------------------------------------
// .Description : 'pBuffer' is a char array which contains the entire file. Number of  
//              : entrees are calculated by counting the number of CRs (0x0D) and
//              : each entree is assigned to a char pointer array and the CR is replaced by a 
//              : Null terminator. 
//              : Allocate a table of short int, 10 times the size of the number of entrees. 
//              : calculate the hash index and call 'InsertValueToTable' to insert the 
//              : index of the string to the hash table.
////////////////////////////////////////////////////////////////////////////////
// 
//	 Date		   Author		Comment
//  4-25-00    sri			First Implementation
//	:
////////////////////////////////////////////////////////////////////////////////
BOOL SVOCRAnalyzeResultClass::BuildHashTable(char *pBuffer)
{
   long lBufIndex = 0;
   BOOL  bRet = TRUE;

   for(int i = 0; i < MAX_ENTREES; i++)
      m_pDataArr[i] = 0;

   for(m_nTotalCount = 0; 
         (m_nTotalCount < MAX_ENTREES) && (lBufIndex < m_lTotalBytes);
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
void SVOCRAnalyzeResultClass::InsertValueToTable(short nValue, int nIndex)
{
// Check whether the location contains a data

   if(m_pIndexTable[nIndex] == 0)
      m_pIndexTable[nIndex] = nValue;
   else
   {
//    
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
int SVOCRAnalyzeResultClass::CheckStringInTable(CString MatchString)
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
$Log:   N:\PVCSarch65\ProjectFiles\archives\SVObserver_SRC\SVObserver\SVOCRAnalyzerResult.cpp_v  $
 * 
 *    Rev 1.4   01 Feb 2014 11:39:20   tbair
 * Project:  SVObserver
 * Change Request (SCR) nbr:  852
 * SCR Title:  Add Multiple Platform Support to SVObserver's Visual Studio Solution
 * Checked in by:  tBair;  Tom Bair
 * Change Description:  
 *   Changed SVSendmessage and processmessage to use LONG_PTR instead of DWORD.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.3   02 Oct 2013 06:48:20   tbair
 * Project:  SVObserver
 * Change Request (SCR) nbr:  852
 * SCR Title:  Add Multiple Platform Support to SVObserver's Visual Studio Solution
 * Checked in by:  tBair;  Tom Bair
 * Change Description:  
 *   Add x64 platform.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.2   13 May 2013 12:36:50   bWalter
 * Project:  SVObserver
 * Change Request (SCR) nbr:  814
 * SCR Title:  Upgrade SVObserver to Compile Using Visual Studio 2010
 * Checked in by:  bWalter;  Ben Walter
 * Change Description:  
 *   Merged with svo_src label SVO 6.10 Beta 014.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
      Rev 3.46   02 May 2013 11:21:46   jspila
   Project:  SVObserver
   Change Request (SCR) nbr:  810
   SCR Title:  Remove support for Legacy Tools
   Checked in by:  rYoho;  Rob Yoho
   Change Description:  
     Updated process message method to fix formatting issues and updated the reset object section to only assert when the HRESULT is a failed status.
   
   /////////////////////////////////////////////////////////////////////////////////////
   
 *    Rev 1.1   06 May 2013 14:38:26   bWalter
 * Project:  SVObserver
 * Change Request (SCR) nbr:  814
 * SCR Title:  Upgrade SVObserver to Compile Using Visual Studio 2010
 * Checked in by:  bWalter;  Ben Walter
 * Change Description:  
 *   Merged with svo_src label SVO 6.10 Beta 009.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
      Rev 3.45   10 Apr 2013 14:53:04   ryoho
   Project:  SVObserver
   Change Request (SCR) nbr:  796
   SCR Title:  Add a License Manager that will handle the Matrox Licenses
   Checked in by:  rYoho;  Rob Yoho
   Change Description:  
     put in check for FastOCRLicense
   
   /////////////////////////////////////////////////////////////////////////////////////
   
 *    Rev 1.0   23 Apr 2013 12:48:20   bWalter
 * Project:  SVObserver
 * Change Request (SCR) nbr:  814
 * SCR Title:  Upgrade SVObserver to Compile Using Visual Studio 2010
 * Checked in by:  bWalter;  Ben Walter
 * Change Description:  
 *   Initial check in to SVObserver_src.  (Merged with svo_src label SVO 6.10 Beta 008.)
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
   
      Rev 3.44   25 Mar 2013 12:53:26   ryoho
   Project:  SVObserver
   Change Request (SCR) nbr:  796
   SCR Title:  Add a License Manager that will handle the Matrox Licenses
   Checked in by:  rYoho;  Rob Yoho
   Change Description:  
     removed check to license error in the CreateObject
   
   /////////////////////////////////////////////////////////////////////////////////////
   
      Rev 3.43   27 Feb 2013 11:25:40   ryoho
   Project:  SVObserver
   Change Request (SCR) nbr:  796
   SCR Title:  Add a License Manager that will handle the Matrox Licenses
   Checked in by:  rYoho;  Rob Yoho
   Change Description:  
     put in checks to see if it has license errors
   
   /////////////////////////////////////////////////////////////////////////////////////
   
      Rev 3.42   12 Jul 2012 15:03:16   jspila
   Project:  SVObserver
   Change Request (SCR) nbr:  769
   SCR Title:  Fix Problems and Crashes with Inspection Document Display Updates
   Checked in by:  Joe;  Joe Spila
   Change Description:  
     Updated source code to add in an additional string identifier for the input and fixed clear functionality assoicated with input structure.
   
   /////////////////////////////////////////////////////////////////////////////////////
   
      Rev 3.41   19 Jun 2012 13:11:00   jspila
   Project:  SVObserver
   Change Request (SCR) nbr:  744
   SCR Title:  Add Shared Memory and Socket Functionality for Run Page Web Server
   Checked in by:  Joe;  Joe Spila
   Change Description:  
     Protected Object Information structure attributes.  This change will add accessor methods to get and set attributes.
   Converted GUID to SVGUID.   This allows the use of standard operators for comparison and methods for conversion.
   
   
   /////////////////////////////////////////////////////////////////////////////////////
   
      Rev 3.40   10 Feb 2011 15:05:20   jspila
   Project:  SVObserver
   Change Request (SCR) nbr:  712
   SCR Title:  Fix issues with black images when using command interface (SIAC)
   Checked in by:  Joe;  Joe Spila
   Change Description:  
     Updated source code to now use shared pointer instead of untracked structure.  This fixes destruction, tracking and rebuilding issues.
   
   /////////////////////////////////////////////////////////////////////////////////////
   
      Rev 3.39   27 Jan 2011 11:41:54   jspila
   Project:  SVObserver
   Change Request (SCR) nbr:  712
   SCR Title:  Fix issues with black images when using command interface (SIAC)
   Checked in by:  Joe;  Joe Spila
   Change Description:  
     Updated source code to fix issues with Matrox Buffer Management.
   
   /////////////////////////////////////////////////////////////////////////////////////
   
      Rev 3.38   22 Jun 2010 10:00:02   jspila
   Project:  SVObserver
   Change Request (SCR) nbr:  693
   SCR Title:  Fix Performance Issue with Inspection Process
   Checked in by:  Joe;  Joe Spila
   Change Description:  
     Fixed issues with registered inputs.
   
   /////////////////////////////////////////////////////////////////////////////////////
   
      Rev 3.37   01 Jun 2010 14:57:18   jspila
   Project:  SVObserver
   Change Request (SCR) nbr:  693
   SCR Title:  Fix Performance Issue with Inspection Process
   Checked in by:  Joe;  Joe Spila
   Change Description:  
     Updated source code to improve ability to track performance.
   
   /////////////////////////////////////////////////////////////////////////////////////
   
      Rev 3.36   16 Dec 2009 12:15:46   jspila
   Project:  SVObserver
   Change Request (SCR) nbr:  677
   SCR Title:  Fix problem in camera notify thread
   Checked in by:  Joe;  Joe Spila
   Change Description:  
     Fix issues with includes and comments.
   
   /////////////////////////////////////////////////////////////////////////////////////
   
      Rev 3.35   03 Sep 2009 10:35:08   jspila
   Project:  SVObserver
   Change Request (SCR) nbr:  665
   SCR Title:  Fix unrecoverable failure when processing acquisitions at high speed
   Checked in by:  Joe;  Joe Spila
   Change Description:  
     Updated files to fix moved file includes.
   
   /////////////////////////////////////////////////////////////////////////////////////
   
      Rev 3.34   24 Jul 2007 15:17:34   tbair
   Project:  SVObserver
   Change Request (SCR) nbr:  597
   SCR Title:  Upgrade Matrox Imaging Library to version 8.0
   Checked in by:  tBair;  Tom Bair
   Change Description:  
     Update MIL 8.0
   
   /////////////////////////////////////////////////////////////////////////////////////
   
      Rev 3.33   22 Jun 2007 10:42:56   Joe
   Project:  SVObserver
   Change Request (SCR) nbr:  598
   SCR Title:  Upgrade SVObserver to compile using vc++ in VS2005
   Checked in by:  jSpila;  Joseph Spila
   Change Description:  
     These changes include removal of all VC6 constraints, project files, and workspace files.  The new VC8 project and solution files will replace the VC6 files with the new tighter compilation rules.
   
   /////////////////////////////////////////////////////////////////////////////////////
   
      Rev 3.32   21 Jun 2007 14:34:00   Joe
   Project:  SVObserver
   Change Request (SCR) nbr:  598
   SCR Title:  Upgrade SVObserver to compile using vc++ in VS2005
   Checked in by:  jSpila;  Joseph Spila
   Change Description:  
     These changes include modification based on fixing compiler-based and project-based differences between VC6 and VC8.  These changes mainly include casting issues, but some include type conversion and assignment of new compiler controlling defines.
   
   /////////////////////////////////////////////////////////////////////////////////////
   
      Rev 3.31   10 Aug 2005 12:40:34   Joe
   Project:  SVObserver
   Change Request (SCR) nbr:  500
   SCR Title:  Reduce delay when adjusting tool parameters with a large toolset
   Checked in by:  Joe;  Joe Spila
   Change Description:  
     Updated methods to improve performance and reduce system delay.
   
   /////////////////////////////////////////////////////////////////////////////////////
   
      Rev 3.30   09 Aug 2005 07:51:18   Joe
   Project:  SVObserver
   Change Request (SCR) nbr:  500
   SCR Title:  Reduce delay when adjusting tool parameters with a large toolset
   Checked in by:  Joe;  Joe Spila
   Change Description:  
     Updated objects and dialogs to use the ActiveX Interface to update inspection data.
   
   /////////////////////////////////////////////////////////////////////////////////////
   
      Rev 3.29   29 Jul 2005 13:08:06   Joe
   Project:  SVObserver
   Change Request (SCR) nbr:  500
   SCR Title:  Reduce delay when adjusting tool parameters with a large toolset
   Checked in by:  Joe;  Joe Spila
   Change Description:  
     Updated object initialization to change the parameters for RegisterEmbeddedObject for a SVValueObjectClass.  The two additional parameters will inform the preperation process before toolset execution.
   
   /////////////////////////////////////////////////////////////////////////////////////
   
      Rev 3.28   21 Jun 2005 08:21:22   ebeyeler
   Project:  SVObserver
   Change Request (SCR) nbr:  464
   SCR Title:  Add array indexing for value objects
   Checked in by:  eBeyeler;  Eric Beyeler
   Change Description:  
     object attributes now use accessor methods
   value object functions now use HRESULT
   
   /////////////////////////////////////////////////////////////////////////////////////
   
      Rev 3.27   09 Mar 2005 07:06:22   Joe
   Project:  SVObserver
   Change Request (SCR) nbr:  456
   SCR Title:  Update Image and Tool Objects to use the new Extent Classes
   Checked in by:  Joe;  Joe Spila
   Change Description:  
     Removed return code check within ResetObject method and moved validity check to OnValidate.
   
   /////////////////////////////////////////////////////////////////////////////////////
   
      Rev 3.26   17 Feb 2005 15:01:20   Joe
   Project:  SVObserver
   Change Request (SCR) nbr:  456
   SCR Title:  Update Image and Tool Objects to use the new Extent Classes
   Checked in by:  Joe;  Joe Spila
   Change Description:  
     Update class with new methodologies based on new base object, extents and reset structure.
   
   /////////////////////////////////////////////////////////////////////////////////////
   
      Rev 3.25   08 Jul 2004 10:38:02   ryoho
   Project:  SVObserver
   Change Request (SCR) nbr:  431
   SCR Title:  Fix problem with using match string files that are zero elements
   Checked in by:  rYoho;  Rob Yoho
   Change Description:  
     put in check to make sure that if using a match string file that the number of elements is above 0.  if not, the inspection will not be allowed to go online
   
   /////////////////////////////////////////////////////////////////////////////////////
   
      Rev 3.24   26 Nov 2003 10:57:08   rschock
   Project:  SVObserver
   Change Request (SCR) nbr:  364
   SCR Title:  OCR Analyzers show wrong result for "line number of match string" in results.
   Checked in by:  rSchock;  Rosco Schock
   Change Description:  
     Forced match string to be copied forward in the onRun.
   
   /////////////////////////////////////////////////////////////////////////////////////
   
      Rev 3.23   21 May 2003 17:51:14   rschock
   Project:  SVObserver
   Change Request (SCR) nbr:  364
   SCR Title:  OCR Analyzers show wrong result for "line number of match string" in results.
   Checked in by:  rSchock;  Rosco Schock
   Change Description:  
     Fixed the Run functions to set the "Line number of match string" value object in the current result data index.
   
   /////////////////////////////////////////////////////////////////////////////////////
   
      Rev 3.22   30 Apr 2003 17:02:24   rschock
   Project:  SVObserver
   Change Request (SCR) nbr:  346
   SCR Title:  Update SVObserver to Version 4.21 Release
   Checked in by:  Joe;  Joe Spila
   Change Description:  
     Fixed a couple of small memory leaks in the ::onRun.
   
   /////////////////////////////////////////////////////////////////////////////////////
   
      Rev 3.21   22 Apr 2003 12:42:02   rschock
   Project:  SVObserver
   Change Request (SCR) nbr:  346
   SCR Title:  Update SVObserver to Version 4.21 Release
   Checked in by:  Joe;  Joe Spila
   Change Description:  
     Redid the #include defines and standardized the Tracker log headers and removed warning from release mode builds.
   
   /////////////////////////////////////////////////////////////////////////////////////
   
      Rev 3.20   30 Jan 2003 15:20:18   joe
   Project:  SVObserver
   Change Request (SCR) nbr:  301
   SCR Title:  Implement Result Image Circle Buffer
   Checked in by:  Joe;  Joe Spila
   Change Description:  
     Updated onRun and CreateObject methods to use the new image circle buffer methodology.
   
   /////////////////////////////////////////////////////////////////////////////////////
   
      Rev 3.19   09 Dec 2002 19:46:32   Joe
   Project:  SVObserver
   Change Request (SCR) nbr:  295
   SCR Title:  Remove Result Data from Configuration Printout
   Checked in by:  Joe;  Joe Spila
   Change Description:  
     CreateObject method was modified to properly update the isCreated flag and to set the printability of the internal value objects.
   
   /////////////////////////////////////////////////////////////////////////////////////
   
      Rev 3.18   20 Nov 2002 10:05:14   ryoho
   Project:  SVObserver
   Change Request (SCR) nbr:  272
   SCR Title:  Outputs do not always get set correctly.
   Checked in by:  rYoho;  Rob Yoho
   Change Description:  
     Add some RegisterEmbeddedObject
   
   /////////////////////////////////////////////////////////////////////////////////////
   
      Rev 3.17   04 Oct 2001 16:45:42   Joe
   Project:  SVObserver
   Change Request (SCR) nbr:  229
   SCR Title:  Upgrade SVObserver to Version 3.34 Release
   Checked in by:  Joe;  Joe Spila
   Change Description:  
     Modified source code to remove compiler warning message.
   
   /////////////////////////////////////////////////////////////////////////////////////
*/
