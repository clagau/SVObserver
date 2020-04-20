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
#include "SVLibrary/SVOIniClass.h"
#include "Operators/SVBarCodeResult.h"   
#include "InspectionEngine/SVBarCodeBuffer.h"
#include "InspectionEngine/SVImageProcessingClass.h"
#include "InspectionEngine/SVImageClass.h"
#include "SVMatroxLibrary/SVOLicenseManager.h"
#include "SVMatroxLibrary/SVMatroxBuffer.h"
#include "SVMatroxLibrary/SVMatroxBarCodeInterface.h"
#include "SVStatusLibrary/GlobalPath.h"
#include "SVStatusLibrary/MessageManager.h"
#include "SVStatusLibrary/MessageTextEnum.h"
#include "SVStatusLibrary/SVRunStatus.h"
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

SV_IMPLEMENT_CLASS (SVBarCodeAnalyzerClass, SvPb::BarCodeAnalyzerClassId);

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

SVBarCodeAnalyzerClass::SVBarCodeAnalyzerClass (SVObjectClass* POwner, int StringResourceID)
	: SVImageAnalyzerClass (POwner, StringResourceID)
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

void SVBarCodeAnalyzerClass::init()
{
	
	m_outObjectInfo.m_ObjectTypeInfo.m_SubType = SvPb::SVBarCodeAnalyzerObjectType;
	
	// Register Embedded Objects
	RegisterEmbeddedObject (&msv_szBarCodeValue, SvPb::BarCodeEId, IDS_OBJECTNAME_BARCODEVALUE, false, SvOi::SVResetItemNone);
	RegisterEmbeddedObject (&msv_szRegExpressionValue, SvPb::RegExpressionEId, IDS_OBJECTNAME_CONTROLVALUE, false, SvOi::SVResetItemOwner);
	RegisterEmbeddedObject (&msv_lBarCodeType, SvPb::BCTypeEId, IDS_OBJECTNAME_BARCODETYPE, false, SvOi::SVResetItemOwner);
	RegisterEmbeddedObject (&msv_dOrientation, SvPb::BCOrientationEId, IDS_OBJECTNAME_ORIENTATION, false, SvOi::SVResetItemOwner);
	RegisterEmbeddedObject (&msv_dSkewNegative, SvPb::BCSkewNegativeEId, IDS_OBJECTNAME_SKEWNEGATIVE, false, SvOi::SVResetItemOwner);
	RegisterEmbeddedObject (&msv_dSkewPositive, SvPb::BCSkewPositiveEId, IDS_OBJECTNAME_SKEWPOSITIVE, false, SvOi::SVResetItemOwner);
	RegisterEmbeddedObject (&msv_dCellMinSize, SvPb::BCCellMinSizeEId, IDS_OBJECTNAME_CELLMINSIZE, false, SvOi::SVResetItemOwner);
	RegisterEmbeddedObject (&msv_dCellMaxSize, SvPb::BCCellMaxSizeEId, IDS_OBJECTNAME_CELLMAXSIZE, false, SvOi::SVResetItemOwner);
	RegisterEmbeddedObject (&msv_dCellNumberX, SvPb::BCCellNumberXEId, IDS_OBJECTNAME_CELLNUMBERX, false, SvOi::SVResetItemOwner);
	RegisterEmbeddedObject (&msv_dCellNumberY, SvPb::BCCellNumberYEId, IDS_OBJECTNAME_CELLNUMBERY, false, SvOi::SVResetItemOwner);
	RegisterEmbeddedObject (&msv_dSpeed, SvPb::BCSpeedEId, IDS_OBJECTNAME_SPEED, false, SvOi::SVResetItemOwner);
	RegisterEmbeddedObject (&msv_dThreshold, SvPb::BCThresholdEId, IDS_OBJECTNAME_THRESHOLD, false, SvOi::SVResetItemOwner);
	RegisterEmbeddedObject (&msv_dStringSize, SvPb::BCStringSizeEId, IDS_OBJECTNAME_STRINGSIZE, false, SvOi::SVResetItemOwner);
	RegisterEmbeddedObject (&msv_dErrorCorrection, SvPb::BCErrorCorrectionEId, IDS_OBJECTNAME_ERRORCORRECTION, false, SvOi::SVResetItemOwner);
	RegisterEmbeddedObject (&msv_dEncoding, SvPb::BCEncodingEId, IDS_OBJECTNAME_ENCODING, false, SvOi::SVResetItemOwner);
	RegisterEmbeddedObject (&msv_dForegroundColor, SvPb::BCForegroundColorEId, IDS_OBJECTNAME_FOREGROUNDCOLOR, false, SvOi::SVResetItemOwner);
	RegisterEmbeddedObject (&msv_bSaveStringInFile, SvPb::BCSaveStringInFileEId, IDS_OBJECTNAME_SAVESTRINGINFILE, false, SvOi::SVResetItemOwner);
	RegisterEmbeddedObject (&msv_szStringFileName, SvPb::BCStringFileNameEId, IDS_OBJECTNAME_STRINGFILENAME, false, SvOi::SVResetItemOwner);
	RegisterEmbeddedObject (&m_bWarnOnFailedRead, SvPb::BCWarnOnFailedReadEId, IDS_OBJECTNAME_WARNONFAILEDREAD, false, SvOi::SVResetItemOwner);
	RegisterEmbeddedObject (&msv_lBarcodeTimeout, SvPb::BCTimeoutEId, IDS_OBJECTNAME_BARCODETIMEOUT, false, SvOi::SVResetItemOwner);
	// IDS_OBJECTNAME_BARCODEUNEVENGRID
	RegisterEmbeddedObject (&msv_bUnEvenGrid, SvPb::BCUnevenGridEId,IDS_OBJECTNAME_BARCODEUNEVENGRID, false, SvOi::SVResetItemOwner);

	// To support special DMCs May 2008.
	RegisterEmbeddedObject (&msv_RawData, SvPb::BCRawDataEId, IDS_OBJECTNAME_BARCODERAWDATA, false, SvOi::SVResetItemOwner);
	RegisterEmbeddedObject (&msv_eStringFormat, SvPb::BCStringFormatEId, IDS_OBJECTNAME_BARCODESTRINGFORMAT, false, SvOi::SVResetItemOwner);
	RegisterEmbeddedObject (&msv_lThresholdType, SvPb::BCThresholdTypeEId, IDS_OBJECTNAME_BARCODETHRESHOLDTYPE, false, SvOi::SVResetItemOwner);

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

	// Set default inputs and outputs
	addDefaultInputObjects();
	
	// Instantiate Children
	SvOp::SVBarCodeResultClass* pAnalyzerResult = new SvOp::SVBarCodeResultClass (this, IDS_CLASSNAME_SVBARCODEANALYZERESULT);
	
	if (nullptr != pAnalyzerResult)
	{
		Add(pAnalyzerResult);
	}
}

