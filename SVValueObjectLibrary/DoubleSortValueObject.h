//*****************************************************************************
/// \copyright (c) 2016,2016 by Seidenader Maschinenbau GmbH
/// \file DoubleSortValueObject.h
/// All Rights Reserved 
//*****************************************************************************
/// This class is a doubleValueObject, but the order of the array will be defined by a map.
//******************************************************************************
#pragma once

#pragma region Includes
//Moved to precompiled header: #include <boost/circular_buffer.hpp>
#include "SVObjectLibrary\SVObjectLibrary.h"
#include "SVDoubleValueObjectClass.h"
#pragma endregion Includes

#pragma region Declarations
typedef boost::circular_buffer<int> ValueObjectSortContainer;
#pragma endregion Declarations

class DoubleSortValueObject : public SVDoubleValueObjectClass
{
	SV_DECLARE_CLASS( DoubleSortValueObject );
#pragma region Constructor
public:
	DoubleSortValueObject( LPCTSTR ObjectName );
	DoubleSortValueObject( SVObjectClass* POwner = nullptr , int StringResourceID = IDS_CLASSNAME_SVDOUBLEVALUEOBJECT );
	DoubleSortValueObject( const DoubleSortValueObject& rhs );
	const DoubleSortValueObject& operator = (const DoubleSortValueObject& rhs);

	virtual ~DoubleSortValueObject();
#pragma endregion Constructor

#pragma region Public Methods
public:
	const ValueObjectSortContainer& getSortContainer(int iBucket) const;
	const ValueObjectSortContainer& getSortContainer() const { return getSortContainer(m_iLastSetIndex); };
	HRESULT setSortContainer(int iBucket, const ValueObjectSortContainer& sortMap);
	HRESULT setSortContainer(const ValueObjectSortContainer& sortMap) { return setSortContainer(m_iLastSetIndex, sortMap); };
	virtual HRESULT CopyValue(int iSourceBucket, int iDestBucket) override;
#pragma endregion Public Methods

#pragma region Protected Methods
protected:
	virtual HRESULT CreateBuckets( ) override;
	/// Return the result size. 
	/// ATTENTION: Do not use m_aiResultSize, this must be on ArraySize because otherwise ValidateIndexes can failed even if index valid.
	/// \param iBucket [in]
	/// \returns int Result size
	virtual int GetResultSize(int iBucket) const override { return static_cast<int>(getSortContainer(iBucket).size()); };

	virtual HRESULT SetValueAt( int iBucket, int iIndex, const CString& value ) override;
	virtual HRESULT SetValueAt( int iBucket, int iIndex, const VARIANT& rValue ) override;
	virtual HRESULT SetValueAt( int iBucket, int iIndex, int value ) override;
	virtual HRESULT SetValueAt( int iBucket, int iIndex, long value ) override;
	virtual HRESULT SetValueAt( int iBucket, int iIndex, double value ) override;

	virtual HRESULT GetValueAt( int iBucket, int iIndex, double& rValue ) const override;
	virtual HRESULT GetValueAt( int iBucket, int iIndex, long& rValue ) const override;
	virtual HRESULT GetValueAt( int iBucket, int iIndex, DWORD& rValue ) const override;
	virtual HRESULT GetValueAt( int iBucket, int iIndex, CString& rValue ) const override;
	virtual HRESULT GetValueAt( int iBucket, int iIndex, VARIANT& rValue ) const override;

	virtual HRESULT GetArrayValues(int iBucket, std::vector<double>& raValues) const override;
	virtual HRESULT GetArrayValuesAsVariant( int iBucket, VARIANT&  rValue ) const override;
	virtual HRESULT GetArrayValuesAsVariantVector( int iBucket, std::vector< _variant_t >&  rValues ) const override;

	virtual void ValidateValue( int iBucket, int iIndex, const SVString& rValue ) const override;
#pragma endregion Protected Methods

#pragma region Private Methods
private:
	void LocalInitialize();
#pragma endregion Private Methods

#pragma region Member Variables
private:
	std::vector<ValueObjectSortContainer> m_sortContainerArray;
#pragma endregion Member Variables
};

typedef SVSharedPtr<DoubleSortValueObject> DoubleSortValuePtr;
