//******************************************************************************
//* COPYRIGHT (c) 2003 by SVResearch, Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVBarCodeAnalyzerClass
//* .File Name       : $Workfile:   svbarcodeanalyzerclass.h  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.1  $
//* .Check In Date   : $Date:   01 Feb 2014 10:16:26  $
//******************************************************************************

#ifndef SVBARCODEANALYZERCLASS_H
#define SVBARCODEANALYZERCLASS_H

#include "SVMatroxLibrary/SVMatroxLibrary.h"
#include "SVAnalyzer.h"

enum 
{
	SVBCStringFormatRemoveCharacters = 0,
	SVBCStringFormatTranslateCharacters,
	SVBCStringFormatReplaceCharacters,
};
class SVBarCodeAnalyzerClass : public SVImageAnalyzerClass  
{
	friend class SVSetupDialogManager;

	SV_DECLARE_CLASS (SVBarCodeAnalyzerClass);

public:
	CString m_csRegExpressionValue;
	CString m_csStringFileName;

	SVBoolValueObjectClass m_bWarnOnFailedRead;
	virtual LONG_PTR processMessage( DWORD DwMessageID, LONG_PTR DwMessageValue, LONG_PTR DwMessageContext );
	SVStringValueObjectClass msv_szBarCodeValue;
	SVStringValueObjectClass msv_szRegExpressionValue;
	SVLongValueObjectClass msv_lBarCodeType;
	SVDoubleValueObjectClass msv_dOrientation;
	SVDoubleValueObjectClass msv_dSkewNegative;
	SVDoubleValueObjectClass msv_dSkewPositive;
	SVDoubleValueObjectClass msv_dCellMinSize;
	SVDoubleValueObjectClass msv_dCellMaxSize;
	SVDoubleValueObjectClass msv_dCellNumberX;
	SVDoubleValueObjectClass msv_dCellNumberY;
	SVDoubleValueObjectClass msv_dSpeed;
	SVDoubleValueObjectClass msv_dThreshold;
	SVDoubleValueObjectClass msv_dStringSize;
	SVDoubleValueObjectClass msv_dErrorCorrection;
	SVDoubleValueObjectClass msv_dEncoding;
	SVDoubleValueObjectClass msv_dForegroundColor;
	SVBoolValueObjectClass msv_bSaveStringInFile;
	SVFileNameValueObjectClass msv_szStringFileName;
	SVLongValueObjectClass msv_lBarcodeTimeout;
	// To support special DMCs May 2008.
	SVByteValueObjectClass msv_RawData;
	SVEnumerateValueObjectClass msv_eStringFormat;
	SVLongValueObjectClass msv_lThresholdType;

	//for MIL 9.0 - New only for DataMatrix Codes
	SVBoolValueObjectClass msv_bUnEvenGrid;
	
	SVMatroxBarCode m_MilCodeId;

	SVBarCodeAnalyzerClass (BOOL BCreateDefaultTaskList = TRUE, SVObjectClass* POwner = NULL, int StringResourceID = IDS_CLASSNAME_SVBARCODEANALYZER);
	void CloseMil ();
	BOOL InitMil ();
	virtual BOOL CreateObject (SVObjectLevelCreateStruct* PCreateStructure);
	virtual BOOL OnValidate ();

	SVResultClass* GetResultObject();

	virtual ~SVBarCodeAnalyzerClass ();
	virtual BOOL CloseObject ();

	virtual HRESULT ResetObject();
	static bool CharIsControl( TCHAR p_Char);
protected:
	void init ();
	virtual BOOL onRun (SVRunStatusClass &RRunStatus);
private:
	BOOL SaveRegExpression ();
	BOOL LoadRegExpression ();

	bool m_bHasLicenseError;
};

#endif // !defined(AFX_SVBARCODEANALYZERCLASS_H__A8960E74_74C9_11D3_A7C8_00106F010A93__INCLUDED_)

