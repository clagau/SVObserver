//******************************************************************************
//* COPYRIGHT (c) 2008 by SVResearch, Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVPLCPropSheet
//* .File Name       : $Workfile:   SVPLCPropSheet.h  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.0  $
//* .Check In Date   : $Date:   23 Apr 2013 13:25:18  $
//******************************************************************************

#pragma once

// SVPLCPropSheet
#include "SVPLCControlPar.h"
#include "SVMaterialsLibrary\SVMaterialsTree.h"
#include <boost/config.hpp>
#include <boost/function.hpp>


class SVPLCPropSheet : public CPropertySheet
{
	DECLARE_DYNAMIC(SVPLCPropSheet)

public:
	SVPLCPropSheet(UINT nIDCaption, CWnd* pParentWnd = NULL, UINT iSelectPage = 0);
	SVPLCPropSheet(LPCTSTR pszCaption, CWnd* pParentWnd = NULL, UINT iSelectPage = 0);
	virtual ~SVPLCPropSheet();
	void AddPages();

	SVMaterials m_ControlMaterials;
	CString m_strCurrentPLCId;

	boost::function< HRESULT (CString, SVMaterials)> m_ValidateBlocks;
protected:
	DECLARE_MESSAGE_MAP()
};

//******************************************************************************
//* LOG HISTORY:
//******************************************************************************
/*
$Log:   N:\PVCSarch65\ProjectFiles\archives\SVObserver_src\SVObserver\SVPLCPropSheet.h_v  $
 * 
 *    Rev 1.0   23 Apr 2013 13:25:18   bWalter
 * Project:  SVObserver
 * Change Request (SCR) nbr:  814
 * SCR Title:  Upgrade SVObserver to Compile Using Visual Studio 2010
 * Checked in by:  bWalter;  Ben Walter
 * Change Description:  
 *   Initial check in to SVObserver_src.  (Merged with svo_src label SVO 6.10 Beta 008.)
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.0   14 Nov 2008 14:10:34   tbair
 * Project:  SVObserver
 * Change Request (SCR) nbr:  615
 * SCR Title:  Integrate PLC Classes into SVObserver Outputs
 * Checked in by:  tBair;  Tom Bair
 * Change Description:  
 *   Initial Check-in
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
*/


