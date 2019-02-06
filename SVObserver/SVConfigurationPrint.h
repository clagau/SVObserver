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

#pragma once

#pragma region Includes
#include "SVFileSystemLibrary/SVFileNameClass.h"
#include "SVInfoStructs.h"
#pragma endregion Includes

#pragma region Declarations
enum
{
	SV_PRINT_LINES_PER_PAGE                     = 70,	// Sri 2/16/00
	SV_DEFAULT_PRINT_TITLE_FONT_HEIGHT          = 100,

	SV_DEFAULT_PRINT_SUB_TITLE_FONT_HEIGHT      = 90,

	SV_DEFAULT_PRINT_TEXT_FONT_HEIGHT           = 80,

	SV_DEFAULT_PAGE_TEXT_FONT_HEIGHT            = 80,
};

namespace SvIe
{
class SVTaskObjectListClass;
}
class SVObjectClass;
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
	void PrintAllChildren(CDC* pDC, SvIe::SVTaskObjectListClass* pTaskObj,  CPoint& ptCurPos, int nIndentLevel);
	void PrintChildren(CDC* pDC, SVObjectClass* pObj, CPoint& ptCurPos, int nIndentLevel);
	void PrintDetails(CDC* pDC, SVObjectClass* pObj, CPoint& ptCurPos, int nIndentLevel);
	void PrintObject(CDC* pDC, SVObjectClass* pObj, CPoint& ptCurPos, int nIndentLevel);

	int  PrintString(CDC* pDC, CPoint& ptCurPos, LPCTSTR lpszStr);
	void PrintValueObject(CDC* pDC, CPoint& ptCurPos, LPCTSTR Name, LPCTSTR Value);

	void PrintCameraSummary(CDC* pDC, CPoint& ptCurPos, int nIndentLevel);
	void PrintTriggerSummary(CDC* pDC, CPoint& ptCurPos, int nIndentLevel);
	void PrintInspectionSummary(CDC* pDC, CPoint& ptCurPos, int nIndentLevel);
	void PrintPPQSummary(CDC* pDC, CPoint& ptCurPos, int nIndentLevel);

	void PrintInputOutputList(CDC* pDC, SVObjectClass* pObj, CPoint& ptCurPos, int nIndentLevel);
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
	std::string* m_pBuffer;
#pragma endregion Member Variables
};
