#pragma once
// The following ifdef block is the standard way of creating macros which make exporting 
// from a DLL simpler. All files within this DLL are compiled with the TOOLDLL_EXPORTS
// symbol defined on the command line. this symbol should not be defined on any project
// that uses this DLL. This way any other project whose source files include this file see 
// TOOLDLL_API functions as being imported from a DLL, wheras this DLL sees symbols
// defined with this macro as being exported.

#ifdef TOOLDLL_EXPORTS
#define TOOLDLL_API __declspec(dllexport)
#else
#define TOOLDLL_API __declspec(dllimport)
#endif

#include <map>

#include "CDllTool.h"
#include "Guid.h"
#include "defines.h"

HINSTANCE hReporterModule;

typedef std::map< GUID, CDllTool* > MapToolsType;
MapToolsType g_DllTools;
CRITICAL_SECTION g_DllToolsCritSec;

/**********
This lookup for the tool
\param guidTool the gui id
\param rpDllTool <out> the tool reference
\return hresult value
***********/
HRESULT LookupTool(GUID guidTool, CDllTool*& rpDllTool );

// exports
TOOLDLL_API long    __stdcall SVSimpleTest (long lValue1, long lValue2);
TOOLDLL_API HRESULT __stdcall SVGetToolName (BSTR* pbstrToolName);
TOOLDLL_API HRESULT __stdcall SVGetToolVersion (long* plVersionNumber);
TOOLDLL_API HRESULT __stdcall SVRunTool (GUID tool, long* plStatus);
TOOLDLL_API HRESULT __stdcall SVStartup ();
TOOLDLL_API HRESULT __stdcall SVShutDown ();
TOOLDLL_API HRESULT __stdcall SVInitializeRun( GUID tool, long lImageArraySize, ImageDefinitionStruct* paStructs, long lValueArraySize, VARIANT* pavInputValues);
TOOLDLL_API HRESULT __stdcall SVUninitializeRun( GUID tool );
TOOLDLL_API HRESULT __stdcall SVGetInputValueDefinitions (long* plArraySize,InputValueDefinitionStruct** ppaStructs);
TOOLDLL_API HRESULT __stdcall SVDestroyInputValueDefinitionStructures (InputValueDefinitionStruct* paStructs);
TOOLDLL_API HRESULT __stdcall SVSetInputValues (GUID tool, long lArraySize, VARIANT* paInputValues);
TOOLDLL_API HRESULT __stdcall SVGetResultValues (GUID tool, long lArraySize, VARIANT* paResultValues);
TOOLDLL_API HRESULT __stdcall SVGetErrorMessageString (unsigned long ulErrorNumber,BSTR* pbstrErrorMessage);
TOOLDLL_API HRESULT __stdcall SVValidateValueParameter (GUID tool, long lParameterNumber, VARIANT vParameterValue);
TOOLDLL_API HRESULT __stdcall SVGetResultValueDefinitions (long* plArraySize, ResultValueDefinitionStruct** ppaResultValues);
TOOLDLL_API HRESULT __stdcall SVDestroyResultValueDefinitionStructures (ResultValueDefinitionStruct* paStructs);
TOOLDLL_API HRESULT __stdcall SVGetNumberOfInputImages (long* plNumberOfInputImages);
TOOLDLL_API HRESULT __stdcall SVSetMILInputImages (GUID tool, long lArraySize, long* paMILhandles);
TOOLDLL_API HRESULT __stdcall SVSetHBITMAPInputImages (GUID tool, HBITMAP* paHandles);
TOOLDLL_API HRESULT __stdcall SVGetHBITMAPResultImages (GUID tool, long lArraySize, HBITMAP* paHandles);
TOOLDLL_API HRESULT __stdcall SVSetMILResultImages (GUID tool, long lArraySize, long* paMILhandles);
TOOLDLL_API HRESULT __stdcall SVGetNumberOfResultImages (long* plNumberOfResultImages);
TOOLDLL_API HRESULT __stdcall SVGetResultImageDefinitions (GUID tool, long* plArraySize, ImageDefinitionStruct** ppaImageDefinitions);
TOOLDLL_API HRESULT __stdcall SVDestroyImageDefinitionStructure (ImageDefinitionStruct* paStructs);
