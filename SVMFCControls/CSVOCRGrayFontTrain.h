//******************************************************************************
//* COPYRIGHT (c) 2003 by SVResearch, Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : CSVOCRGrayFontTrain
//* .File Name       : $Workfile:   CSVOCRGrayFontTrain.h  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.0  $
//* .Check In Date   : $Date:   23 Apr 2013 09:13:10  $
//******************************************************************************

#pragma once

namespace Seidenader
{
	namespace SVMFCControls
	{
		class CSVOCRGrayFontTrain : public CWnd
		{
		protected:
			DECLARE_DYNCREATE(CSVOCRGrayFontTrain)
		public:
			CLSID const& GetClsid()
			{
				static CLSID const clsid
					= { 0x29BF4CF3, 0x27AF, 0x11d4, { 0xA8, 0xBA, 0x00, 0x10, 0x6F, 0x03, 0x09, 0xAB } };
				return clsid;
			}
			virtual BOOL Create(LPCTSTR lpszClassName,
				LPCTSTR lpszWindowName, DWORD dwStyle,
				const RECT& rect,
				CWnd* pParentWnd, UINT nID,
				CCreateContext* pContext = NULL)
			{ return CreateControl(GetClsid(), lpszWindowName, dwStyle, rect, pParentWnd, nID); }

			BOOL Create(LPCTSTR lpszWindowName, DWORD dwStyle,
				const RECT& rect, CWnd* pParentWnd, UINT nID,
				CFile* pPersist = NULL, BOOL bStorage = FALSE,
				BSTR bstrLicKey = NULL)
			{ return CreateControl(GetClsid(), lpszWindowName, dwStyle, rect, pParentWnd, nID,
			pPersist, bStorage, bstrLicKey); }

			// Attributes
		public:

			// Operations
		public:
			BOOL SetLVLastCallBackErrorCode(long FAR* Code);
			long DoSVOCRGrayFontTrain(LPCTSTR PathFileSampleCharImage, LPCTSTR PathFileFontImagesAndLabels);
		};
	} //SVMFCControls
} //Seidenader

namespace SvMc = Seidenader::SVMFCControls;

//******************************************************************************
//* LOG HISTORY:
//******************************************************************************
/*
$Log:   N:\PVCSarch65\ProjectFiles\archives\SVObserver_src\SVObserver\CSVOCRGrayFontTrain.h_v  $
 * 
 *    Rev 1.0   23 Apr 2013 09:13:10   bWalter
 * Project:  SVObserver
 * Change Request (SCR) nbr:  814
 * SCR Title:  Upgrade SVObserver to Compile Using Visual Studio 2010
 * Checked in by:  bWalter;  Ben Walter
 * Change Description:  
 *   Initial check in to SVObserver_src.  (Merged with svo_src label SVO 6.10 Beta 008.)
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.2   17 Apr 2003 15:51:12   rschock
 * Project:  SVObserver
 * Change Request (SCR) nbr:  346
 * SCR Title:  Update SVObserver to Version 4.21 Release
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Redid the #include defines and standardized the Tracker log headers and removed warning from release mode builds.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.1   07 Aug 2000 12:52:22   Joe
 * Project:  SVObserver
 * Change Request (SCR) nbr:  173
 * SCR Title:  Fix OCR Font Training - Pressing ROI Ok button when no ROI is selected
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Added code to pass the global variable containing the FastOCR error code into the Gray Font Training OCX.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.0   16 May 2000 14:38:30   Joe
 * Project:  SVObserver
 * Change Request (SCR) nbr:  133, 132
 * SCR Title:  Add OCR Gray Scale to SVObserver and Create OCR Gray Scale Font Training
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Upgrade Logical Vision WiT v5.3 to Logical Vision FastOCR v1.2.  Completely changed headers, libraries, and variable name types.  Functionality still the same as prior version.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.0   23 Nov 1999 13:23:16   mike
 * Project:  SVObserver
 * Change Request (SCR) nbr:  61
 * SCR Title:  Update PVCS versioning to version 3.0.
 * Checked in by:  Mike;  Mike McCarl
 * Change Description:  
 *   
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.7   Nov 04 1999 16:23:40   Nick
 * Project:  SVObserver
 * Change Request (SCR) nbr:  12
 * SCR Title:  Port OCR functionallity from custom 2.xx version.
 * Checked in by:  Nick;  F Roland "Nick" Bjorklund
 * Change Description:  
 *   Removed 'Test' button and added check for existance of .Wit Font  training file.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.6   Oct 20 1999 07:21:12   Nick
 * Project:  SVObserver
 * Change Request (SCR) nbr:  12
 * SCR Title:  Port OCR functionallity from custom 2.xx version.
 * Checked in by:  Nick;  F Roland "Nick" Bjorklund
 * Change Description:  
 *   Changes for character x and y axis sort work.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.5   Oct 12 1999 16:59:30   Nick
 * Project:  SVObserver
 * Change Request (SCR) nbr:  12
 * SCR Title:  Port OCR functionallity from custom 2.xx version.
 * Checked in by:  Nick;  F Roland "Nick" Bjorklund
 * Change Description:  
 *   Repair Test and Edit Font button sizes and locations.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.4   Aug 31 1999 19:21:46   Nick
 * Project:  SVObserver
 * Change Request (SCR) nbr:  12
 * SCR Title:  Port OCR functionallity from custom 2.xx version.
 * Checked in by:  Nick;  F Roland "Nick" Bjorklund
 * Change Description:  
 *   OCR converted to 3.0 architecture.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
*/