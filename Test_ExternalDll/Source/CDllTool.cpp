//*****************************************************************************
/// \copyright (c) 2017,2017 by Seidenader Maschinenbau GmbH
/// All Rights Reserved 
//*****************************************************************************
/// This is Class includes the main functionalities of the external dll.
//******************************************************************************
#include "stdafx.h"
#include <string>
#include "DebugUtilities.h"
#include "CDllTool.h"
#include <atlsafe.h>
#include <iosfwd>

// Implement CDllTool Class
CDllTool::CDllTool()
{
	::ZeroMemory(&m_GUID, sizeof(GUID));

	//TODO: add initialize of member parameter if necessary.
}

CDllTool::~CDllTool()
{
	cleanData();
}

#pragma region static function

void CDllTool::getInputValuesDefinition(InputValueDefinitionStruct** ppaStructs)
{
	//TODO: add the definition for all inputs, like the comment sample. Delete the comment sample afterwards.
	(*ppaStructs)[InputValue_LONG].lVT = VT_I4;
	(*ppaStructs)[InputValue_LONG].bstrDisplayName = ::SysAllocString(L"Long Wert");
	(*ppaStructs)[InputValue_LONG].bstrHelpText = ::SysAllocString(L"Long Wert eingeben");
	(*ppaStructs)[InputValue_LONG].bstrGroup = ::SysAllocString(L"Common");
	(*ppaStructs)[InputValue_LONG].vDefaultValue.lVal = 4;
	(*ppaStructs)[InputValue_LONG].vDefaultValue.vt = VT_I4;

	(*ppaStructs)[InputValue_DOUBLE].lVT = VT_R8;
	(*ppaStructs)[InputValue_DOUBLE].bstrDisplayName = ::SysAllocString(L"Double Wert");
	(*ppaStructs)[InputValue_DOUBLE].bstrHelpText = ::SysAllocString(L"Double Wert Eingeben");
	(*ppaStructs)[InputValue_DOUBLE].bstrGroup = ::SysAllocString(L"Common");
	(*ppaStructs)[InputValue_DOUBLE].vDefaultValue.dblVal = 2.5;
	(*ppaStructs)[InputValue_DOUBLE].vDefaultValue.vt = VT_R8;

	(*ppaStructs)[InputValue_BSTR].lVT = VT_BSTR;
	(*ppaStructs)[InputValue_BSTR].bstrDisplayName = ::SysAllocString(L"BSTR Wert");
	(*ppaStructs)[InputValue_BSTR].bstrHelpText = ::SysAllocString(L"BSTR Wert eingeben");
	(*ppaStructs)[InputValue_BSTR].bstrGroup = ::SysAllocString(L"Common");
	(*ppaStructs)[InputValue_BSTR].vDefaultValue.bstrVal = ::SysAllocString(L"Markus");;
	(*ppaStructs)[InputValue_BSTR].vDefaultValue.vt = VT_BSTR;

	CComSafeArray<double> sa(50);
	for (int i = 0; i < 50; i++)
		sa[i] = double(i)*1.5;

	(*ppaStructs)[InputValue_DOUBLE_ARRAY].lVT = VT_ARRAY | VT_R8;
	(*ppaStructs)[InputValue_DOUBLE_ARRAY].bstrDisplayName = ::SysAllocString(L"Double Vektor ");
	(*ppaStructs)[InputValue_DOUBLE_ARRAY].bstrHelpText = ::SysAllocString(L"Double Vektor eingeben");
	(*ppaStructs)[InputValue_DOUBLE_ARRAY].bstrGroup = ::SysAllocString(L"Array");
	(*ppaStructs)[InputValue_DOUBLE_ARRAY].vDefaultValue.parray = sa.Detach();
	(*ppaStructs)[InputValue_DOUBLE_ARRAY].vDefaultValue.vt = VT_ARRAY | VT_R8;


	CComSafeArray<int> sai(12);
	for (int i = 0; i < 12; i++)
		sai[i] = i * 12;

	(*ppaStructs)[InputValue_INT_ARRAY].lVT = VT_ARRAY | VT_I4;
	(*ppaStructs)[InputValue_INT_ARRAY].bstrDisplayName = ::SysAllocString(L"Int Vektor");
	(*ppaStructs)[InputValue_INT_ARRAY].bstrHelpText = ::SysAllocString(L"Int Vektor eingeben");
	(*ppaStructs)[InputValue_INT_ARRAY].bstrGroup = ::SysAllocString(L"Array");
	(*ppaStructs)[InputValue_INT_ARRAY].vDefaultValue.parray = sai.Detach();
	(*ppaStructs)[InputValue_INT_ARRAY].vDefaultValue.vt = VT_ARRAY | VT_I4;


	///The next todefinition define a default table tool input
	//the input tabletool can have different sizes
	//Define the table Tool Input create 10 * 2 double Array 
	CComSafeArrayBound bound[2] = {10, 2};
	CComSafeArray<double> tdimsa(bound, 2);
	LONG aIndex[2];
	for (int x = 0; x < 10; x++)
	{
		for (int y = 0; y < 2; y++)
		{
			aIndex[0] = x;
			aIndex[1] = y;
			HRESULT hr = tdimsa.MultiDimSetAt(aIndex, 1);
			ATLASSERT(hr == S_OK);
		}
	}
	(*ppaStructs)[InputValue_TABLE_ARRAY].lVT = VT_ARRAY | VT_R8;
	(*ppaStructs)[InputValue_TABLE_ARRAY].bstrDisplayName = ::SysAllocString(L"Table Objekt");
	(*ppaStructs)[InputValue_TABLE_ARRAY].bstrHelpText = ::SysAllocString(L" Table Object eingeben");
	(*ppaStructs)[InputValue_TABLE_ARRAY].bstrGroup = ::SysAllocString(L"Table");
	(*ppaStructs)[InputValue_TABLE_ARRAY].vDefaultValue.parray = tdimsa.Detach();
	(*ppaStructs)[InputValue_TABLE_ARRAY].vDefaultValue.vt = VT_ARRAY | VT_R8;


	//Define an Array of bstr for the  columname 
	_bstr_t Columname[2] = {"eins", "zwei"};
	CComSafeArray<BSTR> saStr(2);
	for (int i = 0; i < 2; i++)
	{
		saStr.SetAt(i, Columname[i]);
	}

	(*ppaStructs)[InputValue_TABLE_NAMES].lVT = VT_ARRAY | VT_BSTR;
	(*ppaStructs)[InputValue_TABLE_NAMES].bstrDisplayName = ::SysAllocString(L"Table Array ColumnNames");
	(*ppaStructs)[InputValue_TABLE_NAMES].bstrHelpText = ::SysAllocString(L" BSTR Array für Namen");
	(*ppaStructs)[InputValue_TABLE_NAMES].bstrGroup = ::SysAllocString(L"Table");
	(*ppaStructs)[InputValue_TABLE_NAMES].vDefaultValue.parray = saStr.Detach();
	(*ppaStructs)[InputValue_TABLE_NAMES].vDefaultValue.vt = VT_ARRAY | VT_BSTR;


	(*ppaStructs)[InputValue_LONG_TABLE_SELECT].lVT = VT_I4;
	(*ppaStructs)[InputValue_LONG_TABLE_SELECT].bstrDisplayName = ::SysAllocString(L"LONG Value SELECTED Table Index");
	(*ppaStructs)[InputValue_LONG_TABLE_SELECT].bstrHelpText = ::SysAllocString(L"Long selected Colum for table");
	(*ppaStructs)[InputValue_LONG_TABLE_SELECT].bstrGroup = ::SysAllocString(L"TABLE SELECT");
	(*ppaStructs)[InputValue_LONG_TABLE_SELECT].vDefaultValue.lVal = 0;
	(*ppaStructs)[InputValue_LONG_TABLE_SELECT].vDefaultValue.vt = VT_I4;


}

