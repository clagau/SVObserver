//*****************************************************************************
// \copyright COPYRIGHT (c) 2020,2020 by Körber Pharma Inspection GmbH. All Rights Reserved
/// \file BlobAnalyzer2.cpp
/// This is the class for the Blob Analyzer 2
//******************************************************************************

#pragma region Includes
#include "stdafx.h"
#include "Definitions/Color.h"
#include "Definitions/TextDefinesSvDef.h"
#include "ObjectInterfaces/IInspectionProcess.h"
#include "Operators/SVResultLong.h"
#include "Operators/TableObject.h"
#include "SVMatroxLibrary/SVMatroxBlobInterface.h"
#include "SVMatroxLibrary/SVMatroxBuffer.h"
#include "SVProtoBuf/ConverterHelper.h"
#include "InspectionEngine/RunStatus.h"
#include "SVStatusLibrary/SvimState.h"
#include "Tools/SVTool.h"
#include "BlobAnalyzer2.h"
#include "BlobFeatureList.h"
#include "BlobDrawTask.h"
#pragma endregion Includes

namespace SvAo
{

#pragma region Declarations
#ifdef _DEBUG
#undef THIS_FILE
	static char THIS_FILE[] = __FILE__;
#endif

	enum class BlobColorEnum
	{
		SV_BLOB_BLACK = 0,
		SV_BLOB_WHITE = 1
	};// end SV_BLOB_COLOR_ENUM
	const LPCSTR g_strBlobColorEnums = _T("Black=0,NonBlack=1");

	enum class BlobConnectivityEnum
	{
		NEIGHBORS_4 = 0,
		NEIGHBORS_8 = 1
	};// end SV_BLOB_COLOR_ENUM
	const LPCSTR g_strBlobConnectivityEnums = _T("4Neighbors=0,8Neighbors=1");
#pragma endregion Declarations

	SV_IMPLEMENT_CLASS(BlobAnalyzer2, SvPb::BlobAnalyzer2ClassId);

	BlobAnalyzer2::BlobAnalyzer2(SVObjectClass* POwner, int StringResourceID)
		: ImageAnalyzer(POwner, StringResourceID)
		, m_pResultTable(nullptr)
	{
		init();
	}

	void BlobAnalyzer2::init()
	{
		//Indentify our output type.
		m_ObjectTypeInfo.m_SubType = SvPb::BlobAnalyzer2ObjectType;

		RegisterEmbeddedObject(&m_maxBlobDataArraySize, SvPb::MaxBlobDataArraySizeEId, IDS_OBJECTNAME_MAX_BLOB_DATA_ARRAY_SIZE, false, SvOi::SVResetItemOwner, true);
		RegisterEmbeddedObject(&m_numberOfBlobsFound, SvPb::NbrOfBlobsFoundEId, IDS_OBJECTNAME_NBROFBLOBSFOUND, false, SvOi::SVResetItemNone, false);
		m_numberOfBlobsFound.setSaveValueFlag(false);

		RegisterEmbeddedObject(&m_isGrayImageValue, SvPb::IsGrayImageEId, IDS_BLOB_IS_GRAY_IMAGE, false, SvOi::SVResetItemOwner, true);
		RegisterEmbeddedObject(&m_colorBlobEnumValue, SvPb::BlobColorEId, IDS_BLACK_BLOBS, false, SvOi::SVResetItemOwner, true);
		RegisterEmbeddedObject(&m_connectivityEnumValue, SvPb::ConnectivityBlobEId, IDS_BLOB_CONNECTIVITY, false, SvOi::SVResetItemOwner, true);

		m_grayImageInput.SetInputObjectType(SvPb::SVImageObjectType, SvPb::SVImageMonoType);
		registerInputObject(&m_grayImageInput, SvDef::GrayImageConnectionName, SvPb::GrayImageInputEId);

		m_maxBlobDataArraySize.SetDefaultValue(100, true);
		m_isGrayImageValue.SetDefaultValue(BOOL(false), true);
		m_colorBlobEnumValue.SetEnumTypes(g_strBlobColorEnums);
		m_colorBlobEnumValue.SetDefaultValue(static_cast<long>(BlobColorEnum::SV_BLOB_WHITE), true);
		m_connectivityEnumValue.SetEnumTypes(g_strBlobConnectivityEnums);
		m_connectivityEnumValue.SetDefaultValue(static_cast<long>(BlobConnectivityEnum::NEIGHBORS_8), true);

		BlobFeatureList* pBlobFeatureList = new BlobFeatureList;
		Add(pBlobFeatureList);
	}

