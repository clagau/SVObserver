//*****************************************************************************
// \copyright COPYRIGHT (c) 2021,2021 by Körber Pharma Inspection GmbH. All Rights Reserved
/// \file RegressionTestController.cpp
/// This is the class for control the RegressionTest
//******************************************************************************
#include "stdafx.h"
#include "RegressionTestController.h"
#include "SVToolSet.h"
#include "Definitions/SVUserMessage.h"
#include "InspectionCommands/CommandExternalHelper.h"

constexpr LPCTSTR cResultHeader = _T("ObjectID; Results; Acquisition File\r\n");
constexpr char cPlcBad = '5';
constexpr char cPlcGood = '6';
constexpr char cNotUsed = '0';

RegressionRunFileStruct RegressionTestController::RegressionTestSetFiles(RegressionTestStruct& rRegTestStruct, RegressionRuningState& runState)
{
	RegressionRunFileStruct runFileStruct{};
	runFileStruct.ObjectName = rRegTestStruct.Name;

	switch (m_RunMode)
	{
	case RegModePlay:
	{
		runState.bRunFlag = true;
		if (!runState.bFirst)
		{
			++rRegTestStruct.stdIteratorCurrent;
		}
		if (rRegTestStruct.stdIteratorCurrent == rRegTestStruct.stdVectorFile.end())
		{
			rRegTestStruct.stdIteratorCurrent = rRegTestStruct.stdVectorFile.begin();
		}

		if (rRegTestStruct.iFileMethod != RegSingleFile)
		{
			if (m_RunPlayMode != Continue && !runState.bFirst)
			{
				if (rRegTestStruct.stdIteratorCurrent == rRegTestStruct.stdIteratorStart)
				{
					runState.bListDone = true;
					runState.bRunFlag = false;
				}
			}
		}

		if (rRegTestStruct.stdVectorFile.end() != rRegTestStruct.stdIteratorCurrent)
		{
			runFileStruct.FileName = *rRegTestStruct.stdIteratorCurrent;
		}
		break;
	}
	case RegModeSingleStepForward:
	{
		if (!runState.bFirst)
		{
			++rRegTestStruct.stdIteratorCurrent;
		}

		if (rRegTestStruct.stdIteratorCurrent == rRegTestStruct.stdVectorFile.end())
		{
			rRegTestStruct.stdIteratorCurrent = rRegTestStruct.stdVectorFile.begin();
		}
		if (rRegTestStruct.stdVectorFile.end() != rRegTestStruct.stdIteratorCurrent)
		{
			runFileStruct.FileName = *rRegTestStruct.stdIteratorCurrent;
		}
		runState.bRunFlag = true;
		break;
	}
	case RegModeSingleStepBack:
	{
		if (!runState.bFirst)
		{
			if (rRegTestStruct.stdIteratorCurrent == rRegTestStruct.stdVectorFile.begin())
			{
				rRegTestStruct.stdIteratorCurrent = rRegTestStruct.stdVectorFile.end() - 1;
			}
			else
			{
				--rRegTestStruct.stdIteratorCurrent;
			}
		}

		if (rRegTestStruct.stdIteratorCurrent == rRegTestStruct.stdVectorFile.end())
		{
			rRegTestStruct.stdIteratorCurrent = rRegTestStruct.stdVectorFile.end() - 1;
		}

		if (rRegTestStruct.stdVectorFile.end() != rRegTestStruct.stdIteratorCurrent)
		{
			runFileStruct.FileName = *rRegTestStruct.stdIteratorCurrent;
		}
		runState.bRunFlag = true;
		break;
	}
	case RegModeBackToBeginningPlay:
	{
		rRegTestStruct.stdIteratorCurrent = rRegTestStruct.stdIteratorStart;
		if (rRegTestStruct.stdVectorFile.end() != rRegTestStruct.stdIteratorCurrent)
		{
			runFileStruct.FileName = *rRegTestStruct.stdIteratorCurrent;
		}
		runState.bRunFlag = true;
		break;
	}
	case RegModeBackToBeginningStop:
	{
		rRegTestStruct.stdIteratorCurrent = rRegTestStruct.stdIteratorStart;
		if (rRegTestStruct.stdVectorFile.end() != rRegTestStruct.stdIteratorCurrent)
		{
			runFileStruct.FileName = *rRegTestStruct.stdIteratorCurrent;
		}
		runState.bRunFlag = true;
		break;
	}
	case RegModeResetSettings:
	{
		rRegTestStruct.stdIteratorCurrent = rRegTestStruct.stdIteratorStart;
		break;
	}
	default:
	{
		break;
	}
	}//end switch

	return runFileStruct;
}

