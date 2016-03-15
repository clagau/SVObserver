//******************************************************************************
//* COPYRIGHT (c) 2009 by Seidenader Vision, Inc.
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVXmlException
//* .File Name       : $Workfile:   SVXmlException.h  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.0  $
//* .Check In Date   : $Date:   18 Apr 2013 18:10:12  $
//******************************************************************************

#pragma once

#pragma region Includes
#include "SVStatusLibrary\MessageContainer.h"
#include "SVXml.h"
#pragma endregion Includes

class SVXmlException : public SVXml  
{
public:
	SVXmlException();
	virtual ~SVXmlException();
	BOOL operator=( SvStl::MessageContainer& rhs);
	BOOL GetXmlDoc(BSTR * bstrDoc);

protected:
	BOOL InitXml();
	BOOL InitXml(LPCTSTR szSchemaFileName);
	BOOL SetProgramCode(ULONG);
	BOOL SetErrorCode(ULONG);
	BOOL SetOsErrorCode(ULONG);
	BOOL SetSourceLine(LONG);
	BOOL SetLogged(BOOL);
	BOOL SetErrorData(BSTR * Data);
	BOOL SetSourceFile(BSTR * Data);
	BOOL SetSourceDateTime(BSTR * Data);
	BOOL SetCompileDate(BSTR * Data);
	BOOL SetCompileTime(BSTR * Data);
	BOOL GetProgramCode(ULONG * Data);
	BOOL GetErrorCode(ULONG * Data);
	BOOL GetOsErrorCode(ULONG * Data);
	BOOL GetSourceLine(LONG * Data);
	BOOL GetLogged(BOOL * Data);
	BOOL GetErrorData(BSTR * Data);
	BOOL GetSourceFile(BSTR * Data);
	BOOL GetSourceDateTime(BSTR * Data);
	BOOL GetCompileDate(BSTR * Data);
	BOOL GetCompileTime(BSTR * Data);

};

