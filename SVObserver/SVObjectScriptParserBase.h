//******************************************************************************
//* COPYRIGHT (c) 2012 by Seidenader Vision, Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVObjectScriptParserBase
//* .File Name       : $Workfile:   SVObjectScriptParserBase.h  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.0  $
//* .Check In Date   : $Date:   23 Apr 2013 12:41:08  $
//******************************************************************************

#ifndef INCL_SVOBJECTSCRIPTPARSERBASE_H
#define INCL_SVOBJECTSCRIPTPARSERBASE_H

#include "SVObjectLibrary/SVObjectClass.h"

// base class to parser implementations
class SVObjectScriptParserBase
{
public:
	SVObjectScriptParserBase(unsigned long parserHandle, const GUID& OwnerGuid, SVObjectClass* pOwnerObject, CWnd* pWnd);
	
	virtual ~SVObjectScriptParserBase();
	
	const GUID& GetOwnerGuid() const;
	SVObjectClass* GetOwnerObject();

	virtual size_t GetTotal() const = 0;
	virtual HRESULT DoParse()=0;

	void Complete();
	void UpdateProgress( size_t amtProcessed, size_t amtTotal );

protected:
	GUID m_OwnerGuid;
	SVObjectClass* m_pOwnerObject;
	CWnd* m_pWnd;

	unsigned long m_parserHandle;
};

#endif

//******************************************************************************
//* LOG HISTORY:
//******************************************************************************
/*
$Log:   N:\PVCSarch65\ProjectFiles\archives\SVObserver_src\SVObserver\SVObjectScriptParserBase.h_v  $
 * 
 *    Rev 1.0   23 Apr 2013 12:41:08   bWalter
 * Project:  SVObserver
 * Change Request (SCR) nbr:  814
 * SCR Title:  Upgrade SVObserver to Compile Using Visual Studio 2010
 * Checked in by:  bWalter;  Ben Walter
 * Change Description:  
 *   Initial check in to SVObserver_src.  (Merged with svo_src label SVO 6.10 Beta 008.)
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.0   04 Sep 2012 14:00:56   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  602
 * SCR Title:  Revise the Toolset Parsing and Object Creation Methodology
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Initial check-in of new files.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
*/
