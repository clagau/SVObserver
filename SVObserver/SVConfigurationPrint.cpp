//******************************************************************************
//* COPYRIGHT (c) 2003 by SVResearch, Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVConfigurationPrint
//* .File Name       : $Workfile:   SVConfigurationPrint.cpp  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.16  $
//* .Check In Date   : $Date:   06 Jan 2015 10:21:34  $
//******************************************************************************

#pragma region Includes
#include "stdafx.h"
#include <boost/algorithm/string/replace.hpp>
#include "SVConfigurationPrint.h"
#include "SVConfigurationObject.h"

#include "SVIOLibrary/SVIOConfigurationInterfaceClass.h"
#include "SVOMFCLibrary/SVDeviceParams.h"
#include "SVOMFCLibrary/SVBoolValueDeviceParam.h"
#include "SVOMFCLibrary/SVi64ValueDeviceParam.h"
#include "SVOMFCLibrary/SVLongValueDeviceParam.h"
#include "SVOMFCLibrary/SVParamListDeviceParam.h"
#include "SVOMFCLibrary/SVStringValueDeviceParam.h"
#include "SVOMFCLibrary/SVCustomDeviceParam.h"
#include "SVOMFCLibrary/SVFileAcquisitionLoadingModeEnum.h"
#include "SVImageLibrary/SVImagingDeviceParams.h"
#include "SVObjectLibrary/SVObjectClass.h"

#include "SVValueObject.h"
#include "SVEdge.h"
#include "SVEquation.h"
#include "SVImageClass.h"
#include "SVLine.h"	// Sri 2/21/00
#include "SVLineAnalyzer.h"	// Sri 2/21/00
#include "SVObserver.h"
#include "SVTool.h"
#include "SVToolSet.h"
#include "SVInputObjectList.h"
#include "SVDigitalInputObject1.h"
#include "SVDigitalOutputObject1.h"
#include "SVOCRParamStruct.h"
#include "SVOCRGrayParamStruct.h"
#include "SVArchiveTool.h"
#include "SVStatTool.h"
#include "SVObjectLibrary/SVObjectManagerClass.h"
#include "SVAcquisitionClass.h"
#include "SVFileNameArrayClass.h"
#include "SVImageLibrary/SVLightReference.h"
#include "SVIODoc.h"
#include "SVTriggerClass.h"
#include "SVIPDoc.h"
#include "SVShapeMaskHelperClass.h"
#include "SVIOController.h"
#include "SVUserMaskOperatorClass.h"
#include "SVBlobAnalyzer.h"
#include "SVResultDouble.h"
#include "RemoteMonitorList.h"
#include "RemoteMonitorListHelper.h"
#include "SVSystemLibrary/SVThreadManager.h"
#include "ErrorNumbers.h"
#include "SVStatusLibrary/ExceptionManager.h"
#include "TextDefinesSvO.h"
#pragma endregion Includes

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

static const int LEFT_MARGIN = 50;

static GUID* pguidNonPrintArray[] = 
{
	&SVTaskObjectClassIsObjectValidGuid,
	&SVStatusObjectGuid,
	&SVColorObjectGuid,
	&SVConditionalResultObjectGuid,
	&SVFailedObjectGuid,
	&SVWarnedObjectGuid,
	&SVPassedObjectGuid,
	&SVExplicitFailedObjectGuid,
	&SVPassedCountObjectGuid,
	&SVFailedCountObjectGuid,
	&SVWarnedCountObjectGuid,
	&SVEdgeCountObjectGuid,
	&SVEnabledCountObjectGuid,
	&SVProcessedCountObjectGuid,
	&SVBlobAreaObjectGuid,
	&SVBlobBoxXMaxObjectGuid,
	&SVBlobBoxXMinObjectGuid,
	&SVBlobBoxYMaxObjectGuid,
	&SVBlobBoxYMinObjectGuid,
	&SVBlobBreadthObjectGuid,
	&SVBlobCenterOfGravityXObjectGuid,
	&SVBlobCenterOfGravityYObjectGuid,
	&SVBlobConvexPerimeterObjectGuid,
	&SVBlobFeretElongationObjectGuid,
	&SVBlobFeretMaxAngleObjectGuid,
	&SVBlobFeretMaxDiameterObjectGuid,
	&SVBlobFeretMeanDiameterObjectGuid,
	&SVBlobFeretMinAngleObjectGuid,
	&SVBlobFeretMinDiameterObjectGuid,
	&SVBlobFeretXObjectGuid,
	&SVBlobFeretYObjectGuid,
	&SVBlobFirstPointXObjectGuid,
	&SVBlobFirstPointYObjectGuid,
	&SVBlobLabelObjectGuid,
	&SVBlobLengthObjectGuid,
	&SVBlobNbrOfHolesObjectGuid,
	&SVBlobPerimeterObjectGuid,
	&SVBlobRoughnessObjectGuid,
	&SVBlobSumPixelObjectGuid,
	&SVBlobCompactnessObjectGuid,              // v3.1 10 Feb 2000 - frb.
	&SVBlobNumberOfRunsObjectGuid,
	&SVBlobXMinAtYMinObjectGuid,
	&SVBlobXMaxAtYMaxObjectGuid,
	&SVBlobYMinAtXMaxObjectGuid,
	&SVBlobYMaxAtXMinObjectGuid,
	&SVBlobElongationObjectGuid,
	&SVBlobIntercept0ObjectGuid,
	&SVBlobIntercept45ObjectGuid,
	&SVBlobIntercept90ObjectGuid,
	&SVBlobIntercept135ObjectGuid,
	&SVBlobMomentX0Y1,
	&SVBlobMomentX1Y0,
	&SVBlobMomentX1Y1,
	&SVBlobMomentX0Y2,
	&SVBlobMomentX2Y0,
	&SVBlobMomentCentralX0Y2,
	&SVBlobMomentCentralX2Y0,
	&SVBlobMomentCentralX1Y1,
	&SVBlobAxisPrincipalAngle,
	&SVBlobAxisSecondaryAngle,
	&SVBlobEulerNumber,
	&SVBlobCenterOfBoundingBoxYGuid,
	&SVBlobCenterOfBoundingBoxXGuid,
	&SVOCVCharacterResultClassGuid
};

std::pair<GUID **, size_t> NonPrintGuids()
{
	return std::make_pair(pguidNonPrintArray, sizeof(pguidNonPrintArray)/sizeof(GUID *));
}

class SVDeviceParamConfigPrintHelper :
	public Loki::BaseVisitor,
	public Loki::Visitor<SVDeviceParam>,
	public Loki::Visitor<SVLongValueDeviceParam>,
	public Loki::Visitor<SVi64ValueDeviceParam>,
	public Loki::Visitor<SVBoolValueDeviceParam>,
	public Loki::Visitor<SVStringValueDeviceParam>,
	public Loki::Visitor<SVParamListDeviceParam>,
	public Loki::Visitor<SVLutDeviceParam>,
	public Loki::Visitor<SVLightReferenceDeviceParam>,
	public Loki::Visitor<SVCameraFormatsDeviceParam>,
	public Loki::Visitor<SVCustomDeviceParam>
{
public:
	SVDeviceParamConfigPrintHelper(SVConfigurationPrint* pPrint, SVDeviceParamCollection& rCamFileParams, CDC* pDC, CPoint& ptCurPos, int nIndentLevel);
	HRESULT Visit(SVDeviceParam&);
	HRESULT Visit(SVLongValueDeviceParam&);
	HRESULT Visit(SVi64ValueDeviceParam&);
	HRESULT Visit(SVBoolValueDeviceParam&);
	HRESULT Visit(SVStringValueDeviceParam&);
	HRESULT Visit(SVParamListDeviceParam&);
	HRESULT Visit(SVLutDeviceParam&);
	HRESULT Visit(SVLightReferenceDeviceParam&);
	HRESULT Visit(SVCameraFormatsDeviceParam&);
	HRESULT Visit(SVCustomDeviceParam&);

private:
	SVConfigurationPrint* m_pPrint;
	SVDeviceParamCollection& m_rCamFileParams;
	CDC* m_pDC;
	CPoint& m_rptCurPos;
	int m_nIndentLevel;
};

static SVToolGrouping GetToolGroupings(const SVGUID& rInspectionGuid)
{
	// get the document that owns this inspection
	SVIPDoc* pDoc = SVObjectManagerClass::Instance().GetIPDoc(rInspectionGuid);
	if (pDoc)
	{
		return pDoc->GetToolGroupings();
	}
	return SVToolGrouping();
}

static SVObjectClass* GetTool(const SVString& rName, const SVTaskObjectListClass& rToolSet)
{
	SVObjectClass* pObject(nullptr);
	for (int i = 0; !pObject && i < rToolSet.GetSize(); i++)
	{
		if (rToolSet.GetAt(i)->GetName() == rName) 
		{
			pObject = rToolSet.GetAt(i);
		}
	}
	return pObject;
}

/////////////////////////////////////////////////////////////////////////////
// SVConfigurationPrint

IMPLEMENT_DYNCREATE(SVConfigurationPrint, CCmdTarget)

#pragma region Constructor
SVConfigurationPrint::SVConfigurationPrint()
{
	m_NPArraySize            = 0;
	m_toolNumber             = 0;
	m_heightPageNumberPixels = 0;
	m_shortTabPixels         = 50;
	m_pageCenter             = 0;
	
	m_isRealPrint = FALSE;
	
	m_isPrintToStringBuffer = false;
	m_psBuffer = NULL;
	
	// Set SVFileNameClass for Print to File
	CString strDefExt(MAKEINTRESOURCE(AFX_IDS_PRINTDEFAULTEXT));
	
	CString strPrintDef(MAKEINTRESOURCE(AFX_IDS_PRINTDEFAULT));
	
	CString strFilter(MAKEINTRESOURCE(AFX_IDS_PRINTFILTER));
	
	CString strTitle(MAKEINTRESOURCE(AFX_IDS_PRINTCAPTION));
	
	m_svfnFileName.SetDefaultFileExtension(strDefExt);
	
	m_svfnFileName.SetDefaultFileName(strPrintDef);
	
	m_svfnFileName.SetFileSaveFlags(OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT);
	
	m_svfnFileName.SetFileExtensionFilterList(strFilter);
	
	m_svfnFileName.SetFileSaveDialogTitle(strTitle);
}

SVConfigurationPrint::~SVConfigurationPrint()
{
}
#pragma endregion Constructor

BEGIN_MESSAGE_MAP(SVConfigurationPrint, CCmdTarget)
	//{{AFX_MSG_MAP(SVConfigurationPrint)
		// NOTE - the ClassWizard will add and remove mapping macros here.
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

UINT AFXAPI AfxGetFileTitle(LPCTSTR lpszPathName, LPTSTR lpszTitle, UINT nMax);
BOOL CALLBACK _AfxAbortProc(HDC, int);


/////////////////////////////////////////////////////////////////////////////
//
//
//
//
class CPrintingDialog : public CDialog
{
public:
	//{{AFX_DATA(CPrintingDialog)
	enum { IDD = AFX_IDD_PRINTDLG };
	//}}AFX_DATA
	CPrintingDialog(CWnd* pParent)
	{
#ifdef _MAC
		// Note! set m_pView *before* CDialog::Create so that
		// CPrintingDialog::OnInitDialog can use it.
		m_pView = pParent;
#endif
		Create(CPrintingDialog::IDD, pParent);      // modeless !
		_afxWinState->m_bUserAbort = FALSE;
	}
	virtual ~CPrintingDialog() { }
	
	virtual BOOL OnInitDialog();
	virtual void OnCancel();
	
protected:
#ifdef _MAC
	CWnd*   m_pView;        // the view being printed
	
	afx_msg int OnMouseActivate(CWnd* pDesktopWnd, UINT nHitTest, UINT message);
#endif
	
#ifdef _MAC
	//{{AFX_MSG(CPrintingDialog)
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
#endif
};



/////////////////////////////////////////////////////////////////////////////
//
//
//
void SVConfigurationPrint::DoPrintConfig()
{
    SVObserverApp* pApp = dynamic_cast <SVObserverApp*> (AfxGetApp());
	
	ASSERT(m_printInfo.m_pPD != NULL);    // must be set
	
	_AFX_THREAD_STATE* pThreadState = AfxGetThreadState();
	
	if (pThreadState && LOWORD(pThreadState->m_lastSentMsg.wParam) == ID_FILE_PRINT_DIRECT)
	{
		CCommandLineInfo*    pCmdInfo = pApp->m_pCmdInfo;
		
		if (pCmdInfo != NULL)
		{
			if (pCmdInfo->m_nShellCommand == CCommandLineInfo::FilePrintTo)
			{
				m_printInfo.m_pPD->m_pd.hDC = ::CreateDC(pCmdInfo->m_strDriverName,
													   pCmdInfo->m_strPrinterName, 
													   pCmdInfo->m_strPortName,
													   NULL);
				
				if (m_printInfo.m_pPD->m_pd.hDC == NULL)
				{
					AfxMessageBox(AFX_IDP_FAILED_TO_START_PRINT);
					
					return;
				}  // end if( printInfo.m_pPD->m_pd.hDC == NULL )
			}  // end if( pCmdInfo->m_nShellCommand == CCommandLineInfo::FilePrintTo )
		}  // end if( pCmdInfo != NULL )
		
		m_printInfo.m_bDirect = TRUE;
	}  // end if(   pThreadState && ...
	
	if (DoPreparePrinting(&m_printInfo))
	{
		// hDC must be set (did you remember to call DoPreparePrinting?)
		ASSERT(m_printInfo.m_pPD->m_pd.hDC != NULL);
		
		// gather file to print to if print-to-file selected
		CString strOutput;
		
		if (m_printInfo.m_pPD->m_pd.Flags & PD_PRINTTOFILE)
		{
			// begin SES 11-Jan-2001
			// Use SVFileNameClass for browsing
			if (m_svfnFileName.SaveFile() != IDOK)
				return;
			
			// set output device to resulting path name
			strOutput = m_svfnFileName.GetFileName();
			// end SES 11-Jan-2001
		}  // end if( printInfo.m_pPD->m_pd.Flags & PD_PRINTTOFILE )
		
		// Special...
		// Set up doc info an start the config printing process...
		CString strTitle = pApp->getConfigFileName();
		
		if (strTitle.GetLength() > 31)
			strTitle.ReleaseBuffer(31);
		
		DOCINFO docInfo;
		
		// Init docInfo to 0...
		memset(&docInfo, 0, sizeof(DOCINFO));
		
		// Init docInfo members...
		docInfo.cbSize = sizeof(DOCINFO);
		docInfo.lpszDocName = strTitle;

		CString strPortName;
		int nFormatID;
		
		if (strOutput.IsEmpty())
		{
			docInfo.lpszOutput = NULL;
			
			strPortName = m_printInfo.m_pPD->GetPortName();
			
			nFormatID = AFX_IDS_PRINTONPORT;
		}  // end if( strOutput.IsEmpty() )
		else
		{
			docInfo.lpszOutput = strOutput;
			
			AfxGetFileTitle(strOutput, strPortName.GetBuffer(_MAX_PATH), _MAX_PATH);
			
			nFormatID = AFX_IDS_PRINTTOFILE;
		}  // end else [if( strOutput.IsEmpty() )]
		
		m_printDC.Attach(m_printInfo.m_pPD->m_pd.hDC);  // attach printer dc
		
		m_printDC.m_bPrinting = TRUE;
		
		// Call on begin printing...
		OnBeginPrinting();
		
		m_printDC.SetAbortProc(_AfxAbortProc);
		
		// disable main window while printing & init printing status dialog
		AfxGetMainWnd()->EnableWindow(FALSE);
		
		CPrintingDialog   dlgPrintStatus(AfxGetMainWnd());
		
		CString           strTemp;
		
		dlgPrintStatus.SetDlgItemText(AFX_IDC_PRINT_DOCNAME, strTitle);
		dlgPrintStatus.SetDlgItemText(AFX_IDC_PRINT_PRINTERNAME, m_printInfo.m_pPD->GetDeviceName());
		AfxFormatString1(strTemp, nFormatID, strPortName);
		dlgPrintStatus.SetDlgItemText(AFX_IDC_PRINT_PORTNAME, strTemp);
		dlgPrintStatus.ShowWindow(SW_SHOW);
		dlgPrintStatus.UpdateWindow();
		
		// Start config printing process...
		
		// Check for docInfo...
		if (m_printDC.StartDoc(&docInfo) == SP_ERROR)
		{
			// Enable main window before proceeding...
			AfxGetMainWnd()->EnableWindow(TRUE);
			
			// Cleanup and show error message...
			OnEndPrinting();
			
			dlgPrintStatus.DestroyWindow();
			
			m_printDC.Detach();   // will be cleaned up by CPrintInfo destructor
			
			AfxMessageBox(AFX_IDP_FAILED_TO_START_PRINT);
			
			return;
		}  // end if( printDC.StartDoc( &docInfo ) == SP_ERROR )
		
		// Guarantee values are in the valid range
		UINT     nEndPage    = m_printInfo.GetToPage();
		UINT     nStartPage  = m_printInfo.GetFromPage();
		
		if (nEndPage < m_printInfo.GetMinPage())
		{
			nEndPage = m_printInfo.GetMinPage();
		}
		
		if (nEndPage > m_printInfo.GetMaxPage())
		{
			nEndPage = m_printInfo.GetMaxPage();
		}
		
		if (nStartPage < m_printInfo.GetMinPage())
		{
			nStartPage = m_printInfo.GetMinPage();
		}
		
		if (nStartPage > m_printInfo.GetMaxPage())
		{
			nStartPage = m_printInfo.GetMaxPage();
		}
		
		int   nStep    = (nEndPage >= nStartPage) ? 1 : -1;
		
		nEndPage = (nEndPage == 0xffff) ? 0xffff : nEndPage + nStep;
		
		VERIFY(strTemp.LoadString(AFX_IDS_PRINTPAGENUM));
		
		// begin page printing loop
		BOOL  bError = FALSE;
		
        for (m_printInfo.m_nCurPage = nStartPage;m_printInfo.m_nCurPage != nEndPage;m_printInfo.m_nCurPage += nStep)
		{
			OnPrepareDC();
			
			// check for end of print
			if (! m_printInfo.m_bContinuePrinting)
			{
				break;
			}
			// write current page
			TCHAR szBuf[80];
			wsprintf(szBuf, strTemp, m_printInfo.m_nCurPage);
			dlgPrintStatus.SetDlgItemText(AFX_IDC_PRINT_PAGENUM, szBuf);
			
			// set up drawing rect to entire page (in logical coordinates)
			m_printInfo.m_rectDraw.SetRect(0, 0, m_printDC.GetDeviceCaps(HORZRES), m_printDC.GetDeviceCaps(VERTRES));
			m_printDC.DPtoLP(&m_printInfo.m_rectDraw);
			
			// attempt to start the current page
			if (m_printDC.StartPage() < 0)
			{
				bError = TRUE;
				break;
			}  // end if( printDC.StartPage() < 0 )
			
			OnPrepareDC();
			
			ASSERT(m_printInfo.m_bContinuePrinting);
			
			// page successfully started, so now render the page
			PrintPage();
			
			if (m_printDC.EndPage() < 0 || ! _AfxAbortProc(m_printDC.m_hDC, 0))
			{
				bError = TRUE;
				break;
			}  // end if( printDC.EndPage() < 0 || ! _AfxAbortProc( printDC.m_hDC, 0 ) )
		}  // end for(  printInfo.m_nCurPage = nStartPage; ...
		
		// cleanup document printing process
		if (! bError)
			m_printDC.EndDoc();
		else
			m_printDC.AbortDoc();
		
		AfxGetMainWnd()->EnableWindow();          // enable main window
		
		OnEndPrinting();    // clean up after printing
		
		dlgPrintStatus.DestroyWindow();
		
		m_printDC.Detach();   // will be cleaned up by CPrintInfo destructor
	}  // if( OnPreparePrinting( &printInfo ) )
}  // end function void SVConfigurationPrint::DoPrintConfig

