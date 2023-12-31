//*****************************************************************************
/// \copyright (c) 2017,2017 by K�rber Pharma Inspection GmbH. All Rights Reserved
/// All Rights Reserved 
//*****************************************************************************
/// This is Class includes the main functionalities of the external dll.
//******************************************************************************
#include "stdafx.h"
#include <string>
#include "CDllTool.h"
#include <atlsafe.h>
#include <iosfwd>



// Implement CDllTool Class
CDllTool::CDllTool()
{
	::ZeroMemory(&m_GUID, sizeof(GUID));
	m_FunctionMatrix = MbufAlloc2d(M_DEFAULT_HOST, 3, 3, M_FLOAT + 32, M_ARRAY, M_NULL);
	//TODO: add initialize of member parameter if necessary.
}

CDllTool::~CDllTool()
{
	cleanData();
	MbufFree(m_FunctionMatrix);
}

#pragma region static function

void CDllTool::getInputValuesDefinitionEx(std::array<InputValueDefinitionStructEx, NUM_INPUT_VALUES>& inputDefEx)
{

	//TODO: add the definition for all inputs, like the comment sample. Delete the comment sample afterwards.
	inputDefEx[InputValue_LONG].vt = VT_I4;
	inputDefEx[InputValue_LONG].Name = "Eingabe1";
	inputDefEx[InputValue_LONG].HelpText = "Long Wert eingeben";
	inputDefEx[InputValue_LONG].Group = "Common";
	inputDefEx[InputValue_LONG].vDefaultValue.lVal = 4;
	inputDefEx[InputValue_LONG].vDefaultValue.vt = VT_I4;

	inputDefEx[InputValue_DOUBLE].vt = VT_R8;
	inputDefEx[InputValue_DOUBLE].Name = "Wert2";
	inputDefEx[InputValue_DOUBLE].HelpText = "Double Wert Eingeben";
	inputDefEx[InputValue_DOUBLE].Group = "Common";
	inputDefEx[InputValue_DOUBLE].vDefaultValue = 2.5;
	//inputDefEx[InputValue_DOUBLE].vDefaultValue.dblVal = 2.5;
	//inputDefEx[InputValue_DOUBLE].vDefaultValue.vt = VT_R8;

	inputDefEx[InputValue_BSTR].vt = VT_BSTR;
	inputDefEx[InputValue_BSTR].Name = "Name";
	inputDefEx[InputValue_BSTR].HelpText = "String eingeben";
	inputDefEx[InputValue_BSTR].Group = "Common";
	inputDefEx[InputValue_BSTR].vDefaultValue = "Markus";
	//inputDefEx[InputValue_BSTR].vDefaultValue.bstrVal = ::SysAllocString(L"Markus");;
	//inputDefEx[InputValue_BSTR].vDefaultValue.vt = VT_BSTR;

	CComSafeArray<double> sa(50);
	for (int i = 0; i < 50; i++)
		sa[i] = double(i) * 1.5;

	inputDefEx[InputValue_DOUBLE_ARRAY].vt = VT_ARRAY | VT_R8;
	inputDefEx[InputValue_DOUBLE_ARRAY].Name = "DVektor";
	inputDefEx[InputValue_DOUBLE_ARRAY].HelpText = "Double Vektor eingeben";
	inputDefEx[InputValue_DOUBLE_ARRAY].Group = "Array";
	inputDefEx[InputValue_DOUBLE_ARRAY].vDefaultValue.parray = sa.Detach();
	inputDefEx[InputValue_DOUBLE_ARRAY].vDefaultValue.vt = VT_ARRAY | VT_R8;


	CComSafeArray<int> sai(12);
	for (int i = 0; i < 12; i++)
		sai[i] = i * 12;

	inputDefEx[InputValue_INT_ARRAY].vt = VT_ARRAY | VT_I4;
	inputDefEx[InputValue_INT_ARRAY].Name = "IntWert";
	inputDefEx[InputValue_INT_ARRAY].HelpText = "Int Vektor eingeben";
	inputDefEx[InputValue_INT_ARRAY].Group = "Array";
	inputDefEx[InputValue_INT_ARRAY].vDefaultValue.parray = sai.Detach();
	inputDefEx[InputValue_INT_ARRAY].vDefaultValue.vt = VT_ARRAY | VT_I4;


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
			assert(hr == S_OK);
			if (S_OK != hr)
			{
				::OutputDebugString("Error in function MultiDimSetAt");
			}
		}
	}
	inputDefEx[InputValue_TABLE_ARRAY].vt = VT_ARRAY | VT_R8;
	inputDefEx[InputValue_TABLE_ARRAY].Name = "Wert6";
	inputDefEx[InputValue_TABLE_ARRAY].HelpText = "Table Object eingeben";
	inputDefEx[InputValue_TABLE_ARRAY].Group = "Table";
	inputDefEx[InputValue_TABLE_ARRAY].vDefaultValue.parray = tdimsa.Detach();
	inputDefEx[InputValue_TABLE_ARRAY].vDefaultValue.vt = VT_ARRAY | VT_R8;


	//Define an Array of bstr for the  columname 
	_bstr_t Columname[2] = {"eins", "zwei"};
	CComSafeArray<BSTR> saStr(2);
	for (int i = 0; i < 2; i++)
	{
		saStr.SetAt(i, Columname[i]);
	}

	inputDefEx[InputValue_TABLE_NAMES].vt = VT_ARRAY | VT_BSTR;
	inputDefEx[InputValue_TABLE_NAMES].Name = "ColumnNames";
	inputDefEx[InputValue_TABLE_NAMES].HelpText = "BSTR Array f�r Namen";
	inputDefEx[InputValue_TABLE_NAMES].Group = "Table";
	inputDefEx[InputValue_TABLE_NAMES].vDefaultValue.parray = saStr.Detach();
	inputDefEx[InputValue_TABLE_NAMES].vDefaultValue.vt = VT_ARRAY | VT_BSTR;


	CComSafeArray<double> sa2(50);
	for (int i = 0; i < 50; i++)
		sa2[i] = double(i) * 1.5;

	inputDefEx[InputValue_Second_Double_Array].vt = VT_ARRAY | VT_R8;
	inputDefEx[InputValue_Second_Double_Array].Name = "ZDVektor";
	inputDefEx[InputValue_Second_Double_Array].HelpText = "Double Vektor eingeben";
	inputDefEx[InputValue_Second_Double_Array].Group = "ZweitesArray";
	inputDefEx[InputValue_Second_Double_Array].vDefaultValue.parray = sa2.Detach();
	inputDefEx[InputValue_Second_Double_Array].vDefaultValue.vt = VT_ARRAY | VT_R8;
	inputDefEx[InputValue_LONG_TABLE_SELECT].vt = VT_I4;
	inputDefEx[InputValue_LONG_TABLE_SELECT].Name = "tableCol";
	inputDefEx[InputValue_LONG_TABLE_SELECT].HelpText = "Long selected Colum for table";
	inputDefEx[InputValue_LONG_TABLE_SELECT].Group = "TABLE SELECT";
	inputDefEx[InputValue_LONG_TABLE_SELECT].vDefaultValue.lVal = 0;
	inputDefEx[InputValue_LONG_TABLE_SELECT].vDefaultValue.vt = VT_I4;

	inputDefEx[InputValue_LONG_DELAY_MS].vt = VT_I4;
	inputDefEx[InputValue_LONG_DELAY_MS].Name = "Delay";
	inputDefEx[InputValue_LONG_DELAY_MS].HelpText = "Delay(ms)";
	inputDefEx[InputValue_LONG_DELAY_MS].Group = "DelayGroup";
	inputDefEx[InputValue_LONG_DELAY_MS].vDefaultValue.lVal = 0;
	inputDefEx[InputValue_LONG_DELAY_MS].vDefaultValue.vt = VT_I4;



	inputDefEx[InputValue_DOUBLE_CENTER_X].vt = VT_R8;
	inputDefEx[InputValue_DOUBLE_CENTER_X].Name = "CENTER_X";
	inputDefEx[InputValue_DOUBLE_CENTER_X].HelpText = "x koordinate drehpunkt";
	inputDefEx[InputValue_DOUBLE_CENTER_X].Group = "Abbildung";
	inputDefEx[InputValue_DOUBLE_CENTER_X].vDefaultValue = 0.0;

	inputDefEx[InputValue_DOUBLE_CENTER_Y].vt = VT_R8;
	inputDefEx[InputValue_DOUBLE_CENTER_Y].Name = "CENTER_Y";
	inputDefEx[InputValue_DOUBLE_CENTER_Y].HelpText = "Y Koordinate drehpunkt";
	inputDefEx[InputValue_DOUBLE_CENTER_Y].Group = "Abbildung";
	inputDefEx[InputValue_DOUBLE_CENTER_Y].vDefaultValue = 0.0;


	inputDefEx[InputValue_DOUBLE_OFFSET_X].vt = VT_R8;
	inputDefEx[InputValue_DOUBLE_OFFSET_X].Name = "offset_X";
	inputDefEx[InputValue_DOUBLE_OFFSET_X].HelpText = "x koordinate offset";
	inputDefEx[InputValue_DOUBLE_OFFSET_X].Group = "Abbildung";
	inputDefEx[InputValue_DOUBLE_OFFSET_X].vDefaultValue = 0.0;

	inputDefEx[InputValue_DOUBLE_OFFSET_Y].vt = VT_R8;
	inputDefEx[InputValue_DOUBLE_OFFSET_Y].Name = "offste y";
	inputDefEx[InputValue_DOUBLE_OFFSET_Y].HelpText = "Y Koordinate offset";
	inputDefEx[InputValue_DOUBLE_OFFSET_Y].Group = "Abbildung";
	inputDefEx[InputValue_DOUBLE_OFFSET_Y].vDefaultValue = 0.0;

	inputDefEx[InputValue_DOUBLE_ANGEL].vt = VT_R8;
	inputDefEx[InputValue_DOUBLE_ANGEL].Name = "ANGEL";
	inputDefEx[InputValue_DOUBLE_ANGEL].HelpText = "DREHWINKEL";
	inputDefEx[InputValue_DOUBLE_ANGEL].Group = "Abbildung";
	inputDefEx[InputValue_DOUBLE_ANGEL].vDefaultValue = 0.0;



}



