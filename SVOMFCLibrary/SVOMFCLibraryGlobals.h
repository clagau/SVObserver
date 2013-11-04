// ******************************************************************************
// * COPYRIGHT (c) 2003 by SVResearch, Harrisburg
// * All Rights Reserved
// ******************************************************************************
// * .Module Name     : SVOMFCLibraryGlobals
// * .File Name       : $Workfile:   SVOMFCLibraryGlobals.h  $
// * ----------------------------------------------------------------------------
// * .Current Version : $Revision:   1.0  $
// * .Check In Date   : $Date:   25 Apr 2013 13:10:16  $
// ******************************************************************************

#ifndef _SVOMFCLIBRARYGLOBALS_H_
#define _SVOMFCLIBRARYGLOBALS_H_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include <float.h>

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
	return ((f1 - f2) < FLT_EPSILON) || ((f2 - f1) < FLT_EPSILON);
	}

inline bool ApproxEqual(double d1, double d2)
	{
	return ((d1 - d2) < DBL_EPSILON) || ((d2 - d1) < DBL_EPSILON);
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

#endif	//#ifndef _SVOMFCLIBRARYGLOBALS_H_

// ******************************************************************************
// * LOG HISTORY:
// ******************************************************************************
/*
$Log:   N:\PVCSarch65\ProjectFiles\archives\SVObserver_src\SVOMFCLibrary\SVOMFCLibraryGlobals.h_v  $
 * 
 *    Rev 1.0   25 Apr 2013 13:10:16   bWalter
 * Project:  SVObserver
 * Change Request (SCR) nbr:  814
 * SCR Title:  Upgrade SVObserver to Compile Using Visual Studio 2010
 * Checked in by:  bWalter;  Ben Walter
 * Change Description:  
 *   Initial check in to SVObserver_src.  (Merged with svo_src label SVO 6.10 Beta 008.)
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.5   19 May 2005 14:43:36   ebeyeler
 * Project:  SVObserver
 * Change Request (SCR) nbr:  482
 * SCR Title:  New vector based mask
 * Checked in by:  eBeyeler;  Eric Beyeler
 * Change Description:  
 *   added SVLoadString
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.4   09 Mar 2005 09:38:38   ebeyeler
 * Project:  SVObserver
 * Change Request (SCR) nbr:  456
 * SCR Title:  Update Image and Tool Objects to use the new Extent Classes
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   moved SVHRESULT to SVLibrary/SVHRESULT.h
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.3   18 Feb 2005 12:09:16   ebeyeler
 * Project:  SVObserver
 * Change Request (SCR) nbr:  456
 * SCR Title:  Update Image and Tool Objects to use the new Extent Classes
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   added template SVIncompleteTypePtr
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.2   17 Feb 2005 11:42:36   Joe
 * Project:  SVObserver
 * Change Request (SCR) nbr:  456
 * SCR Title:  Update Image and Tool Objects to use the new Extent Classes
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Added SVHRESULT Class to handle HRESULT conversions from BOOL.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.1   02 Apr 2004 12:06:20   ryoho
 * Project:  SVObserver
 * Change Request (SCR) nbr:  411
 * SCR Title:  Don't print Calculated values of Line Analyzers in Configuration Report
 * Checked in by:  rYoho;  Rob Yoho
 * Change Description:  
 *   changed SetBits and Copybits from defines to inline functions
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.0   07 Jul 2003 10:19:34   ebeyeler
 * Project:  SVObserver
 * Change Request (SCR) nbr:  320
 * SCR Title:  Integrate Matrox Meteor II / 1394 Board and 1394 camera into SVObserver
 * Checked in by:  eBeyeler;  Eric Beyeler
 * Change Description:  
 *   transfered from SVObserver project
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
*/