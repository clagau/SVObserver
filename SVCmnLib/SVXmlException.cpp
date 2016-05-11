// ******************************************************************************
// * COPYRIGHT (c) 2007 by SVResearch, Harrisburg
// * All Rights Reserved
// ******************************************************************************
// * .Module Name     : SVXmlException
// * .File Name       : $Workfile:   SVXmlException.cpp  $
// * ----------------------------------------------------------------------------
// * .Current Version : $Revision:   1.0  $
// * .Check In Date   : $Date:   18 Apr 2013 18:10:10  $
// ******************************************************************************

// SVXmlException.cpp: implementation of the SVXmlException class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "SVXmlException.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

TCHAR g_szExceptionSchema[] =
_T("<Schema name=\"SVExceptionSchema\"  \n")
_T("xmlns=\"urn:schemas-microsoft-com:xml-data\"  \n")
_T("xmlns:dt=\"urn:schemas-microsoft-com:datatypes\" > \n")
_T("<AttributeType name=\"ProgramCode\"    dt:type=\"ui4\" /> \n")
_T("<AttributeType name=\"ErrorCode\"      dt:type=\"ui4\" /> \n")
_T("<AttributeType name=\"OsErrorCode\"    dt:type=\"ui4\" /> \n")
_T("<AttributeType name=\"ErrorData\"      dt:type=\"string\" /> \n")
_T("<AttributeType name=\"SourceFile\"     dt:type=\"string\" /> \n")
_T("<AttributeType name=\"SourceLine\"     dt:type=\"i4\" /> \n")
_T("<AttributeType name=\"SourceDateTime\" dt:type=\"string\" /> \n")
_T("<AttributeType name=\"CompileDate\"    dt:type=\"string\" /> \n")
_T("<AttributeType name=\"CompileTime\"    dt:type=\"string\" /> \n")
_T("<AttributeType name=\"Logged\"         dt:type=\"boolean\" /> \n")
_T("<ElementType name=\"SVException\" content=\"empty\" model=\"closed\" > \n")
_T("<attribute type=\"ProgramCode\"/> \n")
_T("<attribute type=\"ErrorCode\"/> \n")
_T("<attribute type=\"OsErrorCode\"/> \n")
_T("<attribute type=\"ErrorData\"/> \n")
_T("<attribute type=\"SourceFile\"/> \n")
_T("<attribute type=\"SourceLine\"/> \n")
_T("<attribute type=\"SourceDateTime\"/> \n")
_T("<attribute type=\"CompileDate\"/> \n")
_T("<attribute type=\"CompileTime\"/> \n")
_T("<attribute type=\"Logged\"/> \n")
_T("</ElementType>      \n")
"</Schema>";

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

SVXmlException::SVXmlException()
{
}

SVXmlException::~SVXmlException()
{

}

BOOL SVXmlException::InitXml()
{
	Reset();
	SetSchemaFilename(CString(""));
	if( !SVXml::InitXml( g_szExceptionSchema ) )return FALSE;
	return TRUE;
}

BOOL SVXmlException::InitXml(LPCTSTR szSchemaFileName)
{
	Reset();
	SetSchemaFilename(CString(szSchemaFileName));
	if (!SVXml::InitXml(nullptr)) return false;
	return TRUE;
}


BOOL  SVXmlException::SetProgramCode(ULONG Data)
{
	Element * pRootElement = nullptr;
	Attribute * pAttribute = nullptr;

	pRootElement = GetTreeRoot();
	if (nullptr == pRootElement) return false;
	//find the Attribute
	pAttribute = pRootElement->GetChildAttribute(_T("ProgramCode"));
	if (nullptr == pAttribute) return false;
	//set the data
	return (pAttribute->SetData(Data));
}


BOOL  SVXmlException::SetErrorCode(ULONG Data)
{
	Element * pRootElement = nullptr;
	Attribute * pAttribute = nullptr;

	pRootElement = GetTreeRoot();
	if (nullptr == pRootElement) return false;
	//find the Attribute
	pAttribute = pRootElement->GetChildAttribute(_T("ErrorCode"));
	if (nullptr == pAttribute) return false;
	//set the data
	return (pAttribute->SetData(Data));
}

BOOL  SVXmlException::SetOsErrorCode(ULONG Data)
{
	Element * pRootElement = nullptr;
	Attribute * pAttribute = nullptr;

	pRootElement = GetTreeRoot();
	if (nullptr == pRootElement)return false;
	//find the Attribute
	pAttribute = pRootElement->GetChildAttribute(_T("OsErrorCode"));
	if (nullptr == pAttribute)return false;
	//set the data
	return (pAttribute->SetData(Data));
}

