// ExternalDll.cpp : Defines the initialization routines for the DLL.
//

#include "stdafx.h"
#include "ExternalDll.h"
#include "defines.h"
#include "CDllTool.h"
#include "OldStructDefinitions.h"

#ifdef _DEBUG
#include "DebugUtilities.h"
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

bool operator < (const GUID& guid1, const GUID& guid2)
{
	// this does not implement REAL ordering, but enough for our map's purposes
	const BYTE* pBytes1 = reinterpret_cast<const BYTE*> (&guid1);
	const BYTE* pBytes2 = reinterpret_cast<const BYTE*> (&guid2);
	for (int i = 0; i < sizeof(GUID); i++)
	{
		if (pBytes1[i] == pBytes2[i])
			continue;
		else if (pBytes1[i] < pBytes2[i])
			return true;
		else
			return false;
	}
	return false;
}

extern "C" int APIENTRY
DllMain(HINSTANCE, DWORD dwReason, LPVOID /*lpReserved (remove if required*/)
{
	if (dwReason == DLL_PROCESS_ATTACH)
	{
#if _DEBUG
		start_fdb(1);
		fdb("DLL_PROCESS_ATTACH\n");
#endif		
		// Insert this DLL into the resource chain
		// NOTE: If this Extension DLL is being implicitly linked to by
		//  an MFC Regular DLL (such as an ActiveX Control)
		//  instead of an MFC application, then you will want to
		//  remove this line from DllMain and put it in a separate
		//  function exported from this Extension DLL.  The Regular DLL
		//  that uses this Extension DLL should then explicitly call that
		//  function to initialize this Extension DLL.  Otherwise,
		//  the CDynLinkLibrary object will not be attached to the
		//  Regular DLL's resource chain, and serious problems will
		//  result.

#if _DEBUG
		fdb("DLL_PROCESS_ATTACH Vers %d  <%s>\n", static_iToolVersion, static_strToolName);
#endif
	}
	else if (dwReason == DLL_PROCESS_DETACH)
	{
#if _DEBUG
		fdb("DLL_PROCESS_DETACH Vers %d  <%s>\n", static_iToolVersion, static_strToolName);
#endif
		MapToolsType::iterator iter;
		for (iter = g_DllTools.begin(); iter != g_DllTools.end(); ++iter)
		{
			delete iter->second;
		}
		g_DllTools.clear();
	}
	return 1;   // ok
}

HRESULT LookupTool(GUID guidTool, CDllTool*& rpDllTool)
{
	HRESULT hRet = S_OK;

	std::lock_guard<std::mutex> lock {g_DllToolsMutex};
	MapToolsType::iterator iter;
	if ((iter = g_DllTools.find(guidTool)) == g_DllTools.end())
	{
		hRet = S_FALSE;
	}
	else	// use existing
	{
		rpDllTool = iter->second;
	}
	return hRet;
}


// ******* Simple Test function to check that the calling convention is correct.
TOOLDLL_API long __stdcall SVSimpleTest(long lValue1, long lValue2)
{
	UNREFERENCED_PARAMETER(lValue1);
#if _DEBUG
	fdb("===== SVSimpleTest - lValue1=%ld lValue2=%ld\n", lValue1, lValue2);
#endif

	return lValue2;
}

// Gets the name of the DLL Tool
TOOLDLL_API HRESULT __stdcall SVGetToolName(BSTR* pbstrToolName)
{
#if _DEBUG
	fdb("===== SVGetToolName - Enter - pbstrToolName=<%s> <%s>\n", static_strToolName, DLLINTERFACEVERSION);
#endif
	char strTemp[512] = "";
	strcpy_s(strTemp, static_strToolName);
	strcat_s(strTemp, "  ");
	strcat_s(strTemp, DLLINTERFACEVERSION);

	BSTR bstrNew = A2BSTR(strTemp);
	if (bstrNew == NULL) return S_FALSE;

	*pbstrToolName = bstrNew;

#if _DEBUG
	fdb("===== SVGetToolName - Exit\n");
#endif
	return S_OK;
}