void SVConfigurationPrint::printConfigToStringBuffer(CString& rsBuffer)
{
	m_isPrintToStringBuffer = true;
	m_psBuffer = &rsBuffer;
	
	// create a dummy DC
	CWnd wnd;
	CRect rect(0, 0, 10, 10);
	BOOL bResult = wnd.CreateEx(0, _T("Button"), _T(""), 0, rect, NULL, 0);
	ASSERT(bResult);
	HDC hDC = wnd.GetDC()->GetSafeHdc();
	m_printDC.Attach(hDC);
	
	// print
	OnBeginPrinting();
    
	m_printDC.Detach();
}

void SVConfigurationPrint::printConfigToStringBuffer(SVString& rsBuffer)
{
	CString l_TempString;

	printConfigToStringBuffer( l_TempString );

	rsBuffer = static_cast< LPCTSTR >( l_TempString );
}

void SVConfigurationPrint::printConfigToStringBuffer(std::string& rsBuffer)
{
	CString l_TempString;

	printConfigToStringBuffer( l_TempString );

	rsBuffer = static_cast< LPCTSTR >( l_TempString );
}

BOOL SVConfigurationPrint::DoPreparePrinting(CPrintInfo* pPrintInfo)
{
    SVObserverApp* pApp = dynamic_cast <SVObserverApp*> (AfxGetApp());
	ASSERT(pPrintInfo != NULL);
	ASSERT(pPrintInfo->m_pPD != NULL);
	
	if (pPrintInfo->m_pPD->m_pd.nMinPage > pPrintInfo->m_pPD->m_pd.nMaxPage)
		pPrintInfo->m_pPD->m_pd.nMaxPage = pPrintInfo->m_pPD->m_pd.nMinPage;
	
	// don't prompt the user if we're doing print preview, printing directly,
	// or printing via IPrint and have been instructed not to ask
	
	if (pPrintInfo->m_bPreview || pPrintInfo->m_bDirect ||
		(pPrintInfo->m_bDocObject && !(pPrintInfo->m_dwFlags & PRINTFLAG_PROMPTUSER)))
	{
		if (pPrintInfo->m_pPD->m_pd.hDC == NULL)
		{
			// if no printer set then, get default printer DC and create DC without calling
			//   print dialog.
			if (! pApp->GetPrinterDeviceDefaults(&pPrintInfo->m_pPD->m_pd))
			{
				// bring up dialog to alert the user they need to install a printer.
				if (! pPrintInfo->m_bDocObject || 
					(pPrintInfo->m_dwFlags & PRINTFLAG_MAYBOTHERUSER))
					if (pApp->DoPrintDialog(pPrintInfo->m_pPD) != IDOK)
						return FALSE;
			}
			
			if (pPrintInfo->m_pPD->m_pd.hDC == NULL)
			{
				// call CreatePrinterDC if DC was not created by above
				if (pPrintInfo->m_pPD->CreatePrinterDC() == NULL)
					return FALSE;
			}
		}
		
		// set up From and To page range from Min and Max
		pPrintInfo->m_pPD->m_pd.nFromPage = (WORD) pPrintInfo->GetMinPage();
		pPrintInfo->m_pPD->m_pd.nToPage   = (WORD) pPrintInfo->GetMaxPage();
	}
	else
	{
		// otherwise, bring up the print dialog and allow user to change things
		// preset From-To range same as Min-Max range
		pPrintInfo->m_pPD->m_pd.nFromPage = (WORD) pPrintInfo->GetMinPage();
		pPrintInfo->m_pPD->m_pd.nToPage = (WORD) pPrintInfo->GetMaxPage();
		
		if (pApp->DoPrintDialog(pPrintInfo->m_pPD) != IDOK)
			return FALSE;       // do not print
	}
	
	ASSERT(pPrintInfo->m_pPD != NULL);
	
	ASSERT(pPrintInfo->m_pPD->m_pd.hDC != NULL);
	
	if (pPrintInfo->m_pPD->m_pd.hDC == NULL)
		return FALSE;
	
	pPrintInfo->m_nNumPreviewPages = pApp->m_nNumPreviewPages;
	
	VERIFY(pPrintInfo->m_strPageDesc.LoadString(AFX_IDS_PREVIEWPAGEDESC));
	
	return TRUE;
}

////////////////////////////////////////////////////////////////////////////////
// .Title       : SVConfigurationPrint:::PrintObject
// -----------------------------------------------------------------------------
// .Description : This function prints the specific object, friend objects, and 
//              :    child object information for the input parameter: pObj.
//              : pObj may be any SVObjectClass object.
// -----------------------------------------------------------------------------
// .Input(s)
//	   Type				Name				   Description
// :  CDC*           pDC               Pointer to Printer Device Context
// :  SVObjectClass* pObj              Pointer to an SVObjectClass to print.
// :  CPoint&        ptCurPos          Reference to the current print position in
// :                                   CPoint format.  Modified by this function.
// :  int            nIndentLevel
// .Output(s)
// :  CPoint&        ptCurPos          Reference to the current print position in
// :                                   CPoint format.  Modified by this function.
// :
// .Return Value
//	:  none
// -----------------------------------------------------------------------------
// .Import Function Reference(s)
//	:
// -----------------------------------------------------------------------------
// .Import Variable Reference(s)
//	:
////////////////////////////////////////////////////////////////////////////////
// .History
//	   Date		      Author		Comment                                       
// :  26-Oct-1999    SES         First Implementation
// :  12-Nov-2002    CHS         Changed Method Parameters
//	:
////////////////////////////////////////////////////////////////////////////////
void SVConfigurationPrint::PrintObject( CDC* pDC, SVObjectClass* pObj, CPoint& ptCurPos, int nIndentLevel )
{
	PrintDetails(pDC, pObj, ptCurPos, nIndentLevel);
	PrintFriends(pDC, pObj, ptCurPos, nIndentLevel + 1);
	PrintChildren(pDC, pObj, ptCurPos, nIndentLevel + 1);
}

////////////////////////////////////////////////////////////////////////////////
// .Title       : SVConfigurationPrint:::PrintDetails
// -----------------------------------------------------------------------------
// .Description : This function prints the specific object information for the 
//              : input parameter: pObj.
//              : pObj may be any SVObjectClass object.
// -----------------------------------------------------------------------------
// .Input(s)
//	   Type				Name				   Description
// :  CDC*           pDC               Pointer to Printer Device Context
// :  SVObjectClass* pObj              Pointer to an SVObjectClass to print.
// :  CPoint&        ptCurPos          Reference to the current print position in
// :                                   CPoint format.  Modified by this function.
// :  int            nIndentLevel
// .Output(s)
// :  CPoint&        ptCurPos          Reference to the current print position in
// :                                   CPoint format.  Modified by this function.
// :
// .Return Value
//	:  none
// -----------------------------------------------------------------------------
// .Import Function Reference(s)
//	:
// -----------------------------------------------------------------------------
// .Import Variable Reference(s)
//	:
////////////////////////////////////////////////////////////////////////////////
// .History
//	   Date		      Author		Comment                                       
// :  26-Oct-1999    SES         First Implementation
// :  12-Nov-2002    CHS         Changed Method Parameters
// :
////////////////////////////////////////////////////////////////////////////////
void SVConfigurationPrint::PrintDetails( CDC* pDC, SVObjectClass* pObj, CPoint& ptCurPos, int nIndentLevel )
{
    SVObserverApp* pApp = dynamic_cast <SVObserverApp*> (AfxGetApp());
    int      nFirstHeight   = 0;
    int      nLastHeight    = 0;
	
	CString	sLabel, sValue;
    CString  strType   = pObj->GetObjectName();
    CString  strName   = pObj->GetName();
	
    CPoint   ptTemp(0, 0);
	
    GUID     guidObjID = pObj->GetClassID();
	
	BOOL	bPrintToolExtents = FALSE;		// Sri 2/17/00
	
    // If object is a value object, get embedded ID.
    if (SV_IS_KIND_OF(pObj, SVValueObjectClass))
    {
        guidObjID = pObj->GetEmbeddedID();
    }  // end if( SV_IS_KIND_OF( pObj, SVValueObjectClass ) )
	
    // Check for non-printing object type.
    for (int nIndex = 0; nIndex < m_NPArraySize; nIndex++)
    {
        // If the GUID is for a non-printing object, Skip printing.
        if (guidObjID == *pguidNonPrintArray[nIndex])
        {
            return;
        }  // end if ( guidObjID == *pguidNonPrintArray [nIndex] )
    }  // end for ( int nIndex = 0; nIndex < m_nNPArraySize; nIndex++ )
	
	// This is to prevent the comments from being sent to the SIAC thru SVGetSVIMConfigPrint
	if( guidObjID == SVToolCommentTypeObjectGuid && m_isPrintToStringBuffer )
	{
		return;
	}
	
	// If object is a value object, print name and value.
	if ( SVValueObjectClass* pValueObject = dynamic_cast<SVValueObjectClass*> (pObj) )
	{
		if ( pObj->ObjectAttributesAllowed() & SV_PRINTABLE )
		{
			sLabel = pValueObject->GetName();
			if (sLabel.Find(_T("OCR Gray Scale Parameters")) >= 0)
			{
				CString  strParameters;
				
				if (pValueObject->GetValue(strParameters) == S_OK )
					PrintOCRGrayScaleParameters(pDC, strParameters, ptCurPos, nIndentLevel);
			}
			else if (sLabel.Find(_T("OCR Parameters")) >= 0)
			{
				CString  strParameters;
				
				if (pValueObject->GetValue(strParameters) == S_OK )
					PrintOCRParameters(pDC, strParameters, ptCurPos, nIndentLevel);
			}
			else
			{
				BOOL bGotValue = FALSE;

				if ( SVDWordValueObjectClass* pdwValueObject = dynamic_cast <SVDWordValueObjectClass*> ( pValueObject ) )
				{
					DWORD dwValue=0;
					bGotValue = (pdwValueObject->GetValue( dwValue ) == S_OK );
					sValue = AsString( dwValue );
				}
				else
				{
					bGotValue = (pValueObject->GetValue( sValue ) == S_OK );
				}

				if ( bGotValue )
				{
					if (sValue.IsEmpty())
					{
						sValue = _T("< I N V A L I D ! >");
					}
				}
				else
				{
					sValue = _T("< I N V A L I D ! >");
				}
				
				ptCurPos.x   = (nIndentLevel + 1) * m_shortTabPixels;
				PrintValueObject(pDC, ptCurPos, sLabel, sValue);
			}  // end else
		}   // end if ( pObj->uObjectAttributesAllowed & SV_PRINTABLE )
	}  // end if( SV_IS_KIND_OF( pObj, SVValueObjectClass ) )
	else
	{
		do
		{
			if ( dynamic_cast <SVShapeMaskHelperClass*> (pObj) )
				if ( !( pObj->ObjectAttributesAllowed() & SV_PRINTABLE) )	// EB 20050818 - hack this instead of doing it right
					break;

			if ( SVToolClass* pTool = dynamic_cast<SVToolClass*> (pObj) )
			{
				// Increment even if disabled, to maintain count.  Starts with zero, so for first
				//    tool, will increment to 1.
				m_toolNumber++;
				
				sLabel.Format("Tool Number %d:", m_toolNumber);
				
				ptCurPos.x  = nIndentLevel * m_shortTabPixels;
				ptTemp      = ptCurPos;
				ptCurPos.y += PrintString(pDC, ptTemp, sLabel);
				ptCurPos.x  = ++nIndentLevel * m_shortTabPixels;
				
				bPrintToolExtents = TRUE;		// Sri 2/17/00
			}  // end if( SV_IS_KIND_OF( pObj, SVToolClass ) )
			
			ptCurPos.x   = nIndentLevel * m_shortTabPixels;
			PrintValueObject(pDC, ptCurPos, _T("Name:"), strName);
			
			// If the object type is different from the name, print the type.
			if (strName != strType && !strType.IsEmpty())
			{
				PrintValueObject(pDC, ptCurPos, _T("Type:"), strType);
			}
			
			// Print the tool length, width, extends, etc here.
			if (bPrintToolExtents && (SV_IS_KIND_OF(pObj, SVToolClass)))
			{
				bPrintToolExtents = FALSE;
				
				SVInputInfoListClass    toolInputList;
				SVImageClass*           pCurrentSourceImage = NULL;
				SVInObjectInfoStruct*   pImageInputInfo = NULL;
				SVToolClass*            pTool = dynamic_cast <SVToolClass*> (pObj);
				
				SVInObjectInfoStruct* l_psvImageInfo = NULL;
				SVInObjectInfoStruct* l_psvLastImageInfo = NULL;

				while( l_psvImageInfo == NULL && pTool->FindNextInputImageInfo( l_psvImageInfo, l_psvLastImageInfo ) == S_OK )
				{
					if( l_psvImageInfo != NULL )
					{
						pImageInputInfo = l_psvImageInfo;

						if( l_psvImageInfo->IsConnected() )
						{
							pCurrentSourceImage = dynamic_cast <SVImageClass*> (l_psvImageInfo->GetInputObjectInfo().PObject);

							PrintValueObject(pDC, ptCurPos, pApp->GetStringResource(IDS_IMAGE_SOURCE_STRING), pCurrentSourceImage->GetCompleteObjectNameToObjectType());
						}
					}
					else
					{
						l_psvLastImageInfo = l_psvImageInfo;
						l_psvImageInfo = NULL;
					}
				}

				SVImageInfoClass* pImageInfo = NULL;
				if (pObj->GetObjectName() != pApp->GetStringResource(IDS_CLASSNAME_SVGAGETOOL))
					pImageInfo = reinterpret_cast <SVImageInfoClass*> ( ::SVSendMessage(pTool, SVM_GETFIRST_IMAGE_INFO, NULL, NULL) );
				
				if (pImageInfo)
				{
					POINT l_oPoint;
					POINT l_oOutputPoint;

					long l_lWidth = 0;
					long l_lHeight = 0;

					if ( pImageInfo->GetExtentProperty( SVExtentPropertyPositionPoint, l_oPoint ) == S_OK &&
						 pImageInfo->GetExtentProperty( SVExtentPropertyOutputPositionPoint, l_oOutputPoint ) == S_OK &&
						 pImageInfo->GetExtentProperty( SVExtentPropertyWidth, l_lWidth ) == S_OK &&
						 pImageInfo->GetExtentProperty( SVExtentPropertyHeight, l_lHeight ) == S_OK )
					{
						sLabel = pApp->GetStringResource(IDS_TOOL_LENGTH_STRING) + _T(":");
						sValue.Format("%d", l_lWidth);
						ptCurPos.x   = nIndentLevel * m_shortTabPixels;
						PrintValueObject(pDC, ptCurPos, sLabel, sValue);
						
						sLabel = pApp->GetStringResource(IDS_TOOL_HEIGHT_STRING) + _T(":");
						sValue.Format("%d", l_lHeight);
						PrintValueObject(pDC, ptCurPos, sLabel, sValue);
						
						sLabel = pApp->GetStringResource(IDS_ABSOLUTE_POS_STRING) + _T(":");
						sValue.Format("(%d, %d)", l_oPoint.x, l_oPoint.y);
						PrintValueObject(pDC, ptCurPos, sLabel, sValue);
						
						sLabel = pApp->GetStringResource(IDS_RELATIVE_POS_STRING) + _T(":");
						sValue.Format("(%d, %d)", l_oOutputPoint.x, l_oOutputPoint.y);
						PrintValueObject(pDC, ptCurPos, sLabel, sValue);
					}
				} // End, if( pImageInfo )
			} // End, if(bPrintToolExtents && ( SV_IS_KIND_OF( pObj, SVToolClass )))
			
			if ( SVArchiveTool* pTool = dynamic_cast <SVArchiveTool*> (pObj) )
			{
				int i, s;
				SVArchiveRecord* pRecord;
				
				ptTemp      = ptCurPos;
				ptCurPos.y += PrintString(pDC, ptTemp, _T("Results"));
				ptCurPos.x  = ++nIndentLevel * m_shortTabPixels;
				s = pTool->m_arrayResultsInfoObjectsToArchive.GetSize();
				for (i = 0; i < s; i++)
				{
					pRecord = pTool->m_arrayResultsInfoObjectsToArchive.GetAt(i);
					if (pRecord->GetObjectReference().Object() != NULL)
					{
						ptCurPos.x   = nIndentLevel * m_shortTabPixels;
						CString sFormat;
						sFormat.Format(_T("Result %d:"), i + 1);
						PrintValueObject(pDC, ptCurPos, sFormat, pRecord->GetObjectReference().GetCompleteObjectName());
					}
				}
				
				ptCurPos.x  = --nIndentLevel * m_shortTabPixels;
				ptTemp      = ptCurPos;
				ptCurPos.y += PrintString(pDC, ptTemp, _T("Images"));
				
				ptCurPos.x  = ++nIndentLevel * m_shortTabPixels;
				s = pTool->m_arrayImagesInfoObjectsToArchive.GetSize();
				for (i = 0; i < s; i++)
				{
					pRecord = pTool->m_arrayImagesInfoObjectsToArchive.GetAt(i);
					ptCurPos.x   = nIndentLevel * m_shortTabPixels;
					CString sFormat;
					sFormat.Format(_T("Image %d:"), i + 1);
					PrintValueObject(pDC, ptCurPos, sFormat, pRecord->GetImageObjectName());
				}
				
				nIndentLevel -= 2;
				ptCurPos.x   = nIndentLevel * m_shortTabPixels;
			}// end if ( SVArchiveTool* pTool = dynamic_cast <SVArchiveTool*> (pObj) )
			
			if ( SVStatisticsToolClass* pTool = dynamic_cast<SVStatisticsToolClass*> (pObj) )
			{
				SVObjectReference refObject = pTool->GetVariableSelected();
				if (refObject.Object())
				{
					CString sName = refObject.GetName();
					ptCurPos.x   = nIndentLevel * m_shortTabPixels;
					PrintValueObject(pDC, ptCurPos, _T("Variable"), sName);
				}
			}  // end if( SV_IS_KIND_OF( pObj, SVStatisticsToolClass ) )

			if (SV_IS_KIND_OF(pObj, SVUserMaskOperatorClass))
			{
				SVUserMaskOperatorClass* maskObj = dynamic_cast <SVUserMaskOperatorClass*>( pObj );
				if ( NULL != maskObj )
				{
					SVImageClass* pImage = maskObj->getMaskInputImage();
					if (nullptr != pImage)
					{
						sLabel = pApp->GetStringResource(IDS_IMAGE_SOURCE_STRING) + _T(":");
						sValue.Format(_T("%s"), pImage->GetCompleteObjectName());
						ptCurPos.x   = (nIndentLevel + 1) * m_shortTabPixels;
						PrintValueObject(pDC, ptCurPos, sLabel, sValue);
					}
				}
			}

			if (SV_IS_KIND_OF(pObj,SVDoubleResultClass))
			{
				SVDoubleResultClass* pBlobResult = dynamic_cast<SVDoubleResultClass*>(pObj);
				if (pBlobResult)
				{
					if (SV_IS_KIND_OF(pBlobResult->GetOwner(),SVBlobAnalyzerClass))
					{  
						sLabel = pApp->GetStringResource(IDS_BLOB_FEATURE_DEFAULT_VALUE) + _T(":");
						SVDoubleValueObjectClass* pDoubleValueObj = pBlobResult->getInputDouble();
						if ( pDoubleValueObj )
						{
							double dVal;
							HRESULT hr = pDoubleValueObj->GetDefaultValue(dVal);
							sValue.Format(_T("%lf"),dVal);
							ptCurPos.x   = (nIndentLevel + 1) * m_shortTabPixels;
							PrintValueObject(pDC, ptCurPos, sLabel, sValue);
						}
					}
				}
			}

			if (SV_IS_KIND_OF(pObj, SVLineAnalyzerClass))
			{
				SVObjectInfoStruct objectInfo;
				objectInfo.ObjectTypeInfo.ObjectType = SVLineObjectType;
				
				SVLineClass* pLineClass = dynamic_cast <SVLineClass*> (reinterpret_cast<SVObjectClass*> (SVSendMessage( pObj, SVM_GETFIRST_OBJECT, reinterpret_cast<DWORD_PTR>( &objectInfo ), NULL) ) );
				
				POINT l_oHeadPoint;
				POINT l_oTailPoint;

				if ( pLineClass != NULL &&
					 pLineClass->GetHeadPoint( l_oHeadPoint ) == S_OK &&
					 pLineClass->GetHeadPoint( l_oTailPoint ) == S_OK )
				{
					sLabel = pApp->GetStringResource(IDS_HEAD_POINT_STRING) + _T(":");
					sValue.Format(_T("(%d, %d)"), (int)(l_oHeadPoint.x), (int)(l_oHeadPoint.y));
					ptCurPos.x   = (nIndentLevel + 1) * m_shortTabPixels;
					PrintValueObject(pDC, ptCurPos, sLabel, sValue);
					
					sLabel = pApp->GetStringResource(IDS_TAIL_POINT_STRING) + _T(":");
					sValue.Format(_T("(%d, %d)"), (int)(l_oTailPoint.x), (int)(l_oTailPoint.y));
					PrintValueObject(pDC, ptCurPos, sLabel, sValue);
				} // End, if(pLineClass)
				
			} // End if(SV_IS_KIND_OF( pObj, SVLineAnalyzerClass))
		} while (false);// end do
	}// End if( SV_IS_KIND_OF( pObj, SVValueObjectClass ) ) else

	if ( SVEquationClass* pEquation = dynamic_cast <SVEquationClass*> (pObj) )
	{
		pEquation->GetEquationText(sValue);
		
		// Print the equation text.
		if (sLabel.IsEmpty())
			sLabel = _T("Equation Text");
		ptCurPos.x  = nIndentLevel * m_shortTabPixels;
		PrintValueObject(pDC, ptCurPos, sLabel, sValue);
	}  // end if( SV_IS_KIND_OF( pObj, SVEquationClass ) )
	
	if (SV_IS_KIND_OF(pObj, SVTaskObjectClass))
	{
		PrintInputOutputList(pDC, pObj, ptCurPos, nIndentLevel);
	}  // end if( SV_IS_KIND_OF( pObj, SVTaskObjectClass )
}  // end function void SVConfigurationPrint:::PrintDetails( ... )

