//******************************************************************************
//* COPYRIGHT (c) 2008 by Seidenader Vision Inc., Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVStringValueObjectClass
//* .File Name       : $Workfile:   SVStringValueObjectClass.h  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.0  $
//* .Check In Date   : $Date:   23 Apr 2013 15:15:24  $
//******************************************************************************

#pragma once

#pragma region Includes
#include "SVOResource/resource.h"
#include "SVValueObjectClassImpl.h"
#include "SVUtilityLibrary/SVString.h"
#pragma endregion Includes

class SVStringValueObjectClass : public SVValueObjectClassImpl < SVString >
{
	SV_DECLARE_CLASS( SVStringValueObjectClass );

public:
	SVStringValueObjectClass( LPCTSTR ObjectName );
	SVStringValueObjectClass( SVObjectClass* POwner = nullptr, int StringResourceID = IDS_CLASSNAME_SVSTRINGVALUEOBJECT );
	SVStringValueObjectClass( const SVStringValueObjectClass& rhs);
	const SVStringValueObjectClass& operator = (const SVStringValueObjectClass& rhs);

	virtual ~SVStringValueObjectClass();

	virtual HRESULT SetObjectValue( SVObjectAttributeClass* pDataObject ) override;
	virtual void Persist(SVObjectWriter& rWriter) override;

//	IMPLEMENT_VALUE_OBJECT_GET_SET()
	template <typename FROMTYPE>
	__forceinline HRESULT SetValue(int iBucket, FROMTYPE value) { return SetValueAt(iBucket, 0, value); }
	template <typename FROMTYPE>
	__forceinline HRESULT SetValue(int iBucket, int iIndex, FROMTYPE value) { return SetValueAt(iBucket, iIndex, value); }
	
	template <typename TOTYPE>
	__forceinline HRESULT GetValue(TOTYPE& rValue) const { return GetValueAt(m_iLastSetIndex, 0, rValue); }
	template <typename TOTYPE>
	__forceinline HRESULT GetValue(int iBucket, TOTYPE& rValue) const { return GetValueAt(iBucket, 0, rValue); }
	template <typename TOTYPE>
	__forceinline HRESULT GetValue(int iBucket, int iIndex, TOTYPE& rValue) const { return GetValueAt(iBucket, iIndex, rValue); }

	virtual HRESULT SetValueAtConvert( int iBucket, int iIndex, const SVString& rValue ) override { return SetValueAt(iBucket, iIndex, rValue); }
	virtual HRESULT SetValueAtConvert( int iBucket, int iIndex, double Value ) override { return S_FALSE; } //SetValueAt(iBucket, iIndex, value); }
	virtual HRESULT SetValueAtConvert( int iBucket, int iIndex, long Value ) override { return S_FALSE; } //return SetValueAt(iBucket, iIndex, value); }
	virtual HRESULT SetValueAtConvert( int iBucket, int iIndex, int Value ) override { return S_FALSE; } //SetValueAt(iBucket, iIndex, value); }

	__forceinline  HRESULT SetValueAt( int iBucket, int iIndex, value_type Value ) {return base::SetValueAt(iBucket, iIndex, Value); }
	__forceinline  HRESULT GetValueAt( int iBucket, int iIndex, value_type& rValue ) const {return base::GetValueAt(iBucket, iIndex, rValue); }

protected:
	virtual HRESULT SetValueAt( int iBucket, int iIndex, const VARIANT& rValue );

	virtual HRESULT GetValueAt( int iBucket, int iIndex, long& rValue ) const;
	virtual HRESULT GetValueAt( int iBucket, int iIndex, double& rValue ) const;
	virtual HRESULT GetValueAt( int iBucket, int iIndex, VARIANT& rValue ) const;

private:
	void LocalInitialize();
};

