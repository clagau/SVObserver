//*****************************************************************************
/// \copyright (c) 2019,2019 by Seidenader Maschinenbau GmbH
/// \file SafeArrayHelper.h
/// All Rights Reserved 
//*****************************************************************************
//Helper function for SAFEARRAY
//******************************************************************************
#pragma once
namespace SvUl
{

	/// function converts var to string. If var is a 1 dim safearray the string is the comma seperated values 
	std::string VariantToString(_variant_t var);

	//! function returns a 1 dim safe array with one element for an scalar variant
	_variant_t VariantToSafeArray(_variant_t var);

	//! function returns a 1 dim safe array with 0 element 
	//_variant_t GetEmptySafeArray(VARTYPE  vart);

	/// function converts 1 dim safearray of type T to  ; seperated values 
	template <typename T> std::string SafeArrayToString(SAFEARRAY  *pArray)
	{
		std::stringstream sst;
		CComSafeArray<T> safeArray(pArray);
		for (int i = safeArray.GetLowerBound(); i <= safeArray.GetUpperBound(); i++)
		{
			sst << safeArray[i];
			if (i < safeArray.GetUpperBound())
			{
				sst << ";";
			}
		}
		return  sst.str();


	}

	///function creates a 1 dim safe array of type T from comma seperated string. return value is size of array  
	///empty string is array of size 0, negative result signalks an error.
	template <typename T> int StringToSafeArray(const std::string& value, _variant_t& rvariant)
	{
		

		if (value == "")
		{
			rvariant.Clear();
			CComSafeArray<T> sa(ULONG(0), LONG(0));
			rvariant.vt = sa.GetType() | VT_ARRAY;
			rvariant.parray = sa.Detach();
			return 0;
		}

		std::vector<std::string> words;
		boost::algorithm::split(words, value, boost::is_any_of(" ,;"), boost::token_compress_on);
		if (words.size() == 0)
		{
			return -1;
		}
		
		CComSafeArray<T> sa(ULONG(words.size()));
		for (int i = 0; i < words.size(); i++)
		{
			try
			{
				sa[i] = boost::lexical_cast<T>(words[i]);
			}
			catch (...)
			{
				return -1;
			}
		}

		rvariant.Clear();
		int ret = sa.GetCount();
		rvariant.vt = sa.GetType() | VT_ARRAY;
		rvariant.parray = sa.Detach();
		return ret;
	}


}