//******************************************************************************
//* COPYRIGHT (c) 2003 by SVResearch, Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVResultsWrapperClass.h
//* .File Name       : $Workfile:   SVResultsWrapperClass.h  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.0  $
//* .Check In Date   : $Date:   23 Apr 2013 14:58:18  $
//******************************************************************************
#pragma once

#pragma region Includes
#include "SVCmnLib/SVXmlCmd.h"
#pragma endregion Includes

class SVResultsWrapperClass
{
public:
	SVResultsWrapperClass() {}
	virtual ~SVResultsWrapperClass() {}
	virtual BOOL AddData(int nItem, const CString& szIPD, const CString& szCol1, const CString& szCol2, const CString& szCol3, const CString& szCol4, DWORD dwColor, int nType)=0;
};

class SVResultsListCtrlWrapper : public SVResultsWrapperClass
{
	CListCtrl* m_pClc;
public:
	//default constructor
	SVResultsListCtrlWrapper() : m_pClc(nullptr) {}
	SVResultsListCtrlWrapper(CListCtrl * pClc) :m_pClc(pClc) {}
	virtual ~SVResultsListCtrlWrapper() {}
	virtual BOOL AddData(int nItem, const CString& szIPD, const CString& szCol1, const CString& szCol2, const CString& szCol3, const CString& szCol4, DWORD dwColor, int nType)
	{
		if (nullptr != m_pClc)
		{
			if ( m_pClc->GetItemCount() <= nItem )
			{
				m_pClc->InsertItem( nItem, szCol1 );
				m_pClc->SetItemText( nItem, 2, szCol3 );
				m_pClc->SetItemText( nItem, 3, szCol4 );
			}

			m_pClc->SetItemText( nItem, 1, szCol2 );
			m_pClc->SetItemData( nItem, dwColor );

			return TRUE;
		}
		return FALSE;
	}
};

class SVResultsXmlWrapper : public SVResultsWrapperClass
{
public:
	SVResultsXmlWrapper()
	: m_pSVXmlCmd(nullptr) {}
	
	SVResultsXmlWrapper(SVXmlCmd* pSVXmlCmd)
	: m_pSVXmlCmd(pSVXmlCmd) {}

	virtual ~SVResultsXmlWrapper() {}

	virtual BOOL AddData(int nItem, const CString& szIPD, const CString& szCol1, const CString& szCol2, const CString& szCol3, const CString& szCol4, DWORD dwColor, int nType)
	{
		CString szType;
		if (nullptr != m_pSVXmlCmd)
		{
			switch(nType)
			{
				// WTF are these Magic Numbers!!!
				case 0x00000000: szType = _T("Not Set"); break;
				case 0x00000001: szType = _T("System"); break; 
				case 0x00000002: szType = _T("Digitizer"); break;
				case 0x00000004: szType = _T("IPDocument"); break;
				case 0x00000005: szType = _T("FreezeSet"); break;
				case 0x00000006: szType = _T("ToolSet"); break;
				case 0x00000007: szType = _T("Tool"); break;
				case 0x00000008: szType = _T("Equation"); break;
				case 0x00000009: szType = _T("ROI"); break;
				case 0x0000000a: szType = _T("Image Arithmetic"); break;
				case 0x0000000b: szType = _T("Math Container"); break;
				case 0x0000000c: szType = _T("Rotate Reference"); break;
				case 0x00000010: szType = _T("Line"); break;
				case 0x00000011: szType = _T("Image"); break; 
				case 0x00000012: szType = _T("Data Buffer Class"); break;
				case 0x00000030: szType = _T("Image To Line Project"); break;
				case 0x00000040: szType = _T("Transform"); break; 
				case 0x00000041: szType = _T("Polar Transform"); break;
				case 0x00000100: szType = _T("DWord Value"); break;	
				case 0x00000101: szType = _T("Long Value"); break; 
				case 0x00000102: szType = _T("Double Value"); break; 
				case 0x00000103: szType = _T("Bool Value"); break;
				case 0x00000104: szType = _T("Point Value"); break;	
				case 0x00000105: szType = _T("Byte Value"); break;
				case 0x00000106: szType = _T("String Value"); break;
				case 0x00000107: szType = _T("Enum Value"); break;
				case 0x00001000: szType = _T("Operator"); break;
				case 0x00002000: szType = _T("Binary Operator"); break;	
				case 0x00003000: szType = _T("Unary Operator"); break;
				case 0x00004000: szType = _T("Unary Image Operator"); break;
				case 0x00005000: szType = _T("Filter"); break;	
				case 0x00006000: szType = _T("Unary Image Operator List"); break;	
				case 0x00010000: szType = _T("Analyzer"); break;
				case 0x00020000: szType = _T("Acquisition"); break; 
				case 0x00100000: szType = _T("Result"); break;	
				case 0x00200000: szType = _T("Range"); break; 
				case 0x10000000: szType = _T("Vector"); break;
				default : szType = _T("Unknown"); break;
			}
			m_pSVXmlCmd->SetResultsData(szIPD, szCol3, szCol2, szType);
			return TRUE;
		}
		return FALSE;
	}
private:
	SVXmlCmd* m_pSVXmlCmd;
};