BOOL  SVXmlException::SetSourceLine(LONG Data)
{
	Element * pRootElement = nullptr;
	Attribute * pAttribute = nullptr;

	pRootElement = GetTreeRoot();
	if (nullptr == pRootElement)return false;
	//find the Attribute
	pAttribute = pRootElement->GetChildAttribute(_T("SourceLine"));
	if (nullptr == pAttribute)return false;
	//set the data
	return (pAttribute->SetData(Data));
}

BOOL  SVXmlException::SetLogged(BOOL Data)
{
	Element * pRootElement = nullptr;
	Attribute * pAttribute = nullptr;

	pRootElement = GetTreeRoot();
	if (nullptr == pRootElement) return false;
	//find the Attribute
	pAttribute = pRootElement->GetChildAttribute(_T("Logged"));
	if (nullptr == pAttribute) return false;
	//set the data
	return (pAttribute->SetData(Data));
}

BOOL  SVXmlException::SetErrorData(BSTR * Data)
{
	Element * pRootElement = nullptr;
	Attribute * pAttribute = nullptr;

	pRootElement = GetTreeRoot();
	if (nullptr == pRootElement) return false;
	//find the Attribute
	pAttribute = pRootElement->GetChildAttribute(_T("ErrorData"));
	if (nullptr == pAttribute) return false;
	//set the data
	return (pAttribute->SetData(Data));
}

BOOL  SVXmlException::SetSourceFile(BSTR * Data)
{
	Element * pRootElement = nullptr;
	Attribute * pAttribute = nullptr;

	pRootElement = GetTreeRoot();
	if (nullptr == pRootElement)return false;
	//find the Attribute
	pAttribute = pRootElement->GetChildAttribute(_T("SourceFile"));
	if (nullptr == pAttribute)return false;
	//set the data
	return (pAttribute->SetData(Data));
}

BOOL  SVXmlException::SetSourceDateTime(BSTR * Data)
{
	Element * pRootElement = nullptr;
	Attribute * pAttribute = nullptr;

	pRootElement = GetTreeRoot();
	if (nullptr == pRootElement) return false;
	//find the Attribute
	pAttribute = pRootElement->GetChildAttribute(_T("SourceDateTime"));
	if (nullptr == pAttribute) return false;
	//set the data
	return (pAttribute->SetData(Data));
}

BOOL  SVXmlException::SetCompileDate(BSTR * Data)
{
	Element * pRootElement = nullptr;
	Attribute * pAttribute = nullptr;

	pRootElement = GetTreeRoot();
	if (nullptr == pRootElement) return false;
	//find the Attribute
	pAttribute = pRootElement->GetChildAttribute(_T("CompileDate"));
	if (nullptr == pAttribute) return false;
	//set the data
	return (pAttribute->SetData(Data));
}

BOOL  SVXmlException::SetCompileTime(BSTR * Data)
{
	Element * pRootElement = nullptr;
	Attribute * pAttribute = nullptr;

	pRootElement = GetTreeRoot();
	if(nullptr == pRootElement) return false;
	//find the Attribute
	pAttribute = pRootElement->GetChildAttribute(_T("CompileTime"));
	if(nullptr == pAttribute) return false;
	//set the data
	return (pAttribute->SetData(Data));
}


BOOL SVXmlException::GetXmlDoc(BSTR * bstrDoc)
{
	if(!CreateXmlDocFromTreeObj())
		{
		*bstrDoc = SysAllocStringByteLen (nullptr, 0 );
		return false;
		}
	if(!SVXml::GetXmlDoc(bstrDoc))return false;
	return true;
}


BOOL SVXmlException::operator=( SvStl::MessageContainer& rhs)
{
	if(!InitXml())
	{
		return false;
	}
	const SvStl::MessageData& rMsg( rhs.getMessage() );

	//set the program code
	if(!SetProgramCode( rMsg.m_ProgramCode ))return false;
	//set the error code
	if(!SetErrorCode( rMsg.m_MessageCode ))return false;
	//set the OS error code
	if(!SetOsErrorCode( rMsg.m_OSErrorCode ))return false;
	//set the error data
	if(!SetErrorData( _bstr_t(rMsg.getAdditionalText().c_str()).GetAddress() ))return false;
	//set the source file 
	if(!SetSourceFile( _bstr_t(rMsg.m_SourceFile.c_str()).GetAddress() ))return false;
	//set the source line
	if(!SetSourceLine( rMsg.m_SourceLine ))return false;
	//set the source date & time
	if(!SetSourceDateTime( _bstr_t(rMsg.m_SourceDateTime.c_str()).GetAddress() ))return false;
	//set the compile date
	if(!SetCompileDate( _bstr_t(rMsg.m_CompileDate.c_str()).GetAddress() ))return false;
	//set the compile time
	if(!SetCompileTime( _bstr_t(rMsg.m_CompileTime.c_str()).GetAddress() ))return false;
	//set logged flag
	if(!SetLogged( rMsg.m_Logged ) )return false;

	return true;
}

