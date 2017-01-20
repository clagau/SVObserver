//******************************************************************************
//* COPYRIGHT (c) 2008 by Seidenader Vision Inc., Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVStaticStringValueObjectClass
//* .File Name       : $Workfile:   SVStaticStringValueObjectClass.h  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.1  $
//* .Check In Date   : $Date:   18 Nov 2013 13:26:12  $
//******************************************************************************
#pragma once

#pragma region Includes
#include "SVOResource/resource.h"
#include "SVUtilityLibrary/SVString.h"
#include "SVStaticValueObjectClassImpl.h"
#pragma endregion Includes

class SVStaticStringValueObjectClass : public SVStaticValueObjectClassImpl < SVString >
{
	SV_DECLARE_CLASS( SVStaticStringValueObjectClass );

public:
	SVStaticStringValueObjectClass( LPCTSTR ObjectName );
	SVStaticStringValueObjectClass( SVObjectClass* POwner = nullptr, int StringResourceID = IDS_CLASSNAME_SVSTATICSTRINGVALUEOBJECT );
	SVStaticStringValueObjectClass( const SVStaticStringValueObjectClass& rhs);
	const SVStaticStringValueObjectClass& operator = (const SVStaticStringValueObjectClass& rhs);

	HRESULT GetValues(std::vector<SVString>& raValues) const {return GetArrayValuesImpl(0, raValues);}
	HRESULT GetValues(int iBucket, std::vector<SVString>& raValues) const {return GetArrayValuesImpl(0, raValues);}

	virtual ~SVStaticStringValueObjectClass();

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

	virtual HRESULT SetValueAtConvert( int iBucket, int iIndex, const SVString& value ) override { return SetValueAt(iBucket, iIndex, value); }
	virtual HRESULT SetValueAtConvert( int iBucket, int iIndex, double value ) override { return S_FALSE; }
	virtual HRESULT SetValueAtConvert( int iBucket, int iIndex, long value ) override { return S_FALSE; }
	virtual HRESULT SetValueAtConvert( int iBucket, int iIndex, int value ) override { return S_FALSE; }

	__forceinline  HRESULT SetValueAt( int iBucket, int iIndex, value_type value ) {return base::SetValueAt(iBucket, iIndex, value); }
	__forceinline  HRESULT GetValueAt( int iBucket, int iIndex, value_type& rValue ) const {return base::GetValueAt(iBucket, iIndex, rValue); }

protected:
	virtual HRESULT SetValueAt( int iBucket, int iIndex, const VARIANT& rValue );

	virtual HRESULT GetValueAt( int iBucket, int iIndex, long& rValue ) const;
	virtual HRESULT GetValueAt( int iBucket, int iIndex, double& rValue ) const;
	virtual HRESULT GetValueAt( int iBucket, int iIndex, VARIANT& rValue ) const;

private:
	void LocalInitialize();
};

