//******************************************************************************
//* COPYRIGHT (c) 2008 by Seidenader Vision Inc., Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVImageFileLoader
//* .File Name       : $Workfile:   SVImageFileLoader.h  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.0  $
//* .Check In Date   : $Date:   22 Apr 2013 10:11:22  $
//******************************************************************************
#pragma  once

#pragma region Includes
#include "SVImageFile.h"
#pragma endregion Includes

class SVImageFileLoader
{
public:
	//************************************
	//! The method loads a bitmap file 
	//! \param FileName <in> The name of the bitmap file
	//! \param rImageFile <out> The image file information
	//! \returns S_OK when successful
	//************************************
	static HRESULT Load( LPCTSTR FileName, SVImageFile& rImageFile );

	//************************************
	//! The method loads the first bitmap file
	//! \param Name <in> The name of the bitmap file or a directory containing the files
	//! \param Ext <in> The file extension to search for the first file
	//! \param rImageFile <out> The image file information
	//! \returns S_OK when successful
	//************************************
	static HRESULT LoadFirstFile( LPCTSTR Name, LPCTSTR Ext, SVImageFile& rImageFile );
};

//******************************************************************************
//* LOG HISTORY:
//******************************************************************************
/*
$Log:   N:\PVCSarch65\ProjectFiles\archives\SVObserver_SRC\SVHBitmapUtilitiesLibrary\SVImageFileLoader.h_v  $
 * 
 *    Rev 1.0   22 Apr 2013 10:11:22   bWalter
 * Project:  SVObserver
 * Change Request (SCR) nbr:  814
 * SCR Title:  Upgrade SVObserver to Compile Using Visual Studio 2010
 * Checked in by:  bWalter;  Ben Walter
 * Change Description:  
 *   Initial check in to SVObserver_src.  (Merged with svo_src label SVO 6.10 Beta 008.)
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.0   10 Sep 2008 10:41:00   sjones
 * Project:  SVObserver
 * Change Request (SCR) nbr:  612
 * SCR Title:  Implement a timer trigger object
 * Checked in by:  sJones;  Steve Jones
 * Change Description:  
 *   Initial CheckIn
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
*/