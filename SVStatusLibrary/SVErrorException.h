//******************************************************************************
//* COPYRIGHT (c) 2004 by SVResearch, Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVErrorException
//* .File Name       : $Workfile:   SVErrorException.h  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.0  $
//* .Check In Date   : $Date:   25 Apr 2013 17:42:02  $
//******************************************************************************

#ifndef SVERROREXCEPTION_H
#define SVERROREXCEPTION_H

#include <list>
#include "SVUtilityLibrary/SVString.h"

struct SVErrorInfo
{
	SVErrorInfo() {hr=S_OK; lErrorCode = 0;}
	SVErrorInfo(HRESULT p_hr, long p_lErrorCode=0, const SVString& p_rsErrorMessage=SVString(_T("")))
	{
		hr = p_hr;
		lErrorCode = p_lErrorCode;
		sErrorMessage = p_rsErrorMessage;
	}

	HRESULT hr;
	long lErrorCode;
	SVString sErrorMessage;
};

class SVErrorException : public std::exception
{
public:
	typedef std::list<SVErrorInfo> SubinfoType;

	SVErrorInfo& info() {return m_info;}
	const SubinfoType& subinfo() {return m_subinfo;}

	virtual const char* what() const {return m_info.sErrorMessage.ToString();}
	void Add(SVErrorInfo& e, HRESULT hr=E_FAIL)
	{
		m_subinfo.push_back(e);
		if ( m_info.hr == S_OK )
			m_info.hr = hr;
	}

protected:
	SVErrorInfo m_info;
	SubinfoType m_subinfo;
};

#endif

//******************************************************************************
//* LOG HISTORY:
//******************************************************************************
/*
$Log:   N:\PVCSarch65\ProjectFiles\archives\SVObserver_src\SVStatusLibrary\SVErrorException.h_v  $
 * 
 *    Rev 1.0   25 Apr 2013 17:42:02   bWalter
 * Project:  SVObserver
 * Change Request (SCR) nbr:  814
 * SCR Title:  Upgrade SVObserver to Compile Using Visual Studio 2010
 * Checked in by:  bWalter;  Ben Walter
 * Change Description:  
 *   Initial check in to SVObserver_src.  (Merged with svo_src label SVO 6.10 Beta 008.)
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.0   02 Sep 2009 15:41:38   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  665
 * SCR Title:  Fix unrecoverable failure when processing acquisitions at high speed
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Moved file to this library to allow for multiple project inclusion.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.0   08 Jul 2004 12:18:42   ebeyeler
 * Project:  SVObserver
 * Change Request (SCR) nbr:  406
 * SCR Title:  Implement External Tool
 * Checked in by:  eBeyeler;  Eric Beyeler
 * Change Description:  
 *   first iteration
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
*/
