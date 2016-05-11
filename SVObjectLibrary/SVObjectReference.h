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

//Moved to precompiled header: #include <vector>
#include "SVObjectAttributeShim.h"
#include "ObjectInterfaces/SVObjectTypeInfoStruct.h"
#include "SVObjectNameInfo.h"

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
	SVObjectReference( SVObjectClass* pObject, long lArrayIndex, CString strDefaultValue = CString() );
	SVObjectReference( SVObjectClass* pObject, const SVObjectNameInfo& p_rNameInfo );
	SVObjectReference( SVObjectClass* pObject );
	SVObjectReference( GUID guid );
	const SVObjectReference& operator = ( const SVObjectReference& rhs );
	bool operator == ( const SVObjectReference& rhs ) const;

	// we don't want automatic conversion to SVObjectClass* (silent loss of information).
	// use the Object() accessor instead
	//operator SVObjectClass*();
	//operator const SVObjectClass*() const;
	SVObjectClass* operator -> ();
	const SVObjectClass* operator -> () const;

	SVObjectClass* Object() const;
	
	//************************************
	//! True if Index is used 
	//! \returns bool
	//************************************
	bool IsIndexPresent() const;
	
	//************************************
	//! returns the 0 based Index. If Index is not used or the the whole array is referenced -1 is returned.
	//! \returns long
	//************************************
	long ArrayIndex() const;
	CString DefaultValue() const;
	
	//************************************
	//! returns true if the whole array is referenced 
	//! \returns bool
	//************************************
	bool IsEntireArray() const;
	GUID Guid() const;

	void SetEntireArray();
	void SetArrayIndex( long lArrayIndex );

	CString GetName() const;
	CString GetCompleteObjectName() const;
	CString GetCompleteObjectNameToObjectType( LPCSTR lpszCompleteName = nullptr, SVObjectTypeEnum objectTypeToInclude = SVToolSetObjectType ) const;

	CString GetOneBasedName() const;
	CString GetCompleteOneBasedObjectName() const;

	const SVObjectNameInfo& GetObjectNameInfo() const;

	const UINT ObjectAttributesAllowed() const;
	const UINT ObjectAttributesSet() const;
	UINT& ObjectAttributesAllowedRef();
	SVObjectAttributeShim ObjectAttributesSetRef();

	bool operator < ( const SVObjectReference& rhs ) const;

	CString GetOneBasedIndexString() const;
	CString GetZeroBasedIndexString() const;

	//************************************
	//! Increments the array index if the reference points to a single array variable. 
	//! Used to correct the index where zero-based name strings are used. 
	//! (Zero-based name strings are only used in deprecated code)
	//! \returns int the new arrayIndex 
	//************************************
	int IncrementIndex();

protected:	
	const SVString& GetIndex() const;
	static GUID GetObjectGuid( SVObjectClass* );	// for compilation dependency separation
	
	void init();
	SVObjectClass* m_pObject;
	GUID m_Guid;
	SVObjectNameInfo m_NameInfo;

	int m_ArrayIndex; // zero based Array index.   -1 and true for IsArray indicates reference to whole array
	bool m_IsArray; 

};

typedef std::vector<SVObjectReference> SVObjectReferenceVector;

template <class T>
class SVCheckedObjectReference : public SVObjectReference
{
public:
	SVCheckedObjectReference() : SVObjectReference() {}

	SVCheckedObjectReference( SVObjectClass* pObject, long lArrayIndex, CString strDefaultValue = CString() );
	SVCheckedObjectReference( T* pObject, long lArrayIndex, CString strDefaultValue = CString() );

	SVCheckedObjectReference( SVObjectClass* pObject, const SVObjectNameInfo& p_rNameInfo );
	SVCheckedObjectReference( T* pObject, const SVObjectNameInfo& p_rNameInfo );

	SVCheckedObjectReference( SVObjectClass* pObject );
	SVCheckedObjectReference( T* pObject );

	SVCheckedObjectReference( const SVObjectReference& rhs );
	SVCheckedObjectReference( const SVCheckedObjectReference<T>& rhs );

	const SVCheckedObjectReference<T>& operator = ( const SVObjectReference& rhs );
	const SVCheckedObjectReference<T>& operator = ( const SVCheckedObjectReference<T>& rhs );

	T* operator -> ();
	const T* operator -> () const;

	T* Object();
	const T* Object() const;
};

#include "SVObjectReference.inl"