	/////////////////////////////////////////////////////////////////////////////
	//
	//
	BlobAnalyzer2::~BlobAnalyzer2()
	{
		BlobAnalyzer2::CloseObject();
	}

#pragma region IBlobAnalyzer2
	SvPb::InspectionCmdResponse BlobAnalyzer2::getFeaturesData() const
	{
		return m_pBlobFeatureList->getFeaturesData();
	}

	SvPb::InspectionCmdResponse BlobAnalyzer2::setFeatures(SvPb::SetFeaturesRequest request)
	{
		BOOL isGrayValid = false;
		m_isGrayImageValue.GetValue(isGrayValid);
		auto cmdResp = m_pBlobFeatureList->setFeatures(request, isGrayValid);
		SvPb::SetFeaturesResponse* pResponse = cmdResp.mutable_setfeaturesresponse();
		if (0 == pResponse->error_list_size())
		{
			SvStl::MessageContainerVector errorMessages;
			updateBlobFeatures(&errorMessages);
			for (const auto& rMessage : errorMessages)
			{
				if (SvStl::Tid_ExcludeBlobUpperBoundLowerThanLowerBound == rMessage.getMessage().m_AdditionalTextId && 1 < rMessage.getMessage().m_AdditionalTextList.size())
				{
					try
					{
						int row = std::stoi(rMessage.getMessage().m_AdditionalTextList[1]);
						auto* pError = pResponse->add_error_list();
						pError->set_row(row);
						pError->set_fieldid(SvPb::FeatureData::kLowerBoundFieldNumber);
						SvPb::convertMessageToProtobuf(rMessage, cmdResp.mutable_errormessage()->add_messages());
					}
					catch (...)
					{
						SvStl::MessageManager Msg(SvStl::MsgType::Log);
						Msg.setMessage(rMessage.getMessage());
					}
				}
				else if (SvStl::Tid_RangeBlobUpperBoundLowerThanLowerBound == rMessage.getMessage().m_AdditionalTextId && 3 < rMessage.getMessage().m_AdditionalTextList.size())
				{
					try
					{
						int row = std::stoi(rMessage.getMessage().m_AdditionalTextList[3]);
						auto* pError = pResponse->add_error_list();
						pError->set_row(row);
						auto textId = SvStl::MessageData::convertAdditionalText2Id(rMessage.getMessage().m_AdditionalTextList[1]);
						if (SvStl::Tid_FailHigh == textId)
						{
							pError->set_fieldid(SvPb::FeatureData::kRangeFailHighFieldNumber);
						}
						else if(SvStl::Tid_WarnHigh == textId)
						{
							pError->set_fieldid(SvPb::FeatureData::kRangeWarnHighFieldNumber);
						}
						else
						{
							pError->set_fieldid(SvPb::FeatureData::kRangeWarnLowFieldNumber);
						}

						
						SvPb::convertMessageToProtobuf(rMessage, cmdResp.mutable_errormessage()->add_messages());
					}
					catch (...)
					{
						SvStl::MessageManager Msg(SvStl::MsgType::Log);
						Msg.setMessage(rMessage.getMessage());
					}
				}
				else
				{
					SvPb::convertMessageToProtobuf(rMessage, cmdResp.mutable_errormessage()->add_messages());
				}
			}
		}


		return cmdResp;
	}
#pragma endregion IBlobAnalyzer