void  RegressionTestController::initialize(HWND hMainWnd, HWND hRegressionWnd, SVInspectionProcess* pInspection, const SvTrig::ObjectIDParameters& rObjectIDParams, SVObjectPtrVector&& OutputValueList)
{
	m_hMainWnd = hMainWnd;
	m_hRegressionWnd = hRegressionWnd;
	m_pInspection = pInspection;
	m_objectIDParams = rObjectIDParams;
	m_OutputValueList = std::move(OutputValueList);
}

DWORD RegressionTestController::runThread()
{
	m_objectIDParams.m_currentObjectID = m_objectIDParams.m_startObjectID;
	m_triggerIndex = 0;
	m_isRunning = true;
	m_RunMode = RegModePause;
	m_isStopping = false;

	SVToolSet* pToolset = m_pInspection->GetToolSet();
	RegressionRuningState runState;

	while (m_RunMode != RegModeStopExit)
	{
		//while in Pause mode, sleep
		runState.bModeReset = false;
		if (m_RunMode == RegModePause)
		{
			::Sleep(50);
			continue;
		}

		size_t regCameraSize = m_regCameras.size();
		size_t regImageSize = m_regImages.size();

		runState.bListDone = false;
		std::vector<RegressionRunFileStruct> fileNameVec;
		fileNameVec.reserve(regCameraSize + regImageSize);

		if (m_RunMode == RegModeResetSettings)
		{
			runState.bFirst = true;
			runState.bRunFlag = false;
			runState.bModeReset = true;
		}

		bool allSingleImage = setImagesForNextRun(*m_pInspection, runState, std::back_inserter(fileNameVec));

		if (m_RunMode == RegModePlay && m_RunPlayMode != Continue)
		{
			if (allSingleImage) //all using single file
			{
				m_RunMode = RegModePause;
				SendMessage(m_hRegressionWnd, WM_REGRESSION_TEST_SET_PLAYPAUSE, reinterpret_cast<LPARAM> (&m_RunMode), 0);
				runState.bModeReset = true;
				runState.bFirst = true;
			}
			else
			{  //not using single files.
				if (runState.bListDone)
				{
					m_RunMode = RegModePause;
					SendMessage(m_hRegressionWnd, WM_REGRESSION_TEST_SET_PLAYPAUSE, reinterpret_cast<LPARAM> (&m_RunMode), 0);
					runState.bModeReset = true;
					runState.bFirst = true;
					runState.bRunFlag = false;
					if (m_fileOutputResult.is_open())
					{
						m_fileOutputResult.close();
					}
				}
			}
		}

		if (m_RunMode == RegModeResetSettings)
		{
			//set back to pause
			m_RunMode = RegModePause;
		}

		if (m_RunMode == RegModeBackToBeginningPlay)
		{
			m_RunMode = RegModePlay;
		}

		if (m_RunMode == RegModeBackToBeginningStop)
		{
			m_RunMode = RegModePause;
			SendMessage(m_hRegressionWnd, WM_REGRESSION_TEST_SET_PLAYPAUSE, reinterpret_cast<LPARAM> (&m_RunMode), 0);
		}

		//send image info to the dialog...
		if (runState.bRunFlag)
		{
			pToolset->setObjectID(m_objectIDParams.m_currentObjectID);
			pToolset->setTriggerIndex(m_triggerIndex);
			pToolset->setTriggerPerObjectID(m_objectIDParams.m_triggerPerObjectID);
			runState.bRunFlag = false;
			SendMessage(m_hRegressionWnd, WM_REGRESSION_TEST_SET_FILENAMES, reinterpret_cast<LPARAM> (&fileNameVec), 0);
			runOnce(*m_pInspection);

			if (runState.bFirst && m_isValidationMode && false == m_fileOutputResult.is_open() && false == m_resultFileName.empty())
			{
				m_fileOutputResult.open(m_resultFileName.c_str(), std::ofstream::out | std::ofstream::binary | std::ofstream::trunc);
				std::string data {cResultHeader};
				m_fileOutputResult.write(data.c_str(), data.size());
			}
			if (m_isValidationMode && m_fileOutputResult.is_open() && m_triggerIndex == m_objectIDParams.m_triggerPerObjectID)
			{
				std::string firstFileName = (0 < fileNameVec.size()) ? fileNameVec[0].FileName : _T("");
				std::string outputData;
				for (const auto& pObject : m_OutputValueList)
				{
					if (nullptr != pObject)
					{
						double value;
						pObject->getValue(value);
						outputData += (value > 0.0) ? cPlcGood : cPlcBad;
					}
					else
					{
						outputData += cNotUsed;
					}
					outputData += ' ';
				}
				outputData =  SvUl::Format(_T("%lu;%s;%s\r\n"), m_objectIDParams.m_currentObjectID, outputData.c_str(), firstFileName.c_str());
				m_fileOutputResult.write(outputData.c_str(), outputData.size());
			}
			fileNameVec.clear();
		}

		if (!m_isStopping)
		{
			if (m_RunMode == RegModePlay)
			{
				if (m_iTimeoutMS > 0)
				{
					Sleep(m_iTimeoutMS);
				}
			}
			else
			{
				m_RunMode = RegModePause;
			}
		}

		if (!runState.bModeReset)
		{
			runState.bFirst = false;
		}

		if (RegModePlay == m_RunMode && m_UsePlayCondition && m_pInspection->shouldPauseRegressionTestByCondition())
		{
			setPlayPause(runState);
		}
	}//end of while loop

	if (m_fileOutputResult.is_open())
	{
		m_fileOutputResult.close();
	}

	//let the IP know that the regression test is done.
	PostMessage(m_hMainWnd, WM_COMMAND, WM_REGRESSION_TEST_COMPLETE, 0L);
	//let the RegressionRunDlg know that it is to shut down...
	SendMessage(m_hRegressionWnd, WM_REGRESSION_TEST_CLOSE_REGRESSION, 0, 0);

	const auto& rCameraList = GetCameras(*m_pInspection);
	for (auto* pCamera : rCameraList)
	{
		SvVol::BasicValueObjectPtr pRegValue = pCamera->getCameraValue(SvDef::FqnCameraRegPath);
		if (nullptr != pRegValue)
		{
			pRegValue->setValue(_T(""));
		}
		pRegValue = pCamera->getCameraValue(SvDef::FqnCameraRegFile);
		if (nullptr != pRegValue)
		{
			pRegValue->setValue(_T(""));
		}
	}

	return 0L;
}

