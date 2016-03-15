// ******************************************************************************
// * COPYRIGHT (c) 2004 by SVResearch, Harrisburg
// * All Rights Reserved
// ******************************************************************************
// * .Module Name     : SVIntekBoardStruct
// * .File Name       : $Workfile:   SVIntekBoardStruct.h  $
// * ----------------------------------------------------------------------------
// * .Current Version : $Revision:   1.0  $
// * .Check In Date   : $Date:   26 Apr 2013 14:50:42  $
// ******************************************************************************

#ifndef SVINTEKBOARDSTRUCT_H
#define SVINTEKBOARDSTRUCT_H

#include "SVContainerLibrary/SVVector.h"
#include "SVCFireBus.h"

struct SVIntekBoardStruct  
{
public:
	SVIntekBoardStruct();
	virtual ~SVIntekBoardStruct();

	CString m_csName;
	CString m_csFireStackName;

	FBCARDID m_ulCardId;

	CFireBus *m_pFireBus;

	HANDLE m_hThread;

	bool m_bFirstBusResetStarted;
	bool m_bFirstBusResetComplete;
};

typedef SVVector< SVIntekBoardStruct* > SVIntekBoardStructPtrCArray;

#endif