void SVConfigurationPrint::PrintAllChildren(CDC* pDC, SVTaskObjectListClass* pTaskObj,  CPoint& ptCurPos, int nIndentLevel)
{
	for (int nCnt = 0; nCnt < pTaskObj->GetSize(); nCnt++)
	{
		SVObjectClass* pChild = pTaskObj->GetAt(nCnt);
		if (pChild)
		{
			PrintObject(pDC, pChild, ptCurPos, nIndentLevel);
		}
	}
}

////////////////////////////////////////////////////////////////////////////////
// .Title       : SVConfigurationPrint:::PrintChildren
// -----------------------------------------------------------------------------
// .Description : This function prints the child object information for the 
//              : input parameter: pObj.
//              : pObj may be any SVObjectClass object.
// -----------------------------------------------------------------------------
// .Input(s)
//	   Type				Name				   Description
// :  CDC*           pDC              Pointer to Printer Device Context
// :  SVObjectClass* pObj             Pointer to an SVObjectClass to print.
// :  CPoint&        ptCurPos         Reference to the current print position in
// :                                  CPoint format.  Modified by this function.
// :  int            nIndentLevel
// .Output(s)
// :  CPoint&        ptCurPos         Reference to the current print position in
// :                                  CPoint format.  Modified by this function.
// :
// .Return Value
//	:  none
// -----------------------------------------------------------------------------
// .Import Function Reference(s)
//	:
// -----------------------------------------------------------------------------
// .Import Variable Reference(s)
//	:
////////////////////////////////////////////////////////////////////////////////
// .History
//	   Date		      Author		Comment                                       
// :  26-Oct-1999    SES         First Implementation
// :  12-Nov-2002    CHS         Changed Method Parameters
// :
////////////////////////////////////////////////////////////////////////////////
void SVConfigurationPrint::PrintChildren( CDC* pDC, SVObjectClass* pObj, CPoint& ptCurPos, int nIndentLevel )
{
	if ( SVTaskObjectListClass* pTaskObj = dynamic_cast <SVTaskObjectListClass*> (pObj) )
    {
		if (SVToolSetClass* pToolSet = dynamic_cast<SVToolSetClass*>(pObj))
		{
			SVToolGrouping& rToolGroupings = GetToolGroupings(pToolSet->GetInspection()->GetUniqueObjectID());
			if (rToolGroupings.size())
			{
				bool bToolGroupActive = false;
				for (SVToolGrouping::const_iterator it = rToolGroupings.begin();it != rToolGroupings.end();++it)
				{
					switch (it->second.m_type)
					{
						case ToolGroupData::StartOfGroup:
						{
							CString sLabel;
							sLabel.Format("Tool Grouping: %s", it->first.c_str());
							ptCurPos.x  = nIndentLevel * m_shortTabPixels;
							CPoint ptTemp = ptCurPos;
							ptCurPos.y += PrintString(pDC, ptTemp, sLabel);
						}
						break;

						case ToolGroupData::EndOfGroup:
						{
							CString sLabel;
							sLabel.Format("End Tool Grouping: %s", it->first.c_str());
							ptCurPos.x  = nIndentLevel * m_shortTabPixels;
							CPoint ptTemp = ptCurPos;
							ptCurPos.y += PrintString(pDC, ptTemp, sLabel);
						}
						break;

						case ToolGroupData::Tool:
						{
							SVObjectClass* pTool = GetTool(it->first.c_str(), *pTaskObj);
							if (pTool)
							{
								PrintObject(pDC, pTool, ptCurPos, nIndentLevel);
							}
						}
						break;
					}
				}
			}
			else
			{
				PrintAllChildren(pDC, pTaskObj,  ptCurPos, nIndentLevel);
			}
		}
		else
		{
			PrintAllChildren(pDC, pTaskObj,  ptCurPos, nIndentLevel);
		}
	}  // end if( SV_IS_KIND_OF( pObj, SVTaskObjectListClass ) )
}  // end function void SVConfigurationPrint:::PrintChildren( ... )

////////////////////////////////////////////////////////////////////////////////
// .Title       : SVConfigurationPrint:::PrintFriends
// -----------------------------------------------------------------------------
// .Description : This function prints the friend object information for the 
//              : input parameter: pObj.  pObj may be any SVObjectClass object.
// -----------------------------------------------------------------------------
// .Input(s)
//	   Type				Name				   Description
// :  CDC*           pDC               Pointer to Printer Device Context
// :  SVObjectClass* pObj              Pointer to an SVObjectClass to print.
// :  CPoint&        ptCurPos          Reference to the current print position in
// :                                   CPoint format.  Modified by this function.
// :  int            nIndentLevel
// .Output(s)
// :  CPoint&        ptCurPos          Reference to the current print position in
// :                                   CPoint format.  Modified by this function.
// :
// .Return Value
//	:  none
// -----------------------------------------------------------------------------
// .Import Function Reference(s)
//	:
// -----------------------------------------------------------------------------
// .Import Variable Reference(s)
//	:
////////////////////////////////////////////////////////////////////////////////
// .History
//	   Date		      Author		Comment                                       
// :  26-Oct-1999    SES         First Implementation
// :  12-Nov-2002    CHS         Changed Method Parameters
// :
////////////////////////////////////////////////////////////////////////////////
void SVConfigurationPrint::PrintFriends( CDC* pDC, SVObjectClass* pObj, CPoint& ptCurPos, int nIndentLevel )
{
	const SVObjectInfoArrayClass& rFriendList = pObj->GetFriendList();
	
	for (size_t nCnt = 0; nCnt < rFriendList.size(); nCnt++)
	{
		const SVObjectInfoStruct& rObjInfo = rFriendList[nCnt];
		
		if (rObjInfo.PObject)
		{
			PrintObject(pDC, rObjInfo.PObject, ptCurPos, nIndentLevel);
		}  // end if( rObjInfo )
	}  // end for( size_t nCnt = 0; nCnt < rFriendList.GetSize (); nCnt++ )
}  // end function void SVConfigurationPrint:::PrintFriends( ... )

////////////////////////////////////////////////////////////////////////////////
// .Title       : SVConfigurationPrint:::PrintInputOutputList
// -----------------------------------------------------------------------------
// .Description : This function prints the friend object information for the 
//              : input parameter: pObj.  pObj may be any SVObjectClass object.
// -----------------------------------------------------------------------------
// .Input(s)
//	   Type				   Name				   Description
// :  CDC*              pDC               Pointer to Printer Device Context
// :  CPrintInfo*       pPrintInfo        Pointer to the default CPrintInfo struct.
// :  SVObjectClass*    pObj              Pointer to an SVObjectClass to print.
// :  CPoint&           ptCurPos          Reference to the current print position in
// :                                      CPoint format.  Modified by this function.
// :  int            nIndentLevel
// .Output(s)
// :  CPoint&           ptCurPos          Reference to the current print position in
// :                                      CPoint format.  Modified by this function.
// :
// .Return Value
//	:  none
// -----------------------------------------------------------------------------
// .Import Function Reference(s)
//	:
// -----------------------------------------------------------------------------
// .Import Variable Reference(s)
//	:
////////////////////////////////////////////////////////////////////////////////
// .History
//	   Date		      Author		Comment                                       
// :  01-Nov-1999    SES         First Implementation
// :  12-Nov-2002    CHS         Changed Method Parameters
// :
////////////////////////////////////////////////////////////////////////////////
void SVConfigurationPrint::PrintInputOutputList( CDC* pDC, SVObjectClass* pObj, CPoint& ptCurPos, int nIndentLevel )
{
	SVTaskObjectClass*      pTaskObj = dynamic_cast <SVTaskObjectClass*> (pObj);

	SVOutputInfoListClass l_OutputList;
	pTaskObj->GetOutputList( l_OutputList );

	for (int nCnt = 0; nCnt < l_OutputList.GetSize(); nCnt++)
	{
		SVOutObjectInfoStruct* pOutput = l_OutputList.GetAt(nCnt);
		
		if (pOutput->PObject->GetOwner() == pObj)
		{
			PrintDetails(pDC, pOutput->PObject, ptCurPos, nIndentLevel);
		}  // end if( pOutput->PObject->GetOwner () == pObj )
	}  // end for( int nCnt = 0; nCnt < pOutputInfoList->GetSize(); nCnt++ )
}  // end function void SVConfigurationPrint:::PrintInputOutputList( ... )

void SVConfigurationPrint::PrintOCRParameters(CDC* pDC, CString strParameters, CPoint &ptCurPos, int nIndentLevel)
{
	SVObserverApp* pApp = dynamic_cast <SVObserverApp*> (AfxGetApp());
	int      nFirstHeight   = 0;
	int      nLastHeight    = 0;
	
	CString  sLabel, sValue;
	CPoint   ptTemp(0, 0);
	
	SVOCRParamStruct*	pOCRParam = new SVOCRParamStruct();
	
	if (pOCRParam && !strParameters.IsEmpty())
	{
		pOCRParam->ConvertStringToParams(strParameters);
		
		ptCurPos.x  = (nIndentLevel + 1) * m_shortTabPixels;
		ptTemp		= ptCurPos;
		ptCurPos.y += PrintString(pDC, ptTemp, _T("OCR Parameters:"));
		ptCurPos.x  = (nIndentLevel + 2) * m_shortTabPixels;
		
		sValue.Format(_T("%f"), pOCRParam->fFeaturesUsed);
		PrintValueObject(pDC, ptCurPos, _T("Features Used"), sValue);
		sValue.Format(_T("%f"), pOCRParam->fmaxAngle);
		PrintValueObject(pDC, ptCurPos, _T("Max Angle"), sValue);
		sValue.Format(_T("%f"), pOCRParam->fmaxScale);
		PrintValueObject(pDC, ptCurPos, _T("Max Scale"), sValue);
		sValue.Format(_T("%f"), pOCRParam->forientationSensitivity);
		PrintValueObject(pDC, ptCurPos, _T("Orientation Sensitivity"), sValue);
		sValue.Format(_T("%f"), pOCRParam->fthreshold);
		PrintValueObject(pDC, ptCurPos, _T("Threshold"), sValue);
		sValue.Format(_T("%s"), pOCRParam->ignoreNoMatch ? _T("Yes") : _T("No"));
		PrintValueObject(pDC, ptCurPos, _T("Ignore No Match"), sValue);
		sValue.Format(_T("(%d, %d)"), pOCRParam->minHeight, pOCRParam->maxHeight);
		PrintValueObject(pDC, ptCurPos, _T("Height (min, max)"), sValue);
		sValue.Format(_T("(%d, %d)"), pOCRParam->minWidth, pOCRParam->maxWidth);
		PrintValueObject(pDC, ptCurPos, _T("Width (min, max)"), sValue);
		sValue.Format(_T("(%d, %d)"), pOCRParam->nMinPixelsInBlob, pOCRParam->nMaxPixelsInBlob);
		PrintValueObject(pDC, ptCurPos, _T("Pixels in Blob (min, max)"), sValue);
		pOCRParam->STRmatchString;
		pOCRParam->STRnoMatchLabel;
	}
}

void SVConfigurationPrint::PrintOCRGrayScaleParameters(CDC* pDC, CString strParameters, CPoint &ptCurPos, int nIndentLevel)
{
	SVObserverApp* pApp = dynamic_cast <SVObserverApp*> (AfxGetApp());
	int      nFirstHeight   = 0;
	int      nLastHeight    = 0;
	
	CString  sLabel, sValue;
	CPoint   ptTemp(0, 0);
	
	SVOCRGrayParamStruct* pOCRGrayParam = new SVOCRGrayParamStruct();
	
	if (pOCRGrayParam && !strParameters.IsEmpty())
	{
		pOCRGrayParam->ConvertStringToParams(strParameters);
		
		ptCurPos.x  = (nIndentLevel + 1) * m_shortTabPixels;
		ptTemp		= ptCurPos;
		ptCurPos.y += PrintString(pDC, ptTemp, _T("OCR Gray Scale Parameters:"));
		ptCurPos.x  = (nIndentLevel + 2) * m_shortTabPixels;
		
		PrintValueObject(pDC, ptCurPos, _T("Match Label"), pOCRGrayParam->STRnoMatchLabel);
		PrintValueObject(pDC, ptCurPos, _T("Match String"), pOCRGrayParam->STRmatchString);
		
		sValue.Format(_T("%d"), pOCRGrayParam->useMatchFile);	// int
		PrintValueObject(pDC, ptCurPos, _T("Use Match File"), sValue);
		sValue.Format(_T("%d"), pOCRGrayParam->maxMatches);		// int
		PrintValueObject(pDC, ptCurPos, _T("Max Matches"), sValue);
		sValue.Format(_T("%d"), pOCRGrayParam->xVicinity);	// int
		PrintValueObject(pDC, ptCurPos, _T("X Vicinity"), sValue);
		sValue.Format(_T("%d"), pOCRGrayParam->yVicinity);	// int
		PrintValueObject(pDC, ptCurPos, _T("Y Vicinity"), sValue);
		sValue.Format(_T("%f"), pOCRGrayParam->rejectThresh);	// float
		PrintValueObject(pDC, ptCurPos, _T("Reject Threshold"), sValue);
		sValue.Format(_T("%f"), pOCRGrayParam->acceptThresh);	// float
		PrintValueObject(pDC, ptCurPos, _T("Accept Threshold"), sValue);
		sValue.Format(_T("%f"), pOCRGrayParam->relThresh);	// float
		PrintValueObject(pDC, ptCurPos, _T("Relative Threshold"), sValue);
		sValue.Format(_T("%f"), pOCRGrayParam->minContrast);	// float
		PrintValueObject(pDC, ptCurPos, _T("Min Contrast"), sValue);
		sValue.Format(_T("%d"), pOCRGrayParam->scale);	// int
		PrintValueObject(pDC, ptCurPos, _T("Scale"), sValue);
		sValue.Format(_T("%d"), pOCRGrayParam->maxCandidates);	// int
		PrintValueObject(pDC, ptCurPos, _T("Max Candidates"), sValue);
		sValue.Format(_T("%d"), pOCRGrayParam->candsInVicinity);	// int
		PrintValueObject(pDC, ptCurPos, _T("Candidates in Vicinity"), sValue);
		sValue.Format(_T("%f"), pOCRGrayParam->candidateThresh);	// float
		PrintValueObject(pDC, ptCurPos, _T("Candidate Threshold"), sValue);
		sValue.Format(_T("%f"), pOCRGrayParam->candidateRelThresh);	// float
		PrintValueObject(pDC, ptCurPos, _T("Candidate Relative Threshold"), sValue);
		sValue.Format(_T("%d"), pOCRGrayParam->output);	// int
		PrintValueObject(pDC, ptCurPos, _T("Output"), sValue);
	}
}

