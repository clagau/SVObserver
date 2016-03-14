//******************************************************************************
//* COPYRIGHT (c) 2003 by SVResearch, Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVMatroxBoardImageProcessingClass
//* .File Name       : $Workfile:   SVMatroxBoardImageProcessingClass.h  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.0  $
//* .Check In Date   : $Date:   23 Apr 2013 12:31:02  $
//******************************************************************************

#if !defined(AFX_SVMatroxBoardImageProcessingClass_H)
#define AFX_SVMatroxBoardImageProcessingClass_H

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "SVBoardImageProcessingClass.h"
#include "SVMatroxDeviceManager.h"

class SVMatroxBoardImageProcessingClass : public SVBoardImageProcessingClass 
{
public:
	SVMatroxBoardImageProcessingClass( SVImageProcessingClass* pSubsystem, 
	                                   SVDigitizerProcessingClass *p_pDigitizerSubsystem );
	SVMatroxBoardImageProcessingClass( SVImageProcessingClass* pSubsystem, 
	                                   SVDigitizerProcessingClass *p_pDigitizerSubsystem,
															       LPCSTR ObjectName );
	virtual ~SVMatroxBoardImageProcessingClass();

	virtual HRESULT Create();
	virtual HRESULT Destroy();
	virtual HRESULT DisconnectDevices();
	virtual HRESULT ConnectDevices();

private:
	HRESULT LocalCreate();
	HRESULT LocalDestroy();

	/*static*/SVMatroxDeviceManager m_DeviceManager;
	// does this need to be static?? if so, we need to work out the problem of SVMatroxAppManager initialization / App init.
};

#endif // !defined(AFX_SVMatroxDisplayImageProcessingClass_H)

