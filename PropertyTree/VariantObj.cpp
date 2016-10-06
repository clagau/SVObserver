// ******************************************************************************
// * COPYRIGHT (c) 2000 by SVResearch, Harrisburg
// * All Rights Reserved
// ******************************************************************************
// * .Module Name     : CVariantObj
// * .File Name       : $Workfile:   VariantObj.cpp  $
// * ----------------------------------------------------------------------------
// * .Current Version : $Revision:   1.0  $
// * .Check In Date   : $Date:   18 Apr 2013 16:43:22  $
// ******************************************************************************
#pragma region Includes
#include "stdafx.h"
//Moved to precompiled header: #include <atlconv.h>
//Moved to precompiled header: #include <math.h>
#include "VariantObj.h"
#pragma endregion Includes

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

#pragma warning(push)
#pragma warning(disable:4290)
#pragma warning(disable:4310)

//////////////////////////////////////////////////////////////////////////////////////////
//
// Constructors
//
//////////////////////////////////////////////////////////////////////////////////////////

CVariantObj::CVariantObj()
{
	::VariantInit(this);
}

CVariantObj::CVariantObj(const VARIANT& varSrc)
{
	::VariantInit(this);
	Copy(varSrc);
}

CVariantObj::CVariantObj(LPCVARIANT pSrc)
{
	::VariantInit(this);
	Copy(pSrc);
}

CVariantObj::CVariantObj(LPCTSTR lpszSrc, VARTYPE vtSrc) 
{
	SetString(lpszSrc, vtSrc);
}

CVariantObj::CVariantObj(bool bSrc)
{
	V_VT(this) = VT_BOOL;
	V_BOOL(this) = (bSrc ? VARIANT_TRUE : VARIANT_FALSE);
}

CVariantObj::CVariantObj(BYTE bSrc)
{
	V_VT(this) = VT_UI1;
	V_UI1(this) = bSrc;
}

CVariantObj::CVariantObj(short iSrc, VARTYPE vtSrc) 
{
	if (vtSrc != VT_I2 && vtSrc != VT_BOOL)
		_com_issue_error(E_INVALIDARG);

	if (vtSrc == VT_BOOL)
	{
		V_VT(this) = VT_BOOL;
		V_BOOL(this) = (iSrc ? VARIANT_TRUE : VARIANT_FALSE);
	}
	else
	{
		V_VT(this) = VT_I2;
		V_I2(this) = iSrc;
	}
}

CVariantObj::CVariantObj(USHORT uiSrc) 
{
	V_VT(this)  = VT_UI2;
	V_UI2(this) = uiSrc;
}

CVariantObj::CVariantObj(long lSrc, VARTYPE vtSrc) 
{
	if (vtSrc != VT_I4 && vtSrc != VT_ERROR && vtSrc != VT_BOOL)
		_com_issue_error(E_INVALIDARG);

	if (vtSrc == VT_BOOL)
	{
		V_VT(this) = VT_BOOL;
		V_BOOL(this) = (lSrc ? VARIANT_TRUE : VARIANT_FALSE);
	}
	else if (vtSrc == VT_ERROR)
	{
		V_VT(this) = VT_ERROR;
		V_ERROR(this) = lSrc;
	}
	else
	{
		V_VT(this) = VT_I4;
		V_I4(this) = lSrc;
	}
}

CVariantObj::CVariantObj(ULONG ulSrc) 
{
	V_VT(this) = VT_UI4;
	V_UI4(this) = ulSrc;
}

CVariantObj::CVariantObj(int intSrc) 
{
	V_VT(this) = VT_INT;
	V_INT(this) = intSrc;
}

CVariantObj::CVariantObj(UINT uintSrc) 
{
	V_VT(this) = VT_UINT;
	V_UINT(this) = uintSrc;
}

CVariantObj::CVariantObj(float fltSrc)
{
	V_VT(this) = VT_R4;
	V_R4(this) = fltSrc;
}

CVariantObj::CVariantObj(double dblSrc) 
{
	V_VT(this) = VT_R8;
	V_R8(this) = dblSrc;
}

//////////////////////////////////////////////////////////////////////////////////////////
//
// Extractors
//
//////////////////////////////////////////////////////////////////////////////////////////

CVariantObj::operator bool() const 
{
	CVariantObj varDest;
	varDest.CopyInd(this);
	
	if (V_VT(&varDest) != VT_BOOL)
	{
		varDest.ChangeType(VT_BOOL);
	}
	
	return V_BOOL(&varDest) ? true : false;
}