static CString ExpandTabs(LPCTSTR text)
{
	//Tabstop every 8 characters - replace tab with eight (8) spaces
	std::basic_string<TCHAR> tmp = text;
	boost::replace_all(tmp, _T("\t"), _T("        "));
	return tmp.c_str();
}

typedef std::deque<CString> WordWrapLines;

static WordWrapLines DoWordWrap(LPCTSTR text, int numChars)
{
	WordWrapLines lines;
	CString tmp(text);
	CString line;
	int cnt = 0;
	int len = tmp.GetLength();
	for (int i = 0 ;i < len;i++)
	{
		cnt++;
		if (tmp[i] == _T('\n'))
		{
			lines.push_back(_T(" "));
			cnt = 0;
			line.Empty();
		}
		else if (cnt > numChars)
		{
			lines.push_back(line);
			cnt = 1;
			line = tmp[i];
		}
		else
		{
			line += tmp[i];
		}
	}
	if (!line.IsEmpty())
	{
		lines.push_back(line);
	}
	return lines;
}

static void OutputTextString(CDC* pDC, LPCTSTR text, const CRect& rRect)
{
	// Get Height and Width of a single character ('W')
	CRect charRect(0, 0, 100, 100);
	CString testStr(_T("W"));
	CSize size = pDC->GetTextExtent(testStr);
	WordWrapLines lines = DoWordWrap(text, rRect.Width() / size.cx);
	
	CRect textRect(rRect);
	for (WordWrapLines::const_iterator it = lines.begin();it != lines.end();++it)
	{
		pDC->TextOut(textRect.left, textRect.top, (*it));
		textRect.top += size.cy;
	}
}

int SVConfigurationPrint::PrintString(CDC* pDC, CPoint& ptCurPos, LPCTSTR _lpszOutput)
{
	if (m_isPrintToStringBuffer)
	{
		*m_psBuffer += CString(_lpszOutput) + _T("\n");
    }
    else if (pDC)
	{
        // fix problem when handling empty string
        CString lpszOutput = _lpszOutput;
        if (lpszOutput.IsEmpty())
            lpszOutput = _T(" ");

		lpszOutput = ExpandTabs(static_cast<LPCTSTR>(lpszOutput));

		CRect crectPrintRect(m_printInfo.m_rectDraw);
		
        crectPrintRect.top  = ptCurPos.y;
		crectPrintRect.left = ptCurPos.x + LEFT_MARGIN;
		
        // Get dimensions
		int nHeight = pDC->DrawText(lpszOutput, &crectPrintRect, DT_LEFT | DT_TOP | DT_WORDBREAK | DT_NOFULLWIDTHCHARBREAK | DT_NOCLIP | DT_CALCRECT);
		ptCurPos.x = crectPrintRect.right;
		
		if (m_isRealPrint)
		{
			// Check for printing this string...
			// ( Is this string a part of the current printed page? )
			int nPageIndex = ((int) m_printInfo.m_nCurPage) - 1;
			int nStartPosition = 0;
			
            if (nPageIndex > 0 && m_virtualPrintPageInfoList.GetSize() > nPageIndex - 1)
			{
				nStartPosition = (int) m_virtualPrintPageInfoList.GetAt(nPageIndex - 1);
			}
			
			int nEndPosition = nStartPosition + m_printInfo.m_rectDraw.Height();
			
            if (nPageIndex >= 0 && m_virtualPrintPageInfoList.GetSize() > nPageIndex)
			{
				nEndPosition = (int) m_virtualPrintPageInfoList.GetAt(nPageIndex);
			}
			
            CRect crectRealRect = crectPrintRect;
            crectRealRect.top += m_printInfo.m_rectDraw.top - nStartPosition;
			crectRealRect.bottom = crectRealRect.top + nHeight;
			
			if (crectPrintRect.top >= nStartPosition && crectPrintRect.bottom <= nEndPosition)
			{
				OutputTextString(pDC, static_cast<LPCTSTR>(lpszOutput), crectRealRect);
                return nHeight;
			}
			else
			{
				return nHeight;
			}
		}
		else
		{
			// Get page positions and check page jumping
			int nStartPosition = 0;
			
			if (m_virtualPrintPageInfoList.GetSize() > 0)
			{
				nStartPosition = (int) m_virtualPrintPageInfoList.GetAt(m_virtualPrintPageInfoList.GetSize() - 1);
			}
			
            int nEndPosition = crectPrintRect.bottom;
			
            // Page jumping
			if (nEndPosition - nStartPosition > m_printInfo.m_rectDraw.Height())
			{
				m_virtualPrintPageInfoList.Add((DWORD) ptCurPos.y);
			}
		}
		OutputTextString(pDC, static_cast<LPCTSTR>(lpszOutput), crectPrintRect);
        return nHeight; 
	}
	return 0;
}

void SVConfigurationPrint::PrintValueObject(CDC* pDC, CPoint& ptCurPos, LPCTSTR lpszName, LPCTSTR lpszValue)
{
	if (m_isPrintToStringBuffer)
	{
		*m_psBuffer += CString(lpszName) + _T("\t") + CString(lpszValue) + _T("\n");
    }
    else
    {
		CPoint	ptTemp;
		int		nFirstHeight = 0;
		int		nLastHeight  = 0;
		
		ptTemp       = ptCurPos;
		nFirstHeight = PrintString(pDC, ptTemp, lpszName);
		ptTemp.x     = (ptTemp.x <(m_pageCenter - 10)) ? m_pageCenter :(ptTemp.x + m_TabPixels);
		nLastHeight  = PrintString(pDC, ptTemp, lpszValue);
		ptCurPos.y  += max(nFirstHeight, nLastHeight); 
    }
}

void SVConfigurationPrint::PrintIOEntryObject(CDC* pDC, CPoint& ptCurPos, int nIndentLevel, LPCTSTR lpszName, SVIOEntryHostStructPtr IOEntry)
{
	SVDigitalInputObject	*pDigInput = NULL;
	SVDigitalOutputObject	*pDigOutput = NULL;
	SVString				sValue;
	
	ptCurPos.x = nIndentLevel * m_shortTabPixels;
	SVObjectClass* l_pObject = SVObjectManagerClass::Instance().GetObject( IOEntry->m_IOId );
	PrintValueObject(pDC, ptCurPos, lpszName, l_pObject->GetName());
	ptCurPos.x = ++nIndentLevel * m_shortTabPixels;
	
	switch (IOEntry->m_ObjectType)
	{
		case IO_DIGITAL_INPUT:
			pDigInput = dynamic_cast< SVDigitalInputObject* >( l_pObject );
			if (pDigInput->IsForced())
				sValue = pDigInput->GetForcedValue() ? _T("1") : _T("0");
			else
				sValue = _T(" ");
			PrintValueObject(pDC, ptCurPos, _T("Forced"), sValue.c_str());
			
			sValue = pDigInput->IsInverted() ? _T("1") : _T(" ");
			PrintValueObject(pDC, ptCurPos, _T("Inverted"), sValue.c_str());
			break;

		case IO_DIGITAL_OUTPUT:
			pDigOutput = dynamic_cast< SVDigitalOutputObject* >( l_pObject );
			if (pDigOutput->IsForced())
				sValue = pDigOutput->GetForcedValue() ? _T("1") : _T("0");
			else
				sValue = _T(" ");
			PrintValueObject(pDC, ptCurPos, _T("Forced"), sValue.c_str());
			
			sValue = pDigOutput->IsInverted() ? _T("1") : _T(" ");
			PrintValueObject(pDC, ptCurPos, _T("Inverted"), sValue.c_str());

			sValue = pDigOutput->IsCombined() ? _T("1") : _T(" ");
			PrintValueObject(pDC, ptCurPos, _T("Combined"), sValue.c_str());
			
			sValue = pDigOutput->GetCombinedValue() ? _T("AND w/ ACK") : _T("OR w/ NAK");
			PrintValueObject(pDC, ptCurPos, _T("Combined using"), sValue.c_str());
			break;

		case IO_REMOTE_INPUT:
			break;

		case IO_REMOTE_OUTPUT:
			break;
	}
}

void SVConfigurationPrint::PrintMonitorListItem(CDC* pDC, CPoint& ptCurPos, int nIndentLevel, LPCTSTR lpszName, LPCTSTR lpszValue)
{
	SVDigitalInputObject	*pDigInput = NULL;
	SVDigitalOutputObject	*pDigOutput = NULL;
	SVString				sValue;
	
	ptCurPos.x = nIndentLevel * m_shortTabPixels;
	
	PrintValueObject(pDC, ptCurPos, lpszName, lpszValue);
}

void SVConfigurationPrint::OnVirtualPrint(BOOL bRealPrintInput /* = FALSE */) 
{
	SVObserverApp*         pApp    = dynamic_cast <SVObserverApp*> (AfxGetApp());
	SVConfigurationObject* pConfig = NULL;
	SVObjectManagerClass::Instance().GetConfigurationObject( pConfig );

    m_isRealPrint = bRealPrintInput;
	
    CDC* pDC = &m_printDC;
    CPrintInfo* pPrintInfo = &m_printInfo;
	
	// Will be called by OnBeginPrinting
	CPoint  ptCurPos(0, 0);
	CPoint	ptTemp(0, 0);
	
	CString  sLabel, sValue;
	CString  strDate;
	CString  strTime;
	
    int      nIndentLevel = 0;
    int      nFirstHeight = 0;
	int      nLastHeight  = 0;
	long	 lSize = 0;
	
    CFont*   pcfontOldFont;
	
	// Exchange font and store old one
	pcfontOldFont = pDC->SelectObject(&m_fontTitle);
	
    // Initialize the Non-Printing GUID pointer array size variable.
    m_NPArraySize = sizeof(pguidNonPrintArray) / sizeof(GUID*);
	
    // Print config title
	sLabel.Format(_T("Configuration %s"), pApp->getConfigFileName());
	
	ptCurPos.x  = nIndentLevel * m_shortTabPixels;
	ptTemp      = ptCurPos;
	ptCurPos.y += PrintString(pDC, ptTemp, sLabel);
	
	pDC->SelectObject(&m_fontText);
	
	// Print config path
	PrintValueObject(pDC, ptCurPos, _T("Current Path:"), pApp->getConfigFullFileName());
	
	// Print current date
	// Get current date
	LPTSTR pString =  strDate.GetBuffer(32);
	if (GetDateFormat(LOCALE_SYSTEM_DEFAULT,	       // locale for which date is to be formatted 
					  0,							   // flags specifying function options 
					  NULL,							   // date to be formatted, NULL = use current
					  _T("ddd',' dd'-'MMM'-'yyyy"),	   // date format string
					  pString,						   // buffer for storing formatted string
					  31							   // size of buffer
					  ) == 0)
	{
		strDate.ReleaseBuffer();
		strDate = _T("(Unknown)");
	}
	else
	{
		pString[31] = '\0';
		strDate.ReleaseBuffer();
	}
	
    // Get current time
	pString =  strTime.GetBuffer(32);
	if (GetTimeFormat(LOCALE_SYSTEM_DEFAULT,	        // locale for which time is to be formatted 
					  0,								// flags specifying function options 
					  NULL,								// time to be formatted, NULL = use current
					  _T("hh':'mm':'ss' 'tt"),	        // time format string, e.g. "01:34:42 PM"
					  pString,							// buffer for storing formatted string
					  31								// size of buffer
					  ) == 0)
    {
		strTime.ReleaseBuffer();
		strTime = _T("(Unknown)");
    }
    else
    {
		pString[31] = '\0';
		strTime.ReleaseBuffer();
    }
	sValue = strDate + _T(" ") + strTime;
	
	PrintValueObject(pDC, ptCurPos, _T("Current Date:"), sValue);
	
	// Print Application Section
	pDC->SelectObject(&m_fontSection);
	ptTemp      = ptCurPos;
	ptCurPos.y += PrintString(pDC, ptTemp, _T("\nConfiguration Settings"));
	
	ptCurPos.x  = ++nIndentLevel * m_shortTabPixels;
	PrintThreadAffinity(pDC, ptCurPos, nIndentLevel);
	PrintTriggerSummary(pDC, ptCurPos, nIndentLevel);
	PrintCameraSummary(pDC, ptCurPos, nIndentLevel);
	PrintInspectionSummary(pDC, ptCurPos, nIndentLevel);
	PrintPPQSummary(pDC, ptCurPos, nIndentLevel);
	ptCurPos.x = --nIndentLevel * m_shortTabPixels;
	
	// Print Inspection Toolsets Section
	PrintInspectionToolSet(pDC, ptCurPos, nIndentLevel);
	
	// Print Input/Output Section
	PrintIOSection(pDC, ptCurPos, nIndentLevel);
	
	// Print PPQ Bar Section
	PrintPPQBarSection(pDC, ptCurPos, nIndentLevel);
}  // end function void SVObserverApp::OnVirtualPrint( ... )

void SVConfigurationPrint::OnPrepareDC() 
{
    CDC* pDC = &m_printDC;
	
	ASSERT_VALID(pDC);
	
    UNUSED(pDC); // unused in release builds

	// Default to one page printing if doc length not known
	m_printInfo.m_bContinuePrinting = (m_printInfo.GetMaxPage() != 0xffff) || (m_printInfo.m_nCurPage == 1);
}

void SVConfigurationPrint::OnBeginPrinting()
{
	CDC* pDC = &m_printDC;
	
	if (pDC)
	{
		pDC->SetMapMode(MM_TEXT);
		pDC->SetTextAlign(TA_LEFT | TA_TOP | TA_NOUPDATECP);

		// set up drawing rect to entire page in device coordinates)
		m_printInfo.m_rectDraw.SetRect(0, 0, pDC->GetDeviceCaps(HORZRES), pDC->GetDeviceCaps(VERTRES));
		
		pDC->DPtoLP(&m_printInfo.m_rectDraw);
		pDC->SetViewportOrg(0, 0);
		pDC->SetViewportExt(m_printInfo.m_rectDraw.Width(), m_printInfo.m_rectDraw.Height());

        //
        // Size the fonts based on the printer driver report of space on the
        // printer 'device context'.
        //
		double fHeightPagePixels = (double)m_printInfo.m_rectDraw.Height();
		double fHeightTextLinePixels = fHeightPagePixels/ ((double)SV_PRINT_LINES_PER_PAGE);
		double fHeightTitleLinePixels = fHeightTextLinePixels *
			((double)SV_DEFAULT_PRINT_TITLE_FONT_HEIGHT/ (double)SV_DEFAULT_PRINT_TEXT_FONT_HEIGHT);
		double fHeightSectionLinePixels = fHeightTextLinePixels *
			((double)SV_DEFAULT_PRINT_SUB_TITLE_FONT_HEIGHT/ (double)SV_DEFAULT_PRINT_TEXT_FONT_HEIGHT);
		double fHeightPageNumberPixels = fHeightTextLinePixels *
			((double)SV_DEFAULT_PAGE_TEXT_FONT_HEIGHT/ (double)SV_DEFAULT_PRINT_TEXT_FONT_HEIGHT);
		m_heightPageNumberPixels = (int)fHeightPageNumberPixels;
		m_pageCenter = m_printInfo.m_rectDraw.Width() * 50 / 100;
		
		//
        // Create fonts based on device units i.e. negative font heights.
        //
		
		// create default fonts...
		CPoint			aPoint;
		
		// Create title font
		BOOL bResult = m_fontTitle.CreateFont(
			- (int)fHeightTitleLinePixels,				// logical height of font 
			0,											// logical average character width 
			0,									        // angle of escapement 
			0,									        // base-line orientation angle 
			FW_DONTCARE,					            // font weight 
			FALSE,								        // italic attribute flag 
			TRUE,									    // underline attribute flag 
			FALSE,								        // strikeout attribute flag 
			DEFAULT_CHARSET,						    // character set identifier 
			OUT_DEFAULT_PRECIS,					        // output precision 
			CLIP_DEFAULT_PRECIS,					    // clipping precision 
			PROOF_QUALITY,						        // output quality 
			DEFAULT_PITCH | TMPF_TRUETYPE | FF_ROMAN,	// pitch and family 
			NULL									    // pointer to typeface name string 
			);
		ASSERT(bResult);
		
		// Create sub title font
		bResult = m_fontSection.CreateFont(
			- (int)fHeightSectionLinePixels,			// logical height of font 
			0,											// logical average character width 
			0,											// angle of escapement 
			0,											// base-line orientation angle 
			FW_DONTCARE,							    // font weight 
			FALSE,										// italic attribute flag 
			TRUE,										// underline attribute flag 
			FALSE,										// strikeout attribute flag 
			DEFAULT_CHARSET,							// character set identifier 
			OUT_DEFAULT_PRECIS,						    // output precision 
			CLIP_DEFAULT_PRECIS,						// clipping precision 
			PROOF_QUALITY,								// output quality 
			DEFAULT_PITCH | TMPF_TRUETYPE | FF_ROMAN,	// pitch and family 
			NULL										// pointer to typeface name string 
			);
		ASSERT(bResult);
		
		// Create sub title font
		bResult = m_fontSubSection.CreateFont(
			- (int)fHeightSectionLinePixels,			// logical height of font 
			0,											// logical average character width 
			0,											// angle of escapement 
			0,											// base-line orientation angle 
			FW_DONTCARE,							    // font weight 
			FALSE,										// italic attribute flag 
			FALSE,										// underline attribute flag 
			FALSE,										// strikeout attribute flag 
			DEFAULT_CHARSET,							// character set identifier 
			OUT_DEFAULT_PRECIS,						    // output precision 
			CLIP_DEFAULT_PRECIS,						// clipping precision 
			PROOF_QUALITY,								// output quality 
			DEFAULT_PITCH | TMPF_TRUETYPE | FF_ROMAN,	// pitch and family 
			NULL										// pointer to typeface name string 
			);
		ASSERT(bResult);
		
		// Create text font
		bResult = m_fontText.CreateFont(
			- (int)fHeightTextLinePixels,				// logical height of font 
			0,											// logical average character width 
			0,										    // angle of escapement 
			0,										    // base-line orientation angle 
			FW_DONTCARE,						        // font weight 
			FALSE,									    // italic attribute flag 
			FALSE,									    // underline attribute flag 
			FALSE,									    // strikeout attribute flag 
			DEFAULT_CHARSET,						    // character set identifier 
			OUT_DEFAULT_PRECIS,					        // output precision 
			CLIP_DEFAULT_PRECIS,					    // clipping precision 
			PROOF_QUALITY,							    // output quality 
			DEFAULT_PITCH | TMPF_TRUETYPE | FF_ROMAN,	// pitch and family 
			NULL									    // pointer to typeface name string 
			);
		ASSERT(bResult);
		
		// Create sub title font
		bResult = m_fontPageNbr.CreateFont(
			-m_heightPageNumberPixels,					// logical height of font 
			0,											// logical average character width 
			0,											// angle of escapement 
			0,											// base-line orientation angle 
			FW_DONTCARE,							    // font weight 
			FALSE,										// italic attribute flag 
			FALSE,										// underline attribute flag 
			FALSE,										// strikeout attribute flag 
			DEFAULT_CHARSET,							// character set identifier 
			OUT_DEFAULT_PRECIS,						    // output precision 
			CLIP_DEFAULT_PRECIS,						// clipping precision 
			PROOF_QUALITY,								// output quality 
			DEFAULT_PITCH | TMPF_TRUETYPE | FF_ROMAN,	// pitch and family 
			NULL										// pointer to typeface name string 
			);
		ASSERT(bResult);
		
		// Save space for page number
		m_printInfo.m_rectDraw.bottom -=(m_heightPageNumberPixels*2);
		
		// Get page number and print contents
		m_virtualPrintPageInfoList.RemoveAll();
		
		OnVirtualPrint(FALSE);    // Not a 'Real' print yet
		
		m_printInfo.SetMaxPage((UINT) m_virtualPrintPageInfoList.GetSize() + 1);
	}
}

