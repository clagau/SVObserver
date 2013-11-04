//******************************************************************************
//* COPYRIGHT (c) 2008 by SVResearch, Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVRemoteOutputPropSheet
//* .File Name       : $Workfile:   SVRemoteOutputPropSheet.h  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.0  $
//* .Check In Date   : $Date:   23 Apr 2013 14:51:02  $
//******************************************************************************

#pragma once

// SVRemoteOutputPropSheet
#include "SVMaterialsLibrary\SVMaterialsTree.h"
#include <boost/config.hpp>
#include <boost/function.hpp>


class SVRemoteOutputPropSheet : public CPropertySheet
{
	DECLARE_DYNAMIC(SVRemoteOutputPropSheet)

public:
	SVRemoteOutputPropSheet(UINT nIDCaption, CWnd* pParentWnd = NULL, UINT iSelectPage = 0);
	SVRemoteOutputPropSheet(LPCTSTR pszCaption, CWnd* pParentWnd = NULL, UINT iSelectPage = 0);
	virtual ~SVRemoteOutputPropSheet();
	void AddPages();

	SVMaterials m_ControlMaterials;
	CString m_strCurrentDLLId;

	boost::function< HRESULT (CString, SVMaterials)> m_ValidateBlocks;
protected:
	DECLARE_MESSAGE_MAP()
};

//******************************************************************************
//* LOG HISTORY:
//******************************************************************************
/*
$Log:   N:\PVCSarch65\ProjectFiles\archives\SVObserver_src\SVObserver\SVRemoteOutputPropSheet.h_v  $
 * 
 *    Rev 1.0   23 Apr 2013 14:51:02   bWalter
 * Project:  SVObserver
 * Change Request (SCR) nbr:  814
 * SCR Title:  Upgrade SVObserver to Compile Using Visual Studio 2010
 * Checked in by:  bWalter;  Ben Walter
 * Change Description:  
 *   Initial check in to SVObserver_src.  (Merged with svo_src label SVO 6.10 Beta 008.)
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
*/