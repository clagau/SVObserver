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
	if(!SVXml::InitXml(NULL))return FALSE;
	return TRUE;
}


BOOL  SVXmlException::SetProgramCode(ULONG Data)
{
	Element * pRootElement = NULL;
	Attribute * pAttribute = NULL;

	pRootElement = GetTreeRoot();
	if(pRootElement == NULL)return FALSE;
	//find the Attribute
	pAttribute = pRootElement->GetChildAttribute(_T("ProgramCode"));
	if(pAttribute == NULL)return FALSE;
	//set the data
	return (pAttribute->SetData(Data));
}


BOOL  SVXmlException::SetErrorCode(ULONG Data)
{
	Element * pRootElement = NULL;
	Attribute * pAttribute = NULL;

	pRootElement = GetTreeRoot();
	if(pRootElement == NULL)return FALSE;
	//find the Attribute
	pAttribute = pRootElement->GetChildAttribute(_T("ErrorCode"));
	if(pAttribute == NULL)return FALSE;
	//set the data
	return (pAttribute->SetData(Data));
}

BOOL  SVXmlException::SetOsErrorCode(ULONG Data)
{
	Element * pRootElement = NULL;
	Attribute * pAttribute = NULL;

	pRootElement = GetTreeRoot();
	if(pRootElement == NULL)return FALSE;
	//find the Attribute
	pAttribute = pRootElement->GetChildAttribute(_T("OsErrorCode"));
	if(pAttribute == NULL)return FALSE;
	//set the data
	return (pAttribute->SetData(Data));
}

BOOL  SVXmlException::SetSourceLine(LONG Data)
{
	Element * pRootElement = NULL;
	Attribute * pAttribute = NULL;

	pRootElement = GetTreeRoot();
	if(pRootElement == NULL)return FALSE;
	//find the Attribute
	pAttribute = pRootElement->GetChildAttribute(_T("SourceLine"));
	if(pAttribute == NULL)return FALSE;
	//set the data
	return (pAttribute->SetData(Data));
}

BOOL  SVXmlException::SetLogged(BOOL Data)
{
	Element * pRootElement = NULL;
	Attribute * pAttribute = NULL;

	pRootElement = GetTreeRoot();
	if(pRootElement == NULL)return FALSE;
	//find the Attribute
	pAttribute = pRootElement->GetChildAttribute(_T("Logged"));
	if(pAttribute == NULL)return FALSE;
	//set the data
	return (pAttribute->SetData(Data));
}

BOOL  SVXmlException::SetErrorData(BSTR * Data)
{
	Element * pRootElement = NULL;
	Attribute * pAttribute = NULL;

	pRootElement = GetTreeRoot();
	if(pRootElement == NULL)return FALSE;
	//find the Attribute
	pAttribute = pRootElement->GetChildAttribute(_T("ErrorData"));
	if(pAttribute == NULL)return FALSE;
	//set the data
	return (pAttribute->SetData(Data));
}

BOOL  SVXmlException::SetSourceFile(BSTR * Data)
{
	Element * pRootElement = NULL;
	Attribute * pAttribute = NULL;

	pRootElement = GetTreeRoot();
	if(pRootElement == NULL)return FALSE;
	//find the Attribute
	pAttribute = pRootElement->GetChildAttribute(_T("SourceFile"));
	if(pAttribute == NULL)return FALSE;
	//set the data
	return (pAttribute->SetData(Data));
}

BOOL  SVXmlException::SetSourceDateTime(BSTR * Data)
{
	Element * pRootElement = NULL;
	Attribute * pAttribute = NULL;

	pRootElement = GetTreeRoot();
	if(pRootElement == NULL)return FALSE;
	//find the Attribute
	pAttribute = pRootElement->GetChildAttribute(_T("SourceDateTime"));
	if(pAttribute == NULL)return FALSE;
	//set the data
	return (pAttribute->SetData(Data));
}

BOOL  SVXmlException::SetCompileDate(BSTR * Data)
{
	Element * pRootElement = NULL;
	Attribute * pAttribute = NULL;

	pRootElement = GetTreeRoot();
	if(pRootElement == NULL)return FALSE;
	//find the Attribute
	pAttribute = pRootElement->GetChildAttribute(_T("CompileDate"));
	if(pAttribute == NULL)return FALSE;
	//set the data
	return (pAttribute->SetData(Data));
}

BOOL  SVXmlException::SetCompileTime(BSTR * Data)
{
	Element * pRootElement = NULL;
	Attribute * pAttribute = NULL;

	pRootElement = GetTreeRoot();
	if(pRootElement == NULL)return FALSE;
	//find the Attribute
	pAttribute = pRootElement->GetChildAttribute(_T("CompileTime"));
	if(pAttribute == NULL)return FALSE;
	//set the data
	return (pAttribute->SetData(Data));
}