CVariantObj::operator BYTE() const 
{
	CVariantObj varDest;
	varDest.CopyInd(this);
	
	if (V_VT(&varDest) != VT_UI1)
	{
		varDest.ChangeType(VT_UI1);
	}
	
	return V_UI1(&varDest);
}

CVariantObj::operator short() const 
{
	CVariantObj varDest;
	varDest.CopyInd(this);
	
	if (V_VT(&varDest) != VT_I2)
	{
		varDest.ChangeType(VT_I2);
	}
	
	return V_I2(&varDest);
}

CVariantObj::operator USHORT() const 
{
	CVariantObj varDest;
	varDest.CopyInd(this);
	
	if (V_VT(&varDest) != VT_UI2)
	{
		varDest.ChangeType(VT_UI2);
	}
	
	return V_UI2(&varDest);
}

CVariantObj::operator long() const 
{
	CVariantObj varDest;
	varDest.CopyInd(this);
	
	if (V_VT(&varDest) != VT_I4)
	{
		varDest.ChangeType(VT_I4);
	}
	
	return V_I4(&varDest);
}

CVariantObj::operator ULONG() const 
{
	CVariantObj varDest;
	varDest.CopyInd(this);
	
	if (V_VT(&varDest) != VT_UI4)
	{
		varDest.ChangeType(VT_UI4);
	}
	
	return V_UI4(&varDest);
}

CVariantObj::operator int() const 
{
	CVariantObj varDest;
	varDest.CopyInd(this);
	
	if (V_VT(&varDest) != VT_INT)
	{
		varDest.ChangeType(VT_INT);
	}
	
	return V_INT(&varDest);
}

CVariantObj::operator UINT() const 
{
	CVariantObj varDest;
	varDest.CopyInd(this);
	
	if (V_VT(&varDest) != VT_UINT)
	{
		varDest.ChangeType(VT_UINT);
	}
	
	return V_UINT(&varDest);
}

CVariantObj::operator float() const 
{
	CVariantObj varDest;
	varDest.CopyInd(this);
	
	if (V_VT(&varDest) != VT_R4)
	{
		varDest.ChangeType(VT_R4);
	}
	
	return V_R4(&varDest);
}

CVariantObj::operator double() const 
{
	CVariantObj varDest;
	varDest.CopyInd(this);
	
	if (V_VT(&varDest) != VT_R8)
	{
		varDest.ChangeType(VT_R8);
	}
	
	return V_R8(&varDest);
}

CVariantObj::operator _bstr_t() const 
{
	CVariantObj varDest;
	varDest.CopyInd(this);
	
	if (V_VT(&varDest) != VT_BSTR)
	{
		varDest.ChangeType(VT_BSTR);
	}
	
	return V_BSTR(&varDest);
}

//////////////////////////////////////////////////////////////////////////////////////////
//
// Assignment operations
//
//////////////////////////////////////////////////////////////////////////////////////////

const CVariantObj& CVariantObj::operator=(const VARIANT& varSrc) 
{
	_com_util::CheckError(::VariantCopy(this, const_cast<VARIANT*>(&varSrc)));
	
	return *this;
}

const CVariantObj& CVariantObj::operator=(LPCVARIANT pSrc) 
{
	_com_util::CheckError(::VariantCopy(this, const_cast<VARIANT*>(pSrc)));
	
	return *this;
}

const CVariantObj& CVariantObj::operator=(bool bSrc) 
{
	switch (V_VT(this))
	{
		case VT_BOOL:
			V_BOOL(this) = (bSrc ? VARIANT_TRUE : VARIANT_FALSE);
			break;
		case VT_BOOL | VT_BYREF:
			*V_BOOLREF(this) = (bSrc ? VARIANT_TRUE : VARIANT_FALSE);
			break;
		default:
			Clear();
			V_VT(this) = VT_BOOL;
			V_BOOL(this) = (bSrc ? VARIANT_TRUE : VARIANT_FALSE);
			break;
	}
	
	return *this;
}

const CVariantObj& CVariantObj::operator=(BYTE bSrc) 
{
	switch (V_VT(this))
	{
		case VT_UI1:
			V_UI1(this) = bSrc;
			break;
		case VT_UI1 | VT_BYREF:
			*V_UI1REF(this) = bSrc;
			break;
		default:
			Clear();
			V_VT(this) = VT_UI1;
			V_UI1(this) = bSrc;
			break;
	}
	
	return *this;
}

