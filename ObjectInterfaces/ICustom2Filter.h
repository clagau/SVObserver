//******************************************************************************
/// COPYRIGHT (c) 2015 by Seidenader Maschinenbau GmbH
/// All Rights Reserved
//******************************************************************************
/// Interface class for class custom 2 filter
//******************************************************************************
#pragma once

#pragma region Includes
//Moved to precompiled header: #include <vector>
#pragma endregion Includes

namespace SvOi
{
	//@TODO: this two const used for the error message in importCustom2Filter. The function should later be void and throw an exception. Then this const can be removed.
	const HRESULT E_CUSTOM_IMPORT_FORMAT_INVALID = E_INVALIDARG;
	const HRESULT E_CUSTOM_IMPORT_VERSION_MISMATCH = RPC_E_VERSION_MISMATCH;

	//************************************
	/// Load a custom 2 filter from a file to the application. 
	/// It returns the values of the filter.
	/// \param filePath [in] File name (plus path) of the filter.
	/// \param kernelWidth [out]
	/// \param kernelHeight [out]
	/// \param normalizationFactor [out]
	/// \param absoluteValue [out]
	/// \param clippingEnabled [out]
	/// \param kernelArray [out]
	/// \returns HRESULT
	//************************************
	HRESULT importCustom2Filter(const std::string &filePath,
		long &kernelWidth, 
		long &kernelHeight, 
		long &normalizationFactor, 
		BOOL &absoluteValue, 
		BOOL &clippingEnabled,
		std::vector<long> &kernelArray);

	//************************************
	/// Save a custom 2 filter to a file
	/// \param filePath [in] File name (plus path) of the filter.
	/// \param kernelWidth [in]
	/// \param kernelHeight [in]
	/// \param normalizationFactor [in]
	/// \param absoluteValue [in]
	/// \param clippingEnabled [in]
	/// \param kernelIteratorBegin [in]
	/// \param kernelIteratorEnd [in]
	/// \returns void
	//************************************
	void exportCustom2Filter(const std::string &filePath, 
		long kernelWidth, 
		long kernelHeight, 
		long normalizationFactor, 
		BOOL absoluteValue, 
		BOOL clippingEnabled,
		const std::vector<long>::const_iterator kernelIteratorBegin,
		const std::vector<long>::const_iterator kernelIteratorEnd);
} //namespace SvOi