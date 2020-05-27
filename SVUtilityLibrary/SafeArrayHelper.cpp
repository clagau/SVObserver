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

_variant_t VariantToSafeArray(_variant_t var)
{
	if (0 != (var.vt & VT_ARRAY))
		return var;
	_variant_t res;

	switch (var.vt)
	{
		case VT_R8:
		{
			CComSafeArray<double> sa(1);
			sa.SetAt(0, var.dblVal);
			res.vt = VT_R8 | VT_ARRAY;
			res.parray = sa.Detach();
		}
		break;

		case VT_R4:
		{
			CComSafeArray<float> sa(1);
			sa.SetAt(0, var.fltVal);
			res.vt = VT_R4 | VT_ARRAY;
			res.parray = sa.Detach();
		}
		break;

		case VT_I4:
		{
			CComSafeArray<long> sa(1);
			sa.SetAt(0, var.lVal);
			res.vt = VT_I4 | VT_ARRAY;
			res.parray = sa.Detach();
		}
		break;

		case VT_UI4:
		{
			CComSafeArray<unsigned long> sa(1);
			sa.SetAt(0, var.ulVal);
			res.vt = VT_UI4 | VT_ARRAY;
			res.parray = sa.Detach();
		}
		break;
		case VT_I2:
		{

			CComSafeArray<short> sa(1);
			sa.SetAt(0, var.iVal);
			res.vt = VT_I2 | VT_ARRAY;
			res.parray = sa.Detach();
		}
		break;
		case VT_UI2:
		{
			CComSafeArray<unsigned short> sa(1);
			sa.SetAt(0, var.uiVal);
			res.vt = VT_I2 | VT_ARRAY;
			res.parray = sa.Detach();
		}
		break;

		default:
			res = var;

	}
	return res;
}

std::string VariantToString(_variant_t var)
{
	_bstr_t temp;
	if ((var.vt & VT_ARRAY) == 0)
	{
		if (VT_BOOL == var.vt)
		{
			return (0 == var.boolVal) ? "false" : "true";
		}
		else
		{
			temp = (_bstr_t)var;
			return  std::string(temp);
		}		
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





