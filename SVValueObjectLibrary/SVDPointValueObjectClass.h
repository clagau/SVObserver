//******************************************************************************
//* COPYRIGHT (c) 2008 by Körber Pharma Inspection GmbH. All Rights Reserved
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
#include "Definitions/GlobalConst.h"
#include "SVUtilityLibrary/SVPoint.h"
#include "SVValueObjectClass.h"
#pragma endregion Includes

namespace SvVol
{

class SVDPointValueObjectClass : public SVValueObjectClass<SVPoint<double>>
{
	SV_DECLARE_CLASS

public:
	explicit SVDPointValueObjectClass(LPCTSTR ObjectName);
	SVDPointValueObjectClass( SVObjectClass* pOwner = nullptr, int StringResourceID = IDS_CLASSNAME_SVDPOINTVALUEOBJECT );
	const SVDPointValueObjectClass& operator = (const SVDPointValueObjectClass& rhs);

	virtual ~SVDPointValueObjectClass();

	void setStandardFormatString() override {} //not currently used in this class
	
	virtual bool  SetMinMaxValues(SVPoint<double> , SVPoint<double> )override { return false; };
	virtual bool  GetMinMaxValues(SVPoint<double>& , SVPoint<double>& ) const override{ return false; };
	virtual bool  setMinMaxValues(const _variant_t& , const _variant_t& ) override { return false; };
	virtual bool getMinMaxValues(_variant_t& , _variant_t& ) const override { return false; };

protected:
	virtual double ValueType2Double(const SVPoint<double>&) const override { return 0.0; }
	virtual _variant_t ValueType2Variant(const SVPoint<double>* pValue) const override;
	virtual SVPoint<double> Variant2ValueType(const _variant_t& rValue) const override;

	//! Convert a string in a SVDPointClass. Throw an exception if the string isn't convertible into a SVDPointClass
	//! \param strValue [in] The input string
	//! \returns converted value.
	virtual SVPoint<double> ConvertString2Type( const std::string& rValue ) const override;

	//! Convert SVDPointClass to std::string 
	//! \param rValue [in] Type to convert
	/// \returns the SVString
	virtual std::string ConvertType2String( const SVPoint<double>& rValue ) const override { return rValue.toString(); }

	//! Returns the variant type of the value object (NOTE this is a special SVObserver VT type!!)
	//! \returns the VT type
	virtual DWORD GetType() const override { return  SvDef::VT_DPOINT; };

	virtual void WriteValues(SvOi::IObjectWriter& rWriter) const override;
	virtual void WriteDefaultValues(SvOi::IObjectWriter& rWriter) const override;

private:
	void LocalInitialize();
};

} //namespace SvVol
