//******************************************************************************
//* COPYRIGHT (c) 2013 by Seidenader Vision Inc., Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVS7PBFMSClass
//* .File Name       : $Workfile:   SVS7PBFMS.h  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.0  $
//* .Check In Date   : $Date:   25 Apr 2013 16:06:00  $
//******************************************************************************

#if !defined(AFX_SVS7PBFMSClass_H__AC88413A_2767_11D4_A8DF_00106F000B22__INCLUDED_)
#define AFX_SVS7PBFMSClass_H__AC88413A_2767_11D4_A8DF_00106F000B22__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif

extern "C"
{
#include "applicom.h"
}
#include "plc_interface.h"
#include "PLCBaseClass.h"

class SV_PLCAPI_DS SVS7PBFMSClass : public SVPLCBaseClass  
{
public:

	SVS7PBFMSClass(long* aplErr);

	virtual ~SVS7PBFMSClass();

   virtual long Initialize ();


   long     SendB (SVPLCBlockStruct* apBlock);


   long     SendInt16 (SVPLCBlockStruct* apBlock);


   long     SendInt32 (SVPLCBlockStruct* apBlock);
  

   long     SendStr (SVPLCBlockStruct* apBlock);


   long     SendSPFloat (SVPLCBlockStruct* apBlock);


   long     GetB (SVPLCBlockStruct* apBlock);


   long     GetInt16 (SVPLCBlockStruct* apBlock);


   long     GetInt32 (SVPLCBlockStruct* apBlock);


   long     GetStr (SVPLCBlockStruct* apBlock);


   long     GetSPFloat (SVPLCBlockStruct* apBlock);

// Open () ------------------------------------------------------------------
// apConnectionString is the input parameter which will indicate all 
// connection parameters.  The format of the input string will be;
// "DriverName, StationNumber" where 
// Driver Name indicates the name of the 
//   connection as referenced by RSLinx.  
// Station Number will reference the station.

   long Open (TCHAR *apConnectionString);


   long Close ();



   long     CopyReceiveBufferToString (SVPLCBlockStruct* apBlock);   


   long     CopyReceiveBufferToFloat (SVPLCBlockStruct* apBlock);   


   long     CopyReceiveBufferToLong (SVPLCBlockStruct* apBlock);   


   long     DataTypeSize (SVPLCDataTypeEnum aDataType);


   long     RebuildAddressString (SVPLCBlockStruct* apBlock);


   long     AddElementToBlock (SVPLCBlockStruct*     apBlock,
                               SVPLCElementStruct*   apElement);
   

   long     QuickSortElements (SVPLCElementStructArray*  apSortArray,
                               long                      alBeginning,
                               long                      alEnd);
   

   long     ParseElements (SVPLCElementStructArray* apAddress);


   long     CheckStatus (SVPLCBlockStruct* apBlock);



   virtual long   CopyLongToSendBuffer (SVPLCBlockStruct* apBlock); 


   virtual long   CopyDoubleToSendBuffer (SVPLCBlockStruct* apBlock); 


   virtual long   CopyStringToSendBuffer (SVPLCBlockStruct* apBlock);



   virtual long   InitializeBlocks (SVPLCBlockStruct* apBlock);

//   virtual BOOL SVS7PBFMSClass::SetupConnection(CString& szConnection);


   void ClearRegisters();

   

   static bool ParseAddress( _TCHAR *szAddr, long &lAddress, SVPLCDataTypeEnum &eTyp );


   static bool ParseBitAddress( CString &csBitAddr, long &lBitAddress );


   static bool ExtractNumber( CString &csStr, long &lVal );


   static long TranslateError( short nStatus );


   bool  AddToRequestQueue( SVPLCBlockStruct* apBlock, long lOperation );


   static CMutex   m_SVDeferQLock;

   static SVPLCCommQueueStructList svmRequestQueue;


   TCHAR       svmczConnectionString [SV_GENERIC_NAME_SIZE];

//jab090600   DTLDRIVER   svmDtlDrivers [DTL_MAX_DRIVERS];

//jab090600   long        svmlNbrDrivers;

//jab090600   long        svmlDriverId;

   // Applicom channel number (normally 0)

   short       svmPLCChannelNo;

   // Applicom(TM) equipment number (0 -> 127)

   short       svmPLCEquipNo;


// svmczPortId - This is purposefully kept as a ASCII character string to be
// compatible with the RSLinx API functions.

   char        svmczPortId [SV_GENERIC_NAME_SIZE];


   char        svmczDriverName [SV_GENERIC_NAME_SIZE];


   static DWORD  m_svmdwDeferThreadId;         // ThreadID for Deferred operations thread


   static HANDLE m_svmhDeferThread;

// The mutex for controlling access to m_svmhDeferThread.
   static CMutex svmDeferThreadLock;

   static long   svmlStopDef;


private:

//jab083000   static CArray <long, long> msvUsedIds; 

// msvUnusedIds only contain a list of indexes which have been previously 
// removed from msvUsedIds.
//jab083000   static CArray <long, long> msvUnusedIds; 

};


