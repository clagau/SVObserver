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

#include <vector>
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
	CString GetCompleteObjectNameToObjectType( LPCSTR lpszCompleteName = NULL, SVObjectTypeEnum objectTypeToInclude = SVToolSetObjectType ) const;

	CString GetOneBasedName() const;
	CString GetCompleteOneBasedObjectName() const;
	CString GetCompleteOneBasedObjectNameToObjectType( LPCSTR lpszCompleteName = NULL, SVObjectTypeEnum objectTypeToInclude = SVToolSetObjectType ) const;

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

//******************************************************************************
//* LOG HISTORY:
//******************************************************************************
/*
$Log:   N:\PVCSarch65\ProjectFiles\archives\SVObserver_SRC\SVObjectLibrary\SVObjectReference.h_v  $
 * 
 *    Rev 1.1   07 Jan 2015 16:06:02   bwalter
 * Project:  SVObserver
 * Change Request (SCR) nbr:  980
 * SCR Title:  Add Non-Inspection Objects to the Result View
 * Checked in by:  mEichengruen;  Marcus Eichengruen
 * Change Description:  
 *   Changed method GetIndex to return const reference.  Changed to use "pragma once".  Added includes region.  Removed dead code.  Cleaned up spacing.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.0   22 Apr 2013 17:00:20   bWalter
 * Project:  SVObserver
 * Change Request (SCR) nbr:  814
 * SCR Title:  Upgrade SVObserver to Compile Using Visual Studio 2010
 * Checked in by:  bWalter;  Ben Walter
 * Change Description:  
 *   Initial check in to SVObserver_src.  (Merged with svo_src label SVO 6.10 Beta 008.)
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.12   30 Nov 2012 08:22:28   tbair
 * Project:  SVObserver
 * Change Request (SCR) nbr:  800
 * SCR Title:  Fix Blob Array Result index string
 * Checked in by:  tBair;  Tom Bair
 * Change Description:  
 *   Added get one based index string functions. Renamed old functions to get zero based index string for clarity.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.11   26 Jul 2012 11:07:32   tbair
 * Project:  SVObserver
 * Change Request (SCR) nbr:  769
 * SCR Title:  Fix Problems and Crashes with Inspection Document Display Updates
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Modified ArrayIndex() to translate from one based indexes. Also added a default bool parameter to allow a zero based translation.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.10   18 Jun 2012 17:25:18   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  744
 * SCR Title:  Add Shared Memory and Socket Functionality for Run Page Web Server
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Separated dotted name parsing and management to its own class.  This allows for the use of this functionality without have to involve the Reference Class.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.0   18 Jun 2012 17:20:12   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  744
 * SCR Title:  Add Shared Memory and Socket Functionality for Run Page Web Server
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Separated dotted name parsing and management to its own class.  This allows for the use of this functionality without have to involve the Reference Class.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.9   28 Oct 2010 13:35:54   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  704
 * SCR Title:  Upgrade SVObserver Version for 5.33 Release
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated source code to remove duplicate definition of HRESULT.  The SVHRESULT definition will be removed in place of the Microsoft defined HRESULT.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.8   28 May 2010 14:08:18   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  694
 * SCR Title:  Upgrade SVObserver version for Chromis release
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated source code to fix issues with includes and make them consistant.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.7   30 Aug 2005 14:10:38   ebeyeler
 * Project:  SVObserver
 * Change Request (SCR) nbr:  464
 * SCR Title:  Add array indexing for value objects
 * Checked in by:  eBeyeler;  Eric Beyeler
 * Change Description:  
 *   optimized construction and copying
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.6   17 Aug 2005 10:19:14   ebeyeler
 * Project:  SVObserver
 * Change Request (SCR) nbr:  500
 * SCR Title:  Reduce delay when adjusting tool parameters with a large toolset
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   added SVGuid.h
 * added operator <
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.5   27 Jul 2005 15:47:50   ebeyeler
 * Project:  SVObserver
 * Change Request (SCR) nbr:  464
 * SCR Title:  Add array indexing for value objects
 * Checked in by:  eBeyeler;  Eric Beyeler
 * Change Description:  
 *   added Guid information so we can store a reference to an object that doesn't exist yet (only store the guid)
 * 
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.4   14 Jul 2005 11:46:54   ebeyeler
 * Project:  SVObserver
 * Change Request (SCR) nbr:  464
 * SCR Title:  Add array indexing for value objects
 * Checked in by:  eBeyeler;  Eric Beyeler
 * Change Description:  
 *   added operator ==
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.3   08 Jul 2005 13:21:16   ebeyeler
 * Project:  SVObserver
 * Change Request (SCR) nbr:  464
 * SCR Title:  Add array indexing for value objects
 * Checked in by:  eBeyeler;  Eric Beyeler
 * Change Description:  
 *   added parsing for entire array syntax []
 * added constructor taking SVObjectNameParseInfoStruct
 * 
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.2   30 Jun 2005 15:11:52   ebeyeler
 * Project:  SVObserver
 * Change Request (SCR) nbr:  464
 * SCR Title:  Add array indexing for value objects
 * Checked in by:  eBeyeler;  Eric Beyeler
 * Change Description:  
 *   updated to use SVObjectReference
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.1   29 Jun 2005 09:31:16   ebeyeler
 * Project:  SVObserver
 * Change Request (SCR) nbr:  464
 * SCR Title:  Add array indexing for value objects
 * Checked in by:  eBeyeler;  Eric Beyeler
 * Change Description:  
 *   added SVCheckedObjectReference template
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.0   27 Jun 2005 14:40:56   ebeyeler
 * Project:  SVObserver
 * Change Request (SCR) nbr:  464
 * SCR Title:  Add array indexing for value objects
 * Checked in by:  eBeyeler;  Eric Beyeler
 * Change Description:  
 *   first iteration
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
*/