void CDllTool::getResultValueDefinition(ResultValueDefinitionStruct** ppaResultValues)
{
	//TODO: Add the definition for all results, like the comment sample. Delete the comment sample afterwards.
	(*ppaResultValues)[ResultValue_LONG].lVT = VT_I4;
	(*ppaResultValues)[ResultValue_LONG].bstrDisplayName = ::SysAllocString(L"copy from input1");

	(*ppaResultValues)[ResultValue_DOUBLE].lVT = VT_R8;
	(*ppaResultValues)[ResultValue_DOUBLE].bstrDisplayName = ::SysAllocString(L"copy from input2");

	(*ppaResultValues)[ResultValue_BSTR].lVT = VT_BSTR;
	(*ppaResultValues)[ResultValue_BSTR].bstrDisplayName = ::SysAllocString(L"copy from input3");

	(*ppaResultValues)[ResultValue_DOUBLE_ARRAY].lVT = VT_ARRAY | VT_R8;
	(*ppaResultValues)[ResultValue_DOUBLE_ARRAY].bstrDisplayName = ::SysAllocString(L"copy from input4");

	(*ppaResultValues)[ResultValue_INT_ARRAY].lVT = VT_ARRAY | VT_I4;
	(*ppaResultValues)[ResultValue_INT_ARRAY].bstrDisplayName = ::SysAllocString(L"copy from input5");


	(*ppaResultValues)[ResultValue_INT_ROWCOUNT].lVT = VT_I4;
	(*ppaResultValues)[ResultValue_INT_ROWCOUNT].bstrDisplayName = ::SysAllocString(L" Row Count input 6,7");

	(*ppaResultValues)[RESULTVALUE_BSTR_ROWNAME].lVT = VT_BSTR;
	(*ppaResultValues)[RESULTVALUE_BSTR_ROWNAME].bstrDisplayName = ::SysAllocString(L"Columname input 6,7");

	(*ppaResultValues)[RESULTVALUE_DOUBLE_ARRAY_ROW].lVT = VT_ARRAY | VT_R8;
	(*ppaResultValues)[RESULTVALUE_DOUBLE_ARRAY_ROW].bstrDisplayName = ::SysAllocString(L"ARRAY input 6,7 ");

}
void CDllTool::getResultTableDefinition(ResultTableDefinitionStruct** ppaResultTables)
{
	_bstr_t ColumnNames[ColumnCountA] = {"eins", "zwei", "drei", "vier","fünf","sechs"};
	CComSafeArray<BSTR> saStr(ColumnCountA);
	for (int i = 0; i < ColumnCountA; i++)
	{
		saStr.SetAt(i, ColumnNames[i]);
	}


	(*ppaResultTables)[FirstResultTable].lVT = VT_R8;
	(*ppaResultTables)[FirstResultTable].bstrDisplayName = ::SysAllocString(L"First table");
	(*ppaResultTables)[FirstResultTable].ColoumnCount = ColumnCountA;
	(*ppaResultTables)[FirstResultTable].type = 0;
	(*ppaResultTables)[FirstResultTable].ColumnNames.vt = VT_ARRAY | VT_BSTR;
	(*ppaResultTables)[FirstResultTable].ColumnNames.parray = saStr.Detach();
	(*ppaResultTables)[FirstResultTable].RowCount = RowCountA;

}