	bool BlobAnalyzer2::CloseObject()
	{
		ImageAnalyzer::CloseObject();

		if (S_OK != SVMatroxBlobInterface::DestroyResult(m_ResultBufferID))
		{
			SvStl::MessageManager Msg(SvStl::MsgType::Log);
			Msg.setMessage(SVMSG_SVO_93_GENERAL_WARNING, SvStl::Tid_Error_MilDestroy, SvStl::SourceFileParams(StdMessageParams), getObjectId());
		}
		if (S_OK != SVMatroxBlobInterface::DestroyContext(m_BlobContextID))
		{
			SvStl::MessageManager Msg(SvStl::MsgType::Log);
			Msg.setMessage(SVMSG_SVO_93_GENERAL_WARNING, SvStl::Tid_Error_MilDestroy, SvStl::SourceFileParams(StdMessageParams), getObjectId());
		}

		m_ResultBufferID = M_NULL;
		m_BlobContextID = M_NULL;
		return true;
	}


	bool BlobAnalyzer2::CreateObject(const SVObjectLevelCreateStruct& rCreateStructure)
	{
		bool Result{ true };

		try
		{
			if (!ImageAnalyzer::CreateObject(rCreateStructure))
			{
				SvStl::MessageManager MesMan(SvStl::MsgType::Log);
				MesMan.setMessage(SVMSG_SVO_103_REPLACE_ERROR_TRAP, SvStl::Tid_UnexpectedError, SvStl::SourceFileParams(StdMessageParams), getObjectId());
				throw;
			}

			m_pResultTable = dynamic_cast<SvOp::TableObject*>(SvOi::FindObject(getObjectId(), SvDef::SVObjectTypeInfoStruct(SvPb::TableObjectType, SvPb::SVNotSetSubObjectType)));
			if (nullptr == m_pResultTable)
			{
				m_pResultTable = new SvOp::TableObject(this);
				Add(m_pResultTable);
				if (!CreateChildObject(m_pResultTable))
				{
					SvStl::MessageContainer message;
					message.setMessage(SVMSG_SVO_92_GENERAL_ERROR, SvStl::Tid_TableObject_CreateFailed, SvStl::SourceFileParams(StdMessageParams), getObjectId());
					SvStl::MessageManager Msg(SvStl::MsgType::Log);
					Msg.setMessage(message.getMessage());
					throw;
				}
			}

			HRESULT MatroxCode = SVMatroxBlobInterface::CreateResult(m_ResultBufferID);

			if (S_OK != MatroxCode || M_NULL == m_ResultBufferID)
			{
				SvStl::MessageManager MesMan(SvStl::MsgType::Log);
				MesMan.setMessage(SVMSG_SVO_103_REPLACE_ERROR_TRAP, SvStl::Tid_UnexpectedError, SvStl::SourceFileParams(StdMessageParams), getObjectId());
				throw;
			}

			MatroxCode = SVMatroxBlobInterface::CreateContext(m_BlobContextID);

			if (S_OK != MatroxCode || M_NULL == m_BlobContextID)
			{
				SvStl::MessageManager MesMan(SvStl::MsgType::Log);
				MesMan.setMessage(SVMSG_SVO_103_REPLACE_ERROR_TRAP, SvStl::Tid_UnexpectedError, SvStl::SourceFileParams(StdMessageParams), getObjectId());
				throw;
			}

			m_pBlobFeatureList = dynamic_cast<BlobFeatureList*>(getFirstObject({ SvPb::BlobFeatureListObjectType }));
			if (nullptr == m_pBlobFeatureList)
			{
				m_pBlobFeatureList = new BlobFeatureList;
				Add(m_pBlobFeatureList);
			}

			if (nullptr == m_pBlobFeatureList)
			{
				SvStl::MessageManager MesMan(SvStl::MsgType::Log);
				MesMan.setMessage(SVMSG_SVO_103_REPLACE_ERROR_TRAP, SvStl::Tid_UnexpectedError, SvStl::SourceFileParams(StdMessageParams), getObjectId());
				throw;
			}
			m_numberOfBlobsFound.SetObjectAttributesAllowed(SvPb::audittrail, SvOi::SetAttributeType::RemoveAttribute);
			m_pBlobFeatureList->setNumberOfBlobsObject(&m_numberOfBlobsFound);
			m_pBlobFeatureList->setResultBufferId(m_ResultBufferID);
			m_pBlobFeatureList->setResultTable(m_pResultTable);

			m_pDrawTask = dynamic_cast<BlobDrawTask*>(getFirstObject({ SvPb::BlobDrawObjectType }));
			if (nullptr == m_pDrawTask)
			{
				m_pDrawTask = new BlobDrawTask;
				Add(m_pDrawTask);
			}

			if (nullptr == m_pDrawTask)
			{
				SvStl::MessageManager MesMan(SvStl::MsgType::Log);
				MesMan.setMessage(SVMSG_SVO_103_REPLACE_ERROR_TRAP, SvStl::Tid_UnexpectedError, SvStl::SourceFileParams(StdMessageParams), getObjectId());
				throw;
			}
			m_pDrawTask->setResultBufferId(m_ResultBufferID);

			updateBlobFeatures();
			allocateBlobNumberResult();

			SvOi::IInspectionProcess* pInspection = GetInspectionInterface();
			if (nullptr != pInspection)
			{
				pInspection->SetDefaultInputs();
			}
		}
		catch (...)
		{
			Result = false;
		}


		if (!Result)
		{
			m_isCreated = false;
		}
		else
		{
			m_isCreated = true;
		}

		return m_isCreated;
	}