DWORD WINAPI S7DeferManager (LPVOID apCallingClass);

//


#endif // !defined(AFX_SVS7PBFMSClass_H__AC88413A_2767_11D4_A8DF_00106F000B22__INCLUDED_)

//-------------------------------------------------------//
//                C O N S T A N T S                      //
//-------------------------------------------------------//

const _TCHAR S7_ADDR_FLAG_CHAR		= _TCHAR('F');
const _TCHAR S7_ADDR_INPUT_CHAR		= _TCHAR('I');
const _TCHAR S7_ADDR_OUTPUT_CHAR	   = _TCHAR('Q');
const _TCHAR S7_ADDR_DBLOCK_CHAR	   = _TCHAR('D');
const _TCHAR S7_ADDR_TIMER_CHAR		= _TCHAR('T');
const _TCHAR S7_ADDR_COUNTER_CHAR	= _TCHAR('C');

const _TCHAR S7_ADDR_BYTE_CHAR		= _TCHAR('B');

const _TCHAR S7_ADDR_WORD_CHAR		= _TCHAR('W');

const _TCHAR S7_ADDR_DOUBLE_CHAR	   = _TCHAR('D');

const _TCHAR S7_ADDR_FLOAT_CHAR		= _TCHAR('F');

const _TCHAR S7_ADDR_DBBIT_CHAR		= _TCHAR('X');

// Number of bytes / datablock
const long S7_BYTES_PER_DB = 65536;

// Number of bits / byte
const long S7_BITS_PER_BYTE = 8;

//Number of bits / datablock
const long S7_BITS_PER_DB = S7_BYTES_PER_DB * S7_BITS_PER_BYTE;


const int S7_STATUS_SUCCESS = 0;

const short S7_STATUS_DEFREQ_NOTCOMPLETED = -5;

const short S7_STATUS_TRANSDIF_WRITECOMPLETE = -1;

const short S7_STATUS_CYCFUNC_NOTFOUND = -6;

const short S7_STATUS_UNKNWN_FUNC = 1;   // Unknown function

const short S7_STATUS_INVALID_ADDR = 2;  // Incorrect address

const short S7_STATUS_INVALID_DATA = 3;  // Incorrect data

const short S7_STATUS_IRRETRIEVABLE_DATA = 4;    // Irretrievable data

const short S7_STATUS_BADFUNC_PARAM = 32;         // Bad function parameter

const short S7_STATUS_CYCFUNC_NOTACTIVE = 35;     // Cyclic function not activated

const short S7_STATUS_DEFREQ_REGFULLEXIT = 40;    
// Deferred read or 
// write attempt when the 
// deferred request register is full.
// Another task must free the resources by making an exitbus   
// "Deferred read or write attempt when the deferred request register is full
// (exitbus() must be used to eliminate error.

const short S7_STATUS_DEFREQ_REGFULLTRANSDIF = 41;       
                     // "Deferred read or write attempt when the deferred request register is full
                     // (transdif() or transdifpack() can be used to eliminate error)
const short S7_STATUS_EMPTY_TRANSDIF = 42;
	                  // Deferred request transfer attempt with empty TRANSDIF (or TRANSDIFPACK

const short S7_STATUS_NOTMEMRESIDIENT = 45;
                     //  Applicom(TM) communication software is not memory resident

const short S7_STATUS_BDNO_NOTCONFIGURED = 46;
                     // "Board number not configured

const short S7_STATUS_NO_INTERFACE = 47;
                     //  No interface present

const short S7_STATUS_DRIVE_SYSTEM = 51;
                     // Driver system problem

const short S7_STATUS_NOKEY = 59;
                     // No protection key present

const short S7_STATUS_IFACE_MEMORY = 66;
                     // Insufficient interface memory


//---------------------------------------------------------
// SVResearch Error Codes
//---------------------------------------------------------
const long  S7_ERR_COMMUNICATIONS = -1550;
                     // S7 Communications error

const long     S7_DEFER_WRITE_OPER = 2;
const long     S7_DEFER_READ_OPER = 1;

// Maximum string length for S7 strings
//const long     S7_MAX_STRING_LENGTH = 42;
const long     S7_MAX_STRING_LENGTH = 218;

//******************************************************************************
//* LOG HISTORY:
//******************************************************************************
/*
$Log:   N:\PVCSarch65\ProjectFiles\archives\SVObserver_src\SVP_SRC\SVS7PBFMS.h_v  $
 * 
 *    Rev 1.0   25 Apr 2013 16:06:00   bWalter
 * Project:  SVObserver
 * Change Request (SCR) nbr:  814
 * SCR Title:  Upgrade SVObserver to Compile Using Visual Studio 2010
 * Checked in by:  bWalter;  Ben Walter
 * Change Description:  
 *   Initial check in to SVObserver_src.  (Merged with svo_src label SVO 6.10 Beta 008.)
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
*/