HRESULT CDllTool::validateInputValues(long lParameterNumber, const	VARIANT vParameterValue)
{
	HRESULT hr = S_OK;

	switch (lParameterNumber)
	{

		case InputValue_LONG:

			if (vParameterValue.vt != VT_I4 || vParameterValue.lVal < c_I1_minValue || vParameterValue.lVal > c_I1_maxValue)
			{
				hr = ERRORCODE_INPUTVALUE_LONG_INVALID;
			}
			else
			{
				hr = S_OK;
			}
			break;
		case InputValue_DOUBLE:
			break;
		case InputValue_BSTR:
			break;
		case  InputValue_DOUBLE_ARRAY:
		{
			if (vParameterValue.vt != (VT_R8 | VT_ARRAY))
			{
				hr = ERRORCODE_INPUTVALUE_DOUBLE_ARRAY_WRONGTYPE;
			}
			else
			{
				CComSafeArray<double> saInput(vParameterValue.parray);
				//if(saInput.GetCount() < 5)
					//hr = ERRORCODE_INPUTVALUE_DOUBLE_ARRAY_TOOSMALL;
			}

			break;
		}
		case  InputValue_INT_ARRAY:
		{
			if (vParameterValue.vt != (VT_ARRAY | VT_I4))
			{
				hr = ERRORCODE_INPUTVALUE_INT_ARRAY_WRONGTYPE;
			}

			break;
		}

		default:
			break;
	}

	return hr;
}