BOOL  SVXmlException::GetProgramCode(ULONG * Data)
{
	Element * pRootElement = nullptr;
	Attribute * pAttribute = nullptr;

	pRootElement = GetTreeRoot();
	if (nullptr == pRootElement) return false;
	//find the Attribute
	pAttribute = pRootElement->GetChildAttribute(_T("ProgramCode"));
	if (nullptr == pAttribute) return false;
	//get the data
	(pAttribute->GetData(Data));
	return true;
}

BOOL  SVXmlException::GetErrorCode(ULONG * Data)
{
	Element * pRootElement = nullptr;
	Attribute * pAttribute = nullptr;

	pRootElement = GetTreeRoot();
	if (nullptr == pRootElement) return false;
	//find the Attribute
	pAttribute = pRootElement->GetChildAttribute(_T("ErrorCode"));
	if (nullptr == pAttribute) return false;
	//get the data
	(pAttribute->GetData(Data));
	return true;
}

BOOL  SVXmlException::GetOsErrorCode(ULONG * Data)
{
	Element * pRootElement = nullptr;
	Attribute * pAttribute = nullptr;

	pRootElement = GetTreeRoot();
	if(nullptr == pRootElement) return false;
	//find the Attribute
	pAttribute = pRootElement->GetChildAttribute(_T("OsErrorCode"));
	if(nullptr == pAttribute) return false;
	//get the data
	(pAttribute->GetData(Data));
	return true;
}

BOOL  SVXmlException::GetSourceLine(LONG * Data)
{
	Element * pRootElement = nullptr;
	Attribute * pAttribute = nullptr;

	pRootElement = GetTreeRoot();
	if(nullptr == pRootElement) return false;
	//find the Attribute
	pAttribute = pRootElement->GetChildAttribute(_T("SourceLine"));
	if(nullptr == pAttribute) return false;
	//get the data
	(pAttribute->GetData(Data));
	return true;
}

BOOL  SVXmlException::GetLogged(BOOL * Data)
{
	Element * pRootElement = nullptr;
	Attribute * pAttribute = nullptr;

	pRootElement = GetTreeRoot();
	if (nullptr == pRootElement) return false;
	//find the Attribute
	pAttribute = pRootElement->GetChildAttribute(_T("Logged"));
	if (nullptr == pAttribute) return false;
	//get the data
	(pAttribute->GetData(Data));
	return true;
}

BOOL  SVXmlException::GetErrorData(BSTR * Data)
{
	Element * pRootElement = nullptr;
	Attribute * pAttribute = nullptr;

	pRootElement = GetTreeRoot();
	if (nullptr == pRootElement) return false;
	//find the Attribute
	pAttribute = pRootElement->GetChildAttribute(_T("ErrorData"));
	if (nullptr == pAttribute) return false;
	//get the data
	(pAttribute->GetData(Data));
	return true;
}

BOOL  SVXmlException::GetSourceFile(BSTR * Data)
{
	Element * pRootElement = nullptr;
	Attribute * pAttribute = nullptr;

	pRootElement = GetTreeRoot();
	if (nullptr == pRootElement) return false;
	//find the Attribute
	pAttribute = pRootElement->GetChildAttribute(_T("SourceFile"));
	if (nullptr == pAttribute) return false;
	//get the data
	(pAttribute->GetData(Data));
	return true;
}

BOOL  SVXmlException::GetSourceDateTime(BSTR * Data)
{
	Element * pRootElement = nullptr;
	Attribute * pAttribute = nullptr;

	pRootElement = GetTreeRoot();
	if (nullptr == pRootElement) return false;
	//find the Attribute
	pAttribute = pRootElement->GetChildAttribute(_T("SourceDateTime"));
	if (nullptr == pAttribute) return false;
	//get the data
	(pAttribute->GetData(Data));
	return true;
}

BOOL  SVXmlException::GetCompileDate(BSTR * Data)
{
	Element * pRootElement = nullptr;
	Attribute * pAttribute = nullptr;

	pRootElement = GetTreeRoot();
	if (nullptr == pRootElement) return false;
	//find the Attribute
	pAttribute = pRootElement->GetChildAttribute(_T("CompileDate"));
	if (nullptr == pAttribute) return false;
	//get the data
	(pAttribute->GetData(Data));
	return true;
}

BOOL  SVXmlException::GetCompileTime(BSTR * Data)
{
	Element * pRootElement = nullptr;
	Attribute * pAttribute = nullptr;

	pRootElement = GetTreeRoot();
	if (nullptr == pRootElement) return false;
	//find the Attribute
	pAttribute = pRootElement->GetChildAttribute(_T("CompileTime"));
	if (nullptr == pAttribute) return false;
	//get the data
	(pAttribute->GetData(Data));
	return true;
}

