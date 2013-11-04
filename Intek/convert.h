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

//******************************************************************************
//* LOG HISTORY:
//******************************************************************************
/*
$Log:   N:\PVCSarch65\ProjectFiles\archives\SVObserver_src\Intek\convert.h_v  $
 * 
 *    Rev 1.0   29 Apr 2013 10:07:38   bWalter
 * Project:  SVObserver
 * Change Request (SCR) nbr:  814
 * SCR Title:  Upgrade SVObserver to Compile Using Visual Studio 2010
 * Checked in by:  bWalter;  Ben Walter
 * Change Description:  
 *   Initial check in to SVObserver_src.  (Merged with svo_src label SVO 6.10 Beta 008.)
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.2   14 Oct 2009 18:35:24   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  633
 * SCR Title:  Upgrade Intek FireStack Driver and API to version 2.29
 * Checked in by:  jSpila;  Joseph Spila
 * Change Description:  
 *   Updated files based on changes from the Intek provider from 2v15 to 2v30.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.1   01 Aug 2006 10:56:04   Joe
 * Project:  SVObserver
 * Change Request (SCR) nbr:  569
 * SCR Title:  Fix Format 7 Color Issue with Intek Acquisition Sub-system
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Added new Y8 conversion routine.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.0   26 Jul 2006 15:24:40   Joe
 * Project:  SVObserver
 * Change Request (SCR) nbr:  569
 * SCR Title:  Fix Format 7 Color Issue with Intek Acquisition Sub-system
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Added to new files to handle converting a flat image block to a Windows Bitmap DIB block.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
*/
