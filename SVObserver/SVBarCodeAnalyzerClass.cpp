//******************************************************************************
//* COPYRIGHT (c) 2003 by SVResearch, Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVBarCodeAnalyzerClass
//* .File Name       : $Workfile:   SVBarCodeAnalyzerClass.cpp  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.11  $
//* .Check In Date   : $Date:   19 Dec 2014 03:59:30  $
//******************************************************************************

#pragma region Includes
#include "stdafx.h"
//Moved to precompiled header: #include <algorithm>
#include "SVBarCodeAnalyzerClass.h"

#include "SVImageLibrary/SVImageBufferHandleImage.h"
#include "SVOMFCLibrary/SVOINIClass.h"

#include "SVBarCodeResult.h"   
#include "SVBarCodeProperties.h"
#include "SVBarCodeBuffer.h"
#include "SVBarCodeResult.h"
#include "SVImageProcessingClass.h"
#include "SVImageClass.h"
#include "SVInspectionProcess.h"
#include "SVOLicenseManager/SVOLicenseManager.h"
#include "TextDefinesSvO.h"
#include "SVStatusLibrary/MessageManagerResource.h"
#include "SVStatusLibrary/GlobalPath.h"
#pragma endregion Includes

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

SV_IMPLEMENT_CLASS (SVBarCodeAnalyzerClass, SVBarCodeAnalyzerClassGuid);

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

SVBarCodeAnalyzerClass::SVBarCodeAnalyzerClass (BOOL BCreateDefaultTaskList, SVObjectClass* POwner, int StringResourceID)
	: SVImageAnalyzerClass (BCreateDefaultTaskList, POwner, StringResourceID)
{
	if ( TheSVOLicenseManager().HasMatroxIdentificationLicense() )
	{
		m_bHasLicenseError = false;
	}
	else
	{
		m_bHasLicenseError = true;
	}
	init ();

}

