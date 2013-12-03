//******************************************************************************
//* COPYRIGHT (c) 2008 by SVResearch, Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVIOTabbedView
//* .File Name       : $Workfile:   SVIOTabbedView.cpp  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.3  $
//* .Check In Date   : $Date:   30 Oct 2013 10:45:20  $
//******************************************************************************

#include "stdafx.h"
#include "SVObserver.h"
#include "SVIOTabbedView.h"
#include "SVDiscreteOutputsView.h"
#include "SVDiscreteInputsView.h"
#ifndef _WIN64
#include "SVPLCOutputsView.h"
#endif
#include "SVRemoteInputsView.h"
#include "SVRemoteOutputsView.h"


// SVIOTabbedView

IMPLEMENT_DYNCREATE(SVIOTabbedView, CMDIChildWnd)

SVIOTabbedView::SVIOTabbedView()
{

}

SVIOTabbedView::~SVIOTabbedView()
{
}


BEGIN_MESSAGE_MAP(SVIOTabbedView, CMDIChildWnd)
	ON_WM_DESTROY()
END_MESSAGE_MAP()


BOOL SVIOTabbedView::OnCreateClient(LPCREATESTRUCT lpcs, CCreateContext* pContext)
{
	TVisualObject *pIOTab = new TVisualObject( SVIOViewID, _T("IO"), pContext, RUNTIME_CLASS(TTabWnd) ); //

	TVisualObject *pInputsView = new TVisualObject( SVIODiscreteInputsViewID, _T("Discrete Inputs"), pContext, RUNTIME_CLASS( SVDiscreteInputsView ) );
	TVisualObject *pRemoteInputsView = new TVisualObject( SVIORemoteInputsViewID, _T("Remote Inputs"), pContext, RUNTIME_CLASS( SVRemoteInputsView ) );

	TVisualObject *pOutputsView = new TVisualObject( SVIODiscreteOutputsViewID, _T("Discrete Outputs"), pContext, RUNTIME_CLASS( SVDiscreteOutputsView ) );
#ifndef _WIN64
	TVisualObject *pPLCOutputsView = new TVisualObject( SVIOPLCOutputsViewID, _T("PLC Outputs"), pContext, RUNTIME_CLASS( SVPLCOutputsView ) );
#endif
	TVisualObject *pRemoteOutputsView = new TVisualObject( SVRemoteOutputsViewID, _T("Remote Outputs"), pContext, RUNTIME_CLASS( SVRemoteOutputsView ) );


	m_Framework.Add( pIOTab );
	m_Framework.Add( pIOTab, pInputsView );
	m_Framework.Add( pIOTab, pRemoteInputsView );

	m_Framework.Add( pIOTab, pOutputsView );
#ifndef _WIN64
	m_Framework.Add( pIOTab, pPLCOutputsView );
#endif
	m_Framework.Add( pIOTab, pRemoteOutputsView );


	BOOL l_bRet = m_Framework.Create(this);

	//pRemoteInputsView->EnableTab(FALSE); // Grays out the tab so you cannot switch to 
	//pRemoteInputsView->Enable(FALSE);		// Grays out the entire tabview so you cannot change anything.
	;
	return l_bRet;
}

CWnd* SVIOTabbedView::GetActivePane(int* /*pRow*/, int* /*pCol*/)
{
	ASSERT_VALID(this);

	// attempt to use active view of frame window
	CWnd* pView = NULL;
	CFrameWnd* pFrameWnd = GetParentFrame();
	ASSERT_VALID(pFrameWnd);
	pView = pFrameWnd->GetActiveView();

	// failing that, use the current focus
	if (pView == NULL)
		pView = GetFocus();

	return pView;
}


// SVIOTabbedView message handlers
void SVIOTabbedView::OnDestroy()
{
	CMDIChildWnd::OnDestroy();
	m_Framework.Destroy();
}


//******************************************************************************
//* LOG HISTORY:
//******************************************************************************
/*
$Log:   N:\PVCSarch65\ProjectFiles\archives\SVObserver_SRC\SVObserver\SVIOTabbedView.cpp_v  $
 * 
 *    Rev 1.3   30 Oct 2013 10:45:20   tbair
 * Project:  SVObserver
 * Change Request (SCR) nbr:  852
 * SCR Title:  Add Multiple Platform Support to SVObserver's Visual Studio Solution
 * Checked in by:  tBair;  Tom Bair
 * Change Description:  
 *   Added #ifndef _WIN64 to prevent depricated PLC code from compiling in 64bit.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.2   01 Oct 2013 14:31:00   tbair
 * Project:  SVObserver
 * Change Request (SCR) nbr:  852
 * SCR Title:  Add Multiple Platform Support to SVObserver's Visual Studio Solution
 * Checked in by:  tBair;  Tom Bair
 * Change Description:  
 *   Add x64 platform.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.1   06 May 2013 20:06:26   bWalter
 * Project:  SVObserver
 * Change Request (SCR) nbr:  814
 * SCR Title:  Upgrade SVObserver to Compile Using Visual Studio 2010
 * Checked in by:  bWalter;  Ben Walter
 * Change Description:  
 *   Merged with svo_src label SVO 6.10 Beta 010.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.4   16 Apr 2013 14:24:16   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  822
 * SCR Title:  Remove DDE Input and Output functionality from SVObserver
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Removed DDE functionality from the source code.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.0   23 Apr 2013 11:09:46   bWalter
 * Project:  SVObserver
 * Change Request (SCR) nbr:  814
 * SCR Title:  Upgrade SVObserver to Compile Using Visual Studio 2010
 * Checked in by:  bWalter;  Ben Walter
 * Change Description:  
 *   Initial check in to SVObserver_src.  (Merged with svo_src label SVO 6.10 Beta 008.)
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.3   30 Jan 2012 11:12:40   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  748
 * SCR Title:  Add Remote Output Steams to SVObserver
 * Checked in by:  jSpila;  Joseph Spila
 * Change Description:  
 *   Modified source code to consolidate PLC and New Output stream functionality to I/O Controller.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.2   13 Jan 2009 15:08:34   tbair
 * Project:  SVObserver
 * Change Request (SCR) nbr:  615
 * SCR Title:  Integrate PLC Classes into SVObserver Outputs
 * Checked in by:  tBair;  Tom Bair
 * Change Description:  
 *   Changed inputs tab description.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.1   09 Jan 2009 09:27:26   tbair
 * Project:  SVObserver
 * Change Request (SCR) nbr:  615
 * SCR Title:  Integrate PLC Classes into SVObserver Outputs
 * Checked in by:  tBair;  Tom Bair
 * Change Description:  
 *   Changed descriptions on IO tabs from inputs and outputs to Discrete Inputs and Discrete Outputs
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.0   14 Nov 2008 14:08:20   tbair
 * Project:  SVObserver
 * Change Request (SCR) nbr:  615
 * SCR Title:  Integrate PLC Classes into SVObserver Outputs
 * Checked in by:  tBair;  Tom Bair
 * Change Description:  
 *   Initial Check-in
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
*/

