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

#include "SVLibrary/InitialInformation.h"

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
	HRESULT LoadAcquisitionTriggerDLL();

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

	SVString m_DigitalOption;			// TRB - Added to have a parameter to send to Rabbit Board.

	SVDigitizerLoadLibraryClass m_svDLLDigitizers;
	SVDigitizerLoadLibraryClass m_svDLLFileAcquisition;

	SVIOTriggerLoadLibraryClass m_svDLLTriggers;
	SVIOTriggerLoadLibraryClass m_svDLLSoftwareTriggers;
	SVIOTriggerLoadLibraryClass m_svDLLAcquisitionTriggers;

	SvLib::InitialInformation m_InitialInfo;
#pragma endregion Member variables


};