void CDllTool::getResultValueDefinitionEx(std::array< ResultValueDefinitionStructEx, NUM_RESULT_VALUES>& ResultValues)
{
	//TODO: Add the definition for all results, like the comment sample. Delete the comment sample afterwards.
	ResultValues[ResultValue_LONG].vt = VT_I4;
	ResultValues[ResultValue_LONG].Name = "cpyFromIn1";
	ResultValues[ResultValue_LONG].HelpText = "copy from input1";
	ResultValues[ResultValue_LONG].Group = "Copy";

	ResultValues[ResultValue_DOUBLE].vt = VT_R8;
	ResultValues[ResultValue_DOUBLE].Name = "cpyFromIn2";;
	ResultValues[ResultValue_DOUBLE].HelpText = "copy from input2";
	ResultValues[ResultValue_DOUBLE].Group = "Copy";

	ResultValues[ResultValue_BSTR].vt = VT_BSTR;
	ResultValues[ResultValue_BSTR].Name = "cpyFromIn3";;
	ResultValues[ResultValue_BSTR].HelpText = "copy from input3";
	ResultValues[ResultValue_BSTR].Group = "Copy";

	ResultValues[ResultValue_DOUBLE_ARRAY].vt = VT_ARRAY | VT_R8;
	ResultValues[ResultValue_DOUBLE_ARRAY].Name = "cpyFromIn4";;;
	ResultValues[ResultValue_DOUBLE_ARRAY].HelpText = "copy from input4";
	ResultValues[ResultValue_DOUBLE_ARRAY].Group = "Copy";
	ResultValues[ResultValue_DOUBLE_ARRAY].ArraySize = 7;


	ResultValues[ResultValue_INT_ARRAY].vt = VT_ARRAY | VT_I4;
	ResultValues[ResultValue_INT_ARRAY].Name = "cpyFromIn5";
	ResultValues[ResultValue_INT_ARRAY].HelpText = "copy from input5";
	ResultValues[ResultValue_INT_ARRAY].Group = "Copy";
	ResultValues[ResultValue_INT_ARRAY].ArraySize = 9;

	ResultValues[ResultValue_INT_ROWCOUNT].vt = VT_I4;
	ResultValues[ResultValue_INT_ROWCOUNT].Name = "RowCountIn_6_7";
	ResultValues[ResultValue_INT_ROWCOUNT].HelpText = " Row Count input 6,7";
	ResultValues[ResultValue_INT_ROWCOUNT].Group = "SpecCopy";

	ResultValues[RESULTVALUE_BSTR_ROWNAME].vt = VT_BSTR;
	ResultValues[RESULTVALUE_BSTR_ROWNAME].Name = "ColInput_6_7";
	ResultValues[RESULTVALUE_BSTR_ROWNAME].HelpText = "Columname input 6,7";
	ResultValues[RESULTVALUE_BSTR_ROWNAME].Group = "SpecCopy";

	ResultValues[RESULTVALUE_DOUBLE_ARRAY_ROW].vt = VT_ARRAY | VT_R8;
	ResultValues[RESULTVALUE_DOUBLE_ARRAY_ROW].Name = "Array_6_7";
	ResultValues[RESULTVALUE_DOUBLE_ARRAY_ROW].HelpText = "ARRAY input 6,7 ";
	ResultValues[RESULTVALUE_DOUBLE_ARRAY_ROW].Group = "SpecCopy";
	ResultValues[RESULTVALUE_DOUBLE_ARRAY_ROW].ArraySize = 10;


	ResultValues[RESULTVALUE_DOUBLE_ARRAY_FUNCTION].vt = VT_ARRAY | VT_R8;
	ResultValues[RESULTVALUE_DOUBLE_ARRAY_FUNCTION].Name = "Abbildungsmatrix";
	ResultValues[RESULTVALUE_DOUBLE_ARRAY_FUNCTION].HelpText = "AbbildungsMatrix from Input image 1 to 1";
	ResultValues[RESULTVALUE_DOUBLE_ARRAY_FUNCTION].Group = "Abbildung";
	ResultValues[RESULTVALUE_DOUBLE_ARRAY_FUNCTION].ArraySize = 9;
}





