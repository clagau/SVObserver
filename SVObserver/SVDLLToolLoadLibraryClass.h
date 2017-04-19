// ******************************************************************************
// * COPYRIGHT (c) 2003 by SVResearch, Harrisburg
// * All Rights Reserved
// ******************************************************************************
// * .Module Name     : SVDLLToolLoadLibraryClass
// * .File Name       : $Workfile:   SVDLLToolLoadLibraryClass.h  $
// * ----------------------------------------------------------------------------
// * .Current Version : $Revision:   1.2  $
// * .Check In Date   : $Date:   21 Oct 2013 08:21:26  $
// ******************************************************************************

#pragma once

#pragma warning (push)
#pragma warning (disable : 4290)

#pragma region Includes
//Moved to precompiled header: #include <boost/function>
#include "SVDLLToolDefinitionStructs.h"
#include "SVMessage\SVMessage.h"
#include "SVStatusLibrary\MessageManager.h"
#pragma endregion Includes

struct SVImageDefinitionStruct;

enum SVMachineTypeEnum
{
	ImageFileUnknownMachine = 0x0,
	ImageFileMachineAM33 = 0x1d3,
	ImageFileMachineAMD64 = 0x8664,
	ImageFileMachineARM = 0x1c0,
	ImageFileMachineEBC = 0xebc,
	ImageFileMachineI386 = 0x14c,
	ImageFileMachineIA64 = 0x200,
	ImageFileMachineM32R = 0x9041,
	ImageFileMachineMIPS16 = 0x266,
	ImageFileMachineMIPSFPU = 0x366,
	ImageFileMachineMIPSFPU16 = 0x466,
	ImageFileMachinePowerPC = 0x1f0,
	ImageFileMachinePowerPCFP = 0x1f1,
	ImageFileMachineR4000 = 0x166,
	ImageFileMachineSH3 = 0x1a2,
	ImageFileMachineSH3DSP = 0x1a3,
	ImageFileMachineSH4 = 0x1a6,
	ImageFileMachineSH5 = 0x1a8,
	ImageFileMachineThumb = 0x1c2,
	ImageFileMachineWCEMIPSV2 = 0x169
};

typedef long    (__stdcall *SimpleTestPtr) (long lValue1, long lValue2);
typedef HRESULT (__stdcall *GetToolNamePtr) (BSTR* pbstrToolName);
typedef HRESULT (__stdcall *GetToolVersionPtr) (long* plVersionNumber);
typedef HRESULT (__stdcall *RunToolPtr) (GUID tool, long* plStatus);
typedef HRESULT (__stdcall *StartupPtr) ();
typedef HRESULT (__stdcall *ShutDownPtr) ();
typedef HRESULT (__stdcall *InitializeRunPtr) ( GUID tool, long lImageArraySize, SVImageDefinitionStruct* paStructs, long lValueArraySize, VARIANT* pavInputValues);
typedef HRESULT (__stdcall *UninitializeRunPtr) ( GUID tool );
typedef HRESULT (__stdcall *GetInputValueDefinitionsPtr) (long* plArraySize, InputValueDefinitionStruct** ppaStructs);
typedef HRESULT (__stdcall *DestroyInputValueDefinitionStructuresPtr) (InputValueDefinitionStruct* paStructs);
typedef HRESULT (__stdcall *SetInputValuesPtr) (GUID tool, long lArraySize, VARIANT* paInputValues);
typedef HRESULT (__stdcall *GetResultValuesPtr) (GUID tool, long lArraySize, VARIANT* paResultValues);
typedef HRESULT (__stdcall *GetMessageStringPtr) (unsigned long ulErrorNumber, BSTR* pbstrErrorMessage);
typedef HRESULT (__stdcall *ValidateValueParameterPtr) (GUID tool, long lParameterNumber, VARIANT vParameterValue);
typedef HRESULT (__stdcall *GetResultValueDefinitionsPtr) (long* plArraySize, ResultValueDefinitionStruct** ppaResultValues);
typedef HRESULT (__stdcall *DestroyResultValueDefinitionStructuresPtr) (ResultValueDefinitionStruct* paStructs);
typedef HRESULT (__stdcall *GetNumberOfInputImagesPtr) (long* plNumberOfInputImages);
typedef HRESULT (__stdcall *SetMILInputImagesPtr) (GUID tool, long lArraySize, long* paMILhandles);
typedef HRESULT (__stdcall *SetHBITMAPInputImagesPtr) (GUID tool, HBITMAP* paHandles);
typedef HRESULT (__stdcall *GetHBITMAPResultImagesPtr) (GUID tool, long lArraySize, HBITMAP* paHandles);
typedef HRESULT (__stdcall *SetMILResultImagesPtr) (GUID tool, long lArraySize, long* paMILhandles);
typedef HRESULT (__stdcall *GetResultImageDefinitionsPtr) (GUID tool, long* plArraySize, SVImageDefinitionStruct** ppaImageDefinitions);
typedef HRESULT (__stdcall *DestroyImageDefinitionStructurePtr) ( SVImageDefinitionStruct* paStructs);


