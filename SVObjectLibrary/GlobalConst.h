//******************************************************************************
//* COPYRIGHT (c) 2014 by Seidenader
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : Global constants
//* .File Name       : $Workfile:   GlobalConst.h  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.0  $
//* .Check In Date   : $Date:   17 Mar 2014 14:09:16  $
//* ----------------------------------------------------------------------------
//* This is the global constant file for the SVObjectLibrary project.
//******************************************************************************

#pragma once

namespace Seidenader
{
	namespace SVObjectLibrary
	{
		#pragma region Declarations
		//Root children nodes
		const char			FqnConfiguration[]				= _T( "Configuration Object" );
		const char			FqnInspections[]				= _T( "Inspections" );
		const char			FqnPPQs[]						= _T( "PPQs" );
		const char			FqnRemoteInputs[]				= _T( "RemoteInputs" );
		const char			FqnCameras[]					= _T( "Cameras" );
		#pragma endregion Declarations
	}
}

//******************************************************************************
//* LOG HISTORY:
//******************************************************************************
/*
$Log:   N:\PVCSarch65\ProjectFiles\archives\SVObserver_SRC\SVObjectLibrary\GlobalConst.h_v  $
 * 
 *    Rev 1.0   17 Mar 2014 14:09:16   bwalter
 * Project:  SVObserver
 * Change Request (SCR) nbr:  869
 * SCR Title:  Add PPQ and Environment Variables to Object Manager and Update Pickers
 * Checked in by:  bWalter;  Ben Walter
 * Change Description:  
 *   Initial check in.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
*/
