//*****************************************************************************
/// \copyright (c) 2019,2019 by Körber Pharma Inspection GmbH. All Rights Reserved
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
std::pair<VTGroups, bool> getVTGroup(VARTYPE type)
{
	bool isArray = (VT_ARRAY == (type & VT_ARRAY));
	type = type & (~VT_ARRAY);
	switch (type)
	{
		case VT_EMPTY:
			return {VTGroups::Empty, isArray};
		case VT_BSTR:
			return {VTGroups::Text, isArray};
		case VT_I2:
		case VT_I4:
		case VT_R4:
		case VT_R8:
		case VT_BOOL:
		case VT_DECIMAL:
		case VT_I1:
		case VT_UI1:
		case VT_UI2:
		case VT_UI4:
		case VT_I8:
		case VT_UI8:
		case VT_INT:
		case VT_UINT:
			return {VTGroups::Numbers, isArray};
		default:
			return {VTGroups::Unknown, isArray};
	}
}

_variant_t GetEmptySafeArray(VARTYPE  vart)
{

	VARTYPE type = vart & (~VT_ARRAY);
	_variant_t res;

	switch (type)
	{
		case VT_R8:
		{
			CComSafeArray<double> sa(ULONG(0), LONG(0));
			res.vt = VT_R8 | VT_ARRAY;
			res.parray = sa.Detach();
		}
		break;

		case VT_R4:
		{
			CComSafeArray<float> sa(ULONG(0), LONG(0));

			res.vt = VT_R4 | VT_ARRAY;
			res.parray = sa.Detach();
		}
		break;

		case VT_I4:
		{
			CComSafeArray<long>  sa(ULONG(0), LONG(0));
			res.vt = VT_I4 | VT_ARRAY;
			res.parray = sa.Detach();
		}
		break;

		case VT_UI4:
		{
			CComSafeArray<unsigned long>  sa(ULONG(0), LONG(0));

			res.vt = VT_UI4 | VT_ARRAY;
			res.parray = sa.Detach();
		}
		break;
		case VT_I2:
		{

			CComSafeArray<short>  sa(ULONG(0), LONG(0));

			res.vt = VT_I2 | VT_ARRAY;
			res.parray = sa.Detach();
		}
		break;
		case VT_UI2:
		{
			CComSafeArray<unsigned short> sa(ULONG(0), LONG(0));

			res.vt = VT_I2 | VT_ARRAY;
			res.parray = sa.Detach();
		}
		break;

		case VT_BSTR:
		{
			CComSafeArray<BSTR> sa(ULONG(0), LONG(0));

			res.vt = VT_BSTR | VT_ARRAY;
			res.parray = sa.Detach();
		}
		break;

	}
	return res;
}


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

template<typename SourceType, typename DestinationType>
_variant_t convertSafeArrayToOtherSafeArray(_variant_t var, VARTYPE  newType)
{
	_variant_t res;
	res.vt = newType;
	CComSafeArray<DestinationType> sa(var.parray->rgsabound[0].cElements);
	for (long i = 0; i < static_cast<int>(var.parray->rgsabound[0].cElements); ++i)
	{
		SourceType sourceValue;
		if (S_OK == ::SafeArrayGetElement(var.parray, &i, &sourceValue))
		{
			DestinationType value = static_cast<DestinationType>(sourceValue);
			sa.SetAt(i, value);
		}
	}
	res.parray = sa.Detach();
	return res;
}

