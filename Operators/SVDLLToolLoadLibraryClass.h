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
//Moved to precompiled header: #include <functional>
#include "SVDLLToolDefinitionStructs.h"
#pragma endregion Includes

struct SVImageDefinitionStruct;

namespace SvStl
{
	class MessageContainer;

}
namespace SvOp
{

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
typedef HRESULT(__stdcall *GetInputValueDefinitionsPtr) (long* plArraySize, InputValueDefinitionStruct** ppaStructs);
typedef HRESULT (__stdcall *DestroyInputValueDefinitionStructuresPtr) (InputValueDefinitionStruct* paStructs);
typedef HRESULT(__stdcall *GetInputValueDefinitionsPtrEx) (long* plArraySize, InputValueDefinitionStructEx** ppaStructs);
typedef HRESULT(__stdcall *DestroyInputValueDefinitionStructuresPtrEx) (InputValueDefinitionStructEx* paStructs);


typedef HRESULT (__stdcall *SetInputValuesPtr) (GUID tool, long lArraySize, VARIANT* paInputValues);
typedef HRESULT (__stdcall *GetInputImageInformationPtr) (long* plArraySize, InputImageInformationStruct** ppaStructs);
typedef HRESULT (__stdcall *GetResultValuesPtr) (GUID tool, long lArraySize, VARIANT* paResultValues);
typedef HRESULT (__stdcall *GetMessageStringPtr) (unsigned long ulErrorNumber, BSTR* pbstrErrorMessage);
typedef HRESULT (__stdcall *ValidateValueParameterPtr) (GUID tool, long lParameterNumber, VARIANT vParameterValue);
typedef HRESULT (__stdcall *GetResultValueDefinitionsPtr) (long* plArraySize, ResultValueDefinitionStruct** ppaResultValues);
typedef HRESULT(__stdcall *GetResultValueDefinitionsPtrEx) (long* plArraySize, ResultValueDefinitionStructEx** ppaResultValues);
typedef HRESULT (__stdcall *DestroyResultValueDefinitionStructuresPtr) (ResultValueDefinitionStruct* paStructs);
typedef HRESULT(__stdcall *DestroyResultValueDefinitionStructuresPtrEx) (ResultValueDefinitionStructEx* paStructs);
typedef HRESULT(__stdcall* DestroyInputImageInformationPtr) (InputImageInformationStruct* paStructs);

typedef HRESULT (__stdcall *GetNumberOfInputImagesPtr) (long* plNumberOfInputImages);
typedef HRESULT (__stdcall *SetMILInputImagesPtr) (GUID tool, long lArraySize, long* paMILhandles);
typedef HRESULT (__stdcall *SetHBITMAPInputImagesPtr) (GUID tool, HBITMAP* paHandles);
typedef HRESULT (__stdcall *GetHBITMAPResultImagesPtr) (GUID tool, long lArraySize, HBITMAP* paHandles);
typedef HRESULT (__stdcall *SetMILResultImagesPtr) (GUID tool, long lArraySize, long* paMILhandles);
typedef HRESULT (__stdcall *GetResultImageDefinitionsPtr) (GUID tool, long* plArraySize, SVImageDefinitionStruct** ppaImageDefinitions);
typedef HRESULT (__stdcall *DestroyImageDefinitionStructurePtr) ( SVImageDefinitionStruct* paStructs);
typedef HRESULT(__stdcall *GetResultTableDefinitionsPtr) (long* pSize, ResultTableDefinitionStruct** ppaResultValues);
typedef HRESULT(__stdcall *GetResultTableDefinitionsPtrEx) (long* pSize, ResultTableDefinitionStructEx** ppaResultValues);
typedef HRESULT(__stdcall *DestroyResultTableDefinitionStructuresPtr) (ResultTableDefinitionStruct* paStructs);
typedef HRESULT(__stdcall *DestroyResultTableDefinitionStructuresPtrEx) (ResultTableDefinitionStructEx* paStructs);
typedef HRESULT(__stdcall *GetResultTablesPtr) (GUID tool, long lArraySize, VARIANT* paResultValues);
typedef HRESULT(__stdcall *GetResultTablesMaxRowSizePtr) (GUID tool, long  Size, int pRowSizes[]);
typedef HRESULT(__stdcall *GetResultValuesMaxArraySizePtr) (GUID tool, long  Size, int pArraySizes[]);



typedef std::function< void(LPCTSTR) > SVDllLoadLibraryCallback;


class SVDLLToolLoadLibraryClass  
{
public:
	SVDLLToolLoadLibraryClass();
	~SVDLLToolLoadLibraryClass();

