//******************************************************************************
//* .Module Name     : Convert
//* .File Name       : $Workfile:   convert.h  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.0  $
//* .Check In Date   : $Date:   29 Apr 2013 10:07:38  $
//******************************************************************************

//* This file include changes of original package code to allow this driver
//* to function in the SVObserver enviroment.

////////////////////////////////////////////////////////////////////////////////
// Headerfile for conversion module for video data.
// C. Kuehnel, intek, 11.5.2001
////////////////////////////////////////////////////////////////////////////////

#ifndef CONVERT_H
#define CONVERT_H

#include <basetype.h>

#ifdef __cplusplus
extern "C" {
#endif

void InitTables();

void ConvertRGB(UINT32 XSize,UINT32 YSize,UINT8 *pBuf,UINT8 *pBGR,UINT16 BitCnt);
void ConvertRGB16(UINT32 XSize,UINT32 YSize,UINT8 *pBuf,UINT8 *pBGR,UINT8 Swap,UINT16 BitCnt);
void Convert444(UINT32 XSize,UINT32 YSize,UINT8 *pBuf,UINT8 *pBGR,UINT16 BitCnt);
void Convert422(UINT32 XSize,UINT32 YSize,UINT8 *pBuf,UINT8 *pBGR,UINT16 BitCnt);
void Convert411(UINT32 XSize,UINT32 YSize,UINT8 *pBuf,UINT8 *pBGR,UINT16 BitCnt);
void ConvertY8(UINT32 XSize,UINT32 YSize,UINT8 *pBuf,UINT8 *pBGR,UINT16 BitCnt);
void ConvertY16(UINT32 XSize,UINT32 YSize,UINT8 *pBuf,UINT8 *pBGR,UINT16 BitCnt);
void ConvertY16a(UINT32 XSize,UINT32 YSize,UINT8 *pBuf,UINT8 *pBGR,UINT16 BitCnt);
void ConvertRawY8(UINT32 XSize,UINT32 YSize,UINT8 *pBuf,UINT8 *pBGR,UINT8 BayerPattern,UINT16 BitCnt);
void ConvertRawY16(UINT32 XSize,UINT32 YSize,UINT8 *pBuf,UINT8 *pBGR,UINT8 BayerPattern,UINT16 BitCnt);
void ConvertRawY16a(UINT32 XSize,UINT32 YSize,UINT8 *pBuf,UINT8 *pBGR,UINT8 BayerPattern,UINT16 BitCnt);

void XPlane444(UINT32 XSize,UINT32 YSize,UINT8 *pBuf,UINT8 *pR,UINT8 *pG,UINT8 *pB);
void XPlane422(UINT32 XSize,UINT32 YSize,UINT8 *pBuf,UINT8 *pR,UINT8 *pG,UINT8 *pB);
void XPlane411(UINT32 XSize,UINT32 YSize,UINT8 *pBuf,UINT8 *pR,UINT8 *pG,UINT8 *pB);

#ifdef __cplusplus
}
#endif

#endif