void SVConfigurationPrint::PrintPage() 
{
    SVObserverApp* pApp = dynamic_cast <SVObserverApp*> (AfxGetApp());
	
	CDC* pDC = &m_printDC;
	
	OnVirtualPrint(TRUE);       // Real print
	
    CString  strPage;
	
    strPage.Format("%s %u %s %u", 
					pApp->GetStringResource(IDS_PAGE_STRING), 
					m_printInfo.m_nCurPage, 
					pApp->GetStringResource(IDS_OF_STRING), 
					m_printInfo.GetMaxPage());
	
	CFont*   pcfontOldFont;
	
	// Exchange font and store old one
	pcfontOldFont = pDC->SelectObject(&m_fontPageNbr);
	
	UINT uOldTextAlign = pDC->SetTextAlign(TA_CENTER | TA_NOUPDATECP);
	
	pDC->TextOut(m_pageCenter, pDC->GetDeviceCaps(VERTRES) - m_heightPageNumberPixels, strPage);
	
	pDC->SetTextAlign(uOldTextAlign);
	
	// Restore old font style
	pDC->SelectObject(pcfontOldFont);
}  // end function void SVConfigurationPrint::PrintPage() 

void SVConfigurationPrint::OnEndPrinting()
{
}

void SVConfigurationPrint::PrintCameraSummary(CDC* pDC, CPoint& ptCurPos, int nIndentLevel)
{
	SVConfigurationObject* pConfig = NULL;
	SVObjectManagerClass::Instance().GetConfigurationObject( pConfig );

	CString	sLabel, sValue;
	int     nFirstHeight = 0;
	int     nLastHeight  = 0;
	long	lSize        = 0;
	CPoint	ptTemp(0, 0);
	
	pDC->SelectObject(&m_fontSection);
	ptCurPos.x  = nIndentLevel * m_shortTabPixels;
	ptTemp      = ptCurPos; 
	ptCurPos.y += PrintString(pDC, ptTemp, _T("\nCamera"));
	pDC->SelectObject(&m_fontText);
	
	pConfig->GetCameraCount(lSize);
	for (long l = 0; l < lSize; l++)
	{
		SVVirtualCamera* pCamera = NULL;
		
		pConfig->GetCamera(l, &pCamera);
		if (pCamera)
		{
			SVFileNameArrayClass* pfnac = NULL;
			SVLightReference* plrcDummy = NULL;
			SVLut* plutDummy = NULL;
			SVDeviceParamCollection* pDeviceParams = NULL;
			SVAcquisitionClassPtr pAcqDevice = pCamera->GetAcquisitionDevice();
			ASSERT( !( pAcqDevice.empty() ) );
			BOOL bOk = pConfig->GetAcquisitionDevice( pAcqDevice->GetRootDeviceName(), pfnac, plrcDummy, plutDummy, pDeviceParams );
			ASSERT( bOk );
			ASSERT( pfnac );
			if ( bOk )
			{
				// print camera name
				ptCurPos.x   = (nIndentLevel+1) * m_shortTabPixels;
				if ( pCamera->mpsvDevice )
				{
					PrintValueObject(pDC, ptCurPos, pCamera->GetName(), pCamera->mpsvDevice->DeviceName());
				}
				else
				{
					PrintValueObject(pDC, ptCurPos, pCamera->GetName(), _T("<No Device>"));
				}

				if (pCamera->IsFileAcquisition())
				{
					ptCurPos.x = (nIndentLevel+2) * m_shortTabPixels;
					
					PrintValueObject(pDC, ptCurPos, _T("Acquisition Type:"), _T("File Acquisition"));

					long loadingMode = pCamera->GetFileLoadingMode();
					if (loadingMode == ContinuousMode)
					{
						PrintValueObject(pDC, ptCurPos, _T("File Loading Mode:"), DeviceParamFileAcqContinuousLoad_String);
					}
					else if (loadingMode == SingleIterationMode)
					{
						PrintValueObject(pDC, ptCurPos, _T("File Loading Mode:"), DeviceParamFileAcqSingleIteration_String);
					}
					else // Single File Mode
					{
						PrintValueObject(pDC, ptCurPos, _T("File Loading Mode:"), DeviceParamFileAcqSingleFile_String);
					}
					
					PrintValueObject(pDC, ptCurPos, _T("Image Filename:"), pCamera->GetImageFilename());
					PrintValueObject(pDC, ptCurPos, _T("Image Directory:"), pCamera->GetImageDirectoryName());
					
					PrintValueObject(pDC, ptCurPos, _T("Image Size Mode:"),
						pCamera->IsFileImageSizeEditModeFileBased() ?  _T( "Use Image Size from File" ) : _T( "User Editable" ));
					
					if (!pCamera->IsFileImageSizeEditModeFileBased())
					{
						long width = pCamera->GetFileImageWidth();
						sValue.Format(_T("%d"), width);
						PrintValueObject(pDC, ptCurPos, _T("Image Width:"), sValue);

						long height = pCamera->GetFileImageHeight();
						sValue.Format(_T("%d"), height);
						PrintValueObject(pDC, ptCurPos, _T("Image Height:"), sValue);
					}
				}
				else
				{
					ptCurPos.x = (nIndentLevel+2) * m_shortTabPixels;

					PrintValueObject(pDC, ptCurPos, _T("Acquisition Type:"), _T("Hardware Acquisition"));

					// print camera file
					PrintValueObject(pDC, ptCurPos, _T("Camera File(s):"), _T("") );
					ptTemp = ptCurPos;
					ptTemp.x += m_shortTabPixels;
					sValue = pfnac->GetFileNameList();
					sValue.Replace(_T(";"), _T("\n"));
					ptCurPos.y += PrintString(pDC, ptTemp, sValue);

					// print camera device params
					if ( pDeviceParams && pDeviceParams->mapParameters.size() > 0 )
					{
						PrintValueObject(pDC, ptCurPos, _T("Camera Parameters:"), _T("") );
						SVDeviceParamCollection l_CameraFileParams;
						HRESULT hr = pAcqDevice->GetCameraFileParameters(l_CameraFileParams);
						CPoint pt(ptCurPos);
						pt.x += m_shortTabPixels;
						SVDeviceParamConfigPrintHelper helper(this, l_CameraFileParams, pDC, pt, nIndentLevel+1);
						const int iDetailLevel = 0;

						SVDeviceParamIndexer index( pDeviceParams->mapParameters );	// print in order
						SVDeviceParamIndexer::iterator iter;
						for (iter = index.begin(); iter != index.end(); ++iter)
						{
							SVDeviceParamWrapper& rw = pDeviceParams->GetParameter( *iter );
							SVDeviceParam* pParam = static_cast <SVDeviceParam*> (rw);
							if ( pParam )
							{
								SVDeviceParam* pCamFileParam = l_CameraFileParams.GetParameter( pParam->Type() );
								if ( pCamFileParam == NULL)
									continue;
								if ( pCamFileParam->DetailLevel() > iDetailLevel )
									continue;

								hr = pParam->Accept(helper);
							}
							// Visitor was used instead of a huge switch statement.
							// It was done this way partly as an example of how to use Visitor.
						}
						ptCurPos.y = pt.y;
					}// end if ( pDeviceParams && pDeviceParams->mapParameters.size() > 0 )
				}
			}// end if ( bOk )
		}// end if (pCamera)
	}// end for (long l = 0; l < lSize; l++)
}

void SVConfigurationPrint::PrintTriggerSummary(CDC* pDC, CPoint& ptCurPos, int nIndentLevel)
{
	SVConfigurationObject* pConfig = NULL;
	SVObjectManagerClass::Instance().GetConfigurationObject( pConfig );

	CString	sLabel, sValue;
	int     nFirstHeight = 0;
	int     nLastHeight  = 0;
	long	lSize = 0;
	CPoint	ptTemp(0, 0);
	
	pDC->SelectObject(&m_fontSection);
	ptCurPos.x  = nIndentLevel * m_shortTabPixels;
	ptTemp      = ptCurPos;
	ptCurPos.y += PrintString(pDC, ptTemp, _T("\nTrigger"));
	pDC->SelectObject(&m_fontText);
	
	pConfig->GetTriggerCount(lSize);
	for (long l = 0; l < lSize; l++)
	{
		SVTriggerObject *pTrigger = NULL;
		
		pConfig->GetTrigger(l, &pTrigger);
		if (pTrigger)
		{
			ptCurPos.x   = (nIndentLevel + 1) * m_shortTabPixels;
			if ( pTrigger->mpsvDevice )
			{
			PrintValueObject(pDC, ptCurPos, pTrigger->GetName(), pTrigger->mpsvDevice->GetDeviceName());
			}
			else
			{
				PrintValueObject(pDC, ptCurPos, pTrigger->GetName(), _T("<No Device>"));
			}
			
			ptCurPos.x   = (nIndentLevel + 2) * m_shortTabPixels;

			if (pTrigger->IsSoftwareTrigger())
			{
				PrintValueObject(pDC, ptCurPos, _T("Trigger Type:"), _T("Software"));

				long period = pTrigger->GetSoftwareTriggerPeriod();
				sValue.Format(_T("%d milliseconds"), period);
				PrintValueObject(pDC, ptCurPos, _T("Timer Period:"), sValue);
			}
			else
			{
				PrintValueObject(pDC, ptCurPos, _T("Trigger Type:"), _T("Hardware"));
				PrintValueObject(pDC, ptCurPos, _T("OutputDelayTime"), _T(""));
			}
		}
	}
}

void SVConfigurationPrint::PrintInspectionSummary(CDC* pDC, CPoint& ptCurPos, int nIndentLevel)
{
	CString	sLabel, sValue;
	int      nFirstHeight   = 0;
	int      nLastHeight    = 0;
	long		lSize = 0;
	CPoint	ptTemp(0, 0);
	SVConfigurationObject* pConfig = NULL;
	SVObjectManagerClass::Instance().GetConfigurationObject( pConfig );
	
	pDC->SelectObject(&m_fontSection);
	ptCurPos.x  = nIndentLevel * m_shortTabPixels;
	ptTemp      = ptCurPos;
	ptCurPos.y += PrintString(pDC, ptTemp, _T("\nInspection"));
	pDC->SelectObject(&m_fontText);
	
	pConfig->GetInspectionCount(lSize);
	for (long l = 0; l < lSize; l++)
	{
		SVInspectionProcess *pInspect = NULL;
		
		pConfig->GetInspection(l, &pInspect);
		if (pInspect)
		{
			ptCurPos.x   = (nIndentLevel + 1) * m_shortTabPixels;
			ptTemp       = ptCurPos;
			ptCurPos.y  += PrintString(pDC, ptTemp, pInspect->GetName());
			
			ptCurPos.x   = (nIndentLevel + 2) * m_shortTabPixels;
			PrintValueObject(pDC, ptCurPos, _T("Number of Panels"), sValue);
		}
	}
}

void SVConfigurationPrint::PrintPPQSummary(CDC* pDC, CPoint& ptCurPos, int nIndentLevel)
{
	CString	sLabel, sValue;
	int     nFirstHeight = 0;
	int     nLastHeight  = 0;
	long	lPPQ = 0, lSize = 0;
	CPoint	ptTemp(0, 0);
	
	SVConfigurationObject* pConfig = NULL;
	SVObjectManagerClass::Instance().GetConfigurationObject( pConfig );
	
	pDC->SelectObject(&m_fontSection);
	ptCurPos.x  = nIndentLevel * m_shortTabPixels;
	ptTemp      = ptCurPos;
	ptCurPos.y += PrintString(pDC, ptTemp, _T("\nPPQ"));
	pDC->SelectObject(&m_fontText);
	
	pConfig->GetPPQCount(lPPQ);
	++nIndentLevel;
	for (long l = 0; l < lPPQ; l++)
	{
		SVPPQObject *pPPQ = NULL;
		
		pConfig->GetPPQ(l, &pPPQ);
		if (pPPQ)
		{
			ptCurPos.x   = nIndentLevel * m_shortTabPixels;
			ptTemp       = ptCurPos;
			ptCurPos.y  += PrintString(pDC, ptTemp, pPPQ->GetName());
			
			SVPPQOutputModeEnum	enumPPQOutputMode;
			long	lPPQLength  = 0;	// PPQLength
			long	lResetDelay = 0;	// PPQOutputResetDelay
			long	lDelayTime  = 0;	// PPQOutputDelayTime
			
			ptCurPos.x = (nIndentLevel + 1) * m_shortTabPixels;
			pPPQ->GetPPQOutputMode(enumPPQOutputMode);
			pPPQ->GetPPQLength(lPPQLength);
			pPPQ->GetResetDelay(lResetDelay);
			pPPQ->GetOutputDelay(lDelayTime);
			
			switch (enumPPQOutputMode)
			{
				case SVPPQNextTriggerMode:	// Resets outputs on trigger. Writes outputs immediately.
					sValue = _T("NextTrigger");
					break;
				case SVPPQTimeDelayMode:	// Resets outputs on trigger. Writes outputs after delay time is over, even if data isn't complete.
					// Aborts waiting if new trigger occurs.
					sValue = _T("TimeDelay");
					break;
				case SVPPQTimeDelayAndDataCompleteMode:	// Resets outputs on trigger. Writes outputs after delay time is over and data is complete.
					// Aborts waiting if new trigger occurs.
					sValue = _T("TimeDelayDataCompletion");
					break;
				case SVPPQExtendedTimeDelayMode:		// Doesn't reset outputs on trigger. Writes outputs after delay time is over, even if data isn't complete.
																	// Aborts waiting if product leaves PPQ.
					sValue = _T("ExtendedTimeDelayMode");
					break;
				case SVPPQExtendedTimeDelayAndDataCompleteMode: // Doesn't reset outputs on trigger. Writes outputs after delay time is over and data is complete.
					// Aborts waiting if product leaves PPQ.
					sValue = _T("ExtendedTimeDelayDataCompletion");
					break;
				default:
					sValue = _T("< I N V A L I D ! >");
					break;
			}
			
			ptTemp       = ptCurPos;
			PrintValueObject(pDC, ptCurPos, _T("PPQMode"), sValue);
			
			sValue.Format(_T("%ld"), lPPQLength);
			PrintValueObject(pDC, ptCurPos, _T("PPQLength"), sValue);
			
			sValue.Format(_T("%ld"), lResetDelay);
			PrintValueObject(pDC, ptCurPos, _T("PPQOutputResetDelay"), sValue);
			
			sValue.Format(_T("%ld"), lDelayTime);
			PrintValueObject(pDC, ptCurPos, _T("PPQOutputDelayTime"), sValue);
			
			ptCurPos.x  = (nIndentLevel + 1) * m_shortTabPixels;
			ptTemp      = ptCurPos;
			ptCurPos.y += PrintString(pDC, ptTemp, _T("Trigger:"));
			
			SVTriggerObject*	pTrigger;
			pPPQ->GetTrigger(pTrigger);
			ptCurPos.x  = (nIndentLevel + 2) * m_shortTabPixels;
			ptTemp      = ptCurPos;
			ptCurPos.y += PrintString(pDC, ptTemp, pTrigger->GetName());
			
			ptCurPos.x  = (nIndentLevel + 1) * m_shortTabPixels;
			ptTemp      = ptCurPos;
			ptCurPos.y += PrintString(pDC, ptTemp, _T("Camera:"));
			
			std::deque< SVVirtualCamera* > l_Cameras;

			pPPQ->GetCameraList( l_Cameras );

			std::deque< SVVirtualCamera* >::iterator l_Iter = l_Cameras.begin();

			while( l_Iter != l_Cameras.end() )
			{
				SVVirtualCamera* pCamera = ( *l_Iter );
				
				if (pCamera)
				{
					ptCurPos.x  = (nIndentLevel + 2) * m_shortTabPixels;
					ptTemp      = ptCurPos;
					ptCurPos.y += PrintString(pDC, ptTemp, pCamera->GetName());
				}

				++l_Iter;
			}
			
			ptCurPos.x  = (nIndentLevel + 1) * m_shortTabPixels;
			ptTemp      = ptCurPos;
			ptCurPos.y += PrintString(pDC, ptTemp, _T("Inspection:"));
			
			pPPQ->GetInspectionCount(lSize);
			for (int intInspection = 0; intInspection < lSize; intInspection++)
			{
				SVInspectionProcess*	pInspection;
				pPPQ->GetInspection(intInspection, pInspection);
				
				if (pInspection)
				{
					ptCurPos.x  = (nIndentLevel + 2) * m_shortTabPixels;
					ptTemp      = ptCurPos;
					ptCurPos.y += PrintString(pDC, ptTemp, pInspection->GetName());
				}
			}
		}
	}
}