void CDllTool::getResultTableDefinitionEx(std::array<ResultTableDefinitionStructEx, NUM_RESULT_TABLES>& ResultTables)
{
	//TODO: Add the definition for all results, like the comment sample. Delete the comment sample afterwards.
	_bstr_t ColumnNames[ColumnCountA] = {"eins", "zwei", "drei", "vier","fuenf","sechs"};
	CComSafeArray<BSTR> saStr(ColumnCountA);
	for (int i = 0; i < ColumnCountA; i++)
	{
		saStr.SetAt(i, ColumnNames[i]);
	}

	ResultTables[FirstResultTable].vt = VT_R8;
	ResultTables[FirstResultTable].Name = "Ftable";
	ResultTables[FirstResultTable].HelpText = "Erstes Table Objekt";
	ResultTables[FirstResultTable].ColoumnCount = ColumnCountA;
	ResultTables[FirstResultTable].ColumnNames.vt = VT_ARRAY | VT_BSTR;
	ResultTables[FirstResultTable].ColumnNames.parray = saStr.Detach();
	ResultTables[FirstResultTable].RowCount = RowCountA;
	ResultTables[FirstResultTable].Group = "TableGroup";

	////////////////////////
	_bstr_t ColumnNamesB[ColumnCountA] = {"A", "B", "C", "D","E","F"};
	CComSafeArray<BSTR> saStrB(ColumnCountA);
	for (int i = 0; i < ColumnCountA; i++)
	{
		saStrB.SetAt(i, ColumnNamesB[i]);
	}

	ResultTables[SecondResultTable].vt = VT_R8;
	ResultTables[SecondResultTable].Name = "Stable";
	ResultTables[SecondResultTable].HelpText = "Zweites Table Objekt";
	ResultTables[SecondResultTable].ColoumnCount = ColumnCountA;
	ResultTables[SecondResultTable].ColumnNames.vt = VT_ARRAY | VT_BSTR;
	ResultTables[SecondResultTable].ColumnNames.parray = saStrB.Detach();
	ResultTables[SecondResultTable].RowCount = RowCountA;
	ResultTables[SecondResultTable].Group = "TableGroup";
}

