//******************************************************************************
//* COPYRIGHT (c) 2003 by SVResearch, Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVObjectScriptParser
//* .File Name       : $Workfile:   SVObjectScriptParser.cpp  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.1  $
//* .Check In Date   : $Date:   01 Oct 2013 15:24:36  $
//******************************************************************************

#include "stdafx.h"
#include "SVObjectScriptParser.h"
#include "SVParserProgressStruct.h"
#include "SVUserMessage.h"

unsigned long SVObjectScriptParserClass::GetParserHandle()
{
	static unsigned long handle=0;
	return ++handle;
}

UINT SVObjectScriptParserClass::ParserThread( LPVOID lParam )
{
	SVObjectScriptParserClass* pParser = reinterpret_cast<SVObjectScriptParserClass*> (lParam);
	
	pParser->Parse(); 
	pParser->Complete();
	return 0;
}

SVObjectScriptParserClass::SVObjectScriptParserClass(SVObjectScriptParserBase* pParser)
: m_pParser(pParser)
{
}

SVObjectScriptParserClass::~SVObjectScriptParserClass()
{
	if (m_pParser)
	{
		delete m_pParser;
		m_pParser = NULL;
	}
}

const GUID& SVObjectScriptParserClass::GetOwnerGuid() const
{
	return m_pParser->GetOwnerGuid();
}

SVObjectClass* SVObjectScriptParserClass::GetOwnerObject()
{
	return m_pParser->GetOwnerObject();
}

unsigned long SVObjectScriptParserClass::GetTotal() const
{
	return static_cast<int>(m_pParser->GetTotal());
}

HRESULT SVObjectScriptParserClass::Parse()
{
	HRESULT hr = E_FAIL;
	if( m_pParser->GetOwnerObject() )
	{
		hr = m_pParser->DoParse();
	}
	return hr;
}

void SVObjectScriptParserClass::Complete()
{
	m_pParser->Complete();
}

//******************************************************************************
//* LOG HISTORY:
//******************************************************************************
/*
$Log:   N:\PVCSarch65\ProjectFiles\archives\SVObserver_SRC\SVObserver\SVObjectScriptParser.cpp_v  $
 * 
 *    Rev 1.1   01 Oct 2013 15:24:36   tbair
 * Project:  SVObserver
 * Change Request (SCR) nbr:  852
 * SCR Title:  Add Multiple Platform Support to SVObserver's Visual Studio Solution
 * Checked in by:  tBair;  Tom Bair
 * Change Description:  
 *   Add x64 platform.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.0   23 Apr 2013 12:40:36   bWalter
 * Project:  SVObserver
 * Change Request (SCR) nbr:  814
 * SCR Title:  Upgrade SVObserver to Compile Using Visual Studio 2010
 * Checked in by:  bWalter;  Ben Walter
 * Change Description:  
 *   Initial check in to SVObserver_src.  (Merged with svo_src label SVO 6.10 Beta 008.)
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
   
      Rev 3.14   04 Sep 2012 15:50:16   jspila
   Project:  SVObserver
   Change Request (SCR) nbr:  602
   SCR Title:  Revise the Toolset Parsing and Object Creation Methodology
   Checked in by:  Joe;  Joe Spila
   Change Description:  
     Updated source code to use new progress dialog functionality.
   
   /////////////////////////////////////////////////////////////////////////////////////
   
      Rev 3.13   08 Dec 2010 13:15:18   jspila
   Project:  SVObserver
   Change Request (SCR) nbr:  707
   SCR Title:  Change Inspection Display Functionality to Force Display of Last Inspected
   Checked in by:  Joe;  Joe Spila
   Change Description:  
     Updated source code to include changes in notification functionality using the Observer Design Pattern.
   
   /////////////////////////////////////////////////////////////////////////////////////
   
      Rev 3.12   06 Aug 2004 12:10:20   ebeyeler
   Project:  SVObserver
   Change Request (SCR) nbr:  428
   SCR Title:  Improve Load Configuration time
   Checked in by:  eBeyeler;  Eric Beyeler
   Change Description:  
     fixed incorrect hex representation of SVO 4.40.
   
   /////////////////////////////////////////////////////////////////////////////////////
   
      Rev 3.11   12 Jul 2004 12:37:00   ebeyeler
   Project:  SVObserver
   Change Request (SCR) nbr:  428
   SCR Title:  Improve Load Configuration time
   Checked in by:  eBeyeler;  Eric Beyeler
   Change Description:  
     implemented multi parser switching
   
   /////////////////////////////////////////////////////////////////////////////////////
   
      Rev 3.10   22 Apr 2003 11:49:34   rschock
   Project:  SVObserver
   Change Request (SCR) nbr:  346
   SCR Title:  Update SVObserver to Version 4.21 Release
   Checked in by:  Joe;  Joe Spila
   Change Description:  
     Redid the #include defines and standardized the Tracker log headers and removed warning from release mode builds.
   
   /////////////////////////////////////////////////////////////////////////////////////
   
      Rev 3.9   20 Nov 2002 09:40:18   ryoho
   Project:  SVObserver
   Change Request (SCR) nbr:  226
   SCR Title:  Monochrome SVIM configuration compatibility between ViperQUAD and ViperDUAL
   Checked in by:  rYoho;  Rob Yoho
   Change Description:  
     Changed TheObjectManager -> TheSVObjectManager
   Changed TheClassRegister -> TheSVClassRegister
   EvaluateOperandExpression
   
   
   /////////////////////////////////////////////////////////////////////////////////////
   
      Rev 3.8   08 Nov 2001 15:06:36   Joe
   Project:  SVObserver
   Change Request (SCR) nbr:  255
   SCR Title:  Add Sub-pixel resolution to Profile and Line Gage Tool Analyzers
   Checked in by:  Joe;  Joe Spila
   Change Description:  
     Updated constructor and ExactValue methods to load sub-pixel results.
   
   /////////////////////////////////////////////////////////////////////////////////////
*/