void SVConfigurationPrint::PrintPPQBarSection(CDC* pDC, CPoint& ptCurPos, int nIndentLevel)
{
	SVConfigurationObject* pConfig = NULL;
	SVObjectManagerClass::Instance().GetConfigurationObject( pConfig );

	CString sLabel, sValue;
	int     nFirstHeight = 0;
	int     nLastHeight  = 0;
	CPoint  ptTemp(0, 0);

	pDC->SelectObject(&m_fontSection);
	ptCurPos.x  = nIndentLevel * m_shortTabPixels;
	ptTemp      = ptCurPos;
	ptCurPos.y += PrintString(pDC, ptTemp, _T("\nPPQ Bar Section"));
	pDC->SelectObject(&m_fontText);

	long lPPQ;
	pConfig->GetPPQCount(lPPQ);
	for (int intPPQ = 0; intPPQ < lPPQ; intPPQ++)
	{
		SVPPQObject* pPPQ;

		if ( !pConfig->GetPPQ( intPPQ, &pPPQ ) )
		{
			SvStl::ExceptionMgr1 e; // The default constructor sets the type to LogOnly.
			e.setMessage( SVMSG_SVO_55_DEBUG_BREAK_ERROR, SvO::ErrorGettingPPQ, StdExceptionParams, Err_17000_SVConfigurationPrint_PrintPPQBarSection_ErrorGettingPPQ );
			DebugBreak();
		}

		if ( !pPPQ ) { continue; }

		ptCurPos.x  = ++nIndentLevel * m_shortTabPixels;
		ptTemp      = ptCurPos;
		ptCurPos.y += PrintString(pDC, ptTemp, pPPQ->GetName());

		long lPPQLength = 0;
		pPPQ->GetPPQLength(lPPQLength);
		for (int intPPQPos = 0; intPPQPos < lPPQLength; intPPQPos++)
		{
			bool bPosPrint = false;
			std::deque< SVVirtualCamera* > l_Cameras;

			pPPQ->GetCameraList( l_Cameras );

			std::deque< SVVirtualCamera* >::iterator l_Iter = l_Cameras.begin();

			while( l_Iter != l_Cameras.end() )
			{
				SVVirtualCamera* pCamera = ( *l_Iter );
				
				if (pCamera)
				{
					long lPos = -1;

					pPPQ->GetCameraPPQPosition( lPos, pCamera );

					if (lPos == intPPQPos)
					{
						if (!bPosPrint)
						{
							sLabel.Format(_T("Position %d:"), intPPQPos + 1);
							ptCurPos.x  = (nIndentLevel + 1) * m_shortTabPixels;
							ptTemp      = ptCurPos;
							ptCurPos.y += PrintString(pDC, ptTemp, sLabel);
							bPosPrint = true;
						}

						ptCurPos.x  = (nIndentLevel + 2) * m_shortTabPixels;
						ptTemp      = ptCurPos;
						ptCurPos.y += PrintString(pDC, ptTemp, pCamera->GetName());
					}
				}

				++l_Iter;
			}

			long lIOEntries;
			SVIOEntryHostStructPtrList ppIOEntries;

			pPPQ->GetAllInputs( ppIOEntries );

			lIOEntries = static_cast< long >( ppIOEntries.size() );

			for (int intIOEntry = 0; intIOEntry < lIOEntries; intIOEntry++)
			{
				if (ppIOEntries[intIOEntry]->m_PPQIndex == intPPQPos)
				{
					SVObjectClass* l_pObject = SVObjectManagerClass::Instance().GetObject( ppIOEntries[intIOEntry]->m_IOId );

					if (!bPosPrint)
					{
						sLabel.Format(_T("Position %d:"), intPPQPos + 1);
						ptCurPos.x  = (nIndentLevel + 1) * m_shortTabPixels;
						ptTemp      = ptCurPos;
						ptCurPos.y += PrintString(pDC, ptTemp, sLabel);
						bPosPrint = true;
					}

					ptCurPos.x  = (nIndentLevel + 2) * m_shortTabPixels;
					ptTemp      = ptCurPos;
                    if ( l_pObject != NULL )
                    {
                        if ( l_pObject->IsValid() )
                        {
							ptCurPos.y += PrintString(pDC, ptTemp, l_pObject->GetName());
                        }//end if
                    }
                    else
                    {
                        if ( ppIOEntries[intIOEntry]->m_pValueObject->IsValid() )
                        {
                            ptCurPos.y += PrintString(pDC, ptTemp, ppIOEntries[intIOEntry]->m_pValueObject->GetName());
                        } // end if
                    } //end else
				}
			}
		}
		--nIndentLevel;
	}
}

void SVConfigurationPrint::PrintInspectionToolSet(CDC* pDC, CPoint& ptCurPos, int nIndentLevel)
{
	SVConfigurationObject* pConfig = NULL;
	SVObjectManagerClass::Instance().GetConfigurationObject( pConfig );
	SVObserverApp*         pApp    = dynamic_cast <SVObserverApp*> (AfxGetApp());

	CString	sLabel, sValue;
	int      nFirstHeight   = 0;
	int      nLastHeight    = 0;
	long     lSize = 0;
	CPoint   ptTemp(0, 0);
	
	SVInspectionProcess*    pInspection = NULL;
	SVToolSetClass*         pToolset = NULL;
	
	// Print all IPDocs
	pConfig->GetInspectionCount(lSize);
	for (int nIPDNumber = 0; nIPDNumber < lSize; ++nIPDNumber)
	{
		pConfig->GetInspection(nIPDNumber, &pInspection);
		sLabel = pInspection->GetName();
		SVToolSetClass* pToolSet = pInspection->GetToolSet();
		pDC->SelectObject(&m_fontSection);
		
		m_toolNumber = 0;
		
		// Print IPDoc number
		sLabel.Format(_T("\nInspection Process %d"), nIPDNumber + 1);
		ptCurPos.x  = nIndentLevel * m_shortTabPixels;
		ptTemp      = ptCurPos;
		ptCurPos.y += PrintString(pDC, ptTemp, sLabel);
		pDC->SelectObject(&m_fontText);
		
		PrintValueObject(pDC, ptCurPos, _T("Title:"), pInspection->GetName());
		
		// Print number of IPD tools
		if (pToolSet)
			sValue.Format("%d", pToolSet->GetSize());
		else
			sValue = pApp->GetStringResource(IDS_NOT_AVAILABLE_STRING);
		
		ptCurPos.x   = nIndentLevel * m_shortTabPixels;
		PrintValueObject(pDC, ptCurPos, _T("Number of Tools:"), sValue);
		
		// Print all tools
		if (pToolSet)
		{
			PrintObject(pDC, pToolSet, ptCurPos, nIndentLevel);
		}  // end if( pToolSet )
	}  // for( int j = 0; j < pApp->pActivatedSystemList->GetAt( i )->GetBoardIPDocNumber();
}

void SVConfigurationPrint::PrintIOSection(CDC* pDC, CPoint& ptCurPos, int nIndentLevel)
{
	SVObserverApp* pApp = dynamic_cast <SVObserverApp*> (AfxGetApp());
	CString	sLabel, sValue;
	int     nFirstHeight   = 0;
	int     nLastHeight    = 0;
	long	lSize = 0;
	CPoint	ptTemp(0, 0);
	
	pDC->SelectObject(&m_fontSection);
	sLabel       =  _T("\nInput/Output:");
	ptCurPos.x  = nIndentLevel * m_shortTabPixels;
	ptTemp      = ptCurPos;
	ptCurPos.y += PrintString(pDC, ptTemp, sLabel);
	pDC->SelectObject(&m_fontText);
	
	PrintModuleIO(pDC, ptCurPos, nIndentLevel);
	PrintResultIO(pDC, ptCurPos, nIndentLevel);
	PrintMonitorListSection(pDC, ptCurPos, nIndentLevel);
}

void SVConfigurationPrint::PrintModuleIO(CDC* pDC, CPoint& ptCurPos, int nIndentLevel)
{
	CString label, value;
	int		nFirstHeight = 0;
	int		nLastHeight = 0;
	long	lSize = 0;
	CPoint	ptTemp(0, 0);

	SVConfigurationObject* pConfig = NULL;
	SVObjectManagerClass::Instance().GetConfigurationObject( pConfig );
	SVObserverApp* pApp = dynamic_cast<SVObserverApp*>(AfxGetApp());

	// Print IODoc contents
	if (pApp->GetIODoc())
	{
		SVInputObjectList* pInputList;
		SVDigitalInputObject* pDigInput;
		SVIOEntryHostStructPtrList ppIOEntries;

		// Get list of available inputs
		if (!pConfig->GetInputObjectList(&pInputList))
		{
			SvStl::ExceptionMgr1 e; // The default constructor sets the type to LogOnly.
			e.setMessage( SVMSG_SVO_55_DEBUG_BREAK_ERROR, SvO::ErrorGettingInputObjectList, StdExceptionParams, Err_17001_SVConfigurationPrint_PrintModuleIO_ErrorGettingInputObjectList );
			DebugBreak();
		}

		if (!pInputList->FillInputs( ppIOEntries ))
		{
			SvStl::ExceptionMgr1 e; // The default constructor sets the type to LogOnly.
			e.setMessage( SVMSG_SVO_55_DEBUG_BREAK_ERROR, SvO::ErrorFillingInputs, StdExceptionParams, Err_17002_SVConfigurationPrint_PrintModuleIO_ErrorFillingInputs );
			DebugBreak();
		}

		lSize = static_cast< long >( ppIOEntries.size() );

		// Print module input title...
		DWORD dwMaxInput = 0;
		SVIOConfigurationInterfaceClass::Instance().GetDigitalInputCount( dwMaxInput );

		value.Format(_T("%ld"), dwMaxInput);
		ptCurPos.x = nIndentLevel * m_shortTabPixels;
		PrintValueObject(pDC, ptCurPos, _T("Digital Inputs:"), value);

		// Module Inputs
		for (long i = 0; i < static_cast<long>(dwMaxInput); ++i)
		{
			label.Format(_T("Digital Input %d:"), i+1);

			// Find each digital input
			for (int j = 0; j < lSize; j++)
			{
				if (ppIOEntries[j]->m_ObjectType != IO_DIGITAL_INPUT) { continue; }

				SVObjectClass* l_pObject = SVObjectManagerClass::Instance().GetObject( ppIOEntries[j]->m_IOId );

				pDigInput = dynamic_cast< SVDigitalInputObject* >( l_pObject );

				if (!pDigInput) { continue; }

				if (i == pDigInput->GetChannel())
				{
					PrintIOEntryObject(pDC, ptCurPos, nIndentLevel + 1, label, ppIOEntries[j]);
					break;
				}
			}
		}

		// Find each Remote input
		int j = 0;
		for (int i = 0; i < lSize; i++)
		{
			if (ppIOEntries[i]->m_ObjectType == IO_REMOTE_INPUT)
			{
				++j;
			}
		}
		value.Format(_T("%d"), j);
		ptCurPos.x = nIndentLevel * m_shortTabPixels;
		PrintValueObject(pDC, ptCurPos, _T("Remote Inputs:"), value);

		if (j)
		{
			for( int k = 0, l = 0; k < lSize; k++ )
			{
				if (ppIOEntries[k]->m_ObjectType != IO_REMOTE_INPUT) { continue; }

				label.Format(_T("Remote Input %d"), (l++)+1);
				PrintIOEntryObject(pDC, ptCurPos, nIndentLevel+1, label, ppIOEntries[k]);
			}
		}
	}
}  // end function void SVObserverApp::PrintModuleIOP( CDC* pDC, ... )

void SVConfigurationPrint::PrintThreadAffinity(CDC* pDC, CPoint& ptCurPos, int nIndentLevel)
{
	CString value;
		
	// Thread Enable
	ptCurPos.y += PrintString(pDC,ptCurPos,_T("\n"));
	BOOL bEnabled = SVThreadManager::Instance().GetThreadAffinityEnabled();
	value = bEnabled ? _T("Enabled") : _T("Disabled");
	ptCurPos.x   = nIndentLevel * m_shortTabPixels;
	PrintValueObject(pDC, ptCurPos, _T("Thread Setup :"), value);

	// Set Thread Affinities
	ptCurPos.x   = (nIndentLevel + 1) * m_shortTabPixels;
	SVThreadManager::ThreadList Threads;
	HRESULT hRet = SVThreadManager::Instance().GetThreadInfo(Threads, SVAffinityUser );
	for( SVThreadManager::ThreadList::const_iterator it = Threads.begin() ; it != Threads.end() ; ++it)
	{
		SVString strName;
		SVString strValue;
		if( it->m_lAffinity > 0 )
		{
			strValue.Format( "%d", it->m_lAffinity);
		}
		else
		{
			strValue = "Not Set";
		}
		strName.Format("Name: %s:", it->m_strName.c_str() );
		PrintValueObject(pDC, ptCurPos, strName.c_str(), strValue.c_str());
	}
}  // end function void SVObserverApp::PrintThreadAffinity( CDC* pDC, ... )

void SVConfigurationPrint::PrintResultIO(CDC* pDC, CPoint& ptCurPos, int nIndentLevel)
{
	CString label, value;
	int		nFirstHeight = 0;
	int		nLastHeight  = 0;
	long	lSize        = 0;
	CPoint	ptTemp(0, 0);

	SVConfigurationObject* pConfig = nullptr;
	SVObjectManagerClass::Instance().GetConfigurationObject( pConfig );
	SVObserverApp* pApp = dynamic_cast<SVObserverApp*>(AfxGetApp());

	// Get the number of PPQs
	long lPPQSize = 0;
	if (!pConfig->GetPPQCount(lPPQSize))
	{
		SvStl::ExceptionMgr1 e; // The default constructor sets the type to LogOnly.
		e.setMessage( SVMSG_SVO_55_DEBUG_BREAK_ERROR, SvO::ErrorGettingPPQCount, StdExceptionParams, Err_17003_SVConfigurationPrint_PrintResultIO_ErrorGettingPPQCount );
		DebugBreak();
	}

	// Print IODoc contents
	if (pApp->GetIODoc())
	{
		SVPPQObject* pPPQ;
		SVDigitalOutputObject* pDigOutput;
		SVIOEntryHostStructPtrList ppIOEntries;

		// Print Result Output title...
		DWORD dwMaxOutput = 0;
		SVIOConfigurationInterfaceClass::Instance().GetDigitalOutputCount( dwMaxOutput );
		value.Format(_T("%ld"), dwMaxOutput);
		ptCurPos.x = nIndentLevel * m_shortTabPixels;
		PrintValueObject(pDC, ptCurPos, _T("Result Outputs:"), value);

		// Result Outputs
		for (long i = 0; i < static_cast<long>(dwMaxOutput); ++i)
		{
			label.Format(_T("Digital Output %d"), i+1);

			SVIOEntryHostStructPtr l_pModuleReady = pConfig->GetModuleReady();

			SVObjectClass* l_pObject = SVObjectManagerClass::Instance().GetObject( l_pModuleReady->m_IOId );

			// Check Module Ready first
			pDigOutput = dynamic_cast< SVDigitalOutputObject* >( l_pObject );
			if (pDigOutput)
			{
				if (i == pDigOutput->GetChannel())
				{
					PrintIOEntryObject(pDC, ptCurPos, nIndentLevel + 1, label, l_pModuleReady);
					continue;
				}// end if
			}// end if

			for (int j = 0; j < lPPQSize; j++)
			{
				if ( !pConfig->GetPPQ( j, &pPPQ ) )
				{
					SvStl::ExceptionMgr1 e; // The default constructor sets the type to LogOnly.
					e.setMessage( SVMSG_SVO_55_DEBUG_BREAK_ERROR, SvO::ErrorGettingPPQ, StdExceptionParams, Err_17004_SVConfigurationPrint_PrintResultIO_ErrorGettingPPQ );
					DebugBreak();
				}

				// Get list of available outputs
				long lIOEntries = 0;
				if ( !pPPQ->GetAllOutputs( ppIOEntries ) )
				{
					SvStl::ExceptionMgr1 e; // The default constructor sets the type to LogOnly.
					e.setMessage( SVMSG_SVO_55_DEBUG_BREAK_ERROR, SvO::ErrorGettingOutputs, StdExceptionParams, Err_17005_SVConfigurationPrint_PrintResultIO_ErrorGettingOutputs );
					DebugBreak();
				}

				lIOEntries = static_cast< long >( ppIOEntries.size() );

				// Find each digital output
				for (int k = 0; k < lIOEntries; k++)
				{
					if ( ppIOEntries[ k ]->m_ObjectType != IO_DIGITAL_OUTPUT ) { continue; }

					SVObjectClass* l_pObject = SVObjectManagerClass::Instance().GetObject( ppIOEntries[k]->m_IOId );

					pDigOutput = dynamic_cast< SVDigitalOutputObject* >( l_pObject );

					if ( !pDigOutput ) { continue; }

					if (i == pDigOutput->GetChannel())
					{
						PrintIOEntryObject(pDC, ptCurPos, nIndentLevel + 1, label, ppIOEntries[k]);
						continue;
					}// end if
				}// end for k
			}// end for j
		}// end for
	}
}  // end function void SVObserverApp::PrintResultIO( CDC* pDC, ... )

void SVConfigurationPrint::PrintMonitorListSection(CDC* pDC, CPoint& ptCurPos, int nIndentLevel)
{
	CString				label, value;
	SVConfigurationObject* pConfig = NULL;
	SVObjectManagerClass::Instance().GetConfigurationObject( pConfig );
	if (pConfig)
	{
		ptCurPos.x = nIndentLevel * m_shortTabPixels;

		PrintValueObject(pDC, ptCurPos, _T("Remote Monitor List"), "");

		const RemoteMonitorList& remoteMonitorLists = pConfig->GetRemoteMonitorList();

		RemoteMonitorList::const_iterator iterMonitorList = remoteMonitorLists.begin();
		while ( remoteMonitorLists.end() != iterMonitorList )
		{
			const SVString& ListName = iterMonitorList->first;
			const RemoteMonitorNamedList& monitorList = iterMonitorList->second;

			// Write out list name
			PrintMonitorListItem(pDC, ptCurPos, nIndentLevel+1, _T("Monitor List Name"), ListName.c_str());

			// Write out PPQ 
			value = monitorList.GetPPQName().c_str();
			PrintMonitorListItem(pDC, ptCurPos, nIndentLevel+1, _T("PPQ"), monitorList.GetPPQName().c_str());

			// Write out reject queue depth
			int Depth = monitorList.GetRejectDepthQueue();
			value.Format("%d",Depth);
			PrintMonitorListItem(pDC, ptCurPos, nIndentLevel+1, _T("Reject Queue Depth"), value);

			// Print Product Value List
			const MonitoredObjectList& ValueList = monitorList.GetProductValuesList();
			MonitoredObjectList::const_iterator vlIt = ValueList.begin();
			PrintMonitorListItem(pDC, ptCurPos, nIndentLevel+1, _T("Product Value List"), "");
			while( vlIt != ValueList.end() )
			{
				const MonitoredObject& rObj = *vlIt;
				const SVString& objectName = RemoteMonitorListHelper::GetNameFromMonitoredObject(rObj);
				if ( !objectName.empty() )
				{
					value = objectName.c_str();
					PrintMonitorListItem(pDC, ptCurPos, nIndentLevel+2, value, "");
				}
				vlIt++;
			}			

			// Print Product Image List
			const MonitoredObjectList& ImageList = monitorList.GetProductImagesList();
			MonitoredObjectList::const_iterator ilIt = ImageList.begin();
			PrintMonitorListItem(pDC, ptCurPos, nIndentLevel+1, _T("Product Image List"), "");
			while( ilIt != ImageList.end() )
			{
				const MonitoredObject& rObj = *ilIt;
				const SVString& objectName = RemoteMonitorListHelper::GetNameFromMonitoredObject(rObj);
				if ( !objectName.empty() )
				{
					value = objectName.c_str();
					PrintMonitorListItem(pDC, ptCurPos, nIndentLevel+2, value, "");
				}
				ilIt++;
			}

			// Print Reject Condition List
			const MonitoredObjectList& RejectList = monitorList.GetRejectConditionList();
			MonitoredObjectList::const_iterator rlIt = RejectList.begin();
			PrintMonitorListItem(pDC, ptCurPos, nIndentLevel+1, _T("Reject Condition List"), "");
			while( rlIt != RejectList.end() )
			{
				const MonitoredObject& rObj = *rlIt;
				const SVString& objectName = RemoteMonitorListHelper::GetNameFromMonitoredObject(rObj);
				if ( !objectName.empty() )
				{
					value = objectName.c_str();
					PrintMonitorListItem(pDC, ptCurPos, nIndentLevel+2, value, "");
				}
				rlIt++;
			}

			// Print Fail Status List
			const MonitoredObjectList& FailList = monitorList.GetFailStatusList();
			MonitoredObjectList::const_iterator flIt = FailList.begin();
			PrintMonitorListItem(pDC, ptCurPos, nIndentLevel+1, _T("Fail Status List"), "");
			while( flIt != FailList.end() )
			{
				const MonitoredObject& rObj = *flIt;
				const SVString& objectName = RemoteMonitorListHelper::GetNameFromMonitoredObject(rObj);
				if ( !objectName.empty() )
				{
					value = objectName.c_str();
					PrintMonitorListItem(pDC, ptCurPos, nIndentLevel+2, value, "");
				}
				flIt++;
			}
			iterMonitorList++;
		}
	}
}

