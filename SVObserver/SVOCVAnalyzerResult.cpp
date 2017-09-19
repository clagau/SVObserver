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
#include "SVOCore/SVTool.h"
#include "SVOLicenseManager/SVOLicenseManager.h"
#include "SVStatusLibrary\MessageManager.h"
#include "TextDefinesSvO.h"
#include "SVStatusLibrary/ErrorNumbers.h"
#include "SVStatusLibrary\MessageManager.h"
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
						:SVResultClass(POwner, StringResourceID )
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
	m_outObjectInfo.m_ObjectTypeInfo.ObjectType = SVResultObjectType;
	m_outObjectInfo.m_ObjectTypeInfo.SubType = SVResultOCVObjectType;

	// Identify our input type needs
	m_inputObjectInfo.SetInputObjectType( SVImageObjectType );
	m_inputObjectInfo.SetObject( GetObjectInfo() );
	RegisterInputObject( &m_inputObjectInfo, _T( "OCVAnalyzerResultImage" ) );

	// Register Embedded Objects
	RegisterEmbeddedObject(
		&m_svoMatchString, 
		SVOCVMatchStringGuid,
		IDS_OBJECTNAME_OCV_MATCH_STRING,
		false, SvOi::SVResetItemNone );
	
	RegisterEmbeddedObject( 
		&m_svoFoundString, 
		SVOCVFoundStringGuid,
		IDS_OBJECTNAME_OCV_FOUND_STRING,
		false, SvOi::SVResetItemNone );
	
	RegisterEmbeddedObject( 
		&m_bvoPerformOCR,
		SVOCVPerformOCRGuid,
		IDS_OBJECTNAME_OCV_PERFORM_OCR,
		false, SvOi::SVResetItemNone );
	
	RegisterEmbeddedObject( 
		&m_lvoMatchLineNumber,
		SVOCVMatchLineNumberGuid,
		IDS_OBJECTNAME_OCV_MATCH_LINE_NUMBER,
		false, SvOi::SVResetItemNone );
	
	// Exposing OCV Match Scores
	RegisterEmbeddedObject( 
		&m_dvoHighestMatchScore,
		SVOCVHighestMatchScoreGuid,
		IDS_OBJECTNAME_OCV_HIGHEST_MATCH_SCORE,
		false, SvOi::SVResetItemNone );
	
	RegisterEmbeddedObject( 
		&m_dvoLowestMatchScore,
		SVOCVLowestMatchScoreGuid,
		IDS_OBJECTNAME_OCV_LOWEST_MATCH_SCORE,
		false, SvOi::SVResetItemNone );
	
	RegisterEmbeddedObject( 
		&m_dvoAverageMatchScore,
		SVOCVAverageMatchScoreGuid,
		IDS_OBJECTNAME_OCV_AVERAGE_MATCH_SCORE,
		false, SvOi::SVResetItemNone );
	
	// OCV File Names
	RegisterEmbeddedObject( 
		&m_fnvoFontFileName,
		SVOCVFontFileNameGuid,
		IDS_OBJECTNAME_OCV_FONT_FILE_NAME,
		false, SvOi::SVResetItemOwner );
	
	RegisterEmbeddedObject( 
		&m_fnvoConstraintsFileName,
		SVOCVConstraintsFileNameGuid,
		IDS_OBJECTNAME_OCV_CONSTRAINTS_FILE_NAME,
		false, SvOi::SVResetItemOwner );
	
	RegisterEmbeddedObject( 
		&m_fnvoControlsFileName,
		SVOCVControlsFileNameGuid,
		IDS_OBJECTNAME_OCV_CONTROLS_FILE_NAME,
		false, SvOi::SVResetItemOwner );
	
	RegisterEmbeddedObject(
		&m_fnvoMatchStringFileName,
		SVOCVMatchStringFileNameGuid,
		IDS_OBJECTNAME_OCV_MATCH_STRING_FILE_NAME,
		false, SvOi::SVResetItemOwner );
	
	RegisterEmbeddedObject( 
		&m_bvoUseMatchFile,
		SVOCVUseMatchFileGuid,
		IDS_OBJECTNAME_OCV_USE_MATCH_FILE,
		false, SvOi::SVResetItemOwner );
	
	for( long l = 0; l < OCV_MAX_RESULTS; l++ )
	{
		SVOCVCharacterResultClass *pResult = new SVOCVCharacterResultClass( this );
		// Add it to our task object list
		Add( pResult );

		SVString Name = SvUl_SF::Format( "Character %d Result", l + 1 );
		pResult->SetName( Name.c_str() );
	}// end for

	// Set Embedded defaults
	m_svoMatchString.SetDefaultValue( _T( "" ), true );
	m_svoFoundString.SetDefaultValue( _T( "" ), true);
	m_svoFoundString.setSaveValueFlag(false);

	m_lvoMatchLineNumber.SetDefaultValue ( -1l);
	m_lvoMatchLineNumber.setSaveValueFlag(false);
	m_bvoPerformOCR.SetDefaultValue ( 1l );
	m_bvoUseMatchFile.SetDefaultValue( BOOL(false));

	// Setting exposed OCV Match Score defaults
	m_dvoHighestMatchScore.SetDefaultValue( -1.0);
	m_dvoHighestMatchScore.setSaveValueFlag(false);
	m_dvoLowestMatchScore.SetDefaultValue( -1.0);
	m_dvoLowestMatchScore.setSaveValueFlag(false);
	m_dvoAverageMatchScore.SetDefaultValue( -1.0);
	m_dvoAverageMatchScore.setSaveValueFlag(false);

	// Setting exposed OCV File Name defaults
	m_fnvoFontFileName.SetDefaultValue( _T(""), true);
	m_fnvoConstraintsFileName.SetDefaultValue( _T(""), true);
	m_fnvoControlsFileName.SetDefaultValue( _T(""), true);
	m_fnvoMatchStringFileName.SetDefaultValue( _T(""), true);
	m_bvoUseMatchFile.SetDefaultValue( BOOL(false), true);

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
		HRESULT l_Code;

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
	if( m_inputObjectInfo.IsConnected() )
	{
		return dynamic_cast<SVImageClass*> (m_inputObjectInfo.GetInputObjectInfo().m_pObject);
	}
	return nullptr;
}