// Tool Version 
TOOLDLL_API HRESULT __stdcall SVGetToolVersion(long* plVersionNumber)
{
#if _DEBUG
	fdb("===== SVGetToolVersion - Enter - static_iToolVersion=%d\n", static_iToolVersion);
#endif

	* plVersionNumber = (long)static_iToolVersion;

#if _DEBUG
	fdb("===== SVGetToolVersion - Exit\n");
#endif
	return S_OK;
}



// Run Tool 
TOOLDLL_API HRESULT __stdcall SVRunTool(GUID guidTool, long* plStatus)
{
#if _DEBUG
	fdb("===== SVRunTool - Enter\n");
#endif

	CDllTool* pTool = NULL;
	HRESULT hr = LookupTool(guidTool, pTool);
	if (hr == S_OK && pTool != NULL)
	{
		hr = pTool->run();
	}
	//int iRes=0;
	*plStatus = 0;

#if _DEBUG
	fdb("===== SVRunTool - Exit - hr=0x%X\n", hr);
#endif
	return hr;
}

TOOLDLL_API HRESULT __stdcall SVStartup()
{
#if _DEBUG
	fdb("===== SVStartup\n");
#endif
	return S_OK;
}

TOOLDLL_API HRESULT __stdcall SVShutDown()
{
#if _DEBUG
	fdb("===== SVShutDown\n");
#endif
	return S_OK;
}

TOOLDLL_API HRESULT __stdcall SVInitializeRun(GUID guidTool, long lImageArraySize,
	ImageDefinitionStruct* paStructs, long lValueArraySize, VARIANT* pavInputValues)
{
#if _DEBUG
	fdb("===== SVInitializeRun - Enter - lImageArraySize %d  lValueArraySize %d\n", lImageArraySize, lValueArraySize);
#endif

	if (lImageArraySize == NUM_INPUT_IMAGES && lValueArraySize == NUM_INPUT_VALUES)
	{
		HRESULT hr = S_OK;

		CDllTool* pDllTool = NULL;

		// Check if this tool guid exists
		HRESULT hrLookup = LookupTool(guidTool, pDllTool);
		if (hrLookup != S_OK)
		{	// create new tool
			pDllTool = new CDllTool;
			if (pDllTool == NULL)
			{
#if _DEBUG
				fdb("      SVInitializeRun - Creation of new tool failed\n");
#endif
				return S_FALSE;
			}

#if _DEBUG
			fdb("      SVInitializeRun - New Tool created\n");
#endif

			pDllTool->setGUID(guidTool);

			// store in map
			{
				std::lock_guard<std::mutex> lock {g_DllToolsMutex};
				g_DllTools[guidTool] = pDllTool;
			}
		}

#if _DEBUG
		fdb("===== SVInitializeRun - initRun - hr=0x%X\n", hr);
#endif
		hr = pDllTool->initRun(paStructs, pavInputValues);

#if _DEBUG
		fdb("===== SVInitializeRun - Exit - hr=0x%X\n", hr);
#endif
		return hr;
	} //if (lImageArraySize == NUM_INPUT_IMAGES && lValueArraySize == NUM_INPUT_VALUES)
	else
	{
		return S_FALSE;
	}
}

TOOLDLL_API HRESULT __stdcall SVUninitializeRun(GUID guidTool)
{
#if _DEBUG
	fdb("===== SVUninitializeRun - Enter\n");
#endif

	HRESULT hr = S_OK;

	CDllTool* pDllTool = NULL;

	{
		std::lock_guard<std::mutex> lock {g_DllToolsMutex};

		MapToolsType::iterator iter;
		if ((iter = g_DllTools.find(guidTool)) == g_DllTools.end())
		{
			hr = S_FALSE;
		}
		else
		{
			pDllTool = iter->second;
			g_DllTools.erase(iter);
		}
	}

	if (pDllTool)
	{
		delete pDllTool;// do this outside critical section
	}
#if _DEBUG
	else
	{
		fdb("===== SVUninitializeRun - DO NOT DELETE DLL-TOOL - Nr=0x%X\n", guidTool);
	}
	fdb("===== SVUninitializeRun - Exit - hr=0x%X\n", hr);
#endif
	return hr;
}

