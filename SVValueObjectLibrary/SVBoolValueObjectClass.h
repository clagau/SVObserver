//******************************************************************************
//* COPYRIGHT (c) 2008 by Seidenader Vision Inc., Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVBoolValueObjectClass
//* .File Name       : $Workfile:   SVBoolValueObjectClass.h  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.1  $
//* .Check In Date   : $Date:   30 Oct 2013 15:18:36  $
//******************************************************************************

#pragma once

#pragma region Includes
#include "SVOResource/resource.h"
#include "ObjectInterfaces/TextDefineSvOi.h"
#include "SVUtilityLibrary/SVString.h"
#include "SVValueObjectClass.h"
#pragma endregion Includes

class SVBoolValueObjectClass : public SVValueObjectClass<BOOL>
{
	SV_DECLARE_CLASS( SVBoolValueObjectClass );

public:
	SVBoolValueObjectClass( LPCTSTR ObjectName );
	SVBoolValueObjectClass( SVObjectClass* POwner = nullptr, int StringResourceID = IDS_CLASSNAME_SVBOOLVALUEOBJECT );
	SVBoolValueObjectClass(const SVBoolValueObjectClass& rhs);
	const SVBoolValueObjectClass& operator = (const SVBoolValueObjectClass& rhs);

	virtual ~SVBoolValueObjectClass();
	virtual void Persist(SVObjectWriter& rWriter) override;

	HRESULT GetValidTypes( SVStringVector& rTypes ) const;

protected:
	virtual double ValueType2Double(const BOOL& rValue) const override { return static_cast<double> (rValue); };
	virtual _variant_t ValueType2Variant( const BOOL& rValue ) const override { return _variant_t( rValue ? true : false ); };
	virtual BOOL Variant2ValueType( const _variant_t& rValue ) const override { return BOOL( rValue ? true : false ); };

	/// Convert a string in a bool. Throw an exception if the string isn't convertible into a bool
	/// \param rValue [in] The input string: "True", 1 or -1 is true, "false" and 0 is false.
	/// \returns bool Return value.
	virtual BOOL ConvertString2Type( const SVString& rValue ) const override;

	virtual SVString ConvertType2String( const BOOL& rValue ) const override {	return rValue ? SvOi::cTrue : SvOi::cFalse; };

private:
	void LocalInitialize();
};