SVDeviceParamConfigPrintHelper::SVDeviceParamConfigPrintHelper(
	SVConfigurationPrint* pPrint, SVDeviceParamCollection& rCamFileParams, CDC* pDC, CPoint& ptCurPos, int nIndentLevel)
	: m_pPrint(pPrint), m_rCamFileParams(rCamFileParams), m_pDC(pDC), m_rptCurPos(ptCurPos), m_nIndentLevel(nIndentLevel)
{
}

HRESULT SVDeviceParamConfigPrintHelper::Visit(SVDeviceParam& param )
{
	return S_OK;
}

HRESULT SVDeviceParamConfigPrintHelper::Visit( SVLongValueDeviceParam& param )
{
	const SVLongValueDeviceParam* pCamFileParam = m_rCamFileParams.Parameter( param.Type() ).DerivedValue( pCamFileParam );
	if ( pCamFileParam )
	{
		CString s;
		if ( pCamFileParam->info.options.size() > 0 )
		{
			SVLongValueDeviceParam::OptionsType::const_iterator iterOption;
			iterOption = std::find( pCamFileParam->info.options.begin(), pCamFileParam->info.options.end(), param.lValue );
			if ( iterOption != pCamFileParam->info.options.end() )
			{
				s = iterOption->strDescription.ToString();
			}
		}
		if ( s.IsEmpty() )
			s.Format("%lu%s", (unsigned long) param.GetScaledValue(), param.info.sUnits.IsEmpty() ? "" : " " + param.info.sUnits );
		m_pPrint->PrintValueObject(m_pDC, m_rptCurPos, CString(pCamFileParam->Name()), s );
	}
	return S_OK;
}

HRESULT SVDeviceParamConfigPrintHelper::Visit( SVi64ValueDeviceParam& param )
{
	const SVi64ValueDeviceParam* pCamFileParam = m_rCamFileParams.Parameter( param.Type() ).DerivedValue( pCamFileParam );
	if ( pCamFileParam )
	{
		CString s;
		s.Format("%I64d", param.iValue );
		m_pPrint->PrintValueObject(m_pDC, m_rptCurPos, CString(pCamFileParam->Name()), s );
	}
	return S_OK;
}

HRESULT SVDeviceParamConfigPrintHelper::Visit( SVBoolValueDeviceParam& param )
{
	const SVBoolValueDeviceParam* pCamFileParam = m_rCamFileParams.Parameter( param.Type() ).DerivedValue( pCamFileParam );
	if ( pCamFileParam )
	{
		CString s;
		if ( pCamFileParam->info.options.size() > 0 )
		{
			SVBoolValueDeviceParam::OptionsType::const_iterator iterOption;
			iterOption = std::find( pCamFileParam->info.options.begin(), pCamFileParam->info.options.end(), param.bValue );
			if ( iterOption != pCamFileParam->info.options.end() )
			{
				s = iterOption->strDescription.ToString();
			}
		}
		if ( s.IsEmpty() )
			s.Format("%s", param.bValue ? _T("TRUE") : _T("FALSE") );
		m_pPrint->PrintValueObject(m_pDC, m_rptCurPos, CString(pCamFileParam->Name()), s );
	}
	return S_OK;
}

HRESULT SVDeviceParamConfigPrintHelper::Visit( SVStringValueDeviceParam& param )
{
	const SVStringValueDeviceParam* pCamFileParam = m_rCamFileParams.Parameter( param.Type() ).DerivedValue( pCamFileParam );
	if ( pCamFileParam )
	{
		m_pPrint->PrintValueObject(m_pDC, m_rptCurPos, pCamFileParam->Name(), param.strValue.ToString() );
	}
	return S_OK;
}

HRESULT SVDeviceParamConfigPrintHelper::Visit( SVParamListDeviceParam& param )
{
	return S_OK;
}

HRESULT SVDeviceParamConfigPrintHelper::Visit( SVLutDeviceParam& param )
{
	return S_OK;
}

HRESULT SVDeviceParamConfigPrintHelper::Visit( SVLightReferenceDeviceParam& param )
{
	return S_OK;
}

HRESULT SVDeviceParamConfigPrintHelper::Visit( SVCameraFormatsDeviceParam& param )
{
	const SVCameraFormatsDeviceParam* pCamFileParam = m_rCamFileParams.Parameter( param.Type() ).DerivedValue( pCamFileParam );
	if ( pCamFileParam )
	{
		CString s;
		SVCameraFormat* pFormat = NULL;
		if ( param.options.size() > 0 )
		{
			SVCameraFormat& rFormat = param.options[ param.strValue ];
			const SVCameraFormat& rCamFileFormat = pCamFileParam->options.find( param.strValue )->second;
			if ( rCamFileFormat.m_strName == param.strValue )
			{
				s = rCamFileFormat.strDescription.ToString();
				pFormat = &rFormat;
			}
		}
		if ( s.IsEmpty() )
			s = param.strValue.ToString();
		m_pPrint->PrintValueObject(m_pDC, m_rptCurPos, pCamFileParam->Name(), s );

		if ( pFormat )
		{
			CPoint pt = m_rptCurPos;
			pt.x += m_pPrint->m_shortTabPixels;

			m_pPrint->PrintValueObject(m_pDC, pt, _T("Left"), AsString(pFormat->lHPos));
			m_pPrint->PrintValueObject(m_pDC, pt, _T("Top"), AsString(pFormat->lVPos));
			m_pPrint->PrintValueObject(m_pDC, pt, _T("Width"), AsString(pFormat->lWidth));
			m_pPrint->PrintValueObject(m_pDC, pt, _T("Height"), AsString(pFormat->lHeight));
			
			m_rptCurPos.y = pt.y;
		}
	}
	return S_OK;
}

HRESULT SVDeviceParamConfigPrintHelper::Visit(SVCustomDeviceParam& param)
{
	const SVCustomDeviceParam* pParam = m_rCamFileParams.Parameter(param.Type()).DerivedValue(pParam);
	if (pParam)
	{
		SVDeviceParamWrapper w(pParam->GetHeldParam());
		SVDeviceParam* p = w;
		Visit(*p);
	}
	return S_OK;
}

