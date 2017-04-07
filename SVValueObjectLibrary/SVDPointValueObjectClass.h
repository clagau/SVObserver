//******************************************************************************
//* COPYRIGHT (c) 2008 by Seidenader Vision Inc., Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVDPointValueObjectClass
//* .File Name       : $Workfile:   SVDPointValueObjectClass.h  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.0  $
//* .Check In Date   : $Date:   23 Apr 2013 10:20:56  $
//******************************************************************************
#pragma once

#pragma region Includes
#include "SVOResource/resource.h"
#include "SVUtilityLibrary/SVString.h"
#include "SVUtilityLibrary/SVDPointClass.h"
#include "SVValueObjectClass.h"
#pragma endregion Includes

class SVDPointValueObjectClass : public SVValueObjectClass<SVDPointClass>
{
	SV_DECLARE_CLASS( SVDPointValueObjectClass );

public:
	SVDPointValueObjectClass( LPCTSTR ObjectName );
	SVDPointValueObjectClass( SVObjectClass* pOwner = nullptr, int StringResourceID = IDS_CLASSNAME_SVDPOINTVALUEOBJECT );
	const SVDPointValueObjectClass& operator = (const SVDPointValueObjectClass& rhs);

	virtual ~SVDPointValueObjectClass();

	virtual void Persist(SVObjectWriter& rWriter) override;

protected:
	virtual double ValueType2Double(const SVDPointClass& rValue) const override { return E_NOTIMPL; };
	virtual _variant_t ValueType2Variant( const SVDPointClass& rValue ) const override;
	virtual SVDPointClass Variant2ValueType( const _variant_t& rValue ) const override;

	//! Convert a string in a SVDPointClass. Throw an exception if the string isn't convertible into a SVDPointClass
	//! \param strValue [in] The input string
	//! \returns converted value.
	virtual SVDPointClass ConvertString2Type( const SVString& rValue ) const override;

	//! Convert SVDPointClass to SVString 
	//! \param rValue [in] Type to convert
	/// \returns the SVString
	virtual SVString ConvertType2String( const SVDPointClass& rValue ) const override;

private:
	void LocalInitialize();
};
