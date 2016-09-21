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

//Moved to precompiled header: #include <float.h>
//Moved to precompiled header: #include <math.h>

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