HRESULT CDllTool::getErrorMessageString(unsigned long ulErrorNumber, BSTR* pbstrErrorMessage)
{
	const int string_length = 100;
	wchar_t tmp_string[string_length];
	HRESULT hr = E_NOTIMPL;
	switch (ulErrorNumber)
	{
		//TODO: ADD case for your error codes. Delete comment sample afterwards. 
		case ERRORCODE_INPUTVALUE_LONG_INVALID:
			swprintf(tmp_string, string_length, L"Invalid size of input value Long : Must between %d and %d", c_I1_minValue, c_I1_maxValue);
			*pbstrErrorMessage = ::SysAllocString(tmp_string);
			hr = S_OK;
			break;
		case ERRORCODE_INPUTVALUE_DOUBLE_ARRAY_WRONGTYPE:
			swprintf(tmp_string, string_length, L"Invalid Type not VT_ARRAY | VT_R8");
			*pbstrErrorMessage = ::SysAllocString(tmp_string);
			hr = S_OK;
			break;

		case   ERRORCODE_INPUTVALUE_DOUBLE_ARRAY_TOOSMALL:
			swprintf(tmp_string, string_length, L" VT_ARRAY | VT_DOUBLE  Array need at least 5 elements");
			*pbstrErrorMessage = ::SysAllocString(tmp_string);
			hr = S_OK;
			break;
		case ERRORCODE_INPUTVALUE_INT_ARRAY_WRONGTYPE:
			swprintf(tmp_string, string_length, L"Invalid Type not VT_ARRAY | VT_I4");
			*pbstrErrorMessage = ::SysAllocString(tmp_string);
			hr = S_OK;
			break;

		default:
			break;
	}
	return hr;
}


#pragma endregion