void SVBarCodeAnalyzerClass::init()
{
	
	m_outObjectInfo.ObjectTypeInfo.SubType = SVBarCodeAnalyzerObjectType;
	
	// Register Embedded Objects
	RegisterEmbeddedObject (&msv_szBarCodeValue, SVBarCodeObjectGuid, IDS_OBJECTNAME_BARCODEVALUE, false, SVResetItemNone);
	RegisterEmbeddedObject (&msv_szRegExpressionValue, SVRegExpressionObjectGuid, IDS_OBJECTNAME_CONTROLVALUE, false, SVResetItemOwner);
	RegisterEmbeddedObject (&msv_lBarCodeType, SVBCTypeObjectGuid, IDS_OBJECTNAME_BARCODETYPE, false, SVResetItemOwner);
	RegisterEmbeddedObject (&msv_dOrientation, SVBCOrientationObjectGuid, IDS_OBJECTNAME_ORIENTATION, false, SVResetItemOwner);
	RegisterEmbeddedObject (&msv_dSkewNegative, SVBCSkewNegativeObjectGuid, IDS_OBJECTNAME_SKEWNEGATIVE, false, SVResetItemOwner);
	RegisterEmbeddedObject (&msv_dSkewPositive, SVBCSkewPositiveObjectGuid, IDS_OBJECTNAME_SKEWPOSITIVE, false, SVResetItemOwner);
	RegisterEmbeddedObject (&msv_dCellMinSize, SVBCCellMinSizeObjectGuid, IDS_OBJECTNAME_CELLMINSIZE, false, SVResetItemOwner);
	RegisterEmbeddedObject (&msv_dCellMaxSize, SVBCCellMaxSizeObjectGuid, IDS_OBJECTNAME_CELLMAXSIZE, false, SVResetItemOwner);
	RegisterEmbeddedObject (&msv_dCellNumberX, SVBCCellNumberXObjectGuid, IDS_OBJECTNAME_CELLNUMBERX, false, SVResetItemOwner);
	RegisterEmbeddedObject (&msv_dCellNumberY, SVBCCellNumberYObjectGuid, IDS_OBJECTNAME_CELLNUMBERY, false, SVResetItemOwner);
	RegisterEmbeddedObject (&msv_dSpeed, SVBCSpeedObjectGuid, IDS_OBJECTNAME_SPEED, false, SVResetItemOwner);
	RegisterEmbeddedObject (&msv_dThreshold, SVBCThresholdObjectGuid, IDS_OBJECTNAME_THRESHOLD, false, SVResetItemOwner);
	RegisterEmbeddedObject (&msv_dStringSize, SVBCStringSizeObjectGuid, IDS_OBJECTNAME_STRINGSIZE, false, SVResetItemOwner);
	RegisterEmbeddedObject (&msv_dErrorCorrection, SVBCErrorCorrectionObjectGuid, IDS_OBJECTNAME_ERRORCORRECTION, false, SVResetItemOwner);
	RegisterEmbeddedObject (&msv_dEncoding, SVBCEncodingObjectGuid, IDS_OBJECTNAME_ENCODING, false, SVResetItemOwner);
	RegisterEmbeddedObject (&msv_dForegroundColor, SVBCForegroundColorObjectGuid, IDS_OBJECTNAME_FOREGROUNDCOLOR, false, SVResetItemOwner);
	RegisterEmbeddedObject (&msv_bSaveStringInFile, SVBCSaveStringInFileObjectGuid, IDS_OBJECTNAME_SAVESTRINGINFILE, false, SVResetItemOwner);
	RegisterEmbeddedObject (&msv_szStringFileName, SVBCStringFileNameObjectGuid, IDS_OBJECTNAME_STRINGFILENAME, false, SVResetItemOwner);
	RegisterEmbeddedObject (&m_bWarnOnFailedRead, SVBCWarnOnFailedReadObjectGuid, IDS_OBJECTNAME_WARNONFAILEDREAD, false, SVResetItemOwner);
	RegisterEmbeddedObject (&msv_lBarcodeTimeout, SVBCTimeoutGuid, IDS_OBJECTNAME_BARCODETIMEOUT, false, SVResetItemOwner);
	// IDS_OBJECTNAME_BARCODEUNEVENGRID
	RegisterEmbeddedObject (&msv_bUnEvenGrid, SVBCUnevenGridGuid,IDS_OBJECTNAME_BARCODEUNEVENGRID, false, SVResetItemOwner);

	// To support special DMCs May 2008.
	RegisterEmbeddedObject (&msv_RawData, SVBCRawDataGuid, IDS_OBJECTNAME_BARCODERAWDATA, false, SVResetItemOwner);
	RegisterEmbeddedObject (&msv_eStringFormat, SVBCStringFormatGuid, IDS_OBJECTNAME_BARCODESTRINGFORMAT, false, SVResetItemOwner);
	RegisterEmbeddedObject (&msv_lThresholdType, SVBCThresholdTypeGuid, IDS_OBJECTNAME_BARCODETHRESHOLDTYPE, false, SVResetItemOwner);

	// Set default values
	msv_szBarCodeValue.SetDefaultValue (_T(""), TRUE);
	msv_szRegExpressionValue.SetDefaultValue (_T(""), TRUE);
	msv_lBarCodeType.SetDefaultValue(SVDataMatrix, TRUE);
	msv_dOrientation.SetDefaultValue(SVValueDefault, TRUE);
	msv_dSkewNegative.SetDefaultValue(SVValueDefault, TRUE);
	msv_dSkewPositive.SetDefaultValue(SVValueDefault, TRUE);
	msv_dCellMinSize.SetDefaultValue(SVValueDefault, TRUE);
	msv_dCellMaxSize.SetDefaultValue(SVValueDefault, TRUE);
	msv_dCellNumberX.SetDefaultValue(SVValueAny, TRUE);
	msv_dCellNumberY.SetDefaultValue(SVValueAny, TRUE);
	msv_dSpeed.SetDefaultValue(SVValueMedium, TRUE);
	msv_dThreshold.SetDefaultValue(SVValueDefault, TRUE);
	msv_dStringSize.SetDefaultValue(SVValueDefault, TRUE);
	msv_dErrorCorrection.SetDefaultValue(SVValueAny, TRUE);
	msv_dEncoding.SetDefaultValue(SVValueAny, TRUE);
	msv_dForegroundColor.SetDefaultValue(SVValueForegroundBlack, TRUE);
	msv_bSaveStringInFile.SetDefaultValue(FALSE, TRUE);
	msv_szStringFileName.SetDefaultValue(_T("BCRegExp.txt"), TRUE);
	m_bWarnOnFailedRead.SetDefaultValue( FALSE, TRUE);

	msv_bUnEvenGrid.SetDefaultValue( FALSE, TRUE);

	// To support special DMCs May 2008.
	// Use ~ to simulate non printable characters...
	CString l_strFormatEnum;
	l_strFormatEnum.Format(_T("Remove GS1 Control Characters=%d,Translate GS1 Control Characters=%d,Replace GS1 Control Characters=%d"),SVBCStringFormatRemoveCharacters,SVBCStringFormatTranslateCharacters,SVBCStringFormatReplaceCharacters );
	msv_eStringFormat.SetEnumTypes( l_strFormatEnum );
	msv_eStringFormat.SetDefaultValue( SVBCStringFormatRemoveCharacters, true );
	msv_lThresholdType.SetDefaultValue( 0, true ); // Default Normal thresholding
	msv_RawData.SetArraySize(256);

	//set MIL Timeout default to be what is in the INI file...
	SvOml::SVOINIClass l_SvimIni(SvStl::GlobalPath::Inst().GetSVIMIniPath());

	int l_mTimeout = l_SvimIni.GetValueInt(_T("SVIM Information"), _T("MILBarcodeTimeout"),20);

	msv_lBarcodeTimeout.SetDefaultValue(l_mTimeout,TRUE);

	// Set default inputs and outputs
	addDefaultInputObjects();
	
	// Instantiate Children
	SVBarCodeResultClass *pAnalyzerResult = new SVBarCodeResultClass (TRUE, this, IDS_CLASSNAME_SVBARCODEANALYZERESULT);
	
	if (pAnalyzerResult)
		Add (pAnalyzerResult);

}