	bool BlobAnalyzer2::onRun(SvIe::RunStatus& rRunStatus, SvStl::MessageContainerVector *pErrorMessages)
	{
		try
		{
			if (!__super::onRun(rRunStatus, pErrorMessages))
			{
				return false;
			}

			SvIe::SVImageClass* pInputImage = getInputImage(true);
			if (nullptr == pInputImage)
			{
				SvStl::MessageManager msg(SvStl::MsgType::Data);
				msg.setMessage(SVMSG_SVO_103_REPLACE_ERROR_TRAP, SvStl::Tid_UnexpectedError, SvStl::SourceFileParams(StdMessageParams), getObjectId());
				msg.Throw();
			}
			SvOi::ITRCImagePtr pImageBuffer = pInputImage->getImageReadOnly(rRunStatus.m_triggerRecord.get());
			if (nullptr == pImageBuffer)
			{
				Log_Assert(false);
				SvStl::MessageManager msg(SvStl::MsgType::Data);
				msg.setMessage(SVMSG_SVO_103_REPLACE_ERROR_TRAP, SvStl::Tid_UnexpectedError, SvStl::SourceFileParams(StdMessageParams), getObjectId());
				msg.Throw();
			}
			if (pImageBuffer->isEmpty())
			{
				Log_Assert(false);
				SvStl::MessageManager msg(SvStl::MsgType::Data);
				msg.setMessage(SVMSG_SVO_103_REPLACE_ERROR_TRAP, SvStl::Tid_UnexpectedError, SvStl::SourceFileParams(StdMessageParams), getObjectId());
				msg.Throw();
			}

			//get grayImageId if should use for blob calc
			MIL_ID grayImageId = M_NULL;
			BOOL isGrayValueUsed{ false };
			m_isGrayImageValue.GetValue(isGrayValueUsed);
			if (isGrayValueUsed)
			{
				SvIe::SVImageClass* pGrayImage = m_grayImageInput.getInput<SvIe::SVImageClass>(true);
				if (pGrayImage)
				{
					SvOi::ITRCImagePtr pGrayImageBuffer = pGrayImage->getImageReadOnly(rRunStatus.m_triggerRecord.get());
					if (nullptr != pGrayImageBuffer && !pGrayImageBuffer->isEmpty())
					{
						grayImageId = pGrayImageBuffer->getHandle()->GetBuffer().GetIdentifier();
					}
					else
					{
						Log_Assert(false);
					}
				}
			}

			HRESULT MatroxCode = SVMatroxBlobInterface::Execute(m_ResultBufferID, pImageBuffer->getHandle()->GetBuffer(), m_BlobContextID, grayImageId);
			if (S_OK != MatroxCode)
			{
				Log_Assert(false);
				SvStl::MessageManager msg(SvStl::MsgType::Data);
				msg.setMessage(SVMSG_SVO_103_REPLACE_ERROR_TRAP, SvStl::Tid_UnexpectedError, SvStl::SourceFileParams(StdMessageParams), getObjectId());
				msg.Throw();
			}

			//set exclude data to mil for mil feature
			for (auto data : m_excludeDataVec)
			{
				double lowerBound = 0;
				data.m_rLowerBoundValue.getValue(lowerBound);
				double upperBound = 0;
				data.m_rUpperBoundValue.getValue(upperBound);
				if (lowerBound <= upperBound)
				{
					MblobSelect(m_ResultBufferID, M_EXCLUDE, data.m_featureType, data.m_isInner ? M_IN_RANGE : M_OUT_RANGE, lowerBound, upperBound);
				}
				else
				{
					SvDef::StringVector msgList;
					msgList.push_back(data.m_name);
					SvStl::MessageManager  Ex(SvStl::MsgType::Log);
					Ex.setMessage(SVMSG_SVO_92_GENERAL_ERROR, SvStl::Tid_ExcludeBlobUpperBoundLowerThanLowerBound, msgList, SvStl::SourceFileParams(StdMessageParams), getObjectId());
				}
			}

			//
			// Get the number blobs found by MblobCalculate()
			//
			long numberOfBlobs = 0;
			MatroxCode = SVMatroxBlobInterface::GetNumber(m_ResultBufferID, numberOfBlobs);
			if (S_OK != MatroxCode)
			{
				SvStl::MessageManager msg(SvStl::MsgType::Data);
				msg.setMessage(SVMSG_SVO_103_REPLACE_ERROR_TRAP, SvStl::Tid_UnexpectedError, SvStl::SourceFileParams(StdMessageParams), getObjectId());
				msg.Throw();
			}

			long maxBlobDataArraySize = 0;
			m_maxBlobDataArraySize.GetValue(maxBlobDataArraySize);

			if (numberOfBlobs > maxBlobDataArraySize)
			{
				SvStl::MessageManager msg(SvStl::MsgType::Log);
				msg.setMessage(SVMSG_SVO_93_GENERAL_WARNING, SvStl::Tid_Too_Many_Blobs, SvStl::SourceFileParams(StdMessageParams), getObjectId());
				rRunStatus.SetFailed();
				m_pResultTable->setSortContainerDummy(SvVol::DummySortContainer(0));
				m_numberOfBlobsFound.SetValue(0);
				return true;
			}

			m_numberOfBlobsFound.SetValue(numberOfBlobs);
			m_pResultTable->setSortContainerDummy(SvVol::DummySortContainer(numberOfBlobs));
		}
		catch (const SvStl::MessageContainer& e)
		{
			rRunStatus.SetInvalid();
			if (nullptr != pErrorMessages)
			{
				pErrorMessages->push_back(e);
			}
			return false;
		}

		return true;
	}