bool SVOCVAnalyzeResultClass::CreateObject(	const SVObjectLevelCreateStruct& rCreateStructure )
{
	bool bOk = SVResultClass::CreateObject(rCreateStructure);

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
	UINT cAttibutes = SvOi::SV_PRINTABLE | SvOi::SV_SETABLE_ONLINE | SvOi::SV_REMOTELY_SETABLE;
	m_svoMatchString.SetObjectAttributesAllowed( cAttibutes, SvOi::SetAttributeType::AddAttribute );
	m_svoFoundString.SetObjectAttributesAllowed( SvOi::SV_PRINTABLE, SvOi::SetAttributeType::RemoveAttribute );
	m_lvoMatchLineNumber.SetObjectAttributesAllowed( SvOi::SV_PRINTABLE, SvOi::SetAttributeType::RemoveAttribute );
	m_bvoPerformOCR.SetObjectAttributesAllowed( cAttibutes, SvOi::SetAttributeType::OverwriteAttribute );

	m_dvoHighestMatchScore.SetObjectAttributesAllowed( SvOi::SV_PRINTABLE, SvOi::SetAttributeType::RemoveAttribute );
	m_dvoLowestMatchScore.SetObjectAttributesAllowed( SvOi::SV_PRINTABLE, SvOi::SetAttributeType::RemoveAttribute );
	m_dvoAverageMatchScore.SetObjectAttributesAllowed( SvOi::SV_PRINTABLE, SvOi::SetAttributeType::RemoveAttribute );

	m_fnvoFontFileName.SetObjectAttributesAllowed( cAttibutes, SvOi::SetAttributeType::AddAttribute );
	m_fnvoConstraintsFileName.SetObjectAttributesAllowed( cAttibutes, SvOi::SetAttributeType::AddAttribute );
	m_fnvoControlsFileName.SetObjectAttributesAllowed( cAttibutes, SvOi::SetAttributeType::AddAttribute );
	m_fnvoMatchStringFileName.SetObjectAttributesAllowed( cAttibutes, SvOi::SetAttributeType::AddAttribute );
	m_bvoUseMatchFile.SetObjectAttributesAllowed( cAttibutes, SvOi::SetAttributeType::OverwriteAttribute );

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
			pResult->SetObjectAttributesAllowed( SvOi::SV_EMBEDABLE, SvOi::SetAttributeType::OverwriteAttribute );

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
bool SVOCVAnalyzeResultClass::CloseObject()
{
	bool bOk = SVResultClass::CloseObject();

	if( bOk )
	{
		HRESULT l_Code;

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
bool SVOCVAnalyzeResultClass::GenerateFontModel()
{
	long	l_lIsFontPreprocessed = 0;

	HRESULT MatroxCode;
	// First destroy the MIL font and result buffer
	if( !m_milFontID.empty() )
	{
		MatroxCode = SVMatroxOcrInterface::Destroy( m_milFontID );
	}// end if

	if( !m_milResultID.empty() )
	{
		MatroxCode = SVMatroxOcrInterface::Destroy( m_milResultID );
	}// end if

	// Now recreate the MIL font and result buffer
	CFileStatus rStatus;
	SVString FontFileName;
	SVString ConstraintsFileName;
	SVString ControlsFileName;

	m_fnvoFontFileName.GetValue( FontFileName );
	bool bOk = (CFile::GetStatus(FontFileName.c_str(), rStatus) ? true : false);
	if ( bOk )
	{
		bOk = 0L < rStatus.m_size;
	}// end if

	if( bOk )
	{
		m_fnvoControlsFileName.GetValue( ControlsFileName );
		if( !ControlsFileName.empty() )
		{
			bOk = (CFile::GetStatus(ControlsFileName.c_str(), rStatus) ? true : false);
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
			bOk = (CFile::GetStatus( ConstraintsFileName.c_str(), rStatus ) ? true : false);
			if ( bOk )
			{
				bOk = 0L < rStatus.m_size;
			}// end if
		}// end if
	}// end if

	if ( bOk )
	{
		SVString Path;
		Path = FontFileName;

		MatroxCode = SVMatroxOcrInterface::RestoreFont( m_milFontID, Path, SVOcrRestore );
		if( m_milFontID.empty() )
		{
			SvStl::MessageMgrStd MesMan( SvStl::LogOnly );
		MesMan.setMessage( SVMSG_SVO_103_REPLACE_ERROR_TRAP, SvStl::Tid_UnexpectedError, SvStl::SourceFileParams(StdMessageParams), SvStl::Err_16126, GetUniqueObjectID());

		}// end if

		if( !ControlsFileName.empty() )
		{
			Path = ControlsFileName;
			MatroxCode = SVMatroxOcrInterface::RestoreFont( m_milFontID, Path, SVOcrLoadControl );
			if( m_milFontID.empty() )
			{
				SvStl::MessageMgrStd MesMan( SvStl::LogOnly );
			MesMan.setMessage( SVMSG_SVO_103_REPLACE_ERROR_TRAP, SvStl::Tid_UnexpectedError, SvStl::SourceFileParams(StdMessageParams), SvStl::Err_16127, GetUniqueObjectID());
			}// end if
		}// end if

		if( !ConstraintsFileName.empty() )
		{
			Path = ConstraintsFileName;
			MatroxCode = SVMatroxOcrInterface::RestoreFont( m_milFontID, Path, SVOcrLoadConstraint );

			if( m_milFontID.empty() )
			{
				SvStl::MessageMgrStd MesMan( SvStl::LogOnly );
			MesMan.setMessage( SVMSG_SVO_103_REPLACE_ERROR_TRAP, SvStl::Tid_UnexpectedError, SvStl::SourceFileParams(StdMessageParams), SvStl::Err_16128, GetUniqueObjectID());
			}// end if
		}// end if

		HRESULT MatroxCode = SVMatroxOcrInterface::Get (m_milFontID, SVOcrIsFontPreprocessed, l_lIsFontPreprocessed);
		if (S_OK != MatroxCode)
		{
			SvStl::MessageMgrStd MesMan( SvStl::LogOnly );
			MesMan.setMessage( SVMSG_SVO_103_REPLACE_ERROR_TRAP, SvStl::Tid_UnexpectedError, SvStl::SourceFileParams(StdMessageParams), SvStl::Err_16129, GetUniqueObjectID());
		}

		if (l_lIsFontPreprocessed == 0)
		{
			MatroxCode = SVMatroxOcrInterface::Preprocess( m_milFontID );

			if (S_OK != MatroxCode)
			{
				SvStl::MessageMgrStd MesMan( SvStl::LogOnly );
				MesMan.setMessage( SVMSG_SVO_103_REPLACE_ERROR_TRAP, SvStl::Tid_UnexpectedError, SvStl::SourceFileParams(StdMessageParams), SvStl::Err_16130, GetUniqueObjectID());
			}
		}

		MatroxCode = SVMatroxOcrInterface::Create( m_milResultID );
		if( m_milResultID.empty() )
		{
			SvStl::MessageMgrStd MesMan( SvStl::LogOnly );
			MesMan.setMessage( SVMSG_SVO_103_REPLACE_ERROR_TRAP, SvStl::Tid_UnexpectedError, SvStl::SourceFileParams(StdMessageParams), SvStl::Err_16131, GetUniqueObjectID());
		}// end if

		MatroxCode = SVMatroxOcrInterface::Get( m_milFontID, SVOcrStringSize, m_lFontStringLength );
		if( m_lFontStringLength < 1 )
		{
			SvStl::MessageMgrStd MesMan( SvStl::LogOnly );
			MesMan.setMessage( SVMSG_SVO_103_REPLACE_ERROR_TRAP, SvStl::Tid_UnexpectedError, SvStl::SourceFileParams(StdMessageParams), SvStl::Err_16132, GetUniqueObjectID());
		}// end if

		MatroxCode = SVMatroxOcrInterface::Get( m_milFontID, SVOcrStringSizeMax, m_lFontStringLengthMax );
		if( m_lFontStringLengthMax < 1 )
		{
			SvStl::MessageMgrStd MesMan( SvStl::LogOnly );
			MesMan.setMessage( SVMSG_SVO_103_REPLACE_ERROR_TRAP, SvStl::Tid_UnexpectedError, SvStl::SourceFileParams(StdMessageParams), SvStl::Err_16133, GetUniqueObjectID());
		}// end if

		if( m_lFontStringLength == SVValueAny )
		{
			m_lFontStringLength = m_lFontStringLengthMax;
		}// end if

		if ( 0 < m_lMatchStringLength && m_lMatchStringLength != m_lFontStringLength && m_lMatchStringLength <= m_lFontStringLengthMax )
		{
			MatroxCode = SVMatroxOcrInterface::Set( m_milFontID, SVOcrStringSize, m_lMatchStringLength);

			MatroxCode = SVMatroxOcrInterface::Get( m_milFontID, SVOcrStringSize, m_lFontStringLength );
			if( m_lFontStringLength < 1 )
			{
				SvStl::MessageMgrStd MesMan( SvStl::LogOnly );
				MesMan.setMessage( SVMSG_SVO_103_REPLACE_ERROR_TRAP, SvStl::Tid_UnexpectedError, SvStl::SourceFileParams(StdMessageParams), SvStl::Err_16134, GetUniqueObjectID());
			}// end if
		}

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
		 if( bUseFile )
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
				 MesMan.setMessage( SVMSG_SVO_103_REPLACE_ERROR_TRAP, SvStl::Tid_UnexpectedError, SvStl::SourceFileParams(StdMessageParams), SvStl::Err_16194, GetUniqueObjectID());
				 LastError = -SvStl::Err_16194;
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
				 MesMan.setMessage( SVMSG_SVO_103_REPLACE_ERROR_TRAP, SvStl::Tid_UnexpectedError, SvStl::SourceFileParams(StdMessageParams), SvStl::Err_16195, GetUniqueObjectID());
				 LastError = SvStl::Err_16195;
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
				 MesMan.setMessage( SVMSG_SVO_103_REPLACE_ERROR_TRAP, SvStl::Tid_UnexpectedError, SvStl::SourceFileParams(StdMessageParams), SvStl::Err_16196, GetUniqueObjectID());
				 LastError = SvStl::Err_16196;
				 break;
			 }
			 
			 // Copy entire file into the buffer.
			 m_lTotalBytes = matchFile.Read( m_pBuffer, (UINT)dwByteNumber );
			 matchFile.Close();
			 m_pBuffer[m_lTotalBytes] = 0;
			 
			 if (0 != LastError)
			 {
				 m_svoMatchString.SetValue(SVString());
			 }
			 
			 if ( dwByteNumber > 0 )
			 {
				 if( !BuildHashTable( m_pBuffer ) )
				 {
					 m_nTotalCount = 0;
					 SvStl::MessageMgrStd MesMan( SvStl::LogOnly );
					 MesMan.setMessage( SVMSG_SVO_103_REPLACE_ERROR_TRAP, SvStl::Tid_UnexpectedError, SvStl::SourceFileParams(StdMessageParams), SvStl::Err_16197, GetUniqueObjectID());
					 LastError = SvStl::Err_16197;
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
	   m_svoMatchString.SetValue(SVString());
	}

	return hrRet;
}

bool SVOCVAnalyzeResultClass::ResetObject(SvStl::MessageContainerVector *pErrorMessages)
{
	bool Result = SVResultClass::ResetObject(pErrorMessages);

	if ( !m_bHasLicenseError )
	{
		//
		// Read the match string from a possible match string file.
		//
		LoadMatchString();

		//
		// Regenerate the Font Model.
		//
		GenerateFontModel(); //Error code must not be checked. If this failed m_milFontID is empty and an error is created anyway

		if ( m_lMatchStringLength != m_lFontStringLength || m_lMatchStringLength > m_lFontStringLengthMax || m_milFontID.empty() )
		{
			Result = false;
			if (nullptr != pErrorMessages)
			{
				SvStl::MessageContainer Msg( SVMSG_SVO_92_GENERAL_ERROR, SvStl::Tid_InvalidData, SvStl::SourceFileParams(StdMessageParams), 0, GetUniqueObjectID() );
				pErrorMessages->push_back(Msg);
	}
		}
	}
	else
	{
		Result = false;
		if (nullptr != pErrorMessages)
		{
			SvStl::MessageContainer Msg( SVMSG_SVO_92_GENERAL_ERROR, SvStl::Tid_SVObserver_MatroxLicenseNotFound, SvStl::SourceFileParams(StdMessageParams), 0, GetUniqueObjectID() );
			pErrorMessages->push_back(Msg);
		}
	}
	return Result;
}

/////////////////////////////////////////////////////////////////////////////
//
//
//
//
bool SVOCVAnalyzeResultClass::onRun( SVRunStatusClass& rRunStatus, SvStl::MessageContainerVector *pErrorMessages )
{
	SVMatroxBuffer l_milImageID;
	BYTE* pMilBuffer = nullptr;
	BOOL bOperation;
	BOOL bUseFile;

	long l_lLength = 0;

	double dHigh( 0.0 );
	double dLow( 0.0 );
	double dAvg( 0.0 );
	double dSum( 0.0 );
	double dValidString( 0.0 );
	double dNbrString( 0.0 );
	SVString l_strLabel;
	SVMatroxDoubleArray l_adScores;
	SVMatroxDoubleArray l_adXCoords;
	SVMatroxDoubleArray l_adYCoords;
	SVMatroxDoubleArray l_adValidChars;
	double dScore( 0.0 );
	double dLength( 0.0 );
	double dCharBoxSizeX( 0.0 );
	double dCharBoxSizeY( 0.0 );

	SVString FoundString;
	SVString MatchString;
	 
	HRESULT MatroxCode(S_OK);
  
	bool bOk = __super::onRun( rRunStatus, pErrorMessages );

	if( bOk && !rRunStatus.IsDisabled() && !rRunStatus.IsDisabledByCondition() )
	{
		SVImageClass* pImage = getInputImage();
		if( nullptr == pImage )
		{
			SetInvalid();
			bOk = false;
			if (nullptr != pErrorMessages)
			{
				SvStl::MessageContainer Msg( SVMSG_SVO_92_GENERAL_ERROR, SvStl::Tid_ErrorGettingInputs, SvStl::SourceFileParams(StdMessageParams), 0, GetUniqueObjectID() );
				pErrorMessages->push_back(Msg);
			}
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

				if( nullptr == pMilBuffer )
				{
					//
					// No MIL image buffer handle..
					//
					if (nullptr != pErrorMessages)
					{
						SvStl::MessageContainer Msg( SVMSG_SVO_93_GENERAL_WARNING, SvStl::Tid_Error_NoMilHostBuffer, SvStl::SourceFileParams(StdMessageParams), SvStl::Err_10169, GetUniqueObjectID() );
						pErrorMessages->push_back(Msg);
					}
					bOk = false;
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
						if (nullptr != pErrorMessages)
						{
							SVStringVector msgList;
							msgList.push_back(SvUl_SF::Format(_T("%x"), imageTypeMil));
							SvStl::MessageContainer Msg( SVMSG_SVO_93_GENERAL_WARNING, SvStl::Tid_Error_MilImageTypeInvalid, msgList, SvStl::SourceFileParams(StdMessageParams), SvStl::Err_10170, GetUniqueObjectID() );
							pErrorMessages->push_back(Msg);
						}
						bOk = false;
					}
				}
			}
		}

		if( bOk )
		{
			//
			// Turn off mouse events for duration of this method.
			//
			if( !lock.Lock( 1 ) )
			{
				bOk = false;
				SetInvalid();
				if (nullptr != pErrorMessages)
				{
					SvStl::MessageContainer Msg( SVMSG_SVO_92_GENERAL_ERROR, SvStl::Tid_LockingFailed, SvStl::SourceFileParams(StdMessageParams), 0, GetUniqueObjectID() );
					pErrorMessages->push_back(Msg);
				}
			}
			else
			{
				long k( 0 );
				// Reset the 'found' string to nothing.
				m_svoFoundString.SetValue(SVString());
				m_lvoMatchLineNumber.SetValue(-1L);

				//
				// Preset for failure
				//
				m_Failed.SetValue(BOOL(true));
				m_Warned.SetValue(BOOL(true));
				m_Passed.SetValue(BOOL(false));

				m_dvoHighestMatchScore.SetValue(0.0);
				m_dvoLowestMatchScore.SetValue(0.0);
				m_dvoAverageMatchScore.SetValue(0.0);

				//
				// Get some settings.
				//
				m_bvoPerformOCR.GetValue( bOperation );

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
							MatroxCode = -12395;
							break;
						}

						if( bOperation ) // read
						{
							m_milFontID.m_bVerify = false;
							strFunctionName = _T("MocrReadString"); iProgramCode = -12383;
							MatroxCode = SVMatroxOcrInterface::Execute( m_milResultID , m_milFontID, l_milImageID );

							if ((MatroxCode & 0xc0000000) != 0)
							{
								MatroxCode = -12396;
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
							MatroxCode = SVMatroxOcrInterface::Execute( m_milResultID , 
								                                    m_milFontID, 
																	l_milImageID );
							if ((MatroxCode & 0xc0000000) != 0)
							{
								MatroxCode = -12397;
								break;
							}
						}// end else //verify

						//
						// Process the OCV chars returned from MocrReadString();
						//
						dValidString = 0.0f;
						dNbrString = 0.0f;
						dLength = 0.0f;
						strFunctionName = _T("MocrGetResult");


						iProgramCode = -12386;


						// Before we try to get any additional results we MUST make sure that
						// we have a string to get results from. Otherwise Matrox gets flaky.
						MatroxCode = SVMatroxOcrInterface::GetResult(  m_milResultID, 
							                                      SVOcrNbrString, 
																  dNbrString );
						if( dNbrString < 1 )
						{
							// There is no reason to go on at this point 
							// and in fact going on will cause trouble.
							break;
						}

						MatroxCode = SVMatroxOcrInterface::GetResult( m_milResultID, 
							                                      SVOcrStringValidFlag, 
																  dValidString );
						if ((MatroxCode & ~0xcf000000) == 23002)
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
							MatroxCode = 0;

//-							It is assumed that l_lStringFound still equals 
//-							false.
							break;
						}

						if ((MatroxCode & 0xc0000000) != 0)
						{
							break;
						}

//-						A string was found!

						iProgramCode = -12385;
						MatroxCode = SVMatroxOcrInterface::GetResult( m_milResultID, 
																  SVOcrResultStringSize, 
																  dLength );


						if ((MatroxCode & 0xc0000000) != 0)
						{
							MatroxCode = -12398;
							break;
						}

						l_lLength = (long) dLength;
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
						MatroxCode = SVMatroxOcrInterface::GetResult( m_milResultID, SVOcrStringScore, dScore);
						if ((MatroxCode & 0xc0000000) != 0)
						{
							MatroxCode = -12399;
							break;
						}

						iProgramCode = -12388;
						MatroxCode = SVMatroxOcrInterface::GetResult( m_milResultID, SVOcrString, l_strLabel);
						if ((MatroxCode & 0xc0000000) != 0)
						{
							MatroxCode = -12400;
							break;
						}

						iProgramCode = -12389;
						MatroxCode = SVMatroxOcrInterface::GetResult( m_milResultID, SVOcrCharScore, l_adScores);

						if ((MatroxCode & 0xc0000000) != 0)
						{
							MatroxCode = -12401;
							break;
						}

						iProgramCode = -12390;
						MatroxCode = SVMatroxOcrInterface::GetResult( m_milResultID, SVOcrCharPosX, l_adXCoords);

						if ((MatroxCode & 0xc0000000) != 0)
						{
							MatroxCode = -12402;
							break;
						}

						iProgramCode = -12391;
						MatroxCode = SVMatroxOcrInterface::GetResult( m_milResultID, SVOcrCharPosY, l_adYCoords);

						if ((MatroxCode & 0xc0000000) != 0)
						{
							MatroxCode = -12403;
							break;
						}

						iProgramCode = -12392;
						MatroxCode = SVMatroxOcrInterface::GetResult( m_milResultID, SVOcrCharValidFlag, l_adValidChars);
						if ((MatroxCode & 0xc0000000) != 0)
						{
							MatroxCode = -12404;
							break;
						}

						strFunctionName = _T("MocrInquire");
						iProgramCode = -12393;
						MatroxCode = SVMatroxOcrInterface::Get( m_milFontID, SVCharCellSizeX, dCharBoxSizeX);
						if ((MatroxCode & 0xc0000000) != 0)
						{
							MatroxCode = -12405;
							break;
						}

						strFunctionName = _T("MocrInquire");
						iProgramCode = -12394;
						MatroxCode = SVMatroxOcrInterface::Get( m_milFontID, SVCharCellSizeY, dCharBoxSizeY);
						if ((MatroxCode & 0xc0000000) != 0)
						{
							MatroxCode = -12406;
							break;
						}

						break;
					} //while (1)

					if( l_lLength == 0 )
					{
						l_strLabel.empty();
					}

					dHigh = 0.0f;
					dLow  = 100.0f;
					dAvg  = 0.0f;
					dSum	= 0.0f;
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
						rRunStatus.SetInvalid();
						rRunStatus.SetFailed();
					}

					if (nullptr != pErrorMessages)
					{
						SVStringVector msgList;
						msgList.push_back(strFunctionName);
						SvStl::MessageContainer Msg( SVMSG_SVO_30_EXCEPTION_IN_MIL, SvStl::Tid_Default, msgList, SvStl::SourceFileParams(StdMessageParams), iProgramCode, GetUniqueObjectID() );
						pErrorMessages->push_back(Msg);
					}
					bOk = false;
					l_lLength = 0;
				}// end catch


				for( k = 0; k < l_lLength; k++ ) // For critical error cases 
					                             // the length must be 0 at 
												 // this point.
				{
					SVOCVCharacterResultClass *pResult = arrayOCVCharacterResults.GetAt( k );

					pResult->m_cvoLabelValue.SetValue(l_strLabel[k]);

					pResult->m_dvoOverlayLeft.SetValue(l_adXCoords[k] - ( dCharBoxSizeX / 2.0 ));
					pResult->m_dvoOverlayTop.SetValue(l_adYCoords[k] - ( dCharBoxSizeY / 2.0 ));
					pResult->m_dvoOverlayWidth.SetValue(dCharBoxSizeX);
					pResult->m_dvoOverlayHeight.SetValue(dCharBoxSizeY);

					pResult->m_dvoMatchScore.SetValue(l_adScores[k]);

					if (l_adScores[k] > dHigh)
					{
						dHigh = l_adScores[k];
					}

					if (l_adScores[k] < dLow)
					{
						dLow = l_adScores[k];
					}

					dSum += l_adScores[k];

				}// end for

//-				Set unused indexes.
				for( long l = l_lLength; l < OCV_MAX_RESULTS; l++ )
				{
					SVOCVCharacterResultClass *pResult = arrayOCVCharacterResults.GetAt( l );

					pResult->m_cvoLabelValue.SetValue(0L);

					pResult->m_dvoOverlayLeft.SetValue(0.0);
					pResult->m_dvoOverlayTop.SetValue(0.0);
					pResult->m_dvoOverlayWidth.SetValue(0.0);
					pResult->m_dvoOverlayHeight.SetValue(0.0);

					pResult->m_dvoMatchScore.SetValue(-1.0);
				}// end for

				if( l_lLength )
				{
					FoundString = l_strLabel.c_str();
					dAvg = dSum / l_lLength;
				}// end if
				else
				{
					FoundString = _T( "" );
					bOk = true;
				}// end else

				// Determine MIL pass/fail status 
				bool bStringPassed = dValidString == SVValueTrue;
				bool bCharsPassed = true;
				for( k = 0; k < l_lLength; k++ )
				{
					bCharsPassed &= l_adValidChars[k] == SVValueTrue;
				}// end for

				// Cleanup results

				m_dvoHighestMatchScore.SetValue(dHigh);
				m_dvoLowestMatchScore.SetValue(dLow);
				m_dvoAverageMatchScore.SetValue(dAvg);

				//
				// Copy OCV result to a storage array element.
				//
				m_svoFoundString.SetValue(FoundString);

				m_svoMatchString.GetValue(MatchString);

				//
				// Is result == matchstring?
				//
				int nResultValue = 0;
				if( bOperation )
				{
					if( m_nTotalCount > 1 )
					{
						long lStringMatch;
						if( ( lStringMatch = CheckStringInTable( FoundString ) ) == -1L )
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

						m_lvoMatchLineNumber.SetValue(lStringMatch);
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
								m_lvoMatchLineNumber.SetValue(1L);
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
						m_Failed.SetValue(BOOL(false));
						m_Warned.SetValue(BOOL(false));
						m_Passed.SetValue(BOOL(true));

						rRunStatus.SetPassed();

						break;
					}
					case 2: // Warned
					{
						m_Failed.SetValue(BOOL(false));
						m_Warned.SetValue(BOOL(true));

						rRunStatus.SetWarned();

						break;
					}
					default: // Failed 
					{
						rRunStatus.SetFailed();

						break;
					}

				}// end switch

			}// end else

			lock.Unlock();
		}// end if

	}// end if

	if (MatroxCode & SV_ARC_ERROR)
	{
		assert (0);
		SetInvalid();
		rRunStatus.SetInvalid();

		SVStringVector msgList;
		msgList.push_back(_T("SVOCVAnalyzeResultClass::onRun"));

		if (nullptr != pErrorMessages)
		{
			SvStl::MessageContainer Msg( static_cast<DWORD> (MatroxCode), SvStl::Tid_ErrorIn, msgList, SvStl::SourceFileParams(StdMessageParams), 0, GetUniqueObjectID() );
			pErrorMessages->push_back(Msg);
		}
		bOk = false;
	}

	return bOk;
}

