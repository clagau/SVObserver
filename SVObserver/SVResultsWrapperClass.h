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

#include "SVCmnLib/SVXmlCmd.h"

class SVResultsWrapperClass
{
public:
	SVResultsWrapperClass(){};
	~SVResultsWrapperClass(){};
	virtual BOOL AddData(int nItem,CString& szIPD,CString& szCol1,CString& szCol2,CString& szCol3,CString& szCol4, DWORD dwColor, int nType)=0;
};


class SVResultsListCtrlWrapper : public SVResultsWrapperClass
{
	CListCtrl* m_pClc;
public:
	//default constructor
	SVResultsListCtrlWrapper(){m_pClc = NULL;};
	SVResultsListCtrlWrapper(CListCtrl * pClc){m_pClc = pClc;};
	~SVResultsListCtrlWrapper(){};
	virtual BOOL AddData(int nItem,CString& szIPD,CString& szCol1,CString& szCol2,CString& szCol3,CString& szCol4, DWORD dwColor, int nType)
	{
		if(m_pClc != NULL)
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
		else
		{
			return FALSE;
		}
	};
};


class SVResultsXmlWrapper : public SVResultsWrapperClass
{
public:
	//default constructor
	SVResultsXmlWrapper(){m_pSVXmlCmd = NULL;};
	SVResultsXmlWrapper(SVXmlCmd* pSVXmlCmd)
	{
	m_pSVXmlCmd = pSVXmlCmd;
	};
	~SVResultsXmlWrapper();
	virtual BOOL AddData(int nItem,CString& szIPD,CString& szCol1,CString& szCol2,CString& szCol3,CString& szCol4, DWORD dwColor, int nType)
	{
		CString szType;
		if(m_pSVXmlCmd != NULL)
		{
		switch(nType)
			{
			case 0x00000000: szType = "Not Set"; break;
			case 0x00000001: szType = "System"; break; 
			case 0x00000002: szType = "Digitizer"; break;
			case 0x00000004: szType = "IPDocument"; break;
			case 0x00000005: szType = "FreezeSet"; break;
			case 0x00000006: szType = "ToolSet"; break;
			case 0x00000007: szType = "Tool"; break;
			case 0x00000008: szType = "Equation"; break;
			case 0x00000009: szType = "ROI"; break;
			case 0x0000000a: szType = "Image Arithmetic"; break;
			case 0x0000000b: szType = "Math Container"; break;
			case 0x0000000c: szType = "Rotate Reference	"; break;
			case 0x00000010: szType = "Line"; break;
			case 0x00000011: szType = "Image"; break; 
			case 0x00000012: szType = "Data Buffer Class"; break;
			case 0x00000030: szType = "Image To Line Project"; break;
			case 0x00000040: szType = "Transform"; break; 
			case 0x00000041: szType = "Polar Transform"; break;
			case 0x00000100: szType = "DWord Value"; break;	
			case 0x00000101: szType = "Long Value"; break; 
			case 0x00000102: szType = "Double Value"; break; 
			case 0x00000103: szType = "Bool Value"; break;
			case 0x00000104: szType = "Point Value"; break;	
			case 0x00000105: szType = "Byte Value"; break;
			case 0x00000106: szType = "String Value"; break;
			case 0x00000107: szType = "Enum Value"; break;
			case 0x00001000: szType = "Operator"; break;
			case 0x00002000: szType = "Binary Operator"; break;	
			case 0x00003000: szType = "Unary Operator"; break;
			case 0x00004000: szType = "Unary Image Operator"; break;
			case 0x00005000: szType = "Filter"; break;	
			case 0x00006000: szType = "Unary Image Operator List"; break;	
			case 0x00010000: szType = "Analyzer"; break;
			case 0x00020000: szType = "Acquisition"; break; 
			case 0x00100000: szType = "Result"; break;	
			case 0x00200000: szType = "Range"; break; 
			case 0x10000000: szType = "Vector";break;
			default : szType = "Unknown";
			}
		m_pSVXmlCmd->SetResultsData(szIPD, szCol3, szCol2, szType);
		return TRUE;
		}
		else
		{
		return FALSE;
		}
	};
private:
	SVXmlCmd* m_pSVXmlCmd;
};

