//*****************************************************************************
// \copyright COPYRIGHT (c) 2021,2021 by Seidenader Maschinenbau GmbH. All Rights Reserved
/// \file RegressionTestController.h
/// This is the class for control the RegressionTest
//******************************************************************************

#pragma once
#include "SVRegressionTestStruct.h"
#include "ObjectInterfaces/IFormulaController.h"
#include "SVInspectionProcess.h"


class RegressionTestController
{
	enum
	{
		MinRegressionTime = 40,
	};

	struct RegressionRuningState
	{
		bool bFirst = true;
		bool bRunFlag = false;
		bool bListDone = false;
		bool bModeReset = false;
		bool isLastImage = false;
	};

public:
	RegressionRunFileStruct RegressionTestSetFiles(RegressionTestStruct& rRegTestStruct, RegressionRuningState& runState);
	DWORD runThread(HWND hMainWnd, HWND hRegressionWnd, SVInspectionProcess& rIP);

	void stopRunning();
	void resetEquationText();
	SvOi::IFormulaControllerPtr getPlayEquationController() const { return m_pPlayEquationController; };
	RegressionRunModeEnum getRunMode() { return m_RunMode; };
	void setRunMode(RegressionRunModeEnum newMode) { m_RunMode = static_cast<RegressionRunModeEnum>(newMode); };
	void setPlayMode(RegressionPlayModeEnum newPlayMode) { m_RunPlayMode = newPlayMode; };
	void usePlayCondition(bool usePlayCondition) { m_UsePlayCondition = usePlayCondition; };
	bool usePlayCondition() { return m_UsePlayCondition; };
	void setPlayEquationController(SvOi::IFormulaControllerPtr pRegressionTestPlayEquationController) { m_pPlayEquationController = pRegressionTestPlayEquationController; }
	std::vector<RegressionTestStruct>& getRegCameras() { return m_regCameras; };
	std::vector<RegressionTestStruct>& getRegImages() { return m_regImages; };
	void setTimeoutPeriod(int p_TimeoutMS) { m_iTimeoutMS = p_TimeoutMS; };
	bool isRunning() const { return m_isRunning; };
	void setLoadEquationText(const std::string& rText) { m_LoadEquationText = rText; };

private:
	SvIe::SVVirtualCameraPtrVector GetCameras(SVInspectionProcess& rIP) const;
	bool runOnce(SVInspectionProcess& rIP);
	void setPlayPause(HWND hRegressionWnd, RegressionRuningState& runState);
	bool setImagesForNextRun(SVInspectionProcess& rIP, RegressionRuningState& runState, std::back_insert_iterator<std::vector<RegressionRunFileStruct>> fileNameInserter);

private:
	RegressionRunModeEnum m_RunMode;
	RegressionPlayModeEnum m_RunPlayMode;
	int m_iTimeoutMS;
	bool m_isRunning{ false };
	bool m_isStopping;
	bool m_UsePlayCondition{ false };
	SvOi::IFormulaControllerPtr m_pPlayEquationController;
	std::string m_LoadEquationText;
	bool m_IsInitEquationText{ false };
	std::vector<RegressionTestStruct> m_regCameras;
	std::vector<RegressionTestStruct> m_regImages;
};