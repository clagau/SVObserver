// ******************************************************************************
// * COPYRIGHT (c) 2000 by SVResearch, Harrisburg
// * All Rights Reserved
// ******************************************************************************
// * .Module Name     : SVPLC500DDEClass
// * .File Name       : $Workfile:   SLC500DDEClass.h  $
// * ----------------------------------------------------------------------------
// * .Current Version : $Revision:   1.0  $
// * .Check In Date   : $Date:   25 Apr 2013 15:36:10  $
// ******************************************************************************

#if !defined(AFX_SVPLC500DDEClass_H__85DDA305_C3AC_11D3_A870_00106F000B22__INCLUDED_)
#define AFX_SVPLC500DDEClass_H__85DDA305_C3AC_11D3_A870_00106F000B22__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif

#include <DTL.h>
#include "PLCBaseClass.h"

class SVPLCBaseClass;
struct SVPLCElementStruct;

class SV_PLCAPI_DS SVPLC500DDEClass : public SVPLCBaseClass  
{
public:

   DTLDRIVER   svmDtlDrivers [DTL_MAX_DRIVERS];

   long        svmlNbrDrivers;


   long        svmlStationNbr;

   long        svmlDriverId;

// svmczPortId - This is purposefully kept as a ASCII character string to be
// compatible with the RSLinx API functions.
   char        svmczPortId [SV_GENERIC_NAME_SIZE];



	SVPLC500DDEClass(long* aplErr);

	 ~SVPLC500DDEClass();


   long     Open (TCHAR *apConnectionString);

   long     Close ();


   long     CheckStatus (SVPLCBlockStruct* apBlock);


   long     ParseElements (SVPLCElementStructArray* apAddress);

   long     QuickSortElements (SVPLCElementStructArray*  apSortArray,
                               long                      alBeginning,
                               long                      alEnd);


   long     AddElementToBlock (SVPLCBlockStruct*     apBlock,
                               SVPLCElementStruct*   apElement);


   long     RebuildAddressString (SVPLCBlockStruct* apBlock);


   long     DataTypeSize (SVPLCDataTypeEnum aDataType);


   long     SendB (SVPLCBlockStruct* apBlock);


   long     GetInt16 (SVPLCBlockStruct* apBlock);

   long     GetStr (SVPLCBlockStruct* apBlock);


   long     CopyReceiveBufferToLong (SVPLCBlockStruct* apBlock);   

   long     CopyReceiveBufferToFloat (SVPLCBlockStruct* apBlock);   

   long     CopyReceiveBufferToString (SVPLCBlockStruct* apBlock);   

protected:


   virtual long   CopyLongToSendBuffer (SVPLCBlockStruct* apBlock); 

   virtual long   CopyDoubleToSendBuffer (SVPLCBlockStruct* apBlock); 

   virtual long   CopyStringToSendBuffer (SVPLCBlockStruct* apBlock);

// RSLINX WRAPPERS ----------------------------------------------------------
// RSLINX has a well defined error system which must be mapped over to the 
// SVResearch error system.  This is easiest using these function wrappers.


   long     DTLCDefine (unsigned long* name_id, const char* definition);

   long     DTLReadCB (unsigned long         name_id,
                       unsigned char*        variable,
                       unsigned long         time_out,
                       DTL_IO_CALLBACK_PROC  callback_proc,
                       unsigned long         callback_param);

// End of RSLINX WRAPPERS. --------------------------------------------------
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


#endif // !defined(AFX_SVPLC500DDEClass_H__85DDA305_C3AC_11D3_A870_00106F000B22__INCLUDED_)

// ******************************************************************************
// * LOG HISTORY:
// ******************************************************************************
/*
$Log:   N:\PVCSarch65\ProjectFiles\archives\SVObserver_src\SVP_SRC\SLC500DDEClass.h_v  $
 * 
 *    Rev 1.0   25 Apr 2013 15:36:10   bWalter
 * Project:  SVObserver
 * Change Request (SCR) nbr:  814
 * SCR Title:  Upgrade SVObserver to Compile Using Visual Studio 2010
 * Checked in by:  bWalter;  Ben Walter
 * Change Description:  
 *   Initial check in to SVObserver_src.  (Merged with svo_src label SVO 6.10 Beta 008.)
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.0   14 Nov 2008 14:56:10   tbair
 * Project:  SVObserver
 * Change Request (SCR) nbr:  615
 * SCR Title:  Integrate PLC Classes into SVObserver Outputs
 * Checked in by:  tBair;  Tom Bair
 * Change Description:  
 *   Initial Check-in
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.1   12 Sep 2000 14:03:52   Jim
 * Project:  SVFocusNT
 * Change Request (SCR) nbr:  14
 * SCR Title:  Stage 2: Expand PLC Support
 * Checked in by:  Jim;  James A. Brown
 * Change Description:  
 *   Added title block information.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.0   Mar 22 2000 17:32:08   mike
 * Initial revision.
*/