const CVariantObj& CVariantObj::operator=(short iSrc) 
{
	switch (V_VT(this))
	{
		case VT_I2:
			V_I2(this) = iSrc;
			break;
		case VT_I2 | VT_BYREF:
			*V_I2REF(this) = iSrc;
			break;
		default:
			Clear();
			V_VT(this) = VT_I2;
			V_I2(this) = iSrc;
			break;
	}
	
	return *this;
}

const CVariantObj& CVariantObj::operator=(USHORT uiSrc) 
{
	switch (V_VT(this))
	{
		case VT_UI2:
			V_UI2(this) = uiSrc;
			break;
		case VT_UI2 | VT_BYREF:
			*V_UI2REF(this) = uiSrc;
			break;
		default:
			Clear();
			V_VT(this) = VT_UI2;
			V_UI2(this) = uiSrc;
			break;
	}
	
	return *this;
}

const CVariantObj& CVariantObj::operator=(int intSrc) 
{
	switch (V_VT(this))
	{
		case VT_INT:
			V_INT(this) = intSrc;
			break;
		case VT_INT | VT_BYREF:
			*V_INTREF(this) = intSrc;
			break;
		default:
			Clear();
			V_VT(this) = VT_INT;
			V_INT(this) = intSrc;
			break;
	}
	
	return *this;
}

const CVariantObj& CVariantObj::operator=(UINT uintSrc) 
{
	switch (V_VT(this))
	{
		case VT_UINT:
			V_UINT(this) = uintSrc;
			break;
		case VT_UINT | VT_BYREF:
			*V_UINTREF(this) = uintSrc;
			break;
		default:
			Clear();
			V_VT(this) = VT_UINT;
			V_UINT(this) = uintSrc;
			break;
	}
	
	return *this;
}

const CVariantObj& CVariantObj::operator=(long lSrc) 
{
	switch (V_VT(this))
	{
		case VT_I4:
			V_I4(this) = lSrc;
			break;
		case VT_I4 | VT_BYREF:
			*V_I4REF(this) = lSrc;
			break;
		default:
			Clear();
			V_VT(this) = VT_I4;
			V_I4(this) = lSrc;
			break;
	}
	
	return *this;
}

const CVariantObj& CVariantObj::operator=(ULONG ulSrc) 
{
	switch (V_VT(this))
	{
		case VT_UI4:
			V_UI4(this) = ulSrc;
			break;
		case VT_UI4 | VT_BYREF:
			*V_UI4REF(this) = ulSrc;
			break;
		default:
			Clear();
			V_VT(this) = VT_UI4;
			V_UI4(this) = ulSrc;
			break;
	}
	
	return *this;
}

const CVariantObj& CVariantObj::operator=(float fltSrc) 
{
	switch (V_VT(this))
	{
		case VT_R4:
			V_R4(this) = fltSrc;
			break;
		case VT_R4 | VT_BYREF:
			*V_R4REF(this) = fltSrc;
			break;
		default:
			Clear();
			V_VT(this) = VT_R4;
			V_R4(this) = fltSrc;
			break;
	}
	
	return *this;
}

const CVariantObj& CVariantObj::operator=(double dblSrc) 
{
	switch (V_VT(this))
	{
		case VT_R8:
			V_R8(this) = dblSrc;
			break;
		case VT_R8 | VT_BYREF:
			*V_R8REF(this) = dblSrc;
			break;
		default:
			Clear();
			V_VT(this) = VT_R8;
			V_R8(this) = dblSrc;
			break;
	}
	
	return *this;
}

const CVariantObj& CVariantObj::operator=(const LPCTSTR lpszSrc) 
{
	// Clear the VARIANT (This will SysFreeString() any previous occupant)
	//
	Clear();
	
	V_VT(this) = VT_BSTR;
	V_BSTR(this) = _com_util::ConvertStringToBSTR(lpszSrc);
	
	if (nullptr == V_BSTR(this) && nullptr != lpszSrc)
	{
		_com_issue_error(E_OUTOFMEMORY);
	}
	
	return *this;
}

const CVariantObj& CVariantObj::operator=(const CString& strSrc) 
{
	// Clear the VARIANT (This will SysFreeString() any previous occupant)
	//
	Clear();
	
	V_VT(this) = VT_BSTR;
	V_BSTR(this) = _com_util::ConvertStringToBSTR(strSrc);
	
	if (nullptr == V_BSTR(this) )
	{
		_com_issue_error(E_OUTOFMEMORY);
	}
	
	return *this;
}

//////////////////////////////////////////////////////////////////////////////////////////
//
// Comparison operations
//
//////////////////////////////////////////////////////////////////////////////////////////

