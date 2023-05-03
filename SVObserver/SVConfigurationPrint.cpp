//******************************************************************************
//* COPYRIGHT (c) 2003 by Körber Pharma Inspection GmbH. All Rights Reserved
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

#include "SVConfigurationPrint.h"
#include "SVConfigurationObject.h"

#include "SVIOLibrary\SVIOConfigurationInterfaceClass.h"
#include "CameraLibrary\SVDeviceParams.h"
#include "CameraLibrary\SVBoolValueDeviceParam.h"
#include "CameraLibrary\SVi64ValueDeviceParam.h"
#include "CameraLibrary\SVLongValueDeviceParam.h"
#include "CameraLibrary\SVStringValueDeviceParam.h"
#include "CameraLibrary\SVCustomDeviceParam.h"
#include "SVFileAcquisitionDevice/SVFileAcquisitionLoadingModeEnum.h"
#include "SVImageLibrary\SVImagingDeviceParams.h"
#include "SVObjectLibrary\SVObjectClass.h"
#include "SVValueObjectLibrary/SVDoubleValueObjectClass.h"
#include "SVValueObjectLibrary/SVDWordValueObjectClass.h"
#include "Operators/SVEquation.h"
#include "InspectionEngine/SVAcquisitionClass.h"
#include "InspectionEngine/SVImageClass.h"
#include "ConfigurationOuttakes.h"
#include "SVObserver.h"
#include "SVIODoc.h"
#include "SVToolSet.h"
#include "SVIOLibrary/SVInputObjectList.h"
#include "SVIOLibrary/SVDigitalInputObject.h"
#include "SVIOLibrary/SVDigitalOutputObject.h"
#include "Tools/SVArchiveTool.h"
#include "Tools/SVArchiveRecord.h"
#include "Tools/SVArchiveRecordsArray.h"
#include "Tools/SVTool.h"
#include "SVObjectLibrary\SVObjectManagerClass.h"
#include "FilesystemUtilities/FileHelperContainer.h"
#include "SVImageLibrary\SVLightReference.h"
#include "SVIPDoc.h"
#include "Operators/SVShapeMaskHelperClass.h"
#include "SVIOController.h"
#include "AnalyzerOperators/BlobAnalyzer.h"
#include "Operators/SVResultDouble.h"
#include "RemoteMonitorList.h"
#include "RemoteMonitorListHelper.h"
#include "SVStatusLibrary\MessageManager.h"
#include "TextDefinesSvO.h"
#include "RootObject.h"
#include "Triggering/SVTriggerClass.h"
#include "Triggering/SVTriggerObject.h"
#pragma endregion Includes

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

constexpr const char* ToolGroupCommentLabel = _T("Tool Comment:");

constexpr int LEFT_MARGIN = 50;

static std::vector<SvPb::EmbeddedIdEnum> g_NonPrintEmbeddedArray =
{
	SvPb::TaskObjectClassIsObjectValidEId,
	SvPb::StatusEId,
	SvPb::ColorEId,
	SvPb::ConditionalResultEId,
	SvPb::FailedEId,
	SvPb::WarnedEId,
	SvPb::PassedEId,
	SvPb::ExplicitFailedEId,
	SvPb::PassedCountEId,
	SvPb::FailedCountEId,
	SvPb::WarnedCountEId,
	SvPb::EdgeCountEId,
	SvPb::EnabledCountEId,
	SvPb::ProcessedCountEId,
	SvPb::BlobFeatureEId+SvOi::SV_AREA,
	SvPb::BlobFeatureEId+SvOi::SV_BOXX_MAX,
	SvPb::BlobFeatureEId+SvOi::SV_BOXX_MIN,
	SvPb::BlobFeatureEId+SvOi::SV_BOXY_MAX,
	SvPb::BlobFeatureEId+SvOi::SV_BOXY_MIN,
	SvPb::BlobFeatureEId+SvOi::SV_BREADTH,
	SvPb::BlobFeatureEId+SvOi::SV_CENTEROFGRAVITY_X,
	SvPb::BlobFeatureEId+SvOi::SV_CENTEROFGRAVITY_Y,
	SvPb::BlobFeatureEId+SvOi::SV_CONVEX_PERIMETER,
	SvPb::BlobFeatureEId+SvOi::SV_FERET_ELONGATION,
	SvPb::BlobFeatureEId+SvOi::SV_FERETMAX_ANGLE,
	SvPb::BlobFeatureEId+SvOi::SV_FERETMAX_DIAMETER,
	SvPb::BlobFeatureEId+SvOi::SV_FERETMEAN_DIAMETER,
	SvPb::BlobFeatureEId+SvOi::SV_FERETMIN_ANGLE,
	SvPb::BlobFeatureEId+SvOi::SV_FERETMIN_DIAMETER,
	SvPb::BlobFeatureEId+SvOi::SV_FERET_X,
	SvPb::BlobFeatureEId+SvOi::SV_FERET_Y,
	SvPb::BlobFeatureEId+SvOi::SV_FIRSTPOINT_X,
	SvPb::BlobFeatureEId+SvOi::SV_FIRSTPOINT_Y,
	SvPb::BlobFeatureEId+SvOi::SV_LABEL,
	SvPb::BlobFeatureEId+SvOi::SV_LENGTH,
	SvPb::BlobFeatureEId+SvOi::SV_NBROF_HOLES,
	SvPb::BlobFeatureEId+SvOi::SV_PERIMETER,
	SvPb::BlobFeatureEId+SvOi::SV_ROUGHNESS,
	SvPb::BlobFeatureEId+SvOi::SV_SUM_PIXEL,
	SvPb::BlobFeatureEId+SvOi::SV_COMPACTNESS,
	SvPb::BlobFeatureEId+SvOi::SV_NBR_RUNS,
	SvPb::BlobFeatureEId+SvOi::SV_XMINAT_YMIN,
	SvPb::BlobFeatureEId+SvOi::SV_XMAXAT_YMAX,
	SvPb::BlobFeatureEId+SvOi::SV_YMINAT_XMAX,
	SvPb::BlobFeatureEId+SvOi::SV_YMAXAT_XMIN,
	SvPb::BlobFeatureEId+SvOi::SV_ELONGATION,
	SvPb::BlobFeatureEId+SvOi::SV_INTERCEPT_0,
	SvPb::BlobFeatureEId+SvOi::SV_INTERCEPT_45,
	SvPb::BlobFeatureEId+SvOi::SV_INTERCEPT_90,
	SvPb::BlobFeatureEId+SvOi::SV_INTERCEPT_135,
	SvPb::BlobFeatureEId+SvOi::SV_MOMENT_X0Y1,
	SvPb::BlobFeatureEId+SvOi::SV_MOMENT_X1Y0,
	SvPb::BlobFeatureEId+SvOi::SV_MOMENT_X1Y1,
	SvPb::BlobFeatureEId+SvOi::SV_MOMENT_X0Y2,
	SvPb::BlobFeatureEId+SvOi::SV_MOMENT_X2Y0,
	SvPb::BlobFeatureEId+SvOi::SV_CENTRAL_X0Y2,
	SvPb::BlobFeatureEId+SvOi::SV_CENTRAL_X2Y0,
	SvPb::BlobFeatureEId+SvOi::SV_CENTRAL_X1Y1,
	SvPb::BlobFeatureEId+SvOi::SV_AXISPRINCIPAL_ANGLE,
	SvPb::BlobFeatureEId+SvOi::SV_AXISSECONDARY_ANGLE,
	SvPb::BlobFeatureEId+SvOi::SV_EULER_NBR,
	SvPb::BlobFeatureEId+SvOi::SV_CENTER_X_SOURCE,
	SvPb::BlobFeatureEId+SvOi::SV_CENTER_Y_SOURCE
};

const std::vector<SvPb::EmbeddedIdEnum>& NonPrintEmbeddeds()
{
	return g_NonPrintEmbeddedArray;
}

static std::vector<SvPb::ClassIdEnum> g_classIdNonPrintArray = { SvPb::OCVCharacterResultClassId };

const std::vector<SvPb::ClassIdEnum>& NonPrintClassIds()
{
	return g_classIdNonPrintArray;
}

class SVDeviceParamConfigPrintHelper
{
public:
	SVDeviceParamConfigPrintHelper(SVConfigurationPrint* pPrint, CDC* pDC, CPoint& ptCurPos, int nIndentLevel);
	HRESULT Visit(SVDeviceParam*);
	HRESULT Visit(SVLongValueDeviceParam*);
	HRESULT Visit(SVi64ValueDeviceParam*);
	HRESULT Visit(SVBoolValueDeviceParam*);
	HRESULT Visit(SVStringValueDeviceParam*);
	HRESULT Visit(SVCameraFormatsDeviceParam*);
	HRESULT Visit(SVCustomDeviceParam*);

private:
	SVConfigurationPrint* m_pPrint;
	CDC* m_pDC;
	CPoint& m_rptCurPos;
	int m_nIndentLevel;
};

