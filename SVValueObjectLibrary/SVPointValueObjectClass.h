//******************************************************************************
//* COPYRIGHT (c) 2008 by Seidenader Vision Inc., Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVPointValueObjectClass
//* .File Name       : $Workfile:   SVPointValueObjectClass.h  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.0  $
//* .Check In Date   : $Date:   23 Apr 2013 13:29:38  $
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

class SVPointValueObjectClass : public SVValueObjectClass<SVPoint<long>>
{
	SV_DECLARE_CLASS( SVPointValueObjectClass );

public:
	SVPointValueObjectClass( LPCTSTR ObjectName );
	SVPointValueObjectClass( SVObjectClass* POwner = nullptr, int StringResourceID = IDS_CLASSNAME_SVPOINTVALUEOBJECT );
	SVPointValueObjectClass( const SVPointValueObjectClass& rhs );
	const SVPointValueObjectClass& operator = (const SVPointValueObjectClass& rhs);

	virtual ~SVPointValueObjectClass();

protected:
	virtual double ValueType2Double(const SVPoint<long>& rValue) const override { return E_NOTIMPL; };
	virtual _variant_t ValueType2Variant( const SVPoint<long>& rValue ) const override;
	virtual SVPoint<long> Variant2ValueType( const _variant_t& rValue ) const override;

	/// Convert a string in a SVPOINT. Throw an exception if the string isn't convertible into a SVPOINT
	/// \param strValue [in] The input string
	/// \returns the converted.
	virtual SVPoint<long> ConvertString2Type(const std::string& rValue ) const override;

	//! Convert SVDPointClass to std::string 
	//! \param rValue [in] Type to convert
	/// \returns the std::string
	virtual std::string ConvertType2String( const SVPoint<long>& rValue ) const override { return rValue.toString(); }

	//! Returns the value object byte size (SVPOINT has x and y value both of type long)
	//! \returns the number of bytes for the data
	virtual long GetByteSize(bool useResultSize = true) const override;

	//! Returns the variant type of the value object (NOTE this is a special SVObserver VT type!!)
	//! \returns the VT type
	virtual DWORD GetType() const override { return SvDef::VT_POINT; };

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