BOOL SVBarCodeAnalyzerClass::InitMil ()
{
	SVImageClass *pSVImage;
	double dParm;

	if ( !m_MilCodeId.empty() )
		CloseMil();
	
	if (pSVImage = getInputImage ())
	{
		// &&&
		SVBarCodeBufferInfoClass svData;

		svData.HBuffer.milBarCode = m_MilCodeId;

		long l_lTmpType;
		msv_lBarCodeType.GetValue( l_lTmpType );
		svData.m_lCodeType = static_cast<SVBarCodeTypesEnum>( l_lTmpType );
		if ( S_OK == SVImageProcessingClass::Instance().CreateBarCodeBuffer( &svData ) )
		{
			
			SVMatroxBarCodeInterface::SVStatusCode l_Code;

			m_MilCodeId = svData.HBuffer.milBarCode;

			// get M_TIMEOUT value from Registry
			//int l_mTimeout = AfxGetApp()->GetProfileInt(_T( "Settings" ),_T( "MILTimeout" ), 2000 );

			long l_mTimeout;
			msv_lBarcodeTimeout.GetValue(l_mTimeout);
			double dL1 = (double) l_mTimeout;

			l_Code = SVMatroxBarCodeInterface::Set( m_MilCodeId, SVBCCodeTimeout, dL1 );
			msv_dOrientation.GetValue (dParm);
			l_Code = SVMatroxBarCodeInterface::Set( m_MilCodeId, SVBCSearchAngle, dParm );

			msv_dSkewNegative.GetValue (dParm);
			l_Code = SVMatroxBarCodeInterface::Set( m_MilCodeId, SVBCSearchAngleDeltaNeg, dParm);

			msv_dSkewPositive.GetValue (dParm);
			l_Code = SVMatroxBarCodeInterface::Set( m_MilCodeId, SVBCSearchAngleDeltaPos, dParm);

			msv_dCellMinSize.GetValue (dParm);
			l_Code = SVMatroxBarCodeInterface::Set( m_MilCodeId, SVBCCellSizeMin, dParm);

			msv_dCellMaxSize.GetValue (dParm);
			l_Code = SVMatroxBarCodeInterface::Set( m_MilCodeId, SVBCCellSizeMax, dParm);

			msv_dCellNumberX.GetValue (dParm);
			l_Code = SVMatroxBarCodeInterface::Set( m_MilCodeId, SVBCCellNumberX, dParm);

			msv_dCellNumberY.GetValue (dParm);
			l_Code = SVMatroxBarCodeInterface::Set( m_MilCodeId, SVBCCellNumberY, dParm);

			msv_dSpeed.GetValue (dParm);
			l_Code = SVMatroxBarCodeInterface::Set( m_MilCodeId, SVBCSpeed, dParm);

			// Threshold
			long l_lThresholdType ;
			msv_lThresholdType.GetValue( l_lThresholdType );
			switch( l_lThresholdType )
			{
				case 0:
				{
					msv_dThreshold.GetValue (dParm);
					l_Code = SVMatroxBarCodeInterface::Set( m_MilCodeId, SVBCThreshold, dParm);
					break;
				}
				case 1:	// Auto Threshold
				{
					dParm = 0;
					l_Code = SVMatroxBarCodeInterface::Set( m_MilCodeId, SVBCAutoThreshold, dParm );
					break;
				}
				case 2:	// Adaptive Threshold
				{
					dParm = 0;
					l_Code = SVMatroxBarCodeInterface::Set( m_MilCodeId, SVBCAdaptiveThreshold, dParm );
					break;
				}
			}

			msv_dStringSize.GetValue (dParm);
			if ( dParm == (double) SVValueDefault ) // M_DEFAULT )
			{
				dParm = (double) SVValueAny;		// M_ANY;
			}
			l_Code = SVMatroxBarCodeInterface::Set( m_MilCodeId, SVBCBarcodeStringSize, dParm);

			msv_dEncoding.GetValue (dParm);
			l_Code = SVMatroxBarCodeInterface::Set( m_MilCodeId, SVBCEncoding, dParm);

			msv_dErrorCorrection.GetValue (dParm);
			l_Code = SVMatroxBarCodeInterface::Set( m_MilCodeId, SVBCErrorCorrection, dParm);

			msv_dForegroundColor.GetValue (dParm);
			l_Code = SVMatroxBarCodeInterface::Set( m_MilCodeId, SVBCForeGroundValue, dParm);

			BOOL bUnevenGrid;
			msv_bUnEvenGrid.GetValue(bUnevenGrid);
			l_Code = SVMatroxBarCodeInterface::Set( m_MilCodeId, SVBCUnEvenGrid, (long)bUnevenGrid );
			return TRUE;
		}
		else
		{
			// McodeAlloc failed
			return FALSE;
		}
	}
	else
	{
		// getInputImage failed
		return FALSE;
	}
}