#pragma region public function
HRESULT CDllTool::initRun(const ImageDefinitionStruct* const p_paStructs, const VARIANT* const p_pavInputValues)
{
	HRESULT hr = S_OK;

	///////////////////////////////////////////////////////
	//set input values and result images
	cleanData();
	m_aInputValues.resize(NUM_INPUT_VALUES);
	m_aInputImages.resize(NUM_INPUT_IMAGES);

	// Set result definition
	m_aResultValues.resize(NUM_RESULT_VALUES);
	// Copy Image Definitions to ToolDll imageDefs
	m_aResultImages.resize(NUM_RESULT_IMAGES);
	m_aResultImageDefs.resize(NUM_RESULT_IMAGES);

	//TODO: add your code and delete comment sample afterwards.
	m_aResultValues[ResultValue_LONG].vt = VT_I4;
	m_aResultValues[ResultValue_LONG].lVal = 2;

	m_aResultValues[ResultValue_DOUBLE].vt = VT_R8;
	m_aResultValues[ResultValue_DOUBLE].dblVal = 1.5;

	m_aResultValues[ResultValue_BSTR].vt = VT_BSTR;
	m_aResultValues[ResultValue_BSTR].bstrVal = ::SysAllocString(L"Result Value");

	CComSafeArray<double> sa(2);
	sa[0] = 0.0;
	sa[1] = 0.0;

	m_aResultValues[ResultValue_DOUBLE_ARRAY].vt = VT_ARRAY | VT_R8;
	m_aResultValues[ResultValue_DOUBLE_ARRAY].parray = sa.Detach();

	CComSafeArray<int> isa(2);
	isa[0] = 0;
	isa[1] = 0;
	m_aResultValues[ResultValue_INT_ARRAY].vt = VT_ARRAY | VT_I4;
	m_aResultValues[ResultValue_INT_ARRAY].parray = isa.Detach();



	m_aResultValues[ResultValue_INT_ROWCOUNT].vt = VT_I4;
	m_aResultValues[ResultValue_INT_ROWCOUNT].lVal = 0;

	m_aResultValues[RESULTVALUE_BSTR_ROWNAME].vt = VT_BSTR;
	m_aResultValues[RESULTVALUE_BSTR_ROWNAME].bstrVal = ::SysAllocString(L"n.a.");


	CComSafeArray<double> sa2(2);
	sa2[0] = 0.0;
	sa2[1] = 0.0;

	m_aResultValues[RESULTVALUE_DOUBLE_ARRAY_ROW].vt = VT_ARRAY | VT_R8;
	m_aResultValues[RESULTVALUE_DOUBLE_ARRAY_ROW].parray = sa2.Detach();


	setInputValues(p_pavInputValues);

	for (int i = 0; i < NUM_RESULT_IMAGES; i++)
	{
		//TODO: set definition for result images. In this sample the input image have the same definition as output image
		m_aResultImageDefs[i] = p_paStructs[i];
		std::wstring strName = L"Result Image " + std::to_wstring(i);
		m_aResultImageDefs[i].bstrDisplayName = ::SysAllocString(strName.c_str());
	}

	//Define the table Tool Input create 10 * 2 double Array 
	CComSafeArrayBound bound[2] = {ColumnCountA,RowCountA};
	CComSafeArray<double> tdimsa(bound, 2);
	LONG aIndex[2];
	for (int x = 0; x < ColumnCountA; x++)
	{
		for (int y = 0; y < RowCountA; y++)
		{
			aIndex[0] = x;
			aIndex[1] = y;
			double value = (x + 1) *(y + 1);
			HRESULT hr = tdimsa.MultiDimSetAt(aIndex, value);
			ATLASSERT(hr == S_OK);
		}
	}
	m_ResultTables.resize(NUM_RESULT_TABLES);
	m_ResultTables[FirstResultTable].vt = VT_ARRAY | VT_R8;
	m_ResultTables[FirstResultTable].parray = tdimsa.Detach();



	return hr;
}

