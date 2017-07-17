//******************************************************************************
//* COPYRIGHT (c) 2008 by Seidenader Vision Inc., Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVDoubleValueObjectClass
//* .File Name       : $Workfile:   SVDoubleValueObjectClass.h  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.0  $
//* .Check In Date   : $Date:   23 Apr 2013 10:20:40  $
//******************************************************************************
#pragma once

#pragma region Includes
#include "SVOResource/resource.h"
#include "SVUtilityLibrary/SVString.h"
#include "SVValueObjectClass.h"
#pragma endregion Includes

class SVDoubleValueObjectClass : public SVValueObjectClass<double>
{
	SV_DECLARE_CLASS( SVDoubleValueObjectClass );

public:
	SVDoubleValueObjectClass( LPCTSTR ObjectName );
	SVDoubleValueObjectClass( SVObjectClass* POwner = nullptr , int StringResourceID = IDS_CLASSNAME_SVDOUBLEVALUEOBJECT );
	SVDoubleValueObjectClass( const SVDoubleValueObjectClass& rhs );
	const SVDoubleValueObjectClass& operator = (const SVDoubleValueObjectClass& rhs);

	virtual ~SVDoubleValueObjectClass();

	virtual HRESULT SetObjectValue( SVObjectAttributeClass* pDataObject ) override;	// for compat loading legacy SVDoubleVectorObjectClass

protected:
	virtual double ValueType2Double(const double& rValue) const override { return rValue; };
	virtual _variant_t ValueType2Variant( const double& rValue ) const override { return _variant_t( rValue ); };
	virtual double Variant2ValueType( const _variant_t& rValue ) const override { return double( rValue ); };

	//! Convert a string in a double. Throw an exception if the string isn't convertible into a double
	//! \param rValue [in] The input string
	//! \returns double value.
	virtual double ConvertString2Type( const SVString& rValue ) const override;

	virtual SVString ConvertType2String( const double& rValue ) const override { return FormatOutput(rValue); };

	virtual void WriteValues(SVObjectWriter &rWriter) override;
	virtual void WriteDefaultValues(SVObjectWriter& rWriter) override;
private:
	void LocalInitialize();
};