#ifdef DEFINE_STRUCTEX 
// cppcheck-suppress unusedFunction
TOOLDLL_API HRESULT __stdcall SVGetInputValueDefinitionsEx(long* plArraySize,
	InputValueDefinitionStructEx** ppaStructs)
{
#if _DEBUG
	fdb("===== SVGetInputValueDefinitions - Enter - NumInputValues %d\n", NUM_INPUT_VALUES);
#endif

	HRESULT hr = S_OK;

	typedef std::basic_string<WCHAR> wstring;

	if (NUM_INPUT_VALUES > 0)
		*ppaStructs = new InputValueDefinitionStructEx[NUM_INPUT_VALUES];
	else
		*ppaStructs = NULL;

	//### check alloc failed
	if (ppaStructs == NULL)
	{
#if _DEBUG
		fdb("      SVGetInputValueDefinitions - allocation InputValueDefinitionStruct failed\n");
#endif
		return S_FALSE;
	}

	int i;
	for (i = 0; i < NUM_INPUT_VALUES; i++)
	{
		::VariantInit(&((*ppaStructs)[i].vDefaultValue));
	}

	/////////////////////////////////////////////////////////////
	/////////////////////////////////////////////////////////////
	//!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
	//!!! Input Start
	//!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!

	/////////////////////////////////////////////////////////////////////////
	//### Algo Input Parameters
	*plArraySize = NUM_INPUT_VALUES;
	using TA = std::array<InputValueDefinitionStructEx, NUM_INPUT_VALUES>;

	CDllTool::getInputValuesDefinitionEx(*(TA*)(*ppaStructs));

#if _DEBUG
	fdb("      SVGetInputValueDefinitions - Checkpoint1\n");
#endif

	//!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
	//!!! Input Stop
	//!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!

	////////////////////////////////////////////////////////////////////////



#if _DEBUG
	fdb("===== SVGetInputValueDefinitions - Exit - hr=0x%X\n", hr);
#endif
	return hr;
}

// cppcheck-suppress unusedFunction
TOOLDLL_API HRESULT __stdcall SVDestroyInputValueDefinitionStructuresEx(
	InputValueDefinitionStructEx* paStructs)
{
#if _DEBUG
	fdb("===== SVDestroyInputValueDefinitionStructures - Enter\n");
#endif

	HRESULT hr = S_OK;
	for (int i = 0; i < NUM_INPUT_VALUES; i++)
	{

		::VariantClear(&paStructs[i].vDefaultValue);
	}
	delete[] paStructs;

#if _DEBUG
	fdb("===== SVDestroyInputValueDefinitionStructures - Exit - hr=0x%X\n", hr);
#endif
	return hr;
}

// cppcheck-suppress unusedFunction
TOOLDLL_API HRESULT __stdcall SVDestroyResultValueDefinitionStructuresEx(ResultValueDefinitionStructEx* paStructs)
{
	delete[] paStructs;
	return S_OK;
}
#endif 

// cppcheck-suppress unusedFunction
TOOLDLL_API HRESULT __stdcall SVSetInputValues(GUID guidTool, long lArraySize,
	VARIANT* paInputValues)
{
	//!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
	//!!! Input Start
	//!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
#if _DEBUG
	fdb("===== SVSetInputValues - Enter - lArraySize=%d = NUM_INPUT_VALUES=%d\n", lArraySize, NUM_INPUT_VALUES);
#endif

	HRESULT hr = S_FALSE;
	if (lArraySize == NUM_INPUT_VALUES)
	{
		CDllTool* pTool = NULL;
		hr = LookupTool(guidTool, pTool);

		if (hr == S_OK)
		{	// found tool
			hr = pTool->setInputValues(paInputValues);
		}
	}
#if _DEBUG
	else
	{
		fdb("      SVSetInputValues - Number of parameters are different!\n");
	}
	fdb("===== SVSetInputValues - Exit - hr=0x%X\n", hr);
#endif
	return hr;
}

