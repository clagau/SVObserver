// ******************************************************************************
// * COPYRIGHT (c) 2000 by SVResearch, Harrisburg
// * All Rights Reserved
// ******************************************************************************
// * .Module Name     : CVariantObj
// * .File Name       : $Workfile:   VariantObj.h  $
// * ----------------------------------------------------------------------------
// * .Current Version : $Revision:   1.0  $
// * .Check In Date   : $Date:   18 Apr 2013 16:43:22  $
// ******************************************************************************

/***
* CVariantObj.h - Native C++ compiler COM support - main definitions header
*
*	Copyright(C) 1996 - 1997 Microsoft Corporation
*	All rights reserved.
*
****/

#if _MSC_VER > 1000
#pragma once
#endif

#if !defined(_INC_VARIANTOBJ)
#define _INC_VARIANTOBJ

#include <comdef.h>

#pragma warning(push)
#pragma warning(disable:4290)
#pragma warning(disable:4310)

//////////////////////////////////////////////////////////////////////////////
//
// Wrapper class for VARIANT
//
//////////////////////////////////////////////////////////////////////////////

/*
 * VARENUM usage key,
 *
 * *[V] - may appear in a VARIANT
 * *[T] - may appear in a TYPEDESC
 * *[P] - may appear in an OLE property set
 * *[S] - may appear in a Safe Array
 * *[C] - supported by class CVariantObj
 *
 *
 *  VT_EMPTY					[V]	[P]			nothing
 *  VT_NULL						[V]	[P]			SQL style Null
 *  VT_I2						[V][T][P][S][C]	2 byte signed int
 *  VT_I4						[V][T][P][S][C]	4 byte signed int
 *  VT_R4						[V][T][P][S][C]	4 byte real
 *  VT_R8						[V][T][P][S][C]	8 byte real
 *  VT_CY						[V][T][P][S][C]	currency
 *  VT_DATE						[V][T][P][S][C]	date
 *  VT_BSTR						[V][T][P][S][C]	OLE Automation string
 *  VT_DISPATCH				[V][T][P][S][C]	IDispatch *
 *  VT_ERROR					[V][T]	[S][C]	SCODE
 *  VT_BOOL						[V][T][P][S][C]	True=-1, False = 0
 *  VT_VARIANT					[V][T][P][S]		VARIANT *
 *  VT_UNKNOWN					[V][T]	[S][C]	IUnknown *
 *  VT_DECIMAL					[V][T]	[S][C]	16 byte fixed point
 *  VT_I1							[T]				signed char
 *  VT_UI1						[V][T][P][S][C]	unsigned char
 *  VT_UI2						[V][T][P]			unsigned short
 *  VT_UI4						[V][T][P]			unsigned long
 *  VT_I8							[T][P]			signed 64 - bit int
 *  VT_UI8							[T][P]			unsigned 64 - bit int
 *  VT_INT						[V][T]				signed machine int
 *  VT_UINT						[V][T]				unsigned machine int
 *  VT_VOID							[T]				C style void
 *  VT_HRESULT						[T]				Standard return type
 *  VT_PTR							[T]		[C]	pointer type
 *  VT_SAFEARRAY					[T]				(use VT_ARRAY in VARIANT)
 *  VT_CARRAY						[T]				C style array
 *  VT_USERDEFINED				[T]		[C]	user defined type
 *  VT_LPSTR					[V][T][P]	[C]	null terminated string
 *  VT_LPWSTR					[V][T][P]	[C]	wide null terminated string
 *  VT_FILETIME						[P]			FILETIME
 *  VT_BLOB								[P]			Length prefixed bytes
 *  VT_STREAM							[P]			Name of the stream follows
 *  VT_STORAGE							[P]			Name of the storage follows
 *  VT_STREAMED_OBJECT				[P]			Stream contains an object
 *  VT_STORED_OBJECT					[P]			Storage contains an object
 *  VT_BLOB_OBJECT					[P]			Blob contains an object
 *  VT_CF								[P]			Clipboard format
 *  VT_CLSID							[P]			A Class ID
 *  VT_VECTOR							[P]			simple counted array
 *  VT_ARRAY					[V]					SAFEARRAY*
 *  VT_BYREF					[V]			[C]	void* for local use
 */

