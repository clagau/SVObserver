// ******************************************************************************
// * COPYRIGHT (c) 2005 by SVResearch, Harrisburg
// * All Rights Reserved
// ******************************************************************************
// * .Module Name     : SVOIntelRAIDStatusClass
// * .File Name       : $Workfile:   SVOIntelRAIDStatusClass.h  $
// * ----------------------------------------------------------------------------
// * .Current Version : $Revision:   1.0  $
// * .Check In Date   : $Date:   23 Apr 2013 13:02:38  $
// ******************************************************************************

#ifndef _SVOINTELRAIDSTATUSCLASS_H
#define _SVOINTELRAIDSTATUSCLASS_H

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

class SVOIntelRAIDStatusClass  
{
public:
	SVOIntelRAIDStatusClass();
	virtual ~SVOIntelRAIDStatusClass();

	HRESULT UpdateStatus();
	HRESULT CheckStatus();

	const CString& GetRaidStatus();
	const CString& GetErrorStatus();

	const HANDLE GetCheckEvent();

protected:

	HANDLE m_hCheckEvent;

	CString m_csRaidStatus;
	CString m_csErrorStatus;

};

#endif // _SVOINTELRAIDSTATUSCLASS_H

