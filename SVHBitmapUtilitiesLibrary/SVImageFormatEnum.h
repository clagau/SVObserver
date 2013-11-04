//******************************************************************************
//* COPYRIGHT (c) 2013 by Seidenader Vision Inc., Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVImageFormatEnum
//* .File Name       : $Workfile:   SVImageFormatEnum.h  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.0  $
//* .Check In Date   : $Date:   22 Apr 2013 10:11:24  $
//******************************************************************************

#ifndef SVIMAGE_FORMAT_ENUM_H
#define SVIMAGE_FORMAT_ENUM_H
#pragma once


enum SVImageFormatEnum           // Matrox Image Format Definitions
{
	SVImageFormatUnknown    = 0,
	SVImageFormatMono8      = 10,  // default format
	SVImageFormatMono16     = 13,  // default format
	SVImageFormatMono32     = 15,  // default format
	SVImageFormatRGB5551    = 20,  // M_RGB15
	SVImageFormatRGB565     = 25,  // M_RGB16
	SVImageFormatRGB888     = 30,  // M_BGR24
	SVImageFormatRGB8888    = 35,  // M_BGR32
	SVImageFormatRGB101010  = 40,
	SVImageFormatRGB161616  = 50,
	SVImageFormatRGBP8      = 60,  // M_RGB24+M_PLANAR
	SVImageFormatRGBP16     = 70,  // M_RGB48
	SVImageFormatUYVY       = 100, // M_YUV16_UYVY
	SVImageFormatYUY2       = 110, // M_YUV16_YUYV 
	SVImageFormatYVYU       = 120, 
	SVImageFormatYUYV       = 130, // M_YUV16_YUYV
	SVImageFormatY411       = 140, 
	SVImageFormatY422       = 150, // M_YUV16+M_PACKED
	SVImageFormatY444       = 160,
};

#endif SVIMAGE_FORMAT_ENUM_H

//******************************************************************************
//* LOG HISTORY:
//******************************************************************************
/*
$Log:   N:\PVCSarch65\ProjectFiles\archives\SVObserver_SRC\SVHBitmapUtilitiesLibrary\SVImageFormatEnum.h_v  $
 * 
 *    Rev 1.0   22 Apr 2013 10:11:24   bWalter
 * Project:  SVObserver
 * Change Request (SCR) nbr:  814
 * SCR Title:  Upgrade SVObserver to Compile Using Visual Studio 2010
 * Checked in by:  bWalter;  Ben Walter
 * Change Description:  
 *   Initial check in to SVObserver_src.  (Merged with svo_src label SVO 6.10 Beta 008.)
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
*/