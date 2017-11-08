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

protected:
	virtual double ValueType2Double(const SVDPointClass& rValue) const override { return E_NOTIMPL; };
	virtual _variant_t ValueType2Variant( const SVDPointClass& rValue ) const override;
	virtual SVDPointClass Variant2ValueType( const _variant_t& rValue ) const override;

	//! Convert a string in a SVDPointClass. Throw an exception if the string isn't convertible into a SVDPointClass
	//! \param strValue [in] The input string
	//! \returns converted value.
	virtual SVDPointClass ConvertString2Type( const std::string& rValue ) const override;

	//! Convert SVDPointClass to std::string 
	//! \param rValue [in] Type to convert
	/// \returns the SVString
	virtual std::string ConvertType2String( const SVDPointClass& rValue ) const override;

	//! Returns the value object byte size (SVDPointClass has x and y value both of type double)
	//! \returns the number of bytes for the data
	virtual DWORD GetByteSize() const override { return 2*sizeof(double); };

	//! Returns the variant type of the value object (NOTE this is a special SVObserver VT type!!)
	//! \returns the VT type
	virtual DWORD GetType() const override { return  VT_DPOINT; };

	//! Copies the value object to the memory block
	//! \param pMemoryBlock [in] Pointer to the byte address of the memory block
	//! \param MemByteSize [in] The memory block byte size
	//! \param Index [in] The index of the array (-1 if no array)
	//! \returns S_OK if successful
	virtual HRESULT CopyToMemoryBlock(BYTE* pMemoryBlock, DWORD MemByteSize, int Index = -1) const override;

	virtual void WriteValues(SVObjectWriter& rWriter) override;
	virtual void WriteDefaultValues(SVObjectWriter& rWriter) override;

private:
	void LocalInitialize();
};
