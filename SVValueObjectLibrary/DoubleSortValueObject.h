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
#include "SVUtilityLibrary\SVString.h"
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
	const ValueObjectSortContainer& getSortContainer() const { return getSortContainer( GetLastSetIndex() ); };
	HRESULT setSortContainer(int iBucket, const ValueObjectSortContainer& sortMap);
	HRESULT setSortContainer(const ValueObjectSortContainer& sortMap) { return setSortContainer( GetLastSetIndex(), sortMap ); };
	virtual HRESULT CopyValue(int iSourceBucket, int iDestBucket) override;
	virtual HRESULT SetValue( const double& rValue, int Bucket, int Index ) override;
	virtual HRESULT GetValue( double& rValue, int Bucket, int Index ) const override;
	virtual HRESULT getValues( std::vector<_variant_t>&  rValues, int Bucket = -1 ) const override;
#pragma endregion Public Methods

#pragma region Protected Methods
protected:
	virtual void CreateBuckets( ) override;
	/// Return the result size. 
	/// ATTENTION: Do not use m_aiResultSize, this must be on ArraySize because otherwise ValidateIndexes can failed even if index valid.
	/// \param iBucket [in]
	/// \returns int Result size
	virtual int getResultSize(int Bucket) const override;

	virtual HRESULT GetArrayValues( std::vector<double>& rValues, int Bucket ) const override;

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
