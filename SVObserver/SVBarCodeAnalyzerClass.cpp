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
#include "SVLibrary/SVOIniClass.h"

#include "SVBarCodeResult.h"   
#include "SVLibrary/SVBarCodeProperties.h"
#include "SVOCore/SVBarCodeBuffer.h"
#include "SVOCore/SVImageProcessingClass.h"
#include "SVOCore/SVImageClass.h"
#include "SVInspectionProcess.h"
#include "SVOLicenseManager/SVOLicenseManager.h"
#include "TextDefinesSvO.h"
#include "SVStatusLibrary/MessageManager.h"
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

SVBarCodeAnalyzerClass::SVBarCodeAnalyzerClass (SVObjectClass* POwner, int StringResourceID)
	: SVImageAnalyzerClass (POwner, StringResourceID)
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
	
	m_outObjectInfo.m_ObjectTypeInfo.SubType = SVBarCodeAnalyzerObjectType;
	
	// Register Embedded Objects
	RegisterEmbeddedObject (&msv_szBarCodeValue, SVBarCodeObjectGuid, IDS_OBJECTNAME_BARCODEVALUE, false, SvOi::SVResetItemNone);
	RegisterEmbeddedObject (&msv_szRegExpressionValue, SVRegExpressionObjectGuid, IDS_OBJECTNAME_CONTROLVALUE, false, SvOi::SVResetItemOwner);
	RegisterEmbeddedObject (&msv_lBarCodeType, SVBCTypeObjectGuid, IDS_OBJECTNAME_BARCODETYPE, false, SvOi::SVResetItemOwner);
	RegisterEmbeddedObject (&msv_dOrientation, SVBCOrientationObjectGuid, IDS_OBJECTNAME_ORIENTATION, false, SvOi::SVResetItemOwner);
	RegisterEmbeddedObject (&msv_dSkewNegative, SVBCSkewNegativeObjectGuid, IDS_OBJECTNAME_SKEWNEGATIVE, false, SvOi::SVResetItemOwner);
	RegisterEmbeddedObject (&msv_dSkewPositive, SVBCSkewPositiveObjectGuid, IDS_OBJECTNAME_SKEWPOSITIVE, false, SvOi::SVResetItemOwner);
	RegisterEmbeddedObject (&msv_dCellMinSize, SVBCCellMinSizeObjectGuid, IDS_OBJECTNAME_CELLMINSIZE, false, SvOi::SVResetItemOwner);
	RegisterEmbeddedObject (&msv_dCellMaxSize, SVBCCellMaxSizeObjectGuid, IDS_OBJECTNAME_CELLMAXSIZE, false, SvOi::SVResetItemOwner);
	RegisterEmbeddedObject (&msv_dCellNumberX, SVBCCellNumberXObjectGuid, IDS_OBJECTNAME_CELLNUMBERX, false, SvOi::SVResetItemOwner);
	RegisterEmbeddedObject (&msv_dCellNumberY, SVBCCellNumberYObjectGuid, IDS_OBJECTNAME_CELLNUMBERY, false, SvOi::SVResetItemOwner);
	RegisterEmbeddedObject (&msv_dSpeed, SVBCSpeedObjectGuid, IDS_OBJECTNAME_SPEED, false, SvOi::SVResetItemOwner);
	RegisterEmbeddedObject (&msv_dThreshold, SVBCThresholdObjectGuid, IDS_OBJECTNAME_THRESHOLD, false, SvOi::SVResetItemOwner);
	RegisterEmbeddedObject (&msv_dStringSize, SVBCStringSizeObjectGuid, IDS_OBJECTNAME_STRINGSIZE, false, SvOi::SVResetItemOwner);
	RegisterEmbeddedObject (&msv_dErrorCorrection, SVBCErrorCorrectionObjectGuid, IDS_OBJECTNAME_ERRORCORRECTION, false, SvOi::SVResetItemOwner);
	RegisterEmbeddedObject (&msv_dEncoding, SVBCEncodingObjectGuid, IDS_OBJECTNAME_ENCODING, false, SvOi::SVResetItemOwner);
	RegisterEmbeddedObject (&msv_dForegroundColor, SVBCForegroundColorObjectGuid, IDS_OBJECTNAME_FOREGROUNDCOLOR, false, SvOi::SVResetItemOwner);
	RegisterEmbeddedObject (&msv_bSaveStringInFile, SVBCSaveStringInFileObjectGuid, IDS_OBJECTNAME_SAVESTRINGINFILE, false, SvOi::SVResetItemOwner);
	RegisterEmbeddedObject (&msv_szStringFileName, SVBCStringFileNameObjectGuid, IDS_OBJECTNAME_STRINGFILENAME, false, SvOi::SVResetItemOwner);
	RegisterEmbeddedObject (&m_bWarnOnFailedRead, SVBCWarnOnFailedReadObjectGuid, IDS_OBJECTNAME_WARNONFAILEDREAD, false, SvOi::SVResetItemOwner);
	RegisterEmbeddedObject (&msv_lBarcodeTimeout, SVBCTimeoutGuid, IDS_OBJECTNAME_BARCODETIMEOUT, false, SvOi::SVResetItemOwner);
	// IDS_OBJECTNAME_BARCODEUNEVENGRID
	RegisterEmbeddedObject (&msv_bUnEvenGrid, SVBCUnevenGridGuid,IDS_OBJECTNAME_BARCODEUNEVENGRID, false, SvOi::SVResetItemOwner);

	// To support special DMCs May 2008.
	RegisterEmbeddedObject (&msv_RawData, SVBCRawDataGuid, IDS_OBJECTNAME_BARCODERAWDATA, false, SvOi::SVResetItemOwner);
	RegisterEmbeddedObject (&msv_eStringFormat, SVBCStringFormatGuid, IDS_OBJECTNAME_BARCODESTRINGFORMAT, false, SvOi::SVResetItemOwner);
	RegisterEmbeddedObject (&msv_lThresholdType, SVBCThresholdTypeGuid, IDS_OBJECTNAME_BARCODETHRESHOLDTYPE, false, SvOi::SVResetItemOwner);

	// Set default values
	msv_szBarCodeValue.SetDefaultValue (_T(""));
	msv_szBarCodeValue.setSaveValueFlag(false);
	msv_szRegExpressionValue.SetDefaultValue (_T(""));
	msv_lBarCodeType.SetDefaultValue(SVDataMatrix);
	msv_dOrientation.SetDefaultValue(SVValueDefault);
	msv_dSkewNegative.SetDefaultValue(SVValueDefault);
	msv_dSkewPositive.SetDefaultValue(SVValueDefault);
	msv_dCellMinSize.SetDefaultValue(SVValueDefault);
	msv_dCellMaxSize.SetDefaultValue(SVValueDefault);
	msv_dCellNumberX.SetDefaultValue(SVValueAny);
	msv_dCellNumberY.SetDefaultValue(SVValueAny);
	msv_dSpeed.SetDefaultValue(SVValueMedium);
	msv_dThreshold.SetDefaultValue(SVValueDefault);
	msv_dStringSize.SetDefaultValue(SVValueDefault);
	msv_dErrorCorrection.SetDefaultValue(SVValueAny);
	msv_dEncoding.SetDefaultValue(SVValueAny);
	msv_dForegroundColor.SetDefaultValue(SVValueForegroundBlack);
	msv_bSaveStringInFile.SetDefaultValue( BOOL(false));
	msv_szStringFileName.SetDefaultValue(_T("BCRegExp.txt"));
	m_bWarnOnFailedRead.SetDefaultValue( BOOL(false));

	msv_bUnEvenGrid.SetDefaultValue( BOOL(false));

	// To support special DMCs May 2008.
	// Use ~ to simulate non printable characters...
	SVString FormatEnum = SvUl_SF::Format(_T("Remove GS1 Control Characters=%d,Translate GS1 Control Characters=%d,Replace GS1 Control Characters=%d"), 
											SVBCStringFormatRemoveCharacters, SVBCStringFormatTranslateCharacters, SVBCStringFormatReplaceCharacters );
	msv_eStringFormat.SetEnumTypes( FormatEnum.c_str() );
	msv_eStringFormat.SetDefaultValue( SVBCStringFormatRemoveCharacters, true );
	msv_lThresholdType.SetDefaultValue( 0, true ); // Default Normal thresholding
	msv_RawData.SetArraySize(256);
	msv_RawData.setSaveValueFlag(false);

	//set MIL Timeout default to be what is in the INI file...
	SvLib::SVOINIClass l_SvimIni(SvStl::GlobalPath::Inst().GetSVIMIniPath());

	int l_mTimeout = l_SvimIni.GetValueInt(_T("SVIM Information"), _T("MILBarcodeTimeout"),20);

	msv_lBarcodeTimeout.SetDefaultValue(l_mTimeout,TRUE);

	// Set default inputs and outputs
	addDefaultInputObjects();
	
	// Instantiate Children
	SVBarCodeResultClass *pAnalyzerResult = new SVBarCodeResultClass (this, IDS_CLASSNAME_SVBARCODEANALYZERESULT);
	
	if (pAnalyzerResult)
		Add (pAnalyzerResult);

}