bool CVariantObj::operator==(LPCVARIANT pSrc) const
{
	bool	bResult = false;

	//
	// Variants not equal if types don't match
	//
	if (V_VT(this) == V_VT(pSrc))
	{
		//
		// Check type specific values
		//
		switch (V_VT(this))
		{
			case VT_EMPTY:
				bResult = true;
				break;
			case VT_BOOL:
				bResult = V_BOOL(this) == V_BOOL(pSrc);
				break;
			case VT_I1:
				bResult = V_I1(this) == V_I1(pSrc);
				break;
			case VT_UI1:
				bResult = V_UI1(this) == V_UI1(pSrc);
				break;
			case VT_I2:
				bResult = V_I2(this) == V_I2(pSrc);
				break;
			case VT_UI2:
				bResult = V_UI2(this) == V_UI2(pSrc);
				break;
			case VT_I4:
				bResult = V_I4(this) == V_I4(pSrc);
				break;
			case VT_UI4:
				bResult = V_UI4(this) == V_UI4(pSrc);
				break;
			case VT_INT:
				bResult = V_INT(this) == V_INT(pSrc);
				break;
			case VT_UINT:
				bResult = V_UINT(this) == V_UINT(pSrc);
				break;
			case VT_R4:
				bResult = V_R4(this) == V_R4(pSrc);
				break;
			case VT_R8:
				bResult = V_R8(this) == V_R8(pSrc);
				break;
			case VT_DATE:
				bResult = V_DATE(this) == V_DATE(pSrc);
				break;
			case VT_CY:
				bResult = V_CY(this).int64 == V_CY(pSrc).int64;
				break;
			case VT_BSTR:
				bResult = wcscmp(V_BSTR(this), V_BSTR(pSrc)) == 0;
				break;
			default:
				bResult = false;
				break;
		}
	}
	
	return bResult;
}

bool CVariantObj::operator!=(LPCVARIANT pSrc) const
{
	return !(*this == pSrc);
}

bool CVariantObj::operator<(LPCVARIANT pSrc) const
{
	bool	bResult = false;
	//
	// Variants not equal if types don't match
	//
	if (V_VT(this) == V_VT(pSrc))
	{
		//
		// Check type specific values
		//
		switch (V_VT(this))
		{
			case VT_EMPTY:
				bResult = false;
				break;
			case VT_BOOL:
				bResult = V_BOOL(this) < V_BOOL(pSrc);
				break;
			case VT_I1:
				bResult = V_I1(this) < V_I1(pSrc);
				break;
			case VT_UI1:
				bResult = V_UI1(this) < V_UI1(pSrc);
				break;
			case VT_I2:
				bResult = V_I2(this) < V_I2(pSrc);
				break;
			case VT_UI2:
				bResult = V_UI2(this) < V_UI2(pSrc);
				break;
			case VT_I4:
				bResult = V_I4(this) < V_I4(pSrc);
				break;
			case VT_UI4:
				bResult = V_UI4(this) < V_UI4(pSrc);
				break;
			case VT_INT:
				bResult = V_INT(this) < V_INT(pSrc);
				break;
			case VT_UINT:
				bResult = V_UINT(this) < V_UINT(pSrc);
				break;
			case VT_R4:
				bResult = V_R4(this) < V_R4(pSrc);
				break;
			case VT_R8:
				bResult = V_R8(this) < V_R8(pSrc);
				break;
			case VT_DATE:
				bResult = V_DATE(this) < V_DATE(pSrc);
				break;
			case VT_CY:
				bResult = V_CY(this).int64 < V_CY(pSrc).int64;
				break;
			case VT_BSTR:
				bResult = wcscmp(V_BSTR(this), V_BSTR(pSrc)) < 0 ? true : false;
				break;
			default:
				bResult = false;
				break;
		}
	}
	
	return bResult;
}

