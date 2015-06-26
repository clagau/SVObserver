//******************************************************************************
//* COPYRIGHT (c) 2003 by SVResearch, Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVConfigurationPrint
//* .File Name       : $Workfile:   SVConfigurationPrint.h  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.4  $
//* .Check In Date   : $Date:   06 Jan 2015 10:21:34  $
//******************************************************************************

#ifndef SVCONFIGURATIONPRINT_H
#define SVCONFIGURATIONPRINT_H

#pragma region Includes
#include "SVFileNameClass.h"  // SES 11-Jan-2001
#include "SVInfoStructs.h"
#pragma region Includes

#pragma region Declarations
enum
{
	SV_PRINT_LINES_PER_PAGE                     = 70,	// Sri 2/16/00
	SV_DEFAULT_PRINT_TITLE_FONT_HEIGHT          = 100,

	SV_DEFAULT_PRINT_SUB_TITLE_FONT_HEIGHT      = 90,

	SV_DEFAULT_PRINT_TEXT_FONT_HEIGHT           = 80,

	SV_DEFAULT_PAGE_TEXT_FONT_HEIGHT            = 80,
};

class SVObjectClass;
class SVTaskObjectListClass;
class SVDiscreteInputsView;
class SVDiscreteOutputsView;
class SVDeviceParamConfigPrintHelper;
#pragma endregion Declarations

class SVConfigurationPrint : public CCmdTarget
{
	DECLARE_DYNCREATE(SVConfigurationPrint)
		
#pragma region Public Methods
public:
#pragma region Constructor
	SVConfigurationPrint();
	virtual ~SVConfigurationPrint();
#pragma endregion Constructor
	
    void DoPrintConfig();
    void printConfigToStringBuffer(CString& rsBuffer);
    void printConfigToStringBuffer(SVString& rsBuffer);
	void printConfigToStringBuffer(std::string& rsBuffer);
#pragma endregion Public Methods
	
	// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(SVConfigurationPrint)
	//}}AFX_VIRTUAL
	
#pragma region Protected Methods
protected:
	BOOL  DoPreparePrinting(CPrintInfo* PInfo);
	// Special for config printing...
	void OnPrepareDC();
	void OnBeginPrinting();
	void OnVirtualPrint(BOOL BRealPrint = FALSE);
	void PrintPage();
	void OnEndPrinting();

	void PrintFriends(CDC* pDC, SVObjectClass* pObj, CPoint& ptCurPos, int nIndentLevel);
	void PrintAllChildren(CDC* pDC, SVTaskObjectListClass* pTaskObj,  CPoint& ptCurPos, int nIndentLevel);
	void PrintChildren(CDC* pDC, SVObjectClass* pObj, CPoint& ptCurPos, int nIndentLevel);
	void PrintDetails(CDC* pDC, SVObjectClass* pObj, CPoint& ptCurPos, int nIndentLevel);
	void PrintObject(CDC* pDC, SVObjectClass* pObj, CPoint& ptCurPos, int nIndentLevel);

	int  PrintString(CDC* pDC, CPoint& ptCurPos, LPCTSTR lpszStr);
	void PrintValueObject(CDC* pDC, CPoint& ptCurPos, LPCTSTR lpszName, LPCTSTR lpszValue);

	void PrintCameraSummary(CDC* pDC, CPoint& ptCurPos, int nIndentLevel);
	void PrintTriggerSummary(CDC* pDC, CPoint& ptCurPos, int nIndentLevel);
	void PrintInspectionSummary(CDC* pDC, CPoint& ptCurPos, int nIndentLevel);
	void PrintPPQSummary(CDC* pDC, CPoint& ptCurPos, int nIndentLevel);

	void PrintInputOutputList(CDC* pDC, SVObjectClass* pObj, CPoint& ptCurPos, int nIndentLevel);
	void PrintOCRParameters(CDC* pDC, CString strParameters, CPoint &ptCurPos, int nIndentLevel);
	void PrintOCRGrayScaleParameters(CDC* pDC, CString strParameters, CPoint &ptCurPos, int nIndentLevel);
	void PrintIOEntryObject(CDC* pDC, CPoint& ptCurPos, int nIndentLevel, LPCTSTR lpszName, SVIOEntryHostStructPtr IOEntry);
	void PrintMonitorListItem(CDC* pDC, CPoint& ptCurPos, int nIndentLevel, LPCTSTR lpszName, LPCTSTR lpszValue);
	void PrintIOSection(CDC* pDC, CPoint& ptCurPos, int nIndentLevel);
	void PrintModuleIO(CDC* pDC, CPoint& ptCurPos, int nIndentLevel);
	void PrintThreadAffinity(CDC* pDC, CPoint& ptCurPos, int nIndentLevel);
	void PrintResultIO(CDC* pDC, CPoint& ptCurPos, int nIndentLevel);
	void PrintMonitorListSection(CDC* pDC, CPoint& ptCurPos, int nIndentLevel);
	void PrintPPQBarSection(CDC* pDC, CPoint& ptCurPos, int nIndentLevel);
	void PrintInspectionToolSet(CDC* pDC, CPoint& ptCurPos, int nIndentLevel);

