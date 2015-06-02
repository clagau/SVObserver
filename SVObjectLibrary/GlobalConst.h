//******************************************************************************
//* COPYRIGHT (c) 2014 by Seidenader
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : Global constants
//* .File Name       : $Workfile:   GlobalConst.h  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.3  $
//* .Check In Date   : $Date:   25 Aug 2014 02:33:18  $
//* ----------------------------------------------------------------------------
//* This is the global constant file for the SVObjectLibrary project.
//******************************************************************************

#pragma once

namespace Seidenader
{
	namespace SVObjectLibrary
	{
		#pragma region Declarations
		const TCHAR			FqnConfiguration[]				= _T( "Configuration Object" );
		const TCHAR			FqnInspections[]				= _T( "Inspections" );
		const TCHAR			FqnPPQs[]						= _T( "PPQs" );
		const TCHAR			FqnPPQVariables[]				= _T( "PPQ Variables" );
		const TCHAR			FqnRemoteInputs[]				= _T( "RemoteInputs" );
		const TCHAR			FqnCameras[]					= _T( "Cameras" );
		const TCHAR			FqnEnvironmentMode[]			= _T( "Environment.Mode" );
		static const LCID LCID_USA = MAKELCID(MAKELANGID(LANG_ENGLISH, SUBLANG_ENGLISH_US), SORT_DEFAULT);
		#pragma endregion Declarations
	}
}

namespace SvOl = Seidenader::SVObjectLibrary;

//******************************************************************************
//* LOG HISTORY:
//******************************************************************************
/*
$Log:   N:\PVCSarch65\ProjectFiles\archives\SVObserver_SRC\SVObjectLibrary\GlobalConst.h_v  $
 * 
 *    Rev 1.3   25 Aug 2014 02:33:18   mziegler
 * Project:  SVObserver
 * Change Request (SCR) nbr:  934
 * SCR Title:  Add Remote Access to Environment.Mode Parameters
 * Checked in by:  mZiegler;  Marc Ziegler
 * Change Description:  
 *   add string const parameter FqnEnvironmentMode
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.2   17 Jul 2014 15:07:54   gramseier
 * Project:  SVObserver
 * Change Request (SCR) nbr:  909
 * SCR Title:  Object Selector replacing Result Picker and Output Selector SVO-72, 40, 130
 * Checked in by:  gRamseier;  Guido Ramseier
 * Change Description:  
 *   Added fqnPPQVariables name
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.1   02 Jun 2014 06:37:02   gramseier
 * Project:  SVObserver
 * Change Request (SCR) nbr:  900
 * SCR Title:  Separate View Image Update, View Result Update flags; remote access E55,E92
 * Checked in by:  gRamseier;  Guido Ramseier
 * Change Description:  
 *   Changed char to TCHAR for constants
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
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
