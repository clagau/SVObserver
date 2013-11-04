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

#if !defined(AFX_SVBSTR_H__58F6DDEB_CCA5_4C77_9C6F_08024EBCD06B__INCLUDED_)
#define AFX_SVBSTR_H__58F6DDEB_CCA5_4C77_9C6F_08024EBCD06B__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000



//##ModelId=3D3C81920244
class SVBStr  
{
public:
// SVBStr () ----------------------------------------------------------------
// Constructs SVBstr class with an empty string. ----------------------------
	//##ModelId=3D3C8192024E
	SVBStr();
	//##ModelId=3D3C81920258
	virtual ~SVBStr();

// All equals operators assume a null terminated source string.
	//##ModelId=3D3C81920262
	const SVBStr& operator=(const SVBStr&  stringSrc);

	//##ModelId=3D3C81920264
// operator=(const BSTR& stringSrc) - Makes copy. ---------------------------
   const SVBStr& operator=(const BSTR&    stringSrc);

// operator=(const WCHAR&		stringSrc) - DO NOT USE. ---------------------- 
// In VC++, BSTR is typedef'ed from LPWSTR, and
//   WCHAR is typedef'ed from DWORD which is typedef'ed from unsigned long.
//   In the end, because the BSTR& equals operator is first, even when you 
//	  pass in a WCHAR pointer, the BSTR function gets called.  For this 
//	  reason, a WCHAR* should not use an equals operator to copy to a SVBStr.  
//   Use CopyFromWChar () instead.
//	const SVBStr& operator=(const WCHAR&		stringSrc);

	//##ModelId=3D3C8192026C
	const SVBStr& operator=(const TCHAR&   stringSrc);
	//##ModelId=3D3C8192026E
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

	//##ModelId=3D3C81920270
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
	//##ModelId=3D3C81920277
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

	//##ModelId=3D3C81920281
   long     GetLength () const;

//	Clear () -----------------------------------------------------------------
// This function while set the bstr string size to zero.  It will also set 
// element 1 to zero.
	long		Clear ();

	long		SetNULL ();

	//##ModelId=3D3C81920283
// CopyFromWChar () ---------------------------------------------------------
// This function assumes a NULL termination to awcpSource.  Although space 
// will be allocated within SVBStr to accommodate the NULL, the size 
// maintained by SVBStr will not include the terminating NULL.  This function
// will not accept embedded NULLs.
//
// return values
//	  2 = input was NULL.
   long     CopyFromWChar (const WCHAR* awcpSource);

	long		CharHexDump (char*	p_csHexDump, 
				  			    long	 p_lSizeOfDest);



	//##ModelId=3D3C8192028C
   long     EnterCriticalSection () const;
	//##ModelId=3D3C8192028E
   long     LeaveCriticalSection () const;

private:

	//##ModelId=3D3C81920295
   long     InitializeCriticalSection();
	//##ModelId=3D3C81920296
   long     DeleteCriticalSection();

	//##ModelId=3D3C8192028A
   long     InternalGetLength () const;

	long		InternalClear ();

	long		InternalSetNULL ();


	long		InternalPreAllocate (unsigned long p_ulNbrOfCharacters);


// DoWeNeedNewBBuffer () ----------------------------------------------------
// alSourceLength - input, This refers to the byte length!!!
	long*		DoWeNeedNewBuffer (long	alSourceLength);

	//##ModelId=3D3C8192029E
   CRITICAL_SECTION svmCriticalSection; 


// svmlAllocationPageSize ---------------------------------------------------
// Number of bytes!!!	
	//##ModelId=3D3C8192029F
   long     svmlAllocationPageSize;

// svmlCurrrentAllocationSize -----------------------------------------------
// Number of bytes!!!
	//##ModelId=3D3C819202A8
   long     svmlCurrentAllocationSize;

// svmlLength ---------------------------------------------------------------
// Number of WCHARs.
	//##ModelId=3D3C819202B2
   long     svmlLength;

	long		m_lNULL;

// This value is only used for outputting a char* copy of the wide character 
// string.  If the contents of this string are changed, the changes will be 
// ignored and lost.
	//##ModelId=3D3C819202BC
   char*    svmcpCharStringBuffer;
	//##ModelId=3D3C819202BD
   long     svmlCharStringAllocationSize;