class CVariantObj : public ::tagVARIANT 
{
	// Constructors
public:
	CVariantObj() throw();
	
	CVariantObj(const VARIANT& varSrc) throw(_com_error);
	CVariantObj(LPCVARIANT pSrc) throw(_com_error);
	
	CVariantObj(LPCTSTR lpszSrc, VARTYPE vtSrc) throw(_com_error);		// Creates a VT_BSTR
	CVariantObj(CString& strSrc) throw(_com_error);							// Creates a VT_BSTR

	CVariantObj(bool bSrc) throw();												// Creates a VT_BOOL
	CVariantObj(BYTE bSrc) throw();												// Creates a VT_UI1
	CVariantObj(short sSrc, VARTYPE vtSrc=VT_I2) throw(_com_error);	// Creates a VT_I2
	CVariantObj(USHORT sSrc) throw();											// Creates a VT_UI2
	CVariantObj(long lSrc, VARTYPE vtSrc=VT_I4) throw(_com_error);		// Creates a VT_I4
	CVariantObj(ULONG ulSrc) throw();											// Creates a VT_UI4
	CVariantObj(int intSrc) throw();												// Creates a VT_INT
	CVariantObj(UINT uintSrc) throw();											// Creates a VT_UINT
	CVariantObj(float fltSrc) throw();											// Creates a VT_R4
	CVariantObj(double dblSrc) throw();											// Creates a VT_R8
	
	// Destructor
	//
	~CVariantObj() throw(_com_error);
	
	// Extractors
	//
	operator bool() const throw(_com_error);			// Extracts a bool from a VT_BOOL
	operator BYTE() const throw(_com_error);			// Extracts a BTYE (unsigned char) from a VT_UI1
	operator short() const throw(_com_error);			// Extracts a short from a VT_I2
	operator USHORT() const throw(_com_error);
	operator long() const throw(_com_error);			// Extracts a long from a VT_I4
	operator ULONG() const throw(_com_error);
	operator int() const throw(_com_error);
	operator UINT() const throw(_com_error);
	operator float() const throw(_com_error);			// Extracts a float from a VT_R4
	operator double() const throw(_com_error);		// Extracts a double from a VT_R8
	operator _bstr_t() const throw(_com_error);		// Extracts a _bstr_t from a VT_BSTR


	// Assignment operations
	//
	const CVariantObj& operator=(const VARIANT& varSrc) throw(_com_error);
	const CVariantObj& operator=(LPCVARIANT pSrc) throw(_com_error);
	
	const CVariantObj& operator=(const LPCTSTR lpszSrc);
	const CVariantObj& operator=(const CString& strSrc);

	const CVariantObj& operator=(bool bSrc) throw(_com_error);			// Assign a VT_BOOL
	const CVariantObj& operator=(BYTE bSrc) throw(_com_error);			// Assign a VT_UI1
	const CVariantObj& operator=(short iSrc) throw(_com_error);			// Assign a VT_I2
	const CVariantObj& operator=(USHORT uiSrc) throw(_com_error);		// Assign a VT_I2
	const CVariantObj& operator=(long lSrc) throw(_com_error);			// Assign a VT_I4
	const CVariantObj& operator=(ULONG ulSrc) throw(_com_error);		// Assign a VT_UI4
	const CVariantObj& operator=(int intSrc) throw(_com_error);			// Assign a VT_INT
	const CVariantObj& operator=(UINT uintSrc) throw(_com_error);		// Assign a VT_UINT
	const CVariantObj& operator=(float fltSrc) throw(_com_error);		// Assign a VT_R4
	const CVariantObj& operator=(double dblSrc) throw(_com_error);		// Assign a VT_R8
	