bool CVariantObj::operator>(LPCVARIANT pSrc) const
{
	bool	bResult = false;
	//
	// Variants not equal if types don't match
	//
	if (V_VT(this) == V_VT(pSrc))
	{
		//
		// Check type specific values
		//
		switch (V_VT(this))
		{
			case VT_EMPTY:
				bResult = false;
				break;
			case VT_BOOL:
				bResult = V_BOOL(this) > V_BOOL(pSrc);
				break;
			case VT_I1:
				bResult = V_I1(this) > V_I1(pSrc);
				break;
			case VT_UI1:
				bResult = V_UI1(this) > V_UI1(pSrc);
				break;
			case VT_I2:
				bResult = V_I2(this) > V_I2(pSrc);
				break;
			case VT_UI2:
				bResult = V_UI2(this) > V_UI2(pSrc);
				break;
			case VT_I4:
				bResult = V_I4(this) > V_I4(pSrc);
				break;
			case VT_UI4:
				bResult = V_UI4(this) > V_UI4(pSrc);
				break;
			case VT_INT:
				bResult = V_INT(this) > V_INT(pSrc);
				break;
			case VT_UINT:
				bResult = V_UINT(this) > V_UINT(pSrc);
				break;
			case VT_R4:
				bResult = V_R4(this) > V_R4(pSrc);
				break;
			case VT_R8:
				bResult = V_R8(this) > V_R8(pSrc);
				break;
			case VT_DATE:
				bResult = V_DATE(this) > V_DATE(pSrc);
				break;
			case VT_CY:
				bResult = V_CY(this).int64 > V_CY(pSrc).int64;
				break;
			case VT_BSTR:
				bResult = wcscmp(V_BSTR(this), V_BSTR(pSrc)) > 0 ? true : false;
				break;
			default:
				bResult = false;
				break;
		}
	}
	
	return bResult;
}

bool CVariantObj::operator<=(LPCVARIANT pSrc) const
{
	bool	bResult = false;
	//
	// Variants not equal if types don't match
	//
	if (V_VT(this) == V_VT(pSrc))
	{
		//
		// Check type specific values
		//
		switch (V_VT(this))
		{
			case VT_EMPTY:
				bResult = false;
				break;				
			case VT_BOOL:
				bResult = V_BOOL(this) <= V_BOOL(pSrc);
				break;
			case VT_I1:
				bResult = V_I1(this) <= V_I1(pSrc);
				break;
			case VT_UI1:
				bResult = V_UI1(this) <= V_UI1(pSrc);
				break;
			case VT_I2:
				bResult = V_I2(this) <= V_I2(pSrc);
				break;
			case VT_UI2:
				bResult = V_UI2(this) <= V_UI2(pSrc);
				break;
			case VT_I4:
				bResult = V_I4(this) <= V_I4(pSrc);
				break;
			case VT_UI4:
				bResult = V_UI4(this) <= V_UI4(pSrc);
				break;
			case VT_INT:
				bResult = V_INT(this) <= V_INT(pSrc);
				break;
			case VT_UINT:
				bResult = V_UINT(this) <= V_UINT(pSrc);
				break;
			case VT_R4:
				bResult = V_R4(this) <= V_R4(pSrc);
				break;
			case VT_R8:
				bResult = V_R8(this) <= V_R8(pSrc);
				break;
			case VT_DATE:
				bResult = V_DATE(this) <= V_DATE(pSrc);
				break;
			case VT_CY:
				bResult = V_CY(this).int64 <= V_CY(pSrc).int64;
				break;
			case VT_BSTR:
				bResult = wcscmp(V_BSTR(this), V_BSTR(pSrc)) <= 0 ? true : false;
				break;
			default:
				bResult = false;
				break;
		}
	}
	
	return bResult;
}

bool CVariantObj::operator>=(LPCVARIANT pSrc) const
{
	bool	bResult = false;
	//
	// Variants not equal if types don't match
	//
	if (V_VT(this) == V_VT(pSrc))
	{
		//
		// Check type specific values
		//
		switch (V_VT(this))
		{
			case VT_EMPTY:
				bResult = false;
				break;				
			case VT_BOOL:
				bResult = V_BOOL(this) >= V_BOOL(pSrc);
				break;
			case VT_I1:
				bResult = V_I1(this) >= V_I1(pSrc);
				break;
			case VT_UI1:
				bResult = V_UI1(this) >= V_UI1(pSrc);
				break;
			case VT_I2:
				bResult = V_I2(this) >= V_I2(pSrc);
				break;
			case VT_UI2:
				bResult = V_UI2(this) >= V_UI2(pSrc);
				break;
			case VT_I4:
				bResult = V_I4(this) >= V_I4(pSrc);
				break;
			case VT_UI4:
				bResult = V_UI4(this) >= V_UI4(pSrc);
				break;
			case VT_INT:
				bResult = V_INT(this) >= V_INT(pSrc);
				break;
			case VT_UINT:
				bResult = V_UINT(this) >= V_UINT(pSrc);
				break;
			case VT_R4:
				bResult = V_R4(this) >= V_R4(pSrc);
				break;
			case VT_R8:
				bResult = V_R8(this) >= V_R8(pSrc);
				break;
			case VT_DATE:
				bResult = V_DATE(this) >= V_DATE(pSrc);
				break;
			case VT_CY:
				bResult = V_CY(this).int64 >= V_CY(pSrc).int64;
				break;
			case VT_BSTR:
				bResult = wcscmp(V_BSTR(this), V_BSTR(pSrc)) >= 0 ? true : false;
				break;
			default:
				bResult = false;
				break;
		}
	}
	
	return bResult;
}

