//*****************************************************************************
/// \copyright (c) 2016,2016 by Seidenader Maschinenbau GmbH
/// \file DoubleSortValueObject.cpp
/// All Rights Reserved 
//*****************************************************************************
/// This class is a doubleValueObject, but the order of the array will be defined by a map.
//*****************************************************************************

#pragma region Includes
#include "stdafx.h"
#include "DoubleSortValueObject.h"
#include "SVObjectLibrary\SVClsids.h"
#include "SVObjectLibrary\SVObjectLibrary.h"
#pragma endregion Includes

#pragma region Declarations
#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

SV_IMPLEMENT_CLASS(DoubleSortValueObject, DoubleSortValueObjectGuid);
#pragma endregion Declarations

#pragma region Constructor
DoubleSortValueObject::DoubleSortValueObject(LPCTSTR ObjectName)
	: SVDoubleValueObjectClass(ObjectName) 
{
	LocalInitialize();
}

DoubleSortValueObject::DoubleSortValueObject(SVObjectClass* POwner, int StringResourceID)
	: SVDoubleValueObjectClass(POwner, StringResourceID) 
{
	LocalInitialize();
}

DoubleSortValueObject::DoubleSortValueObject( const DoubleSortValueObject& rhs )
	: SVDoubleValueObjectClass()
{
	LocalInitialize();
	*this = rhs;
}

const DoubleSortValueObject& DoubleSortValueObject::operator = (const DoubleSortValueObject& rhs)
{
	base::operator = (rhs);
	return *this;
}

DoubleSortValueObject::~DoubleSortValueObject()
{
	// Delete all object's data arrays...
	SetObjectDepth(0);
	CreateBuckets();
}
#pragma endregion Constructor

#pragma region Public Methods

#pragma endregion Public Methods

#pragma region Protected Methods
HRESULT DoubleSortValueObject::SetValueAt( int iBucket, int iIndex, const VARIANT& rvtValue )
{
	if (0 <= iIndex && m_sortContainer.size()>iIndex)
	{
		return SVDoubleValueObjectClass::SetValueAt(iBucket, m_sortContainer[iIndex], rvtValue);
	}
	return S_FALSE;
}

HRESULT DoubleSortValueObject::SetValueAt(int iBucket, int iIndex, const int value)
{
	if (0 <= iIndex && m_sortContainer.size()>iIndex)
	{
		return SVDoubleValueObjectClass::SetValueAt(iBucket, m_sortContainer[iIndex], value);
	}
	return S_FALSE;
}

HRESULT DoubleSortValueObject::SetValueAt(int iBucket, int iIndex, const long value)
{
	if (0 <= iIndex && m_sortContainer.size()>iIndex)
	{
		return SVDoubleValueObjectClass::SetValueAt(iBucket, m_sortContainer[iIndex], value);
	}
	return S_FALSE;
}

HRESULT DoubleSortValueObject::SetValueAt(int iBucket, int iIndex, const double value)
{
	if (0 <= iIndex && m_sortContainer.size()>iIndex)
	{
		return SVDoubleValueObjectClass::SetValueAt(iBucket, m_sortContainer[iIndex], value);
	}
	return S_FALSE;
}

HRESULT DoubleSortValueObject::SetValueAt( int iBucket, int iIndex, CString value )
{
	if (0 <= iIndex && m_sortContainer.size()>iIndex)
	{
		return SVDoubleValueObjectClass::SetValueAt(iBucket, m_sortContainer[iIndex], value);
	}
	return S_FALSE;
}

HRESULT DoubleSortValueObject::GetValueAt( int iBucket, int iIndex, double& rValue ) const
{
	if (0 <= iIndex && m_sortContainer.size()>iIndex)
	{
		return SVDoubleValueObjectClass::GetValueAt(iBucket, m_sortContainer[iIndex], rValue);
	}
	return S_FALSE;
}

HRESULT DoubleSortValueObject::GetValueAt( int iBucket, int iIndex, long& rValue ) const
{
	if (0 <= iIndex && m_sortContainer.size()>iIndex)
	{
		return SVDoubleValueObjectClass::GetValueAt(iBucket, m_sortContainer[iIndex], rValue);
	}
	return S_FALSE;
}

HRESULT DoubleSortValueObject::GetValueAt( int iBucket, int iIndex, DWORD& rValue ) const
{
	if (0 <= iIndex && m_sortContainer.size()>iIndex)
	{
		return SVDoubleValueObjectClass::GetValueAt(iBucket, m_sortContainer[iIndex], rValue);
	}
	return S_FALSE;
}

HRESULT DoubleSortValueObject::GetValueAt( int iBucket, int iIndex, CString& rValue) const
{
	if (0 <= iIndex && m_sortContainer.size()>iIndex)
	{
		return SVDoubleValueObjectClass::GetValueAt(iBucket, m_sortContainer[iIndex], rValue);
	}
	return S_FALSE;
}

HRESULT DoubleSortValueObject::GetValueAt( int iBucket, int iIndex, VARIANT& rValue ) const
{
	if (0 <= iIndex && m_sortContainer.size()>iIndex)
	{
		return SVDoubleValueObjectClass::GetValueAt(iBucket, m_sortContainer[iIndex], rValue);
	}
	return S_FALSE;
}
#pragma endregion Protected Methods

#pragma region Private Methods
void DoubleSortValueObject::LocalInitialize()
{
	m_outObjectInfo.ObjectTypeInfo.ObjectType = DoubleSortValueObjectType;
	m_strTypeName = "Decimal";
	m_bLegacyVectorObjectCompatibility = false;
}
#pragma endregion Private Methods
