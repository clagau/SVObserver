// ******************************************************************************
// * COPYRIGHT (c) 2003 by SVResearch, Harrisburg
// * All Rights Reserved
// ******************************************************************************
// * .Module Name     : SVOMFCLibraryGlobals
// * .File Name       : $Workfile:   SVOMFCLibraryGlobals.h  $
// * ----------------------------------------------------------------------------
// * .Current Version : $Revision:   1.2  $
// * .Check In Date   : $Date:   17 Jul 2014 17:31:08  $
// ******************************************************************************

#pragma once

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include <float.h>
#include <math.h>

#define SVHANDLE_DEFINED
typedef unsigned long SVHANDLE;

enum
{
	BIT0  = 0x00000001,
	BIT1  = 0x00000002,
	BIT2  = 0x00000004,
	BIT3  = 0x00000008,
	BIT4  = 0x00000010,
	BIT5  = 0x00000020,
	BIT6  = 0x00000040,
	BIT7  = 0x00000080,
	BIT8  = 0x00000100,
	BIT9  = 0x00000200,
	BIT10 = 0x00000400,
	BIT11 = 0x00000800,
	BIT12 = 0x00001000,
	BIT13 = 0x00002000,
	BIT14 = 0x00004000,
	BIT15 = 0x00008000,
	BIT16 = 0x00010000,
	BIT17 = 0x00020000,
	BIT18 = 0x00040000,
	BIT19 = 0x00080000,
	BIT20 = 0x00100000,
	BIT21 = 0x00200000,
	BIT22 = 0x00400000,
	BIT23 = 0x00800000,
	BIT24 = 0x01000000,
	BIT25 = 0x02000000,
	BIT26 = 0x04000000,
	BIT27 = 0x08000000,
	BIT28 = 0x10000000,
	BIT29 = 0x20000000,
	BIT30 = 0x40000000,
	BIT31 = 0x80000000,
};

const HRESULT SV_FALSE = S_FALSE | BIT31;





inline CString SVLoadString(UINT uiResourceIDS) {CString s; s.LoadString(uiResourceIDS);return s;}

inline CString AsString(short n) {CString s;s.Format("%d", n); return s;}
inline CString AsString(int n) {CString s;s.Format("%d", n); return s;}
inline CString AsString(long n) {CString s;s.Format("%d", n); return s;}
inline CString AsString(unsigned short n) {CString s;s.Format("%u", n); return s;}
inline CString AsString(unsigned int n) {CString s;s.Format("%u", n); return s;}
inline CString AsString(unsigned long n) {CString s;s.Format("%u", n); return s;}
inline CString AsString(double d) {CString s;s.Format("%f", d); return s;}

inline bool ApproxEqual(float f1, float f2)
{
	return (fabs(f1 - f2) < FLT_EPSILON);
}

inline bool ApproxEqual(double d1, double d2)
{
	return (fabs(d1 - d2) < DBL_EPSILON);
}

inline long LongRand()
{
	// rand returns number {0,32767}. Let's create a 30-bit value from two random numbers
	long l = rand() << 15;
	l |= rand();
	return l;
}

inline DWORD ROR(DWORD& rdwValue, BYTE byNumBitsToShift = 1)
{
	DWORD dwMask = (1 << byNumBitsToShift) - 1;
	DWORD dwMoveBits = rdwValue & dwMask;
	rdwValue >>= byNumBitsToShift;
	dwMoveBits <<= (8*sizeof(DWORD) - byNumBitsToShift);
	rdwValue |= dwMoveBits;
	return rdwValue;
}

inline WORD ROR(WORD& rwValue, BYTE byNumBitsToShift = 1)
{
	WORD wMask = (1 << byNumBitsToShift) - 1;
	WORD wMoveBits = rwValue & wMask;
	rwValue >>= byNumBitsToShift;
	wMoveBits <<= (8*sizeof(WORD) - byNumBitsToShift);
	rwValue |= wMoveBits;
	return rwValue;
}

inline BYTE ROR(BYTE& rbyValue, BYTE byNumBitsToShift = 1)
{
	BYTE byMask = (1 << byNumBitsToShift) - 1;
	BYTE byMoveBits = rbyValue & byMask;
	rbyValue >>= byNumBitsToShift;
	byMoveBits <<= (8 - byNumBitsToShift);
	rbyValue |= byMoveBits;
	return rbyValue;
}