BOOL SVXmlException::GetXmlDoc(BSTR * bstrDoc)
{
	if(!CreateXmlDocFromTreeObj())
		{
		*bstrDoc = SysAllocStringByteLen (NULL, 0 );
		return FALSE;
		}
	if(!SVXml::GetXmlDoc(bstrDoc))return FALSE;
	return TRUE;
}


BOOL SVXmlException::operator=( SVException& rhs)
{
	BSTR bstrErrorData,bstrSourceFile,bstrSourceDateTime,bstrCompileDate,bstrCompileTime;

	if(!InitXml())
		{
		return FALSE;
		}
	//set the program code
	if(!SetProgramCode(rhs.GetProgramCode()))return FALSE;
	//set the error code
	if(!SetErrorCode(rhs.GetErrorCode()))return FALSE;
	//set the OS error code
	if(!SetOsErrorCode(rhs.GetOSErrorCode()))return FALSE;
	//set the error data
	rhs.GetErrorData(&bstrErrorData);
	if(!SetErrorData(&bstrErrorData))return FALSE;
	//set the source file 
	rhs.GetSourceFile(&bstrSourceFile);
	if(!SetSourceFile(&bstrSourceFile))return FALSE;
	//set the source line
	if(!SetSourceLine(rhs.GetSourceLine()))return FALSE;
	//set the source date & time
	rhs.GetSourceDateTime(&bstrSourceDateTime);
	if(!SetSourceDateTime(&bstrSourceDateTime))return FALSE;
	//set the compile date
	rhs.GetCompileDate(&bstrCompileDate);
	if(!SetCompileDate(&bstrCompileDate))return FALSE;
	//set the compile time
	rhs.GetCompileTime(&bstrCompileTime);
	if(!SetCompileTime(&bstrCompileTime))return FALSE;
	//set logged flag
	if(!SetLogged(rhs.GetLoggedStatus()))return FALSE;

	return TRUE;
}


#pragma warning ( disable : 4102 )	// unreferenced lable warning (CleanUp)
SVException * SVXmlException::CreateSVExceptionObject(BSTR * bstrDoc)
{
USES_CONVERSION;
	ULONG ErrorCode = 0;
	BSTR bstrCompileDate;
	BSTR bstrCompileTime;
	BSTR bstrErrorData;
	UINT cbErrorData;
	BSTR bstrSourceFile;
	BSTR bstrSourceDateTime;
	long SourceLine;
	ULONG ProgramCode;
	ULONG OsErrorCode;
	ULONG * pPtr = NULL;

	m_SvException.ResetException();

	if(!InitXml())
		{
		return NULL;
		}

	if(!CreateTreeObjFromXmlDoc(bstrDoc))return NULL;
	
	if(!GetErrorCode(&ErrorCode))return NULL;
	if(!GetCompileDate(&bstrCompileDate))return NULL;
	if(!GetCompileTime(&bstrCompileTime))return NULL;
	if(!GetErrorData(&bstrErrorData))return NULL;
	if(!GetSourceFile(&bstrSourceFile))return NULL;
	if(!GetSourceLine(&SourceLine))return NULL;
	if(!GetSourceDateTime(&bstrSourceDateTime))return NULL;
	if(!GetProgramCode(&ProgramCode))return NULL;
	if(!GetOsErrorCode(&OsErrorCode))return NULL;

	pPtr = (ULONG*)bstrErrorData;
	pPtr--;
	cbErrorData = (UINT)*pPtr;

	m_SvException.SetException(ErrorCode,  W2T(bstrCompileDate),  W2T(bstrCompileTime), (LPVOID)bstrErrorData, cbErrorData, 
							    W2T(bstrSourceFile), SourceLine,  W2T(bstrSourceDateTime) , ProgramCode,OsErrorCode);

CleanUp:
	return &m_SvException;
}




BOOL  SVXmlException::GetProgramCode(ULONG * Data)
{
	Element * pRootElement = NULL;
	Attribute * pAttribute = NULL;

	pRootElement = GetTreeRoot();
	if(pRootElement == NULL)return FALSE;
	//find the Attribute
	pAttribute = pRootElement->GetChildAttribute(_T("ProgramCode"));
	if(pAttribute == NULL)return FALSE;
	//get the data
	(pAttribute->GetData(Data));
	return TRUE;
}


BOOL  SVXmlException::GetErrorCode(ULONG * Data)
{
	Element * pRootElement = NULL;
	Attribute * pAttribute = NULL;

	pRootElement = GetTreeRoot();
	if(pRootElement == NULL)return FALSE;
	//find the Attribute
	pAttribute = pRootElement->GetChildAttribute(_T("ErrorCode"));
	if(pAttribute == NULL)return FALSE;
	//get the data
	(pAttribute->GetData(Data));
	return TRUE;
}

BOOL  SVXmlException::GetOsErrorCode(ULONG * Data)
{
	Element * pRootElement = NULL;
	Attribute * pAttribute = NULL;

	pRootElement = GetTreeRoot();
	if(pRootElement == NULL)return FALSE;
	//find the Attribute
	pAttribute = pRootElement->GetChildAttribute(_T("OsErrorCode"));
	if(pAttribute == NULL)return FALSE;
	//get the data
	(pAttribute->GetData(Data));
	return TRUE;
}