	bool BlobAnalyzer2::IsPtOverResult( const POINT& rPoint )
	{
		long currentNbrOfBlobs = 0;
	
		m_nBlobIndex = -1;
	
		if ( S_OK == m_numberOfBlobsFound.GetValue(currentNbrOfBlobs) && 0 != currentNbrOfBlobs && 
			nullptr != m_ResultColumnForOverlayArray[0] && nullptr != m_ResultColumnForOverlayArray[1] && nullptr != m_ResultColumnForOverlayArray[2] && nullptr != m_ResultColumnForOverlayArray[3])
		{
		
			auto pToolExtent = getToolExtentPtr();
		
			if (nullptr != pToolExtent)
			{
				std::vector<double> minXArray;
				m_ResultColumnForOverlayArray[0]->GetArrayValues(minXArray);
				std::vector<double> maxXArray;
				m_ResultColumnForOverlayArray[1]->GetArrayValues(maxXArray);
				std::vector<double> minYArray;
				m_ResultColumnForOverlayArray[2]->GetArrayValues(minYArray);
				std::vector<double> maxYArray;
				m_ResultColumnForOverlayArray[3]->GetArrayValues(maxYArray);
	
				size_t number = std::min({ static_cast<size_t>(currentNbrOfBlobs), minXArray.size(), maxXArray.size(), minYArray.size(), maxYArray.size() });
				
				for (int i = 0; i < number; i++)
				{
					RECT l_oRect;
					l_oRect.top = static_cast<long>(minYArray[i]);
					l_oRect.left = static_cast<long>(minXArray[i]);
					l_oRect.bottom = static_cast<long>(maxYArray[i]);
					l_oRect.right = static_cast<long>(maxXArray[i]);
					
					SVExtentFigureStruct l_svFigure{ l_oRect };
				
					pToolExtent->TranslateFromOutputSpace(l_svFigure, l_svFigure);
	
					if( S_OK == l_svFigure.IsPointOverFigure( SVPoint<double>(rPoint) ) )
					{
						m_nBlobIndex = i; 	
						break;
					}
				}
			}
		}
	
		return ( m_nBlobIndex != -1 );
	}