bool CVariantObj::operator==(const VARIANT& varSrc) const
{
	return *this == &varSrc;
}

bool CVariantObj::operator!=(const VARIANT& varSrc) const
{
	return !(*this == &varSrc);
}

bool CVariantObj::operator<=(const VARIANT& varSrc) const
{
	return *this <= &varSrc;
}

bool CVariantObj::operator<(const VARIANT& varSrc) const
{
	return *this < &varSrc;
}

bool CVariantObj::operator>=(const VARIANT& varSrc) const
{
	return *this >= &varSrc;
}

bool CVariantObj::operator>(const VARIANT& varSrc) const
{
	return *this > &varSrc;
}

//////////////////////////////////////////////////////////////////////////////////////////
//
// Low-level operations
//
//////////////////////////////////////////////////////////////////////////////////////////

void CVariantObj::Clear() 
{
	_com_util::CheckError(::VariantClear(this));
}

void CVariantObj::Copy(const VARIANT& varSrc)
{
	_com_util::CheckError(::VariantCopy(this, const_cast<VARIANT*>(&varSrc)));
}

void CVariantObj::Copy(LPCVARIANT pSrc)
{
	_com_util::CheckError(::VariantCopy(this, const_cast<VARIANT*>(pSrc)));
}

void CVariantObj::CopyInd(const VARIANT& varSrc)
{
	_com_util::CheckError(::VariantCopyInd(this, const_cast<VARIANT*>(&varSrc)));
}

void CVariantObj::CopyInd(LPCVARIANT pSrc)
{
	_com_util::CheckError(::VariantCopyInd(this, const_cast<VARIANT*>(pSrc)));
}

void CVariantObj::SetString(LPCTSTR lpszSrc, VARTYPE vtSrc) 
{
	USES_CONVERSION;
	ASSERT(vtSrc == VT_BSTR || vtSrc == VT_BSTRT);
	UNUSED(vtSrc);
	
	// Free up previous VARIANT
	Clear();
	
	V_VT(this) = VT_BSTR;
	V_BSTR(this) = nullptr;
	
	if (nullptr != lpszSrc)
	{
#ifndef _UNICODE
		if (VT_BSTRT == vtSrc)
		{
			int nLen = lstrlen(lpszSrc);
			V_BSTR(this) = ::SysAllocStringByteLen(lpszSrc, nLen);
		}
		else
#endif
		{
			V_BSTR(this) = ::SysAllocString(T2COLE(lpszSrc));
		}
		
		if (nullptr == V_BSTR(this) )
		{
			_com_issue_error(E_OUTOFMEMORY);
		}
	}
}

void CVariantObj::Attach(VARIANT& varSrc) 
{
	//
	// Free up previous VARIANT
	//
	Clear();
	
	//
	// Give control of data to CVariantObj
	//
	memcpy(this, &varSrc, sizeof(varSrc));
	V_VT(&varSrc) = VT_EMPTY;
}

void CVariantObj::AttachRef(BYTE* pbSrc)
{
	Clear();
	V_VT(this) = VT_UI1 | VT_BYREF;
	V_UI1REF(this) = pbSrc;
}

void CVariantObj::AttachRef(short* piSrc) 
{
	Clear();
	V_VT(this) = VT_I2 | VT_BYREF;
	V_I2REF(this) = piSrc;
}

void CVariantObj::AttachRef(USHORT* puiSrc) 
{
	Clear();
	V_VT(this)  = VT_UI2 | VT_BYREF;
	V_UI2REF(this) = puiSrc;
}

void CVariantObj::AttachRef(long* plSrc) 
{
	Clear();
	V_VT(this) = VT_I4 | VT_BYREF;
	V_I4REF(this) = plSrc;
}

void CVariantObj::AttachRef(ULONG* pulSrc) 
{
	Clear();
	V_VT(this) = VT_UI4 | VT_BYREF;
	V_UI4REF(this) = pulSrc;
}

void CVariantObj::AttachRef(int* pintSrc) 
{
	Clear();
	V_VT(this) = VT_INT | VT_BYREF;
	V_INTREF(this) = pintSrc;
}

void CVariantObj::AttachRef(UINT* puintSrc) 
{
	Clear();
	V_VT(this) = VT_UINT | VT_BYREF;
	V_UINTREF(this) = puintSrc;
}

