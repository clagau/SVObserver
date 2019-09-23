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
#include "Definitions/GlobalConst.h"
#include "SVUtilityLibrary/SVPoint.h"
#include "SVValueObjectClass.h"
#pragma endregion Includes

namespace SvVol
{

class SVDPointValueObjectClass : public SVValueObjectClass<SVPoint<double>>
{
	SV_DECLARE_CLASS( SVDPointValueObjectClass );

public:
	SVDPointValueObjectClass( LPCTSTR ObjectName );
	SVDPointValueObjectClass( SVObjectClass* pOwner = nullptr, int StringResourceID = IDS_CLASSNAME_SVDPOINTVALUEOBJECT );
	const SVDPointValueObjectClass& operator = (const SVDPointValueObjectClass& rhs);

	virtual ~SVDPointValueObjectClass();

protected:
	virtual double ValueType2Double(const SVPoint<double>& rValue) const override { return E_NOTIMPL; };
	virtual _variant_t ValueType2Variant( const SVPoint<double>& rValue ) const override;
	virtual SVPoint<double> Variant2ValueType( const _variant_t& rValue ) const override;

	//! Convert a string in a SVDPointClass. Throw an exception if the string isn't convertible into a SVDPointClass
	//! \param strValue [in] The input string
	//! \returns converted value.
	virtual SVPoint<double> ConvertString2Type( const std::string& rValue ) const override;

	//! Convert SVDPointClass to std::string 
	//! \param rValue [in] Type to convert
	/// \returns the SVString
	virtual std::string ConvertType2String( const SVPoint<double>& rValue ) const override { return rValue.toString(); }

	//! Returns the value object byte size (SVDPointClass has x and y value both of type double)
	//! \returns the number of bytes for the data
	virtual long GetByteSize(bool useResultSize = true) const override;

	//! Returns the variant type of the value object (NOTE this is a special SVObserver VT type!!)
	//! \returns the VT type
	virtual DWORD GetType() const override { return  SvDef::VT_DPOINT; };

	//! Copies the value object to the memory block
	//! \param pMemoryBlock [in] Pointer to the byte address of the memory block
	//! \param MemByteSize [in] The memory block byte size
	//! \returns the number of bytes copied
	virtual long CopyToMemoryBlock(BYTE* pMemoryBlock, long MemByteSize) const override;

	virtual void WriteValues(SvOi::IObjectWriter& rWriter) override;
	virtual void WriteDefaultValues(SvOi::IObjectWriter& rWriter) override;

private:
	void LocalInitialize();
};

} //namespace SvVol