	bool BlobAnalyzer2::ResetObject(SvStl::MessageContainerVector *pErrorMessages)
	{
		bool result = __super::ResetObject(pErrorMessages);

		if (nullptr == m_pResultBlob || nullptr == m_pBlobFeatureList)
		{
			result = false;
			if (nullptr != pErrorMessages)
			{
				SvStl::MessageContainer Msg(SVMSG_SVO_92_GENERAL_ERROR, SvStl::Tid_ErrorGettingInputs, SvStl::SourceFileParams(StdMessageParams), getObjectId());
				pErrorMessages->push_back(Msg);
			}
		}

		result &= updateBlobFeatures(pErrorMessages);

		m_pBlobFeatureList->setSortControls(m_BlobContextID);

		m_grayImageInput.validateInput();
		BOOL isGrayValueUsed{ false };
		m_isGrayImageValue.GetValue(isGrayValueUsed);
		if (isGrayValueUsed)
		{
			if (nullptr == m_grayImageInput.getInput<SvIe::SVImageClass>())
			{
				result = false;
				if (nullptr != pErrorMessages)
				{
					SvStl::MessageContainer Msg(SVMSG_SVO_92_GENERAL_ERROR, SvStl::Tid_ErrorGettingInputs, SvStl::SourceFileParams(StdMessageParams), getObjectId());
					pErrorMessages->push_back(Msg);
				}
			}
		}

		long connectivityBlobEnum;
		m_connectivityEnumValue.GetValue(connectivityBlobEnum);
		SVMatroxBlobInterface::Set(m_BlobContextID, M_CONNECTIVITY, (static_cast<long>(BlobConnectivityEnum::NEIGHBORS_4) == connectivityBlobEnum) ? M_4_CONNECTED : M_8_CONNECTED);

		long colorBlobEnum;
		m_colorBlobEnumValue.GetValue(colorBlobEnum);
		SVMatroxBlobInterface::SetForeground(m_BlobContextID, BlobColorEnum::SV_BLOB_BLACK == static_cast<BlobColorEnum>(colorBlobEnum));

		return result;
	}