	// Comparison operations
	//
	bool operator==(const VARIANT& varSrc) const throw(_com_error);
	bool operator==(LPCVARIANT pSrc) const throw(_com_error);
	
	bool operator!=(const VARIANT& varSrc) const throw(_com_error);
	bool operator!=(LPCVARIANT pSrc) const throw(_com_error);
	
	bool operator<(const VARIANT& varSrc) const throw(_com_error);
	bool operator<(LPCVARIANT pSrc) const throw(_com_error);
	
	bool operator>(const VARIANT& varSrc) const throw(_com_error);
	bool operator>(LPCVARIANT pSrc) const throw(_com_error);
	
	bool operator<=(const VARIANT& varSrc) const throw(_com_error);
	bool operator<=(LPCVARIANT pSrc) const throw(_com_error);
	
	bool operator>=(const VARIANT& varSrc) const throw(_com_error);
	bool operator>=(LPCVARIANT pSrc) const throw(_com_error);
	
	// Low-level operations
	//
	void Clear() throw(_com_error);
	void Copy(const VARIANT& varSrc) throw(_com_error);
	void Copy(LPCVARIANT pSrc) throw(_com_error);
	void CopyInd(const VARIANT& varSrc) throw(_com_error);
	void CopyInd(LPCVARIANT pSrc) throw(_com_error);
	
	void Attach(VARIANT& varSrc) throw();
	void AttachRef(bool* pbSrc) throw();						// Creates a VT_BOOLREF
	void AttachRef(BYTE* pbSrc) throw();						// Creates a VT_UI1REF
	void AttachRef(short* piSrc) throw();						// Creates a VT_I2REF
	void AttachRef(USHORT* puiSrc) throw();					// Creates a VT_UI2REF
	void AttachRef(long* plSrc) throw();						// Creates a VT_I4REF
	void AttachRef(ULONG* pulSrc) throw();						// Creates a VT_UI4REF
	void AttachRef(int* pintSrc) throw();						// Creates a VT_INTREF
	void AttachRef(UINT* pintSrc) throw();						// Creates a VT_UINTREF
	void AttachRef(float* pfltSrc) throw();					// Creates a VT_R4REF
	void AttachRef(double* pdblSrc) throw();					// Creates a VT_R8REF
	void AttachRef(_bstr_t* bstrSrc) throw();

	VARIANT Detach() throw(_com_error);
	
	void ChangeType(VARTYPE vartype, const CVariantObj* pSrc = NULL, USHORT wFlags = 0) throw(_com_error);
	
	void SetString(LPCTSTR lpszSrc, VARTYPE vtSrc) throw(_com_error); // used to set ANSI string
	void toString(CString& strSrc) throw(_com_error);
	void fromString(LPCTSTR lpszSrc) throw(_com_error);

};

#pragma warning(pop)

#endif  /* _INC_VARIANT */

// ******************************************************************************
// * LOG HISTORY:
// ******************************************************************************
/*
$Log:   N:\PVCSarch65\ProjectFiles\archives\SVObserver_src\PropertyTree\VariantObj.h_v  $
 * 
 *    Rev 1.0   18 Apr 2013 16:43:22   bWalter
 * Project:  SVObserver
 * Change Request (SCR) nbr:  814
 * SCR Title:  Upgrade SVObserver to Compile Using Visual Studio 2010
 * Checked in by:  bWalter;  Ben Walter
 * Change Description:  
 *   Initial check in to SVObserver_src.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.0   15 Nov 2002 15:42:28   ryoho
 * Project:  SVObserver
 * Change Request (SCR) nbr:  226
 * SCR Title:  Monochrome SVIM configuration compatibility between ViperQUAD and ViperDUAL
 * Checked in by:  rYoho;  Rob Yoho
 * Change Description:  
 *   first version of the file
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
*/