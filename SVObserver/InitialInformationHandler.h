//*****************************************************************************
/// \copyright COPYRIGHT (c) 2017,2017 by Seidenader Maschinenbau GmbH
/// \file InitialInformationHandler.h
/// All Rights Reserved
//*****************************************************************************
/// Declaration of class InitialInformationHandler
/// which oversees the loading and unloading of various DLLs as well as the 
/// evaluation reading of INI files
//******************************************************************************

#pragma once

#pragma region Includes
#include "SVIOLibrary/SVIOTriggerLoadLibraryClass.h"
#include "SVLibrary/InitialInformation.h"
#include "Triggering/SVDigitizerLoadLibraryClass.h"
#pragma endregion Includes

class InitialInformationHandler
{
public:
#pragma region Public Methods

	InitialInformationHandler(): m_InitializationStatusFlags(SV_HARDWARE_FAILURE_ALL) {}

	void ResetInformation();

	void LoadIniFilesAndDlls();

	HRESULT LoadTriggerDLL();
	HRESULT CloseTriggerDLL();

	HRESULT LoadSoftwareTriggerDLL();
	HRESULT LoadCameraTriggerDLL();

	HRESULT LoadAcquisitionDLL();
	HRESULT CloseAcquisitionDLL();

	HRESULT LoadFileAcquisitionDLL();

	HRESULT LoadDigitalDLL();
	HRESULT CloseDigitalDLL();

	HRESULT INIReset();
	HRESULT INIClose();

	const SvLib::InitialInformation &GetInitialInfo() { return m_InitialInfo; }

	HRESULT GetInitializationStatusFlags() { return m_InitializationStatusFlags; }

#pragma endregion Public Methods

#pragma region Member variables
private:

	HRESULT m_InitializationStatusFlags;

	std::string m_IOBoardOption;			// TRB - Added to have a parameter to send to Rabbit Board. (original name: m_DigitalOption)

	SvTrig::SVDigitizerLoadLibraryClass m_dllDigitizers;
	SvTrig::SVDigitizerLoadLibraryClass m_dllFileAcquisition;

	SVIOTriggerLoadLibraryClass m_svDLLTriggers;
	SVIOTriggerLoadLibraryClass m_svDLLSoftwareTriggers;
	SVIOTriggerLoadLibraryClass m_svDLLCameraTriggers;

	SvLib::InitialInformation m_InitialInfo;
#pragma endregion Member variables


};