//******************************************************************************
//* LOG HISTORY:
//******************************************************************************
/*
$Log:   N:\PVCSarch65\ProjectFiles\archives\SVObserver_SRC\SVObserver\SVConfigurationPrint.cpp_v  $
 * 
 *    Rev 1.16   06 Jan 2015 10:21:34   tbair
 * Project:  SVObserver
 * Change Request (SCR) nbr:  960
 * SCR Title:  Pipe/core management
 * Checked in by:  tBair;  Tom Bair
 * Change Description:  
 *   Add print capability for Thread Management
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.15   18 Sep 2014 12:51:58   sjones
 * Project:  SVObserver
 * Change Request (SCR) nbr:  945
 * SCR Title:  Configuration Print Report doesn't print correctly under 64 Bit
 * Checked in by:  sJones;  Steve Jones
 * Change Description:  
 *   Revised to call TextOut instead of DrawText.
 * Added word wrap logic.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.14   23 Jul 2014 10:34:08   ryoho
 * Project:  SVObserver
 * Change Request (SCR) nbr:  876
 * SCR Title:  Add user mask image to print configuration
 * Checked in by:  bWalter;  Ben Walter
 * Change Description:  
 *   changd PrintDetails for the SVUserMaskOperatorClass to check to make sure that the SVImageClass is not NULL
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.13   16 Jul 2014 07:53:50   ryoho
 * Project:  SVObserver
 * Change Request (SCR) nbr:  886
 * SCR Title:  Add RunReject Server Support to SVObserver
 * Checked in by:  rYoho;  Rob Yoho
 * Change Description:  
 *   added methods PrintMonitorListSection and PrintMonitorListItem
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.0   16 Jul 2014 07:52:24   ryoho
 * Project:  SVObserver
 * Change Request (SCR) nbr:  886
 * SCR Title:  Add RunReject Server Support to SVObserver
 * Checked in by:  rYoho;  Rob Yoho
 * Change Description:  
 *   added methods PrintMonitorListSection and PrintMonitorListItem
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.12   14 Jul 2014 14:54:16   sjones
 * Project:  SVObserver
 * Change Request (SCR) nbr:  906
 * SCR Title:  SVObserver Tool Grouping
 * Checked in by:  sJones;  Steve Jones
 * Change Description:  
 *   Added PrintAllChildren method.
 * Removed GetToolGroup function.
 * Aded GetToolGroupings function.
 * Added GetTool function.
 * Revised PrintObject method.
 * Revised PrintChildren method.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.11   11 Jul 2014 09:50:06   sjones
 * Project:  SVObserver
 * Change Request (SCR) nbr:  906
 * SCR Title:  SVObserver Tool Grouping
 * Checked in by:  sJones;  Steve Jones
 * Change Description:  
 *   Added GetToolGroup static function.
 * Revised PrintObject to print tool groupings.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.10   01 Jul 2014 15:12:10   ryoho
 * Project:  SVObserver
 * Change Request (SCR) nbr:  913
 * SCR Title:  Add Blob Feature Default Value to the Configuration Print (SVO-214)
 * Checked in by:  rYoho;  Rob Yoho
 * Change Description:  
 *   changed PrintDetails - to check for BlobAnalyzerResults and to print the default values for the features
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.9   15 May 2014 11:10:30   sjones
 * Project:  SVObserver
 * Change Request (SCR) nbr:  852
 * SCR Title:  Add Multiple Platform Support to SVObserver's Visual Studio Solution
 * Checked in by:  tBair;  Tom Bair
 * Change Description:  
 *   Revised SVSendMessage to use DWORD_PTR
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.8   04 Feb 2014 13:24:48   bwalter
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
 *    Rev 1.7   01 Feb 2014 10:32:22   tbair
 * Project:  SVObserver
 * Change Request (SCR) nbr:  852
 * SCR Title:  Add Multiple Platform Support to SVObserver's Visual Studio Solution
 * Checked in by:  tBair;  Tom Bair
 * Change Description:  
 *   Changed sendmessage to use LONG_PTR instead of DWORD.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.6   03 Dec 2013 13:50:48   bwalter
 * Project:  SVObserver
 * Change Request (SCR) nbr:  876
 * SCR Title:  Add user mask image to print configuration
 * Checked in by:  bWalter;  Ben Walter
 * Change Description:  
 *   Changed PrintDetails method to get "Image Source" from string table (IDS_IMAGE_SOURCE_STRING) and print the image source for SVUserMaskOperatorClass.
 * Removed empty lines, empty comments, and dead code.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.5   01 Oct 2013 12:48:18   tbair
 * Project:  SVObserver
 * Change Request (SCR) nbr:  852
 * SCR Title:  Add Multiple Platform Support to SVObserver's Visual Studio Solution
 * Checked in by:  tBair;  Tom Bair
 * Change Description:  
 *   Add x64 platform.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.4   13 Aug 2013 09:48:48   bWalter
 * Project:  SVObserver
 * Change Request (SCR) nbr:  814
 * SCR Title:  Upgrade SVObserver to Compile Using Visual Studio 2010
 * Checked in by:  bWalter;  Ben Walter
 * Change Description:  
 *   Included SVShapeMaskHelperClass.h instead of SVOperator.h.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.3   07 Aug 2013 13:26:24   sjones
 * Project:  SVObserver
 * Change Request (SCR) nbr:  809
 * SCR Title:  Non I/O SVIM
 * Checked in by:  tBair;  Tom Bair
 * Change Description:  
 *   Renamed SVIOEntryHostStructPtrVector to SVIOEntryHostStructPtrList.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.2   18 Jun 2013 19:22:30   bwalter
 * Project:  SVObserver
 * Change Request (SCR) nbr:  814
 * SCR Title:  Upgrade SVObserver to Compile Using Visual Studio 2010
 * Checked in by:  bWalter;  Ben Walter
 * Change Description:  
 *   Merged with svo_src label SVO 6.10 Beta 019.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.76   03 Jun 2013 03:24:36   jHanebach
 * Project:  SVObserver
 * Change Request (SCR) nbr:  841
 * SCR Title:  Change configuration print report format to xml
 * Checked in by:  jHanebach;  Jack Hanebach
 * Change Description:  
 *   Added accessor for no-print guids.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.1   06 May 2013 20:05:58   bWalter
 * Project:  SVObserver
 * Change Request (SCR) nbr:  814
 * SCR Title:  Upgrade SVObserver to Compile Using Visual Studio 2010
 * Checked in by:  bWalter;  Ben Walter
 * Change Description:  
 *   Merged with svo_src label SVO 6.10 Beta 010.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.75   16 Apr 2013 14:15:08   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  822
 * SCR Title:  Remove DDE Input and Output functionality from SVObserver
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Removed DDE functionality from the source code.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.0   23 Apr 2013 10:02:52   bWalter
 * Project:  SVObserver
 * Change Request (SCR) nbr:  814
 * SCR Title:  Upgrade SVObserver to Compile Using Visual Studio 2010
 * Checked in by:  bWalter;  Ben Walter
 * Change Description:  
 *   Initial check in to SVObserver_src.  (Merged with svo_src label SVO 6.10 Beta 008.)
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.74   01 Apr 2013 16:03:20   sjones
 * Project:  SVObserver
 * Change Request (SCR) nbr:  820
 * SCR Title:  Baumer Camera Trigger Debounce
 * Checked in by:  sJones;  Steve Jones
 * Change Description:  
 *   Added support for SVCustomDeviceParam
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.73   27 Mar 2013 12:43:38   tbair
 * Project:  SVObserver
 * Change Request (SCR) nbr:  813
 * SCR Title:  Remove unused Class IDs from the SVCLSIDs
 * Checked in by:  tBair;  Tom Bair
 * Change Description:  
 *   Removed references to unused GUID pointers in the NotPrintArray.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.72   11 Feb 2013 14:16:40   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  812
 * SCR Title:  Add New Remote Command Functionality
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated source code to use the new framework for the remote socket interface.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.71   04 Oct 2012 11:06:38   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  602
 * SCR Title:  Revise the Toolset Parsing and Object Creation Methodology
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Remove scope declaration on header methods.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.70   04 Oct 2012 10:38:42   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  602
 * SCR Title:  Revise the Toolset Parsing and Object Creation Methodology
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Changed from post incrementor to pre incrementor.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.69   15 Aug 2012 14:30:26   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  779
 * SCR Title:  Fix Problems with DDE Inputs and DDE Outputs
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated source code to use the new Singlton version of the SVIOConfigurationInterfaceClass.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.68   05 Jul 2012 14:12:44   tbair
 * Project:  SVObserver
 * Change Request (SCR) nbr:  764
 * SCR Title:  Add tool comments to all tools in SVObserver
 * Checked in by:  tBair;  Tom Bair
 * Change Description:  
 *   New dialogs to allow tool comment editing
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.67   19 Jun 2012 10:32:42   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  744
 * SCR Title:  Add Shared Memory and Socket Functionality for Run Page Web Server
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Protected Object Information structure attributes.  This change will add accessor methods to get and set attributes.
 * Converted GUID to SVGUID.   This allows the use of standard operators for comparison and methods for conversion.
 * Centralized the location of the configuration object identifier and removed synchronization problems.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.66   28 Feb 2012 09:03:52   ryoho
 * Project:  SVObserver
 * Change Request (SCR) nbr:  752
 * SCR Title:  Fix configuration print for the Archive Tool
 * Checked in by:  rYoho;  Rob Yoho
 * Change Description:  
 *   Will printout complete object name for the Archive Tool Results
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.65   30 Jan 2012 11:09:30   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  748
 * SCR Title:  Add Remote Output Steams to SVObserver
 * Checked in by:  jSpila;  Joseph Spila
 * Change Description:  
 *   Modified source code to consolidate PLC and New Output stream functionality to I/O Controller.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.64   16 Sep 2011 15:50:00   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  730
 * SCR Title:  Adjust SVObserver to fix issues with Inspection resource handshaking
 * Checked in by:  tBair;  Tom Bair
 * Change Description:  
 *   Updated camera management functionality.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.63   07 Sep 2011 11:22:34   tbair
 * Project:  SVObserver
 * Change Request (SCR) nbr:  738
 * SCR Title:  Change Configuration Print order for Conditional Equations
 * Checked in by:  tBair;  Tom Bair
 * Change Description:  
 *   Changed order of "Equation Text" to be after the owner name. Also forced the label "Equation Text"to be printed reguardless of wether the equation is blank or not.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.62   19 Apr 2011 16:07:18   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  707
 * SCR Title:  Change Inspection Display Functionality to Force Display of Last Inspected
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated code to fix issues with IO Assignment.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.61   18 Apr 2011 09:22:14   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  707
 * SCR Title:  Change Inspection Display Functionality to Force Display of Last Inspected
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated code to move the shared data for the IO Entry data to a different class to fix issues with PPQ and Inspection shared data.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.60   22 Mar 2011 07:45:12   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  707
 * SCR Title:  Change Inspection Display Functionality to Force Display of Last Inspected
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated static global classes to singletons to promote proper and defined construction and destruction.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.59   16 Mar 2011 13:23:04   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  712
 * SCR Title:  Fix issues with black images when using command interface (SIAC)
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated extent functionality and renamed extent methods to correct extent issues and add clarity to the logical flow.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.58   18 Feb 2011 09:58:04   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  700
 * SCR Title:  Remove String Buffer Problems
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated source to remove duplicate string class, and fixed string conversion issues.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.57   15 Dec 2010 09:49:52   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  707
 * SCR Title:  Change Inspection Display Functionality to Force Display of Last Inspected
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated source code to fix connections and synchronization issues with the IO Sub-system.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.56   13 Dec 2010 11:15:06   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  707
 * SCR Title:  Change Inspection Display Functionality to Force Display of Last Inspected
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Change the usage of _variant_t to clear the variable after use to eliminate invalid variant type for assignment.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.55   08 Dec 2010 07:51:20   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  707
 * SCR Title:  Change Inspection Display Functionality to Force Display of Last Inspected
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated source code to include changes in notification functionality using the Observer Design Pattern.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.54   01 Jun 2010 14:13:04   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  693
 * SCR Title:  Fix Performance Issue with Inspection Process
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated source code to improve ability to track performance.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.53   01 Jun 2010 10:34:42   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  693
 * SCR Title:  Fix Performance Issue with Inspection Process
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated code to remove redundent methodologies and fix missing or incorrect calling functionality.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.52   15 Dec 2009 15:13:48   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  677
 * SCR Title:  Fix problem in camera notify thread
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated include information and comments.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.51   30 Jul 2009 11:18:02   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  665
 * SCR Title:  Fix unrecoverable failure when processing acquisitions at high speed
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Merged branced changes into current code branch with appropriate updates.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.50   03 Jun 2009 09:50:58   sjones
 * Project:  SVObserver
 * Change Request (SCR) nbr:  650
 * SCR Title:  Integrate Gigabit ethernet cameras
 * Checked in by:  sJones;  Steve Jones
 * Change Description:  
 *   Revised Include file definitions due to changes in SVDeviceParams
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.49   05 Dec 2008 15:26:20   sjones
 * Project:  SVObserver
 * Change Request (SCR) nbr:  634
 * SCR Title:  Implement a File Acquistion Device
 * Checked in by:  sJones;  Steve Jones
 * Change Description:  
 *   Revised Loading modes for File Acquisiiton
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.48   08 Oct 2008 09:17:36   sjones
 * Project:  SVObserver
 * Change Request (SCR) nbr:  634
 * SCR Title:  Implement a File Acquistion Device
 * Checked in by:  sJones;  Steve Jones
 * Change Description:  
 *   Revised Camera print section for File Acquisiton settings
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.47   07 Oct 2008 16:20:10   sjones
 * Project:  SVObserver
 * Change Request (SCR) nbr:  612
 * SCR Title:  Implement a timer trigger object
 * Checked in by:  sJones;  Steve Jones
 * Change Description:  
 *   Revised the trigger print section for the Software Trigger
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.46   10 Sep 2008 16:47:08   sjones
 * Project:  SVObserver
 * Change Request (SCR) nbr:  612
 * SCR Title:  Implement a timer trigger object
 * Checked in by:  sJones;  Steve Jones
 * Change Description:  
 *   Revised Trigger Device Name access
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.45   21 Jun 2007 12:01:28   Joe
 * Project:  SVObserver
 * Change Request (SCR) nbr:  598
 * SCR Title:  Upgrade SVObserver to compile using vc++ in VS2005
 * Checked in by:  jSpila;  Joseph Spila
 * Change Description:  
 *   These changes include modification based on fixing compiler-based and project-based differences between VC6 and VC8.  These changes mainly include casting issues, but some include type conversion and assignment of new compiler controlling defines.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.44.1.0   14 Jul 2009 12:44:30   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  665
 * SCR Title:  Fix unrecoverable failure when processing acquisitions at high speed
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Udated code to use the new data manager objects and signal processing.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.44   04 Aug 2006 08:56:52   ryoho
 * Project:  SVObserver
 * Change Request (SCR) nbr:  576
 * SCR Title:  Fix print report to start I/O counting at 1 instead of 0
 * Checked in by:  rYoho;  Rob Yoho
 * Change Description:  
 *   fixed config print to to start counting I/O at 1 instead of 0 to match the I/O page
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.43   19 Jan 2006 10:24:48   Joe
 * Project:  SVObserver
 * Change Request (SCR) nbr:  534
 * SCR Title:  Add Image Select for Auxiliary Extent Source
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated methodology to get the input info objects and ingnor the Aux Extent input info.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.42   10 Nov 2005 06:44:12   Joe
 * Project:  SVObserver
 * Change Request (SCR) nbr:  516
 * SCR Title:  Allow SVObserver to use the Intek 1394 driver for camera acquisition
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Update code to rename member variable for more clarity.  Used new SVObserverApp method to determine type of hardware.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.41   14 Oct 2005 11:18:56   Joe
 * Project:  SVObserver
 * Change Request (SCR) nbr:  500
 * SCR Title:  Reduce delay when adjusting tool parameters with a large toolset
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Added missing blob feature guids to the do not print list.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.40   22 Aug 2005 10:09:12   Joe
 * Project:  SVObserver
 * Change Request (SCR) nbr:  504
 * SCR Title:  Fix lock-up problem when pending product count is larger than PPQ size
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated methods to include changes to SVProductInfoStruct and supporting classes to handle proper assignment and copy construction.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.39   18 Aug 2005 10:27:28   ryoho
 * Project:  SVObserver
 * Change Request (SCR) nbr:  482
 * SCR Title:  New vector based mask
 * Checked in by:  eBeyeler;  Eric Beyeler
 * Change Description:  
 *   Fixed problem with object names not printing
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.38   08 Aug 2005 13:58:12   ebeyeler
 * Project:  SVObserver
 * Change Request (SCR) nbr:  450
 * SCR Title:  Add asynchronous functionality to the archive tool
 * Checked in by:  eBeyeler;  Eric Beyeler
 * Change Description:  
 *   SVToolArchive renamed to SVArchiveTool
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.37   27 Jul 2005 16:05:58   ebeyeler
 * Project:  SVObserver
 * Change Request (SCR) nbr:  464
 * SCR Title:  Add array indexing for value objects
 * Checked in by:  eBeyeler;  Eric Beyeler
 * Change Description:  
 *   updated StatisticsTool to use SVObjectReference (array value objects) instead of GUIDs
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.36   19 Jul 2005 14:49:34   ebeyeler
 * Project:  SVObserver
 * Change Request (SCR) nbr:  464
 * SCR Title:  Add array indexing for value objects
 * Checked in by:  eBeyeler;  Eric Beyeler
 * Change Description:  
 *   implemented new value object Get/Set
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.35   14 Jul 2005 12:29:40   ebeyeler
 * Project:  SVObserver
 * Change Request (SCR) nbr:  464
 * SCR Title:  Add array indexing for value objects
 * Checked in by:  eBeyeler;  Eric Beyeler
 * Change Description:  
 *   changed Archive Tool printing to handle array objects
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.34   22 Jun 2005 09:19:52   ebeyeler
 * Project:  SVObserver
 * Change Request (SCR) nbr:  464
 * SCR Title:  Add array indexing for value objects
 * Checked in by:  eBeyeler;  Eric Beyeler
 * Change Description:  
 *   object attributes use accessor functions
 * value object uses SVHRESULT
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.33   20 Jun 2005 10:09:00   ebeyeler
 * Project:  SVObserver
 * Change Request (SCR) nbr:  450
 * SCR Title:  Add asynchronous functionality to the archive tool
 * Checked in by:  eBeyeler;  Eric Beyeler
 * Change Description:  
 *   updated SVArchiveRecord code
 * updated all casts to proper C++ casts.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.32   20 May 2005 12:26:20   ebeyeler
 * Project:  SVObserver
 * Change Request (SCR) nbr:  450
 * SCR Title:  Add asynchronous functionality to the archive tool
 * Checked in by:  eBeyeler;  Eric Beyeler
 * Change Description:  
 *   Archive member variable name changed
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.31   19 May 2005 14:50:58   ebeyeler
 * Project:  SVObserver
 * Change Request (SCR) nbr:  482
 * SCR Title:  New vector based mask
 * Checked in by:  eBeyeler;  Eric Beyeler
 * Change Description:  
 *   fixed SV_PRINTABLE check
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.30   17 Feb 2005 13:28:04   Joe
 * Project:  SVObserver
 * Change Request (SCR) nbr:  456
 * SCR Title:  Update Image and Tool Objects to use the new Extent Classes
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Update class with new methodologies based on new base object, extents and reset structure.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.29   16 Feb 2005 13:18:48   ryoho
 * Project:  SVObserver
 * Change Request (SCR) nbr:  440
 * SCR Title:  Create Internal Tool Object to Managing Tool Extents and Result Extents
 * Checked in by:  rYoho;  Rob Yoho
 * Change Description:  
 *   updated header files
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.28   02 Apr 2004 12:10:20   ryoho
 * Project:  SVObserver
 * Change Request (SCR) nbr:  413
 * SCR Title:  Use decimal values in Configuration Report
 * Checked in by:  rYoho;  Rob Yoho
 * Change Description:  
 *   changed code so that it would print decimal values
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.27   12 Jan 2004 10:45:36   tbair
 * Project:  SVObserver
 * Change Request (SCR) nbr:  404
 * SCR Title:  Change Camera Parameters that represent time to use standard units
 * Checked in by:  tBair;  Tom Bair
 * Change Description:  
 *   Changed SVDeviceParamHelper(SVLongDeviceParam& param) to print units and scaled value.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.26   10 Oct 2003 12:56:52   rschock
 * Project:  SVObserver
 * Change Request (SCR) nbr:  381
 * SCR Title:  Combine PPQ Data to Inspection Data for a Digital Output
 * Checked in by:  rSchock;  Rosco Schock
 * Change Description:  
 *   Made changes to correct configuration print issues.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.25   10 Oct 2003 12:55:36   rschock
 * Project:  SVObserver
 * Change Request (SCR) nbr:  384
 * SCR Title:  Add new Matrox OCR/OCV analyzer to SVObserver
 * Checked in by:  rSchock;  Rosco Schock
 * Change Description:  
 *   Made changes to correct configuation print issues.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.24   08 Oct 2003 16:23:08   rschock
 * Project:  SVObserver
 * Change Request (SCR) nbr:  385
 * SCR Title:  Expose additional parameters for SIAC
 * Checked in by:  rYoho;  Rob Yoho
 * Change Description:  
 *   Made more changes to the line analyzers to support printing and correct use of the edge related value objects.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.23   27 Aug 2003 10:20:06   ebeyeler
 * Project:  SVObserver
 * Change Request (SCR) nbr:  320
 * SCR Title:  Integrate Matrox Meteor II / 1394 Board and 1394 camera into SVObserver
 * Checked in by:  eBeyeler;  Eric Beyeler
 * Change Description:  
 *   cleaned up file
 * added SVDeviceParamConfigPrintHelper to add printing of device params
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.22   18 Aug 2003 15:38:04   Joe
 * Project:  SVObserver
 * Change Request (SCR) nbr:  322
 * SCR Title:  Add Additional Digital IO Resources to SVObserver
 * Checked in by:  rYoho;  Rob Yoho
 * Change Description:  
 *   Updated PrintResultIO and PrintModuleIO methods with new Digital I/O Interface.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.21   08 Jul 2003 11:53:54   ebeyeler
 * Project:  SVObserver
 * Change Request (SCR) nbr:  320
 * SCR Title:  Integrate Matrox Meteor II / 1394 Board and 1394 camera into SVObserver
 * Checked in by:  eBeyeler;  Eric Beyeler
 * Change Description:  
 *   changed Digital IO
 * changed GetAcquisitionDevice call for Digital system
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.20   22 May 2003 16:23:34   rschock
 * Project:  SVObserver
 * Change Request (SCR) nbr:  342
 * SCR Title:  Configuration report printout changes after going online.
 * Checked in by:  rSchock;  Rosco Schock
 * Change Description:  
 *   Fixed some bugs in the configuration print.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.19   21 May 2003 17:46:40   rschock
 * Project:  SVObserver
 * Change Request (SCR) nbr:  342
 * SCR Title:  Configuration report printout changes after going online.
 * Checked in by:  rSchock;  Rosco Schock
 * Change Description:  
 *   Fixed bug with the printing of DDE outputs.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.18   17 Apr 2003 17:25:22   rschock
 * Project:  SVObserver
 * Change Request (SCR) nbr:  346
 * SCR Title:  Update SVObserver to Version 4.21 Release
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Redid the #include defines and standardized the Tracker log headers and removed warning from release mode builds.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.17   10 Feb 2003 13:29:18   ryoho
 * Project:  SVObserver
 * Change Request (SCR) nbr:  321
 * SCR Title:  Print Config - crashes on certain configurations
 * Checked in by:  rYoho;  Rob Yoho
 * Change Description:  
 *   fixed problem with trying to get the IO list of the PPQ
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.16   29 Jan 2003 15:09:00   joe
 * Project:  SVObserver
 * Change Request (SCR) nbr:  301
 * SCR Title:  Implement Result Image Circle Buffer
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated PrintDetails method to handle the acquisition device information correctly.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.15   13 Jan 2003 08:40:34   ebeyeler
 * Project:  SVObserver
 * Change Request (SCR) nbr:  306
 * SCR Title:  Implement the ability for the SIAC to remotely setup the RGB LUT for a camera
 * Checked in by:  eBeyeler;  Eric Beyeler
 * Change Description:  
 *   Changed function call for SVConfigurationObject::GetAcquisitionObject for LUT
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.14   03 Jan 2003 13:14:16   ryoho
 * Project:  SVObserver
 * Change Request (SCR) nbr:  305
 * SCR Title:  Implement the ability to perform RunOnce from a SIAC client
 * Checked in by:  rYoho;  Rob Yoho
 * Change Description:  
 *   Fixed problems where it would crash SVObserver if no frame grabber was present
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.13   26 Nov 2002 16:03:04   Joe
 * Project:  SVObserver
 * Change Request (SCR) nbr:  311
 * SCR Title:  Fix Light Reference save for RGB Mono
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated SVConfigurationPrint::PrintCameraSummary with new method call GetRootDeviceName.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.12   26 Nov 2002 15:43:10   Joe
 * Project:  SVObserver
 * Change Request (SCR) nbr:  295
 * SCR Title:  Remove Result Data from Configuration Printout
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Added a check in SVConfigurationPrint::PrintDetails to see if a particular value object should be printed.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.11   19 Nov 2002 13:10:32   ryoho
 * Project:  SVObserver
 * Change Request (SCR) nbr:  226
 * SCR Title:  Monochrome SVIM configuration compatibility between ViperQUAD and ViperDUAL
 * Checked in by:  rYoho;  Rob Yoho
 * Change Description:  
 *   Changed left margin to work better with Text file printouts
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.10   13 Nov 2002 11:53:04   cschmittinger
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
 *    Rev 1.9   08 Nov 2002 12:01:42   cschmittinger
 * Project:  SVObserver
 * Change Request(SCR) nbr:  250
 * SCR Title:  The SVObserver Configuration Report is missing data do to page layout..
 * Checked in by:  cSchmittinger;  Carl Schmittinger
 * Change Description:  
 *   Modified SVConfigurationPrint::OnBeginPrinting() to increase the margin at the bottom of the page used to print the page numbers.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.8   31 Oct 2001 13:20:56   cSchmittinger
 * Project:  SVObserver
 * Change Request(SCR) nbr:  247
 * SCR Title:  Print Configuration: Conditional tools that are disabled
 * Checked in by:  cSchmittinger;  Carl Schmittinger
 * Change Description:  
 *   Modified the member function SVConfigurationPrint::PrintDetails to print information about a tool even if it is disabled.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.7   12 Jun 2001 13:11:20   ed
 * Project:  SVObserver
 * Change Request(SCR) nbr:  200
 * SCR Title:  Update code to implement SVIM COM Server
 * Checked in by:  Ed;  Ed Chobanoff
 * Change Description:  
 *   Revised file header.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.6   Apr 17 2001 15:25:22   steve
 * Project:  SVObserver
 * Change Request(SCR) nbr:  196
 * SCR Title:  Restructure Scripted Load/Save Procedures Functional Requirement
 * Checked in by:  Steve;  Stephen S. Steffan
 * Change Description:  
 *   Header file chages due to restructuring of include files.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.5   01 Feb 2001 11:55:06   Steve
 * Project:  SVObserver
 * Change Request(SCR) nbr:  191
 * SCR Title:  Restructure File Management
 * Checked in by:  Steve;  Stephen S. Steffan
 * Change Description:  
 *   Modified files to use the SVFileNameClass and the SVFileNameManagerClass if applicable, if the object refers to a filename.  The SVFileNameManagerClass is used if the Object is a persistant file object, that is, if the file object is saved as part of the configuration data.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.4   25 Jan 2001 16:15:52   Joe
 * Project:  SVObserver
 * Change Request(SCR) nbr:  191
 * SCR Title:  Restructure File Management
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   These changes fixed the SVObserverApp file handling and associated systems.  These systems include the .SEC, .IOD, and .IPD files as well as the camera files.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.3.1.0   16 May 2001 14:07:38   rschock
 * Project:  SVObserver
 * Change Request(SCR) nbr:  202
 * SCR Title:  Operator, delete and cut should not be available in File Open/Save Dialogs
 * Checked in by:  rSchock;  Rosco Schock
 * Change Description:  
 *   Changed all occurences of CFileDialog to CSVFileDialog to enable the new security features and privileges
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.3   26 Apr 2000 11:49:38   Jim
 * Project:  SVObserver
 * Change Request(SCR) nbr:  127
 * SCR Title:  Blob analyzer causes crashes.  Can not load previous version blob tools.
 * Checked in by:  JimAdmin;  James A. Brown
 * Change Description:  
 *   SVBlobXMinAtXMinObjectGuid was changed to SVBlobYMaxAtXMinObjectGuid
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.2   03 Mar 2000 12:29:06   sjones
 * Project:  SVObserver
 * Change Request(SCR) nbr:  112
 * SCR Title:  Expose Extents of Images
 * Checked in by:  sJones;  Steve Jones
 * Change Description:  
 *   Revised to use SVLineInfoClass methods to get line extents.
 * Revised to use SVImageInfoClass methods to get image extents.
 * 
 * Revised to use message to get pointer to SVLineClass.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.1   24 Feb 2000 17:11:48   sMarakkath
 * Project:  SVObserver
 * Change Request(SCR) nbr:  110
 * SCR Title:  Change Configuration Printout to print Tool "extents".
 * Checked in by:  sMarakkath;  Srithaj Marakkath
 * Change Description:  
 *   Modified the functions 'PrintDetails', 'PrintString', 'PrintPage' and the constructor
 * 
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.0   Jan 04 2000 09:21:20   Nick
 * Project:  SVObserver
 * Change Request(SCR) nbr:  82
 * SCR Title:  Configuration Printing
 * Checked in by:  Nick;  F Roland "Nick" Bjorklund
 * Change Description:  
 *   New class to encapsulate the configuration printing function.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
*/