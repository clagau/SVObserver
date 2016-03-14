//******************************************************************************
//* COPYRIGHT (c) 2013 by Seidenader Vision Inc., Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVXmlCmd
//* .File Name       : $Workfile:   SVXmlCmd.h  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.0  $
//* .Check In Date   : $Date:   18 Apr 2013 18:10:10  $
//******************************************************************************

#if !defined(AFX_SVXMLCMD_H__26B5F1B3_FE83_11D4_BDC4_0050B71464A5__INCLUDED_)
#define AFX_SVXMLCMD_H__26B5F1B3_FE83_11D4_BDC4_0050B71464A5__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "SVXml.h"

//##ModelId=3A9E4BC60314
class SVXmlCmd : public SVXml  
{
public:
	//##ModelId=3A9E4BC6034A
	BOOL GetXmlDoc(BSTR * bstrDoc);
	//##ModelId=3A9E4BC60345
	BOOL GetResultsData(int index,CString szIPD, CString& szFullname, CString& szValue, CString& szType);
	//##ModelId=3A9E4BC60343
	BOOL LoadDoc(BSTR *);
	//##ModelId=3A9E4BC60342
	long EnumerateResultsData(LPTSTR);
	//##ModelId=3A9E4BC60341
	SVXmlCmd();
	//##ModelId=3A9E4BC60340
	virtual ~SVXmlCmd();
	//##ModelId=3A9E4BC6033D
	BOOL InitXml();
	//##ModelId=3A9E4BC6033E
	BOOL InitXml(LPCTSTR szSchemaFileName);
	//##ModelId=3A9E4BC6033B
	BOOL SetCommand(LPCTSTR Data);
//	BOOL SetCmdType(LPCTSTR Data);
	//##ModelId=3A9E4BC60339
	BOOL SetSVIMfilename(LPCTSTR Data);
	//##ModelId=3A9E4BC60337
	BOOL SetRemotefilename(LPCTSTR Data);
	//##ModelId=3A9E4BC60335
	BOOL SetSVIMstate(ULONG Data);
	//##ModelId=3A9E4BC60332
	BOOL SetBinData(BYTE *pBuf, ULONG FileLength);
	//##ModelId=3A9E4BC6032E
	BOOL SetResultsData(CString szIPD, CString szFullname, CString szValue, CString szType);
	
	BOOL ClearResultsData();

	//##ModelId=3A9E4BC6032D
	BOOL GetCommand(LPTSTR*);
//	LPTSTR GetCmdType();
	//##ModelId=3A9E4BC6032C
	BOOL GetSVIMfilename(LPTSTR*);
	//##ModelId=3A9E4BC6032B
	BOOL GetRemotefilename(LPTSTR*);
	//##ModelId=3A9E4BC60329
	BOOL GetSVIMstate(ULONG * Data);
	//##ModelId=3A9E4BC60325
	BOOL GetBinData(BYTE **pBuf, ULONG * FileLength);

protected:
	//##ModelId=3A9E4BC60322
	Element ** m_pResultsDataElements;
	//##ModelId=3A9E4BC6031F
	int m_ResultsDataLength;
};

#endif // !defined(AFX_SVXMLCMD_H__26B5F1B3_FE83_11D4_BDC4_0050B71464A5__INCLUDED_)