// cppcheck-suppress unusedFunction
TOOLDLL_API HRESULT __stdcall SVGetResultValues(GUID guidTool, long lArraySize, VARIANT* paResultValues)
{

	HRESULT hr = S_FALSE;
	if (lArraySize == NUM_RESULT_VALUES)
	{
		CDllTool* pTool = NULL;
		hr = LookupTool(guidTool, pTool);

		if (hr == S_OK)
		{	// found tool
			hr = pTool->getResultValues(paResultValues);
		}
	}

	return hr;
}


// cppcheck-suppress unusedFunction
TOOLDLL_API HRESULT __stdcall SVGetErrorMessageString(unsigned long ulErrorNumber,
	BSTR* pbstrErrorMessage)
{
#if _DEBUG
	fdb("===== SVGetErrorMessageString - Enter: ErrorCode: %x\n", ulErrorNumber);
#endif
	HRESULT hr = CDllTool::getErrorMessageString(ulErrorNumber, pbstrErrorMessage);

#if _DEBUG
	fdb("===== SVGetErrorMessageString - Exit\n");
#endif
	return hr;
}

// cppcheck-suppress unusedFunction
TOOLDLL_API HRESULT __stdcall SVValidateValueParameter(GUID, long lParameterNumber,
	VARIANT vParameterValue)
{
#if _DEBUG
	fdb("===== SVValidateValueParameter - Enter - PNum %d\n", lParameterNumber);

#endif
	//HRESULT hr = S_OK;
	HRESULT hr = CDllTool::validateInputValues(lParameterNumber, vParameterValue);

#if _DEBUG
	fdb("===== SVValidateValueParameter - Exit - hr=0x%X\n", hr);
#endif
	return hr;
}

// cppcheck-suppress unusedFunction
TOOLDLL_API HRESULT __stdcall SVGetResultValueDefinitionsEx(long* plArraySize, ResultValueDefinitionStructEx** ppaResultValues)
{
	HRESULT hr = S_OK;
	*plArraySize = NUM_RESULT_VALUES;
	*ppaResultValues = new ResultValueDefinitionStructEx[NUM_RESULT_VALUES];
	if (nullptr == *ppaResultValues)
	{
		return S_FALSE;
	}
	using TA = std::array<ResultValueDefinitionStructEx, NUM_RESULT_VALUES>;
	CDllTool::getResultValueDefinitionEx(*(TA*)(*ppaResultValues));
	return hr;
}

// cppcheck-suppress unusedFunction
TOOLDLL_API HRESULT __stdcall SVGetNumberOfInputImages(long* plNumberOfInputImages)
{
#if _DEBUG
	fdb("===== SVGetNumberOfInputImages - Enter - NUM_INPUT_IMAGES=%d\n", NUM_INPUT_IMAGES);
#endif

	* plNumberOfInputImages = NUM_INPUT_IMAGES;

	HRESULT hr = S_OK;

#if _DEBUG
	fdb("===== SVGetNumberOfInputImages - Exit - hr=0x%X\n", hr);
#endif
	return hr;
}


// Only implement One of the set of InputImages / ResultImages
// cppcheck-suppress unusedFunction
TOOLDLL_API HRESULT __stdcall SVSetMILInputImages(GUID guidTool, long, long* paMILhandles)
{
#if _DEBUG
	fdb("===== SVSetMILInputImages - Enter\n");
#endif
	HRESULT hr = S_FALSE;

	//	if ( lArraySize == NUM_INPUT_IMAGES )
	{
		CDllTool* pTool = NULL;
		hr = LookupTool(guidTool, pTool);
		if (hr == S_OK)	// found tool
		{
			hr = pTool->setMILInputImages(paMILhandles);
		}
	}

#if _DEBUG
	fdb("===== SVSetMILInputImages - Exit - hr=0x%X\n", hr);
#endif
	return hr;
}

