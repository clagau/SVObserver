//*****************************************************************************
/// \copyright (c) 2017,2017 by Seidenader Maschinenbau GmbH
/// All Rights Reserved 
//*****************************************************************************
/// This is Class includes the main functionalities of the external dll.
//******************************************************************************
#pragma once
#include <mil.h>
#include <vector>
#include "defines.h"
#include "StructDefinitions.h"
#include <comutil.h>

class CDllTool
{
public:
	//Constructor/Destructor
	CDllTool();
	~CDllTool();

#pragma region static function
	/**********
	 Return the input value definition.
	 \param ppaStructs <in/output> the structure must be preallocated with the length NUM_INPUT_VALUES.
	***********/
	static void getInputValuesDefinition(InputValueDefinitionStruct** ppaStructs);
	/**********
	 Return the result value definition.
	 \param ppaResultValues <in/output> the structure must be preallocated with the length NUM_RESULT_VALUES.
	***********/
	static void getResultValueDefinition(ResultValueDefinitionStruct** ppaResultValues);
	/**********
	 Check if the value valid and return an error code. S_OK if value valid.
	 \param lParameterNumber <in> the index of the parameter. Must be at least 0 and less than NUM_INPUT_VALUES.
	 \param vParameterValue <in> the parameter value
	 \return hresult value
	***********/
	static HRESULT validateInputValues(long lParameterNumber, const	VARIANT vParameterValue);
	/**********
	 Return an error string to an error code.
	 \param ulErrorNumber <in> the error code.
	 \param vParameterValue <out> the error string. Will be allocated by the function and must be freed by the caller. If return value not S_OK, nothing is allocated.
	 \return hresult value
	***********/
	static HRESULT getErrorMessageString(unsigned long ulErrorNumber, BSTR* pbstrErrorMessage);
#pragma endregion

#pragma region public function
public:
	/**********
	 This function initialized the class and prepared it for the run. It reset all inputs and outputs, and allocated the ring buffer.
	 \param p_paStructs <in> an array of the image definitionStructs for the input images, the size must be NUM_INPUT_IMAGES.
	 \param p_pavInputValues <in> an array of the current input values, the size must be NUM_INPUT_VALUES.
	 \return hresult value
	***********/
	HRESULT initRun(const ImageDefinitionStruct* const p_paStructs, const VARIANT* const p_pavInputValues);
	/**********
	 This function run this dll function.
	 \return hresult value
	***********/
	HRESULT run();
#pragma region set function
	/**********
	 Set the GUID-number for this class.
	 \param guid GUID-number
	***********/
	void setGUID(const GUID guid);
	/**********
	 Set the input values for the next run.
	 \param p_pavInputValues <in> an array of the current input values, the size must be NUM_INPUT_VALUES.
	 \return hresult value
	***********/
	HRESULT setInputValues(const VARIANT* const p_pavInputValues);
	/**********
	 Set the input MIL images for the next run.
	 \param paHandles <in> an array of the input handles.
	 \return hresult value
	***********/
	HRESULT setMILInputImages (const long* const paMILhandles);
	/**********
	 Set the MIl handles to the result images. 
	 \param ppaStructs <in/output> the handles of the result images, the size must be NUM_RESULT_IMAGES.
	 \return hresult value
	***********/
	HRESULT setMILResultImages (long* paMILHandles);
#pragma endregion
#pragma region get function
	/**********
	 Return the result image definition. It create the structure. It HAVE TO be freed by the caller
	 \param plArraySize <output> the size of the result images, it is NUM_RESULT_IMAGES.
	 \param ppaStructs <output> it allocate the the structure must be preallocated with the length NUM_RESULT_VALUES.
	 \return hresult value
	***********/
	HRESULT getResultImageDefs (long* plArraySize, ImageDefinitionStruct** ppaStructs) const;
	/**********
	 Return the result values.
	 \param ppaStructs <in/output> it must be preallocated by the caller, the size must be NUM_RESULT_VALUES. It return the values of the last run.
	 \return hresult value
	***********/
	HRESULT getResultValues(VARIANT* paResultValues) const;
#pragma endregion
#pragma endregion

#pragma region private function
private:
	/**********
	 Copy an HBITIMAGES. 
	 \param l_hBitMapSource <in> the handle to the source image.
	 \param l_hBitMapDestination <in> the handle to the destination image.
	 \return hresult value
	***********/
	//HRESULT copyHBitMap(HBITMAP l_hBitMapSource, HBITMAP l_hBitMapDestination);
	/**********
	 Clear the data of the class. Clear the vectors and destroy the objects in this vectors. Have to be called by the destructor.
	 ***********/
	void cleanData();
#pragma endregion

#pragma region member parameters
private:
	GUID m_GUID;

	std::vector<_variant_t> m_aInputValues;
	std::vector<_variant_t> m_aResultValues;

	// Images
	std::vector<ImageDefinitionStruct> m_aResultImageDefs;
	std::vector<MIL_ID> m_aInputImages;
	std::vector<MIL_ID> m_aResultImages;

	//TODO: add your result parameter and delete comment sample afterwards.
	static const int c_I1_minValue = 0;
	static const int c_I1_maxValue = 1000000;

#pragma endregion
};