HRESULT CDllTool::run_copyTableInput2Output()
{
	if (m_ResultTables[FirstResultTable].vt == (VT_ARRAY | VT_R8) && m_ResultTables[FirstResultTable].parray)
	{
		SafeArrayDestroy(m_ResultTables[FirstResultTable].parray);
		m_ResultTables[FirstResultTable].parray = nullptr;
		m_ResultTables[FirstResultTable].vt = VT_EMPTY;
	}

	if (m_aInputValues[InputValue_TABLE_ARRAY].vt == (VT_ARRAY | VT_R8))
	{
		CComSafeArray<double> saInput((m_aInputValues[InputValue_TABLE_ARRAY].parray));
		int dim = saInput.GetDimensions();
		ATLASSERT(dim == 2);
		unsigned long NX = saInput.GetCount();
		unsigned long NY = saInput.GetCount(1);
		if (NX > 0 && NY > 0)
		{
			CComSafeArrayBound bound[2] = {{ULONG(ColumnCountA),0}, {NY,0}};
			CComSafeArray<double> tdimsa(bound, 2);
			LONG aIndex[2];
			for (unsigned long x = 0; x < ColumnCountA; x++)
			{
				for (unsigned long  y = 0; y < NY; y++)
				{
					aIndex[0] = x;
					aIndex[1] = y;
					double val {0};

					if (x < NX &&  y < NY)
					{
						saInput.MultiDimGetAt(aIndex, val);
					}

					HRESULT hr = tdimsa.MultiDimSetAt(aIndex, val);
					ATLASSERT(hr == S_OK);
				}
			}

			m_ResultTables[FirstResultTable].vt = VT_ARRAY | VT_R8;

			SafeArrayDestroy(m_ResultTables[FirstResultTable].parray);

			m_ResultTables[FirstResultTable].parray = tdimsa.Detach();
		}
	}
	return S_OK;
}
HRESULT CDllTool::run_copySelectedTableInput2Output(int Select)
{

	if ((m_aResultValues[RESULTVALUE_DOUBLE_ARRAY_ROW].vt == (VT_ARRAY | VT_R8)) && m_aResultValues[RESULTVALUE_DOUBLE_ARRAY_ROW].parray)
	{
		SafeArrayDestroy(m_aResultValues[RESULTVALUE_DOUBLE_ARRAY_ROW].parray);
	}
	m_aResultValues[RESULTVALUE_DOUBLE_ARRAY_ROW].vt = VT_EMPTY;
	m_aResultValues[RESULTVALUE_DOUBLE_ARRAY_ROW].parray = nullptr;

	if (m_aInputValues[InputValue_TABLE_ARRAY].vt == (VT_ARRAY | VT_R8))
	{
		CComSafeArray<double> saInput((m_aInputValues[InputValue_TABLE_ARRAY].parray));
		int dim = saInput.GetDimensions();
		ATLASSERT(dim == 2);
		int NX = saInput.GetCount();
		int NY = saInput.GetCount(1);

		m_aResultValues[ResultValue_INT_ROWCOUNT] = NY;

		long Index[2];
		std::stringstream sts;
		double val {0};
		::OutputDebugString(" Table Array:\n");

		for (int y = 0; y < NY; y++)
		{
			sts = std::stringstream();
			for (int x = 0; x < NX; x++)
			{
				Index[0] = x;
				Index[1] = y;
				saInput.MultiDimGetAt(Index, val);
				sts << "(" << x << "," << y << "): " << val << " ";
			}
			sts << std::endl;
			::OutputDebugString(sts.str().c_str());
		}

		if (Select < NX && NY >0)
		{
			CComSafeArray<double> saOutput(NY);
			for (int y = 0; y < NY; y++)
			{
				Index[0] = Select;
				Index[1] = y;
				saInput.MultiDimGetAt(Index, val);
				saOutput[y] = val;
			}

			m_aResultValues[RESULTVALUE_DOUBLE_ARRAY_ROW].vt = VT_ARRAY | VT_R8;
			SafeArrayDestroy(m_aResultValues[RESULTVALUE_DOUBLE_ARRAY_ROW].parray);
			m_aResultValues[RESULTVALUE_DOUBLE_ARRAY_ROW].parray = saOutput.Detach();
		}

	}

	m_aResultValues[RESULTVALUE_BSTR_ROWNAME] = _bstr_t();

	if (m_aInputValues[InputValue_TABLE_NAMES].vt == (VT_ARRAY | VT_BSTR))
	{

		CComSafeArray<BSTR> saInput((m_aInputValues[InputValue_TABLE_NAMES].parray));
		int dim = saInput.GetDimensions();
		ATLASSERT(dim == 1);
		int len = saInput.GetCount();
		std::stringstream sts;
		::OutputDebugString(" Table Names\n");

		for (int y = 0; y < len; y++)
		{
			CComBSTR  name = saInput.GetAt(y);
			_bstr_t bstrname = name.m_str;
			if (Select == y)
			{
				m_aResultValues[RESULTVALUE_BSTR_ROWNAME] = bstrname;
			}
			::OutputDebugString(LPCSTR(bstrname));
			::OutputDebugString("\n");
		}

	}
	return S_OK;
}


