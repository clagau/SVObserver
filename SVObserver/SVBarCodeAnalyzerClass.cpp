//******************************************************************************
//* COPYRIGHT (c) 2003 by SVResearch, Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVBarCodeAnalyzerClass
//* .File Name       : $Workfile:   SVBarCodeAnalyzerClass.cpp  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.9  $
//* .Check In Date   : $Date:   01 Feb 2014 10:16:34  $
//******************************************************************************

#include "stdafx.h"
#include <algorithm>
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
	
	outObjectInfo.ObjectTypeInfo.SubType = SVBarCodeAnalyzerObjectType;
	
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
	SVOINIClass l_SvimIni;

	l_SvimIni.SetFile(_T("C:\\SVObserver\\bin\\SVIM.INI"));

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
		if ( SVImageProcessingClass::Instance().CreateBarCodeBuffer( &svData ) == S_OK )
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
	isCreated = bOk;

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
	SVResultClass* pAnalyzerResult = NULL;

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

				SVMatroxString l_strBarCodeValue;

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
								SVMatroxString l_strRemoved;
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
								l_strRemoved[j] = 0; // NULL...
								l_strRemoved.resize(j);
								l_strBarCodeValue = l_strRemoved;
								break;
							}
							case SVBCStringFormatTranslateCharacters:
							{
								SVMatroxString l_strTranslated;
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
								l_strTranslated[j] = 0; // NULL...
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


BOOL SVBarCodeAnalyzerClass::LoadRegExpression()
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
			TRACE( "Exception : File = %s -- Line = %d\n", _T(__FILE__), __LINE__ );

			AfxMessageBox(_T("Bar Code Analyzer was unable to read\nthe match string from file. Check the\nanalyzer settings for proper match string."), MB_OK, 0);
			return FALSE;
		}
		END_CATCH
	}

  return TRUE;
}

BOOL SVBarCodeAnalyzerClass::SaveRegExpression()
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
			AfxMessageBox(_T("Bar Code Analyzer was unable to save\nthe match string to file. Check the\nanalyzer settings for proper match string."), MB_OK, 0);
			return FALSE;
		}
		END_CATCH
	}
	return TRUE;
}

