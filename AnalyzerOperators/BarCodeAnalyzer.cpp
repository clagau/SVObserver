//******************************************************************************
//* COPYRIGHT (c) 2003 by Körber Pharma Inspection GmbH. All Rights Reserved
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : BarCodeAnalyzer
//* .File Name       : $Workfile:   BarCodeAnalyzer.cpp  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.11  $
//* .Check In Date   : $Date:   19 Dec 2014 03:59:30  $
//******************************************************************************

#pragma region Includes
#include "stdafx.h"
//Moved to precompiled header: #include <algorithm>
#include "BarCodeAnalyzer.h"
#include "ObjectInterfaces/IBarCode.h"
#include "SVLibrary/SVOIniClass.h"
#include "Operators/SVBarCodeResult.h"   
#include "InspectionEngine/SVBarCodeBuffer.h"
#include "InspectionEngine/SVImageProcessingClass.h"
#include "InspectionEngine/SVImageClass.h"
#include "Definitions/SVMatroxSimpleEnums.h"
#include "SVMatroxLibrary/SVOLicenseManager.h"
#include "SVMatroxLibrary/SVMatroxBuffer.h"
#include "SVMatroxLibrary/SVMatroxBarCodeInterface.h"
#include "SVStatusLibrary/GlobalPath.h"
#include "SVStatusLibrary/MessageManager.h"
#include "SVStatusLibrary/MessageTextEnum.h"
#include "InspectionEngine/RunStatus.h"
#include "SVUtilityLibrary/StringHelper.h"
#pragma endregion Includes