void SVBarCodeAnalyzerClass::CloseMil ()
{
	if ( m_bHasLicenseError )
	{
		return;
	}
	SVMatroxBarCodeInterface::Destroy( m_MilCodeId );
}

BOOL SVBarCodeAnalyzerClass::CreateObject (SVObjectLevelCreateStruct* pCreateStructure)
{
	BOOL bOk = FALSE;


	if ( !m_bHasLicenseError )
	{
		if (SVImageAnalyzerClass::CreateObject (pCreateStructure))
		{

			if (InitMil ())
			{
				LoadRegExpression();

				bOk = TRUE;
			}
			else
			{
				// InitMil
				SVImageAnalyzerClass::CloseObject ();
			}
		}
	}
	else
	{
		bOk = TRUE;
	}

	// *** Set/Reset Printable Attributes ***
	msv_szBarCodeValue.ObjectAttributesAllowedRef() &= ~SV_PRINTABLE;
	msv_szRegExpressionValue.ObjectAttributesAllowedRef() |= SV_PRINTABLE;
	msv_lBarCodeType.ObjectAttributesAllowedRef() |= SV_PRINTABLE;
	msv_dOrientation.ObjectAttributesAllowedRef() |= SV_PRINTABLE;
	msv_dSkewNegative.ObjectAttributesAllowedRef() |= SV_PRINTABLE;
	msv_dSkewPositive.ObjectAttributesAllowedRef() |= SV_PRINTABLE;
	msv_dCellMinSize.ObjectAttributesAllowedRef() |= SV_PRINTABLE;
	msv_dCellMaxSize.ObjectAttributesAllowedRef() |= SV_PRINTABLE;
	msv_dCellNumberX.ObjectAttributesAllowedRef() |= SV_PRINTABLE;
	msv_dCellNumberY.ObjectAttributesAllowedRef() |= SV_PRINTABLE;
	msv_dSpeed.ObjectAttributesAllowedRef() |= SV_PRINTABLE | SV_REMOTELY_SETABLE;
	msv_dThreshold.ObjectAttributesAllowedRef() |= SV_PRINTABLE;
	msv_dStringSize.ObjectAttributesAllowedRef() |= SV_PRINTABLE;
	msv_dErrorCorrection.ObjectAttributesAllowedRef() |= SV_PRINTABLE;
	msv_dEncoding.ObjectAttributesAllowedRef() |= SV_PRINTABLE;
	msv_dForegroundColor.ObjectAttributesAllowedRef() |= SV_PRINTABLE;
	msv_bSaveStringInFile.ObjectAttributesAllowedRef() |= SV_PRINTABLE;
	msv_szStringFileName.ObjectAttributesAllowedRef() |= SV_PRINTABLE;
	m_bWarnOnFailedRead.ObjectAttributesAllowedRef() |= SV_PRINTABLE | SV_REMOTELY_SETABLE;
	msv_lBarcodeTimeout.ObjectAttributesAllowedRef() |= SV_PRINTABLE | SV_REMOTELY_SETABLE;
	msv_RawData.ObjectAttributesAllowedRef() &= ~SV_PRINTABLE;
	m_isCreated = bOk;

	if ( m_bHasLicenseError )
	{
		TheSVOLicenseManager().AddLicenseErrorToList(GetUniqueObjectID());
	}

	return bOk;
}

