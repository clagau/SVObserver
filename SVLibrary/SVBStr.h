// ******************************************************************************
// * COPYRIGHT (c) 2002 by SVResearch, Harrisburg
// * All Rights Reserved
// ******************************************************************************
// * .Module Name     : SVBStr
// * .File Name       : $Workfile:   SVBStr.h  $
// * ----------------------------------------------------------------------------
// * .Current Version : $Revision:   1.0  $
// * .Check In Date   : $Date:   22 Apr 2013 13:31:04  $
// ******************************************************************************

#pragma once


class SVBStr  
{
public:
// SVBStr () ----------------------------------------------------------------
// Constructs SVBstr class with an empty string. ----------------------------
	SVBStr();
	virtual ~SVBStr();

// All equals operators assume a null terminated source string.
	const SVBStr& operator=(const SVBStr&  stringSrc);

// operator=(const BSTR& stringSrc) - Makes copy. ---------------------------
   const SVBStr& operator=(const BSTR&    stringSrc);

// operator=(const WCHAR&		stringSrc) - DO NOT USE. ---------------------- 
// In VC++, BSTR is typedef'ed from LPWSTR, and
//   WCHAR is typedef'ed from DWORD which is typedef'ed from unsigned long.
//   In the end, because the BSTR& equals operator is first, even when you 
//	  pass in a WCHAR pointer, the BSTR function gets called.  For this 
//	  reason, a WCHAR* should not use an equals operator to copy to a SVBStr.  
//   Use CopyFromWChar () instead.

	const SVBStr& operator=(const TCHAR&   stringSrc);
	const SVBStr& operator=(const CString&   stringSrc);

// operator [] --------------------------------------------------------------
// p_nIndex - input, the index refers to a wide character position and not a 
//   byte position.
	WCHAR	 operator[](int p_nIndex ) const;

// operator [] --------------------------------------------------------------
// p_nIndex - input, the index refers to a wide character position and not a 
//   byte position.
//
// This function should not be used when the instance exists across threads.
// It is NOT thread safe.  The array being referenced as the return value, 
// could be destroyed by another thread before the assignment takes place 
//	(for a[x] = b).
	WCHAR& operator[](int p_nIndex );

	operator const BSTR() const;
	operator       BSTR();

// This will return the wide char pointer. The returned value should be 
//	treated as a const, and should NOT be allowed to be destroyed or 
// reallocated as a BSTR.
	operator       BSTR*();
	operator const BSTR*() const;


// This allocates a char array, copies the WCHAR string into it, and then
// returns a pointer to the char array.  The char array is reused on 
// subsequent calls, and is eventually deleted during the destruction of 
// this class. --------------------------------------------------------------
	operator const char*() const;

// PreAllocate () -----------------------------------------------------------
//  This will allow to manually affect the current allocated buffer size.  
//  If there are currently contents in the SVBStr, this function will copy 
//  them to the new buffer.  For large strings where it will grow a little at
//  a time, 
// Parameters
//  p_ulNbrOfCharacters - input, This indicates the number of characters to 
//    preallocate memory for.
	long		PreAllocate (unsigned long p_ulNbrOfCharacters);


// SetLength () -------------------------------------------------------------
// CAUTION - Know what you're doing before using this function.
// p_lLength - input, this sets the number of WCHARs.
	long		SetLength (long	p_lLength);

   long     GetLength () const;

//	Clear () -----------------------------------------------------------------
// This function while set the bstr string size to zero.  It will also set 
// element 1 to zero.
	long		Clear ();

	long		SetNULL ();

// CopyFromWChar () ---------------------------------------------------------
// This function assumes a NULL termination to awcpSource.  Although space 
// will be allocated within SVBStr to accommodate the NULL, the size 
// maintained by SVBStr will not include the terminating NULL.  This function
// will not accept embedded NULLs.
//
// return values
//	  2 = input was NULL.
	long CopyFromWChar (const WCHAR* awcpSource);

   long     EnterCriticalSection () const;
   long     LeaveCriticalSection () const;

private:
	long    InitializeCriticalSection();
	long    DeleteCriticalSection();
	long    InternalGetLength () const;
	long	InternalClear ();

	long	InternalSetNULL ();
	long	InternalPreAllocate (unsigned long p_ulNbrOfCharacters);

// DoWeNeedNewBBuffer () ----------------------------------------------------
// alSourceLength - input, This refers to the byte length!!!
	long*		DoWeNeedNewBuffer (long	alSourceLength);

   CRITICAL_SECTION svmCriticalSection; 

// svmlAllocationPageSize ---------------------------------------------------
// Number of bytes!!!	
   static const long svmlAllocationPageSize=4000; //Arvid 2015-01-08 made this a static const since it is always set to 4000 anyway

// svmlCurrrentAllocationSize -----------------------------------------------
// Number of bytes!!!
   long     svmlCurrentAllocationSize;

// svmlLength ---------------------------------------------------------------
// Number of WCHARs.
   long     svmlLength;
	long		m_lNULL;

// This value is only used for outputting a char* copy of the wide character 
// string.  If the contents of this string are changed, the changes will be 
// ignored and lost.
   char*    svmcpCharStringBuffer;
   long     svmlCharStringAllocationSize;
   WCHAR*   svmwcBStringBuffer;

// svmlpBStringLength -------------------------------------------------------
// This length is number of bytes.
   long*    svmlpBStringLength;

// svmwcOverflow ------------------------------------------------------------
// Use by the indexing operators only when there is an allocation failure.
	WCHAR		svmwcOverflow;
};