inline DWORD ROL(DWORD& rdwValue, BYTE byNumBitsToShift = 1)
{
	DWORD dwMask = (1 << byNumBitsToShift) - 1;
	dwMask <<= (8*sizeof(DWORD) - byNumBitsToShift);
	DWORD dwMoveBits = rdwValue & dwMask;
	rdwValue <<= byNumBitsToShift;
	dwMoveBits >>= (8*sizeof(DWORD) - byNumBitsToShift);
	rdwValue |= dwMoveBits;
	return rdwValue;
}

inline WORD ROL(WORD& rwValue, BYTE byNumBitsToShift = 1)
{
	WORD wMask = (1 << byNumBitsToShift) - 1;
	wMask <<= (8*sizeof(WORD) - byNumBitsToShift);
	WORD wMoveBits = rwValue & wMask;
	rwValue <<= byNumBitsToShift;
	wMoveBits >>= (8*sizeof(WORD) - byNumBitsToShift);
	rwValue |= wMoveBits;
	return rwValue;
}

inline BYTE ROL(BYTE& rbyValue, BYTE byNumBitsToShift = 1)
{
	BYTE byMask = (1 << byNumBitsToShift) - 1;
	byMask <<= (8 - byNumBitsToShift);
	BYTE byMoveBits = rbyValue & byMask;
	rbyValue <<= byNumBitsToShift;
	byMoveBits >>= (8 - byNumBitsToShift);
	rbyValue |= byMoveBits;
	return rbyValue;
}


#define AreBitsSet(number, bits) (((number) & (bits)) == (bits))
#define IsBitSet(number, bit) (((number) & (bit)) > 0)

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
inline DWORD GetValueAtBits(DWORD dwInput, DWORD dwBitRange/*Must be contiguous, as in BIT5 | BIT6 | BIT7*/)
// returns decimal value of the bits masked by dwBitRange
// e.g. GetValueAtBits(11 /*00001011*/, BIT2 | BIT3) returns the value 2 /*00000010*/
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
{
	DWORD dwValue=0;
	if (dwBitRange == 0)
		return 0;
	
	// first, mask out all unwanted values
	dwInput &= dwBitRange;

	// now find low end of range
	BYTE byShift=0;
	while (!(dwBitRange & BIT0))	// while low bit is not set
	{
		byShift++;
		dwBitRange >>= 1;
	}
	
	// and shift that many positions
	dwValue = dwInput >> byShift;

	return dwValue;
}


///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
inline void SetValueAtBits(DWORD* pdwSource, DWORD dwBitRange/*Must be contiguous, as in BIT5 | BIT6 | BIT7*/,
						   DWORD dwValue)
// sets value at the bits masked by dwBitRange
// e.g. DWORD dwVar = 11; /* 00001011 in binary */
//      SetValueAtBits(&dwVar, BIT4 | BIT5 | BIT6, 5 /* 101 in binary */);
//      dwVar == 91 /* 01011011 in binary */;
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
{
	if (dwBitRange == 0)
		return;
	// first, mask out all nonzero values in Source that fall in bitrange
	(*pdwSource) &= ~dwBitRange;

	// now find low end of range
	BYTE byShift=0;
	while (!(dwBitRange & BIT0))
	{
		byShift++;
		dwBitRange >>= 1;
	}
	
	// mask out all unwanted bits in value
	dwValue &= dwBitRange;
	// and shift value that many positions
	dwValue <<= byShift;

	// now assign the bits to Source
	(*pdwSource) |= dwValue;

}