bool SVBarCodeAnalyzerClass::InitMil (SvStl::MessageContainerVector *pErrorMessages)
{
	CloseMil();
	
	SvIe::SVImageClass *pInputImage = getInputImage();
	if (nullptr != pInputImage)
	{
		// &&&
		SvIe::SVBarCodeBufferInfoClass svData;

		svData.HBuffer.milBarCode = m_MilCodeId;

		long l_lTmpType;
		msv_lBarCodeType.GetValue( l_lTmpType );
		svData.m_lCodeType = static_cast<SVBarCodeTypesEnum>( l_lTmpType );
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

			msv_dEncoding.GetValue (dParm);
			/*MatroxCode = */SVMatroxBarCodeInterface::Set( m_MilCodeId, SVBCEncoding, dParm);

			msv_dErrorCorrection.GetValue (dParm);
			/*MatroxCode = */SVMatroxBarCodeInterface::Set( m_MilCodeId, SVBCErrorCorrection, dParm);

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
				SvStl::MessageContainer Msg( SVMSG_SVO_92_GENERAL_ERROR, SvStl::Tid_MilBarCodeInitFailed, SvStl::SourceFileParams(StdMessageParams), 0, getObjectId() );
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
			SvStl::MessageContainer Msg( SVMSG_SVO_92_GENERAL_ERROR, SvStl::Tid_NoSourceImage, SvStl::SourceFileParams(StdMessageParams), 0, getObjectId() );
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

bool SVBarCodeAnalyzerClass::CreateObject(const SVObjectLevelCreateStruct& rCreateStructure)
{
	bool bOk = false;


	if ( !m_bHasLicenseError )
	{
		if (SVImageAnalyzerClass::CreateObject (rCreateStructure))
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
	msv_szBarCodeValue.SetObjectAttributesAllowed( SvPb::printable, SvOi::SetAttributeType::RemoveAttribute );
	msv_szRegExpressionValue.SetObjectAttributesAllowed( SvPb::printable, SvOi::SetAttributeType::AddAttribute );
	msv_lBarCodeType.SetObjectAttributesAllowed( SvPb::printable, SvOi::SetAttributeType::AddAttribute );
	msv_dOrientation.SetObjectAttributesAllowed( SvPb::printable, SvOi::SetAttributeType::AddAttribute );
	msv_dSkewNegative.SetObjectAttributesAllowed( SvPb::printable, SvOi::SetAttributeType::AddAttribute );
	msv_dSkewPositive.SetObjectAttributesAllowed( SvPb::printable, SvOi::SetAttributeType::AddAttribute );
	msv_dCellMinSize.SetObjectAttributesAllowed( SvPb::printable, SvOi::SetAttributeType::AddAttribute );
	msv_dCellMaxSize.SetObjectAttributesAllowed( SvPb::printable, SvOi::SetAttributeType::AddAttribute );
	msv_dCellNumberX.SetObjectAttributesAllowed( SvPb::printable, SvOi::SetAttributeType::AddAttribute );
	msv_dCellNumberY.SetObjectAttributesAllowed( SvPb::printable, SvOi::SetAttributeType::AddAttribute );
	msv_dSpeed.SetObjectAttributesAllowed( SvPb::printable | SvPb::remotelySetable, SvOi::SetAttributeType::AddAttribute );
	msv_dThreshold.SetObjectAttributesAllowed( SvPb::printable, SvOi::SetAttributeType::AddAttribute );
	msv_dStringSize.SetObjectAttributesAllowed( SvPb::printable, SvOi::SetAttributeType::AddAttribute );
	msv_dErrorCorrection.SetObjectAttributesAllowed( SvPb::printable, SvOi::SetAttributeType::AddAttribute );
	msv_dEncoding.SetObjectAttributesAllowed( SvPb::printable, SvOi::SetAttributeType::AddAttribute );
	msv_dForegroundColor.SetObjectAttributesAllowed( SvPb::printable, SvOi::SetAttributeType::AddAttribute );
	msv_bSaveStringInFile.SetObjectAttributesAllowed( SvPb::printable, SvOi::SetAttributeType::AddAttribute );
	msv_szStringFileName.SetObjectAttributesAllowed( SvPb::printable, SvOi::SetAttributeType::AddAttribute );
	m_bWarnOnFailedRead.SetObjectAttributesAllowed( SvPb::printable | SvPb::remotelySetable, SvOi::SetAttributeType::AddAttribute );
	msv_lBarcodeTimeout.SetObjectAttributesAllowed( SvPb::printable | SvPb::remotelySetable, SvOi::SetAttributeType::AddAttribute );
	msv_RawData.SetObjectAttributesAllowed( SvPb::printable, SvOi::SetAttributeType::RemoveAttribute );
	m_isCreated = bOk;

	if ( m_bHasLicenseError )
	{
		SVOLicenseManager::Instance().AddLicenseErrorToList(getObjectId());
	}

	return bOk;
}

SVBarCodeAnalyzerClass::~SVBarCodeAnalyzerClass()
{
	if ( m_bHasLicenseError )
	{
		m_bHasLicenseError = false;
		SVOLicenseManager::Instance().RemoveLicenseErrorFromList(getObjectId());
	}

	SVBarCodeAnalyzerClass::CloseObject();
}

bool SVBarCodeAnalyzerClass::CloseObject()
{
	CloseMil ();
	return SVImageAnalyzerClass::CloseObject();
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
			SvStl::MessageContainer Msg( SVMSG_SVO_92_GENERAL_ERROR, SvStl::Tid_SVObserver_MatroxLicenseNotFound, SvStl::SourceFileParams(StdMessageParams), 0, getObjectId() );
			pErrorMessages->push_back(Msg);
		}
		rRunStatus.SetInvalid ();
		return false;
	}
	
	msv_szBarCodeValue.SetValue(std::string());
	if (SVImageAnalyzerClass::onRun (rRunStatus, pErrorMessages))
	{
		if (nullptr == m_pBarCodeResult)
		{
			if (nullptr != pErrorMessages)
			{
				SvStl::MessageContainer Msg(SVMSG_SVO_92_GENERAL_ERROR, SvStl::Tid_Error_NoResultObject, SvStl::SourceFileParams(StdMessageParams), 0, getObjectId());
				pErrorMessages->push_back(Msg);
			}
			return false;
		}

		BOOL bWarnOnFailedRead;
		m_bWarnOnFailedRead.GetValue(bWarnOnFailedRead); // Add value from checkbox instead of true to warn on fail.
									// default value will be false.
		m_pBarCodeResult->m_bFailedToRead = bWarnOnFailedRead != FALSE;// Preset flag to failed condition..

		SvIe::SVImageClass* pInputImage = getInputImage(true);
		SvTrc::IImagePtr pImageBuffer = pInputImage->getImageReadOnly(rRunStatus.m_triggerRecord.get());
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
								SvStl::MessageContainer Msg( SVMSG_SVO_92_GENERAL_ERROR, SvStl::Tid_SetValueFailed, SvStl::SourceFileParams(StdMessageParams), 0, getObjectId() );
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
					SvStl::MessageContainer Msg( SVMSG_SVO_92_GENERAL_ERROR, SvStl::Tid_UnknownException, SvStl::SourceFileParams(StdMessageParams), 0, getObjectId() );
					pErrorMessages->push_back(Msg);
				}
			}// end catch
		}
	}
	rRunStatus.SetInvalid ();
	return false;
}