HRESULT CDllTool::run()
{
	///copy input to output
	HRESULT hr = S_OK;

	long inputLong = m_aInputValues[InputValue_LONG];
	m_aResultValues[ResultValue_LONG] = inputLong;

	double inputdouble = m_aInputValues[InputValue_DOUBLE];
	m_aResultValues[ResultValue_DOUBLE].dblVal = inputdouble;

	_bstr_t inputstring = m_aInputValues[InputValue_BSTR];
	std::wstring output = L"Input: ";
	output += (wchar_t*)inputstring;
	//::SysFreeString(m_aResultValues[ResultValue_BSTR].bstrVal);

	m_aResultValues[ResultValue_BSTR] = ::SysAllocString(output.c_str());

	long Select = m_aInputValues[InputValue_LONG_TABLE_SELECT];


	if (m_aResultValues[ResultValue_DOUBLE_ARRAY].vt == (VT_ARRAY | VT_R8) && m_aResultValues[ResultValue_DOUBLE_ARRAY].parray)
	{
		SafeArrayDestroy(m_aResultValues[ResultValue_DOUBLE_ARRAY].parray);
		m_aResultValues[ResultValue_DOUBLE_ARRAY].parray = nullptr;
		m_aResultValues[ResultValue_DOUBLE_ARRAY].vt = VT_EMPTY;
	}

	
	if (m_aInputValues[InputValue_DOUBLE_ARRAY].vt == (VT_ARRAY | VT_R8))
	{
		int inputarrayLen = 0;
		int inputarrayLowerBound = 0;
		CComSafeArray<double> saInput((m_aInputValues[InputValue_DOUBLE_ARRAY].parray));

		inputarrayLen = saInput.GetCount();
		inputarrayLowerBound = saInput.GetLowerBound();

		//copy input to output 
		CComSafeArray<double> saOutput(inputarrayLen);
		for (int i = 0; i < inputarrayLen; i++)
		{
			saOutput[i] = saInput[i];
		}
		if (inputarrayLen)
		{
			m_aResultValues[ResultValue_DOUBLE_ARRAY].vt = VT_ARRAY | VT_R8;
			m_aResultValues[ResultValue_DOUBLE_ARRAY].parray = saOutput.Detach();
		}
	}

	if (m_aResultValues[ResultValue_INT_ARRAY].parray && m_aResultValues[ResultValue_INT_ARRAY].vt == (VT_ARRAY | VT_I4))
	{
		SafeArrayDestroy(m_aResultValues[ResultValue_INT_ARRAY].parray);
		m_aResultValues[ResultValue_INT_ARRAY].parray = nullptr;
		m_aInputValues[InputValue_INT_ARRAY].vt = VT_EMPTY;
	}

	if (m_aInputValues[InputValue_INT_ARRAY].vt == (VT_ARRAY | VT_I4))
	{
		int inputarrayLen = 0;
		int inputarrayLowerBound = 0;
		CComSafeArray<int> saInput((m_aInputValues[InputValue_INT_ARRAY].parray));

		inputarrayLen = saInput.GetCount();
		inputarrayLowerBound = saInput.GetLowerBound();

		//copy input to output 
		CComSafeArray<int> saOutput(inputarrayLen);
		for (int i = 0; i < inputarrayLen; i++)
		{
			saOutput[i] = saInput[i];
		}

		if (inputarrayLen)
		{
			m_aResultValues[ResultValue_INT_ARRAY].vt = VT_ARRAY | VT_I4;
			m_aResultValues[ResultValue_INT_ARRAY].parray = saOutput.Detach();
		}
	}

	run_copySelectedTableInput2Output(Select);
	run_copyTableInput2Output();

	//copy first input image to first result image
	static_assert(0 < NUM_INPUT_IMAGES && 0 < NUM_RESULT_IMAGES, "for this sample you need at least one input and one result image");
	MbufCopy(m_aInputImages[0], m_aResultImages[0]);
	MbufCopy(m_aInputImages[1], m_aResultImages[1]);
	return hr;
}