	void BlobAnalyzer2::addOverlayGroups(SVExtentMultiLineStructVector& rMultiLineArray, const SVImageExtentClass& rImageExtents) const
	{
		// only if ToolSet/Tool was not Disabled
		
		auto pTool = GetToolInterface();
		if (pTool && pTool->WasEnabled())
		{
			if (nullptr == m_ResultColumnForOverlayArray[0] || nullptr == m_ResultColumnForOverlayArray[1] || nullptr == m_ResultColumnForOverlayArray[2] || nullptr == m_ResultColumnForOverlayArray[3])
			{
				Log_Assert(false);
				return;
			}
			std::vector<double> minXArray;
			m_ResultColumnForOverlayArray[0]->GetArrayValues(minXArray);
			std::vector<double> maxXArray;
			m_ResultColumnForOverlayArray[1]->GetArrayValues(maxXArray);
			std::vector<double> minYArray;
			m_ResultColumnForOverlayArray[2]->GetArrayValues(minYArray);
			std::vector<double> maxYArray;
			m_ResultColumnForOverlayArray[3]->GetArrayValues(maxYArray);

			size_t numberOfBlobs = std::min({ minXArray.size(), maxXArray.size(), minYArray.size(), maxYArray.size() });
		

			// if running only show N Blob Figures according to the specified
			// MaxBlobDataArraySize variable
			if (SvimState::CheckState(SV_STATE_RUNNING | SV_STATE_TEST) && 1 < numberOfBlobs)
			{
				numberOfBlobs = 1;
			}

			for (size_t i = 0; i < numberOfBlobs; ++i)
			{
				RECT l_oRect;
				l_oRect.top = static_cast<long> (minYArray[i]);
				l_oRect.left = static_cast<long> (minXArray[i]);
				l_oRect.bottom = static_cast<long> (maxYArray[i]+1);
				l_oRect.right = static_cast<long> (maxXArray[i]+1);

				SVExtentFigureStruct svFigure{ l_oRect };			
				rImageExtents.TranslateFromOutputSpace(svFigure, svFigure);;
				
				SVExtentMultiLineStruct multiLine;
				multiLine.m_Color = SvDef::DefaultSubFunctionColor1;
				multiLine.AssignExtentFigure(svFigure, SvDef::DefaultSubFunctionColor1);
				UpdateOverlayIDs(multiLine);
				rMultiLineArray.push_back(multiLine);
			}
			
		}
		return;
	}

	bool BlobAnalyzer2::Run(SvIe::RunStatus& rRunStatus, SvStl::MessageContainerVector* pErrorMessages)
	{
		bool retVal = __super::Run(rRunStatus, pErrorMessages);
		//set the TableTool to the same state than the BlobFeatureList
		m_pResultTable->setStatus(m_pBlobFeatureList->GetObjectColor(), m_pBlobFeatureList->GetStatusTag());
		return retVal;
	}

	void BlobAnalyzer2::addOverlayGroups(SvPb::Overlay& rOverlay) const
	{
		auto* pGroup = rOverlay.add_shapegroups();
		pGroup->set_name("Blobs");
		pGroup->set_detaillevel(SvPb::Level1);
		auto* pShape = pGroup->add_shapes();
		pShape->mutable_color()->set_value(SvDef::DefaultSubFunctionColor1);
		auto* pRectArray = pShape->mutable_rectarray()->mutable_x12data();
		pRectArray->set_x1trpos(m_ResultColumnForOverlayArray[0]->getTrPos() + 1);
		pRectArray->set_x2trpos(m_ResultColumnForOverlayArray[1]->getTrPos() + 1);
		pRectArray->set_y1trpos(m_ResultColumnForOverlayArray[2]->getTrPos() + 1);
		pRectArray->set_y2trpos(m_ResultColumnForOverlayArray[3]->getTrPos() + 1);
	}

	SvDef::StringVector BlobAnalyzer2::getAnalyzerResult()
	{
		long currentNbrOfBlobs = 0;
		m_numberOfBlobsFound.GetValue(currentNbrOfBlobs);
		if (m_nBlobIndex >= 0 && m_nBlobIndex < currentNbrOfBlobs)
		{
			return m_pBlobFeatureList->getAnalyzerResult(m_nBlobIndex);	
		}
		return {};
	}