void CVariantObj::AttachRef(float* pfltSrc)
{
	Clear();
	V_VT(this) = VT_R4 | VT_BYREF;
	V_R4REF(this) = pfltSrc;
}

void CVariantObj::AttachRef(double* pdblSrc) 
{
	Clear();
	V_VT(this) = VT_R8 | VT_BYREF;
	V_R8REF(this) = pdblSrc;
}

VARIANT CVariantObj::Detach() 
{
	VARIANT varResult = *this;
	V_VT(this) = VT_EMPTY;
	
	return varResult;
}

void CVariantObj::ChangeType(VARTYPE vartype, const CVariantObj* pSrc, USHORT wFlags) 
{
	//
	// If pDest is nullptr, convert type in place
	//
	if (nullptr == pSrc)
	{
		pSrc = this;
	}
	
	if ((this != pSrc) || (vartype != V_VT(this)))
	{
		_com_util::CheckError(::VariantChangeType(static_cast<VARIANT*>(this),
										const_cast<VARIANT*>(static_cast<const VARIANT*>(pSrc)),
										wFlags, vartype));
	}
}

void CVariantObj::toString(CString& strSrc) 
{
	switch (V_VT(this))
	{
		case VT_BOOL:
			strSrc = V_BOOL(this) ? _T("True") : _T("False");
			break;
		case VT_BOOL | VT_BYREF:
			strSrc = *V_BOOLREF(this) ? _T("True") : _T("False");
			break;
		case VT_I1:
			strSrc.Format(_T("%c"), V_I1(this));
			break;
		case VT_I1 | VT_BYREF:
			strSrc.Format(_T("%c"), *V_I1REF(this));
			break;
		case VT_UI1:
			strSrc.Format(_T("%#01.1hx"), V_UI1(this));
//			strSrc.Format(_T("%hu"), V_UI1(this));
			break;
		case VT_UI1 | VT_BYREF:
			strSrc.Format(_T("%#01.1hx"), *V_UI1REF(this));
//			strSrc.Format(_T("%hu"), *V_UI1REF(this));
			break;
		case VT_I2:
			strSrc.Format(_T("%hd"), V_I2(this));
			break;
		case VT_I2 | VT_BYREF:
			strSrc.Format(_T("%hd"), *V_I2REF(this));
			break;
		case VT_UI2:
			strSrc.Format(_T("%#02.2hx"), V_UI2(this));
//			strSrc.Format(_T("%hu"), V_UI2(this));
			break;
		case VT_UI2 | VT_BYREF:
			strSrc.Format(_T("%#02.2hx"), *V_UI2REF(this));
//			strSrc.Format(_T("%hu"), *V_UI2REF(this));
			break;
		case VT_I4:
			strSrc.Format(_T("%ld"), V_I4(this));
			break;
		case VT_I4 | VT_BYREF:
			strSrc.Format(_T("%ld"), *V_I4REF(this));
			break;
		case VT_UI4:
			strSrc.Format(_T("%#04.4lx"), V_UI4(this));
//			strSrc.Format(_T("%lu"), V_UI4(this));
			break;
		case VT_UI4 | VT_BYREF:
			strSrc.Format(_T("%#04.4lx"), *V_UI4REF(this));
//			strSrc.Format(_T("%lu"), *V_UI4REF(this));
			break;
		case VT_INT:
			strSrc.Format(_T("%d"), V_INT(this));
			break;
		case VT_INT | VT_BYREF:
			strSrc.Format(_T("%d"), *V_INTREF(this));
			break;
		case VT_UINT:
			strSrc.Format(_T("%#04.4x"), V_UINT(this));
//			strSrc.Format(_T("%u"), V_UINT(this));
			break;
		case VT_UINT | VT_BYREF:
			strSrc.Format(_T("%#04.4x"), *V_UINTREF(this));
//			strSrc.Format(_T("%u"), *V_UINTREF(this));
			break;
		case VT_R4:
			strSrc.Format(_T("%g"), V_R4(this));
			break;
		case VT_R4 | VT_BYREF:
			strSrc.Format(_T("%g"), *V_R4REF(this));
			break;
		case VT_R8:
			strSrc.Format(_T("%g"), V_R8(this));
			break;
		case VT_R8 | VT_BYREF:
			strSrc.Format(_T("%g"), *V_R8REF(this));
			break;
		case VT_BSTR:
			strSrc = V_BSTR(this);
			break;
		case VT_BSTR | VT_BYREF:
			strSrc = *V_BSTRREF(this);
			break;
		default:
//			varDest.ChangeType(VT_BSTR);
//			strSrc = V_BSTR(&varDest);
			break;
	}
}