	//************************************
	//! Method to print the Global Constants
	//! \param pDC [in] pointer to the printer device context
	//************************************
	void PrintGlobalConstants( CDC* pDC, CPoint& rCurPos, int nIndentLevel ) ;
#pragma endregion Protected Methods

	// Generated message map functions
	//{{AFX_MSG(SVConfigurationPrint)
	// NOTE - the ClassWizard will add and remove member functions here.
	//}}AFX_MSG
	
	DECLARE_MESSAGE_MAP()

	friend class SVDeviceParamConfigPrintHelper;

#pragma region Member Variables
protected:
	CDWordArray m_virtualPrintPageInfoList; 

	int m_NPArraySize;
	int m_toolNumber;

	int m_shortTabPixels;
	static const int m_TabPixels = 100;
	int m_pageCenter;

	CFont m_fontTitle;
	CFont m_fontSection;
	CFont m_fontSubSection;
	CFont m_fontText;
	CFont m_fontPageNbr;

	int m_heightPageNumberPixels;

	CPrintInfo m_printInfo;
	CDC m_printDC;

	BOOL m_isRealPrint;

	SVFileNameClass m_svfnFileName;    // SES 11-Jan-2001

	bool m_isPrintToStringBuffer;
	CString* m_psBuffer;
#pragma endregion Member Variables
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif

//******************************************************************************
//* LOG HISTORY:
//******************************************************************************
/*
$Log:   N:\PVCSarch65\ProjectFiles\archives\SVObserver_SRC\SVObserver\SVConfigurationPrint.h_v  $
 * 
 *    Rev 1.4   06 Jan 2015 10:21:34   tbair
 * Project:  SVObserver
 * Change Request (SCR) nbr:  960
 * SCR Title:  Pipe/core management
 * Checked in by:  tBair;  Tom Bair
 * Change Description:  
 *   Add print capability for Thread Management
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.3   16 Jul 2014 07:53:50   ryoho
 * Project:  SVObserver
 * Change Request (SCR) nbr:  886
 * SCR Title:  Add RunReject Server Support to SVObserver
 * Checked in by:  rYoho;  Rob Yoho
 * Change Description:  
 *   added methods PrintMonitorListSection and PrintMonitorListItem
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.0   16 Jul 2014 07:52:22   ryoho
 * Project:  SVObserver
 * Change Request (SCR) nbr:  886
 * SCR Title:  Add RunReject Server Support to SVObserver
 * Checked in by:  rYoho;  Rob Yoho
 * Change Description:  
 *   added methods PrintMonitorListSection and PrintMonitorListItem
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.2   14 Jul 2014 14:54:42   sjones
 * Project:  SVObserver
 * Change Request (SCR) nbr:  906
 * SCR Title:  SVObserver Tool Grouping
 * Checked in by:  sJones;  Steve Jones
 * Change Description:  
 *   Added PrintAllChildren method.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.1   04 Feb 2014 13:26:08   bwalter
 * Project:  SVObserver
 * Change Request (SCR) nbr:  880
 * SCR Title:  Remove .SEC
 * Checked in by:  mZiegler;  Marc Ziegler
 * Change Description:  
 *   Added regions.
 * Changed member variables to follow coding guidelines.
 * Changed member variable nSECTabPixels to m_TabPixels.
 * Changed SEC to Config in method names, parameter names, and comments.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.0   23 Apr 2013 10:03:00   bWalter
 * Project:  SVObserver
 * Change Request (SCR) nbr:  814
 * SCR Title:  Upgrade SVObserver to Compile Using Visual Studio 2010
 * Checked in by:  bWalter;  Ben Walter
 * Change Description:  
 *   Initial check in to SVObserver_src.  (Merged with svo_src label SVO 6.10 Beta 008.)
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.12   11 Feb 2013 14:16:42   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  812
 * SCR Title:  Add New Remote Command Functionality
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated source code to use the new framework for the remote socket interface.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.11   19 Apr 2011 16:07:16   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  707
 * SCR Title:  Change Inspection Display Functionality to Force Display of Last Inspected
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated code to fix issues with IO Assignment.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.10   18 Apr 2011 09:22:12   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  707
 * SCR Title:  Change Inspection Display Functionality to Force Display of Last Inspected
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated code to move the shared data for the IO Entry data to a different class to fix issues with PPQ and Inspection shared data.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.9   08 Dec 2010 07:51:16   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  707
 * SCR Title:  Change Inspection Display Functionality to Force Display of Last Inspected
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated source code to include changes in notification functionality using the Observer Design Pattern.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.8   14 Nov 2008 13:56:38   tbair
 * Project:  SVObserver
 * Change Request (SCR) nbr:  615
 * SCR Title:  Integrate PLC Classes into SVObserver Outputs
 * Checked in by:  tBair;  Tom Bair
 * Change Description:  
 *   Added IOView Tabs
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.7   08 Oct 2003 16:23:02   rschock
 * Project:  SVObserver
 * Change Request (SCR) nbr:  385
 * SCR Title:  Expose additional parameters for SIAC
 * Checked in by:  rYoho;  Rob Yoho
 * Change Description:  
 *   Made more changes to the line analyzers to support printing and correct use of the edge related value objects.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.6   27 Aug 2003 10:19:32   ebeyeler
 * Project:  SVObserver
 * Change Request (SCR) nbr:  320
 * SCR Title:  Integrate Matrox Meteor II / 1394 Board and 1394 camera into SVObserver
 * Checked in by:  eBeyeler;  Eric Beyeler
 * Change Description:  
 *   cleaned up file
 * added friend class SVDeviceParamConfigPrintHelper
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.5   17 Apr 2003 17:25:02   rschock
 * Project:  SVObserver
 * Change Request (SCR) nbr:  346
 * SCR Title:  Update SVObserver to Version 4.21 Release
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Redid the #include defines and standardized the Tracker log headers and removed warning from release mode builds.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.4   13 Nov 2002 11:53:04   cschmittinger
 * Project:  SVObserver
 * Change Request (SCR) nbr:  256
 * SCR Title:  Improvements to SVObserver's Configuration Report layout.
 * Checked in by:  cSchmittinger;  Carl Schmittinger
 * Change Description:  
 *   The whole report layout was changed.
 * This is a complete restructuring of SVConfigurationPrint.
 * New sections were added.
 * Some of the additions to the report make reference to the following SCR's:
 * SVObserver #226
 * SVObserver #254
 * SVObserver #272 
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.3   Apr 17 2001 15:30:46   steve
 * Project:  SVObserver
 * Change Request(SCR) nbr:  196
 * SCR Title:  Restructure Scripted Load/Save Procedures Functional Requirement
 * Checked in by:  Steve;  Stephen S. Steffan
 * Change Description:  
 *   Header file chages due to restructuring of include files.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.2   01 Feb 2001 11:55:08   Steve
 * Project:  SVObserver
 * Change Request(SCR) nbr:  191
 * SCR Title:  Restructure File Management
 * Checked in by:  Steve;  Stephen S. Steffan
 * Change Description:  
 *   Modified files to use the SVFileNameClass and the SVFileNameManagerClass if applicable, if the object refers to a filename.  The SVFileNameManagerClass is used if the Object is a persistant file object, that is, if the file object is saved as part of the configuration data.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.1   24 Feb 2000 17:12:50   sMarakkath
 * Project:  SVObserver
 * Change Request(SCR) nbr:  110
 * SCR Title:  Change Configuration Printout to print Tool "extents".
 * Checked in by:  sMarakkath;  Srithaj Marakkath
 * Change Description:  
 *    Modified SV_PRINT_LINES_PER_PAGE, Added the variable 'nSECTabShiftPixels' and  Modified function declaration 'PrintString'.
 * 
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.1   feb 24 2000 15:31:30   Sri
 * Project:  SVObserver
 * Change Request(SCR) nbr:  110
 * SCR Title:  Change Configuration Printout to print Tool "extents".
 * Checked in by:  Sri;  Srithaj Marakkath
 * Change Description:  
 *    Modified SV_PRINT_LINES_PER_PAGE, Added the variable 'nSECTabShiftPixels' and 
 *	  Modified function declaration 'PrintString'.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
$Log:   N:\PVCSarch65\ProjectFiles\archives\SVO_SRC\SVConfigurationPrint.h_v  $
 * 
 *    Rev 1.0   Jan 04 2000 09:21:18   Nick
 * Project:  SVObserver
 * Change Request(SCR) nbr:  82
 * SCR Title:  Configuration Printing
 * Checked in by:  Nick;  F Roland "Nick" Bjorklund
 * Change Description:  
 *   New class to encapsulate the configuration printing function.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
*/