// provide correct functions for all of the integral sizes
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
inline void SetValueAtBits(WORD* pwSource, WORD wBitRange, WORD wValue)
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
{
	if (wBitRange == 0) return;
	(*pwSource) &= ~wBitRange;
	BYTE byShift=0;
	while (!(wBitRange & BIT0))
		{byShift++; wBitRange >>= 1;}
	wValue &= wBitRange;
	wValue <<= byShift;
	(*pwSource) |= wValue;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
inline void SetValueAtBits(BYTE* pbySource, BYTE byBitRange, BYTE byValue)
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
{
	if (byBitRange == 0) return;
	(*pbySource) &= ~byBitRange;
	BYTE byShift=0;
	while (!(byBitRange & BIT0))
		{byShift++;	byBitRange >>= 1;}
	byValue &= byBitRange;
	byValue <<= byShift;
	(*pbySource) |= byValue;
}




///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
inline void FlipBits(DWORD& rdwInput, DWORD dwBits)
// Flips the specified bits of dwInput
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
{
	rdwInput = (rdwInput & ~dwBits) | (~rdwInput & dwBits);
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
inline void FlipBits(WORD& rwInput, WORD wBits)
// Flips the specified bits of wInput
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
{
	rwInput = (rwInput & ~wBits) | (~rwInput & wBits);
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
inline void FlipBits(BYTE& rbyInput, BYTE byBits)
// Flips the specified bits of byInput
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
{
	rbyInput = (rbyInput & ~byBits) | (~rbyInput & byBits);
}


/*
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
#define SetBits(nData, bits, bOn) if (bOn) (nData) |= (bits); else (nData) &= ~(bits);
// sets the specified bits of the input value to either on or off
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
*/
template <typename DATATYPE, typename BITSTYPE>
inline void SetBits(DATATYPE& rDest, BITSTYPE bits, bool bOnOff)
{
	if (bOnOff)
		rDest |= bits;
	else
		rDest &= ~bits;
}



/*
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
#define CopyBits(dest,src,bits) ((dest) = ((src) & (bits)) | ((dest) & (~bits)))
// copies specified bits from source to dest
// use like CopyBits(newvar, oldvar, BITS)
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
*/
template <typename DATATYPE, typename BITSTYPE>
inline DATATYPE CopyBits(DATATYPE& rDest, DATATYPE src, BITSTYPE bits)
{
	return rDest = ((src & bits) | (rDest & ~bits));
}


///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
inline DWORD ComputeChangedBits(DWORD dwOld, DWORD dwNew, bool bChangedTo)
// sets the bits in the returned value that represent the bits that have changed to the bChangedTo value (true/false)
// you can use x-or logic if you just want to see what bits changed regardless of on/off state.
// e.g. (1101, 1010, true) returns 0010
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
{
	DWORD dwResult = 0;
	DWORD bit=1;
	for (BYTE bitPos = 0; bitPos < 8*sizeof(DWORD); bitPos++, bit <<= 1)
	{
		if (((dwNew & bit) != 0) == bChangedTo &&
			((dwOld & bit) != 0) == !bChangedTo)
		{
			dwResult |= bit;
		}
	}
	return dwResult;
}




// this is supposed to be cheap to compile so it is located in this fairly lean header
// allows std:: containers (and other template containers) to use pointers to incomplete types.
// this helps reduce dependencies between compilation units
// example of use:
//
// struct MyStruct;
// typedef std::set< SVIncompleteTypePtr<MyStruct> > MyStructPtrSetType;
//
// you can put pointers to MyStruct into MyStructPtrSetType;

template <typename T>
class SVIncompleteTypePtr
{
	public:
	typedef T* value_type;

	SVIncompleteTypePtr(T* t) : m_t(t) {}
	SVIncompleteTypePtr(const SVIncompleteTypePtr<T>& rhs) : m_t(rhs.m_t) {}

	operator T* () { return m_t; }

	bool operator == ( const SVIncompleteTypePtr& rhs ) const
	{
		return m_t == rhs.m_t;
	}
	bool operator != ( const SVIncompleteTypePtr& rhs ) const
	{
		return m_t != rhs.m_t;
	}
	bool operator < ( const SVIncompleteTypePtr& rhs ) const
	{
		return m_t < rhs.m_t;
	}
	bool operator <= ( const SVIncompleteTypePtr& rhs ) const
	{
		return m_t <= rhs.m_t;
	}
	bool operator > ( const SVIncompleteTypePtr& rhs ) const
	{
		return m_t > rhs.m_t;
	}
	bool operator >= ( const SVIncompleteTypePtr& rhs ) const
	{
		return m_t >= rhs.m_t;
	}

	private:
	T* m_t;
};

//******************************************************************************
// Method:		GetAndEnableWindow
// Description:	Gets a control from a dialog and enables or disables it.
// Parameter:	dialog - This should be a pointer the dialog containing the 
//		control to be enabled or disabled.
// Parameter:	ctrlId - This should be the identifier for the control.
// Parameter:	enable - Indicates if the control should be enabled {TRUE, default}
//		or disabled {FALSE}.
// Returns:		HRESULT -
//		S_OK - The control was not previously disabled.
//		S_FALSE - The control was not previously disabled.
//		E_FAIL - ctrlId was not found in dialog.
//******************************************************************************
inline HRESULT GetAndEnableWindow( CWnd* dialog, const int ctrlId, const BOOL enable = TRUE )
{
	HRESULT hr = E_FAIL;

	if ( nullptr != dialog )
	{
		CWnd* item = dialog->GetDlgItem( ctrlId );
		if ( nullptr != item )
		{
			BOOL wasDisabled = item->EnableWindow( enable );
			hr = wasDisabled == FALSE ? S_FALSE : S_OK;
		}
	}

	return hr;
}