HRESULT CDllTool::validateInputValues(long lParameterNumber, const	VARIANT vParameterValue)
{
	HRESULT hr = S_OK;
	//TODO: ADD check if input values valid. Delete comment sample afterwards. 
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
	for (int x = 0; x < ColumnCountA && S_OK == hr; x++)
	{
		for (int y = 0; y < RowCountA && S_OK == hr; y++)
		{
			aIndex[0] = x;
			aIndex[1] = y;
			double value = (x + 1) * (y + 1);
			hr = tdimsa.MultiDimSetAt(aIndex, value);
			assert(S_OK == hr);
		}
	}
	CComSafeArray<double> tdimsb(tdimsa);
	m_ResultTables.resize(NUM_RESULT_TABLES);
	m_ResultTables[FirstResultTable].vt = VT_ARRAY | VT_R8;
	m_ResultTables[FirstResultTable].parray = tdimsa.Detach();


	m_ResultTables[SecondResultTable].vt = VT_ARRAY | VT_R8;
	m_ResultTables[SecondResultTable].parray = tdimsb.Detach();

	//xr = a[0]x + a[1]y + a[2}
	//yr = a[3]x + a[4] y + a[5] 
	//a[6] angel
	CComSafeArray<double> a(7);
	a[0] = 1.0;
	a[1] = 0.0;
	a[2] = 0.0;
	a[3] = 0.0;
	a[4] = 1.0;
	a[5] = 0.0;
	a[6] =0.0;



	m_aResultValues[RESULTVALUE_DOUBLE_ARRAY_FUNCTION].vt = VT_ARRAY | VT_R8;
	m_aResultValues[RESULTVALUE_DOUBLE_ARRAY_FUNCTION].parray = a.Detach();

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

	assert(m_aInputValues[InputValue_TABLE_ARRAY].vt == (VT_ARRAY | VT_R8));
	if (m_aInputValues[InputValue_TABLE_ARRAY].vt == (VT_ARRAY | VT_R8))
	{
		CComSafeArray<double> saInput((m_aInputValues[InputValue_TABLE_ARRAY].parray));
		int dim = saInput.GetDimensions();
		assert(dim == 2);
		if (dim != 2)
		{
			::OutputDebugString("Input array not dimension size 2!");
		}
		unsigned long NX = saInput.GetCount();
		unsigned long NY = saInput.GetCount(1);
		if (NX > 0 && NY > 0)
		{
			CComSafeArrayBound bound[2] = {{ULONG(ColumnCountA),0}, {NY,0}};
			CComSafeArray<double> tdimsa(bound, 2);
			LONG aIndex[2];
			for (unsigned long x = 0; x < ColumnCountA; x++)
			{
				for (unsigned long y = 0; y < NY; y++)
				{
					aIndex[0] = x;
					aIndex[1] = y;
					double val {0};

					if (x < NX)
					{
						saInput.MultiDimGetAt(aIndex, val);
					}

					HRESULT hr = tdimsa.MultiDimSetAt(aIndex, val);
					assert(S_OK == hr);
					if (S_OK != hr)
					{
						::OutputDebugString("Error in function MultiDimSetAt");
					}
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


	m_aResultValues[RESULTVALUE_DOUBLE_ARRAY_ROW].Clear();
	assert(m_aInputValues[InputValue_TABLE_ARRAY].vt == (VT_ARRAY | VT_R8));
	if (m_aInputValues[InputValue_TABLE_ARRAY].vt == (VT_ARRAY | VT_R8))
	{
		CComSafeArray<double> saInput((m_aInputValues[InputValue_TABLE_ARRAY].parray));
		int dim = saInput.GetDimensions();
		assert(dim == 2);
		if (dim != 2)
		{
			::OutputDebugString("Input array not dimension size 2!");
		}
		int NX = saInput.GetCount();
		int NY = saInput.GetCount(1);

		m_aResultValues[ResultValue_INT_ROWCOUNT] = NY;

		long Index[2];

		double val {0};


		for (int y = 0; y < NY; y++)
		{

			for (int x = 0; x < NX; x++)
			{
				Index[0] = x;
				Index[1] = y;
				saInput.MultiDimGetAt(Index, val);

			}

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
			m_aResultValues[RESULTVALUE_DOUBLE_ARRAY_ROW].parray = saOutput.Detach();
		}

	}

	m_aResultValues[RESULTVALUE_BSTR_ROWNAME] = _bstr_t();

	if (m_aInputValues[InputValue_TABLE_NAMES].vt == (VT_ARRAY | VT_BSTR))
	{
		CComSafeArray<BSTR> saInput((m_aInputValues[InputValue_TABLE_NAMES].parray));
		int dim = saInput.GetDimensions();
		assert(dim == 1);
		if (dim != 1)
		{
			::OutputDebugString("Input array not dimension size 1!");
		}
		int len = saInput.GetCount();

		for (int y = 0; y < len; y++)
		{
			CComBSTR  name = saInput.GetAt(y);
			_bstr_t bstrname = name.m_str;
			if (Select == y)
			{
				m_aResultValues[RESULTVALUE_BSTR_ROWNAME] = bstrname;
			}
			//::OutputDebugString(LPCSTR(bstrname));
			//::OutputDebugString("\n");
		}

	}
	return S_OK;
}


HRESULT CDllTool::run()
{
	///copy input to output
	HRESULT hr = S_OK;

	long delay = m_aInputValues[InputValue_LONG_DELAY_MS];
	delay = delay > 3000 ? 3000 : delay;
	if (delay > 0)
	{
		::Sleep(delay);
	}

	long inputLong = m_aInputValues[InputValue_LONG];
	m_aResultValues[ResultValue_LONG] = inputLong;

	double inputdouble = m_aInputValues[InputValue_DOUBLE];
	m_aResultValues[ResultValue_DOUBLE].dblVal = inputdouble;

	_bstr_t inputstring = m_aInputValues[InputValue_BSTR].bstrVal;
	std::wstring output = L"Input: ";
	output += static_cast<wchar_t*> (inputstring);

	m_aResultValues[ResultValue_BSTR] = _bstr_t(output.c_str());
	long Select = m_aInputValues[InputValue_LONG_TABLE_SELECT];

	m_aResultValues[ResultValue_DOUBLE_ARRAY].Clear();

	assert(m_aInputValues[InputValue_DOUBLE_ARRAY].vt == (VT_ARRAY | VT_R8));
	if (m_aInputValues[InputValue_DOUBLE_ARRAY].vt == (VT_ARRAY | VT_R8))
	{
		int inputarrayLen = 0;

		CComSafeArray<double> saInput((m_aInputValues[InputValue_DOUBLE_ARRAY].parray));

		inputarrayLen = saInput.GetCount();


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

	m_aResultValues[ResultValue_INT_ARRAY].Clear();

	assert(m_aInputValues[InputValue_INT_ARRAY].vt == (VT_ARRAY | VT_I4));
	if (m_aInputValues[InputValue_INT_ARRAY].vt == (VT_ARRAY | VT_I4))
	{
		CComSafeArray<int> saInput((m_aInputValues[InputValue_INT_ARRAY].parray));

		int inputarrayLen = saInput.GetCount();


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

	assert(m_aInputValues[InputValue_Second_Double_Array].vt == (VT_ARRAY | VT_R8));
	if (m_aInputValues[InputValue_Second_Double_Array].vt == (VT_ARRAY | VT_R8))
	{
		double out {0};
		int inputarrayLen = 0;
		CComSafeArray<double> saInput((m_aInputValues[InputValue_Second_Double_Array].parray));

		inputarrayLen = saInput.GetCount();


		for (int i = 0; i < inputarrayLen; i++)
		{
			out += saInput[i];
		}
		m_aResultValues[ResultValue_DOUBLE].dblVal += out;
	}

	run_copySelectedTableInput2Output(Select);
	run_copyTableInput2Output();

	//copy first input image to first result image
	static_assert(0 < NUM_INPUT_IMAGES && 0 < NUM_RESULT_IMAGES, "for this sample you need at least one input and one result image");
	//MbufCopy(m_aInputImages[0], m_aResultImages[0]);
	MIL_DOUBLE  SX {0}, SY {0}, OffsetX {0}, OffsetY {0};
	MIL_DOUBLE Angel {0};


	SX = m_aInputValues[InputValue_DOUBLE_CENTER_X];
	SY = m_aInputValues[InputValue_DOUBLE_CENTER_Y];
	OffsetX = m_aInputValues[InputValue_DOUBLE_OFFSET_X];
	OffsetY = m_aInputValues[InputValue_DOUBLE_OFFSET_Y];

	Angel = m_aInputValues[InputValue_DOUBLE_ANGEL];


	// MIL_ID  FunctionMatrix = MbufAlloc2d(M_DEFAULT_HOST, 3, 3, M_FLOAT + 32, M_ARRAY, M_NULL);

	MimRotate(M_NULL, m_FunctionMatrix, Angel, SX, SY, SX - OffsetX, SY - OffsetY, M_DEFAULT);

	float FunctionMatrix[3][3];


	MbufGet2d(m_FunctionMatrix, 0, 0, 3, 3, FunctionMatrix);
	MbufClear(m_aResultImages[0], M_COLOR_BLACK);
	MimRotate(m_aInputImages[0], m_aResultImages[0], Angel, SX, SY, SX - OffsetX, SY - OffsetY, M_DEFAULT+ M_OVERSCAN_CLEAR);
	MbufCopy(m_aInputImages[1], m_aResultImages[1]);

	m_aResultValues[RESULTVALUE_DOUBLE_ARRAY_FUNCTION].Clear();
	CComSafeArray<double> saMatrix(9);


	saMatrix[0] = FunctionMatrix[0][0];
	saMatrix[1] = FunctionMatrix[0][1];
	saMatrix[2] = FunctionMatrix[0][2];
	saMatrix[3] = FunctionMatrix[1][0];
	saMatrix[4] = FunctionMatrix[1][1];
	saMatrix[5] = FunctionMatrix[1][2];
	saMatrix[6] = Angel;
	m_aResultValues[RESULTVALUE_DOUBLE_ARRAY_FUNCTION].vt = VT_ARRAY | VT_R8;
	m_aResultValues[RESULTVALUE_DOUBLE_ARRAY_FUNCTION].parray = saMatrix.Detach();
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


HRESULT CDllTool::getResultTablesMaxRowSize(std::array<int, NUM_RESULT_TABLES>& rRowSizes) const
{
	for (int i = 0; i < rRowSizes.size(); i++)
	{
		rRowSizes[i] = 40;
	}
	return S_OK;

}


HRESULT  CDllTool::getResultValuesMaxArraySize(std::array<int, NUM_RESULT_VALUES>& rArraySizes) const
{
	for (int i = 0; i < rArraySizes.size(); i++)
	{
		rArraySizes[i] = 40;
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



HRESULT CDllTool::getTransformationDefinitions(std::array<TransformDefs, 1>& transformdefs)
{
	
	transformdefs[0].imageinput = 0;
	transformdefs[0].imageoutput = NUM_RESULT_IMAGES;// for all outputs images 
	transformdefs[0].resultindex = RESULTVALUE_DOUBLE_ARRAY_FUNCTION;
	return S_OK;

}

#pragma endregion