SVBarCodeAnalyzerClass::~SVBarCodeAnalyzerClass ()
{
	if ( m_bHasLicenseError )
	{
		m_bHasLicenseError = false;
		TheSVOLicenseManager().RemoveLicenseErrorFromList(GetUniqueObjectID());
	}

	CloseObject();
}

BOOL SVBarCodeAnalyzerClass::CloseObject ()
{
	CloseMil ();
	return SVImageAnalyzerClass::CloseObject();
}

BOOL SVBarCodeAnalyzerClass::OnValidate ()
{
	BOOL bRetVal = SVImageAnalyzerClass::OnValidate();
	if( bRetVal )
	{
		if (!m_MilCodeId.empty())
		{
			isObjectValid.SetValue (1, TRUE);
			return TRUE;
		}
	}
	else
	{
		SetInvalid();
		bRetVal = FALSE;
	}

	if( m_bHasLicenseError )
	{
		bRetVal = FALSE;
	}

	return bRetVal;
}

SVResultClass* SVBarCodeAnalyzerClass::GetResultObject()
{
	SVResultClass* pAnalyzerResult = nullptr;

	// Find the result Object in Our List
	pAnalyzerResult = (SVResultClass *)GetAt(0);

	return( pAnalyzerResult );
}

bool SVBarCodeAnalyzerClass::CharIsControl( TCHAR p_Char )
{
	return p_Char < ' ' || p_Char > 126;
}