bool SVBarCodeAnalyzerClass::InitMil (SvStl::MessageContainerVector *pErrorMessages)
{
	SVImageClass *pSVImage;
	double dParm;

	if ( !m_MilCodeId.empty() )
	{
		CloseMil();
	}
	
	if (pSVImage = getInputImage ())
	{
		// &&&
		SVBarCodeBufferInfoClass svData;

		svData.HBuffer.milBarCode = m_MilCodeId;

		long l_lTmpType;
		msv_lBarCodeType.GetValue( l_lTmpType );
		svData.m_lCodeType = static_cast<SVBarCodeTypesEnum>( l_lTmpType );
		if ( S_OK == SVImageProcessingClass::CreateBarCodeBuffer( &svData ) )
		{
			
			HRESULT MatroxCode;

			m_MilCodeId = svData.HBuffer.milBarCode;

			// get M_TIMEOUT value from Registry
			//int l_mTimeout = AfxGetApp()->GetProfileInt(_T( "Settings" ),_T( "MILTimeout" ), 2000 );

			long l_mTimeout;
			msv_lBarcodeTimeout.GetValue(l_mTimeout);
			double dL1 = (double) l_mTimeout;

			MatroxCode = SVMatroxBarCodeInterface::Set( m_MilCodeId, SVBCCodeTimeout, dL1 );
			msv_dOrientation.GetValue (dParm);
			MatroxCode = SVMatroxBarCodeInterface::Set( m_MilCodeId, SVBCSearchAngle, dParm );

			msv_dSkewNegative.GetValue (dParm);
			MatroxCode = SVMatroxBarCodeInterface::Set( m_MilCodeId, SVBCSearchAngleDeltaNeg, dParm);

			msv_dSkewPositive.GetValue (dParm);
			MatroxCode = SVMatroxBarCodeInterface::Set( m_MilCodeId, SVBCSearchAngleDeltaPos, dParm);

			msv_dCellMinSize.GetValue (dParm);
			MatroxCode = SVMatroxBarCodeInterface::Set( m_MilCodeId, SVBCCellSizeMin, dParm);

			msv_dCellMaxSize.GetValue (dParm);
			MatroxCode = SVMatroxBarCodeInterface::Set( m_MilCodeId, SVBCCellSizeMax, dParm);

			msv_dCellNumberX.GetValue (dParm);
			MatroxCode = SVMatroxBarCodeInterface::Set( m_MilCodeId, SVBCCellNumberX, dParm);

			msv_dCellNumberY.GetValue (dParm);
			MatroxCode = SVMatroxBarCodeInterface::Set( m_MilCodeId, SVBCCellNumberY, dParm);

			msv_dSpeed.GetValue (dParm);
			MatroxCode = SVMatroxBarCodeInterface::Set( m_MilCodeId, SVBCSpeed, dParm);

			// Threshold
			long l_lThresholdType ;
			msv_lThresholdType.GetValue( l_lThresholdType );
			switch( l_lThresholdType )
			{
				case 0:
				{
					msv_dThreshold.GetValue (dParm);
					MatroxCode = SVMatroxBarCodeInterface::Set( m_MilCodeId, SVBCThreshold, dParm);
					break;
				}
				case 1:	// Auto Threshold
				{
					dParm = 0;
					MatroxCode = SVMatroxBarCodeInterface::Set( m_MilCodeId, SVBCAutoThreshold, dParm );
					break;
				}
				case 2:	// Adaptive Threshold
				{
					dParm = 0;
					MatroxCode = SVMatroxBarCodeInterface::Set( m_MilCodeId, SVBCAdaptiveThreshold, dParm );
					break;
				}
			}

			msv_dStringSize.GetValue (dParm);
			if ( dParm == (double) SVValueDefault ) // M_DEFAULT )
			{
				dParm = (double) SVValueAny;		// M_ANY;
			}
			MatroxCode = SVMatroxBarCodeInterface::Set( m_MilCodeId, SVBCBarcodeStringSize, dParm);

			msv_dEncoding.GetValue (dParm);
			MatroxCode = SVMatroxBarCodeInterface::Set( m_MilCodeId, SVBCEncoding, dParm);

			msv_dErrorCorrection.GetValue (dParm);
			MatroxCode = SVMatroxBarCodeInterface::Set( m_MilCodeId, SVBCErrorCorrection, dParm);

			msv_dForegroundColor.GetValue (dParm);
			MatroxCode = SVMatroxBarCodeInterface::Set( m_MilCodeId, SVBCForeGroundValue, dParm);

			BOOL bUnevenGrid;
			msv_bUnEvenGrid.GetValue(bUnevenGrid);
			MatroxCode = SVMatroxBarCodeInterface::Set( m_MilCodeId, SVBCUnEvenGrid, (long)bUnevenGrid );
			return true;
		}
		else
		{
			// McodeAlloc failed
			if (nullptr != pErrorMessages)
			{
				SvStl::MessageContainer Msg( SVMSG_SVO_92_GENERAL_ERROR, SvStl::Tid_MilBarCodeInitFailed, SvStl::SourceFileParams(StdMessageParams), 0, GetUniqueObjectID() );
				pErrorMessages->push_back(Msg);
			}
			return false;
		}
	}
	else
	{
		// getInputImage failed
		if (nullptr != pErrorMessages)
		{
			SvStl::MessageContainer Msg( SVMSG_SVO_92_GENERAL_ERROR, SvStl::Tid_NoSourceImage, SvStl::SourceFileParams(StdMessageParams), 0, GetUniqueObjectID() );
			pErrorMessages->push_back(Msg);
		}
		return false;
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

bool SVBarCodeAnalyzerClass::CreateObject(SVObjectLevelCreateStruct* pCreateStructure)
{
	bool bOk = false;


	if ( !m_bHasLicenseError )
	{
		if (SVImageAnalyzerClass::CreateObject (pCreateStructure))
		{

			if (InitMil ())
			{
				LoadRegExpression();

				bOk = true;
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
		bOk = true;
	}

	// *** Set/Reset Printable Attributes ***
	msv_szBarCodeValue.SetObjectAttributesAllowed( SvOi::SV_PRINTABLE, SvOi::SetAttributeType::RemoveAttribute );
	msv_szRegExpressionValue.SetObjectAttributesAllowed( SvOi::SV_PRINTABLE, SvOi::SetAttributeType::AddAttribute );
	msv_lBarCodeType.SetObjectAttributesAllowed( SvOi::SV_PRINTABLE, SvOi::SetAttributeType::AddAttribute );
	msv_dOrientation.SetObjectAttributesAllowed( SvOi::SV_PRINTABLE, SvOi::SetAttributeType::AddAttribute );
	msv_dSkewNegative.SetObjectAttributesAllowed( SvOi::SV_PRINTABLE, SvOi::SetAttributeType::AddAttribute );
	msv_dSkewPositive.SetObjectAttributesAllowed( SvOi::SV_PRINTABLE, SvOi::SetAttributeType::AddAttribute );
	msv_dCellMinSize.SetObjectAttributesAllowed( SvOi::SV_PRINTABLE, SvOi::SetAttributeType::AddAttribute );
	msv_dCellMaxSize.SetObjectAttributesAllowed( SvOi::SV_PRINTABLE, SvOi::SetAttributeType::AddAttribute );
	msv_dCellNumberX.SetObjectAttributesAllowed( SvOi::SV_PRINTABLE, SvOi::SetAttributeType::AddAttribute );
	msv_dCellNumberY.SetObjectAttributesAllowed( SvOi::SV_PRINTABLE, SvOi::SetAttributeType::AddAttribute );
	msv_dSpeed.SetObjectAttributesAllowed( SvOi::SV_PRINTABLE | SvOi::SV_REMOTELY_SETABLE, SvOi::SetAttributeType::AddAttribute );
	msv_dThreshold.SetObjectAttributesAllowed( SvOi::SV_PRINTABLE, SvOi::SetAttributeType::AddAttribute );
	msv_dStringSize.SetObjectAttributesAllowed( SvOi::SV_PRINTABLE, SvOi::SetAttributeType::AddAttribute );
	msv_dErrorCorrection.SetObjectAttributesAllowed( SvOi::SV_PRINTABLE, SvOi::SetAttributeType::AddAttribute );
	msv_dEncoding.SetObjectAttributesAllowed( SvOi::SV_PRINTABLE, SvOi::SetAttributeType::AddAttribute );
	msv_dForegroundColor.SetObjectAttributesAllowed( SvOi::SV_PRINTABLE, SvOi::SetAttributeType::AddAttribute );
	msv_bSaveStringInFile.SetObjectAttributesAllowed( SvOi::SV_PRINTABLE, SvOi::SetAttributeType::AddAttribute );
	msv_szStringFileName.SetObjectAttributesAllowed( SvOi::SV_PRINTABLE, SvOi::SetAttributeType::AddAttribute );
	m_bWarnOnFailedRead.SetObjectAttributesAllowed( SvOi::SV_PRINTABLE | SvOi::SV_REMOTELY_SETABLE, SvOi::SetAttributeType::AddAttribute );
	msv_lBarcodeTimeout.SetObjectAttributesAllowed( SvOi::SV_PRINTABLE | SvOi::SV_REMOTELY_SETABLE, SvOi::SetAttributeType::AddAttribute );
	msv_RawData.SetObjectAttributesAllowed( SvOi::SV_PRINTABLE, SvOi::SetAttributeType::RemoveAttribute );
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

bool SVBarCodeAnalyzerClass::CloseObject()
{
	CloseMil ();
	return SVImageAnalyzerClass::CloseObject();
}

SvOi::IObjectClass* SVBarCodeAnalyzerClass::GetResultObject()
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

bool SVBarCodeAnalyzerClass::onRun (SVRunStatusClass &rRunStatus, SvStl::MessageContainerVector *pErrorMessages)
{
	if ( m_bHasLicenseError )
	{
		if (nullptr != pErrorMessages)
		{
			SvStl::MessageContainer Msg( SVMSG_SVO_92_GENERAL_ERROR, SvStl::Tid_SVObserver_MatroxLicenseNotFound, SvStl::SourceFileParams(StdMessageParams), 0, GetUniqueObjectID() );
			pErrorMessages->push_back(Msg);
		}
		SetInvalid ();
		rRunStatus.SetInvalid ();
		return false;
	}
	
	msv_szBarCodeValue.SetValue(SVString());
	if (SVImageAnalyzerClass::onRun (rRunStatus, pErrorMessages))
	{
		SVSmartHandlePointer ImageHandle;
		
		// Used in Result Class to know whether to Fail or Warn.
		SVBarCodeResultClass* pResult = static_cast<SVBarCodeResultClass*>(GetResultObject());
		BOOL bWarnOnFailedRead;
		m_bWarnOnFailedRead.GetValue(bWarnOnFailedRead); // Add value from checkbox instead of true to warn on fail.
									// default value will be false.
		pResult->m_bFailedToRead = bWarnOnFailedRead != FALSE;// Preset flag to failed condition..

		SVImageClass* pInputImage = getInputImage ();		
		if ( pInputImage->GetImageHandle( ImageHandle ) && !( ImageHandle.empty() ) )
		{
			try
			{
				SVImageBufferHandleImage l_MilBuffer;
				ImageHandle->GetData( l_MilBuffer );

				SVMatroxBuffer ImageBufId = l_MilBuffer.GetBuffer();

				bool Result = SVMatroxBarCodeInterface::Execute( m_MilCodeId, ImageBufId, pErrorMessages );

				long lMilResult = SVBCStatusNotFound;
				Result = Result && SVMatroxBarCodeInterface::GetResult( m_MilCodeId, SVBCBarCodeStatus, lMilResult, pErrorMessages );

				if (!Result)
				{
					if (nullptr != pErrorMessages)
					{
						for (SvStl::MessageContainerVector::iterator iter = pErrorMessages->begin(); pErrorMessages->end() != iter; iter++ )
						{	//set GUId of this object to the error message without a GUID. (should be error message from the matrox interface)
							if (SV_GUID_NULL == iter->getObjectId())
							{
								iter->setObjectId(GetUniqueObjectID());
							}
						}
					}
					SetInvalid ();
					rRunStatus.SetInvalid ();
					return false;
				}

				switch ( lMilResult )
				{
					case SVBCStatusReadOK:
					{
						pResult->m_bFailedToRead = false;
						long cbBarCodeValue = 0;
						Result &= SVMatroxBarCodeInterface::GetResult( m_MilCodeId, SVBCBarcodeStringSize, cbBarCodeValue, pErrorMessages );
						SVString BarCodeString;
						Result &= SVMatroxBarCodeInterface::GetResult( m_MilCodeId, SVBCBarCodeString, BarCodeString, pErrorMessages );
						double dScore = 0.0;
						Result &= SVMatroxBarCodeInterface::GetResult( m_MilCodeId, SVBCBarcodeScore, dScore, pErrorMessages );

						pResult->m_dReadScore.SetValue(dScore * 100);

						// To support special DMCs May 2008.
						// Copy string to Byte Value Object
						if( BarCodeString.size() > static_cast<size_t>(msv_RawData.getArraySize()) )
						{
							msv_RawData.SetArraySize( static_cast< int >( BarCodeString.size() ) );
						}

						std::vector<BYTE> BarCodeValue;
						BarCodeValue.resize(BarCodeString.size() );
						//Convert string character to byte vector
						std::copy( BarCodeString.begin(), BarCodeString.end(), BarCodeValue.begin() );
						if ( S_OK != msv_RawData.SetResultSize( static_cast<int> ( BarCodeString.size() ) ) || S_OK != msv_RawData.SetArrayValues( BarCodeValue.begin(), BarCodeValue.end()))
						{
							Result = false;
							if (nullptr != pErrorMessages)
							{
								SvStl::MessageContainer Msg( SVMSG_SVO_92_GENERAL_ERROR, SvStl::Tid_SetValueFailed, SvStl::SourceFileParams(StdMessageParams), 0, GetUniqueObjectID() );
								pErrorMessages->push_back(Msg);
							}
						}

						// based on msv_eBarcodeStringFormat replace control characters...
						long l_lTmpFormat=SVBCStringFormatReplaceCharacters;
						msv_eStringFormat.GetValue( l_lTmpFormat );
						switch( l_lTmpFormat )
						{
							case SVBCStringFormatReplaceCharacters:
							{
								replace_if( BarCodeString.begin(),
											BarCodeString.end(),
											CharIsControl,
											_T('~'));
								break;
							}
							case SVBCStringFormatRemoveCharacters:
							{
								SVString l_strRemoved;
								l_strRemoved.resize( BarCodeString.size() + 1);
								int j = 0;
								for( size_t i = 0 ; i < BarCodeString.size(); i++ )
								{
									if( CharIsControl(BarCodeString[i]) )
									{
										continue;
									}
									l_strRemoved[j++]=BarCodeString[i];
								}
								l_strRemoved[j] = 0; // Teminate String...
								l_strRemoved.resize(j);
								BarCodeString = l_strRemoved;
								break;
							}
							case SVBCStringFormatTranslateCharacters:
							{
								SVString l_strTranslated;
								l_strTranslated.resize( BarCodeString.size()+256);
								int j = 0;
								for( size_t i = 0 ; i < BarCodeString.size(); i++ )
								{
									if( CharIsControl(BarCodeString[i]) )
									{
										SVString Temp = SvUl_SF::Format(_T("%03d"),BarCodeString[i]);
										l_strTranslated[j++] = '\\';
										l_strTranslated[j++] = Temp[0];
										l_strTranslated[j++] = Temp[1];
										l_strTranslated[j++] = Temp[2];
									}
									else if ( BarCodeString[i] == '\\' )
									{
										l_strTranslated[j++] = '\\';
										l_strTranslated[j++] = '\\';
									}
									else
									{
										l_strTranslated[j++]=BarCodeString[i];
									}
								}
								l_strTranslated[j] = 0; // Terminate String...
								l_strTranslated.resize(j);
								BarCodeString = l_strTranslated;
								break;
							}
							default:
							{
								break;
							}
						}
						msv_szBarCodeValue.SetValue(BarCodeString);
						if (!Result)
						{
							if (nullptr != pErrorMessages)
							{	//set GUId of this object to the error message without a GUID. (should be error message from the matrox interface)
								for (SvStl::MessageContainerVector::iterator iter = pErrorMessages->begin(); pErrorMessages->end() != iter; iter++ )
								{
									if (SV_GUID_NULL == iter->getObjectId())
									{
										iter->setObjectId(GetUniqueObjectID());
									}
								}
							}
							SetInvalid ();
							rRunStatus.SetInvalid ();
						}
						return Result;
					}
					case SVBCStatusCRCFailed:
					case SVBCStatusECCUnknown:
					case SVBCStatusENCUnknown:
					case SVBCStatusNotFound:
					default:
					{
						// Clear The Raw Data if no read.
						msv_RawData.SetResultSize(0);
						break;
					}
				}
				SetInvalid();
				if( bWarnOnFailedRead )
				{
					rRunStatus.SetWarned();
				}
				else
				{
					rRunStatus.SetFailed();
				}
				pResult->m_dReadScore.SetValue(0.0);
				return true;
			}// end try
			catch( ... )
			{
				if (nullptr != pErrorMessages)
				{
					SvStl::MessageContainer Msg( SVMSG_SVO_92_GENERAL_ERROR, SvStl::Tid_UnknownException, SvStl::SourceFileParams(StdMessageParams), 0, GetUniqueObjectID() );
					pErrorMessages->push_back(Msg);
				}
			}// end catch
		}
	}
	SetInvalid ();
	rRunStatus.SetInvalid ();
	return false;
}


bool SVBarCodeAnalyzerClass::LoadRegExpression( bool DisplayErrorMessage, SvStl::MessageContainerVector *pErrorMessages )
{
	BOOL bSaveStringInFile;

	msv_bSaveStringInFile.GetValue(bSaveStringInFile);
	if (bSaveStringInFile)
	{
		TRY
		{
			SVString FileName;
			DWORD dwFileLen;
			SVString RegExp;

			msv_szStringFileName.GetValue(FileName);
			CFile fRegExp (FileName.c_str(), CFile::modeRead);

			dwFileLen = static_cast<DWORD>(fRegExp.GetLength());
			RegExp.resize(dwFileLen +1);
			fRegExp.Read( &RegExp[0], dwFileLen);
			msv_szRegExpressionValue.SetValue(RegExp);
			fRegExp.Close();

			msv_szRegExpressionValue.GetValue(m_RegExpressionValue);
		    msv_szStringFileName.GetValue( m_StringFileName );
		}
		CATCH (CFileException, e)
		{
#if defined (TRACE_THEM_ALL) || defined (TRACE_FAILURE)
			TRACE( "Exception : File = %s -- Line = %d\n", _T(__FILE__), __LINE__ );
#endif
			SvStl::MessageContainer message;
			message.setMessage(SVMSG_SVO_93_GENERAL_WARNING, SvStl::Tid_BarCode_UnableToRead, SvStl::SourceFileParams(StdMessageParams), SvStl::Err_10038, GetUniqueObjectID());
			if (nullptr != pErrorMessages)
			{
				pErrorMessages->push_back(message);
			}
			if( DisplayErrorMessage )
			{
				SvStl::MessageMgrStd Msg( SvStl::LogAndDisplay );
				Msg.setMessage( message.getMessage() ); 
			}
			return false;
		}
		END_CATCH
	}

  return true;
}

bool SVBarCodeAnalyzerClass::SaveRegExpression( SvStl::MessageContainerVector *pErrorMessage )
{
	BOOL bSaveStringInFile;
	
	msv_bSaveStringInFile.GetValue(bSaveStringInFile);
	if (bSaveStringInFile)
	{
		TRY
		{
			SVString FileName;
			DWORD dwFileLen;
			SVString RegExp;
			
			msv_szRegExpressionValue.GetValue(RegExp);
			msv_szStringFileName.GetValue(FileName);
			
			CFile fRegExp( FileName.c_str(), CFile::modeCreate | CFile::modeWrite | CFile::shareDenyNone );
			
			dwFileLen = static_cast<DWORD> ( RegExp.size() );
			fRegExp.Write( RegExp.c_str(), dwFileLen );
			
			fRegExp.Close();

			msv_szRegExpressionValue.GetValue(m_RegExpressionValue);
			msv_szStringFileName.GetValue(m_StringFileName);
		}
		CATCH (CFileException, e)
		{
			if (nullptr != pErrorMessage)
			{
				SvStl::MessageContainer message(SVMSG_SVO_93_GENERAL_WARNING, SvStl::Tid_BarCode_UnableToSave, SvStl::SourceFileParams(StdMessageParams), SvStl::Err_10039, GetUniqueObjectID());
				pErrorMessage->push_back(message);
			}
			return false;
		}
		END_CATCH
	}
	return true;
}

bool SVBarCodeAnalyzerClass::ResetObject(SvStl::MessageContainerVector *pErrorMessages)
{
	bool Result = __super::ResetObject(pErrorMessages);

	if ( !m_bHasLicenseError )
	{
		if ( InitMil(pErrorMessages) )
		{
			SVString TempRegExpressionValue;
			SVString TempStringFileName;

			msv_szRegExpressionValue.GetValue( TempRegExpressionValue );
			msv_szStringFileName.GetValue( TempStringFileName );
			if( TempRegExpressionValue != m_RegExpressionValue || TempStringFileName != m_StringFileName )
			{
				Result = SaveRegExpression(pErrorMessages) && Result;
			}

			Result = LoadRegExpression( false, pErrorMessages ) && Result;
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
	}

	return Result;
}

