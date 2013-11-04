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

#ifndef SVIMAGECOMPRESSIONCLASS_H
#define SVIMAGECOMPRESSIONCLASS_H

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


#endif // !defined(AFX_SVIMAGECOMPRESSIONCLASS_H__17BC8A0D_5C0D_4D9D_A61E_4841D0293075__INCLUDED_)

// ******************************************************************************
// * LOG HISTORY:
// ******************************************************************************
/*
$Log:   N:\PVCSarch65\ProjectFiles\archives\SVObserver_SRC\SVImageCompression\SVImageCompressionClass.h_v  $
 * 
 *    Rev 1.0   22 Apr 2013 10:22:02   bWalter
 * Project:  SVObserver
 * Change Request (SCR) nbr:  814
 * SCR Title:  Upgrade SVObserver to Compile Using Visual Studio 2010
 * Checked in by:  bWalter;  Ben Walter
 * Change Description:  
 *   Initial check in to SVObserver_src.  (Merged with svo_src label SVO 6.10 Beta 008.)
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.7   15 Feb 2010 08:19:02   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  682
 * SCR Title:  Upgrade SVObserver version for 5.31 release
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated old and added missing source code comments to existing code.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.6   13 Jul 2009 12:17:46   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  665
 * SCR Title:  Fix unrecoverable failure when processing acquisitions at high speed
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Added new transaction id variable used to track the data manager index.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.5   19 Jun 2007 13:06:20   Joe
 * Project:  SVObserver
 * Change Request (SCR) nbr:  598
 * SCR Title:  Upgrade SVObserver to compile using vc++ in VS2005
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   These changes include modification based on fixing compiler-based and project-based differences between VC6 and VC8.  These changes mainly include casting issues, but some include type conversion and assignment of new compiler controlling defines.
 * 
 * This project includes a change to fix a problem with the attachment of the Lead Tools ActiveX to an SVObserver Dialog.  All Lead Tools includes have been moved from the header files to the cpp files and the appropriate externs were generated to get access to thedesired data.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.4   09 Apr 2003 15:47:18   tbair
 * Project:  SVObserver
 * Change Request (SCR) nbr:  344
 * SCR Title:  Fix Bug in SVImageCompressionClass so shutdown is clean
 * Checked in by:  tBair;  Tom Bair
 * Change Description:  
 *   Added CreateResourcePool and DestroyResourcePool.  These functions are used to control when resources are created and destroyed.  This is because when they were left to automatically destroy, they crashed.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.3   28 Feb 2003 08:53:40   Joe
 * Project:  SVObserver
 * Change Request (SCR) nbr:  318
 * SCR Title:  Image Transfer Compression
 * Checked in by:  eBeyeler;  Eric Beyeler
 * Change Description:  
 *   Added PVCS header information.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
*/