void RegressionTestController::stopRunning()
{
	m_isRunning = false;
	m_RunPlayMode = RunToEnd;
	m_isStopping = true;
	m_RunMode = RegModeStopExit;  //should cause dialog to go away...
}

void RegressionTestController::resetEquationText()
{
	if (!m_IsInitEquationText)
	{
		double value;
		SvStl::MessageContainerVector errorMessages;
		m_pPlayEquationController->ValidateEquation(m_LoadEquationText, value, true, errorMessages);
		if (!errorMessages.empty())
		{
			SvStl::MessageManager msg(SvStl::MsgType::Log);
			msg.setMessage(errorMessages[0].getMessage());
		}
		m_IsInitEquationText = true;
	}
}

SvIe::SVVirtualCameraPtrVector RegressionTestController::GetCameras(const SVInspectionProcess& rIP) const
{
	return rIP.GetCameras();
}

bool RegressionTestController::runOnce(SVInspectionProcess& rIP)
{
	return (S_OK == SvCmd::RunOnceSynchronous(rIP.getObjectId()));
}

void RegressionTestController::setPlayPause(RegressionRuningState& runState)
{
	m_RunMode = RegModePause;
	SendMessage(m_hRegressionWnd, WM_REGRESSION_TEST_SET_PLAYPAUSE, reinterpret_cast<LPARAM> (&m_RunMode), 0);
	runState.bRunFlag = false;
	if (m_RunPlayMode != Continue)
	{
		//Check if end of list reached and if then set it to begin.
		bool bEndOfList = false;
		for (const auto& rRegTestStruct : m_regCameras)
		{
			if (rRegTestStruct.iFileMethod != RegSingleFile)
			{
				if (rRegTestStruct.stdVectorFile.end() == rRegTestStruct.stdIteratorCurrent + 1)
				{
					bEndOfList = true;
				}
			}
		}
		for (auto& regData : m_regImages)
		{
			if (regData.iFileMethod != RegSingleFile)
			{
				if (regData.stdVectorFile.end() == regData.stdIteratorCurrent + 1)
				{
					bEndOfList = true;
				}
			}
		}//end for i to num cameras...

		if (bEndOfList)
		{
			runState.bFirst = true;
			for (auto& rRegTestStruct : m_regCameras)
			{
				rRegTestStruct.stdIteratorCurrent = rRegTestStruct.stdVectorFile.begin();
			}

			for (auto& regData : m_regImages)
			{
				regData.stdIteratorCurrent = regData.stdVectorFile.begin();
			}
		}
	}
}

