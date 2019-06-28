//*****************************************************************************
/// \copyright (c) 2019,2019 by Seidenader Maschinenbau GmbH
/// \file SafeArrayHelper.cpp
/// All Rights Reserved 
//*****************************************************************************
//Helper function for SAFEARRAY
//******************************************************************************
#pragma region Includes
#include "stdafx.h"
#include <boost/lexical_cast.hpp>
#include "Safearrayhelper.h"
#include <atlsafe.h>
#pragma endregion Includes
namespace SvUl
{

std::string VariantToString(_variant_t var)
{
	_bstr_t temp;
	if(  (var.vt & VT_ARRAY) == 0)
	{
		temp = (_bstr_t)var;
		return  std::string(temp);
	}
	else if (var.vt == (VT_ARRAY | VT_R8))
	{
	
		return SafeArrayToString<double>(var.parray);

	}
	else if (var.vt == (VT_ARRAY | VT_R4))
	{

		return SafeArrayToString<float>(var.parray);

	}
	else if (var.vt == (VT_ARRAY | VT_I4))
	{

		return SafeArrayToString<long>(var.parray);

	}
	else if (var.vt == (VT_ARRAY | VT_UI4))
	{

		return SafeArrayToString<unsigned long>(var.parray);

	}
	else if (var.vt == (VT_ARRAY | VT_I8))
	{

		return SafeArrayToString<long long>(var.parray);

	}
	else if (var.vt == (VT_ARRAY | VT_I2))
	{

		return SafeArrayToString<short>(var.parray);

	}
	else if (var.vt == (VT_ARRAY | VT_UI2))
	{

		return SafeArrayToString<unsigned short>(var.parray);

	}
	else return std::string();

}



}