namespace SvAo
{

#pragma region Declarations
#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif
#pragma endregion Declarations

SV_IMPLEMENT_CLASS (BarCodeAnalyzer, SvPb::BarCodeAnalyzerClassId);

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

BarCodeAnalyzer::BarCodeAnalyzer (SVObjectClass* POwner, int StringResourceID)
	: ImageAnalyzer (POwner, StringResourceID)
{
	if (SVOLicenseManager::Instance().HasMatroxIdentificationLicense())
	{
		m_bHasLicenseError = false;
	}
	else
	{
		m_bHasLicenseError = true;
	}

	init ();

}

void BarCodeAnalyzer::init()
{
	m_ObjectTypeInfo.m_SubType = SvPb::SVBarCodeAnalyzerObjectType;
	
	// Register Embedded Objects
	RegisterEmbeddedObject (&msv_szBarCodeValue, SvPb::BarCodeEId, IDS_OBJECTNAME_BARCODEVALUE, false, SvOi::SVResetItemNone, false);
	RegisterEmbeddedObject (&msv_szRegExpressionValue, SvPb::RegExpressionEId, IDS_OBJECTNAME_CONTROLVALUE, false, SvOi::SVResetItemOwner, true);
	RegisterEmbeddedObject (&msv_lBarCodeType, SvPb::BCTypeEId, IDS_OBJECTNAME_BARCODETYPE, false, SvOi::SVResetItemOwner, true);
	RegisterEmbeddedObject (&msv_dOrientation, SvPb::BCOrientationEId, IDS_OBJECTNAME_ORIENTATION, false, SvOi::SVResetItemOwner, true);
	RegisterEmbeddedObject (&msv_dSkewNegative, SvPb::BCSkewNegativeEId, IDS_OBJECTNAME_SKEWNEGATIVE, false, SvOi::SVResetItemOwner, true);
	RegisterEmbeddedObject (&msv_dSkewPositive, SvPb::BCSkewPositiveEId, IDS_OBJECTNAME_SKEWPOSITIVE, false, SvOi::SVResetItemOwner, true);
	RegisterEmbeddedObject (&msv_dCellMinSize, SvPb::BCCellMinSizeEId, IDS_OBJECTNAME_CELLMINSIZE, false, SvOi::SVResetItemOwner, true);
	RegisterEmbeddedObject (&msv_dCellMaxSize, SvPb::BCCellMaxSizeEId, IDS_OBJECTNAME_CELLMAXSIZE, false, SvOi::SVResetItemOwner, true);
	RegisterEmbeddedObject (&msv_dCellNumberX, SvPb::BCCellNumberXEId, IDS_OBJECTNAME_CELLNUMBERX, false, SvOi::SVResetItemOwner, true);
	RegisterEmbeddedObject (&msv_dCellNumberY, SvPb::BCCellNumberYEId, IDS_OBJECTNAME_CELLNUMBERY, false, SvOi::SVResetItemOwner, true);
	RegisterEmbeddedObject (&msv_dSpeed, SvPb::BCSpeedEId, IDS_OBJECTNAME_SPEED, false, SvOi::SVResetItemOwner, true);
	RegisterEmbeddedObject (&msv_dThreshold, SvPb::BCThresholdEId, IDS_OBJECTNAME_THRESHOLD, false, SvOi::SVResetItemOwner, true);
	RegisterEmbeddedObject (&msv_dStringSize, SvPb::BCStringSizeEId, IDS_OBJECTNAME_STRINGSIZE, false, SvOi::SVResetItemOwner, true);
	RegisterEmbeddedObject (&msv_dErrorCorrection, SvPb::BCErrorCorrectionEId, IDS_OBJECTNAME_ERRORCORRECTION, false, SvOi::SVResetItemOwner, true);
	RegisterEmbeddedObject (&msv_dEncoding, SvPb::BCEncodingEId, IDS_OBJECTNAME_ENCODING, false, SvOi::SVResetItemOwner, true);
	RegisterEmbeddedObject (&msv_dForegroundColor, SvPb::BCForegroundColorEId, IDS_OBJECTNAME_FOREGROUNDCOLOR, false, SvOi::SVResetItemOwner, true);
	RegisterEmbeddedObject (&msv_bSaveStringInFile, SvPb::BCSaveStringInFileEId, IDS_OBJECTNAME_SAVESTRINGINFILE, false, SvOi::SVResetItemOwner, true);
	RegisterEmbeddedObject (&msv_szStringFileName, SvPb::BCStringFileNameEId, IDS_OBJECTNAME_STRINGFILENAME, false, SvOi::SVResetItemOwner, true);
	RegisterEmbeddedObject (&m_bWarnOnFailedRead, SvPb::BCWarnOnFailedReadEId, IDS_OBJECTNAME_WARNONFAILEDREAD, false, SvOi::SVResetItemOwner, true);
	RegisterEmbeddedObject (&msv_lBarcodeTimeout, SvPb::BCTimeoutEId, IDS_OBJECTNAME_BARCODETIMEOUT, false, SvOi::SVResetItemOwner, true);
	// IDS_OBJECTNAME_BARCODEUNEVENGRID
	RegisterEmbeddedObject (&msv_bUnEvenGrid, SvPb::BCUnevenGridEId,IDS_OBJECTNAME_BARCODEUNEVENGRID, false, SvOi::SVResetItemOwner, true);

	// To support special DMCs May 2008.
	RegisterEmbeddedObject (&msv_RawData, SvPb::BCRawDataEId, IDS_OBJECTNAME_BARCODERAWDATA, false, SvOi::SVResetItemOwner, false);
	RegisterEmbeddedObject (&msv_eStringFormat, SvPb::BCStringFormatEId, IDS_OBJECTNAME_BARCODESTRINGFORMAT, false, SvOi::SVResetItemOwner, true);
	RegisterEmbeddedObject (&msv_lThresholdType, SvPb::BCThresholdTypeEId, IDS_OBJECTNAME_BARCODETHRESHOLDTYPE, false, SvOi::SVResetItemOwner, true);

	// Set default values
	msv_szBarCodeValue.SetDefaultValue (_T(""));
	msv_szBarCodeValue.setSaveValueFlag(false);
	msv_szBarCodeValue.SetMaxByteSize();
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
	msv_dErrorCorrection.SetDefaultValue(SVValueDefault);
	msv_dEncoding.SetDefaultValue(SVValueDefault);
	msv_dForegroundColor.SetDefaultValue(SVValueForegroundBlack);
	msv_bSaveStringInFile.SetDefaultValue( BOOL(false));
	msv_szStringFileName.SetDefaultValue(_T("BCRegExp.txt"));
	m_bWarnOnFailedRead.SetDefaultValue( BOOL(false));

	msv_bUnEvenGrid.SetDefaultValue( BOOL(false));

	// To support special DMCs May 2008.
	// Use ~ to simulate non printable characters...
	std::string FormatEnum = SvUl::Format(_T("Remove GS1 Control Characters=%d,Translate GS1 Control Characters=%d,Replace GS1 Control Characters=%d"), 
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

	// Instantiate Children
	SvOp::SVBarCodeResult* pAnalyzerResult = new SvOp::SVBarCodeResult (this, IDS_CLASSNAME_SVBARCODEANALYZERRESULT);
	
	if (nullptr != pAnalyzerResult)
	{
		Add(pAnalyzerResult);
	}
}

bool BarCodeAnalyzer::InitMil (SvStl::MessageContainerVector *pErrorMessages)
{
	CloseMil();
	
	SvIe::SVImageClass *pInputImage = getInputImage();
	if (nullptr != pInputImage)
	{
		SvIe::SVBarCodeBufferInfoClass svData;
		svData.HBuffer.milBarCode = m_MilCodeId;

		long type;
		msv_lBarCodeType.GetValue(type);
		//check if ecc and enc correct to type
		double encValue = 0;
		msv_dEncoding.GetValue(encValue);
		double eccValue = 0;
		msv_dErrorCorrection.GetValue(eccValue);
		if (!checkEccAndEncValues(type, eccValue, encValue, pErrorMessages))
		{
			return false;
		}

		svData.m_lCodeType = static_cast<SVBarCodeTypesEnum>(type);
		if ( S_OK == SvIe::SVImageProcessingClass::CreateBarCodeBuffer( &svData ) )
		{
			m_MilCodeId = svData.HBuffer.milBarCode;

			// get M_TIMEOUT value from Registry
			//int l_mTimeout = AfxGetApp()->GetProfileInt(_T( "Settings" ),_T( "MILTimeout" ), 2000 );

			long l_mTimeout;
			msv_lBarcodeTimeout.GetValue(l_mTimeout);
			double dL1 = (double) l_mTimeout;

			double dParm = 0.;
			/*HRESULT MatroxCode = */SVMatroxBarCodeInterface::Set( m_MilCodeId, SVBCCodeTimeout, dL1 );
			msv_dOrientation.GetValue (dParm);
			/*MatroxCode = */SVMatroxBarCodeInterface::Set( m_MilCodeId, SVBCSearchAngle, dParm );

			msv_dSkewNegative.GetValue (dParm);
			/*MatroxCode = */SVMatroxBarCodeInterface::Set( m_MilCodeId, SVBCSearchAngleDeltaNeg, dParm);

			msv_dSkewPositive.GetValue (dParm);
			/*MatroxCode = */SVMatroxBarCodeInterface::Set( m_MilCodeId, SVBCSearchAngleDeltaPos, dParm);

			msv_dCellMinSize.GetValue (dParm);
			/*MatroxCode = */SVMatroxBarCodeInterface::Set( m_MilCodeId, SVBCCellSizeMin, dParm);

			msv_dCellMaxSize.GetValue (dParm);
			/*MatroxCode = */SVMatroxBarCodeInterface::Set( m_MilCodeId, SVBCCellSizeMax, dParm);

			msv_dCellNumberX.GetValue (dParm);
			/*MatroxCode = */SVMatroxBarCodeInterface::Set( m_MilCodeId, SVBCCellNumberX, dParm);

			msv_dCellNumberY.GetValue (dParm);
			/*MatroxCode = */SVMatroxBarCodeInterface::Set( m_MilCodeId, SVBCCellNumberY, dParm);

			msv_dSpeed.GetValue (dParm);
			/*MatroxCode = */SVMatroxBarCodeInterface::Set( m_MilCodeId, SVBCSpeed, dParm);

			// Threshold
			long l_lThresholdType ;
			msv_lThresholdType.GetValue( l_lThresholdType );
			switch( l_lThresholdType )
			{
				case 0:
				{
					msv_dThreshold.GetValue (dParm);
					/*MatroxCode = */SVMatroxBarCodeInterface::Set( m_MilCodeId, SVBCThreshold, dParm);
					break;
				}
				case 1:	// Auto Threshold
				{
					dParm = 0;
					/*MatroxCode = */SVMatroxBarCodeInterface::Set( m_MilCodeId, SVBCAutoThreshold, dParm );
					break;
				}
				case 2:	// Adaptive Threshold
				{
					dParm = 0;
					/*MatroxCode = */SVMatroxBarCodeInterface::Set( m_MilCodeId, SVBCAdaptiveThreshold, dParm );
					break;
				}
			}

			msv_dStringSize.GetValue (dParm);
			if ( dParm == (double) SVValueDefault ) // M_DEFAULT )
			{
				dParm = (double) SVValueAny;		// M_ANY;
			}
			/*MatroxCode = */SVMatroxBarCodeInterface::Set( m_MilCodeId, SVBCBarcodeStringSize, dParm);

			/*MatroxCode = */SVMatroxBarCodeInterface::Set( m_MilCodeId, SVBCEncoding, encValue);
			/*MatroxCode = */SVMatroxBarCodeInterface::Set( m_MilCodeId, SVBCErrorCorrection, eccValue);

			msv_dForegroundColor.GetValue (dParm);
			/*MatroxCode = */SVMatroxBarCodeInterface::Set( m_MilCodeId, SVBCForeGroundValue, dParm);

			BOOL bUnevenGrid;
			msv_bUnEvenGrid.GetValue(bUnevenGrid);
			/*MatroxCode = */SVMatroxBarCodeInterface::Set( m_MilCodeId, SVBCUnEvenGrid, (long)bUnevenGrid );
			return true;
		}
		else
		{
			// McodeAlloc failed
			if (nullptr != pErrorMessages)
			{
				SvStl::MessageContainer Msg( SVMSG_SVO_92_GENERAL_ERROR, SvStl::Tid_MilBarCodeInitFailed, SvStl::SourceFileParams(StdMessageParams), getObjectId() );
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
			SvStl::MessageContainer Msg( SVMSG_SVO_92_GENERAL_ERROR, SvStl::Tid_NoSourceImage, SvStl::SourceFileParams(StdMessageParams), getObjectId() );
			pErrorMessages->push_back(Msg);
		}
		return false;
	}
}

void BarCodeAnalyzer::CloseMil ()
{
	if ( m_bHasLicenseError )
	{
		return;
	}
	SVMatroxBarCodeInterface::Destroy( m_MilCodeId );
}

bool BarCodeAnalyzer::CreateObject(const SVObjectLevelCreateStruct& rCreateStructure)
{
	if ( !m_bHasLicenseError )
	{
		if (ImageAnalyzer::CreateObject (rCreateStructure))
		{
			if (InitMil ())
			{
				LoadRegExpression();
			}
		}
	}

	// *** Set/Reset Printable Attributes ***
	msv_szBarCodeValue.SetObjectAttributesAllowed( SvPb::audittrail, SvOi::SetAttributeType::RemoveAttribute );
	msv_szRegExpressionValue.SetObjectAttributesAllowed( SvPb::audittrail, SvOi::SetAttributeType::AddAttribute );
	msv_lBarCodeType.SetObjectAttributesAllowed( SvPb::audittrail, SvOi::SetAttributeType::AddAttribute );
	msv_dOrientation.SetObjectAttributesAllowed( SvPb::audittrail, SvOi::SetAttributeType::AddAttribute );
	msv_dSkewNegative.SetObjectAttributesAllowed( SvPb::audittrail, SvOi::SetAttributeType::AddAttribute );
	msv_dSkewPositive.SetObjectAttributesAllowed( SvPb::audittrail, SvOi::SetAttributeType::AddAttribute );
	msv_dCellMinSize.SetObjectAttributesAllowed( SvPb::audittrail, SvOi::SetAttributeType::AddAttribute );
	msv_dCellMaxSize.SetObjectAttributesAllowed( SvPb::audittrail, SvOi::SetAttributeType::AddAttribute );
	msv_dCellNumberX.SetObjectAttributesAllowed( SvPb::audittrail, SvOi::SetAttributeType::AddAttribute );
	msv_dCellNumberY.SetObjectAttributesAllowed( SvPb::audittrail, SvOi::SetAttributeType::AddAttribute );
	msv_dSpeed.SetObjectAttributesAllowed( SvPb::audittrail | SvPb::remotelySetable, SvOi::SetAttributeType::AddAttribute );
	msv_dThreshold.SetObjectAttributesAllowed( SvPb::audittrail, SvOi::SetAttributeType::AddAttribute );
	msv_dStringSize.SetObjectAttributesAllowed( SvPb::audittrail, SvOi::SetAttributeType::AddAttribute );
	msv_dErrorCorrection.SetObjectAttributesAllowed( SvPb::audittrail, SvOi::SetAttributeType::AddAttribute );
	msv_dEncoding.SetObjectAttributesAllowed( SvPb::audittrail, SvOi::SetAttributeType::AddAttribute );
	msv_dForegroundColor.SetObjectAttributesAllowed( SvPb::audittrail, SvOi::SetAttributeType::AddAttribute );
	msv_bSaveStringInFile.SetObjectAttributesAllowed( SvPb::audittrail, SvOi::SetAttributeType::AddAttribute );
	msv_szStringFileName.SetObjectAttributesAllowed( SvPb::audittrail, SvOi::SetAttributeType::AddAttribute );
	m_bWarnOnFailedRead.SetObjectAttributesAllowed( SvPb::audittrail | SvPb::remotelySetable, SvOi::SetAttributeType::AddAttribute );
	msv_lBarcodeTimeout.SetObjectAttributesAllowed( SvPb::audittrail | SvPb::remotelySetable, SvOi::SetAttributeType::AddAttribute );
	msv_RawData.SetObjectAttributesAllowed( SvPb::audittrail, SvOi::SetAttributeType::RemoveAttribute );
	m_isCreated = true;

	if ( m_bHasLicenseError )
	{
		SVOLicenseManager::Instance().AddLicenseErrorToList(GetCompleteName());
	}

	return true;
}

BarCodeAnalyzer::~BarCodeAnalyzer()
{
	if ( m_bHasLicenseError )
	{
		m_bHasLicenseError = false;
		SVOLicenseManager::Instance().RemoveLicenseErrorFromList(GetCompleteName());
	}

	BarCodeAnalyzer::CloseObject();
}

bool BarCodeAnalyzer::CloseObject()
{
	CloseMil ();
	return ImageAnalyzer::CloseObject();
}

bool BarCodeAnalyzer::CharIsControl( TCHAR p_Char )
{
	return p_Char < ' ' || p_Char > 126;
}

bool BarCodeAnalyzer::onRun (SvIe::RunStatus &rRunStatus, SvStl::MessageContainerVector *pErrorMessages)
{
	if ( m_bHasLicenseError )
	{
		if (nullptr != pErrorMessages)
		{
			SvStl::MessageContainer Msg( SVMSG_SVO_92_GENERAL_ERROR, SvStl::Tid_SVObserver_MatroxLicenseNotFound, SvStl::SourceFileParams(StdMessageParams), getObjectId() );
			pErrorMessages->push_back(Msg);
		}
		rRunStatus.SetInvalid ();
		return false;
	}
	
	msv_szBarCodeValue.SetValue(std::string());
	if (ImageAnalyzer::onRun (rRunStatus, pErrorMessages))
	{
		if (nullptr == m_pBarCodeResult)
		{
			if (nullptr != pErrorMessages)
			{
				SvStl::MessageContainer Msg(SVMSG_SVO_92_GENERAL_ERROR, SvStl::Tid_Error_NoResultObject, SvStl::SourceFileParams(StdMessageParams), getObjectId());
				pErrorMessages->push_back(Msg);
			}
			return false;
		}

		BOOL bWarnOnFailedRead;
		m_bWarnOnFailedRead.GetValue(bWarnOnFailedRead); // Add value from checkbox instead of true to warn on fail.
									// default value will be false.
		m_pBarCodeResult->m_bFailedToRead = bWarnOnFailedRead != FALSE;// Preset flag to failed condition..

		SvIe::SVImageClass* pInputImage = getInputImage(true);
		SvOi::ITRCImagePtr pImageBuffer = pInputImage->getImageReadOnly(rRunStatus.m_triggerRecord.get());
		if (nullptr != pImageBuffer && !pImageBuffer->isEmpty())
		{
			try
			{
				SVMatroxBuffer ImageBufId = pImageBuffer->getHandle()->GetBuffer();

				bool Result = SVMatroxBarCodeInterface::Execute( m_MilCodeId, ImageBufId, pErrorMessages );

				long lMilResult = SVBCStatusNotFound;
				Result = Result && SVMatroxBarCodeInterface::GetResult( m_MilCodeId, SVBCBarCodeStatus, lMilResult, pErrorMessages );

				if (!Result)
				{
					if (nullptr != pErrorMessages)
					{
						for (SvStl::MessageContainer& rMessage : *pErrorMessages)
						{
							if (SvDef::InvalidObjectId == rMessage.getObjectId())
							{
								rMessage.setObjectId(getObjectId());
							}
						}
					}
					rRunStatus.SetInvalid ();
					return false;
				}

				switch ( lMilResult )
				{
					case SVBCStatusReadOK:
					{
						m_pBarCodeResult->m_bFailedToRead = false;
						long cbBarCodeValue = 0;
						Result &= SVMatroxBarCodeInterface::GetResult( m_MilCodeId, SVBCBarcodeStringSize, cbBarCodeValue, pErrorMessages );
						std::string BarCodeString;
						Result &= SVMatroxBarCodeInterface::GetResult( m_MilCodeId, SVBCBarCodeString, BarCodeString, pErrorMessages );
						double dScore = 0.0;
						Result &= SVMatroxBarCodeInterface::GetResult( m_MilCodeId, SVBCBarcodeScore, dScore, pErrorMessages );

						m_pBarCodeResult->m_dReadScore.SetValue(dScore * 100);

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
						msv_RawData.SetResultSize(static_cast<int32_t> (BarCodeString.size()));
						if(S_OK != msv_RawData.SetArrayValues(BarCodeValue))
						{
							Result = false;
							if (nullptr != pErrorMessages)
							{
								SvStl::MessageContainer Msg( SVMSG_SVO_92_GENERAL_ERROR, SvStl::Tid_SetValueFailed, SvStl::SourceFileParams(StdMessageParams), getObjectId() );
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
								std::string l_strRemoved;
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
								std::string l_strTranslated;
								l_strTranslated.resize( BarCodeString.size()+256);
								int j = 0;
								for( size_t i = 0 ; i < BarCodeString.size(); i++ )
								{
									if( CharIsControl(BarCodeString[i]) )
									{
										std::string Temp = SvUl::Format(_T("%03d"),BarCodeString[i]);
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
							{	//set Id of this object to the error message without a ID. (should be error message from the matrox interface)
								for (SvStl::MessageContainer& rMessage : *pErrorMessages)
								{
									if (SvDef::InvalidObjectId == rMessage.getObjectId())
									{
										rMessage.setObjectId(getObjectId());
									}
								}
							}
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
				if( bWarnOnFailedRead )
				{
					rRunStatus.SetWarned();
				}
				else
				{
					rRunStatus.SetFailed();
				}
				m_pBarCodeResult->m_dReadScore.SetValue(0.0);
				return true;
			}// end try
			catch( ... )
			{
				if (nullptr != pErrorMessages)
				{
					SvStl::MessageContainer Msg( SVMSG_SVO_92_GENERAL_ERROR, SvStl::Tid_UnknownException, SvStl::SourceFileParams(StdMessageParams), getObjectId() );
					pErrorMessages->push_back(Msg);
				}
			}// end catch
		}
	}
	rRunStatus.SetInvalid ();
	return false;
}


bool BarCodeAnalyzer::LoadRegExpression( bool DisplayErrorMessage, SvStl::MessageContainerVector *pErrorMessages )
{
	BOOL bSaveStringInFile;

	msv_bSaveStringInFile.GetValue(bSaveStringInFile);
	if (bSaveStringInFile)
	{
		try
		{
			std::string FileName;
			DWORD dwFileLen;
			std::string RegExp;

			msv_szStringFileName.GetValue(FileName);
			std::ifstream fileRegExp{FileName.c_str(), std::ifstream::in | std::ifstream::binary | std::ifstream::ate};

			dwFileLen = static_cast<unsigned long> (fileRegExp.tellg());
			fileRegExp.seekg(0, std::ios::beg);

			RegExp.resize(dwFileLen + 1);
			fileRegExp.read( &RegExp[0], dwFileLen);
			fileRegExp.close();
			msv_szRegExpressionValue.SetValue(RegExp);

			msv_szRegExpressionValue.GetValue(m_RegExpressionValue);
		    msv_szStringFileName.GetValue( m_StringFileName );
		}
		catch (...)
		{
			SvStl::MessageContainer message;
			message.setMessage(SVMSG_SVO_93_GENERAL_WARNING, SvStl::Tid_BarCode_UnableToRead, SvStl::SourceFileParams(StdMessageParams), getObjectId());
			if (nullptr != pErrorMessages)
			{
				pErrorMessages->push_back(message);
			}
			if( DisplayErrorMessage )
			{
				SvStl::MessageManager Msg(SvStl::MsgType::Log | SvStl::MsgType::Display );
				Msg.setMessage( message.getMessage() ); 
			}
			return false;
		}
	}

  return true;
}

bool BarCodeAnalyzer::SaveRegExpression( SvStl::MessageContainerVector *pErrorMessage )
{
	BOOL bSaveStringInFile;
	
	msv_bSaveStringInFile.GetValue(bSaveStringInFile);
	if (bSaveStringInFile)
	{
		try
		{
			std::string FileName;
			std::string RegExp;
			
			msv_szRegExpressionValue.GetValue(RegExp);
			msv_szStringFileName.GetValue(FileName);
			
			std::ofstream fileRegExp {FileName.c_str(), std::ofstream::out | std::ofstream::trunc};
			if(fileRegExp.is_open())
			{
				fileRegExp.write(RegExp.c_str(), RegExp.size());
				fileRegExp.close();
			}

			msv_szRegExpressionValue.GetValue(m_RegExpressionValue);
			msv_szStringFileName.GetValue(m_StringFileName);
		}
		catch(...)
		{
			if (nullptr != pErrorMessage)
			{
				SvStl::MessageContainer message(SVMSG_SVO_93_GENERAL_WARNING, SvStl::Tid_BarCode_UnableToSave, SvStl::SourceFileParams(StdMessageParams), getObjectId());
				pErrorMessage->push_back(message);
			}
			return false;
		}
	}
	return true;
}

bool BarCodeAnalyzer::ResetObject(SvStl::MessageContainerVector *pErrorMessages)
{
	bool Result = __super::ResetObject(pErrorMessages);

	//Get the pointer on reset
	m_pBarCodeResult = dynamic_cast<SvOp::SVBarCodeResult*> (GetResultObject());
	if(!Result || nullptr == m_pBarCodeResult)
	{
		Result = false;
		if (nullptr != pErrorMessages)
		{
			SvStl::MessageContainer Msg(SVMSG_SVO_92_GENERAL_ERROR, SvStl::Tid_Error_NoResultObject, SvStl::SourceFileParams(StdMessageParams), getObjectId());
			pErrorMessages->push_back(Msg);
		}
	}
	else
	{
		if (!m_bHasLicenseError)
		{
			if (InitMil(pErrorMessages))
			{
				std::string TempRegExpressionValue;
				std::string TempStringFileName;

				msv_szRegExpressionValue.GetValue(TempRegExpressionValue);
				msv_szStringFileName.GetValue(TempStringFileName);
				if (TempRegExpressionValue != m_RegExpressionValue || TempStringFileName != m_StringFileName)
				{
					Result = SaveRegExpression(pErrorMessages) && Result;
				}

				Result = LoadRegExpression(false, pErrorMessages) && Result;
			}
		}
		else
		{
			Result = false;
			if (nullptr != pErrorMessages)
			{
				SvStl::MessageContainer Msg( SVMSG_SVO_92_GENERAL_ERROR, SvStl::Tid_SVObserver_MatroxLicenseNotFound, SvStl::SourceFileParams(StdMessageParams), getObjectId() );
				pErrorMessages->push_back(Msg);
			}
		}
	}

	return Result;
}

bool BarCodeAnalyzer::checkEccAndEncValues(long type, double eccValue, double encValue, SvStl::MessageContainerVector* pErrorMessages)
{
	const auto& rTypeInfoMessage = SvOi::getBarCodeTypeInfo();
	auto iter = std::find_if(rTypeInfoMessage.typeparameters().cbegin(), rTypeInfoMessage.typeparameters().cend(), [type](const auto& rEntry) { return rEntry.type() == type; });
	if (iter != rTypeInfoMessage.typeparameters().cend())
	{
		if (SVValueDefault != static_cast<SVMatroxBufferValues> (eccValue))
		{
			auto eccIter = std::find_if(iter->possibleecctypes().cbegin(), iter->possibleecctypes().cend(), [eccValue](const auto& rEntry) { return rEntry == eccValue; });
			if (eccIter == iter->possibleecctypes().cend())
			{
				if (nullptr != pErrorMessages)
				{
					SvStl::MessageContainer Msg(SVMSG_SVO_92_GENERAL_ERROR, SvStl::Tid_MilBarCodeECCNotFit, SvStl::SourceFileParams(StdMessageParams), getObjectId());
					pErrorMessages->push_back(Msg);
				}
				return false;
			}
		}
		if (SVValueDefault != static_cast<SVMatroxBufferValues> (encValue))
		{
			auto encIter = std::find_if(iter->possibleenctypes().cbegin(), iter->possibleenctypes().cend(), [encValue](const auto& rEntry) { return rEntry == encValue; });
			if (encIter == iter->possibleenctypes().cend())
			{
				if (nullptr != pErrorMessages)
				{
					SvStl::MessageContainer Msg(SVMSG_SVO_92_GENERAL_ERROR, SvStl::Tid_MilBarCodeENCNotFit, SvStl::SourceFileParams(StdMessageParams), getObjectId());
					pErrorMessages->push_back(Msg);
				}
				return false;
			}
		}
	}
	else
	{
		if (nullptr != pErrorMessages)
		{
			SvStl::MessageContainer Msg(SVMSG_SVO_92_GENERAL_ERROR, SvStl::Tid_MilBarCodeUnknowType, SvStl::SourceFileParams(StdMessageParams), getObjectId());
			pErrorMessages->push_back(Msg);
		}
		return false;
	}
	return true;
}

} //namespace SvAo

SvPb::GetBarCodeTypeInfosResponse SvOi::getBarCodeTypeInfo()
{
	static SvPb::GetBarCodeTypeInfosResponse response;
	if (0 == response.eccnames_size())
	{
		std::array <std::pair<long, std::string>, 21> tmpArray = { 
			std::pair<long, std::string>{SVValueEccNone, "None" },
			std::pair<long, std::string>{SVValueAny, "Any" },
			std::pair<long, std::string>{SVValueEcc200, "ECC-200" },
			std::pair<long, std::string>{SVValueEccCheckDigit, "Check Digit" },
			std::pair<long, std::string>{SVValueEccReedSolomon, "Reed-Solomon" },
			std::pair<long, std::string>{SVValueEccReedSolomon1, "Reed-Solomon 1" },
			std::pair<long, std::string>{SVValueEccReedSolomon2, "Reed-Solomon 2" },
			std::pair<long, std::string>{SVValueEccReedSolomon3, "Reed-Solomon 3" },
			std::pair<long, std::string>{SVValueEccReedSolomon4, "Reed-Solomon 4" },
			std::pair<long, std::string>{SVValueEccReedSolomon5, "Reed-Solomon 5" },
			std::pair<long, std::string>{SVValueEccReedSolomon6, "Reed-Solomon 6" },
			std::pair<long, std::string>{SVValueEccReedSolomon7, "Reed-Solomon 7" },
			std::pair<long, std::string>{SVValueEccReedSolomon8, "Reed-Solomon 8" },
			std::pair<long, std::string>{SVValueEccH, "ECC-H" },
			std::pair<long, std::string>{SVValueEccL, "ECC-L" },
			std::pair<long, std::string>{SVValueEccM, "ECC-M" },
			std::pair<long, std::string>{SVValueEccQ, "ECC-Q" }
		};

		for (const auto& rValue : tmpArray)
		{
			auto* pPair = response.add_eccnames();
			pPair->set_value(rValue.first);
			pPair->set_name(rValue.second);
		}
	}
	
	if (0 == response.encnames_size())
	{
		std::array <std::pair<long, std::string>, 24> tmpArray = {
			std::pair<long, std::string>{SVValueAny, "Any" },
			std::pair<long, std::string>{SVValueEncStandard, "Standard" },
			std::pair<long, std::string>{SVValueEncNum, "Numeric" },
			std::pair<long, std::string>{SVValueEncUpcAAddOn, "UpcA AddOn" },
			std::pair<long, std::string>{SVValueEncUpcEAddOn, "UpcE AddOn" },
			std::pair<long, std::string>{SVValueENCAlpha, "Alpha" },
			std::pair<long, std::string>{SVValueENCAlphaNum, "Alpha-Numeric" },
			std::pair<long, std::string>{SVValueENCAlphaNumPunc, "Alpha-Numeric with Punctuation" },
			std::pair<long, std::string>{SVValueENCAscii, "ASCII" },
			std::pair<long, std::string>{SVValueENCIso8, "ISO-8" },
			std::pair<long, std::string>{SVValueENCMode2, "Mode 2" },
			std::pair<long, std::string>{SVValueENCMode3, "Mode 3" },
			std::pair<long, std::string>{SVValueENCMode4, "Mode 4" },
			std::pair<long, std::string>{SVValueENCMode5, "Mode 5" },
			std::pair<long, std::string>{SVValueENCMode6, "Mode 6" },
			std::pair<long, std::string>{SVValueENCGS1, "GS1" },
			std::pair<long, std::string>{SVValueENCGS1Trunc, "GS1 Truncated" },
			std::pair<long, std::string>{SVValueENCGS1Limited, "GS1 Limited" },
			std::pair<long, std::string>{SVValueENCGS1Expanded, "GS1 Expanded" },
			std::pair<long, std::string>{SVValueENCGS1Stacked, "GS1 Stacked" },
			std::pair<long, std::string>{SVValueENCGS1StackedOmni, "GS1 Stacked Omni" },
			std::pair<long, std::string>{SVValueENCGS1ExpandedStacked, "GS1 Expanded Stacked" },
			std::pair<long, std::string>{SVValueEncQRCodeModel1, "QRCode Model1" },
			std::pair<long, std::string>{SVValueEncQRCodeModel2, "QRCode Model2" }
		};

		for (const auto& rValue : tmpArray)
		{
			auto* pPair = response.add_encnames();
			pPair->set_value(rValue.first);
			pPair->set_name(rValue.second);
		}
	}

	if (0 == response.typeparameters_size())
	{
		std::array<std::tuple<long, std::string, std::vector<long>, long, std::vector<long>, long>, 16> arrTmp = { 
			std::make_tuple(SVDataMatrix, "Data Matrix", std::vector<long>{ SVValueEncNum, SVValueENCAlpha, SVValueENCAlphaNum, SVValueENCAlphaNumPunc, SVValueENCAscii, SVValueENCIso8, SVValueAny}, SVValueAny, 
				std::vector<long>{ SVValueEccNone, SVValueAny, SVValueEcc200 }, SVValueAny),
			std::make_tuple(SVEan13, "EAN-13", std::vector<long>{ SVValueEncNum }, SVValueEncNum, std::vector<long>{ SVValueEccCheckDigit }, SVValueEccCheckDigit),
			std::make_tuple(SVCode39, "3 of 9", std::vector<long>{ SVValueEncStandard, SVValueENCAscii }, SVValueEncStandard, std::vector<long>{ SVValueEccNone, SVValueEccCheckDigit }, SVValueEccNone),
			std::make_tuple(SVInterleaved25, "Interleaved 2 of 5", std::vector<long>{ SVValueEncNum }, SVValueEncNum, std::vector<long>{ SVValueEccNone, SVValueEccCheckDigit }, SVValueEccNone),
			std::make_tuple(SVCode128, "Code 128", std::vector<long>{ SVValueENCAscii }, SVValueENCAscii, std::vector<long>{ SVValueEccCheckDigit }, SVValueEccCheckDigit),
			std::make_tuple(SVPDF417, "PDF417", std::vector<long>{ SVValueEncStandard }, SVValueEncStandard, 
					std::vector<long>{ SVValueAny, SVValueEccReedSolomon1, SVValueEccReedSolomon2, SVValueEccReedSolomon3, SVValueEccReedSolomon4, SVValueEccReedSolomon5, SVValueEccReedSolomon6, SVValueEccReedSolomon7, SVValueEccReedSolomon8 }, SVValueAny),
			std::make_tuple(SVBC412, "BC412", std::vector<long>{ SVValueEncStandard }, SVValueEncStandard, std::vector<long>{ SVValueEccNone }, SVValueEccNone),
			std::make_tuple(SVCodeABar, "Codabar", std::vector<long>{ SVValueEncStandard }, SVValueEncStandard, std::vector<long>{ SVValueEccNone }, SVValueEccNone),
			std::make_tuple(SVMaxiCode, "Maxicode", std::vector<long>{ SVValueAny, SVValueENCMode2, SVValueENCMode3, SVValueENCMode4, SVValueENCMode5, SVValueENCMode6 }, SVValueAny, std::vector<long>{ SVValueEccReedSolomon }, SVValueEccReedSolomon),
			std::make_tuple(SVPostNet, "Postnet", std::vector<long>{ SVValueEncNum }, SVValueEncNum, std::vector<long>{ SVValueEccCheckDigit }, SVValueEccCheckDigit),
			std::make_tuple(SVPlanet, "Planet", std::vector<long>{ SVValueEncNum }, SVValueEncNum, std::vector<long>{ SVValueEccCheckDigit }, SVValueEccCheckDigit),
			std::make_tuple(SVUpcA, "UPC-A", std::vector<long>{ SVValueEncNum, SVValueEncUpcAAddOn }, SVValueEncNum, std::vector<long>{ SVValueEccCheckDigit }, SVValueEccCheckDigit),
			std::make_tuple(SVUpcE, "UPC-E", std::vector<long>{ SVValueEncNum, SVValueEncUpcEAddOn }, SVValueEncNum, std::vector<long>{ SVValueEccCheckDigit }, SVValueEccCheckDigit),
			std::make_tuple(SVPharmaCode, "Pharmacode", std::vector<long>{ SVValueEncNum }, SVValueEncNum, std::vector<long>{ SVValueEccNone }, SVValueEccNone),
			std::make_tuple(SVGS1Code, "GS1", std::vector<long>{ SVValueENCGS1, SVValueENCGS1Trunc, SVValueENCGS1Limited, SVValueENCGS1Expanded, SVValueENCGS1Stacked, SVValueENCGS1StackedOmni, SVValueENCGS1ExpandedStacked }, 
					SVValueENCGS1, std::vector<long>{ SVValueEccCheckDigit }, SVValueEccCheckDigit),
			std::make_tuple(SVQRCode, "QR Code", std::vector<long>{ SVValueEncQRCodeModel1, SVValueEncQRCodeModel2, SVValueAny }, SVValueAny, std::vector<long>{ SVValueAny, SVValueEccH, SVValueEccL, SVValueEccM, SVValueEccQ }, SVValueAny),
		};

		for (const auto& tupleTmp  : arrTmp)
		{
			long type;
			std::string name;
			std::vector<long> eccVec;
			long defaultEcc;
			std::vector<long> encVec;
			long defaultEnc;
			std::tie(type, name, encVec, defaultEnc, eccVec, defaultEcc) = tupleTmp;
			auto* pTypes = response.add_typeparameters();
			pTypes->set_type(type);
			pTypes->set_name(name);
			pTypes->set_defaultecctype(defaultEcc);
			for (auto eccType : eccVec)
			{
				pTypes->add_possibleecctypes(eccType);
			}
			pTypes->set_defaultenctype(defaultEnc);
			for (auto encType : encVec)
			{
				pTypes->add_possibleenctypes(encType);
			}
		}
	}

	return response;
}