bool RegressionTestController::setImagesForNextRun(SVInspectionProcess& rIP, RegressionRuningState& runState, std::back_insert_iterator<std::vector<RegressionRunFileStruct>> fileNameInserter)
{
	if (false == m_isValidationMode && RegressionRunModeEnum::RegModeBackToBeginningStop == m_RunMode)
	{
		m_objectIDParams.m_currentObjectID = m_objectIDParams.m_startObjectID;
		m_triggerIndex = 0;
	}

	bool allSingleImage = true;
	for (auto& regData : m_regCameras)
	{
		allSingleImage &= (regData.iFileMethod == RegSingleFile);
		RegressionRunFileStruct runFileStruct = RegressionTestSetFiles(regData, runState);

		if (runState.bRunFlag)
		{
			// cppcheck-suppress unreadVariable symbolName=fileNameInserter ; cppCheck doesn't know back_insert_iterator
			fileNameInserter = runFileStruct;

			const auto& rCameraList = GetCameras(rIP);
			auto cameraIter = std::find_if(rCameraList.begin(), rCameraList.end(), [runFileStruct](auto pCamera)
				{
					return pCamera != nullptr && runFileStruct.ObjectName == pCamera->GetName();
				});
			std::string regPath;
			std::string regFile;
			size_t pos = runFileStruct.FileName.find_last_of('\\');
			if (std::string::npos != pos)
			{
				regPath = runFileStruct.FileName.substr(0, pos);
				regFile = runFileStruct.FileName.substr(pos + 1);
			}
			else
			{
				regFile = runFileStruct.FileName;
			}
			SvVol::BasicValueObjectPtr pRegValue = (*cameraIter)->getCameraValue(SvDef::FqnCameraRegPath);
			if (nullptr != pRegValue)
			{
				pRegValue->setValue(regPath.c_str());
			}
			pRegValue = (*cameraIter)->getCameraValue(SvDef::FqnCameraRegFile);
			if (nullptr != pRegValue)
			{
				pRegValue->setValue(regFile.c_str());
			}
			rIP.AddInputImageRequestByCameraName(runFileStruct.ObjectName, runFileStruct.FileName);

			if (true == m_isValidationMode)
			{
				size_t pos2 = regFile.find_last_of('.');
				size_t pos1 = regFile.find_last_of('_', pos2);
				if (std::string::npos != pos1 && std::string::npos != pos2)
				{
					m_triggerIndex = std::stoul(regFile.substr(pos1 + 1, pos2 - pos1 - 1));
					pos2 = pos1 - 1;
				}
				pos1 = regFile.find_last_of('_', pos2);
				if (std::string::npos != pos1 && std::string::npos != pos2)
				{
					m_objectIDParams.m_currentObjectID = std::stoul(regFile.substr(pos1 + 1, pos2 - pos1));
				}
			}
			else
			{
				if (RegressionRunModeEnum::RegModeSingleStepBack == m_RunMode)
				{
					--m_triggerIndex;
				}
				else
				{
					++m_triggerIndex;
				}

				if (m_objectIDParams.m_triggerPerObjectID < m_triggerIndex)
				{
					++m_objectIDParams.m_currentObjectID;
					m_triggerIndex = 1L;
				}
				else if (1L > m_triggerIndex)
				{
					--m_objectIDParams.m_currentObjectID;
					m_triggerIndex = m_objectIDParams.m_triggerPerObjectID;
				}
				else if (0 == m_objectIDParams.m_startObjectID && 0 == m_objectIDParams.m_currentObjectID)
				{
					m_objectIDParams.m_currentObjectID = 1L;
				}
			}
		}
	}//end for i to num cameras...

	for (auto& regData : m_regImages)
	{
		allSingleImage &= (regData.iFileMethod == RegSingleFile);
		RegressionRunFileStruct runFileStruct = RegressionTestSetFiles(regData, runState);

		if (runState.bRunFlag)
		{
			fileNameInserter = runFileStruct;
			//add inspection name and Toolset to the objectName
			rIP.AddInputImageRequestToTool(runFileStruct.ObjectName, regData.objectId, runFileStruct.FileName);
		}
	}//end for i to num cameras...
	return allSingleImage;
}