//******************************************************************************
//* COPYRIGHT (c) 2013 by Seidenader Vision Inc., Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVABPLCBaseClass
//* .File Name       : $Workfile:   SVABPLCBaseClass.h  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.1  $
//* .Check In Date   : $Date:   08 May 2013 15:36:52  $
//******************************************************************************

#ifndef SVABPLCBaseClass_H_INCLUDED
#define SVABPLCBaseClass_H_INCLUDED

#include "PLCBaseClass.h"

class SV_PLCAPI_DS SVABPLCBaseClass : public SVPLCBaseClass  
{
public:
	SVABPLCBaseClass(long* aplErr);
	virtual ~SVABPLCBaseClass();

// SendBitBlock () - Normally Block transfers are at the base class level,
// but bit transfers are very PLC type specific.
	
   long     SendBitBlock (long* aplValueArray, SVPLCBlockStruct* apBlock);

// GetBitBlock () - Normally Block transfers are at the base class level,
// but bit transfers are very PLC type specific.
	
   long     GetBitBlock (long* aplValueArray, SVPLCBlockStruct* apBlock);

   long     CopyReceiveBufferToString (SVPLCBlockStruct* apBlock);   
	
   long     CopyReceiveBufferToFloat (SVPLCBlockStruct* apBlock);   
	
   long     CopyReceiveBufferToLong (SVPLCBlockStruct* apBlock);   

// CopyPLCStringToPCString () - Allen - Bradley PLC data is stored (as raw 
// data) in a low byte, high byte format.  All bytes will need swapped in 
// order to be properly interpreted. 
// The first two bytes then represent the string size.  The following number
// of bytes equivilent to the string size represent the string.  The raw data
// will not be zero terminated.
	
   long     CopyPLCStringToPCString (TCHAR*  atzPCString,
                                     char*   apPLCRawData,
                                     long    alMaxLength);

   virtual long DataTypeSize (SVPLCDataTypeEnum aDataType);
	
   long     RebuildAddressString (SVPLCBlockStruct* apBlock);
	
   long     AddElementToBlock (SVPLCBlockStruct*     apBlock,
                               SVPLCElementStruct*   apElement);
	
   long     QuickSortElements (SVPLCElementStructArray*  apSortArray,
                               long                      alBeginning,
                               long                      alEnd);
	
   long     ParseElements (SVPLCElementStructArray* apAddress);

// IsSameElement () - 
// Return values:
//  0 (FALSE) indicates that they are not part of the same element.
//  1 (TRUE) indicates that they are part of the same element.
//  < 0 indicates that there was an error.
	
   long     IsSameElement (SVPLCElementStruct*   apElement1,
                           SVPLCElementStruct*   apElement2);

   virtual long   CopyLongToSendBuffer (SVPLCBlockStruct* apBlock); 
	
   virtual long   CopyDoubleToSendBuffer (SVPLCBlockStruct* apBlock); 
	
   virtual long   CopyStringToSendBuffer (SVPLCBlockStruct* apBlock);

   virtual long   GetMaxDTSize( void );
	
   virtual long   TestCommunications();
	
   TCHAR       svmczConnectionString [120];

   DTLDRIVER   svmDtlDrivers [DTL_MAX_DRIVERS];
	
   long        svmlNbrDrivers;

   long        svmlDriverId;

// svmczNonUCPortId - This is purposefully kept as a ASCII character string to be
// compatible with the RSLinx API functions.
// Absolutely NOT unicode.
	
   char        svmczNonUCPortId [SV_GENERIC_NAME_SIZE];

// svmczNonUCDriverName - Absolutely NOT unicode.
	
   char        svmczNonUCDriverName [SV_GENERIC_NAME_SIZE];
	
   char        svmczNonUCStationNbr [SV_GENERIC_NAME_SIZE];
	
   char        svmczNonUCIPAddress [SV_GENERIC_NAME_SIZE];

   long        Open (TCHAR *apConnectionString);

protected:
   static CMutex svmDriverIdListLock;
	
   static CArray <long, long> msvUsedIds; 

// msvUnusedIds only contain a list of indexes which have been previously 
// removed from msvUsedIds.
	
   static CArray <long, long> msvUnusedIds; 
};

#endif // !defined(SVABPLCBaseClass_H_INCLUDED)

//******************************************************************************
//* LOG HISTORY:
//******************************************************************************
/*
$Log:   N:\PVCSarch65\ProjectFiles\archives\SVObserver_src\SVP_SRC\SVABPLCBaseClass.h_v  $
 * 
 *    Rev 1.1   08 May 2013 15:36:52   bWalter
 * Project:  SVObserver
 * Change Request (SCR) nbr:  814
 * SCR Title:  Upgrade SVObserver to Compile Using Visual Studio 2010
 * Checked in by:  bWalter;  Ben Walter
 * Change Description:  
 *   Initial check in to SVObserver_src.  (Merged with svo_src label SVO 6.10 Beta 012.)
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.2   29 Apr 2013 13:29:02   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  830
 * SCR Title:  Consolidate all Time Stamp and Clock Functionality
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated source code to use the new SVClock::SVTimeStamp type for all time stamp variables.  Update to use new SVClock::GetTimeStamp() function to get the time stamp value in milliseconds.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.0   25 Apr 2013 15:38:24   bWalter
 * Project:  SVObserver
 * Change Request (SCR) nbr:  814
 * SCR Title:  Upgrade SVObserver to Compile Using Visual Studio 2010
 * Checked in by:  bWalter;  Ben Walter
 * Change Description:  
 *   Initial check in to SVObserver_src.  (Merged with svo_src label SVO 6.10 Beta 008.)
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
*/