	bool BlobAnalyzer2::updateBlobFeatures(SvStl::MessageContainerVector* pErrorMessages)
	{
		m_excludeDataVec.clear();
		bool result = m_pBlobFeatureList->updateBlobFeatures(pErrorMessages, std::back_inserter(m_excludeDataVec));
		
		long maxBlobDataArraySize = 0;
		m_maxBlobDataArraySize.GetValue(maxBlobDataArraySize);
		std::set<MIL_ID> tmpFeatureGroups;
		m_pBlobFeatureList->updateTableDefinition(maxBlobDataArraySize, m_ResultColumnForOverlayArray, tmpFeatureGroups);

		MblobControl(m_BlobContextID, M_ALL_FEATURES, M_DISABLE);
		for (auto group : tmpFeatureGroups)
		{
			MblobControl(m_BlobContextID, group, M_ENABLE);
		}

		std::set<MIL_ID> diff;
		std::set_difference(m_featureGroups.begin(), m_featureGroups.end(), tmpFeatureGroups.begin(), tmpFeatureGroups.end(), std::inserter(diff, diff.begin()));
		if (0 < diff.size())
		{
			result = m_pDrawTask->resetStepsForDeletedFeature(diff, pErrorMessages) && result;
		}
		m_featureGroups = tmpFeatureGroups;
		return result;
	}

	void BlobAnalyzer2::allocateBlobNumberResult()
	{
		m_pResultBlob = dynamic_cast<SvOp::SVLongResult*>(getFirstObject({ SvPb::SVResultObjectType, SvPb::SVResultLongObjectType }));
		if (nullptr == m_pResultBlob)
		{
			SvDef::SVObjectTypeInfoStruct  interfaceInfo;
			interfaceInfo.m_EmbeddedID = m_numberOfBlobsFound.GetEmbeddedID();
			SvIe::SVClassInfoStruct       resultClassInfo;
			resultClassInfo.m_DesiredInputVector.push_back(interfaceInfo);

			resultClassInfo.m_ObjectTypeInfo.m_ObjectType = SvPb::SVResultObjectType;
			resultClassInfo.m_ObjectTypeInfo.m_SubType = SvPb::SVResultLongObjectType;
			resultClassInfo.m_ClassId = SvPb::LongResultClassId;
			resultClassInfo.m_ClassName = SvUl::LoadStdString(IDS_OBJECTNAME_RESULT);
			std::string Title = m_numberOfBlobsFound.GetName();
			resultClassInfo.m_ClassName += _T(" ") + Title;

			// Construct the result class
			m_pResultBlob = dynamic_cast<SvOp::SVLongResult*> (resultClassInfo.Construct());

			if (nullptr == m_pResultBlob)
			{
				SvStl::MessageManager  Ex(SvStl::MsgType::Log);
				Ex.setMessage(SVMSG_SVO_103_REPLACE_ERROR_TRAP, SvStl::Tid_UnexpectedError, SvStl::SourceFileParams(StdMessageParams), getObjectId());
				return;
			}

			Add(m_pResultBlob);

			m_pResultBlob->connectAllInputs();

			// And last - Create (initialize) it

			if (!m_pResultBlob->IsCreated())
			{
				// And finally try to create the child object...
				if (!CreateChildObject(m_pResultBlob))
				{
					SvStl::MessageManager Msg(SvStl::MsgType::Log);
					Msg.setMessage(SVMSG_SVO_93_GENERAL_WARNING, SvStl::Tid_BlobAnalyzer_ResultCreationFailed, SvStl::SourceFileParams(StdMessageParams), getObjectId());

					Delete(m_pResultBlob);
					m_pResultBlob = nullptr;
				}
			}
		}
	}
} //namespace SvAo