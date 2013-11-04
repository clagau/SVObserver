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

#ifndef SVXMLEXCEPTION_H
#define SVXMLEXCEPTION_H

#include "SVStatusLibrary/SVException.h"
#include "SVXml.h"

class SVXmlException : public SVXml  
{
public:
	SVXmlException();
	virtual ~SVXmlException();
	BOOL operator=( SVException& rhs);
	SVException * CreateSVExceptionObject(BSTR * bstrDoc);
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
	SVException m_SvException;
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

#endif

// ******************************************************************************
// * LOG HISTORY:
// ******************************************************************************
/*
$Log:   N:\PVCSarch65\ProjectFiles\archives\SVObserver_src\SVCmnLib\SVXmlException.h_v  $
 * 
 *    Rev 1.0   18 Apr 2013 18:10:12   bWalter
 * Project:  SVObserver
 * Change Request (SCR) nbr:  814
 * SCR Title:  Upgrade SVObserver to Compile Using Visual Studio 2010
 * Checked in by:  bWalter;  Ben Walter
 * Change Description:  
 *   Initial check in to SVObserver_src.  (Merged with svo_src label SVO 6.10 Beta 008.)
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.1   02 Sep 2009 16:11:00   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  665
 * SCR Title:  Fix unrecoverable failure when processing acquisitions at high speed
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated code to fix include issues.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
*/