// Only implement One of the set of InputImages / ResultImages
// cppcheck-suppress unusedFunction
TOOLDLL_API HRESULT __stdcall SVSetMILResultImages(GUID guidTool, long lArraySize, long* paMILhandles)
{
#if _DEBUG
	fdb("===== SVSetMILResultImages - Enter\n");
#endif
	HRESULT hr = S_FALSE;

	if (NUM_RESULT_IMAGES == 0)
	{
		// do nothing
		return S_OK;
	}

	if (lArraySize == NUM_RESULT_IMAGES)
	{
		CDllTool* pTool = NULL;
		hr = LookupTool(guidTool, pTool);
		if (hr == S_OK)	// found tool
		{
			hr = pTool->setMILResultImages(paMILhandles);
		}
	}

#if _DEBUG
	fdb("===== SVSetMILResultImages - Exit - hr=0x%X\n", hr);
#endif
	return hr;
}

// cppcheck-suppress unusedFunction
TOOLDLL_API HRESULT __stdcall SVGetResultImageDefinitions(GUID guidTool, long* plArraySize, ImageDefinitionStruct** ppaStructs)
{
#if _DEBUG
	fdb("===== SVGetResultImageDefinitions - Enter\n");
#endif

	HRESULT hr = S_OK;

	if (NUM_RESULT_IMAGES == 0)
	{
		*plArraySize = NUM_RESULT_IMAGES;
		return S_OK;
	}

	CDllTool* pTool = NULL;

	HRESULT hrLookup = LookupTool(guidTool, pTool);
	if (hrLookup != S_OK)
	{
		hr = S_FALSE;
	}
	else // found tool
	{
		hr = pTool->getResultImageDefs(plArraySize, ppaStructs);
	}

#if _DEBUG
	fdb("===== SVGetResultImageDefinitions - Exit - hr=0x%X\n", hr);
#endif
	return hr;
}

// cppcheck-suppress unusedFunction
TOOLDLL_API HRESULT __stdcall SVDestroyImageDefinitionStructure(ImageDefinitionStruct* paStructs)
{
#if _DEBUG
	fdb("===== SVDestroyImageDefinitionStructure - Enter\n");
#endif

	HRESULT hr = S_OK;
	for (int i = 0; i < NUM_RESULT_IMAGES; i++)
	{
		::SysFreeString(paStructs[i].bstrDisplayName);
	}
	delete[] paStructs;

#if _DEBUG
	fdb("===== SVDestroyImageDefinitionStructure - Exit - hr=0x%X\n", hr);
#endif
	return hr;
}

// cppcheck-suppress unusedFunction
TOOLDLL_API HRESULT __stdcall GetResultTableDefinitionsEx(long* pSize, ResultTableDefinitionStructEx** ppaResultTableDefs)
{
	HRESULT hr = S_OK;
	if (NUM_RESULT_TABLES > 0)
	{
		*ppaResultTableDefs = new ResultTableDefinitionStructEx[NUM_RESULT_TABLES];
	}
	else
	{
		*ppaResultTableDefs = NULL;
	}
	///@TODO IST das Richtig?
	if (*ppaResultTableDefs == NULL)
	{

		return S_FALSE;
	}
	*pSize = NUM_RESULT_TABLES;
	using TA = std::array<ResultTableDefinitionStructEx, NUM_RESULT_TABLES>;
	CDllTool::getResultTableDefinitionEx(*(TA*)(*ppaResultTableDefs));

	return hr;
}

// cppcheck-suppress unusedFunction
TOOLDLL_API HRESULT __stdcall DestroyResultTableDefinitionStructuresEx(ResultTableDefinitionStructEx* paStructs)
{
	HRESULT hr = S_OK;
	delete[] paStructs;
	return hr;
}

// cppcheck-suppress unusedFunction
TOOLDLL_API HRESULT __stdcall GetResultTables(GUID guidTool, long lArraySize, VARIANT* paResultValues)
{
	HRESULT hr = S_FALSE;
	if (lArraySize == NUM_RESULT_TABLES)
	{
		CDllTool* pTool = NULL;
		hr = LookupTool(guidTool, pTool);

		if (hr == S_OK)
		{	// found tool
			hr = pTool->getResultTables(paResultValues);
		}
	}
	return hr;
}