void CVariantObj::fromString(LPCTSTR lpszSrc) 
{
	TCHAR		*stopstring;
	
	switch (V_VT(this))
	{

	case VT_BOOL:
	case VT_BOOL | VT_BYREF:
		{
			bool val(false);
			if( _tcsncmp(lpszSrc,_T("1"), 1 ) == 0  ||  _tcsnicmp(lpszSrc,_T("True"), 4 ) == 0)
			{

				val = true;
			}

			if (V_ISBYREF(this))
			{
				*V_BOOLREF(this) = val; 
			}
			else 
			{
				V_BOOL(this) = val;
			}

		}
		break;
		
	case VT_UI1:
		case VT_UI1 | VT_BYREF:
		{
			ULONG ul = _tcstoul(lpszSrc, &stopstring, 0);
			if (*stopstring == 0)
			{
				if (V_ISBYREF(this))
					*V_UI1REF(this) = (BYTE)ul;
				else
					V_UI1(this) = (BYTE)ul;
			}
			break;
		}
		case VT_UI2:
		case VT_UI2 | VT_BYREF:
		{
			ULONG ul = _tcstoul(lpszSrc, &stopstring, 0);
			if (*stopstring == 0)
			{
				if (V_ISBYREF(this))
					*V_UI2REF(this) = (USHORT)ul;
				else
					V_UI2(this) = (USHORT)ul;
			}
			break;
		}
		case VT_UI4:
		case VT_UI4 | VT_BYREF:
		{
			ULONG ul = _tcstoul(lpszSrc, &stopstring, 0);
			if (*stopstring == 0)
			{
				if (V_ISBYREF(this))
					*V_UI4REF(this) = ul;
				else
					V_UI4(this) = ul;
			}
			break;
		}
		case VT_UINT:
		case VT_UINT | VT_BYREF:
		{
			ULONG ul = _tcstoul(lpszSrc, &stopstring, 0);
			if (*stopstring == 0)
			{
				if (V_ISBYREF(this))
					*V_UINTREF(this) = ul;
				else
					V_UINT(this) = ul;
			}
			break;
		}
		case VT_INT:
		case VT_INT | VT_BYREF:
		{
			long l = _tcstol(lpszSrc, &stopstring, 0);
			if (*stopstring == 0)
			{
				if (V_ISBYREF(this))
					*V_INTREF(this) = l;
				else
					V_INT(this) = l;
			}
			break;
		}
		case VT_I1:
		case VT_I1 | VT_BYREF:
		{
			if (_tcslen(lpszSrc) > 0)
			{
				if (V_ISBYREF(this))
					*V_I1REF(this) = (char)lpszSrc[0];
				else
					V_I1(this) = (char)lpszSrc;
			}
			break;
		}
		case VT_I2:
		case VT_I2 | VT_BYREF:
		{
			long l = _tcstol(lpszSrc, &stopstring, 0);
			if (*stopstring == 0)
			{
				if (V_ISBYREF(this))
					*V_I2REF(this) = (short)l;
				else
					V_I2(this) = (short)l;
			}
			break;
		}
		case VT_I4:
		case VT_I4 | VT_BYREF:
		{
			long l = _tcstol(lpszSrc, &stopstring, 0);
			if (*stopstring == 0)
			{
				if (V_ISBYREF(this))
					*V_I4REF(this) = l;
				else
					V_I4(this) = l;
			}
			break;
		}
		case VT_R4:
		case VT_R4 | VT_BYREF:
		{
			float flt = (float)_tcstod(lpszSrc, &stopstring);
			if (*stopstring == 0)
			{
				if (V_ISBYREF(this))
					*V_R4REF(this) = flt;
				else
					V_R4(this) = flt;
			}
			break;
		}
		case VT_R8:
		case VT_R8 | VT_BYREF:
		{
			double dbl = _tcstod(lpszSrc, &stopstring);
			if (*stopstring == 0)
			{
				if (V_ISBYREF(this))
					*V_R8REF(this) = dbl;
				else
					V_R8(this) = dbl;
			}
			break;
		}
		case VT_BSTR:
		case VT_BSTR | VT_BYREF:
			SetString(lpszSrc, VT_BSTR);
			break;
	}
}

//////////////////////////////////////////////////////////////////////////////////////////
//
// Destructor
//
//////////////////////////////////////////////////////////////////////////////////////////

CVariantObj::~CVariantObj() throw(_com_error)
{
	Clear();
}

#pragma warning(pop)