	//##ModelId=3D3C819202C6
   WCHAR*   svmwcBStringBuffer;
	//##ModelId=3D3C819202D0
// svmlpBStringLength -------------------------------------------------------
// This length is number of bytes.
   long*    svmlpBStringLength;

// svmwcOverflow ------------------------------------------------------------
// Use by the indexing operators only when there is an allocation failure.
	WCHAR		svmwcOverflow;
};

#endif // !defined(AFX_SVBSTR_H__58F6DDEB_CCA5_4C77_9C6F_08024EBCD06B__INCLUDED_)

// ******************************************************************************
// * LOG HISTORY:
// ******************************************************************************
/*
$Log:   N:\PVCSarch65\ProjectFiles\archives\SVObserver_src\SVLibrary\SVBStr.h_v  $
 * 
 *    Rev 1.0   22 Apr 2013 13:31:04   bWalter
 * Project:  SVObserver
 * Change Request (SCR) nbr:  814
 * SCR Title:  Upgrade SVObserver to Compile Using Visual Studio 2010
 * Checked in by:  bWalter;  Ben Walter
 * Change Description:  
 *   Initial check in to SVObserver_src.  (Merged with svo_src label SVO 6.10 Beta 008.)
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.4   06 Oct 2005 10:47:50   jBrown
 * Project:  SVObserver
 * Change Request (SCR) nbr:  505
 * SCR Title:  Encrypt security file
 * Checked in by:  JimAdmin;  James A. Brown
 * Change Description:  
 *   Added CharHexDump () function to assist in debugging encryption problem.
 * 
 * 
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.3   16 Sep 2005 17:00:12   jBrown
 * Project:  SVObserver
 * Change Request (SCR) nbr:  505
 * SCR Title:  Encrypt security file
 * Checked in by:  JimAdmin;  James A. Brown
 * Change Description:  
 *   SVBStr needed to be sygnificantly enhanced to support the XML encryption.
 * 
 * The following functions where modified:
 *  SVBStr::SVBStr ()
 *  SVBStr::operator const BSTR* ()
 *  SVBStr::operator = (const SVBStr&)
 *  SVBStr::operator = (const BSTR&)
 *  SVBStr::operator = (const BString&), and
 *  SVBStr::CopyFromWChar ()
 * 
 * SVBStr::operator BSTR () const has been removed.
 * 
 * The following functions have been added:
 *  SVBStr::WCHAR operator []
 *  SVBStr::WCHAR& operator []
 *  SVBStr::operator const BSTR () const
 *  SVBStr::operator BSTR* ()
 *  SVBStr::operator const BSTR* () const
 *  SVBStr::DoWeNeedNewBuffer ()
 *  SVBStr::SetLength ()
 *  SVBStr::PreAllocate ()
 *  SVBStr::InternalPreAllocate ()
 *  SVBStr::Clear ()
 *  SVBStr::InternalClear ()
 *  SVBStr::SetNULL (), and
 *  SVBStr::InternalSetNULL ()
 * 
 * 
 * 
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.2   22 Oct 2004 09:23:20   jBrown
 * Project:  SVObserver
 * Change Request (SCR) nbr:  375
 * SCR Title:  Remodel XML usage for configuration storage and SIAC transfers.
 * Checked in by:  JimAdmin;  James A. Brown
 * Change Description:  
 *   Refined the implementation of SVBStr to make it work within the new XML implementation.
 * 
 * Changed 
 *    long     CopyFromWChar (WCHAR* awcpSource)
 * to
 *    long     CopyFromWChar (const WCHAR* awcpSource).
 * 
 * 
 * 
 * 
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.1   10 Jan 2003 13:31:48   cschmittinger
 * Project:  SVObserver
 * Change Request (SCR) nbr:  318
 * SCR Title:  Image Transfer Compression
 * Checked in by:  cSchmittinger;  Carl Schmittinger
 * Change Description:  
 *   Code was moved to the SVLibrary Project
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.0   18 Nov 2002 10:17:06   ryoho
 * Project:  SVObserver
 * Change Request (SCR) nbr:  226
 * SCR Title:  Monochrome SVIM configuration compatibility between ViperQUAD and ViperDUAL
 * Checked in by:  rYoho;  Rob Yoho
 * Change Description:  
 *   first version of file
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
*/