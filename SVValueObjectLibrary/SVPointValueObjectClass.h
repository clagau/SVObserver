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
#include "SVUtilityLibrary/SVString.h"
#include "SVUtilityLibrary/SVPOINT.h"
#include "SVValueObjectClass.h"
#pragma endregion Includes

class SVPointValueObjectClass : public SVValueObjectClass<SVPOINT>
{
	SV_DECLARE_CLASS( SVPointValueObjectClass );

public:
	SVPointValueObjectClass( LPCTSTR ObjectName );
	SVPointValueObjectClass( SVObjectClass* POwner = nullptr, int StringResourceID = IDS_CLASSNAME_SVPOINTVALUEOBJECT );
	SVPointValueObjectClass( const SVPointValueObjectClass& rhs );
	const SVPointValueObjectClass& operator = (const SVPointValueObjectClass& rhs);

	virtual ~SVPointValueObjectClass();

protected:
	virtual double ValueType2Double(const SVPOINT& rValue) const override { return E_NOTIMPL; };
	virtual _variant_t ValueType2Variant( const SVPOINT& rValue ) const override;
	virtual SVPOINT Variant2ValueType( const _variant_t& rValue ) const override;

	/// Convert a string in a SVPOINT. Throw an exception if the string isn't convertible into a SVPOINT
	/// \param strValue [in] The input string
	/// \returns the converted.
	virtual SVPOINT ConvertString2Type(const SVString& rValue ) const override;

	//! Convert SVDPointClass to SVString 
	//! \param rValue [in] Type to convert
	/// \returns the SVString
	virtual SVString ConvertType2String( const SVPOINT& rValue ) const override;

	//! Returns the value object byte size (SVPOINT has x and y value both of type long)
	//! \returns the number of bytes for the data
	virtual DWORD GetByteSize() const override { return 2*sizeof(long); };

	//! Returns the variant type of the value object (NOTE this is a special SVObserver VT type!!)
	//! \returns the VT type
	virtual DWORD GetType() const override { return VT_POINT; };

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