	HRESULT Open(LPCTSTR libraryName, std::vector<std::string>& rStatusMsgs);
	HRESULT Close();

	HRESULT GetToolName (BSTR* pbstrToolName);
	HRESULT GetToolVersion (long* plVersionNumber);
	HRESULT RunTool (uint32_t toolId, long* plStatus);
	HRESULT Startup ();
	HRESULT ShutDown ();
	HRESULT InitializeRun (uint32_t toolId, long lImageArraySize, SVImageDefinitionStruct* paStructs, long lValueArraySize, VARIANT* pavInputValues);
	HRESULT UninitializeRun (uint32_t toolId);
	HRESULT GetInputValueDefinitions (long* plArraySize,InputValueDefinitionStruct** ppaStructs);
	HRESULT DestroyInputValueDefinitionStructures (InputValueDefinitionStruct* paStructs);
	HRESULT GetInputValueDefinitions(long* plArraySize, InputValueDefinitionStructEx** ppaStructs);
	HRESULT DestroyInputValueDefinitionStructures(InputValueDefinitionStructEx* paStructs);

	HRESULT SetInputValues (uint32_t toolId, long lArraySize, VARIANT* paInputValues);
	HRESULT GetInputImageInformation(std::vector<SvOp::InputImageInformationStruct> *pVector);
	HRESULT GetResultValues (uint32_t toolId, long lArraySize, VARIANT* paResultValues);
	HRESULT GetMessageString (unsigned long ulErrorNumber, BSTR* pbstrErrorMessage) const;
	HRESULT ValidateValueParameter(uint32_t toolId, long lParameterNumber, VARIANT vParameterValue);
	HRESULT GetResultValueDefinitions (long* plArraySize, ResultValueDefinitionStruct** ppaResultValues);
	HRESULT GetResultValueDefinitions(long* plArraySize, ResultValueDefinitionStructEx** ppaResultValues);
	HRESULT DestroyResultValueDefinitionStructures (ResultValueDefinitionStruct* paStructs);
	HRESULT DestroyResultValueDefinitionStructures(ResultValueDefinitionStructEx* paStructs);
	HRESULT GetNumberOfInputImages (long* plNumberOfInputImages);
	HRESULT SetMILInputImages (uint32_t toolId, long lArraySize, long* paMILhandles);
	HRESULT SetHBITMAPInputImages (uint32_t toolId, HBITMAP* paHandles);
	HRESULT GetHBITMAPResultImages (uint32_t toolId, long lArraySize, HBITMAP* paHandles);
	HRESULT SetMILResultImages (uint32_t toolId, long lArraySize, long* paMILhandles);
	HRESULT GetResultImageDefinitions (uint32_t toolId, long* plArraySize, SVImageDefinitionStruct** ppaImageDefinitions);
	HRESULT DestroyImageDefinitionStructure ( SVImageDefinitionStruct* paStructs);

	HRESULT getResultTableDefinitions(long* Size, ResultTableDefinitionStruct** ppaResultTableDefs);
	HRESULT getResultTableDefinitions(long* Size, ResultTableDefinitionStructEx** ppaResultTableDefs);
	HRESULT destroyResultTableDefinitionStructures(ResultTableDefinitionStruct* paStructs);
	HRESULT destroyResultTableDefinitionStructures(ResultTableDefinitionStructEx* paStructs);
	HRESULT getResultTables(uint32_t toolId, long lArraySize, VARIANT* paResultValues);

	HRESULT getResultTablesMaxRowSize(uint32_t toolId, long  Size, int pRowSizes[]);
	HRESULT getResultValuesMaxArraySize(uint32_t toolId, long  Size, int pArraySizes[]);

	
	bool UseResultTablesMaxRowSize() const;
	bool  UseResultValuesMaxArraySize() const;
	bool UseMil();
	bool IsHandleNull();
	bool UseTableOutput() const;