BOOL SVBarCodeAnalyzerClass::onRun (SVRunStatusClass &RRunStatus)
{
	long lMilResult;
	SVImageClass *pInputImage;
	CString szBarCodeValue;
	long cbBarCodeValue;

	if ( m_bHasLicenseError )
	{
		SetInvalid ();
		RRunStatus.SetInvalid ();
		return FALSE;
	}
	
	szBarCodeValue.Empty(); // clear previous value
	msv_szBarCodeValue.SetValue( RRunStatus.m_lResultDataIndex, szBarCodeValue );
	
	if (SVImageAnalyzerClass::onRun (RRunStatus))
	{
		SVSmartHandlePointer ImageHandle;
		
		// Used in Result Class to know whether to Fail or Warn.
		SVBarCodeResultClass* pResult = static_cast<SVBarCodeResultClass*>(GetResultObject());
		BOOL bWarnOnFailedRead;
		m_bWarnOnFailedRead.GetValue(bWarnOnFailedRead); // Add value from checkbox instead of true to warn on fail.
									// default value will be false.
		pResult->m_bFailedToRead = bWarnOnFailedRead != FALSE;// Preset flag to failed condition..

		pInputImage = getInputImage ();		
		if ( pInputImage->GetImageHandle( ImageHandle ) && !( ImageHandle.empty() ) )
		{
			try
			{
				
				SVMatroxBarCodeInterface::SVStatusCode l_Code;

				SVImageBufferHandleImage l_MilBuffer;
				ImageHandle->GetData( l_MilBuffer );

				SVMatroxBuffer ImageBufId = l_MilBuffer.GetBuffer();

				l_Code = SVMatroxBarCodeInterface::Execute( m_MilCodeId, ImageBufId );

				l_Code = SVMatroxBarCodeInterface::GetResult( m_MilCodeId, SVBCBarCodeStatus, lMilResult );
				double dScore = 0.0;

				SVString l_strBarCodeValue;

				switch ( lMilResult )
				{
					case SVBCStatusReadOK:
					{
						pResult->m_bFailedToRead = false;
						l_Code = SVMatroxBarCodeInterface::GetResult( m_MilCodeId, SVBCBarcodeStringSize, cbBarCodeValue );


						l_Code = SVMatroxBarCodeInterface::GetResult( m_MilCodeId, SVBCBarCodeString, l_strBarCodeValue );

						l_Code = SVMatroxBarCodeInterface::GetResult( m_MilCodeId, SVBCBarcodeScore, dScore );

						pResult->m_dReadScore.SetValue(RRunStatus.m_lResultDataIndex, dScore * 100);

						// To support special DMCs May 2008.
						// Copy string to Byte Value Object
						if( l_strBarCodeValue.size() > static_cast<size_t>(msv_RawData.GetArraySize()) )
						{
							msv_RawData.SetArraySize( static_cast< int >( l_strBarCodeValue.size() ) );
						}
						l_Code = msv_RawData.SetResultSize( RRunStatus.m_lResultDataIndex, static_cast< int >( l_strBarCodeValue.size() ) );
						msv_RawData.SetArrayValues( RRunStatus.m_lResultDataIndex, l_strBarCodeValue.begin(), l_strBarCodeValue.end() );

						// based on msv_eBarcodeStringFormat replace control characters...
						long l_lTmpFormat=SVBCStringFormatReplaceCharacters;
						msv_eStringFormat.GetValue( l_lTmpFormat );
						switch( l_lTmpFormat )
						{
							case SVBCStringFormatReplaceCharacters:
							{
								replace_if( l_strBarCodeValue.begin(),
											l_strBarCodeValue.end(),
											CharIsControl,
											_T('~'));
								break;
							}
							case SVBCStringFormatRemoveCharacters:
							{
								SVString l_strRemoved;
								l_strRemoved.resize( l_strBarCodeValue.size() + 1);
								int j = 0;
								for( size_t i = 0 ; i < l_strBarCodeValue.size(); i++ )
								{
									if( CharIsControl(l_strBarCodeValue[i]) )
									{
										continue;
									}
									l_strRemoved[j++]=l_strBarCodeValue[i];
								}
								l_strRemoved[j] = 0; // Teminate String...
								l_strRemoved.resize(j);
								l_strBarCodeValue = l_strRemoved;
								break;
							}
							case SVBCStringFormatTranslateCharacters:
							{
								SVString l_strTranslated;
								l_strTranslated.resize( l_strBarCodeValue.size()+256);
								int j = 0;
								for( size_t i = 0 ; i < l_strBarCodeValue.size(); i++ )
								{
									if( CharIsControl(l_strBarCodeValue[i]) )
									{
										CString l_strTmp;
										l_strTmp.Format(_T("%03d"),l_strBarCodeValue[i]);
										l_strTranslated[j++] = '\\';
										l_strTranslated[j++] = l_strTmp[0];
										l_strTranslated[j++] = l_strTmp[1];
										l_strTranslated[j++] = l_strTmp[2];
									}
									else if ( l_strBarCodeValue[i] == '\\' )
									{
										l_strTranslated[j++] = '\\';
										l_strTranslated[j++] = '\\';
									}
									else
									{
										l_strTranslated[j++]=l_strBarCodeValue[i];
									}
								}
								l_strTranslated[j] = 0; // Terminate String...
								l_strTranslated.resize(j);
								l_strBarCodeValue = l_strTranslated;
								break;
							}
							default:
							{
								break;
							}
						}
						msv_szBarCodeValue.SetValue( RRunStatus.m_lResultDataIndex, l_strBarCodeValue.c_str() );
						return TRUE;
						
						break;
					}
					case SVBCStatusCRCFailed:
					case SVBCStatusECCUnknown:
					case SVBCStatusENCUnknown:
					case SVBCStatusNotFound:
					default:
					{


						// Clear The Raw Data if no read.
						msv_RawData.SetResultSize( RRunStatus.m_lResultDataIndex, 0 );
						break;
					}
				}
				SetInvalid();
				if( bWarnOnFailedRead )
					RRunStatus.SetWarned();
				else
					RRunStatus.SetFailed();
				pResult->m_dReadScore.SetValue(RRunStatus.m_lResultDataIndex, dScore * 100);
				return TRUE;

			}// end try
			catch( ... )
			{
			}// end catch
		}
	}
	SetInvalid ();
	RRunStatus.SetInvalid ();
	return FALSE;
}


