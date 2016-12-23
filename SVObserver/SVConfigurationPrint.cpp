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
//Moved to precompiled header: #include <boost\algorithm\string.hpp>
//Moved to precompiled header: #include <boost\algorithm\string\replace.hpp>

#include "SVConfigurationPrint.h"
#include "SVConfigurationObject.h"

#include "SVIOLibrary\SVIOConfigurationInterfaceClass.h"
#include "CameraLibrary\SVDeviceParams.h"
#include "CameraLibrary\SVBoolValueDeviceParam.h"
#include "CameraLibrary\SVi64ValueDeviceParam.h"
#include "CameraLibrary\SVLongValueDeviceParam.h"
#include "CameraLibrary\SVParamListDeviceParam.h"
#include "CameraLibrary\SVStringValueDeviceParam.h"
#include "CameraLibrary\SVCustomDeviceParam.h"
#include "SVFileAcquisitionDevice/SVFileAcquisitionLoadingModeEnum.h"
#include "SVImageLibrary\SVImagingDeviceParams.h"
#include "SVObjectLibrary\SVObjectClass.h"

#include "SVValueObjectLibrary/SVValueObject.h"
#include "SVEquation.h"
#include "SVOCore/SVImageClass.h"
#include "SVObserver.h"
#include "SVTool.h"
#include "SVToolSet.h"
#include "SVInputObjectList.h"
#include "SVDigitalInputObject.h"
#include "SVDigitalOutputObject.h"
#include "SVArchiveTool.h"
#include "SVArchiveRecord.h"
#include "SVArchiveRecordsArray.h"
#include "SVStatTool.h"
#include "SVObjectLibrary\SVObjectManagerClass.h"
#include "SVAcquisitionClass.h"
#include "SVSystemLibrary/SVFileNameArrayClass.h"
#include "SVImageLibrary\SVLightReference.h"
#include "SVIODoc.h"
#include "TriggerHandling/SVTriggerClass.h"
#include "SVIPDoc.h"
#include "SVShapeMaskHelperClass.h"
#include "SVIOController.h"
#include "SVUserMaskOperatorClass.h"
#include "SVBlobAnalyzer.h"
#include "SVResultDouble.h"
#include "RemoteMonitorList.h"
#include "RemoteMonitorListHelper.h"
#include "SVSystemLibrary\SVThreadManager.h"
#include "ObjectInterfaces\ErrorNumbers.h"
#include "SVStatusLibrary\MessageManager.h"
#include "TextDefinesSvO.h"
#include "RootObject.h"
#pragma endregion Includes

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

static const LPCTSTR ToolGroupCommentLabel = _T("Tool Comment:");

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
	SVIPDoc* pDoc = TheSVObserverApp.GetIPDoc(rInspectionGuid);
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
		SVString ToolName( rToolSet.GetAt(i)->GetName() );
		if ( ToolName == rName) 
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
	m_psBuffer = nullptr;
	
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
		Create(CPrintingDialog::IDD, pParent);      // modeless !
		_afxWinState->m_bUserAbort = FALSE;
	}
	virtual ~CPrintingDialog() { }
	
	virtual BOOL OnInitDialog() override;
	virtual void OnCancel() override;
};


