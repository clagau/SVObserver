//******************************************************************************
//* COPYRIGHT (c) 2003 by SVResearch, Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVBarCodeProperties
//* .File Name       : $Workfile:   SVBarCodeProperties.cpp  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.0  $
//* .Check In Date   : $Date:   23 Apr 2013 09:42:32  $
//******************************************************************************

#include "stdafx.h"
#include "SVBarCodeProperties.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// SVBarCodeProperties

IMPLEMENT_DYNAMIC(SVBarCodeProperties, CPropertySheet)

SVBarCodeProperties::SVBarCodeProperties(UINT nIDCaption, CWnd* pParentWnd, UINT iSelectPage)
	:CPropertySheet(nIDCaption, pParentWnd, iSelectPage)
{
  AddPage (&m_dlgBarCodeGeneral);
  AddPage (&m_dlgBarCodeStringMatch);
  AddPage (&m_dlgBarCodeAttributes);
  AddPage (&m_dlgBarCodeDataMatrix);
}


SVBarCodeProperties::~SVBarCodeProperties()
{
}


BEGIN_MESSAGE_MAP(SVBarCodeProperties, CPropertySheet)
	//{{AFX_MSG_MAP(SVBarCodeProperties)
		// NOTE - the ClassWizard will add and remove mapping macros here.
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

//******************************************************************************
//* LOG HISTORY:
//******************************************************************************
/*
$Log:   N:\PVCSarch65\ProjectFiles\archives\SVObserver_src\SVObserver\SVBarCodeProperties.cpp_v  $
 * 
 *    Rev 1.0   23 Apr 2013 09:42:32   bWalter
 * Project:  SVObserver
 * Change Request (SCR) nbr:  814
 * SCR Title:  Upgrade SVObserver to Compile Using Visual Studio 2010
 * Checked in by:  bWalter;  Ben Walter
 * Change Description:  
 *   Initial check in to SVObserver_src.  (Merged with svo_src label SVO 6.10 Beta 008.)
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.1   17 Apr 2003 16:54:06   rschock
 * Project:  SVObserver
 * Change Request (SCR) nbr:  346
 * SCR Title:  Update SVObserver to Version 4.21 Release
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Redid the #include defines and standardized the Tracker log headers and removed warning from release mode builds.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
*/