bool SVOCVAnalyzeResultClass::BuildHashTable( char *pBuffer )
{
	long lBufIndex = 0;
	bool  bRet = true;
	
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
		{
			m_lLowValue = m_lHighValue = lIndexValue;
		}
		else if(lIndexValue < m_lLowValue)
		{
			m_lLowValue = lIndexValue; 
		}
		else if(lIndexValue > m_lHighValue)
		{
			m_lHighValue = lIndexValue; 
		}
	}
	
	// Allocate a table that is 10 times the size of m_nTotalCount
	while(1)
	{
		if(nullptr != m_pIndexTable)
		{
			delete m_pIndexTable;
			m_pIndexTable = nullptr;
		}
		// If there is only one entry in the file, make it work the old way.   
		if(m_nTotalCount == 1)
		{
			m_svoMatchString.SetValue(SVString(pBuffer));
			m_pIndexTable = nullptr;
			break; // No need to create hash table
		}
		
		m_pIndexTable = new short[m_nTotalCount * 10];
		if( nullptr == m_pIndexTable)
		{
			bRet = false;
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
long SVOCVAnalyzeResultClass::CheckStringInTable(const SVString& rMatchString)
{
	long Result( -1L );
	
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
		int nActualIndex = static_cast<int> ((((double)(lIndexValue - m_lLowValue)) / m_dFactor) * 9.5);
		// Check whether the string at this location is the matchString.
		
		while(m_pIndexTable[nActualIndex] != 0) 
		{
			TCHAR *pData = m_pDataArr[m_pIndexTable[nActualIndex] - 1];
			
			if( rMatchString == pData )
			{
				Result = static_cast<long> (m_pIndexTable[nActualIndex]);
				break;
			}
			nActualIndex++;
			
			// if we reach the end of the hash table, start from the begining( starting from 3rd position) 
			if(nActualIndex >= m_nTotalCount * 10 )
			{
				nActualIndex = 2;
			}
		}
	}
	return Result;
} 

HRESULT SVOCVAnalyzeResultClass::onCollectOverlays(SVImageClass* p_pImage, SVExtentMultiLineStructVector& p_rMultiLineArray )
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

				double Value;
				pResult->m_dvoOverlayLeft.GetValue( Value );
				lLeft = static_cast<long> (Value);
				pResult->m_dvoOverlayTop.GetValue( Value );
				lTop = static_cast<long> (Value);
				pResult->m_dvoOverlayWidth.GetValue( Value );
				lWidth = static_cast<long> (Value);
				pResult->m_dvoOverlayHeight.GetValue( Value );
				lHeight = static_cast<long> (Value);

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