//******************************************************************************
//* LOG HISTORY:
//******************************************************************************
/*
$Log:   N:\PVCSarch65\ProjectFiles\archives\SVObserver_SRC\SVObserver\svbarcodeanalyzerclass.h_v  $
 * 
 *    Rev 1.1   01 Feb 2014 10:16:26   tbair
 * Project:  SVObserver
 * Change Request (SCR) nbr:  852
 * SCR Title:  Add Multiple Platform Support to SVObserver's Visual Studio Solution
 * Checked in by:  tBair;  Tom Bair
 * Change Description:  
 *   Changed sendmessage to use LONG_PTR instead of DWORD.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.0   23 Apr 2013 09:40:28   bWalter
 * Project:  SVObserver
 * Change Request (SCR) nbr:  814
 * SCR Title:  Upgrade SVObserver to Compile Using Visual Studio 2010
 * Checked in by:  bWalter;  Ben Walter
 * Change Description:  
 *   Initial check in to SVObserver_src.  (Merged with svo_src label SVO 6.10 Beta 008.)
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.22   27 Feb 2013 11:19:56   ryoho
 * Project:  SVObserver
 * Change Request (SCR) nbr:  796
 * SCR Title:  Add a License Manager that will handle the Matrox Licenses
 * Checked in by:  rYoho;  Rob Yoho
 * Change Description:  
 *   Added check to see if the identification license is present
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.21   04 Oct 2012 11:06:36   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  602
 * SCR Title:  Revise the Toolset Parsing and Object Creation Methodology
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Remove scope declaration on header methods.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.20   18 Jul 2012 13:23:46   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  769
 * SCR Title:  Fix Problems and Crashes with Inspection Document Display Updates
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Removed obsolete methods assiciated with overlay drawling and fixed problem with copying data to byte buffer have wrong format.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.19   29 Jun 2012 14:56:20   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  771
 * SCR Title:  Upgrade to the Next Version of Matrox Imaging Library (MIL)
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated resource file to add the new barcode parameter to fix an issues with MIL 9 processing.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.18   10 Feb 2011 14:30:24   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  712
 * SCR Title:  Fix issues with black images when using command interface (SIAC)
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated source code to now use shared pointer instead of untracked structure.  This fixes destruction, tracking and rebuilding issues.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.17   27 Jan 2011 10:47:14   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  712
 * SCR Title:  Fix issues with black images when using command interface (SIAC)
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated source code to fix issues with Matrox Buffer Management.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.16   03 Jun 2008 14:24:34   tbair
 * Project:  SVObserver
 * Change Request (SCR) nbr:  627
 * SCR Title:  Add RawData object to the Barcode Analyzer and format read string
 * Checked in by:  tBair;  Tom Bair
 * Change Description:  
 *   Added Threshold Type Radio to Dialog.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.15   30 May 2008 14:43:40   tbair
 * Project:  SVObserver
 * Change Request (SCR) nbr:  627
 * SCR Title:  Add RawData object to the Barcode Analyzer and format read string
 * Checked in by:  tBair;  Tom Bair
 * Change Description:  
 *   Added new value object to hold adaptive threshold bool for barcode.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.14   29 May 2008 09:52:16   tbair
 * Project:  SVObserver
 * Change Request (SCR) nbr:  627
 * SCR Title:  Add RawData object to the Barcode Analyzer and format read string
 * Checked in by:  tBair;  Tom Bair
 * Change Description:  
 *   Modified OnRun function to remove, translate, or replace control characters. 
 * 
 * Also added RawData output.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.13   24 Jul 2007 10:49:32   tbair
 * Project:  SVObserver
 * Change Request (SCR) nbr:  597
 * SCR Title:  Upgrade Matrox Imaging Library to version 8.0
 * Checked in by:  tBair;  Tom Bair
 * Change Description:  
 *   Update to Matrox Imaging Library 8.0
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.12   21 Jun 2007 11:56:30   Joe
 * Project:  SVObserver
 * Change Request (SCR) nbr:  598
 * SCR Title:  Upgrade SVObserver to compile using vc++ in VS2005
 * Checked in by:  jSpila;  Joseph Spila
 * Change Description:  
 *   These changes include modification based on fixing compiler-based and project-based differences between VC6 and VC8.  These changes mainly include casting issues, but some include type conversion and assignment of new compiler controlling defines.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.11   29 Sep 2005 09:48:44   ryoho
 * Project:  SVObserver
 * Change Request (SCR) nbr:  498
 * SCR Title:  Add individual timeout capability to Bar Code analyzers
 * Checked in by:  rYoho;  Rob Yoho
 * Change Description:  
 *   added new value object for the Bar Code Timeout
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.10   08 Aug 2005 15:27:30   Joe
 * Project:  SVObserver
 * Change Request (SCR) nbr:  500
 * SCR Title:  Reduce delay when adjusting tool parameters with a large toolset
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated objects and dialogs to use the ActiveX Interface to update inspection data.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.9   28 Jun 2005 15:49:50   ebeyeler
 * Project:  SVObserver
 * Change Request (SCR) nbr:  498
 * SCR Title:  Add individual timeout capability to Bar Code analyzers
 * Checked in by:  eBeyeler;  Eric Beyeler
 * Change Description:  
 *   added #include <static_check.h>
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.8   28 Jun 2005 10:01:54   ebeyeler
 * Project:  SVObserver
 * Change Request (SCR) nbr:  498
 * SCR Title:  Add individual timeout capability to Bar Code analyzers
 * Checked in by:  eBeyeler;  Eric Beyeler
 * Change Description:  
 *   added check for MIL 7.5 Processing Pack 1
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.7   17 Feb 2005 13:10:06   Joe
 * Project:  SVObserver
 * Change Request (SCR) nbr:  456
 * SCR Title:  Update Image and Tool Objects to use the new Extent Classes
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Update class with new methodologies based on new base object, extents and reset structure.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.6   08 Sep 2004 11:10:22   tbair
 * Project:  SVObserver
 * Change Request (SCR) nbr:  444
 * SCR Title:  Change Bar Code Analyzer Pass / Fail Result
 * Checked in by:  tBair;  Tom Bair
 * Change Description:  
 *   Modified OnRun to include Match Score and warn on fail logic.
 * Modified CreateObject and Init to support new value objects
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.5   17 Apr 2003 16:45:18   rschock
 * Project:  SVObserver
 * Change Request (SCR) nbr:  346
 * SCR Title:  Update SVObserver to Version 4.21 Release
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Redid the #include defines and standardized the Tracker log headers and removed warning from release mode builds.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
*/
