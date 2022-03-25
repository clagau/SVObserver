//*****************************************************************************
// \copyright COPYRIGHT (c) 2021,2021 by Körber Pharma Inspection GmbH. All Rights Reserved
/// \file RegressionTestController.h
/// This is the class for control the RegressionTest
//******************************************************************************

#pragma once
#include "SVInspectionProcess.h"
#include "SVRegressionTestStruct.h"
#include "ObjectInterfaces/IFormulaController.h"
#include "Triggering/ObjectIDParameters.h"



class RegressionTestController
{
	struct RegressionRuningState
	{
		bool bFirst = true;
		bool bRunFlag = false;
		bool bListDone = false;
		bool bModeReset = false;
		bool isLastImage = false;
	};

public:
	RegressionTestController() = default;
	RegressionRunFileStruct RegressionTestSetFiles(RegressionTestStruct& rRegTestStruct, RegressionRuningState& runState);
	DWORD runThread();

	void initialize(HWND hMainWnd, HWND hRegressionWnd, SVInspectionProcess* pInspectionProcess, const SvTrig::ObjectIDParameters& rObjectIDParams, SVObjectPtrVector&& OutputValueList);
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
	void setValidationMode(bool validationMode) { m_isValidationMode = validationMode; }
	void setValidationFile(LPCTSTR filePath) { m_resultFileName = filePath; }

private:
	SvIe::SVVirtualCameraPtrVector GetCameras(const SVInspectionProcess& rIP) const;
	bool runOnce(SVInspectionProcess& rIP);
	void setPlayPause(RegressionRuningState& runState);
	bool setImagesForNextRun(SVInspectionProcess& rIP, RegressionRuningState& runState, std::back_insert_iterator<std::vector<RegressionRunFileStruct>> fileNameInserter);

private:
	HWND m_hMainWnd {nullptr};
	HWND m_hRegressionWnd {nullptr};
	SVInspectionProcess* m_pInspection {nullptr};
	SVObjectPtrVector m_OutputValueList;
	RegressionRunModeEnum m_RunMode {RegressionRunModeEnum::RegModePause};
	RegressionPlayModeEnum m_RunPlayMode {RegressionPlayModeEnum::RunToEnd};
	int m_iTimeoutMS = 0;
	bool m_isRunning {false};
	bool m_isStopping {false};
	bool m_UsePlayCondition {false};
	bool m_IsInitEquationText {false};
	bool m_isValidationMode {false};
	long m_triggerIndex {0L};
	std::string m_resultFileName;
	std::ofstream m_fileOutputResult;
	SvOi::IFormulaControllerPtr m_pPlayEquationController;
	std::string m_LoadEquationText;
	SvTrig::ObjectIDParameters m_objectIDParams;
	std::vector<RegressionTestStruct> m_regCameras;
	std::vector<RegressionTestStruct> m_regImages;
};