// cppcheck-suppress unusedFunction
TOOLDLL_API HRESULT __stdcall GetResultTablesMaxRowSize(GUID guidTool, long Size, int pRowSizes[])
{
	HRESULT hr = S_FALSE;
	if (Size == NUM_RESULT_TABLES)
	{
		CDllTool* pTool = NULL;
		hr = LookupTool(guidTool, pTool);

		if (hr == S_OK)
		{

			hr = pTool->getResultTablesMaxRowSize(*(std::array<int, NUM_RESULT_TABLES>*)pRowSizes);
		}
	}
	return hr;
}


// cppcheck-suppress unusedFunction
TOOLDLL_API HRESULT __stdcall GetResultValuesMaxArraySize(GUID guidTool, long Size, int Arraysize[])
{
	HRESULT hr = S_FALSE;
	if (Size == NUM_RESULT_VALUES)
	{
		CDllTool* pTool = NULL;
		hr = LookupTool(guidTool, pTool);

		if (hr == S_OK)
		{
			hr = pTool->getResultValuesMaxArraySize(*(std::array<int, NUM_RESULT_VALUES>*) Arraysize);
		}
	}
	return hr;
}




//older Interface previous to Observer 10.0

TOOLDLL_API HRESULT __stdcall SVGetInputValueDefinitions(long* plArraySize, InputValueDefinitionStruct** ppaStructs)
{

	HRESULT hr = S_OK;

	typedef std::basic_string<WCHAR> wstring;

	if (NUM_INPUT_VALUES > 0)
		*ppaStructs = new InputValueDefinitionStruct[NUM_INPUT_VALUES];
	else
		*ppaStructs = NULL;



	//### check alloc failed
	if (ppaStructs == NULL)
	{
		return S_FALSE;
	}

	int i;
	for (i = 0; i < NUM_INPUT_VALUES; i++)
	{
		::VariantInit(&((*ppaStructs)[i].vDefaultValue));
	}

	*plArraySize = NUM_INPUT_VALUES;

	std::array<InputValueDefinitionStructEx, NUM_INPUT_VALUES> InputsEx;
	CDllTool::getInputValuesDefinitionEx(InputsEx);
	for (int index = 0; index < NUM_INPUT_VALUES; index++)
	{
		(*ppaStructs)[index].bstrDisplayName = InputsEx[index].Name.copy();

		(*ppaStructs)[index].lVT = InputsEx[index].vt;

		(*ppaStructs)[index].bstrHelpText = InputsEx[index].HelpText.copy();
		(*ppaStructs)[index].bstrGroup = InputsEx[index].Group.copy();
		VARIANT defaultValue = InputsEx[index].vDefaultValue.Detach();
		(*ppaStructs)[index].vDefaultValue.Attach(defaultValue);


	}

	return hr;
}

TOOLDLL_API HRESULT __stdcall SVDestroyInputValueDefinitionStructures(InputValueDefinitionStruct* paStructs)
{

	HRESULT hr = S_OK;
	for (int i = 0; i < NUM_INPUT_VALUES; i++)
	{
		::SysFreeString(paStructs[i].bstrDisplayName);
		::SysFreeString(paStructs[i].bstrGroup);
		::SysFreeString(paStructs[i].bstrHelpText);
		::VariantClear(&paStructs[i].vDefaultValue);
	}
	delete[] paStructs;

	return hr;
}

TOOLDLL_API HRESULT __stdcall SVGetResultValueDefinitions(long* plArraySize, ResultValueDefinitionStruct** ppaResultValues)
{

	HRESULT hr = S_OK;

	*plArraySize = NUM_RESULT_VALUES;
	*ppaResultValues = new ResultValueDefinitionStruct[NUM_RESULT_VALUES];

	if (*ppaResultValues == NULL)
	{
		return S_FALSE;
	}


	std::array<ResultValueDefinitionStructEx, NUM_RESULT_VALUES> ResultsEx;
	CDllTool::getResultValueDefinitionEx(ResultsEx);

	for (int index = 0; index < NUM_RESULT_VALUES; index++)
	{
		(*ppaResultValues)[index].bstrDisplayName = ResultsEx[index].Name.copy();

		(*ppaResultValues)[index].lVT = ResultsEx[index].vt;

	}

	return hr;
}