#pragma region set function
void CDllTool::setGUID(const GUID guid)
{
	m_GUID = guid;
}

HRESULT CDllTool::setInputValues(const VARIANT* const p_pavInputValues)
{
	for (int i = 0; i < NUM_INPUT_VALUES; i++)
	{
		::VariantClear(&m_aInputValues[i]);
		::VariantCopy(&m_aInputValues[i], &p_pavInputValues[i]);
	}
	return S_OK;
}

HRESULT CDllTool::setMILInputImages(const long* const paMILhandles)
{
	for (int i = 0; i < NUM_INPUT_IMAGES; i++)
	{
		m_aInputImages[i] = static_cast<MIL_ID> (paMILhandles[i]);
	}
	return S_OK;
}

HRESULT CDllTool::setMILResultImages(long* paMILHandles)
{
	for (int i = 0; i < NUM_RESULT_IMAGES; i++)
	{
		m_aResultImages[i] = static_cast<MIL_ID> (paMILHandles[i]);
	}
	return S_OK;
}
#pragma endregion

#pragma region get function
HRESULT CDllTool::getResultImageDefs(long* plArraySize, ImageDefinitionStruct** ppaStructs) const
{
	HRESULT hr = S_OK;
	if (m_aResultImageDefs.size() == NUM_RESULT_IMAGES)
	{
		*ppaStructs = new ImageDefinitionStruct[NUM_RESULT_IMAGES];
		for (int i = 0; i < NUM_RESULT_IMAGES; i++)
		{
			(*ppaStructs)[i].eImageFormat = m_aResultImageDefs[i].eImageFormat;
			(*ppaStructs)[i].lWidth = m_aResultImageDefs[i].lWidth;
			(*ppaStructs)[i].lHeight = m_aResultImageDefs[i].lHeight;
			(*ppaStructs)[i].bstrDisplayName = ::SysAllocString(m_aResultImageDefs[i].bstrDisplayName);
		}

		*plArraySize = NUM_RESULT_IMAGES;
	}
	else
		hr = S_FALSE;

	return hr;
}

HRESULT CDllTool::getResultValues(VARIANT* p_paResultValues) const
{
	try
	{
		for (int i = 0; i < NUM_RESULT_VALUES; i++)
		{
			::VariantCopy(&p_paResultValues[i], &m_aResultValues[i]);
		}
	}
	catch (...)
	{
		return E_FAIL;
	}

	return S_OK;
}
HRESULT CDllTool::getResultTables(VARIANT* pResultValues) const
{
	try
	{
		for (int i = 0; i < NUM_RESULT_TABLES; i++)
		{
			::VariantCopy(&pResultValues[i], &m_ResultTables[i]);
		}
	}
	catch (...)
	{
		return E_FAIL;
	}

	return S_OK;
}


#pragma endregion
#pragma endregion

#pragma region private function


void CDllTool::cleanData()
{
	for (int i = 0; i < m_aInputValues.size(); i++)
	{
		::VariantClear(&m_aInputValues[i]);
	}
	m_aInputValues.clear();

	for (int i = 0; i < m_aResultValues.size(); i++)
	{
		::VariantClear(&m_aResultValues[i]);
	}
	m_aResultValues.clear();

	m_aInputImages.clear();
	m_aResultImages.clear();
	for (int i = 0; i < m_aResultImageDefs.size(); i++)
	{
		::SysFreeString(m_aResultImageDefs[i].bstrDisplayName);
	}
	m_aResultImageDefs.clear();

	//TODO: add your code if necessary.
}
#pragma endregion