BOOL  SVXmlException::GetSourceLine(LONG * Data)
{
	Element * pRootElement = NULL;
	Attribute * pAttribute = NULL;

	pRootElement = GetTreeRoot();
	if(pRootElement == NULL)return FALSE;
	//find the Attribute
	pAttribute = pRootElement->GetChildAttribute(_T("SourceLine"));
	if(pAttribute == NULL)return FALSE;
	//get the data
	(pAttribute->GetData(Data));
	return TRUE;
}

BOOL  SVXmlException::GetLogged(BOOL * Data)
{
	Element * pRootElement = NULL;
	Attribute * pAttribute = NULL;

	pRootElement = GetTreeRoot();
	if(pRootElement == NULL)return FALSE;
	//find the Attribute
	pAttribute = pRootElement->GetChildAttribute(_T("Logged"));
	if(pAttribute == NULL)return FALSE;
	//get the data
	(pAttribute->GetData(Data));
	return TRUE;
}

BOOL  SVXmlException::GetErrorData(BSTR * Data)
{
	Element * pRootElement = NULL;
	Attribute * pAttribute = NULL;

	pRootElement = GetTreeRoot();
	if(pRootElement == NULL)return FALSE;
	//find the Attribute
	pAttribute = pRootElement->GetChildAttribute(_T("ErrorData"));
	if(pAttribute == NULL)return FALSE;
	//get the data
	(pAttribute->GetData(Data));
	return TRUE;
}

BOOL  SVXmlException::GetSourceFile(BSTR * Data)
{
	Element * pRootElement = NULL;
	Attribute * pAttribute = NULL;

	pRootElement = GetTreeRoot();
	if(pRootElement == NULL)return FALSE;
	//find the Attribute
	pAttribute = pRootElement->GetChildAttribute(_T("SourceFile"));
	if(pAttribute == NULL)return FALSE;
	//get the data
	(pAttribute->GetData(Data));
	return TRUE;
}

BOOL  SVXmlException::GetSourceDateTime(BSTR * Data)
{
	Element * pRootElement = NULL;
	Attribute * pAttribute = NULL;

	pRootElement = GetTreeRoot();
	if(pRootElement == NULL)return FALSE;
	//find the Attribute
	pAttribute = pRootElement->GetChildAttribute(_T("SourceDateTime"));
	if(pAttribute == NULL)return FALSE;
	//get the data
	(pAttribute->GetData(Data));
	return TRUE;
}

BOOL  SVXmlException::GetCompileDate(BSTR * Data)
{
	Element * pRootElement = NULL;
	Attribute * pAttribute = NULL;

	pRootElement = GetTreeRoot();
	if(pRootElement == NULL)return FALSE;
	//find the Attribute
	pAttribute = pRootElement->GetChildAttribute(_T("CompileDate"));
	if(pAttribute == NULL)return FALSE;
	//get the data
	(pAttribute->GetData(Data));
	return TRUE;
}

BOOL  SVXmlException::GetCompileTime(BSTR * Data)
{
	Element * pRootElement = NULL;
	Attribute * pAttribute = NULL;

	pRootElement = GetTreeRoot();
	if(pRootElement == NULL)return FALSE;
	//find the Attribute
	pAttribute = pRootElement->GetChildAttribute(_T("CompileTime"));
	if(pAttribute == NULL)return FALSE;
	//get the data
	(pAttribute->GetData(Data));
	return TRUE;
}

// ******************************************************************************
// * LOG HISTORY:
// ******************************************************************************
/*
$Log:   N:\PVCSarch65\ProjectFiles\archives\SVObserver_src\SVCmnLib\SVXmlException.cpp_v  $
 * 
 *    Rev 1.0   18 Apr 2013 18:10:10   bWalter
 * Project:  SVObserver
 * Change Request (SCR) nbr:  814
 * SCR Title:  Upgrade SVObserver to Compile Using Visual Studio 2010
 * Checked in by:  bWalter;  Ben Walter
 * Change Description:  
 *   Initial check in to SVObserver_src.  (Merged with svo_src label SVO 6.10 Beta 008.)
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.2.1.0   22 Oct 2010 08:00:50   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  704
 * SCR Title:  Upgrade SVObserver Version for 5.33 Release
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated source code to fix include problems and compilier efficiency.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.2   19 Jun 2007 12:48:10   Joe
 * Project:  SVObserver
 * Change Request (SCR) nbr:  598
 * SCR Title:  Upgrade SVObserver to compile using vc++ in VS2005
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   These changes include modification based on fixing compiler-based and project-based differences between VC6 and VC8.  These changes mainly include casting issues, but some include type conversion and assignment of new compiler controlling defines.
 * 
 * Other source files were removed from the project because they are not used as part of the SVObserver project.
 * 
 * 
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
*/