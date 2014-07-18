//******************************************************************************
//* COPYRIGHT (c) 2014 by Seidenader
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVOResourceApp
//* .File Name       : $Workfile:   SVOResource.cpp  $
//* .Description	 : The class is the main app of the dll
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.0  $
//* .Check In Date   : $Date:   17 Jul 2014 13:56:08  $
//******************************************************************************

#pragma region Includes
#include "stdafx.h"
#include "SVOResource.h"
#pragma endregion Includes

#pragma region Declarations
using namespace Seidenader::SVOResource;

SVOResourceApp theApp;

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif
#pragma endregion Declarations

BEGIN_MESSAGE_MAP(SVOResourceApp, CWinApp)
END_MESSAGE_MAP()



#pragma region Constructor
SVOResourceApp::SVOResourceApp()
{
}
#pragma endregion Constructor

#pragma region Public Methods
BOOL SVOResourceApp::InitInstance()
{
	CWinApp::InitInstance();

	return TRUE;
}

#pragma endregion Protected Methods

//******************************************************************************
//* LOG HISTORY:
//******************************************************************************
/*
$Log:   N:\PVCSarch65\ProjectFiles\archives\SVObserver_SRC\SVOResource\SVOResource.cpp_v  $
 * 
 *    Rev 1.0   17 Jul 2014 13:56:08   gramseier
 * Project:  SVObserver
 * Change Request (SCR) nbr:  909
 * SCR Title:  Object Selector replacing Result Picker and Output Selector SVO-72, 40, 130
 * Checked in by:  gRamseier;  Guido Ramseier
 * Change Description:  
 *   Initial check in.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
*/
