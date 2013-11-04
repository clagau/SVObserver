//******************************************************************************
//* COPYRIGHT (c) 2013 by Seidenader Vision Inc., Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVRSLinxABPLCBaseClass
//* .File Name       : $Workfile:   SVRSLinxABPLCBaseClass.h  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.0  $
//* .Check In Date   : $Date:   25 Apr 2013 16:04:54  $
//******************************************************************************

#ifndef SVRSLINXABPLCBASECLASS_H_INCLUDED
#define SVRSLINXABPLCBASECLASS_H_INCLUDED

#if _MSC_VER > 1000
#pragma once
#endif

#include <DTL.h>

#include "SVABPLCBaseClass.h"

class SV_PLCAPI_DS SVRSLinxABPLCBaseClass : public SVABPLCBaseClass  
{
public:
	SVRSLinxABPLCBaseClass(long* aplErr);
	virtual ~SVRSLinxABPLCBaseClass();

	long CheckStatus (SVPLCBlockStruct* apBlock);
	long DeinitializeBlocks (SVPLCBlockStruct* apBlock);
	long Open (TCHAR *apConnectionString);
	long Close ();
	long SendB (SVPLCBlockStruct* apBlock);
	long SendInt16 (SVPLCBlockStruct* apBlock);
	long SendStr (SVPLCBlockStruct* apBlock);
	long SendSPFloat (SVPLCBlockStruct* apBlock);
	long GetB (SVPLCBlockStruct* apBlock);
	long GetInt16 (SVPLCBlockStruct* apBlock);
	long GetStr (SVPLCBlockStruct* apBlock);
	long GetSPFloat (SVPLCBlockStruct* apBlock);

};

//
// NAME
//	SolCallback
//
// DESCRIPTION
//	This is the callback function used for the read and write sample applictions.
//	This callback expects the calling program to pass a pointer to an sCALLBACK.
//	This sCALLBACK should have a valid event in sCALLBACK.hEvent that the caller will
//	expect this callback to set.
//
//	This callback is used for the DTL_READ_CB() and DTL_WRITE_CB() sample applications.
//
void DTL_CALLBACK SolCallback(DWORD adwParam,DWORD adwIostatus);

#endif // !defined(SVRSLINXABPLCBASECLASS_H_INCLUDED)

//******************************************************************************
//* LOG HISTORY:
//******************************************************************************
/*
$Log:   N:\PVCSarch65\ProjectFiles\archives\SVObserver_src\SVP_SRC\SVRSLinxABPLCBaseClass.h_v  $
 * 
 *    Rev 1.0   25 Apr 2013 16:04:54   bWalter
 * Project:  SVObserver
 * Change Request (SCR) nbr:  814
 * SCR Title:  Upgrade SVObserver to Compile Using Visual Studio 2010
 * Checked in by:  bWalter;  Ben Walter
 * Change Description:  
 *   Initial check in to SVObserver_src.  (Merged with svo_src label SVO 6.10 Beta 008.)
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
*/