TOOLDLL_API HRESULT __stdcall SVDestroyResultValueDefinitionStructures(ResultValueDefinitionStruct* paStructs)
{

	HRESULT hr = S_OK;
	for (int i = 0; i < NUM_RESULT_VALUES; i++)
	{
		::SysFreeString(paStructs[i].bstrDisplayName);
	}
	delete[] paStructs;
	return hr;
}


TOOLDLL_API HRESULT __stdcall GetResultValueDefinitionsAd(long* plArraySize, ResultValueDefinitionStructAd** ppaResultValues)
{

	HRESULT hr = S_OK;

	*plArraySize = NUM_RESULT_VALUES;
	*ppaResultValues = new ResultValueDefinitionStructAd[NUM_RESULT_VALUES];
	if (nullptr == *ppaResultValues)
	{
		return S_FALSE;
	}

	std::array<ResultValueDefinitionStructEx, NUM_RESULT_VALUES> ResultsEx;
	CDllTool::getResultValueDefinitionEx(ResultsEx);

	for (int index = 0; index < NUM_RESULT_VALUES; index++)
	{
		(*ppaResultValues)[index].MaxArraylen = ResultsEx[index].ArraySize;
	}
	return hr;
}

TOOLDLL_API HRESULT __stdcall DestroyResultValueDefinitionStructuresAd(ResultValueDefinitionStructAd* paStructs)
{
	HRESULT hr = S_OK;

	delete[] paStructs;

	return hr;
}

TOOLDLL_API HRESULT __stdcall GetResultTableDefinitions(long* pSize, ResultTableDefinitionStruct** ppaResultTableDefs)
{
	HRESULT hr = S_OK;

	if (NUM_RESULT_TABLES > 0)
	{
		*ppaResultTableDefs = new ResultTableDefinitionStruct[NUM_RESULT_TABLES];
	}
	else
	{
		*ppaResultTableDefs = NULL;
	}


	if (ppaResultTableDefs == NULL)
	{

		return S_FALSE;
	}


	*pSize = NUM_RESULT_TABLES;
	std::array<ResultTableDefinitionStructEx, NUM_RESULT_TABLES> ResultsEx;
	CDllTool::getResultTableDefinitionEx(ResultsEx);

	for (int index = 0; index < NUM_RESULT_TABLES; index++)
	{
		(*ppaResultTableDefs)[index].bstrDisplayName = ResultsEx[index].HelpText.copy();

		(*ppaResultTableDefs)[index].lVT = ResultsEx[index].vt;


		(*ppaResultTableDefs)[index].ColoumnCount = ResultsEx[index].ColoumnCount;
		(*ppaResultTableDefs)[index].RowCount = ResultsEx[index].RowCount;
		VARIANT columnName = ResultsEx[index].ColumnNames.Detach();
		(*ppaResultTableDefs)[index].ColumnNames.Attach(columnName);
		//smart array of bstr with names
	}


	return hr;
}

TOOLDLL_API HRESULT __stdcall DestroyResultTableDefinitionStructures(ResultTableDefinitionStruct* paStructs)
{
	HRESULT hr = S_OK;
	delete[] paStructs;
	return hr;
}


// cppcheck-suppress unusedFunction
TOOLDLL_API HRESULT __stdcall GetTransformationDefinitions(long* pSize, TransformDefs** ppTransformdefs)
{
	HRESULT hr = S_OK;


	*ppTransformdefs = new TransformDefs[1];

	if (*ppTransformdefs == NULL)
	{

		return S_FALSE;
	}
	*pSize = 1;
	using TA = std::array<TransformDefs, 1>;
	CDllTool::getTransformationDefinitions(*(TA*)(*ppTransformdefs));

	return hr;
}

// cppcheck-suppress unusedFunction
TOOLDLL_API HRESULT __stdcall  DestroyTransformationDefinitions(TransformDefs* pTransformdefs)
{
	HRESULT hr = S_OK;
	delete[] pTransformdefs;
	return hr;
}