bool SVBarCodeAnalyzerClass::LoadRegExpression( bool DisplayErrorMessage, SvStl::MessageContainerVector *pErrorMessages )
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
			message.setMessage(SVMSG_SVO_93_GENERAL_WARNING, SvStl::Tid_BarCode_UnableToRead, SvStl::SourceFileParams(StdMessageParams), SvStl::Err_10038, getObjectId());
			if (nullptr != pErrorMessages)
			{
				pErrorMessages->push_back(message);
			}
			if( DisplayErrorMessage )
			{
				SvStl::MessageMgrStd Msg(SvStl::MsgType::Log | SvStl::MsgType::Display );
				Msg.setMessage( message.getMessage() ); 
			}
			return false;
		}
	}

  return true;
}

bool SVBarCodeAnalyzerClass::SaveRegExpression( SvStl::MessageContainerVector *pErrorMessage )
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
				SvStl::MessageContainer message(SVMSG_SVO_93_GENERAL_WARNING, SvStl::Tid_BarCode_UnableToSave, SvStl::SourceFileParams(StdMessageParams), SvStl::Err_10039, getObjectId());
				pErrorMessage->push_back(message);
			}
			return false;
		}
	}
	return true;
}

bool SVBarCodeAnalyzerClass::ResetObject(SvStl::MessageContainerVector *pErrorMessages)
{
	bool Result = __super::ResetObject(pErrorMessages);

	//Get the pointer on reset
	m_pBarCodeResult = dynamic_cast<SvOp::SVBarCodeResultClass*> (GetResultObject());
	if(nullptr == m_pBarCodeResult)
	{
		Result = false;
		if (nullptr != pErrorMessages)
		{
			SvStl::MessageContainer Msg(SVMSG_SVO_92_GENERAL_ERROR, SvStl::Tid_Error_NoResultObject, SvStl::SourceFileParams(StdMessageParams), 0, getObjectId());
			pErrorMessages->push_back(Msg);
		}
	}

	if ( Result && !m_bHasLicenseError )
	{
		if ( InitMil(pErrorMessages) )
		{
			std::string TempRegExpressionValue;
			std::string TempStringFileName;

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
				SvStl::MessageContainer Msg( SVMSG_SVO_92_GENERAL_ERROR, SvStl::Tid_SVObserver_MatroxLicenseNotFound, SvStl::SourceFileParams(StdMessageParams), 0, getObjectId() );
				pErrorMessages->push_back(Msg);
			}
		}
	}

	return Result;
}

} //namespace SvAo