_variant_t convertSafeArrayToOtherSafeArray(_variant_t var, VARTYPE  newType)
{
	if ((VT_ARRAY & var.vt) == 0 || (VT_ARRAY & newType) == 0 || nullptr == var.parray)
	{
		return {};
	}

	switch ((~VT_ARRAY) & newType)
	{
		case VT_R8:
		{
			switch ((~VT_ARRAY) & var.vt)
			{
				case VT_R4:
					return convertSafeArrayToOtherSafeArray<float, double>(var, newType);
				case VT_I4:
					return convertSafeArrayToOtherSafeArray<long, double>(var, newType);
				case VT_UI4:
					return convertSafeArrayToOtherSafeArray<unsigned long, double>(var, newType);
				case VT_I2:
					return convertSafeArrayToOtherSafeArray<short, double>(var, newType);
				case VT_UI2:
					return convertSafeArrayToOtherSafeArray<unsigned short, double>(var, newType);
			}
			break;
		}

		case VT_R4:
		{
			switch ((~VT_ARRAY) & var.vt)
			{
				case VT_R8:
					return convertSafeArrayToOtherSafeArray<double, float>(var, newType);
				case VT_I4:
					return convertSafeArrayToOtherSafeArray<long, float>(var, newType);
				case VT_UI4:
					return convertSafeArrayToOtherSafeArray<unsigned long, float>(var, newType);
				case VT_I2:
					return convertSafeArrayToOtherSafeArray<short, float>(var, newType);
				case VT_UI2:
					return convertSafeArrayToOtherSafeArray<unsigned short, float>(var, newType);
			}
			break;
		}

		case VT_I4:
		{
			switch ((~VT_ARRAY) & var.vt)
			{
				case VT_R8:
					return convertSafeArrayToOtherSafeArray<double, long>(var, newType);
				case VT_R4:
					return convertSafeArrayToOtherSafeArray<float, long>(var, newType);
				case VT_UI4:
					return convertSafeArrayToOtherSafeArray<unsigned long, long>(var, newType);
				case VT_I2:
					return convertSafeArrayToOtherSafeArray<short, long>(var, newType);
				case VT_UI2:
					return convertSafeArrayToOtherSafeArray<unsigned short, long>(var, newType);
			}
			break;
		}

		case VT_UI4:
		{
			switch ((~VT_ARRAY) & var.vt)
			{
				case VT_R8:
					return convertSafeArrayToOtherSafeArray<double, unsigned long>(var, newType);
				case VT_R4:
					return convertSafeArrayToOtherSafeArray<float, unsigned long>(var, newType);
				case VT_I4:
					return convertSafeArrayToOtherSafeArray<long, unsigned long>(var, newType);
				case VT_I2:
					return convertSafeArrayToOtherSafeArray<short, unsigned long>(var, newType);
				case VT_UI2:
					return convertSafeArrayToOtherSafeArray<unsigned short, unsigned long>(var, newType);
			}
			break;
		}

		case VT_I2:
		{
			switch ((~VT_ARRAY) & var.vt)
			{
				case VT_R8:
					return convertSafeArrayToOtherSafeArray<double, short>(var, newType);
				case VT_R4:
					return convertSafeArrayToOtherSafeArray<float, short>(var, newType);
				case VT_I4:
					return convertSafeArrayToOtherSafeArray<long, short>(var, newType);
				case VT_UI4:
					return convertSafeArrayToOtherSafeArray<unsigned long, short>(var, newType);
				case VT_UI2:
					return convertSafeArrayToOtherSafeArray<unsigned short, short>(var, newType);
			}
			break;
		}

		case VT_UI2:
		{
			switch ((~VT_ARRAY) & var.vt)
			{
				case VT_R8:
					return convertSafeArrayToOtherSafeArray<double, unsigned short>(var, newType);
				case VT_R4:
					return convertSafeArrayToOtherSafeArray<float, unsigned short>(var, newType);
				case VT_I4:
					return convertSafeArrayToOtherSafeArray<long, unsigned short>(var, newType);
				case VT_UI4:
					return convertSafeArrayToOtherSafeArray<unsigned long, unsigned short>(var, newType);
				case VT_I2:
					return convertSafeArrayToOtherSafeArray<short, unsigned short>(var, newType);
			}
			break;
		}
	}

	return {};
}

std::string VariantToString(_variant_t var)
{
	_bstr_t temp;
	if ((var.vt & VT_ARRAY) == 0)
	{
		if (VT_BOOL == var.vt)
		{
			return (0 == var.boolVal) ? "FALSE" : "TRUE";
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

bool isSameVar(const _variant_t& rValue1, const _variant_t& rValue2)
{
	if (rValue1.vt != rValue2.vt)
	{
		return false;
	}

	if ((rValue1.vt & VT_ARRAY) != VT_ARRAY)
	{
		return rValue1 == rValue2;
	}
	else
	{
		if (nullptr == rValue1.parray || nullptr == rValue2.parray)
		{
			return rValue1.parray == rValue2.parray;
		}

		if (rValue1.parray->cDims != rValue2.parray->cDims)
		{
			return false;
		}

		Log_Assert(1 == rValue1.parray->cDims); //for other dims, it is not implemented.
		if (1 == rValue1.parray->cDims)
		{
			size_t size1 = rValue1.parray->cbElements * rValue1.parray->rgsabound[0].cElements;
			size_t size2 = rValue2.parray->cbElements * rValue2.parray->rgsabound[0].cElements;

			if (size1 != size2)
			{
				return false;
			}

			if ((VT_ARRAY | VT_BSTR) != rValue1.vt)
			{
				return 0 == memcmp(rValue1.parray->pvData, rValue2.parray->pvData, size1);
			}
			else
			{
				//not implemented for VT_BSTR
				Log_Assert(false);
				return false;
			}
		}
		else
		{
			return false;
		}
	}
}

namespace
{
template<typename T>
variant_t vectorToSafeArray(const std::vector<T>& rVec, VARTYPE varType)
{
	SAFEARRAYBOUND arrayBound;
	arrayBound.lLbound = 0;
	arrayBound.cElements = static_cast<ULONG>(rVec.size());
	_variant_t result;
	result.parray = ::SafeArrayCreate(varType, 1, &arrayBound);
	result.vt = varType | VT_ARRAY;

	for (long i = 0; i < rVec.size(); ++i)
	{
		T val = rVec[i];
		::SafeArrayPutElement(result.parray, &i, static_cast<void*>(&val));
	}

	return result;
}
}

template<>
variant_t vectorToSafeArray<double>(const std::vector<double>& rVec)
{
	return vectorToSafeArray(rVec, VT_R8);
}

template<>
variant_t vectorToSafeArray<long>(const std::vector<long>& rVec)
{
	return vectorToSafeArray(rVec, VT_I4);
}

template<>
variant_t vectorToSafeArray<__int64>(const std::vector<__int64>& rVec)
{
	return vectorToSafeArray(rVec, VT_I8);
}
}





