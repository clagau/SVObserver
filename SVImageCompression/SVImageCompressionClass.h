// ******************************************************************************
// * COPYRIGHT (c) 2003 by SVResearch, Harrisburg
// * All Rights Reserved
// ******************************************************************************
// * .Module Name     : SVImageCompressionClass
// * .File Name       : $Workfile:   SVImageCompressionClass.h  $
// * ----------------------------------------------------------------------------
// * .Current Version : $Revision:   1.0  $
// * .Check In Date   : $Date:   22 Apr 2013 10:22:02  $
// ******************************************************************************

#pragma once

class SVCompressionDlg;

#ifdef ICOMPRESSIONMASTER
#  define ICOMPEXT
#else
#  define ICOMPEXT extern
#endif

#ifndef SV_ICOMP_DS
   #ifdef BUILD_ICOMP_DLL
   #define SV_ICOMP_DS __declspec (dllexport)
   #else
   #define SV_ICOMP_DS __declspec (dllimport)
   #endif
#endif

extern long lHardICompAbstraction [1100][2];

/*
The Image Compression Class is the interface class to the image compression system.  This class presents method to compress and extract images based on the desired compression algorithm.
*/
class SV_ICOMP_DS SVImageCompressionClass  
{
public:
	SVImageCompressionClass (long alPoolSize = 0);
	virtual ~SVImageCompressionClass();

	HRESULT	CreateResourcePool ();
	HRESULT	DestroyResourcePool ();

	HRESULT  ImportGlobalAllocDIBData ();

	HRESULT  ImportBSTR (BSTR abstrInputImage);

	HRESULT  ExportBSTR (BSTR* apbstrOutputImage, long  alColorDepth, long  alCompression);

	HRESULT  ExportGlobalAllocDIBData (HGLOBAL* ahpBitmap);

	HRESULT  ExportIPicture (LPPICTUREDISP* appIPictureDispatch);

	HRESULT  GetAvailableResource ();

	HRESULT  GetAndLockInputDIBBuffer (char** appInputDIBBuffer);

	HRESULT  UnLockInputDIBBuffer ();

private:
	// This attribute is used to hold the index into the image data resource used by LEAD Tools for the compression operations.
	long m_DMIndex;

	// This attribute is used to hold the transaction identifier for the index of the image data resource used by LEAD Tools for the compression operations.
	long m_DMTxnID;

	// This attribute contains the current pool size for the image compression system.
	long						svmlICompressionPoolSize;

	// This pointer preserves the instance of the lead tool within StaticImageCompressionPool when a pool is not being used.
	SVCompressionDlg*    svmpICompressionDlg;

};


