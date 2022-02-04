//*****************************************************************************
// \copyright COPYRIGHT (c) 2021,2021 by Seidenader Maschinenbau GmbH. All Rights Reserved
/// \file RegressionTestController.cpp
/// This is the class for control the RegressionTest
//******************************************************************************
#include "stdafx.h"
#include "RegressionTestController.h"
#include "SVToolSet.h"
#include "Definitions/SVUserMessage.h"
#include "InspectionCommands/CommandExternalHelper.h"


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


DWORD RegressionTestController::runThread(HWND hMainWnd, HWND hRegressionWnd, SVInspectionProcess& rIP, const SvTrig::ObjectIDParameters& rObjectIDParams)
{
	m_objectIDParams = rObjectIDParams;
	m_objectIDParams.m_currentObjectID = m_objectIDParams.m_startObjectID;
	m_triggerIndex = 0;
	m_isRunning = true;
	m_RunMode = RegModePause;
	m_isStopping = false;

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

		bool allSingleImage = setImagesForNextRun(rIP, runState, std::back_inserter(fileNameVec));

		if (m_RunMode == RegModePlay && m_RunPlayMode != Continue)
		{
			if (allSingleImage) //all using single file
			{
				m_RunMode = RegModePause;
				SendMessage(hRegressionWnd, WM_REGRESSION_TEST_SET_PLAYPAUSE, reinterpret_cast<LPARAM> (&m_RunMode), 0);
				runState.bModeReset = true;
				runState.bFirst = true;
			}
			else
			{  //not using single files.
				if (runState.bListDone)
				{
					m_RunMode = RegModePause;
					SendMessage(hRegressionWnd, WM_REGRESSION_TEST_SET_PLAYPAUSE, reinterpret_cast<LPARAM> (&m_RunMode), 0);
					runState.bModeReset = true;
					runState.bFirst = true;
					runState.bRunFlag = false;
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
			SendMessage(hRegressionWnd, WM_REGRESSION_TEST_SET_PLAYPAUSE, reinterpret_cast<LPARAM> (&m_RunMode), 0);
		}

		//send image info to the dialog...
		if (runState.bRunFlag)
		{
			rIP.GetToolSet()->setObjectID(m_objectIDParams.m_currentObjectID);
			rIP.GetToolSet()->setTriggerIndex(m_triggerIndex);
			rIP.GetToolSet()->setTriggerPerObjectID(m_objectIDParams.m_triggerPerObjectID);
			runState.bRunFlag = false;
			SendMessage(hRegressionWnd, WM_REGRESSION_TEST_SET_FILENAMES, reinterpret_cast<LPARAM> (&fileNameVec), 0);
			runOnce(rIP);
			fileNameVec.clear();
		}

		if (!m_isStopping)
		{
			if (m_RunMode == RegModePlay)
			{
				int iMS = m_iTimeoutMS;
				if (iMS < MinRegressionTime)
				{
					iMS = MinRegressionTime;
				}
				Sleep(iMS);
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

		if (RegModePlay == m_RunMode && m_UsePlayCondition && rIP.shouldPauseRegressionTestByCondition())
		{
			setPlayPause(hRegressionWnd, runState);
		}
	}//end of while loop

	//let the IP know that the regression test is done.
	PostMessage(hMainWnd, WM_COMMAND, WM_REGRESSION_TEST_COMPLETE, 0L);
	//let the RegressionRunDlg know that it is to shut down...
	SendMessage(hRegressionWnd, WM_REGRESSION_TEST_CLOSE_REGRESSION, 0, 0);

	const auto& rCameraList = GetCameras(rIP);
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

void RegressionTestController::setPlayPause(HWND hRegressionWnd, RegressionRuningState& runState)
{
	m_RunMode = RegModePause;
	SendMessage(hRegressionWnd, WM_REGRESSION_TEST_SET_PLAYPAUSE, reinterpret_cast<LPARAM> (&m_RunMode), 0);
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
	if (RegressionRunModeEnum::RegModeBackToBeginningStop == m_RunMode)
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