	bool UseResultEx()
	{
		return m_bUseResultEx;
	}
	bool UseInputEx()
	{
		return m_bUseInputEx;
	}
private:
	
	SVMachineTypeEnum CheckBitness(LPCTSTR p_szFile);
	HRESULT  InitResultValueDef(SvStl::MessageContainer& rR);
	HRESULT  InitInputValueDef(SvStl::MessageContainer& rR);

	HMODULE m_hmHandle {nullptr};
	bool m_bUseMil {false};
	bool m_bUseResultEx {false};
	bool m_bUseInputEx {false};
	SimpleTestPtr m_pfnSimpleTest {nullptr};
	GetToolNamePtr m_pfnGetToolName {nullptr};
	GetToolVersionPtr m_pfnGetToolVersion {nullptr};
	RunToolPtr m_pfnRunTool {nullptr};
	StartupPtr m_pfnStartup {nullptr};
	ShutDownPtr m_pfnShutDown {nullptr};
	InitializeRunPtr m_pfnInitializeRun {nullptr};
	UninitializeRunPtr m_pfnUninitializeRun {nullptr};
	GetInputValueDefinitionsPtr m_pfnGetInputValueDefinitions {nullptr};
	DestroyInputValueDefinitionStructuresPtr m_pfnDestroyInputValueDefinitionStructures {nullptr};
	GetInputValueDefinitionsPtrEx m_pfnGetInputValueDefinitionsEx {nullptr};
	DestroyInputValueDefinitionStructuresPtrEx m_pfnDestroyInputValueDefinitionStructuresEx {nullptr};
	SetInputValuesPtr m_pfnSetInputValues {nullptr};
	GetInputImageInformationPtr m_pfnGetInputImageInformation {nullptr};
	DestroyInputImageInformationPtr m_pfnDestroyInputImageInformationPtr{ nullptr };
	GetResultValuesPtr m_pfnGetResultValues {nullptr};
	GetMessageStringPtr m_pfnGetMessageString {nullptr};
	ValidateValueParameterPtr m_pfnValidateValueParameter {nullptr};
	GetResultValueDefinitionsPtr m_pfnGetResultValueDefinitions {nullptr};
	GetResultValueDefinitionsPtrEx m_pfnGetResultValueDefinitionsEx {nullptr};
	DestroyResultValueDefinitionStructuresPtr m_pfnDestroyResultValueDefinitionStructures {nullptr};
	DestroyResultValueDefinitionStructuresPtrEx m_pfnDestroyResultValueDefinitionStructuresEx {nullptr};
	GetNumberOfInputImagesPtr m_pfnGetNumberOfInputImages {nullptr};
	SetMILInputImagesPtr m_pfnSetMILInputImages {nullptr};
	SetHBITMAPInputImagesPtr m_pfnSetHBITMAPInputImages {nullptr};
	GetHBITMAPResultImagesPtr m_pfnGetHBITMAPResultImages {nullptr};
	SetMILResultImagesPtr m_pfnSetMILResultImages {nullptr};
	GetResultImageDefinitionsPtr m_pfnGetResultImageDefinitions {nullptr};
	DestroyImageDefinitionStructurePtr m_pfnDestroyImageDefinitionStructure {nullptr};
	GetResultTableDefinitionsPtr m_pfnGetResultTableDefinitions {nullptr};
	GetResultTableDefinitionsPtrEx m_pfnGetResultTableDefinitionsEx {nullptr};
	DestroyResultTableDefinitionStructuresPtr m_pfnDestroyResultTableDefinitionStructures {nullptr};
	DestroyResultTableDefinitionStructuresPtrEx m_pfnDestroyResultTableDefinitionStructuresEx {nullptr};
	GetResultTablesPtr  m_pfnGetResultTables {nullptr};
	GetResultTablesMaxRowSizePtr m_pfnGetResultTablesMaxRowSize {nullptr};
	GetResultValuesMaxArraySizePtr m_pfnGetResultValuesMaxArraySize {nullptr};
};

} //namespace SvOp

#pragma warning (pop)
