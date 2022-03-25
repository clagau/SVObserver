//******************************************************************************
//* COPYRIGHT (c) 2008 by Körber Pharma Inspection GmbH. All Rights Reserved
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
#include "Definitions/StringTypeDef.h"
#include "Definitions/TextDefineSVDef.h"
#include "SVValueObjectClass.h"
#pragma endregion Includes

namespace SvVol
{

class SVBoolValueObjectClass : public SVValueObjectClass<BOOL>
{
	SV_DECLARE_CLASS

public:
	explicit SVBoolValueObjectClass(LPCTSTR ObjectName);
	SVBoolValueObjectClass( SVObjectClass* POwner = nullptr, int StringResourceID = IDS_CLASSNAME_SVBOOLVALUEOBJECT );
	SVBoolValueObjectClass(const SVBoolValueObjectClass& rhs);
	const SVBoolValueObjectClass& operator = (const SVBoolValueObjectClass& rhs);

	virtual ~SVBoolValueObjectClass();

	HRESULT GetValidTypes( SvDef::StringVector& rTypes ) const;

	void setStandardFormatString() override {} //not currently used in this class

protected:
	virtual double ValueType2Double(const BOOL& rValue) const override { return static_cast<double> (rValue); }
	virtual _variant_t ValueType2Variant(const BOOL* pValue) const override { return (nullptr != pValue) ? _variant_t( *pValue ? true : false ) : _variant_t{};}
	virtual BOOL Variant2ValueType(const _variant_t& rValue) const override { return BOOL( rValue ? true : false ); };

	/// Convert a string in a bool. Throw an exception if the string isn't convertible into a bool
	/// \param rValue [in] The input string: "True", 1 or -1 is true, "false" and 0 is false.
	/// \returns bool Return value.
	virtual BOOL ConvertString2Type( const std::string& rValue ) const override;

	virtual std::string ConvertType2String( const BOOL& rValue ) const override {	return rValue ? SvDef::cTrue : SvDef::cFalse; };

	virtual void WriteValues(SvOi::IObjectWriter& rWriter) const override;
	virtual void WriteDefaultValues(SvOi::IObjectWriter& rWriter) const override;
private:
	void LocalInitialize();
};

} //namespace SvVol