BOOL SVBarCodeAnalyzerClass::LoadRegExpression( BOOL DisplayErrorMessage )
{
	BOOL bSaveStringInFile;

	msv_bSaveStringInFile.GetValue(bSaveStringInFile);
	if (bSaveStringInFile)
	{
		TRY
		{
			CString szFileName;
			DWORD dwFileLen;
			CString szRegExp;
			LPTSTR lpRegExp;

			msv_szStringFileName.GetValue(szFileName);
			CFile fRegExp (szFileName, CFile::modeRead);

			dwFileLen = static_cast<DWORD>(fRegExp.GetLength());
			lpRegExp = szRegExp.GetBuffer(dwFileLen);
			fRegExp.Read(lpRegExp, dwFileLen);
			szRegExp.ReleaseBuffer(dwFileLen);
			msv_szRegExpressionValue.SetValue(1, szRegExp);
			fRegExp.Close();

			msv_szRegExpressionValue.GetValue(m_csRegExpressionValue);
		    msv_szStringFileName.GetValue(m_csStringFileName);

			return TRUE;
		}
		CATCH (CFileException, e)
		{
#if defined (TRACE_THEM_ALL) || defined (TRACE_FAILURE)
			TRACE( "Exception : File = %s -- Line = %d\n", _T(__FILE__), __LINE__ );
#endif
			m_errId = SvOi::Tid_BarCode_UnableToRead;

			if( DisplayErrorMessage )
			{
				SvStl::MessageMgrDisplayAndNotify Msg( SvStl::LogAndDisplay );
				Msg.setMessage( SVMSG_SVO_93_GENERAL_WARNING, m_errId, SvStl::SourceFileParams(StdMessageParams), SvOi::Err_10038 ); 
			}
			return FALSE;
		}
		END_CATCH
	}

  return TRUE;
}