/////////////////////////////////////////////////////////////////////////////
//
//
//
void SVConfigurationPrint::DoPrintConfig()
{
    SVObserverApp* pApp = dynamic_cast <SVObserverApp*> (AfxGetApp());
	
	ASSERT(nullptr != m_printInfo.m_pPD);    // must be set
	
	_AFX_THREAD_STATE* pThreadState = AfxGetThreadState();
	
	if (pThreadState && LOWORD(pThreadState->m_lastSentMsg.wParam) == ID_FILE_PRINT_DIRECT)
	{
		CCommandLineInfo*    pCmdInfo = pApp->m_pCmdInfo;
		
		if (nullptr != pCmdInfo)
		{
			if (pCmdInfo->m_nShellCommand == CCommandLineInfo::FilePrintTo)
			{
				m_printInfo.m_pPD->m_pd.hDC = ::CreateDC(pCmdInfo->m_strDriverName,
													   pCmdInfo->m_strPrinterName, 
													   pCmdInfo->m_strPortName,
													   nullptr);
				
				if (nullptr == m_printInfo.m_pPD->m_pd.hDC)
				{
					CString message;
					message.Format(AFX_IDP_FAILED_TO_START_PRINT);
					SvStl::MessageMgrStd Msg( SvStl::LogAndDisplay );
					Msg.setMessage( SVMSG_SVO_93_GENERAL_WARNING, message, SvStl::SourceFileParams(StdMessageParams), SvOi::Err_10238 );
					return;
				}  // end if( nullptr == printInfo.m_pPD->m_pd.hDC  )
			}  // end if( pCmdInfo->m_nShellCommand == CCommandLineInfo::FilePrintTo )
		}  // end if( nullptr != pCmdInfo )
		
		m_printInfo.m_bDirect = TRUE;
	}  // end if(   pThreadState && ...
	
	if (DoPreparePrinting(&m_printInfo))
	{
		// hDC must be set (did you remember to call DoPreparePrinting?)
		ASSERT(nullptr != m_printInfo.m_pPD->m_pd.hDC);
		
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
			docInfo.lpszOutput = nullptr;
			
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
			
			CString message;
			message.Format(AFX_IDP_FAILED_TO_START_PRINT);
			SvStl::MessageMgrStd Msg( SvStl::LogAndDisplay );
			Msg.setMessage( SVMSG_SVO_93_GENERAL_WARNING, message, SvStl::SourceFileParams(StdMessageParams), SvOi::Err_10239 );
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
	BOOL bResult = wnd.CreateEx(0, _T("Button"), _T(""), 0, rect, nullptr, 0);
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

BOOL SVConfigurationPrint::DoPreparePrinting(CPrintInfo* pPrintInfo)
{
    SVObserverApp* pApp = dynamic_cast <SVObserverApp*> (AfxGetApp());
	ASSERT(nullptr != pPrintInfo);
	ASSERT(nullptr != pPrintInfo->m_pPD);
	
	if (pPrintInfo->m_pPD->m_pd.nMinPage > pPrintInfo->m_pPD->m_pd.nMaxPage)
		pPrintInfo->m_pPD->m_pd.nMaxPage = pPrintInfo->m_pPD->m_pd.nMinPage;
	
	// don't prompt the user if we're doing print preview, printing directly,
	// or printing via IPrint and have been instructed not to ask
	
	if (pPrintInfo->m_bPreview || pPrintInfo->m_bDirect ||
		(pPrintInfo->m_bDocObject && !(pPrintInfo->m_dwFlags & PRINTFLAG_PROMPTUSER)))
	{
		if (nullptr == pPrintInfo->m_pPD->m_pd.hDC)
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
			
			if (nullptr == pPrintInfo->m_pPD->m_pd.hDC)
			{
				// call CreatePrinterDC if DC was not created by above
				if (nullptr == pPrintInfo->m_pPD->CreatePrinterDC())
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
	
	ASSERT(nullptr != pPrintInfo->m_pPD);
	
	ASSERT(nullptr != pPrintInfo->m_pPD->m_pd.hDC);
	
	if (nullptr == pPrintInfo->m_pPD->m_pd.hDC)
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
	
	SVValueObjectClass* pValueObject = dynamic_cast<SVValueObjectClass*> (pObj);
	// If object is a value object, get embedded ID.
	if (nullptr != pValueObject)
	{
		guidObjID = pObj->GetEmbeddedID();
	}
	
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
	if ( nullptr != pValueObject )
	{
		if ( pObj->ObjectAttributesAllowed() & SV_PRINTABLE )
		{
			BOOL bGotValue = FALSE;

			if ( SVDWordValueObjectClass* pdwValueObject = dynamic_cast <SVDWordValueObjectClass*> ( pValueObject ) )
			{
				DWORD dwValue=0;
				bGotValue = (S_OK == pdwValueObject->GetValue( dwValue ));
				sValue = AsString( dwValue );
			}
			else
			{
				bGotValue = (S_OK == pValueObject->GetValue( sValue ));
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
		}   // end if ( pObj->uObjectAttributesAllowed & SV_PRINTABLE )
	}  // end if( nullptr != pValueObject )
	else
	{
		do
		{
			if ( dynamic_cast <SVShapeMaskHelperClass*> (pObj) )
				if ( !( pObj->ObjectAttributesAllowed() & SV_PRINTABLE) )	// EB 20050818 - hack this instead of doing it right
					break;

			SVToolClass* pTool = dynamic_cast<SVToolClass*> (pObj);
			if ( nullptr != pTool )
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
			}
			
			ptCurPos.x   = nIndentLevel * m_shortTabPixels;
			PrintValueObject(pDC, ptCurPos, _T("Name:"), strName);
			
			// If the object type is different from the name, print the type.
			if (strName != strType && !strType.IsEmpty())
			{
				PrintValueObject(pDC, ptCurPos, _T("Type:"), strType);
			}
			
			// Print the tool length, width, extends, etc here.
			if (bPrintToolExtents && nullptr != pTool)
			{
				bPrintToolExtents = FALSE;
				
				SVInputInfoListClass    toolInputList;
				SVImageClass*           pCurrentSourceImage = nullptr;
				SVInObjectInfoStruct*   pImageInputInfo = nullptr;
				
				SVInObjectInfoStruct* l_psvImageInfo = nullptr;
				SVInObjectInfoStruct* l_psvLastImageInfo = nullptr;

				while( nullptr == l_psvImageInfo && S_OK == pTool->FindNextInputImageInfo( l_psvImageInfo, l_psvLastImageInfo ) )
				{
					if( nullptr != l_psvImageInfo )
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
						l_psvImageInfo = nullptr;
					}
				}

				const SVImageInfoClass* pImageInfo = pTool->getFirstImageInfo();
				if (pImageInfo)
				{
					POINT l_oPoint;
					POINT l_oOutputPoint;

					long l_lWidth = 0;
					long l_lHeight = 0;

					if ( S_OK == pImageInfo->GetExtentProperty( SVExtentPropertyPositionPoint, l_oPoint ) &&
						 S_OK == pImageInfo->GetExtentProperty( SVExtentPropertyOutputPositionPoint, l_oOutputPoint ) &&
						 S_OK == pImageInfo->GetExtentProperty( SVExtentPropertyWidth, l_lWidth ) &&
						 S_OK == pImageInfo->GetExtentProperty( SVExtentPropertyHeight, l_lHeight ) )
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
			} // End, if(bPrintToolExtents && ( nullptr != pTool ))
			
			if ( SVArchiveTool* pArchiveTool = dynamic_cast <SVArchiveTool*> (pObj) )
			{
				int i, s;
				SVArchiveRecord* pRecord;
				
				ptTemp      = ptCurPos;
				ptCurPos.y += PrintString(pDC, ptTemp, _T("Results"));
				ptCurPos.x  = ++nIndentLevel * m_shortTabPixels;
				s = pArchiveTool->m_arrayResultsInfoObjectsToArchive.GetSize();
				for (i = 0; i < s; i++)
				{
					pRecord = pArchiveTool->m_arrayResultsInfoObjectsToArchive.GetAt(i);
					if (nullptr != pRecord->GetObjectReference().Object())
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
				s = pArchiveTool->m_arrayImagesInfoObjectsToArchive.GetSize();
				for (i = 0; i < s; i++)
				{
					pRecord = pArchiveTool->m_arrayImagesInfoObjectsToArchive.GetAt(i);
					ptCurPos.x   = nIndentLevel * m_shortTabPixels;
					CString sFormat;
					sFormat.Format(_T("Image %d:"), i + 1);
					PrintValueObject(pDC, ptCurPos, sFormat, pRecord->GetImageObjectName());
				}
				
				nIndentLevel -= 2;
				ptCurPos.x   = nIndentLevel * m_shortTabPixels;
			}// end if ( SVArchiveTool* pArchiveTool = dynamic_cast <SVArchiveTool*> (pObj) )
			
			if ( SVStatisticsToolClass* pStatisticsTool = dynamic_cast<SVStatisticsToolClass*> (pObj) )
			{
				SVObjectReference refObject = pStatisticsTool->GetVariableSelected();
				if (refObject.Object())
				{
					CString sName = refObject.GetName();
					ptCurPos.x   = nIndentLevel * m_shortTabPixels;
					PrintValueObject(pDC, ptCurPos, _T("Variable"), sName);
				}
			}  

			if (SVUserMaskOperatorClass* maskObj = dynamic_cast <SVUserMaskOperatorClass*>( pObj ))
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

			if (SVDoubleResultClass* pBlobResult = dynamic_cast<SVDoubleResultClass*>(pObj))
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
		} while (false);// end do
	}// End if( nullptr != pValueObject ) else

	if ( SVEquationClass* pEquation = dynamic_cast <SVEquationClass*> (pObj) )
	{
		pEquation->GetEquationText(sValue);
		
		// Print the equation text.
		if (sLabel.IsEmpty())
			sLabel = _T("Equation Text");
		ptCurPos.x  = nIndentLevel * m_shortTabPixels;
		PrintValueObject(pDC, ptCurPos, sLabel, sValue);
	}
	
	if (SV_IS_KIND_OF(pObj, SVTaskObjectClass))
	{
		PrintInputOutputList(pDC, pObj, ptCurPos, nIndentLevel);
	}
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

							// print the tool grouping comment - do not do for SIAC
							if (!m_isPrintToStringBuffer)
							{
								ptCurPos.x = (nIndentLevel + 1) * m_shortTabPixels;
								PrintValueObject(pDC, ptCurPos, ToolGroupCommentLabel, it->second.m_comment.c_str());
							}
						}
						break;

						case ToolGroupData::EndOfGroup:
						{
							CString sLabel;
							sLabel.Format("End Tool Grouping: %s", it->first.c_str());
							ptCurPos.x  = nIndentLevel * m_shortTabPixels;
							CPoint ptTemp = ptCurPos;
							ptCurPos.y += PrintString(pDC, ptTemp, sLabel);

							// print the tool grouping comment - do not do for SIAC
							if (!m_isPrintToStringBuffer)
							{
								ptCurPos.x = (nIndentLevel + 1) * m_shortTabPixels;
								PrintValueObject(pDC, ptCurPos, ToolGroupCommentLabel, it->second.m_comment.c_str());
							}
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
	}
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
	
	// remove all carriage returns
	std::string tmp = text;
	boost::replace_all(tmp, "\r", "");

	// split into multiple lines based on linefeed
	typedef std::deque<std::string> split_container_type;
	split_container_type splitContainer;
	boost::algorithm::split(splitContainer, tmp, boost::algorithm::is_any_of("\n"), boost::algorithm::token_compress_off);
	// Iterate over the container of lines and do Word Wrap
	std::for_each(splitContainer.begin(), splitContainer.end(), [&numChars, &lines](const std::string& rText)->void 
	{
		CString tmp(rText.c_str());
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
	}
	); // end of for_each
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
		ptCurPos.y  += std::max(nFirstHeight, nLastHeight); 
    }
}

void SVConfigurationPrint::PrintIOEntryObject(CDC* pDC, CPoint& ptCurPos, int nIndentLevel, LPCTSTR lpszName, SVIOEntryHostStructPtr IOEntry)
{
	SVDigitalInputObject	*pDigInput = nullptr;
	SVDigitalOutputObject	*pDigOutput = nullptr;
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
	SVDigitalInputObject	*pDigInput = nullptr;
	SVDigitalOutputObject	*pDigOutput = nullptr;
	SVString				sValue;
	
	ptCurPos.x = nIndentLevel * m_shortTabPixels;
	
	PrintValueObject(pDC, ptCurPos, lpszName, lpszValue);
}

void SVConfigurationPrint::OnVirtualPrint(BOOL bRealPrintInput /* = FALSE */) 
{
	SVObserverApp*         pApp    = dynamic_cast <SVObserverApp*> (AfxGetApp());

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
					  nullptr,							   // date to be formatted, nullptr = use current
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
					  nullptr,								// time to be formatted, nullptr = use current
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
			nullptr									    // pointer to typeface name string 
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
			nullptr										// pointer to typeface name string 
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
			nullptr										// pointer to typeface name string 
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
			nullptr									    // pointer to typeface name string 
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
			nullptr										// pointer to typeface name string 
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
	SVConfigurationObject* pConfig( nullptr );
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
	
	//The nullptr check here is enough because then lSize would be 0
	if( nullptr != pConfig){ lSize = pConfig->GetCameraCount(); }
	for (long l = 0; l < lSize; l++)
	{
		SVVirtualCamera* pCamera = pConfig->GetCamera(l);
		if( nullptr != pCamera )
		{
			SVFileNameArrayClass* pfnac = nullptr;
			SVLightReference* plrcDummy = nullptr;
			SVLut* plutDummy = nullptr;
			SVDeviceParamCollection* pDeviceParams = nullptr;
			SVAcquisitionClassPtr pAcqDevice = pCamera->GetAcquisitionDevice();
			ASSERT( !( pAcqDevice.empty() ) );
			BOOL bOk = pConfig->GetAcquisitionDevice( pAcqDevice->DeviceName(), pfnac, plrcDummy, plutDummy, pDeviceParams );
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
								if ( nullptr == pCamFileParam )
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
	SVConfigurationObject* pConfig( nullptr );
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
	
	//The nullptr check here is enough because then lSize would be 0
	if( nullptr != pConfig) { lSize = pConfig->GetTriggerCount(); }
	for (long l = 0; l < lSize; l++)
	{
		SvTi::SVTriggerObject* pTrigger = pConfig->GetTrigger(l);
		if( nullptr != pTrigger )
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
	SVConfigurationObject* pConfig( nullptr );
	SVObjectManagerClass::Instance().GetConfigurationObject( pConfig );
	
	pDC->SelectObject(&m_fontSection);
	ptCurPos.x  = nIndentLevel * m_shortTabPixels;
	ptTemp      = ptCurPos;
	ptCurPos.y += PrintString(pDC, ptTemp, _T("\nInspection"));
	pDC->SelectObject(&m_fontText);
	
	//The nullptr check here is enough because then lSize would be 0
	if( nullptr != pConfig) { lSize = pConfig->GetInspectionCount(); }
	for (long l = 0; l < lSize; l++)
	{
		SVInspectionProcess* pInspect = pConfig->GetInspection(l);
		if( nullptr != pInspect )
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
	
	SVConfigurationObject* pConfig( nullptr );
	SVObjectManagerClass::Instance().GetConfigurationObject( pConfig );
	
	pDC->SelectObject(&m_fontSection);
	ptCurPos.x  = nIndentLevel * m_shortTabPixels;
	ptTemp      = ptCurPos;
	ptCurPos.y += PrintString(pDC, ptTemp, _T("\nPPQ"));
	pDC->SelectObject(&m_fontText);
	
	//The nullptr check here is enough because then lSize would be 0
	if( nullptr != pConfig) { lPPQ = pConfig->GetPPQCount(); }
	++nIndentLevel;
	for (long l = 0; l < lPPQ; l++)
	{
		SVPPQObject* pPPQ = pConfig->GetPPQ(l);
		if( nullptr != pPPQ )
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
			
			SvTi::SVTriggerObject* pTrigger( nullptr );
			pPPQ->GetTrigger(pTrigger);
			sValue.Empty();
			if(nullptr != pTrigger ) { sValue = pTrigger->GetName();}
			ptCurPos.x  = (nIndentLevel + 2) * m_shortTabPixels;
			ptTemp      = ptCurPos;
			ptCurPos.y += PrintString(pDC, ptTemp, sValue);
			
			ptCurPos.x  = (nIndentLevel + 1) * m_shortTabPixels;
			ptTemp      = ptCurPos;
			ptCurPos.y += PrintString(pDC, ptTemp, _T("Camera:"));
			
			std::deque< SVVirtualCamera* > l_Cameras;

			pPPQ->GetCameraList( l_Cameras );

			std::deque< SVVirtualCamera* >::iterator l_Iter = l_Cameras.begin();

			while( l_Iter != l_Cameras.end() )
			{
				SVVirtualCamera* pCamera = ( *l_Iter );
				
				if ( nullptr != pCamera )
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
				
				if ( nullptr != pInspection )
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
	SVConfigurationObject* pConfig( nullptr );
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

	long lPPQ = 0;
	//The nullptr check here is enough because then lPPQ would be 0
	if( nullptr != pConfig) { lPPQ = pConfig->GetPPQCount(); }
	for (int intPPQ = 0; intPPQ < lPPQ; intPPQ++)
	{
		SVPPQObject* pPPQ = pConfig->GetPPQ( intPPQ );

		if ( nullptr == pPPQ )
		{
			SvStl::MessageMgrStd e( SvStl::LogOnly );
			e.setMessage( SVMSG_SVO_55_DEBUG_BREAK_ERROR, SvOi::Tid_ErrorGettingPPQ, SvStl::SourceFileParams(StdMessageParams), SvOi::Err_17000_ErrorGettingPPQ );
			DebugBreak();
			continue;
		}

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
				
				if ( nullptr != pCamera )
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
                    if ( nullptr != l_pObject )
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
	SVConfigurationObject* pConfig( nullptr );
	SVObjectManagerClass::Instance().GetConfigurationObject( pConfig );
	SVObserverApp*         pApp    = dynamic_cast <SVObserverApp*> (AfxGetApp());

	CString	sLabel, sValue;
	int      nFirstHeight   = 0;
	int      nLastHeight    = 0;
	long     lSize = 0;
	CPoint   ptTemp(0, 0);
	
	SVInspectionProcess*    pInspection( nullptr );
	SVToolSetClass*         pToolset( nullptr );
	
	// Print all IPDocs
	//The nullptr check here is enough because then lSize would be 0
	if( nullptr != pConfig) { lSize = pConfig->GetInspectionCount(); }
	for (int nIPDNumber = 0; nIPDNumber < lSize; ++nIPDNumber)
	{
		pInspection = pConfig->GetInspection(nIPDNumber);
		if( nullptr != pInspection )
		{
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
			if ( nullptr != pToolSet )
				sValue.Format("%d", pToolSet->GetSize());
			else
				sValue = pApp->GetStringResource(IDS_NOT_AVAILABLE_STRING);
		
			ptCurPos.x   = nIndentLevel * m_shortTabPixels;
			PrintValueObject(pDC, ptCurPos, _T("Number of Tools:"), sValue);
		
			// Print all tools
			if( nullptr != pToolSet)
			{
				PrintObject(pDC, pToolSet, ptCurPos, nIndentLevel);
			}  // end if( pToolSet )
		}
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
	PrintGlobalConstants( pDC, ptCurPos, nIndentLevel );
}

void SVConfigurationPrint::PrintModuleIO(CDC* pDC, CPoint& ptCurPos, int nIndentLevel)
{
	CString label, value;
	int		nFirstHeight = 0;
	int		nLastHeight = 0;
	long	lSize = 0;
	CPoint	ptTemp(0, 0);

	SVConfigurationObject* pConfig( nullptr );
	SVObjectManagerClass::Instance().GetConfigurationObject( pConfig );
	SVObserverApp* pApp = dynamic_cast<SVObserverApp*>(AfxGetApp());

	// Print IODoc contents
	if (pApp->GetIODoc())
	{
		SVInputObjectList* pInputList = nullptr;
		SVDigitalInputObject* pDigInput;
		SVIOEntryHostStructPtrList ppIOEntries;

		// Get list of available inputs
		if ( nullptr != pConfig ) { pInputList = pConfig->GetInputObjectList(); }
		if ( nullptr == pInputList )
		{
			SvStl::MessageMgrStd e( SvStl::LogOnly );
			e.setMessage( SVMSG_SVO_55_DEBUG_BREAK_ERROR, SvOi::Tid_ErrorGettingInputObjectList, SvStl::SourceFileParams(StdMessageParams), SvOi::Err_17001_ErrorGettingInputObjectList );
			DebugBreak();
		}
		else if (!pInputList->FillInputs( ppIOEntries ))
		{
			SvStl::MessageMgrStd e( SvStl::LogOnly );
			e.setMessage( SVMSG_SVO_55_DEBUG_BREAK_ERROR, SvOi::Tid_ErrorFillingInputs, SvStl::SourceFileParams(StdMessageParams), SvOi::Err_17002_ErrorFillingInputs );
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
			strValue = SvUl_SF::Format( "%d", it->m_lAffinity);
		}
		else
		{
			strValue = "Not Set";
		}
		strName = SvUl_SF::Format("Name: %s:", it->m_strName.c_str() );
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

	SVConfigurationObject* pConfig( nullptr );
	SVObjectManagerClass::Instance().GetConfigurationObject( pConfig );
	SVObserverApp* pApp = dynamic_cast<SVObserverApp*>(AfxGetApp());

	// Get the number of PPQs
	long lPPQSize = 0;
	if ( nullptr != pConfig )
	{
		lPPQSize = pConfig->GetPPQCount();
	}
	else
	{
		SvStl::MessageMgrStd e( SvStl::LogOnly );
		e.setMessage( SVMSG_SVO_55_DEBUG_BREAK_ERROR, SvOi::Tid_ErrorGettingPPQCount, SvStl::SourceFileParams(StdMessageParams), SvOi::Err_17003_ErrorGettingPPQCount );
		DebugBreak();
	}

	// Print IODoc contents
	if (pApp->GetIODoc())
	{
		SVPPQObject* pPPQ( nullptr );
		SVDigitalOutputObject* pDigOutput( nullptr );
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
				pPPQ = pConfig->GetPPQ( j );
				if ( nullptr == pPPQ )
				{
					SvStl::MessageMgrStd e( SvStl::LogOnly );
					e.setMessage( SVMSG_SVO_55_DEBUG_BREAK_ERROR, SvOi::Tid_ErrorGettingPPQ, SvStl::SourceFileParams(StdMessageParams), SvOi::Err_17004_ErrorGettingPPQ );
					DebugBreak();
				}

				// Get list of available outputs
				long lIOEntries = 0;
				if ( !pPPQ->GetAllOutputs( ppIOEntries ) )
				{
					SvStl::MessageMgrStd e( SvStl::LogOnly );
					e.setMessage( SVMSG_SVO_55_DEBUG_BREAK_ERROR, SvOi::Tid_ErrorGettingOutputs, SvStl::SourceFileParams(StdMessageParams), SvOi::Err_17005_ErrorGettingOutputs );
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
	SVConfigurationObject* pConfig( nullptr );
	SVObjectManagerClass::Instance().GetConfigurationObject( pConfig );

	if( nullptr != pConfig )
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

void SVConfigurationPrint::PrintGlobalConstants( CDC* pDC, CPoint& ptCurPos, int nIndentLevel )
{
	CString	Label;
	CString Value;
	int Index (0);

	BasicValueObjects::ValueVector GlobalConstantObjects;
	RootObject::getRootChildObjectList( GlobalConstantObjects, SvOl::FqnGlobal );

	Label = _T( "Global Constants" );
	Value.Format( _T("%d"),  GlobalConstantObjects.size() );
	PrintValueObject(pDC, ptCurPos, Label, Value );
	BasicValueObjects::ValueVector::const_iterator Iter( GlobalConstantObjects.cbegin() );
	while ( GlobalConstantObjects.cend() != Iter )
	{
		const BasicValueObjectPtr& pGlobalConstant = *Iter;

		if( !pGlobalConstant.empty() )
		{
			//Print Global Constant name
			Label.Format( _T("Global Constant %d"), ++Index );
			Value = pGlobalConstant->GetCompleteObjectName();
			PrintValueObject(pDC, ptCurPos, Label, Value );

			//Print Global Constant value
			SVString Temp;
			Label =  _T("Value");
			pGlobalConstant->getValue( Temp );
			Value = Temp.c_str();
			PrintValueObject(pDC, ptCurPos, Label, Value );

			//Print Global Constant type
			Label = _T("Type");
			Temp = pGlobalConstant->getTypeName();
			Value = Temp.c_str();
			PrintValueObject(pDC, ptCurPos, Label, Value );
		}

		++Iter;
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
				s = iterOption->strDescription.c_str();
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
				s = iterOption->strDescription.c_str();
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
		m_pPrint->PrintValueObject(m_pDC, m_rptCurPos, pCamFileParam->Name(), param.strValue.c_str() );
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
		SVCameraFormat* pFormat = nullptr;
		if ( param.options.size() > 0 )
		{
			SVCameraFormat& rFormat = param.options[ param.strValue ];
			const SVCameraFormat& rCamFileFormat = pCamFileParam->options.find( param.strValue )->second;
			if ( rCamFileFormat.m_strName == param.strValue )
			{
				s = rCamFileFormat.m_strDescription.c_str();
				pFormat = &rFormat;
			}
		}
		if ( s.IsEmpty() )
			s = param.strValue.c_str();
		m_pPrint->PrintValueObject(m_pDC, m_rptCurPos, pCamFileParam->Name(), s );

		if ( pFormat )
		{
			CPoint pt = m_rptCurPos;
			pt.x += m_pPrint->m_shortTabPixels;

			m_pPrint->PrintValueObject(m_pDC, pt, _T("Left"), AsString(pFormat->m_lHPos));
			m_pPrint->PrintValueObject(m_pDC, pt, _T("Top"), AsString(pFormat->m_lVPos));
			m_pPrint->PrintValueObject(m_pDC, pt, _T("Width"), AsString(pFormat->m_lWidth));
			m_pPrint->PrintValueObject(m_pDC, pt, _T("Height"), AsString(pFormat->m_lHeight));
			
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

