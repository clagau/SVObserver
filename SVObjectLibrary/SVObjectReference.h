//******************************************************************************
//* COPYRIGHT (c) 2005 by SVResearch, Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVObjectReference
//* .File Name       : $Workfile:   SVObjectReference.h  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.1  $
//* .Check In Date   : $Date:   07 Jan 2015 16:06:02  $
//******************************************************************************

#pragma once

#pragma region Includes
//Moved to precompiled header: #include <vector>
#include "ObjectInterfaces/SVObjectTypeInfoStruct.h"
#include "ObjectInterfaces/IObjectClass.h"
#include "ObjectInterfaces/IValueObject.h"
#include "SVUtilityLibrary/SVString.h"
#include "SVObjectNameInfo.h"
//! Do not include SVObjectClass.h this causes circular includes with SVOutObjectInfoStruct and SVInObjectInfoStruct
////@TODO[GRA][7.50][13.02.2017] The circular include dependencies need to be fixed
#pragma endregion Includes

class SVObjectClass;

// This class is made to allow seamless handling of array value objects.
// this class couples the object pointer with an array index.
// array value objects can be specified like:
//  Inspection_1.Tool Set.Window Tool.Blob Analyzer.Area[ 2 ]
//   where 2 is the 1-based index
// or
//  Inspection_1.Tool Set.Window Tool.Blob Analyzer.Area[2{-1}]
//   where {-1} specifies the default value
// whitespace inside the [] is ignored

class SVObjectReference
{
public:
	SVObjectReference();
	SVObjectReference( const SVObjectReference& rhs );
	SVObjectReference( SVObjectClass* pObject, long lArrayIndex, SVString strDefaultValue = SVString() );
	SVObjectReference( SVObjectClass* pObject, const SVObjectNameInfo& p_rNameInfo );
	SVObjectReference( SVObjectClass* pObject );
	SVObjectReference( GUID guid );
	/// This constructor create an object depending of a GUID and if required an index. 
	/// \param guidAndIndexString [in] A string with a GUID and if required an index (e.g.{7407F882-3AA5-48E2-B2E9-542538CB1650}[1])
	SVObjectReference(const SVString& guidAndIndexString);
	const SVObjectReference& operator = ( const SVObjectReference& rhs );
	bool operator == ( const SVObjectReference& rhs ) const;

	SVObjectClass* getObject() const;

	SvOi::IValueObject* getValueObject() const;
	
	//************************************
	//! returns the 0 based Index. If Index is not used or the the whole array is referenced -1 is returned.
	//! \returns long
	//************************************
	long ArrayIndex() const;

	//************************************
	//! returns the 0 based Index. If Index is -1 then 0 used
	//! \returns long
	//************************************
	long getValidArrayIndex() const;

	SVString DefaultValue() const;
	
	//************************************
	//! returns true if value object is an array
	//! \returns bool
	//************************************
	bool isArray() const;

	//************************************
	//! returns true if the whole array is referenced 
	//! \returns bool
	//************************************
	bool isEntireArray() const;
	GUID Guid() const;

	void SetEntireArray();
	void SetArrayIndex( long lArrayIndex );

	SVString GetName() const;
	SVString GetCompleteName() const;
	SVString GetCompleteObjectNameToObjectType( LPCTSTR lpszCompleteName = nullptr, SVObjectTypeEnum objectTypeToInclude = SVToolSetObjectType ) const;

	SVString GetOneBasedName() const;
	SVString GetCompleteOneBasedObjectName() const;

	/// Get a string first with the GUID and if array and index this attached. (e.g.{7407F882-3AA5-48E2-B2E9-542538CB1650}[1])
	/// \returns SVString
	SVString GetGuidAndIndexOneBased() const;

	const SVObjectNameInfo& GetObjectNameInfo() const;

	UINT ObjectAttributesAllowed() const;
	UINT ObjectAttributesSet() const;
	UINT SetObjectAttributesAllowed( UINT Attributes, SvOi::SetAttributeType Type );
	UINT SetObjectAttributesSet( UINT Attributes, SvOi::SetAttributeType Type );

	bool operator < ( const SVObjectReference& rhs ) const;

	SVString GetOneBasedIndexString() const;
	SVString GetZeroBasedIndexString() const;

	/// Get the value of object depend of the index. If object is not a value object, it will return an error.
	/// \param rValue [out] The return value.
	/// \param Bucket [in] The bucket. Default = -1
	/// \returns HRESULT
	HRESULT getValue(_variant_t& rValue, int Bucket = -1) const;

	//************************************
	//! Increments the array index if the reference points to a single array variable. 
	//! Used to correct the index where zero-based name strings are used. 
	//! (Zero-based name strings are only used in deprecated code)
	//! \returns long the new arrayIndex 
	//************************************
	long IncrementIndex();

protected:	
	const SVString& GetIndex() const;
	
	void init();
	SVObjectClass* m_pObject;
	mutable SvOi::IValueObject* m_pValueObject;
	GUID m_Guid;
	SVObjectNameInfo m_NameInfo;
	long m_ArrayIndex; // zero based Array index.   -1 and true for isArray indicates reference to whole array
};

typedef std::vector<SVObjectReference> SVObjectReferenceVector;

#include "SVObjectReference.inl"

