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

#pragma once

#pragma region Includes
#include "SVXml.h"
#pragma endregion Includes

class SVXmlCmd : public SVXml  
{
public:
	BOOL GetXmlDoc(BSTR * bstrDoc);
	BOOL GetResultsData(int index,CString szIPD, CString& szFullname, CString& szValue, CString& szType);
	BOOL LoadDoc(BSTR *);
	long EnumerateResultsData(LPTSTR);

	SVXmlCmd();
	virtual ~SVXmlCmd();

	BOOL InitXml();
	BOOL InitXml(LPCTSTR szSchemaFileName);
	BOOL SetCommand(LPCTSTR Data);
	BOOL SetSVIMfilename(LPCTSTR Data);
	BOOL SetRemotefilename(LPCTSTR Data);
	BOOL SetSVIMstate(ULONG Data);
	BOOL SetBinData(BYTE *pBuf, ULONG FileLength);
	BOOL SetResultsData(CString szIPD, CString szFullname, CString szValue, CString szType);
	
	BOOL ClearResultsData();

	BOOL GetCommand(LPTSTR*);
	BOOL GetSVIMfilename(LPTSTR*);
	BOOL GetRemotefilename(LPTSTR*);
	BOOL GetSVIMstate(ULONG * Data);
	BOOL GetBinData(BYTE **pBuf, ULONG * FileLength);

protected:
	Element ** m_pResultsDataElements;
	int m_ResultsDataLength;
};