static SVToolGrouping GetToolGroupings(uint32_t inspectionId)
{
	// get the document that owns this inspection
	SVIPDoc* pDoc = GetIPDocByInspectionID(inspectionId);
	if (pDoc)
	{
		return pDoc->GetToolGroupings();
	}
	return SVToolGrouping();
}

static SVObjectClass* GetTool(const std::string& rName, const SvIe::SVTaskObjectListClass& rToolSet)
{
	SVObjectClass* pObject(nullptr);
	for (int i = 0; !pObject && i < rToolSet.GetSize(); i++)
	{
		std::string ToolName( rToolSet.getTaskObject(i)->GetName() );
		if ( ToolName == rName) 
		{
			pObject = rToolSet.getTaskObject(i);
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
	m_toolNumber             = 0;
	m_heightPageNumberPixels = 0;
	m_shortTabPixels         = 50;
	m_pageCenter             = 0;
	
	m_isRealPrint = FALSE;
	
	m_isPrintToStringBuffer = false;
	m_pBuffer = nullptr;
	
	// Set SvFs::FileHelper for Print to File
	
	m_svfnFileName.SetDefaultFileExtension(std::string(CString(MAKEINTRESOURCE(AFX_IDS_PRINTDEFAULTEXT))));
	
	m_svfnFileName.SetDefaultFileName(std::string(CString(MAKEINTRESOURCE(AFX_IDS_PRINTDEFAULT))));
	
	m_svfnFileName.SetFileSaveFlags(OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT);
	
	m_svfnFileName.SetFileExtensionFilterList(std::string(CString(MAKEINTRESOURCE(AFX_IDS_PRINTFILTER))));
	
	m_svfnFileName.SetFileSaveDialogTitle(CString(MAKEINTRESOURCE(AFX_IDS_PRINTCAPTION)).GetString());
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
void SVConfigurationPrint::DoPrintConfig()
{
    SVObserverApp* pApp = dynamic_cast <SVObserverApp*> (AfxGetApp());
	
	Log_Assert(nullptr != m_printInfo.m_pPD);    // must be set
	
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
					SvStl::MessageManager Msg(SvStl::MsgType::Log | SvStl::MsgType::Display );
					Msg.setMessage( SVMSG_SVO_93_GENERAL_WARNING, message, SvStl::SourceFileParams(StdMessageParams));
					return;
				}  // end if( nullptr == printInfo.m_pPD->m_pd.hDC  )
			}  // end if( pCmdInfo->m_nShellCommand == CCommandLineInfo::FilePrintTo )
		}  // end if( nullptr != pCmdInfo )
		
		m_printInfo.m_bDirect = TRUE;
	}  // end if(   pThreadState && ...
	
	if (DoPreparePrinting(&m_printInfo))
	{
		// hDC must be set (did you remember to call DoPreparePrinting?)
		Log_Assert(nullptr != m_printInfo.m_pPD->m_pd.hDC);
		
		// gather file to print to if print-to-file selected
		std::string strOutput;
		
		if (m_printInfo.m_pPD->m_pd.Flags & PD_PRINTTOFILE)
		{
			// begin SES 11-Jan-2001
			// Use SvFs::FileHelper for browsing
			if (!m_svfnFileName.SaveFile())
			{
				return;
			}
			
			// set output device to resulting path name
			strOutput = m_svfnFileName.GetFileName();
			// end SES 11-Jan-2001
		}  // end if( printInfo.m_pPD->m_pd.Flags & PD_PRINTTOFILE )
		
		// Special...
		// Set up doc info an start the config printing process...
		std::string Title = getConfigFileName();
		
		if( 31 < Title.size() )
		{
			Title = SvUl::Left( Title, 31 );
		}
		
		DOCINFO docInfo;
		
		// Init docInfo to 0...
		memset(&docInfo, 0, sizeof(DOCINFO));
		
		// Init docInfo members...
		docInfo.cbSize = sizeof(DOCINFO);
		docInfo.lpszDocName = Title.c_str();

		CString PortName;
		int nFormatID;
		
		if (strOutput.empty())
		{
			docInfo.lpszOutput = nullptr;
			
			PortName = m_printInfo.m_pPD->GetPortName();
			nFormatID = AFX_IDS_PRINTONPORT;
		}  // end if( strOutput.IsEmpty() )
		else
		{
			docInfo.lpszOutput = strOutput.c_str();
			
			AfxGetFileTitle(strOutput.c_str(), PortName.GetBuffer(_MAX_PATH), _MAX_PATH);
			
			nFormatID = AFX_IDS_PRINTTOFILE;
		}  // end else [if( strOutput.IsEmpty() )]
		
		m_printDC.Attach(m_printInfo.m_pPD->m_pd.hDC);  // attach printer dc
		
		m_printDC.m_bPrinting = TRUE;
		
		// Call on begin printing...
		OnBeginPrinting();
		
		// disable main window while printing & init printing status dialog
		AfxGetMainWnd()->EnableWindow(FALSE);
		
		CPrintDialog   dlgPrintStatus(true, PD_ALLPAGES | PD_USEDEVMODECOPIES | PD_NOPAGENUMS | PD_HIDEPRINTTOFILE | PD_NOSELECTION, AfxGetMainWnd());
		
		CString           strTemp;
		
		dlgPrintStatus.SetDlgItemText(AFX_IDC_PRINT_DOCNAME, Title.c_str());
		dlgPrintStatus.SetDlgItemText(AFX_IDC_PRINT_PRINTERNAME, m_printInfo.m_pPD->GetDeviceName());
		AfxFormatString1(strTemp, nFormatID, PortName);
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
			SvStl::MessageManager Msg(SvStl::MsgType::Log | SvStl::MsgType::Display );
			Msg.setMessage( SVMSG_SVO_93_GENERAL_WARNING, message, SvStl::SourceFileParams(StdMessageParams));
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
			
			Log_Assert(m_printInfo.m_bContinuePrinting);
			
			// page successfully started, so now render the page
			PrintPage();
			
			if (m_printDC.EndPage() < 0)
			{
				bError = TRUE;
				break;
			}
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

void SVConfigurationPrint::printConfigToStringBuffer(std::string& rBuffer)
{
	m_isPrintToStringBuffer = true;
	m_pBuffer = &rBuffer;
	
	// create a dummy DC
	CWnd wnd;
	CRect rect(0, 0, 10, 10);
	BOOL bResult = wnd.CreateEx(0, _T("Button"), _T(""), 0, rect, nullptr, 0);
	Log_Assert(bResult); UNREFERENCED_PARAMETER(bResult);
	HDC hDC = wnd.GetDC()->GetSafeHdc();
	m_printDC.Attach(hDC);
	
	// print
	OnBeginPrinting();
    
	m_printDC.Detach();
}

BOOL SVConfigurationPrint::DoPreparePrinting(CPrintInfo* pPrintInfo)
{
    SVObserverApp* pApp = dynamic_cast <SVObserverApp*> (AfxGetApp());
	Log_Assert(nullptr != pPrintInfo);
	Log_Assert(nullptr != pPrintInfo->m_pPD);
	
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
	
	Log_Assert(nullptr != pPrintInfo->m_pPD);
	
	Log_Assert(nullptr != pPrintInfo->m_pPD->m_pd.hDC);
	
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
	auto* pTask = dynamic_cast<SvIe::SVTaskObjectClass*>(pObj);
	if (nullptr != pTask)
	{
		PrintFriends(pDC, pTask, ptCurPos, nIndentLevel + 1);
	}
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
void SVConfigurationPrint::PrintDetails( CDC* pDC, SVObjectClass* pObject, CPoint& ptCurPos, int nIndentLevel )
{
	SvPb::ClassIdEnum classID = pObject->GetClassID();
	const auto& nonPCIds = NonPrintClassIds();
	auto result = std::find_if(std::begin(nonPCIds), std::end(nonPCIds), [&classID](const auto& rEntry) { return classID == rEntry; });
	if (result != std::end(nonPCIds))
	{
		return;
	}

	// If object is a value object, get embedded ID which is NonPrintable.
	if( nullptr != dynamic_cast<SvOi::IValueObject*> (pObject) )
	{
		SvPb::EmbeddedIdEnum embeddedID = pObject->GetEmbeddedID();
		const auto& nPrs = NonPrintEmbeddeds();
		// Check for non-printing object type.
		auto iter = std::find_if(std::begin(nPrs), std::end(nPrs), [&embeddedID](const auto& rEntry) { return embeddedID == rEntry; });
		if (iter != std::end(nPrs))
		{
			return;
		} 
	}
	
	std::string sLabel, sValue;
	// If object is a value object, print name and value.
	SvOi::IValueObject* pValueObject = dynamic_cast<SvOi::IValueObject*> (pObject);
	if( nullptr != pValueObject )
	{
		sLabel = pObject->GetName();
		if ( pObject->ObjectAttributesAllowed() & SvPb::audittrail )
		{
			bool bGotValue = false;

			if (SvVol::SVDWordValueObjectClass* pdwValueObject = dynamic_cast <SvVol::SVDWordValueObjectClass*> (pObject))
			{
				DWORD dwValue=0;
				bGotValue = (S_OK == pdwValueObject->GetValue( dwValue ));
				sValue = SvUl::AsString( dwValue );
			}
			else if (SvVol::LinkedValue* pLinkedValue = dynamic_cast<SvVol::LinkedValue*>(pObject))
			{
				ptCurPos.x = (nIndentLevel + 1) * m_shortTabPixels;
				PrintValueObject(pDC, ptCurPos, sLabel.c_str(), "");
				ptCurPos.x = (nIndentLevel + 2) * m_shortTabPixels;
				PrintValueObject(pDC, ptCurPos, "Type", std::to_string(pLinkedValue->getSelectedOption()).c_str());
				PrintValueObject(pDC, ptCurPos, "Content", pLinkedValue->getContentStr().c_str());
			}
			else
			{
				bGotValue = (S_OK == pValueObject->getValue( sValue ));
			}

			if (bGotValue && !sValue.empty())
			{
				ptCurPos.x = (nIndentLevel + 1) * m_shortTabPixels;
				PrintValueObject(pDC, ptCurPos, sLabel.c_str(), sValue.c_str());
			}
		}   // end if ( pObj->uObjectAttributesAllowed & SvPb::printable )
	}  // end if( nullptr != pValueObject )
	else
	{
		std::string strType = pObject->GetObjectName();
		std::string strName = pObject->GetName();
		do
		{
			if ( nullptr != dynamic_cast <SvOp::SVShapeMaskHelperClass*> (pObject) )
			{
				if ( !( pObject->ObjectAttributesAllowed() & SvPb::audittrail) )	// EB 20050818 - hack this instead of doing it right
				{
					break;
				}
			}
			
			SvTo::SVToolClass* pTool = dynamic_cast<SvTo::SVToolClass*> (pObject);
			if ( nullptr != pTool )
			{
				// Increment even if disabled, to maintain count.  Starts with zero, so for first
				//    tool, will increment to 1.
				m_toolNumber++;
				
				sLabel = std::format( _T("Tool Number {:d}:"), m_toolNumber );
				
				ptCurPos.x  = nIndentLevel * m_shortTabPixels;
				CPoint ptTemp      = ptCurPos;
				ptCurPos.y += PrintString(pDC, ptTemp, sLabel.c_str());
				ptCurPos.x  = ++nIndentLevel * m_shortTabPixels;
			}
			
			ptCurPos.x   = nIndentLevel * m_shortTabPixels;
			PrintValueObject(pDC, ptCurPos, _T("Name:"), strName.c_str());
			
			// If the object type is different from the name, print the type.
			if (strName != strType && !strType.empty())
			{
				PrintValueObject(pDC, ptCurPos, _T("Type:"), strType.c_str());
			}
			
			SvTo::SVArchiveTool* pArchiveTool = dynamic_cast <SvTo::SVArchiveTool*> (pObject);
			if( nullptr != pArchiveTool )
			{
			
				CPoint ptTemp      = ptCurPos;
				ptCurPos.y += PrintString(pDC, ptTemp, _T("Results"));
				ptCurPos.x  = ++nIndentLevel * m_shortTabPixels;
				const auto& rRecVec = pArchiveTool->m_ResultCollection.getRecordVec();
				for (int i = 0; i < rRecVec.size(); ++i)
				{
					if (nullptr != rRecVec[i].GetObjectReference().getObject())
					{
						ptCurPos.x   = nIndentLevel * m_shortTabPixels;
						sLabel = std::format(_T("Result {:d}:"), i + 1);
						sValue = rRecVec[i].GetObjectReference().GetCompleteName();
						PrintValueObject(pDC, ptCurPos, sLabel.c_str(), sValue.c_str());
					}
				}
				
				ptCurPos.x  = --nIndentLevel * m_shortTabPixels;
				ptTemp      = ptCurPos;
				ptCurPos.y += PrintString(pDC, ptTemp, _T("Images"));
				
				ptCurPos.x  = ++nIndentLevel * m_shortTabPixels;
				const auto& rRecImageVec = pArchiveTool->m_ImageCollection.getRecordVec();
				for (int i = 0; i < rRecImageVec.size(); ++i)
				{
					ptCurPos.x   = nIndentLevel * m_shortTabPixels;
					sLabel = std::format( _T("Image {:d}:"), i + 1);
					PrintValueObject( pDC, ptCurPos, sLabel.c_str(), rRecImageVec[i].GetImageObjectName().c_str() );
				}
				
				nIndentLevel -= 2;
				ptCurPos.x   = nIndentLevel * m_shortTabPixels;
			}// end if ( SVArchiveTool* pArchiveTool = dynamic_cast <SVArchiveTool*> (pObj) )
			
			SvOp::SVDoubleResult* pBlobResult = dynamic_cast<SvOp::SVDoubleResult*> (pObject);
			if( nullptr != pBlobResult )
			{
				if (SV_IS_KIND_OF(pBlobResult->GetParent(), SvAo::BlobAnalyzer))
				{  
					sLabel = SvUl::LoadStdString(IDS_BLOB_FEATURE_DEFAULT_VALUE) + _T(":");
					const SvVol::SVDoubleValueObjectClass* pDoubleValueObj = dynamic_cast<const SvVol::SVDoubleValueObjectClass*> (pBlobResult->getInput());
					if ( pDoubleValueObj )
					{
						double dVal = pDoubleValueObj->GetDefaultValue();
						sValue = std::format( _T("{:f}"), dVal );
						ptCurPos.x   = (nIndentLevel + 1) * m_shortTabPixels;
						PrintValueObject(pDC, ptCurPos, sLabel.c_str(), sValue.c_str());
					}
				}
			}
		} while (false);// end do
	}// End if( nullptr != pValueObject ) else

	SvOp::SVEquation* pEquation = dynamic_cast <SvOp::SVEquation*> (pObject);
	if( nullptr != pEquation )
	{
		sValue = pEquation->GetEquationText();
		
		// Print the equation text.
		if (sLabel.empty())
		{
			sLabel = _T("Equation Text");
		}
		ptCurPos.x  = nIndentLevel * m_shortTabPixels;
		PrintValueObject(pDC, ptCurPos, sLabel.c_str(), sValue.c_str());
	}
	
	if( SV_IS_KIND_OF(pObject, SvIe::SVTaskObjectClass) )
	{
		PrintInputOutputList(pDC, pObject, ptCurPos, nIndentLevel);
	}
}  // end function void SVConfigurationPrint:::PrintDetails( ... )

void SVConfigurationPrint::PrintAllChildren(CDC* pDC, SvIe::SVTaskObjectListClass* pTaskObj,  CPoint& ptCurPos, int nIndentLevel)
{
	for (int nCnt = 0; nCnt < pTaskObj->GetSize(); nCnt++)
	{
		SVObjectClass* pChild = pTaskObj->getTaskObject(nCnt);
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
	if (SvIe::SVTaskObjectListClass* pTaskObj = dynamic_cast<SvIe::SVTaskObjectListClass*> (pObj) )
    {
		if (SVToolSet* pToolSet = dynamic_cast<SVToolSet*>(pObj))
		{
			SVToolGrouping toolGroupings = GetToolGroupings(pToolSet->GetInspection()->getObjectId());
			if (toolGroupings.size())
			{
				for (SVToolGrouping::iterator it = toolGroupings.begin();it != toolGroupings.end();++it)
				{
					switch (it->second.m_type)
					{
						case ToolGroupData::StartOfGroup:
						{
							std::string sLabel;
							sLabel = std::format( _T("Tool Grouping: {}"), it->first.c_str() );
							ptCurPos.x  = nIndentLevel * m_shortTabPixels;
							CPoint ptTemp = ptCurPos;
							ptCurPos.y += PrintString(pDC, ptTemp, sLabel.c_str());

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
							std::string sLabel;
							sLabel = std::format( _T("End Tool Grouping: {}"), it->first.c_str());
							ptCurPos.x  = nIndentLevel * m_shortTabPixels;
							CPoint ptTemp = ptCurPos;
							ptCurPos.y += PrintString(pDC, ptTemp, sLabel.c_str());

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
void SVConfigurationPrint::PrintFriends( CDC* pDC, SvIe::SVTaskObjectClass* pObj, CPoint& ptCurPos, int nIndentLevel )
{
	auto rFriendList = pObj->GetFriendList().getContainerCopy();
	
	for (auto* pFriend : rFriendList)
	{
		if (nullptr != pFriend)
		{
			PrintObject(pDC, pFriend, ptCurPos, nIndentLevel);
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
	SvIe::SVTaskObjectClass* pTaskObj = dynamic_cast<SvIe::SVTaskObjectClass*> (pObj);
	const auto& rInputList = pTaskObj->GetPrivateInputList();
	if (0 < rInputList.size())
	{
		for (auto* pInput : rInputList)
		{
			if (nullptr != pInput && pInput->IsConnected() && SvPb::noAttributes != pInput->ObjectAttributesAllowed())
			{
				ptCurPos.x = nIndentLevel * m_shortTabPixels;
				PrintValueObject(pDC, ptCurPos, pInput->GetName(), pInput->GetInputObjectInfo().getObject()->GetObjectNameToObjectType().c_str());
			}
		}
	}

	std::vector<SvOi::IObjectClass*> outputList;
	pTaskObj->getOutputList(std::back_inserter(outputList));
	for (auto* pIObject : outputList)
	{
		auto* pObject = dynamic_cast<SVObjectClass*> (pIObject);
		if (nullptr != pObject && pObject->GetParent() == pObj)
		{
			PrintDetails(pDC, pObject, ptCurPos, nIndentLevel);
		}  // end if( pOutput->PObject->GetOwner () == pObj )
	}  // end for( int nCnt = 0; nCnt < pOutputInfoList->GetSize(); nCnt++ )
}  // end function void SVConfigurationPrint:::PrintInputOutputList( ... )


static std::string ExpandTabs(LPCTSTR text)
{
	//Tabstop every 8 characters - replace tab with eight (8) spaces
	std::string tmp = text;
	SvUl::searchAndReplace( tmp, _T("\t"), _T("        "));
	return tmp;
}

typedef std::deque<std::string> WordWrapLines;

static WordWrapLines DoWordWrap(LPCTSTR Text, int numChars)
{
	WordWrapLines lines;
	
	// remove all carriage returns
	std::string Temp( Text );
	SvUl::searchAndReplace( Temp, _T("\r"), _T("") );

	// split into multiple lines based on linefeed
	typedef std::deque<std::string> split_container_type;
	split_container_type splitContainer;
	boost::algorithm::split(splitContainer, Temp, boost::algorithm::is_any_of("\n"), boost::algorithm::token_compress_off);
	// Iterate over the container of lines and do Word Wrap
	std::for_each(splitContainer.begin(), splitContainer.end(), [&numChars, &lines](const std::string& rText)->void 
	{
		std::string Line;

		int cnt = 0;
		size_t len = rText.size();
		for( size_t i = 0 ;i < len; i++ )
		{
			cnt++;
			if( rText[i] == _T('\n') )
			{
				lines.push_back( std::string(_T(" ")) );
				cnt = 0;
				Line.clear();
			}
			else if (cnt > numChars)
			{
				lines.push_back(Line);
				cnt = 1;
				Line = rText[i];
			}
			else
			{
				Line += rText[i];
			}
		}
		if (!Line.empty())
		{
			lines.push_back(Line);
		}
	}
	); // end of for_each
	return lines;
}

static void OutputTextString(CDC* pDC, LPCTSTR text, const CRect& rRect)
{
	// Get Height and Width of a single character ('W')
	CRect charRect(0, 0, 100, 100);
	CSize size = pDC->GetTextExtent( _T("W"), 1 );
	WordWrapLines lines = DoWordWrap(text, rRect.Width() / size.cx);
	
	CRect textRect(rRect);
	for (WordWrapLines::const_iterator it = lines.begin();it != lines.end();++it)
	{
		pDC->TextOut( textRect.left, textRect.top, it->c_str() );
		textRect.top += size.cy;
	}
}

int SVConfigurationPrint::PrintString(CDC* pDC, CPoint& ptCurPos, LPCTSTR Output)
{
	if (m_isPrintToStringBuffer)
	{
		*m_pBuffer += std::string(Output) + _T("\n");
    }
    else if (pDC)
	{
        // fix problem when handling empty string
        std::string OutputText = Output;
        if (OutputText.empty())
		{
            OutputText = _T(" ");
		}

		OutputText = ExpandTabs( OutputText.c_str() );

		CRect crectPrintRect(m_printInfo.m_rectDraw);
		
        crectPrintRect.top  = ptCurPos.y;
		crectPrintRect.left = ptCurPos.x + LEFT_MARGIN;
		
        // Get dimensions
		int nHeight = pDC->DrawText(OutputText.c_str(), &crectPrintRect, DT_LEFT | DT_TOP | DT_WORDBREAK | DT_NOFULLWIDTHCHARBREAK | DT_NOCLIP | DT_CALCRECT);
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
				OutputTextString(pDC, OutputText.c_str(), crectRealRect);
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
		OutputTextString(pDC, OutputText.c_str(), crectPrintRect);
        return nHeight; 
	}
	return 0;
}

void SVConfigurationPrint::PrintValueObject(CDC* pDC, CPoint& ptCurPos, LPCTSTR Name, LPCTSTR Value)
{
	if (m_isPrintToStringBuffer)
	{
		*m_pBuffer += std::string(Name) + _T("\t") + std::string(Value) + _T("\n");
    }
    else
    {
		CPoint	ptTemp = ptCurPos;
		int nFirstHeight = PrintString(pDC, ptTemp, Name);
		ptTemp.x     = (ptTemp.x <(m_pageCenter - 10)) ? m_pageCenter :(ptTemp.x + m_TabPixels);
		int nLastHeight  = PrintString(pDC, ptTemp, Value);
		ptCurPos.y  += std::max(nFirstHeight, nLastHeight); 
    }
}

void SVConfigurationPrint::PrintIOEntryObject(CDC* pDC, CPoint& ptCurPos, int nIndentLevel, LPCTSTR lpszName, SVIOEntryHostStructPtr IOEntry)
{
	SVDigitalInputObject	*pDigInput = nullptr;
	SVDigitalOutputObject	*pDigOutput = nullptr;
	std::string				sValue;
	
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

			sValue = pDigOutput->isCombined() ? _T("1") : _T(" ");
			PrintValueObject(pDC, ptCurPos, _T("Combined"), sValue.c_str());
			
			sValue = pDigOutput->isAndACK() ? _T("AND w/ ACK") : _T("OR w/ NAK");
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
	ptCurPos.x = nIndentLevel * m_shortTabPixels;
	
	PrintValueObject(pDC, ptCurPos, lpszName, lpszValue);
}

void SVConfigurationPrint::OnVirtualPrint(BOOL bRealPrintInput /* = FALSE */) 
{
    m_isRealPrint = bRealPrintInput;
	
    CDC* pDC = &m_printDC;
	
	// Will be called by OnBeginPrinting
	CPoint  ptCurPos(0, 0);
	CPoint	ptTemp(0, 0);
	
    int      nIndentLevel = 0;
	
	// Exchange font and store old one
	pDC->SelectObject(&m_fontTitle);
	
    // Print config title
	std::string Label = std::format(_T("Configuration {}"), getConfigFileName() );
	
	ptCurPos.x  = nIndentLevel * m_shortTabPixels;
	ptTemp      = ptCurPos;
	ptCurPos.y += PrintString(pDC, ptTemp, Label.c_str() );
	
	pDC->SelectObject(&m_fontText);
	
	// Print config path
	PrintValueObject(pDC, ptCurPos, _T("Current Path:"), getConfigFullFileName().c_str());
	
	// Print current date
	// Get current date
	CString  strDate;
	CString  strTime;
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
	std::string Value = (strDate + _T(" ") + strTime).GetString();
	
	PrintValueObject(pDC, ptCurPos, _T("Current Date:"), Value.c_str() );
	
	// Print Application Section
	pDC->SelectObject(&m_fontSection);
	ptTemp      = ptCurPos;
	ptCurPos.y += PrintString(pDC, ptTemp, _T("\nConfiguration Settings"));
	
	ptCurPos.x  = ++nIndentLevel * m_shortTabPixels;
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
		Log_Assert(bResult);
		
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
		Log_Assert(bResult);
		
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
		Log_Assert(bResult);
		
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
		Log_Assert(bResult);
		
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
		Log_Assert(bResult);
		
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
	CDC* pDC = &m_printDC;
	
	OnVirtualPrint(TRUE);       // Real print
	
    CString  strPage;
	
	CFont*   pcfontOldFont;
	
	// Exchange font and store old one
	pcfontOldFont = pDC->SelectObject(&m_fontPageNbr);
	
	UINT uOldTextAlign = pDC->SetTextAlign(TA_CENTER | TA_NOUPDATECP);
	
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
		SvIe::SVVirtualCamera* pCamera = pConfig->GetCamera(l);
		if( nullptr != pCamera )
		{
			SvFs::FileNameContainer* pfnac = nullptr;
			SVLightReference* plrcDummy = nullptr;
			SVLut* plutDummy = nullptr;
			SVDeviceParamCollection* pDeviceParams = nullptr;
			SvIe::SVAcquisitionClassPtr pAcqDevice = pCamera->GetAcquisitionDevice();
			Log_Assert( nullptr != pAcqDevice);
			bool bOk = pConfig->GetAcquisitionDevice( pAcqDevice->DeviceName().c_str(), pfnac, plrcDummy, plutDummy, pDeviceParams );
			Log_Assert( bOk );
			Log_Assert( pfnac );
			SVDeviceParamCollection  DeviceParams;
			if (bOk)
			{
				if (nullptr != pAcqDevice && nullptr != pfnac && S_OK == pAcqDevice->GetDeviceParameters(DeviceParams))
				{
					pDeviceParams = &DeviceParams;
				}

				// print camera name
				ptCurPos.x   = (nIndentLevel+1) * m_shortTabPixels;
				if (nullptr != pCamera->GetAcquisitionDevice())
				{
					PrintValueObject(pDC, ptCurPos, pCamera->GetName(), pCamera->GetAcquisitionDevice()->DeviceName().c_str());
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
						std::string Value = std::format( _T("{:d}"), width );
						PrintValueObject( pDC, ptCurPos, _T("Image Width:"), Value.c_str() );

						long height = pCamera->GetFileImageHeight();
						Value = std::format( _T("{:d}"), height );
						PrintValueObject(pDC, ptCurPos, _T("Image Height:"), Value.c_str() );
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
					std::string Value = pfnac->GetFileNameList();
					SvUl::searchAndReplace( Value, _T(";"), _T("\n") );
					ptCurPos.y += PrintString(pDC, ptTemp, Value.c_str());

					// print camera device params
					if ( pDeviceParams && pDeviceParams->mapParameters.size() > 0 )
					{
						PrintValueObject(pDC, ptCurPos, _T("Camera Parameters:"), _T("") );
						SVDeviceParamCollection l_CameraFileParams;
						/*HRESULT hr =*/ pAcqDevice->GetCameraFileParameters(l_CameraFileParams);
						CPoint pt(ptCurPos);
						pt.x += m_shortTabPixels;

						SVDeviceParamIndexer index( pDeviceParams->mapParameters );	// print in order
						SVDeviceParamIndexer::iterator iter;
						for (iter = index.begin(); iter != index.end(); ++iter)
						{
							SVDeviceParamWrapper& rw = pDeviceParams->GetParameter( *iter );
							SVDeviceParam* pParam = static_cast <SVDeviceParam*> (rw);
							if (nullptr != pParam)
							{
								constexpr int cHiddenDetail = 99;
								SVDeviceParam* pCamFileParam = l_CameraFileParams.GetParameter(pParam->Type());
								if (nullptr != pCamFileParam && pCamFileParam->DetailLevel() != cHiddenDetail)
								{
									SVDeviceParamConfigPrintHelper helper(this, pDC, pt, nIndentLevel + 1);
									if (SVDeviceParamDataTypeEnum::DeviceDataTypeCustom == pParam->DataType())
									{
										pParam->SetName(pCamFileParam->VisualName());
									}
									else
									{
										pParam->SetName(pCamFileParam->Name());
									}

									helper.Visit(pParam);
								}
							}
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
		SvTrig::SVTriggerObject* pTrigger = pConfig->GetTrigger(l);
		if( nullptr != pTrigger )
		{
			ptCurPos.x   = (nIndentLevel + 1) * m_shortTabPixels;
			if ( pTrigger->getDevice() )
			{
				PrintValueObject(pDC, ptCurPos, pTrigger->GetName(), pTrigger->getDevice()->GetDeviceName());
			}
			else
			{
				PrintValueObject(pDC, ptCurPos, pTrigger->GetName(), _T("<No Device>"));
			}
			
			ptCurPos.x   = (nIndentLevel + 2) * m_shortTabPixels;

			if (SvDef::TriggerType::SoftwareTrigger == pTrigger->getType())
			{
				PrintValueObject(pDC, ptCurPos, _T("Trigger Type:"), _T("Software"));

				long period = pTrigger->GetSoftwareTriggerPeriod();
				std::string Value = std::format( _T("{:d} milliseconds"), period );
				PrintValueObject( pDC, ptCurPos, _T("Timer Period:"), Value.c_str() );
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
			PrintValueObject(pDC, ptCurPos, _T("Number of Panels"), _T("") );
		}
	}
}

void SVConfigurationPrint::PrintPPQSummary(CDC* pDC, CPoint& ptCurPos, int nIndentLevel)
{
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
			
			ptCurPos.x = (nIndentLevel + 1) * m_shortTabPixels;

			std::string Value;
			switch (pPPQ->getPPQOutputMode())
			{
				case SvDef::SVPPQNextTriggerMode:	// Resets outputs on trigger. Writes outputs immediately.
					Value = _T("NextTrigger");
					break;
				case SvDef::SVPPQTimeDelayMode:	// Resets outputs on trigger. Writes outputs after delay time is over, even if data isn't complete.
					// Aborts waiting if new trigger occurs.
					Value = _T("TimeDelay");
					break;
				case SvDef::SVPPQTimeDelayAndDataCompleteMode:	// Resets outputs on trigger. Writes outputs after delay time is over and data is complete.
					// Aborts waiting if new trigger occurs.
					Value = _T("TimeDelayDataCompletion");
					break;
				case SvDef::SVPPQExtendedTimeDelayMode:		// Doesn't reset outputs on trigger. Writes outputs after delay time is over, even if data isn't complete.
																	// Aborts waiting if product leaves PPQ.
					Value = _T("ExtendedTimeDelayMode");
					break;
				case SvDef::SVPPQExtendedTimeDelayAndDataCompleteMode: // Doesn't reset outputs on trigger. Writes outputs after delay time is over and data is complete.
					// Aborts waiting if product leaves PPQ.
					Value = _T("ExtendedTimeDelayDataCompletion");
					break;
				default:
					Value = _T("< I N V A L I D ! >");
					break;
			}
			
			PrintValueObject( pDC, ptCurPos, _T("PPQMode"), Value.c_str() );
			
			Value = std::format( _T("{:d}"), pPPQ->getPPQLength());
			PrintValueObject(pDC, ptCurPos, _T("PPQLength"), Value.c_str() );
			
			Value = std::format( _T("{:d}"), pPPQ->getResetDelay());
			PrintValueObject(pDC, ptCurPos, _T("PPQOutputResetDelay"), Value.c_str() );
			
			Value = std::format( _T("{:d}"), pPPQ->getOutputDelay());
			PrintValueObject(pDC, ptCurPos, _T("PPQOutputDelayTime"), Value.c_str() );

			Value = std::format(_T("{:d}"), pPPQ->getMaxProcessingOffset4Interest());
			PrintValueObject(pDC, ptCurPos, _T("PPQMaxProcessingOffsetGap4Interest"), Value.c_str());
			
			ptCurPos.x  = (nIndentLevel + 1) * m_shortTabPixels;
			ptTemp      = ptCurPos;
			ptCurPos.y += PrintString(pDC, ptTemp, _T("Trigger:"));
			
			Value.clear();
			if(nullptr != pPPQ->GetTrigger()) { Value = pPPQ->GetTrigger()->GetName();}
			ptCurPos.x  = (nIndentLevel + 2) * m_shortTabPixels;
			ptTemp      = ptCurPos;
			ptCurPos.y += PrintString(pDC, ptTemp, Value.c_str());
			
			ptCurPos.x  = (nIndentLevel + 1) * m_shortTabPixels;
			ptTemp      = ptCurPos;
			ptCurPos.y += PrintString(pDC, ptTemp, _T("Camera:"));
			
			SvIe::SVVirtualCameraPtrVector cameraVector = pPPQ->GetVirtualCameras(true);
			for (const auto* pCamera : cameraVector)
			{
				if (nullptr != pCamera)
				{
					ptCurPos.x = (nIndentLevel + 2) * m_shortTabPixels;
					ptTemp = ptCurPos;
					ptCurPos.y += PrintString(pDC, ptTemp, pCamera->GetName());
				}
			}

			ptCurPos.x  = (nIndentLevel + 1) * m_shortTabPixels;
			ptTemp      = ptCurPos;
			ptCurPos.y += PrintString(pDC, ptTemp, _T("Inspection:"));
			
			lSize = pPPQ->GetInspectionCount();
			for (int intInspection = 0; intInspection < lSize; intInspection++)
			{
				SVInspectionProcess* pInspection{ nullptr };
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
			SvStl::MessageManager e(SvStl::MsgType::Log );
			e.setMessage( SVMSG_SVO_55_DEBUG_BREAK_ERROR, SvStl::Tid_ErrorGettingPPQ, SvStl::SourceFileParams(StdMessageParams));
			DebugBreak();
			continue;
		}

		ptCurPos.x  = ++nIndentLevel * m_shortTabPixels;
		ptTemp      = ptCurPos;
		ptCurPos.y += PrintString(pDC, ptTemp, pPPQ->GetName());

		long lPPQLength = pPPQ->getPPQLength();
		for (int intPPQPos = 0; intPPQPos < lPPQLength; intPPQPos++)
		{
			bool bPosPrint = false;
			SvIe::SVVirtualCameraPtrVector cameraVector = pPPQ->GetVirtualCameras(true);
			
			for (const auto* const pCamera : cameraVector)
			{
				if ( nullptr != pCamera )
				{
					long lPos = -1;

					pPPQ->GetCameraPPQPosition( lPos, pCamera );

					if (lPos == intPPQPos)
					{
						if (!bPosPrint)
						{
							std::string Label = std::format( _T("Position {:d}:"), intPPQPos + 1 );
							ptCurPos.x  = (nIndentLevel + 1) * m_shortTabPixels;
							ptTemp      = ptCurPos;
							ptCurPos.y += PrintString(pDC, ptTemp, Label.c_str() );
							bPosPrint = true;
						}

						ptCurPos.x  = (nIndentLevel + 2) * m_shortTabPixels;
						ptTemp      = ptCurPos;
						ptCurPos.y += PrintString(pDC, ptTemp, pCamera->GetName());
					}
				}
			}

			for(const auto& pEntry : pPPQ->GetUsedInputs())
			{
				if (pEntry->m_PPQIndex == intPPQPos)
				{
					SVObjectClass* l_pObject = SVObjectManagerClass::Instance().GetObject( pEntry->m_IOId );

					if (!bPosPrint)
					{
						std::string Label = std::format( _T("Position {:d}:"), intPPQPos + 1 );
						ptCurPos.x  = (nIndentLevel + 1) * m_shortTabPixels;
						ptTemp      = ptCurPos;
						ptCurPos.y += PrintString(pDC, ptTemp, Label.c_str() );
						bPosPrint = true;
					}

					ptCurPos.x  = (nIndentLevel + 2) * m_shortTabPixels;
					ptTemp      = ptCurPos;
                    if ( nullptr != l_pObject )
                    {
                        if ( l_pObject->IsCreated() )
                        {
							ptCurPos.y += PrintString(pDC, ptTemp, l_pObject->GetName());
                        }//end if
                    }
                    else
                    {
                        if ( pEntry->getObject()->IsCreated() )
                        {
                            ptCurPos.y += PrintString(pDC, ptTemp, pEntry->m_name.c_str());
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

	long     lSize = 0;
	SVInspectionProcess*    pInspection( nullptr );
	
	// Print all IPDocs
	//The nullptr check here is enough because then lSize would be 0
	if( nullptr != pConfig) { lSize = pConfig->GetInspectionCount(); }
	for (int nIPDNumber = 0; nIPDNumber < lSize; ++nIPDNumber)
	{
		pInspection = pConfig->GetInspection(nIPDNumber);
		if( nullptr != pInspection )
		{
			SVToolSet* pToolSet = pInspection->GetToolSet();
			pDC->SelectObject(&m_fontSection);
		
			m_toolNumber = 0;
		
			// Print IPDoc number
			std::string sLabel = std::format(_T("\nInspection Process {:d}"), nIPDNumber + 1);
			ptCurPos.x  = nIndentLevel * m_shortTabPixels;
			CPoint ptTemp      = ptCurPos;
			ptCurPos.y += PrintString(pDC, ptTemp, sLabel.c_str());
			pDC->SelectObject(&m_fontText);
		
			PrintValueObject(pDC, ptCurPos, _T("Title:"), pInspection->GetName());
		
			std::string sValue;
			// Print number of IPD tools
			if ( nullptr != pToolSet )
			{
				sValue = std::format( _T("{:d}"), pToolSet->GetSize() );
			}
			else
			{
				sValue = SvUl::LoadStdString(IDS_NOT_AVAILABLE_STRING);
			}
		
			ptCurPos.x   = nIndentLevel * m_shortTabPixels;
			PrintValueObject(pDC, ptCurPos, _T("Number of Tools:"), sValue.c_str());
		
			// Print all tools
			if( nullptr != pToolSet)
			{
				PrintObject(pDC, pToolSet, ptCurPos, nIndentLevel);
			}  // end if( pToolSet )
		}
	} 
}

void SVConfigurationPrint::PrintIOSection(CDC* pDC, CPoint& ptCurPos, int nIndentLevel)
{
	CPoint	ptTemp(0, 0);
	
	pDC->SelectObject(&m_fontSection);
	ptCurPos.x  = nIndentLevel * m_shortTabPixels;
	ptTemp      = ptCurPos;
	ptCurPos.y += PrintString( pDC, ptTemp, _T("\nInput/Output:") );
	pDC->SelectObject(&m_fontText);
	
	PrintModuleIO(pDC, ptCurPos, nIndentLevel);
	PrintResultIO(pDC, ptCurPos, nIndentLevel);
	PrintMonitorListSection(pDC, ptCurPos, nIndentLevel);
	PrintGlobalConstants( pDC, ptCurPos, nIndentLevel );
}

void SVConfigurationPrint::PrintModuleIO(CDC* pDC, CPoint& ptCurPos, int nIndentLevel)
{
	CPoint	ptTemp(0, 0);

	SVConfigurationObject* pConfig( nullptr );
	SVObjectManagerClass::Instance().GetConfigurationObject( pConfig );

	// Print IODoc contents
	if (GetTheIODoc())
	{
		SVInputObjectList* pInputList = nullptr;
		SVDigitalInputObject* pDigInput;
		SVIOEntryHostStructPtrVector inputEntryVector;

		// Get list of available inputs
		if ( nullptr != pConfig ) { pInputList = pConfig->GetInputObjectList(); }
		if (nullptr != pInputList)
		{
			inputEntryVector = pInputList->getInputList();
		}
		else
		{
			SvStl::MessageManager e(SvStl::MsgType::Log );
			e.setMessage( SVMSG_SVO_55_DEBUG_BREAK_ERROR, SvStl::Tid_ErrorGettingInputObjectList, SvStl::SourceFileParams(StdMessageParams));
			DebugBreak();
		}

		long lSize = static_cast< long >(inputEntryVector.size() );

		// Print module input title...
		long maxInput {SVIOConfigurationInterfaceClass::Instance().GetDigitalInputCount()};

		std::string Value = std::format(_T("{:d}"), maxInput);
		ptCurPos.x = nIndentLevel * m_shortTabPixels;
		PrintValueObject(pDC, ptCurPos, _T("Digital Inputs:"), Value.c_str() );

		// Module Inputs
		for (long i = 0; i < (maxInput); ++i)
		{
			std::string Label = std::format( _T("Digital Input {:d}:"), i+1 );

			// Find each digital input
			for (const auto& pIOEntry : inputEntryVector)
			{
				if (pIOEntry->m_ObjectType != IO_DIGITAL_INPUT)
				{
					continue;
				}

				pDigInput = dynamic_cast< SVDigitalInputObject* >(SVObjectManagerClass::Instance().GetObject(pIOEntry->m_IOId));
				if (!pDigInput)
				{
					continue;
				}

				if (i == pDigInput->GetChannel())
				{
					PrintIOEntryObject(pDC, ptCurPos, nIndentLevel + 1, Label.c_str(), pIOEntry);
					break;
				}
			}
		}

		// Find each Remote input
		int j = 0;
		for (int i = 0; i < lSize; i++)
		{
			if (inputEntryVector[i]->m_ObjectType == IO_REMOTE_INPUT)
			{
				++j;
			}
		}
		Value = std::format( _T("{:d}"), j );
		ptCurPos.x = nIndentLevel * m_shortTabPixels;
		PrintValueObject( pDC, ptCurPos, _T("Remote Inputs:"), Value.c_str() );

		if (j)
		{
			for( int k = 0, l = 0; k < lSize; k++ )
			{
				if (inputEntryVector[k]->m_ObjectType != IO_REMOTE_INPUT) { continue; }

				std::string Label = SvO::cRemoteInputNumberLabel + std::to_string ( (++l) +1 );
				PrintIOEntryObject(pDC, ptCurPos, nIndentLevel+1, Label.c_str(), inputEntryVector[k]);
			}
		}
	}
}  // end function void SVObserverApp::PrintModuleIOP( CDC* pDC, ... )

void SVConfigurationPrint::PrintResultIO(CDC* pDC, CPoint& ptCurPos, int nIndentLevel)
{
	CPoint	ptTemp(0, 0);

	SVConfigurationObject* pConfig( nullptr );
	SVObjectManagerClass::Instance().GetConfigurationObject( pConfig );

	// Get the number of PPQs
	long lPPQSize = 0;
	if ( nullptr != pConfig )
	{
		lPPQSize = pConfig->GetPPQCount();
	}
	else
	{
		SvStl::MessageManager e(SvStl::MsgType::Log );
		e.setMessage( SVMSG_SVO_55_DEBUG_BREAK_ERROR, SvStl::Tid_ErrorGettingPPQCount, SvStl::SourceFileParams(StdMessageParams));
		DebugBreak();
	}

	// Print IODoc contents
	if (GetTheIODoc())
	{
		// Print Result Output title...
		long maxOutput {SVIOConfigurationInterfaceClass::Instance().GetDigitalOutputCount()};
		std::string Value = std::format( _T("{:d}"), maxOutput );
		ptCurPos.x = nIndentLevel * m_shortTabPixels;
		PrintValueObject( pDC, ptCurPos, _T("Result Outputs:"), Value.c_str() );

		// Result Outputs
		for (long i = 0; i < maxOutput; ++i)
		{
			std::string Label = std::format( _T("Digital Output {:d}"), i+1 );

			SVIOEntryHostStructPtr pModuleReady = pConfig->GetModuleReady();

			// Check Module Ready first
			SVDigitalOutputObject* pDigOutput = (nullptr == pModuleReady) ? nullptr : dynamic_cast< SVDigitalOutputObject* >(SVObjectManagerClass::Instance().GetObject(pModuleReady->m_IOId));
			if (pDigOutput)
			{
				if (i == pDigOutput->GetChannel())
				{
					PrintIOEntryObject(pDC, ptCurPos, nIndentLevel + 1, Label.c_str(), pModuleReady);
					continue;
				}// end if
			}// end if

			for (int j = 0; j < lPPQSize; j++)
			{
				SVPPQObject* pPPQ = pConfig->GetPPQ( j );
				if ( nullptr == pPPQ )
				{
					SvStl::MessageManager e(SvStl::MsgType::Log );
					e.setMessage( SVMSG_SVO_55_DEBUG_BREAK_ERROR, SvStl::Tid_ErrorGettingPPQ, SvStl::SourceFileParams(StdMessageParams));
					DebugBreak();
				}

				// Find each digital output
				for (const auto& pIOEntry : pPPQ->getUsedOutputs())
				{
					if ( pIOEntry->m_ObjectType != IO_DIGITAL_OUTPUT ) { continue; }

					pDigOutput = dynamic_cast<SVDigitalOutputObject*> (SVObjectManagerClass::Instance().GetObject( pIOEntry->m_IOId ));

					if (nullptr == pDigOutput ) { continue; }

					if (i == pDigOutput->GetChannel())
					{
						PrintIOEntryObject(pDC, ptCurPos, nIndentLevel + 1, Label.c_str(), pIOEntry);
						continue;
					}// end if
				}// end for k
			}// end for j
		}// end for
	}
}  // end function void SVObserverApp::PrintResultIO( CDC* pDC, ... )

void SVConfigurationPrint::PrintMonitorListSection(CDC* pDC, CPoint& ptCurPos, int nIndentLevel)
{
	SVConfigurationObject* pConfig( nullptr );
	SVObjectManagerClass::Instance().GetConfigurationObject( pConfig );

	if( nullptr != pConfig )
	{
		ptCurPos.x = nIndentLevel * m_shortTabPixels;

		PrintValueObject(pDC, ptCurPos, _T("Remote Monitor List"), "");

		const RemoteMonitorListMap& remoteMonitorLists = pConfig->GetRemoteMonitorList();

		RemoteMonitorListMap::const_iterator iterMonitorList = remoteMonitorLists.begin();
		while ( remoteMonitorLists.end() != iterMonitorList )
		{
			const std::string& ListName = iterMonitorList->first;
			const RemoteMonitorNamedList& monitorList = iterMonitorList->second;

			// Write out list name
			PrintMonitorListItem(pDC, ptCurPos, nIndentLevel+1, _T("Monitor List Name"), ListName.c_str());

			// Write out PPQ 
			PrintMonitorListItem(pDC, ptCurPos, nIndentLevel+1, _T("PPQ"), monitorList.GetPPQName().c_str());

			// Write out reject queue depth
			int Depth = monitorList.GetRejectDepthQueue();
			std::string Value = std::format( _T("{:d}"), Depth );
			PrintMonitorListItem(pDC, ptCurPos, nIndentLevel+1, _T("Reject Queue Depth"), Value.c_str());

			// Print Product Value List
			const MonitoredObjectList& ValueList = monitorList.GetProductValuesList();
			MonitoredObjectList::const_iterator vlIt = ValueList.begin();
			PrintMonitorListItem(pDC, ptCurPos, nIndentLevel+1, _T("Product Value List"), "");
			while( vlIt != ValueList.end() )
			{
				const MonitoredObject& rObj = *vlIt;
				const std::string& rObjectName = RemoteMonitorListHelper::GetNameFromMonitoredObject(rObj);
				if ( !rObjectName.empty() )
				{
					Value = rObjectName;
					PrintMonitorListItem(pDC, ptCurPos, nIndentLevel+2, Value.c_str(), "");
				}
				++vlIt;
			}			

			// Print Product Image List
			const MonitoredObjectList& ImageList = monitorList.GetProductImagesList();
			MonitoredObjectList::const_iterator ilIt = ImageList.begin();
			PrintMonitorListItem(pDC, ptCurPos, nIndentLevel+1, _T("Product Image List"), "");
			while( ilIt != ImageList.end() )
			{
				const MonitoredObject& rObj = *ilIt;
				const std::string& rObjectName = RemoteMonitorListHelper::GetNameFromMonitoredObject(rObj);
				if ( !rObjectName.empty() )
				{
					Value = rObjectName;
					PrintMonitorListItem(pDC, ptCurPos, nIndentLevel+2, Value.c_str(), "");
				}
				++ilIt;
			}

			// Print Reject Condition List
			const MonitoredObjectList& RejectList = monitorList.GetRejectConditionList();
			MonitoredObjectList::const_iterator rlIt = RejectList.begin();
			PrintMonitorListItem(pDC, ptCurPos, nIndentLevel+1, _T("Reject Condition List"), "");
			while( rlIt != RejectList.end() )
			{
				const MonitoredObject& rObj = *rlIt;
				const std::string& rObjectName = RemoteMonitorListHelper::GetNameFromMonitoredObject(rObj);
				if ( !rObjectName.empty() )
				{
					Value = rObjectName;
					PrintMonitorListItem(pDC, ptCurPos, nIndentLevel+2, Value.c_str(), "");
				}
				++rlIt;
			}

			// Print Fail Status List
			const MonitoredObjectList& FailList = monitorList.GetFailStatusList();
			MonitoredObjectList::const_iterator flIt = FailList.begin();
			PrintMonitorListItem(pDC, ptCurPos, nIndentLevel+1, _T("Fail Status List"), "");
			while( flIt != FailList.end() )
			{
				const MonitoredObject& rObj = *flIt;
				const std::string& rObjectName = RemoteMonitorListHelper::GetNameFromMonitoredObject(rObj);
				if ( !rObjectName.empty() )
				{
					Value = rObjectName.c_str();
					PrintMonitorListItem(pDC, ptCurPos, nIndentLevel+2, Value.c_str(), "");
				}
				++flIt;
			}
			++iterMonitorList;
		}
	}
}

void SVConfigurationPrint::PrintGlobalConstants( CDC* pDC, CPoint& ptCurPos, int)
{
	std::string Label;
	std::string Value;
	int Index (0);

	SvVol::BasicValueObjects::ValueVector GlobalConstantObjects;
	RootObject::getRootChildObjectList( GlobalConstantObjects, SvDef::FqnGlobal );

	Label = _T( "Global Constants" );
	Value = std::format( _T("{:d}"),  GlobalConstantObjects.size() );
	PrintValueObject(pDC, ptCurPos, Label.c_str(), Value.c_str() );
	SvVol::BasicValueObjects::ValueVector::const_iterator Iter( GlobalConstantObjects.cbegin() );
	while ( GlobalConstantObjects.cend() != Iter )
	{
		const SvVol::BasicValueObjectPtr& pGlobalConstant = *Iter;

		if(nullptr != pGlobalConstant)
		{
			//Print Global Constant name
			Label = std::format( _T("Global Constant {:d}"), ++Index );
			Value = pGlobalConstant->GetCompleteName();
			PrintValueObject(pDC, ptCurPos, Label.c_str(), Value.c_str() );

			//Print Global Constant value
			Label =  _T("Value");
			pGlobalConstant->getValue( Value );
			PrintValueObject(pDC, ptCurPos, Label.c_str(), Value.c_str() );

			//Print Global Constant type
			Label = _T("Type");
			Value = pGlobalConstant->getTypeName();
			PrintValueObject(pDC, ptCurPos, Label.c_str(), Value.c_str() );
		}

		++Iter;
	}
}

SVDeviceParamConfigPrintHelper::SVDeviceParamConfigPrintHelper(SVConfigurationPrint* pPrint, CDC* pDC, CPoint& ptCurPos, int nIndentLevel)
	: m_pPrint(pPrint), m_pDC(pDC), m_rptCurPos(ptCurPos), m_nIndentLevel(nIndentLevel)
{
}

HRESULT SVDeviceParamConfigPrintHelper::Visit(SVDeviceParam* pParam)
{
	if (nullptr != pParam)
	{
		if (SVDeviceParamEnum::DeviceParamCameraFormats == pParam->Type())
		{
			SVCameraFormatsDeviceParam* pCameraFormat = dynamic_cast<SVCameraFormatsDeviceParam*> (pParam);
			return Visit(pCameraFormat);
		}
		switch (pParam->DataType())
		{
			case SVDeviceParamDataTypeEnum::DeviceDataTypeLong:
			{
				SVLongValueDeviceParam* pLong = dynamic_cast<SVLongValueDeviceParam*> (pParam);
				return Visit(pLong);
			}

			case SVDeviceParamDataTypeEnum::DeviceDataTypei64:
			{
				SVi64ValueDeviceParam* pI64 = dynamic_cast<SVi64ValueDeviceParam*> (pParam);
				return Visit(pI64);
			}

			case SVDeviceParamDataTypeEnum::DeviceDataTypeBool:
			{
				SVBoolValueDeviceParam* pBool = dynamic_cast<SVBoolValueDeviceParam*> (pParam);
				return Visit(pBool);
			}

			case SVDeviceParamDataTypeEnum::DeviceDataTypeString:
			{
				SVStringValueDeviceParam* pString = dynamic_cast<SVStringValueDeviceParam*> (pParam);
				return Visit(pString);
			}

			case SVDeviceParamDataTypeEnum::DeviceDataTypeCustom:
			{
				SVCustomDeviceParam* pCustom = dynamic_cast<SVCustomDeviceParam*> (pParam);
				return Visit(pCustom);
			}

			default:
			{
				return S_OK;
			}
		}
	}
	return E_POINTER;
}

HRESULT SVDeviceParamConfigPrintHelper::Visit(SVLongValueDeviceParam* pParam)
{
	if (nullptr != pParam)
	{
		std::string Value;
		if (pParam->info.options.size() > 0 )
		{
			SVLongValueDeviceParam::OptionsType::const_iterator iterOption;
			iterOption = std::find(pParam->info.options.begin(), pParam->info.options.end(), pParam->lValue );
			if ( iterOption != pParam->info.options.end() )
			{
				Value = iterOption->m_Description;
			}
		}
		if ( Value.empty() )
		{
			Value = std::format( _T("{}{}"), static_cast<unsigned long> (pParam->GetScaledValue()), pParam->info.sUnits.empty() ? _T("") : std::string(_T(" ") + pParam->info.sUnits).c_str() );
		}
		m_pPrint->PrintValueObject(m_pDC, m_rptCurPos, pParam->Name(), Value.c_str() );
		return S_OK;
	}
	return E_POINTER;
}

HRESULT SVDeviceParamConfigPrintHelper::Visit(SVi64ValueDeviceParam* pParam)
{
	if (nullptr != pParam)
	{
		std::string Text = std::format( _T("{:d}"), pParam->iValue );
		m_pPrint->PrintValueObject(m_pDC, m_rptCurPos, pParam->Name(), Text.c_str() );
		return S_OK;
	}
	return E_POINTER;
}

HRESULT SVDeviceParamConfigPrintHelper::Visit(SVBoolValueDeviceParam* pParam)
{
	if (nullptr != pParam)
	{
		std::string Text;
		if (pParam->info.options.size() > 0 )
		{
			SVBoolValueDeviceParam::OptionsType::const_iterator iterOption;
			iterOption = std::find(pParam->info.options.begin(), pParam->info.options.end(), pParam->bValue );
			if ( iterOption != pParam->info.options.end() )
			{
				Text = iterOption->m_Description.c_str();
			}
		}
		if ( Text.empty() )
		{
			Text = pParam->bValue ? _T("TRUE") : _T("FALSE");
		}
		m_pPrint->PrintValueObject(m_pDC, m_rptCurPos, pParam->Name(), Text.c_str());
		return S_OK;
	}
	return E_POINTER;
}

HRESULT SVDeviceParamConfigPrintHelper::Visit(SVStringValueDeviceParam* pParam)
{
	if (nullptr != pParam)
	{
		m_pPrint->PrintValueObject(m_pDC, m_rptCurPos, pParam->Name(), pParam->strValue.c_str() );
		return S_OK;
	}
	return E_POINTER;
}

HRESULT SVDeviceParamConfigPrintHelper::Visit(SVCameraFormatsDeviceParam* pParam)
{
	if (nullptr != pParam)
	{
		std::string Text;
		SVCameraFormat* pFormat = nullptr;
		if (pParam->options.size() > 0 )
		{
			SVCameraFormat& rFormat = pParam->options[pParam->strValue];
			const SVCameraFormat& rCamFileFormat = pParam->options.find(pParam->strValue )->second;
			if ( rCamFileFormat.m_strName == pParam->strValue )
			{
				Text = rCamFileFormat.m_strDescription;
				pFormat = &rFormat;
			}
		}
		if ( Text.empty() )
		{
			Text = pParam->strValue;
		}
		m_pPrint->PrintValueObject(m_pDC, m_rptCurPos, pParam->Name(), Text.c_str() );

		if ( pFormat )
		{
			CPoint pt = m_rptCurPos;
			pt.x += m_pPrint->m_shortTabPixels;

			m_pPrint->PrintValueObject( m_pDC, pt, _T("Left"), SvUl::AsString(pFormat->m_lHPos).c_str() );
			m_pPrint->PrintValueObject( m_pDC, pt, _T("Top"), SvUl::AsString(pFormat->m_lVPos).c_str() );
			m_pPrint->PrintValueObject( m_pDC, pt, _T("Width"), SvUl::AsString(pFormat->m_lWidth).c_str() );
			m_pPrint->PrintValueObject( m_pDC, pt, _T("Height"), SvUl::AsString(pFormat->m_lHeight).c_str() );
			
			m_rptCurPos.y = pt.y;
		}
	}
	return S_OK;
}

HRESULT SVDeviceParamConfigPrintHelper::Visit(SVCustomDeviceParam* pParam)
{
	HRESULT result {E_POINTER};
	if (nullptr != pParam)
	{
		SVDeviceParamWrapper w(pParam->GetHeldParam());
		SVDeviceParam* pHeldParam = static_cast<SVDeviceParam*> (w);
		pHeldParam->SetName(pParam->Name());
		result = Visit(pHeldParam);
	}
	return result;
}

