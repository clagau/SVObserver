// ******************************************************************************
// * COPYRIGHT (c) 2003 by SVResearch, Harrisburg
// * All Rights Reserved
// ******************************************************************************
// * .Module Name     : SVPropertySheetCancelImpl
// * .File Name       : $Workfile:   SVPropertySheetCancelImpl.h  $
// * ----------------------------------------------------------------------------
// * .Current Version : $Revision:   1.0  $
// * .Check In Date   : $Date:   23 Apr 2013 14:38:12  $
// ******************************************************************************

#ifndef SVPROPERTYSHEETCANCELIMPL_H
#define SVPROPERTYSHEETCANCELIMPL_H
#pragma once

#include "ISVCancel.h"

class SVPropertySheetCancelImpl : public ISVCancel
{
public:
	virtual bool CanCancel();
	virtual HRESULT GetCancelData(SVCancelData*& ppData);
	virtual HRESULT SetCancelData(SVCancelData* pData);
};

#endif  //#ifndef SVPROPERTYSHEETCANCELIMPL_H

// ******************************************************************************
// * LOG HISTORY:
// ******************************************************************************
/*
$Log:   N:\PVCSarch65\ProjectFiles\archives\SVObserver_src\SVObserver\SVPropertySheetCancelImpl.h_v  $
 * 
 *    Rev 1.0   23 Apr 2013 14:38:12   bWalter
 * Project:  SVObserver
 * Change Request (SCR) nbr:  814
 * SCR Title:  Upgrade SVObserver to Compile Using Visual Studio 2010
 * Checked in by:  bWalter;  Ben Walter
 * Change Description:  
 *   Initial check in to SVObserver_src.  (Merged with svo_src label SVO 6.10 Beta 008.)
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.0   08 Jul 2004 12:39:30   ebeyeler
 * Project:  SVObserver
 * Change Request (SCR) nbr:  406
 * SCR Title:  Implement External Tool
 * Checked in by:  eBeyeler;  Eric Beyeler
 * Change Description:  
 *   first iteration
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
*/