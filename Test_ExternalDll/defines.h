#pragma once

//TODO: set all defines and static parameters to you needed values
#define	DLLVERSION 2,0,0,0
#define TOOLNAME					"Test External DLL"
#define	DLLINTERFACEVERSION		"DLL-V 2.0 (15.06.2021)"
#define	LSTRGROUPNAME			TOOLNAME "-ExternalDll"
#define DEBUGFILE				"C:\\SVIMDLLDebug_" TOOLNAME ".txt"

static const char	static_strToolName[512]=TOOLNAME;
static const int	static_iToolVersion = 12;

//ATTENTION: Do not asign numbers to the enums. The NUM_* entries must be the last entry of every enum.
//			 If only the NUM_* defined, this input or result is zero.
//TODO: add enums for your inputs and result (DO NOT assign numbers) and remove comment sample afterwards
enum InputValuesEnum
{
	InputValue_LONG,
	InputValue_DOUBLE,
	InputValue_BSTR,
	InputValue_DOUBLE_ARRAY,
	InputValue_INT_ARRAY,
	InputValue_TABLE_ARRAY,
	InputValue_TABLE_NAMES,
	InputValue_Second_Double_Array,
	InputValue_LONG_TABLE_SELECT,
	InpitValue_LONG_DELAY_MS,
	NUM_INPUT_VALUES
};
enum InputImagesEnum
{
	InputImage_1,
	InputImage_2,
	NUM_INPUT_IMAGES
};
enum ResultValuesEnum
{
	ResultValue_LONG,
	ResultValue_DOUBLE,
	ResultValue_BSTR,
	ResultValue_DOUBLE_ARRAY,
	ResultValue_INT_ARRAY,
	ResultValue_INT_ROWCOUNT,
	RESULTVALUE_BSTR_ROWNAME,
	RESULTVALUE_DOUBLE_ARRAY_ROW,
	NUM_RESULT_VALUES
};
enum ResultImagesEnum
{
	ResultImage_1,
	ResultImage_2,
	NUM_RESULT_IMAGES
};
enum ResultTablesEnum
{
	FirstResultTable ,
	SecondResultTable ,
	NUM_RESULT_TABLES 
};

#define ColumnCountA 6
#define RowCountA 9


//ATTENTION: S_FALSE is 1L, increase for every define not the last digital.
//TODO: add your error code and delete comment sample afterwards
#define ERRORCODE_INPUTVALUE_LONG_INVALID S_FALSE|10
#define  ERRORCODE_INPUTVALUE_DOUBLE_ARRAY_WRONGTYPE  S_FALSE|20
#define  ERRORCODE_INPUTVALUE_DOUBLE_ARRAY_TOOSMALL  S_FALSE|30
#define	 ERRORCODE_INPUTVALUE_INT_ARRAY_WRONGTYPE   S_FALSE|40