typedef boost::function< void(LPCTSTR) > SVDllLoadLibraryCallback;


class SVDLLToolLoadLibraryClass  
{
public:
	SVDLLToolLoadLibraryClass();
	~SVDLLToolLoadLibraryClass();

	HRESULT Open( LPCTSTR p_szLibrary, SVDllLoadLibraryCallback fnNotifyProgress = [](LPCTSTR) {});
	HRESULT Close();
	bool IsOpen();

	HRESULT GetToolName (BSTR* pbstrToolName);
	HRESULT GetToolVersion (long* plVersionNumber);
	HRESULT RunTool (GUID tool, long* plStatus);
	HRESULT Startup ();
	HRESULT ShutDown ();
	HRESULT InitializeRun ( GUID tool, long lImageArraySize, SVImageDefinitionStruct* paStructs, long lValueArraySize, VARIANT* pavInputValues);
	HRESULT UninitializeRun ( GUID tool );
	HRESULT GetInputValueDefinitions (long* plArraySize,InputValueDefinitionStruct** ppaStructs);
	HRESULT DestroyInputValueDefinitionStructures (InputValueDefinitionStruct* paStructs);
	HRESULT SetInputValues (GUID tool, long lArraySize, VARIANT* paInputValues);
	HRESULT GetResultValues (GUID tool, long lArraySize, VARIANT* paResultValues);
	HRESULT GetMessageString (unsigned long ulErrorNumber, BSTR* pbstrErrorMessage) const;
	HRESULT ValidateValueParameter (GUID tool, long lParameterNumber, VARIANT vParameterValue);
	HRESULT GetResultValueDefinitions (long* plArraySize, ResultValueDefinitionStruct** ppaResultValues);
	HRESULT DestroyResultValueDefinitionStructures (ResultValueDefinitionStruct* paStructs);
	HRESULT GetNumberOfInputImages (long* plNumberOfInputImages);
	HRESULT SetMILInputImages (GUID tool, long lArraySize, long* paMILhandles);
	HRESULT SetHBITMAPInputImages (GUID tool, HBITMAP* paHandles);
	HRESULT GetHBITMAPResultImages (GUID tool, long lArraySize, HBITMAP* paHandles);
	HRESULT SetMILResultImages (GUID tool, long lArraySize, long* paMILhandles);
	HRESULT GetResultImageDefinitions (GUID tool, long* plArraySize, SVImageDefinitionStruct** ppaImageDefinitions);
	HRESULT DestroyImageDefinitionStructure ( SVImageDefinitionStruct* paStructs);
	bool UseMil();
	BOOL IsHandleNull();
private:
	HMODULE m_hmHandle;

	bool m_bUseMil;

	SimpleTestPtr m_pfnSimpleTest;
	GetToolNamePtr m_pfnGetToolName;
	GetToolVersionPtr m_pfnGetToolVersion;
	RunToolPtr m_pfnRunTool;
	StartupPtr m_pfnStartup;
	ShutDownPtr m_pfnShutDown;
	InitializeRunPtr m_pfnInitializeRun;
	UninitializeRunPtr m_pfnUninitializeRun;
	GetInputValueDefinitionsPtr m_pfnGetInputValueDefinitions;
	DestroyInputValueDefinitionStructuresPtr m_pfnDestroyInputValueDefinitionStructures;
	SetInputValuesPtr m_pfnSetInputValues;
	GetResultValuesPtr m_pfnGetResultValues;
	GetMessageStringPtr m_pfnGetMessageString;
	ValidateValueParameterPtr m_pfnValidateValueParameter;
	GetResultValueDefinitionsPtr m_pfnGetResultValueDefinitions;
	DestroyResultValueDefinitionStructuresPtr m_pfnDestroyResultValueDefinitionStructures;
	GetNumberOfInputImagesPtr m_pfnGetNumberOfInputImages;
	SetMILInputImagesPtr m_pfnSetMILInputImages;
	SetHBITMAPInputImagesPtr m_pfnSetHBITMAPInputImages;
	GetHBITMAPResultImagesPtr m_pfnGetHBITMAPResultImages;
	SetMILResultImagesPtr m_pfnSetMILResultImages;
	GetResultImageDefinitionsPtr m_pfnGetResultImageDefinitions;
	DestroyImageDefinitionStructurePtr m_pfnDestroyImageDefinitionStructure;
	SVMachineTypeEnum CheckBitness( LPCTSTR p_szFile );

};

#include "SVDLLToolLoadLibraryClass.inl"

#pragma warning (pop)