BOOL SVBarCodeAnalyzerClass::SaveRegExpression( BOOL DisplayErrorMessage )
{
	BOOL bSaveStringInFile;
	
	msv_bSaveStringInFile.GetValue(bSaveStringInFile);
	if (bSaveStringInFile)
	{
		TRY
		{
			CString szFileName;
			DWORD dwFileLen;
			CString szRegExp;
			
			msv_szRegExpressionValue.GetValue(szRegExp);
			msv_szStringFileName.GetValue(szFileName);
			
			CFile fRegExp( szFileName, CFile::modeCreate | CFile::modeWrite | CFile::shareDenyNone );
			
			dwFileLen = szRegExp.GetLength();
			fRegExp.Write( szRegExp, dwFileLen );
			
			fRegExp.Close();

			msv_szRegExpressionValue.GetValue(m_csRegExpressionValue);
			msv_szStringFileName.GetValue(m_csStringFileName);

			return TRUE;
		}
		CATCH (CFileException, e)
		{
			m_errId = SvOi::Tid_BarCode_UnableToSave;

			if( DisplayErrorMessage )
			{
				SvStl::MessageMgrDisplayAndNotify Msg( SvStl::LogAndDisplay );
				Msg.setMessage( SVMSG_SVO_93_GENERAL_WARNING, m_errId, SvStl::SourceFileParams(StdMessageParams), SvOi::Err_10039 ); 
			}
			return FALSE;
		}
		END_CATCH
	}
	return TRUE;
}

DWORD_PTR SVBarCodeAnalyzerClass::processMessage(DWORD DwMessageID, DWORD_PTR DwMessageValue, DWORD_PTR DwMessageContext)
{
	DWORD_PTR dwResult = 0;

	switch (DwMessageID & SVM_PURE_MESSAGE)
	{
	case SVMSGID_RESET_ALL_OBJECTS :
		{
			HRESULT l_ResetStatus = ResetObject();
			if( S_OK != l_ResetStatus )
			{
				BOOL SilentReset = static_cast<BOOL> (DwMessageValue);

				if( !SilentReset && SvOi::Tid_Empty != m_errId )
				{
					SvStl::MessageMgrDisplayAndNotify Msg( SvStl::LogAndDisplay );
					Msg.setMessage( SVMSG_SVO_93_GENERAL_WARNING, m_errId, SvStl::SourceFileParams(StdMessageParams), SvOi::Err_10040 ); 
				}
				dwResult = SVMR_NO_SUCCESS;
			}
			else
			{
				dwResult = SVMR_SUCCESS;
			}
			break;
		}
	}
	return (dwResult | SVImageAnalyzerClass::processMessage(DwMessageID, DwMessageValue, DwMessageContext));
}

HRESULT SVBarCodeAnalyzerClass::ResetObject()
{
	HRESULT l_hrOk = SVImageAnalyzerClass::ResetObject();
	m_errId = SvOi::Tid_Empty;

	if ( !m_bHasLicenseError )
	{
		if ( InitMil() )
		{
			CString m_csTempRegExpressionValue;
			CString m_csTempStringFileName;

		if( ( S_OK == msv_szRegExpressionValue.GetValue( m_csTempRegExpressionValue )  &&
					0 != m_csTempRegExpressionValue.Compare( m_csRegExpressionValue )  ) ||
			( S_OK == msv_szStringFileName.GetValue( m_csTempStringFileName )  &&
					0 != m_csTempStringFileName.Compare( m_csStringFileName )  ) )
			{
				if( ! SaveRegExpression( FALSE ) )
				{
					l_hrOk = S_FALSE;
				}
			}

			if ( ! LoadRegExpression( FALSE ) )
			{
				l_hrOk = S_FALSE;
			}
		}
		else
		{
			l_hrOk = S_FALSE;
		}
	}

	return l_hrOk;
}