LONG_PTR SVBarCodeAnalyzerClass::processMessage(DWORD DwMessageID, LONG_PTR DwMessageValue, LONG_PTR DwMessageContext)
{
	LONG_PTR dwResult = 0L;

	switch (DwMessageID & SVM_PURE_MESSAGE)
	{
	case SVMSGID_RESET_ALL_OBJECTS :
		{
			HRESULT l_ResetStatus = ResetObject();
			if( l_ResetStatus != S_OK )
			{
				ASSERT( SUCCEEDED( l_ResetStatus ) );

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

	if ( !m_bHasLicenseError )
	{
		if ( InitMil() )
		{
			CString m_csTempRegExpressionValue;
			CString m_csTempStringFileName;

		if( ( msv_szRegExpressionValue.GetValue( m_csTempRegExpressionValue ) == S_OK &&
					m_csTempRegExpressionValue.Compare( m_csRegExpressionValue ) != 0 ) ||
			( msv_szStringFileName.GetValue( m_csTempStringFileName ) == S_OK &&
					m_csTempStringFileName.Compare( m_csStringFileName ) != 0 ) )
			{
				if( ! SaveRegExpression() )
				{
					l_hrOk = S_FALSE;
				}
			}

			if ( ! LoadRegExpression() )
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

//******************************************************************************
//* LOG HISTORY:
//******************************************************************************
/*
$Log:   N:\PVCSarch65\ProjectFiles\archives\SVObserver_SRC\SVObserver\SVBarCodeAnalyzerClass.cpp_v  $
 * 
 *    Rev 1.9   01 Feb 2014 10:16:34   tbair
 * Project:  SVObserver
 * Change Request (SCR) nbr:  852
 * SCR Title:  Add Multiple Platform Support to SVObserver's Visual Studio Solution
 * Checked in by:  tBair;  Tom Bair
 * Change Description:  
 *   Changed sendmessage to use LONG_PTR instead of DWORD.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.8   01 Oct 2013 11:54:40   tbair
 * Project:  SVObserver
 * Change Request (SCR) nbr:  852
 * SCR Title:  Add Multiple Platform Support to SVObserver's Visual Studio Solution
 * Checked in by:  tBair;  Tom Bair
 * Change Description:  
 *   Add x64 platform.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.7   30 Jul 2013 11:47:48   tbair
 * Project:  SVObserver
 * Change Request (SCR) nbr:  838
 * SCR Title:  Remove Raw Data under the Bar Code Analyzer from the Configuration Print
 * Checked in by:  tBair;  Tom Bair
 * Change Description:  
 *   Fixed possible Index out of range exception.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.6   18 Jun 2013 19:22:28   bwalter
 * Project:  SVObserver
 * Change Request (SCR) nbr:  814
 * SCR Title:  Upgrade SVObserver to Compile Using Visual Studio 2010
 * Checked in by:  bWalter;  Ben Walter
 * Change Description:  
 *   Merged with svo_src label SVO 6.10 Beta 019.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.51   29 May 2013 09:19:18   ryoho
 * Project:  SVObserver
 * Change Request (SCR) nbr:  838
 * SCR Title:  Remove Raw Data under the Bar Code Analyzer from the Configuration Print
 * Checked in by:  rYoho;  Rob Yoho
 * Change Description:  
 *   changed the attributes for Raw Data so it will not show up in the configuration print
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.5   11 Jun 2013 15:25:54   bWalter
 * Project:  SVObserver
 * Change Request (SCR) nbr:  814
 * SCR Title:  Upgrade SVObserver to Compile Using Visual Studio 2010
 * Checked in by:  bWalter;  Ben Walter
 * Change Description:  
 *   Merged with svo_src label SVO 6.10 Beta 017.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.50   21 May 2013 08:31:08   tbair
 * Project:  SVObserver
 * Change Request (SCR) nbr:  796
 * SCR Title:  Add a License Manager that will handle the Matrox Licenses
 * Checked in by:  rYoho;  Rob Yoho
 * Change Description:  
 *   Fixed OnValidate merge error.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.4   15 May 2013 19:45:40   bWalter
 * Project:  SVObserver
 * Change Request (SCR) nbr:  814
 * SCR Title:  Upgrade SVObserver to Compile Using Visual Studio 2010
 * Checked in by:  bWalter;  Ben Walter
 * Change Description:  
 *   Merged with svo_src label SVO 6.10 Beta 016.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.49   14 May 2013 10:37:18   tbair
 * Project:  SVObserver
 * Change Request (SCR) nbr:  796
 * SCR Title:  Add a License Manager that will handle the Matrox Licenses
 * Checked in by:  rYoho;  Rob Yoho
 * Change Description:  
 *   Modified OnValidate to always call SVImageAnalyzerClass::OnValidate().
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.3   13 May 2013 12:15:48   bWalter
 * Project:  SVObserver
 * Change Request (SCR) nbr:  814
 * SCR Title:  Upgrade SVObserver to Compile Using Visual Studio 2010
 * Checked in by:  bWalter;  Ben Walter
 * Change Description:  
 *   Merged with svo_src label SVO 6.10 Beta 014.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.48   07 May 2013 12:59:30   ryoho
 * Project:  SVObserver
 * Change Request (SCR) nbr:  796
 * SCR Title:  Add a License Manager that will handle the Matrox Licenses
 * Checked in by:  rYoho;  Rob Yoho
 * Change Description:  
 *   removed the license check from InitMil to ResetObject.  In ResetObect it will return not run through the MIL part if there is a license error
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.47   02 May 2013 11:02:58   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  810
 * SCR Title:  Remove support for Legacy Tools
 * Checked in by:  rYoho;  Rob Yoho
 * Change Description:  
 *   Updated process message method to fix formatting issues and updated the reset object section to only assert when the HRESULT is a failed status.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.2   08 May 2013 16:01:38   bWalter
 * Project:  SVObserver
 * Change Request (SCR) nbr:  814
 * SCR Title:  Upgrade SVObserver to Compile Using Visual Studio 2010
 * Checked in by:  bWalter;  Ben Walter
 * Change Description:  
 *   Initial check in to SVObserver_src.  (Merged with svo_src label SVO 6.10 Beta 012.)
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.46   26 Apr 2013 11:26:10   ryoho
 * Project:  SVObserver
 * Change Request (SCR) nbr:  796
 * SCR Title:  Add a License Manager that will handle the Matrox Licenses
 * Checked in by:  rYoho;  Rob Yoho
 * Change Description:  
 *   changed the createobject so that it will return true even if the licensemanger says there is no license.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.1   06 May 2013 14:38:06   bWalter
 * Project:  SVObserver
 * Change Request (SCR) nbr:  814
 * SCR Title:  Upgrade SVObserver to Compile Using Visual Studio 2010
 * Checked in by:  bWalter;  Ben Walter
 * Change Description:  
 *   Merged with svo_src label SVO 6.10 Beta 009.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.45   10 Apr 2013 14:26:06   ryoho
 * Project:  SVObserver
 * Change Request (SCR) nbr:  796
 * SCR Title:  Add a License Manager that will handle the Matrox Licenses
 * Checked in by:  rYoho;  Rob Yoho
 * Change Description:  
 *   -changed where it adds the license error.  in OnRun if there is an error it will SetInvalid
 * -in the destructor if there was a license error remove it from the list
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.0   23 Apr 2013 09:40:22   bWalter
 * Project:  SVObserver
 * Change Request (SCR) nbr:  814
 * SCR Title:  Upgrade SVObserver to Compile Using Visual Studio 2010
 * Checked in by:  bWalter;  Ben Walter
 * Change Description:  
 *   Initial check in to SVObserver_src.  (Merged with svo_src label SVO 6.10 Beta 008.)
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.44   25 Mar 2013 12:40:00   ryoho
 * Project:  SVObserver
 * Change Request (SCR) nbr:  796
 * SCR Title:  Add a License Manager that will handle the Matrox Licenses
 * Checked in by:  rYoho;  Rob Yoho
 * Change Description:  
 *   changed the adding of the License error
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.43   05 Mar 2013 08:45:34   ryoho
 * Project:  SVObserver
 * Change Request (SCR) nbr:  796
 * SCR Title:  Add a License Manager that will handle the Matrox Licenses
 * Checked in by:  rYoho;  Rob Yoho
 * Change Description:  
 *   changed error string text
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.42   27 Feb 2013 11:19:56   ryoho
 * Project:  SVObserver
 * Change Request (SCR) nbr:  796
 * SCR Title:  Add a License Manager that will handle the Matrox Licenses
 * Checked in by:  rYoho;  Rob Yoho
 * Change Description:  
 *   Added check to see if the identification license is present
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.41   18 Jul 2012 13:23:46   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  769
 * SCR Title:  Fix Problems and Crashes with Inspection Document Display Updates
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Removed obsolete methods assiciated with overlay drawling and fixed problem with copying data to byte buffer have wrong format.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.40   29 Jun 2012 14:56:20   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  771
 * SCR Title:  Upgrade to the Next Version of Matrox Imaging Library (MIL)
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated resource file to add the new barcode parameter to fix an issues with MIL 9 processing.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.39   10 Feb 2011 14:30:24   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  712
 * SCR Title:  Fix issues with black images when using command interface (SIAC)
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated source code to now use shared pointer instead of untracked structure.  This fixes destruction, tracking and rebuilding issues.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.38   27 Jan 2011 10:47:12   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  712
 * SCR Title:  Fix issues with black images when using command interface (SIAC)
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated source code to fix issues with Matrox Buffer Management.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.37   08 Dec 2010 07:40:22   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  707
 * SCR Title:  Change Inspection Display Functionality to Force Display of Last Inspected
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated source code to include changes in notification functionality using the Observer Design Pattern.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.36   04 Nov 2010 13:18:22   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  707
 * SCR Title:  Change Inspection Display Functionality to Force Display of Last Inspected
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated source code to fix issues with image processing and display image processing classes and associated includes.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.35   15 Oct 2010 15:25:02   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  701
 * SCR Title:  Add New Font Functionality to the Existing Command Interface
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated source code assoicated with casting issues and HBITMAP changes.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.34   30 Aug 2010 13:48:32   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  700
 * SCR Title:  Remove String Buffer Problems
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Update code to remove or fix uses of CString GetBuffer and ReleaseBuffer methods.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.33   01 Jun 2010 14:13:00   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  693
 * SCR Title:  Fix Performance Issue with Inspection Process
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated source code to improve ability to track performance.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.32   01 Mar 2010 08:32:28   tbair
 * Project:  SVObserver
 * Change Request (SCR) nbr:  689
 * SCR Title:  Fix the Barcode Score
 * Checked in by:  tBair;  Tom Bair
 * Change Description:  
 *   Fixed Match Score in the onRun Function.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.31   09 Apr 2009 14:58:34   ryoho
 * Project:  SVObserver
 * Change Request (SCR) nbr:  657
 * SCR Title:  Remove Apply button from the Tool Adjust Dialogs
 * Checked in by:  rYoho;  Rob Yoho
 * Change Description:  
 *   removed the apply button
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.30   06 Jun 2008 15:55:48   tbair
 * Project:  SVObserver
 * Change Request (SCR) nbr:  627
 * SCR Title:  Add RawData object to the Barcode Analyzer and format read string
 * Checked in by:  tBair;  Tom Bair
 * Change Description:  
 *   Changed Translate to use 3 digits and added GS1 text on String Format Combo Box.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.29   06 Jun 2008 11:22:20   tbair
 * Project:  SVObserver
 * Change Request (SCR) nbr:  627
 * SCR Title:  Add RawData object to the Barcode Analyzer and format read string
 * Checked in by:  tBair;  Tom Bair
 * Change Description:  
 *   Added static_cast<size_t> to remove warning in onRun
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.28   03 Jun 2008 14:24:34   tbair
 * Project:  SVObserver
 * Change Request (SCR) nbr:  627
 * SCR Title:  Add RawData object to the Barcode Analyzer and format read string
 * Checked in by:  tBair;  Tom Bair
 * Change Description:  
 *   Added Threshold Type Radio to Dialog.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.27   30 May 2008 14:43:40   tbair
 * Project:  SVObserver
 * Change Request (SCR) nbr:  627
 * SCR Title:  Add RawData object to the Barcode Analyzer and format read string
 * Checked in by:  tBair;  Tom Bair
 * Change Description:  
 *   Added new value object to hold adaptive threshold bool for barcode.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.26   29 May 2008 09:52:18   tbair
 * Project:  SVObserver
 * Change Request (SCR) nbr:  627
 * SCR Title:  Add RawData object to the Barcode Analyzer and format read string
 * Checked in by:  tBair;  Tom Bair
 * Change Description:  
 *   Modified OnRun function to remove, translate, or replace control characters. 
 * 
 * Also added RawData output.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.25   24 Jul 2007 10:49:32   tbair
 * Project:  SVObserver
 * Change Request (SCR) nbr:  597
 * SCR Title:  Upgrade Matrox Imaging Library to version 8.0
 * Checked in by:  tBair;  Tom Bair
 * Change Description:  
 *   Update to Matrox Imaging Library 8.0
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.24   22 Jun 2007 10:35:52   Joe
 * Project:  SVObserver
 * Change Request (SCR) nbr:  598
 * SCR Title:  Upgrade SVObserver to compile using vc++ in VS2005
 * Checked in by:  jSpila;  Joseph Spila
 * Change Description:  
 *   These changes include removal of all VC6 constraints, project files, and workspace files.  The new VC8 project and solution files will replace the VC6 files with the new tighter compilation rules.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.23   21 Jun 2007 11:56:30   Joe
 * Project:  SVObserver
 * Change Request (SCR) nbr:  598
 * SCR Title:  Upgrade SVObserver to compile using vc++ in VS2005
 * Checked in by:  jSpila;  Joseph Spila
 * Change Description:  
 *   These changes include modification based on fixing compiler-based and project-based differences between VC6 and VC8.  These changes mainly include casting issues, but some include type conversion and assignment of new compiler controlling defines.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.22   20 Oct 2005 14:19:08   Joe
 * Project:  SVObserver
 * Change Request (SCR) nbr:  500
 * SCR Title:  Reduce delay when adjusting tool parameters with a large toolset
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated code to handle a reset problem with the multi-result match string file.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.21   29 Sep 2005 09:54:52   ryoho
 * Project:  SVObserver
 * Change Request (SCR) nbr:  498
 * SCR Title:  Add individual timeout capability to Bar Code analyzers
 * Checked in by:  rYoho;  Rob Yoho
 * Change Description:  
 *   made the attributes for the timeout to be able to be set remotely.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.20   29 Sep 2005 09:48:44   ryoho
 * Project:  SVObserver
 * Change Request (SCR) nbr:  498
 * SCR Title:  Add individual timeout capability to Bar Code analyzers
 * Checked in by:  rYoho;  Rob Yoho
 * Change Description:  
 *   added new value object for the Bar Code Timeout
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.19   16 Aug 2005 08:52:38   Joe
 * Project:  SVObserver
 * Change Request (SCR) nbr:  500
 * SCR Title:  Reduce delay when adjusting tool parameters with a large toolset
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Removed un-necessary casting from AddInputRequest method.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.18   08 Aug 2005 15:27:30   Joe
 * Project:  SVObserver
 * Change Request (SCR) nbr:  500
 * SCR Title:  Reduce delay when adjusting tool parameters with a large toolset
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated objects and dialogs to use the ActiveX Interface to update inspection data.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.17   29 Jul 2005 12:03:30   Joe
 * Project:  SVObserver
 * Change Request (SCR) nbr:  500
 * SCR Title:  Reduce delay when adjusting tool parameters with a large toolset
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated object initialization to change the parameters for RegisterEmbeddedObject for a SVValueObjectClass.  The two additional parameters will inform the preperation process before toolset execution.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.16   22 Jun 2005 09:14:14   ebeyeler
 * Project:  SVObserver
 * Change Request (SCR) nbr:  464
 * SCR Title:  Add array indexing for value objects
 * Checked in by:  eBeyeler;  Eric Beyeler
 * Change Description:  
 *   object attributes use accessor functions
 * cleaned up tabs
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.15   14 Jun 2005 09:14:36   ryoho
 * Project:  SVObserver
 * Change Request (SCR) nbr:  494
 * SCR Title:  Add MIL Barcode Timeout (PP1)
 * Checked in by:  rYoho;  Rob Yoho
 * Change Description:  
 *   Added M_TIMEOUT  (MIL PP1)
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.14   17 Feb 2005 13:10:06   Joe
 * Project:  SVObserver
 * Change Request (SCR) nbr:  456
 * SCR Title:  Update Image and Tool Objects to use the new Extent Classes
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Update class with new methodologies based on new base object, extents and reset structure.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.13   08 Sep 2004 11:10:24   tbair
 * Project:  SVObserver
 * Change Request (SCR) nbr:  444
 * SCR Title:  Change Bar Code Analyzer Pass / Fail Result
 * Checked in by:  tBair;  Tom Bair
 * Change Description:  
 *   Modified OnRun to include Match Score and warn on fail logic.
 * Modified CreateObject and Init to support new value objects
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.12   08 Oct 2003 10:50:06   ryoho
 * Project:  SVObserver
 * Change Request (SCR) nbr:  385
 * SCR Title:  Expose additional parameters for SIAC
 * Checked in by:  rYoho;  Rob Yoho
 * Change Description:  
 *   changed attributes on value objects so that they can be set by the activeX
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.11   08 Jul 2003 08:47:02   ebeyeler
 * Project:  SVObserver
 * Change Request (SCR) nbr:  322
 * SCR Title:  Add Additional Digital IO Resources to SVObserver
 * Checked in by:  eBeyeler;  Eric Beyeler
 * Change Description:  
 *   changed mpsvImaging to use HRESULT instead of BOOL
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.10   23 May 2003 11:45:30   ryoho
 * Project:  SVObserver
 * Change Request (SCR) nbr:  367
 * SCR Title:  BarCodes - IsCreated flag becomes invalid after editing a configuration
 * Checked in by:  rYoho;  Rob Yoho
 * Change Description:  
 *   In IntiMil() it will check the MIL Id has been closed, if it has not it will close the MIL Id.  Also changed the stringSize to use the M_ANY instead of M_DEFAULT
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.9   17 Apr 2003 16:45:16   rschock
 * Project:  SVObserver
 * Change Request (SCR) nbr:  346
 * SCR Title:  Update SVObserver to Version 4.21 Release
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Redid the #include defines and standardized the Tracker log headers and removed warning from release mode builds.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.8   27 Mar 2003 15:27:46   rschock
 * Project:  SVObserver
 * Change Request (SCR) nbr:  329
 * SCR Title:  Add bar codes to SVObserver
 * Checked in by:  eBeyeler;  Eric Beyeler
 * Change Description:  
 *   Added try/catch code to the OnRun to catch exceptions from the MIL library.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.7   27 Mar 2003 11:17:26   Joe
 * Project:  SVObserver
 * Change Request (SCR) nbr:  329
 * SCR Title:  Add bar codes to SVObserver
 * Checked in by:  eBeyeler;  Eric Beyeler
 * Change Description:  
 *   Updated onRun method to add additional error reporting.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.6   05 Mar 2003 11:42:26   ebeyeler
 * Project:  SVObserver
 * Change Request (SCR) nbr:  329
 * SCR Title:  Add bar codes to SVObserver
 * Checked in by:  eBeyeler;  Eric Beyeler
 * Change Description:  
 *   Added Bar codes
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.5   29 Jan 2003 11:33:10   joe
 * Project:  SVObserver
 * Change Request (SCR) nbr:  301
 * SCR Title:  Implement Result Image Circle Buffer
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated onRun method to include new image circle buffer logic.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.4   09 Dec 2002 18:56:08   Joe
 * Project:  SVObserver
 * Change Request (SCR) nbr:  295
 * SCR Title:  Remove Result Data from Configuration Printout
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   CreateObject method was modified to properly update the isCreated flag and to set the printability of the internal value objects.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
*/
