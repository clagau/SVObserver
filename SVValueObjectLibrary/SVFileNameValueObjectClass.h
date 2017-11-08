//******************************************************************************
//* COPYRIGHT (c) 2008 by Seidenader Vision Inc., Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVFileNameValueObjectClass
//* .File Name       : $Workfile:   SVFileNameValueObjectClass.h  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.0  $
//* .Check In Date   : $Date:   23 Apr 2013 10:38:34  $
//******************************************************************************

#pragma once

#pragma region Includes
#include "SVOResource/resource.h"
#include "SVLibrary/SVFileNameClass.h"
#include "SVUtilityLibrary/StringHelper.h"
#include "SVValueObjectClass.h"
#pragma endregion Includes

class SVFileNameValueObjectClass : public SVValueObjectClass<std::string>
{
	SV_DECLARE_CLASS( SVFileNameValueObjectClass );

public:
	SVFileNameValueObjectClass( LPCTSTR ObjectName );
	SVFileNameValueObjectClass( SVObjectClass* POwner = nullptr, int StringResourceID = IDS_CLASSNAME_SVFILENAMEVALUEOBJECT );
	SVFileNameValueObjectClass( const SVFileNameValueObjectClass& rhs);
	const SVFileNameValueObjectClass& operator = ( const SVFileNameValueObjectClass& rhs );

	virtual ~SVFileNameValueObjectClass();

	virtual HRESULT SetObjectValue( SVObjectAttributeClass* pDataObject ) override;

	virtual void Persist(SVObjectWriter& rWriter) override;

	virtual HRESULT SetValue( const std::string& rValue, int Index = -1 ) override;
	HRESULT SetDefaultValue( const std::string& rValue, bool bResetAll = true ) override;

protected:
	virtual double ValueType2Double(const std::string& rValue) const override { return E_NOTIMPL; };
	virtual _variant_t ValueType2Variant( const std::string& rValue ) const override { return _variant_t( rValue.c_str() ); };
	virtual std::string Variant2ValueType( const _variant_t& rValue ) const override { return SvUl::createStdString( rValue ); };

	//! This just returns the input value as no coversion is required
	//! \param rValue [in] The input string
	//! \returns converted value.
	virtual std::string ConvertString2Type( const std::string& rValue ) const override;

	//! This conversion needs to be overloaded for type std::string
	//! \param rValue [in] Type to convert
	/// \returns the std::string
	virtual std::string ConvertType2String( const std::string& rValue ) const override { return rValue; };

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

	SVFileNameClass m_FileName;
};

