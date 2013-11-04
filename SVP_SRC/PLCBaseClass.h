// ******************************************************************************
// * COPYRIGHT (c) 2000 by SVResearch, Harrisburg
// * All Rights Reserved
// ******************************************************************************
// * .Module Name     : SVPLC
// * .File Name       : $Workfile:   PLCBaseClass.h  $
// * ----------------------------------------------------------------------------
// * .Current Version : $Revision:   1.0  $
// * .Check In Date   : $Date:   25 Apr 2013 15:32:52  $
// ******************************************************************************

#ifndef SVPLCBaseClass_H_INCLUDED
#define SVPLCBaseClass_H_INCLUDED

#include <afxmt.h>      // for CMutex
#include <afxtempl.h>   // for CArray template

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#ifndef SV_PLCAPI_DS
   #ifdef BUILD_PLCAPI_DLL
   #define SV_PLCAPI_DS __declspec (dllexport)
   #else
   #define SV_PLCAPI_DS __declspec (dllimport)
   #endif
#endif

#define SV_PLC_ADDRESS_SIZE      30
#define SV_GENERIC_NAME_SIZE     40
#define SV_MAX_SLC500_ST_LENGTH  84

#define SV_MANUFACTURER _T("Manufacturer")
#define SV_PLC_TYPE     _T("PLCType")
#define SV_CONNECTION_TYPE _T("ConnectionType")
#define SV_DRIVER_NAME  _T("DriverName")
#define SV_STATION_NBR  _T("StationNumber")
#define SV_IP_ADDRESS   _T("IPAddress")
#define SV_DHNODE_ADDRESS _T("DHNodeAddress")
#define SV_CHANNEL_NBR  _T("ChannelNumber")
#define SV_EQUIPMENT_NBR _T("EquipmentNumber")

// MANUFACTURER -------------------------------------------------------------
#define SV_ALLENBRADLEY _T("AllenBradley")
#define SV_SIEMENS      _T("Siemens")
// End of MANUFACTURER ------------------------------------------------------

// PLC TYPES ----------------------------------------------------------------
#define SV_SLC500       _T("SLC500")
#define SV_PLC5         _T("PLC5")
#define SV_MICROLOGIX   _T("Micrologix")
#define SV_S7           _T("Step7")
// End of PLC TYPES ---------------------------------------------------------

// CONNECTIONS --------------------------------------------------------------
#define SV_KTXDDE       _T("KTXDDE")
#define SV_ETHERNET     _T("Ethernet")
#define SV_KTXDH        _T("KTXDataHighway")
// The SV_APPLICOM connection probably supports considerably more than the 
// profibus connection, but for now, that will be how it is identified.
#define SV_APPLICOM     _T("ProfibusS7")
#define SV_ASABTCP      _T("ASAB_Ethernet")

#pragma pack( push , 4  )
 
enum SVPCDataTypeEnum
{


   SV_PCTYPE_UNKNOWN = 0,

   SV_PCTYPE_BIT,

   SV_PCTYPE_INT,

   SV_PCTYPE_FLOAT,

   SV_PCTYPE_STRING
};


enum SVPLCDataTypeEnum
{

   SV_PLC_UNKNOWN = 0,
// AB - B

   SV_PLC_BIT,
// AB - N

   SV_PLC_INT16,

   SV_PLC_INT32,
// AB - ST (NOT A)

   SV_PLC_STRING,
// AB - A

   SV_PLC_ASCII,
// AB - F

   SV_PLC_SPFLOAT,
// AB - Not supported

   SV_PLC_DPFLOAT

};



struct SV_PLCAPI_DS SVPLCElementStruct
{

	SVPLCElementStruct ();

// FOR API ------------------------------------------------------------------
// DATA TABLE ADDRESS -------------------------------------------------------

   char       svmPLCAddress [SV_PLC_ADDRESS_SIZE];//Text, as entered string.

   int         svmPLCFileNbr;       
// End of DATA TABLE ADDRESS ------------------------------------------------
// End of FOR API -----------------------------------------------------------

// FOR DDE ------------------------------------------------------------------

   TCHAR       svmConnectionName [SV_GENERIC_NAME_SIZE];

   TCHAR       svmTopicName [SV_GENERIC_NAME_SIZE];

   TCHAR       svmItemName [SV_GENERIC_NAME_SIZE];
// End of FOR DDE -----------------------------------------------------------


   SVPLCDataTypeEnum svmElementType;
// svmElement - For API this will reference the element index in the PLC data 
// table.
// For DDE this will reference the element index in the recieved item data.

   long         svmElement;


   unsigned long svmBitMask;

   long        svmlBitNum;


   long        svmlParsed;

// svmlOriginalIndex - This indicates the index within the origianl element 
// array as it was sent to CreateBlocks ().  This value is set be CreateBlocks ().

   long        svmlOriginalIndex;
};


#pragma warning( disable : 4251 )



typedef  CArray <SVPLCElementStruct*, SVPLCElementStruct*> SVPLCElementStructArray;



enum SVPLCManufacturerEnum
{

   SV_PLCMANUFACTURER_DONTCARE = 0,

   SV_PLCMANUFACTURER_ALLENBRADLEY,

   SV_PLCMANUFACTURER_SEIMENS
};



enum SVPLCModelEnum
{

   SV_PLCMODEL_DONTCARE = 0,

   SV_PLCMODEL_PLC5,

   SV_PLCMODEL_SLC500,

   SV_PLCMODEL_STEP7,

   SV_PLCMODEL_MICROLOGIX
};


enum SVPLCSubModelEnum
{

   SV_PLCSUBMODEL_DONTCARE = 0,

   SV_PLCSUBMODEL_PLC5_20,

   SV_PLCSUBMODEL_SLC504,

   SV_PLCSUBMODEL_SLC505,

   SV_PLCSUBMODEL_STEP7_200,

   SV_PLCSUBMODEL_STEP7_300,

   SV_PLCSUBMODEL_MICROLOGIX1000,

   SV_PLCSUBMODEL_MICROLOGIX1200,

   SV_PLCSUBMODEL_MICROLOGIX1500
};


enum SVPLCConnectionMethodEnum
{

   SV_PLCCONNECTIONMETHOD_DONTCARE = 0,

   SV_PLCCONNECTIONMETHOD_APIKTX,

   SV_PLCCONNECTIONMETHOD_APIETH,

   SV_PLCCONNECTIONMETHOD_DDE,

   SV_PLCCONNECTIONMETHOD_APIAPL,

   SV_PLCCONNECTIONMETHOD_ASABTCP,
};

// SVPLCCommStatusEnum

enum SVPLCCommStatusEnum
{
// Only CheckMessage () should ever set to this value.
// CheckMessage () should only be called from ResponseManager.

   SV_PLCCOMMSTATUS_COMPLETE = 0,

// Only WriteBlock () and ReadBlock () should ever set to this value.

   SV_PLCCOMMSTATUS_BEGIN,

// Only SendManager () should ever set to this value.

   SV_PLCCOMMSTATUS_PENDING,

// This value is presently set in callback.  For other platforms, should be 
// set by the code that first receives the response.

   SV_PLCCOMMSTATUS_ALMOSTCOMPLETE,

// This value is set by CancelTransaction () and CheckMessage ().
// CancelTransaction () should be used causiously. Any non-RSLinx origin
// to cancelling a transaction will result in the receive buffer (and possibly 
// the entire block structure) that RSLinx was handed being deleted. This 
// will result in a crash if RSLinx ever does respond.

   SV_PLCCOMMSTATUS_ERRORDETECTED,
   
// Only SendManager () and ResponseManager () should ever set to this value.

   SV_PLCCOMMSTATUS_ERROR
};

// SVPLCBlockStruct - There should not be multiple outstanding requests for a
// single instantiation of a block.  Each block should only have one 
// outstanding request at a time.  If, for speed reasons, you wish to start 
// another request while one is still being fulfilled, then make multiple 
// copies of that block.  You may still reference the same element array and
// use the same data array to receive the incoming data. --------------------

struct SV_PLCAPI_DS SVPLCBlockStruct
{


   SVPLCBlockStruct ()
   {
      svmInternalConnectionName [0] = _T('\0');
      svmConnectionName [0] = _T('\0');
      svmTopicName [0] = _T('\0');
      svmItemName [0] = _T('\0');

      svmcpTxRxBuffer = NULL;
      svmpPreviousBlock = NULL;
      svmpNextBlock = NULL;
      svmpParamBuffer = NULL;
      svmStatus = SV_PLCCOMMSTATUS_COMPLETE;
      svmlCommunicationError = 0;
      svmlPLCDataFixedLength = 0;
      svmlPCDataFixedLength = 0;

//      svmlInUse = 0;
   };


   TCHAR       svmInternalConnectionName [SV_GENERIC_NAME_SIZE]; // size to be determined.

// FOR API-------------------------------------------------------------------------------
// DATA TABLE ADDRESS -------------------------------------------------------------------

   long        svmlPLCFileNbr;

// smnlFileType - This value refers to PLC data type that the data will be shoved into.   

   SVPLCDataTypeEnum svmPLCDataType;

// svmlPLCDataFixedLength - For now this is only being used when transferring
// a PC string data type , SV_PCTYPE_STRING, using a PLC data type other 
// than SV_PLC_STRING.

   long        svmlPLCDataFixedLength;

   long        svmlPCDataFixedLength;



   long        svmlFirstElement;

   long        svmlNbrElements;


   char        svmRebuiltDTAddress [20];
// svmDefinitionString - This string is used by the DTL_C_DEFINE function. --

   char        svmDefinitionString [256];
// End of DATA TABLE ADDRESS ------------------------------------------------

   unsigned long svmlTransactionId;

// End of FOR API -----------------------------------------------------------


// FOR DDE ------------------------------------------------------------------

   TCHAR       svmConnectionName [SV_GENERIC_NAME_SIZE];

   TCHAR       svmTopicName [SV_GENERIC_NAME_SIZE];

   TCHAR       svmItemName [SV_GENERIC_NAME_SIZE];
// End of FOR DDE -----------------------------------------------------------


   SVPCDataTypeEnum  svmPCDataType;


// svmulRawStatus - This value will be set by the callback function and will
// contain the raw iostatus returned by RSLinx. -----------------------------

   unsigned long svmulRawStatus;


   SVPLCCommStatusEnum svmStatus;

   long        svmlCommunicationError;


   SVPLCBlockStruct* svmpPreviousBlock;

   SVPLCBlockStruct* svmpNextBlock;
   

   SVPLCElementStructArray svmpIndividualElementArray;

// svmcpTxRxBuffer - pointer to a temporary buffer holding transmitted and 
// received buffers. This buffer is created, managed, and destroyed by the 
// PLC class.

   char*       svmcpTxRxBuffer;

// svmpParamBuffer - This is a pointer to the buffer which was passed in to
// a "Get" block routine. This value is then used by the 
// PLCResponseManager () to shove the data into the requested memory 
// location. This buffer this points to must be pre-allocated by the 
// programmer and must be of a size large enough to hold the requested data.

   char*       svmpParamBuffer;

//   long        svmlInUse;
};



struct SVPLCCommQueueStruct
{

// SVPLCCommQueueStruct () - The void represents SVPLCBaseClass, which would
// be a circular definition.

   SVPLCCommQueueStruct (SVPLCBlockStruct* apBlock,
                         void* apParent = NULL)
   {
      svmpBlock = apBlock;
      svmpParent = apParent;
      svmlPendingOperation = 0;
   };

//   SVPLCCommQueueStruct (SVPLCBlockStruct* apBlock,
//                         SVPLCBaseClass*   apParent)
//   {
//      svmpBlock = apBlock;
//      svmpParent = apParent;
//   };



   SVPLCBlockStruct* svmpBlock;

// 1 = READ
// 2 = WRITE

   long        svmlPendingOperation;

// svmpParent - The void represents SVPLCBaseClass, which would
// be a circular definition.
//
// At this time this value is only used by the Seimens derived PLC class.
// This value is so that the single Defer Manager thread can track which
// instance of the class sent the request.
//

   void*       svmpParent;
};

//typedef CList <SVPLCCommQueueStruct*, SVPLCCommQueueStruct*> SVPLCCommQueueStructList ;

class SVPLCCommQueueStructList : public CList <SVPLCCommQueueStruct*, SVPLCCommQueueStruct*> 
{
public:

   long  ResetParent (void* apOldParent, 
                      void* apNewParent, 
                      CMutex* QLock);


};

struct SVPLCConnectionParameters
{
// CONNECTION DATA ----------------------------------------------------------

   SVPLCManufacturerEnum svmPLCManufacturer;

   SVPLCModelEnum    svmPLCModel;

   SVPLCSubModelEnum svmPLCSubModel;

   SVPLCConnectionMethodEnum svmConnectionMethod;

   TCHAR             svmczManufacturer [SV_GENERIC_NAME_SIZE];
   TCHAR             svmczPLCType [SV_GENERIC_NAME_SIZE];
   TCHAR             svmczConnection [SV_GENERIC_NAME_SIZE];

   TCHAR             svmczDriverName [SV_GENERIC_NAME_SIZE];

   TCHAR             svmczIPAddress [SV_GENERIC_NAME_SIZE];

   TCHAR             svmczDHNodeAddress [SV_GENERIC_NAME_SIZE];

   TCHAR             svmczConnectionParameters [SV_GENERIC_NAME_SIZE];

   long				 svmlParameterStringSize;

   TCHAR             svmczChannelNbr [SV_GENERIC_NAME_SIZE];

   TCHAR             svmczEquipmentNbr [SV_GENERIC_NAME_SIZE];

// End of CONNECTION DATA ---------------------------------------------------

};

class SVTimerInterfaceClass;

// PLC base class is meant to refer to a single PLC using a single platform.
// 
// Each platform to be supported must be represented by a class derived from 
// the SVPLCBaseClass.  
//
// Currently supported platorms are... 
//    communication to an Allen-Bradley SLC504 using; 
//       direct API calls (dll function calls),
//       RSLinx, 
//       a KTX card, and 
//       a datahighway+ network.
//
//    communication to an Allen-Bradley SLC500 (series) using;
//       DDE for the data exchange, and
//       RSLinxs.
//
//
// The significant differences between direct (API) connections and DDE 
// connections are...
//    With DDE connections the majority of the configuring will come from 
//       DDE server product (outside the SVFocus range of influence).  
//       Configurations will not be as integrated.
//    With API connections, the majority of the configuring will come from 
//       SVFocus. Configurations will be part of the SVFocus configuration.
//       Standard Seidenader addresses will present themselves as defaults.
//    In general, it will be easier for the programmers to impliment a new 
//       DDE platform and more difficult for the integrators to impliment
//       a DDE platform.
//    In general, it will be more difficult for the programmers to impliment
//       a new API platform and easier for the integrators to impliment
//       an API platform.
//    For integrators who have access to PC support and specifically DDE 
//       expertise with the device driver they are attempting to connect 
//       with, the DDE connections will offer more flexabillity.
//    The API connections will offer more flexabillity for actual address 
//       assignments.
//
// It is suggested that the programmer handle his addresses as 
// SVPLCElementStruct structures and as SVPLCElementStructArray arrays.
//
// Using the PLC API:
//   The Connection:
//      Each instance of the PLC class represents a single connection to a 
//      single PLC.
//
//      The programmer must then use the Open () member to initialize the 
//      connection.
//
//      The programmer must use the Close () member function when desiring to
//      terminate a connection.  
//   
//      A connection may be closed and another connection openned without 
//      destroying the instance.
//
//   Single Element Memory Access:
//      All single element memory access will be syncronous.
//      Step 1.  The programmer must have an instance of SVPLCElementStruct.
//      Step 2.  The programmer will verify that there is a valid address 
//               entered into the SVPLCElementStruct variable.  The specific 
//               structure members which need initialized for a valid 
//               address varies depending on the communication platform.  For
//               The SLC504APIClass implimentation, the only required 
//               member is svmPLCAddress.
//      Step 3.  The programmer will call either of the single element Send, 
//               Get, And , or Or functions.
//      Note about SVPLCElementStruct:
//               Some addresses may need parsed from their initially entered 
//               form.  Parsing is handled automatically whenever one of the 
//               "Step 3" functions is called. For effitency, once an 
//               element is parsed, the structure maintains the parsed 
//               results and flags it as parsed.  If the programmer wishes to
//               reuse an element, or change an address, the programmer must 
//               change the address in the SVPLCElementStruct and then set 
//               svmlParsed to FALSE.
//
//   Block Memory Access:
//      All block memory access will be treated as asyncronous by the PLC 
//      class, recongizing that some communication platforms may only support
//      syncronous communication.
//
//      Step 1.  Create an SVPLCElementStructArray of SVPLCElementStruct 
//               elements.  
//      Step 2.  Each SVPLCElementStruct must have the desired 
//               PLC address initialized in the same manner indicated in the 
//               Single Element Memory Access description.  The PLC addresses
//               do not need to be contiguous, and do not need to reference
//               the same data file.  The PLC addresses don't even need to 
//               referece the same PLC data type, although the data type 
//               must be convertable to the data type into which you are 
//               reading.  
//               Examples:  Reading a float into an integer will result in
//               a rounded integer, reading a float into a bit is not 
//               supported and will result in an error.
//      Step 3.  The programmer must have access to a pointer to the block 
//               structure SVPLCBlockStruct.  The pointer should be 
//               initialized to NULL.
//      Step 4.  Call the member function CreateBlocks () or 
//               CreateBlocksEx (). CreateBlocks () (or CreateBlocksEx ())
//               may, in fact, create a chain of blocks.  All elements in
//               the SVPLCElementStructArray will be parsed, sorted, and 
//               organized into transfer blocks (elements in the passed in 
//               array will be modified to reflect the parsing, but will not
//               be re-orderred to reflect the sorting.  CreateBlocks () will
//               create transfer blocks and organize the elements into those 
//               blocks to optimize data transfers. The generated blocks all
//               have pointers to the original SVPLCElementStruct elements of
//               SVPLCElementStructArray, so do not destroy those elements 
//               until after you have destroyed the blocks.
//      Step 5.  The programmer will call either of the SendBlock or GetBlock 
//               functions (excluding SendBlock () and GetBlock () 
//               themselves).   No data is actually returned by these 
//               functions.  All block functions automatically process 
//               through a linked chain of blocks such as those returned by 
//               CreateBlocks ().  
//      Step 6.  Wait. The programmer must call the WaitTillComplete ()
//               function, which will then wait until the response has been 
//               received for all blocks.  When this function returns, the 
//               values referenced by block will have been updated (in the 
//               case of a read). Values will be loaded and read to/from 
//               the read or written data values based on the order of the  
//               original address array passed into CreateBlocks ().
//      Step 7.  When finished with a group of blocks, the blocks should be 
//               destroyed by calling DestroyBlocks ().
//      WARNING to the Programmer:
//               Block writes can be very dangerous unless you are in a VERY
//               controlled environment.  If you allow the operator to 
//               configure 2 write addresses, N100:1 and N100:10, and then 
//               send that array information off to CreateBlocks (), whos
//               resulting blocks will then be used to perform a Send Block
//               function, then addresses N100:2 through N100:9 will be 
//               over written with garbage, even though that might not be
//               what the operator intended (possibly destroying important
//               data. Before you even suggest that we read the existing data
//               first, and only modify the values we want to change; this 
//               would require a locking mechanism/protocol between the PC
//               and the PLC, which excedes our current scope.
//
//
// Deriving a new Communication Platform:
//    The following functions MUST be defined in a derived PLC platform 
//    class.
//
//    Public:
//       long Open ();
//         The Open () function should begin with a call to the base class 
//         Open () function.
//       long Close ();
//         The Close () function should end with a call to the base class 
//         Close () function.
//       long CheckStatus (SVPLCBlockStruct* apBlock);
//       long ParseElements (SVPLCElementStructArray* apAddress);
//       long QuickSortElements (SVPLCElementStructArray*  apSortArray,
//                               long                      alBeginning,
//                               long                      alEnd);
//       long AddElementToBlock (SVPLCBlockStruct*     apBlock,
//                               SVPLCElementStruct*   apElement);
//       long SendB (SVPLCBlockStruct* apBlock);
//       long DataTypeSize (SVPLCDataTypeEnum aDataType);
//       long GetInt16 (SVPLCBlockStruct* apBlock);
//       long GetStr (SVPLCBlockStruct* apBlock);
//       long CopyReceiveBufferToLong (SVPLCBlockStruct* apBlock);   
//       long CopyReceiveBufferToDouble (SVPLCBlockStruct* apBlock);   
//       long CopyReceiveBufferToString (SVPLCBlockStruct* apBlock);   
//    Private:
//       long CopyLongToSendBuffer (SVPLCBlockStruct* apBlock);
//       long CopyDoubleToSendBuffer (SVPLCBlockStruct* apBlock);
//       long CopyStringToSendBuffer (SVPLCBlockStruct* apBlock);
//       long InitializeBlocks (SVPLCBlockStruct* apBlock)
//       long DeinitializeBlocks (SVPLCBlockStruct* apBlock)
//          If custom block initialization is required.

class SV_PLCAPI_DS SVPLCBaseClass
{
public:

   long           SendBlock (SVPLCBlockStruct* apBlock);

// SendBit () ---------------------------------------------------------------
// Will set the bit level PLC address specified by apAddress to 1 or 0 based 
// on the value in alValue. An alValue of 0 will clear the bit and an alValue
// of non 0 will set the bit.  This will allow logically anded values to set
// the bit, regardless of the bit position. 
//   
// Specific implimentation may vary based on the PLC. Where bit level access 
// is supported by the PLC, this function may simply send the single bit.
// Where bit level access is derived from integer access, we will read the 
// values first, do a logical "and" or "or" as appropriate, and then write 
// the values. 
//   
// If a bit level address is not specified, the result is undefined.  The bit
// mask will propably still be at whatever the initialized value is. --------

   virtual long   SendBit (long alValue, SVPLCElementStruct* apAddress);

	//Sets the PLC address specified apAddress to the long 
	//integer value supplied in alValue.

   virtual long   SendInt (long alValue, SVPLCElementStruct* apAddress);

	//Sets the PLC address specified apAddress to the double 
	//value supplied in adValue.

   virtual long   SendFloat (double adValue, SVPLCElementStruct* apAddress);

	//Sets the PLC address specified apAddress to the string 
	//value supplied in aptczValue for alFixedLength bytes.

   virtual long   SendString (char*               aptczValue, 
                              long                 alFixedLength, 
                              SVPLCElementStruct*  apAddress);

// SendBitBlock () - 
// Must be supported in derived class.

   virtual long   SendBitBlock (long* aplValueArray, SVPLCBlockStruct* apBlock);

// All "valueArray"'s which are passed into the block functions must be large enough to 
// accomodate the number of elements as specified by SVPLCBlockStruct::svmNbrElements.

   virtual long   SendIntBlock (long* aplValueArray, SVPLCBlockStruct* apBlock);


   // This interface is used because SVObserver uses LPSTR strings.
   // we will do the conversion in the base and then call the standard sendIntBlock.
   virtual HRESULT SendIntBlock( char** p_apstrAddresses, long* p_aplValues, long p_lSize);

   virtual HRESULT SendStringBlock( char** p_psAddresses, char** p_psValues, long p_lSize);


   virtual long   SendFloatBlock (double* apfValueArray, SVPLCBlockStruct* apBlock);

   virtual long   SendStringBlock (char**             aptczValueArray, 
                                   long               alFixedLength, 
                                   SVPLCBlockStruct*  apBlock);
// All single element get functions will always be syncronous.  This is 
// because at this level there is no access to the block which is managing
// the transaction, so there is no way to track the transaction.

   virtual long   GetInt (long* aplValue, SVPLCElementStruct* apAddress);
// All single element send functions will always be syncronous.  This is 
// because at this level there is no access to the block which is managing
// the transaction, so there is no way to track the transaction.

   virtual long   GetLong (long* aplValue, SVPLCElementStruct* apAddress);
// All single element send functions will always be syncronous.  This is 
// because at this level there is no access to the block which is managing
// the transaction, so there is no way to track the transaction.

   virtual long   GetFloat (double* apdValue, SVPLCElementStruct* apAddress);
// All single element send functions will always be syncronous.  This is 
// because at this level there is no access to the block which is managing
// the transaction, so there is no way to track the transaction.

   virtual long   GetString (TCHAR* aptczValue,  
                             long alFixedLength,
                             SVPLCElementStruct* apAddress);

// GET BLOCK FUNCTIONS ------------------------------------------------------
// Depending on the actual communication platform, the process for reading 
// blocks may be syncronous or asyncronous; but it will always be handled as
// though it were asyncronous. The "Get" function will make the request for 
// data which will not necessarly be fulfilled upon returning from the "Get"
// function.  The programmer should check the status of a particular block
// (svmStatus) to determine whether or not the data has been transferred.
// Keep in mind that the apBlock reference for a single "Get" may be 
// referencing a chain of blocks, each of which will need its status 
// checked.  A receiving thread will automatically place the data in the 
// receive buffer when the data is received. --------------------------------


   virtual long   GetIntBlock (long*               valueArray, 
                               SVPLCBlockStruct*   apBlock);

   virtual long   GetLongBlock (long*               valueArray, 
                               SVPLCBlockStruct*   apBlock);

   virtual long   GetFloatBlock (double*           apfValueArray, 
                                 SVPLCBlockStruct* apBlock);

   virtual long   GetStringBlock (TCHAR*           aptcValueArray, 
                                  int              aFixedLength,
                                  SVPLCBlockStruct* apBlock);

// GetBitBlock () - 
// Must be supported in derived class.

   virtual long   GetBitBlock (long* aplValueArray, SVPLCBlockStruct* apBlock);



   virtual long   GetInt32 (SVPLCBlockStruct* apBlock);
// GetStr () - Not to be confused with GetString (). GetString () is meant to
// be used by the programmer as part of the class interface.  GetStr () 
// should not be used by the programmer.

   virtual long   GetStr (SVPLCBlockStruct* apBlock);

   virtual long   GetASCII (SVPLCBlockStruct* apBlock);

   virtual long   GetSPFloat (SVPLCBlockStruct* apBlock);

   virtual long   GetDPFloat (SVPLCBlockStruct* apBlock);
// End of GET BLOCK FUNCTIONS. ----------------------------------------------


   long           WaitTillComplete (SVPLCBlockStruct* apBlock);

   long           GetBlock (SVPLCBlockStruct* apBlock);

	//Logically 'OR's the supplied integer value with the 
	//integer value located at apAddress in the PLC and 
	//stores the resulting value in at apAddress in the PLC.

   virtual long   OrInt (long* aplValue, SVPLCElementStruct* apAddress);
	//Logically 'AND's the supplied integer value with the 
	//integer value located at apAddress in the PLC and 
	//stores the resulting value in at apAddress in the PLC.

   virtual long   AndInt (long* aplValue, SVPLCElementStruct* apAddress);

// UNSUPPORTED FUNCTIONS ----------------------------------------------------
// The following functions are not supported in the base class, but will be 
// defined returning an "unsupported" error message in case the dirived class
// does not have a replacement for that function. ---------------------------


   virtual long   SendB (SVPLCBlockStruct* apBlock);
	//

   virtual long   SendInt16 (SVPLCBlockStruct* apBlock);

   virtual long   SendInt32 (SVPLCBlockStruct* apBlock);

   virtual long   SendStr (SVPLCBlockStruct* apBlock);

   virtual long   SendASCII (SVPLCBlockStruct* apBlock);

   virtual long   SendSPFloat (SVPLCBlockStruct* apBlock);

   virtual long   SendDPFloat (SVPLCBlockStruct* apBlock);

   virtual long   GetB (SVPLCBlockStruct* apBlock);

   virtual long   GetInt16 (SVPLCBlockStruct* apBlock);

   virtual long   TestCommunications();

// USED BY PLCSendManager () and PLCResponseManager () ----------------------

   virtual long   CopyReceiveBufferToLong (SVPLCBlockStruct* apBlock) = 0;   

   virtual long   CopyReceiveBufferToFloat (SVPLCBlockStruct* apBlock) = 0;   

   virtual long   CopyReceiveBufferToString (SVPLCBlockStruct* apBlock) = 0;

   virtual long   GetMaxDTSize( void );
	//The SetupConnection (szConnection) establishes the 
	//connection specified by szConnection.
	//
	//Control Component Implementation: Determine the desired 
	//underlying protocol, load the boundary component that 
	//implements that protocol, then re-direct the remaining 
	//control component member functions to the coinciding 
	//functions in the boundary component.
	//
	//Boundary Component Implementation: Obtains the protocol 
	//specific values necessary to establish a connection.
	//
	//If the connection specified by szConnection does not 
	//exist, SetupConnection (szConnection) calls 
	//SetupConnection().

//	long           SetupConnection(TCHAR* apszConnection, long alSize);
//TRB Move SetupConnection

   TCHAR          svmInternalConnectionName [SV_GENERIC_NAME_SIZE]; // size to be determined.



/*
   static long    GetValue (TCHAR*   apczSourceString, 
                            TCHAR*   apczKeyString, 
                            TCHAR*   apValueString,
                            long     alLength);
*/

//   long           ParseSetupParameterString (TCHAR* apszConnection);
   
//   SVPLCConnectionClass*
//               svmInternalConnection;


SVPLCConnectionParameters  m_ConnectionData;


   SVPLCBlockStruct  svmDefaultBlock;

// SVPLCBaseClass -----------------------------------------------------------
// After instantiating the PLC class, the programmer must call the 
// Initialize () function. --------------------------------------------------
	SVPLCBaseClass (long* aplErr);

// Initialize () ------------------------------------------------------------
// This virtual function must be called after the instantiation of the class.
   virtual long Initialize ();

   long         IsInitialized ();

	virtual ~SVPLCBaseClass();



   virtual long Open (TCHAR *apConnectionString) = 0;

	//(** base class implementation does nothing **)

   virtual long Close ();


   long     SetTimeOutLimit (__int64 aTimeOutLimit);


   long     CancelTransaction (SVPLCBlockStruct* apBlock, long aErr);

// End of UNSUPPORTED FUNCTIONS. --------------------------------------------



   virtual long   CheckStatus (SVPLCBlockStruct* apBlock) = 0;



   long           CopyElementAddressToBlock (SVPLCBlockStruct* apDestBlock,
                                          SVPLCElementStruct *apElement);

   long           CopyBlockConnectionData (SVPLCBlockStruct* apDestBlock, 
                                        SVPLCBlockStruct* apSourceBlock);

   long           SortElements (SVPLCElementStructArray* apAddress);
// DataTypeSize () - This function should be overridden by the dirived class.
// If not overridden by the dirived class, this function will return error 
// number -1158, indicating that the function is not supported.  When
// successful, this function should return the number of bytes required by 
// the data type. If the data type requested is not supported by the 
// instantiated PLC type, a negative error value should be returned. --------

   virtual long   DataTypeSize (SVPLCDataTypeEnum aDataType);
	//Destroys blocks created by CreateBlocks.

   long           DestroyBlocks (SVPLCBlockStruct** appBlock);
	//Creates a linked list of SVPLCBlockStructs containing 
	//the SVPLCElementStructArray passed in to the function 
	//and initializes those blocks by calling 
	//InitializeBlocks.

   long           CreateBlocks (SVPLCBlockStruct** apBlock, SVPLCElementStructArray* apAddressArray);

// CreateBlocksEx () - Creates a linked list of SVPLCBlockStructs 
// containing the SVPLCElementStructArray passed in to the function 
// and initializes those blocks by calling 
// InitializeBlocks. 
//
// This function can be used in place of CreateBlocks () when a "complex"
// data structure is being used.  
//
// A "complex" data type will be defined, in this instance, as a case where
// a single PLC address may need to reference multiple PLC elements in order
// to be translated into a PC data type.  An example of this would be 
// translating a PLC integer type into a PC string.  Although only one 
// PLC integer address is specified, it will require multiple integer 
// elements in order to read a string. --------------------------------------

   long           CreateBlocksEx (SVPLCBlockStruct** apBlock, 
                                  SVPLCElementStructArray* apAddressArray, 
                                  long alPLCFixedDataLength);

	// InitializeBlocks () - This gives the derived class the 
	//opportunity to
	// initialize the blocks if they need to.  If you are 
	//deriving from a derived
	// class, you may need to call your class's base class 
	//InitializeBlocks () 
	// function. (** base class implementation does nothing 
	//**)

   virtual long   InitializeBlocks (SVPLCBlockStruct* apBlock);

// DeinitializeBlocks () - This gives the derived class the opportunity to
// initialize the blocks if they need to.  If you are deriving from a 
// derived class, you may need to call your class's base class 
// DeinitializeBlocks () function. (** base class implementation does 
// nothing **)

   virtual long   DeinitializeBlocks (SVPLCBlockStruct* apBlock);

// The format of the outgoing buffer will depend on the PLC/Connection type.

   virtual long   CopyLongToSendBuffer (SVPLCBlockStruct* apBlock) = 0; 

   virtual long   CopyDoubleToSendBuffer (SVPLCBlockStruct* apBlock) = 0; 

   virtual long   CopyStringToSendBuffer (SVPLCBlockStruct* apBlock) = 0;
// UNSUPPORTED FUNCTIONS ----------------------------------------------------
// The following functions are not supported in the base class, but will be 
// defined returning an "unsupported" error message in case the dirived class
// does not have a replacement for that function. ---------------------------


   virtual long   ParseElements (SVPLCElementStructArray* apAddress);

   virtual long   QuickSortElements (SVPLCElementStructArray*  apSortArray,
                                     long                      alBeginning,
                                     long                      alEnd);


   virtual long   AddElementToBlock (SVPLCBlockStruct*     apBlock,
                                     SVPLCElementStruct*   apElement);

// End of UNSUPPORTED FUNCTIONS. --------------------------------------------


   long        svmlIsOpen;

   static long svmlNbrOpen;

   static CMutex svmNbrOpenLock;

   static long svmlNbrInstances;

   static CMutex svmNbrInstancesLock;

// USED BY SendManager () and ResponseManager () ----------------------------

   long        svmlStopSM;

   long        svmlStopRM;

   CMutex      SVSendQLock;

   SVPLCCommQueueStructList svmSendQueue;


   CMutex      SVPendingQLock;

   SVPLCCommQueueStructList svmPendingQueue;
// End of USED BY SendManager () and ResponseManager () ---------------------

// DEPENDENT DLLS -----------------------------------------------------------
// Handles for independantly loaded (DelayLoad/LoadLibrary) DLLs used by
// SVPLCBaseClass.
//
// General comments from Jim.  During this integration I have adopted the  
// opinion that there realy does need to be a supervisory (control) class in 
// this DLL which would handle things like loading of the DLLs.  Since this 
// not so yet, these pointers will exist in the base class.

   HINSTANCE   svmhRSLinxLib;
   HINSTANCE   svmhApplicomLib;
// End of DEPENDENT DLLS. ---------------------------------------------------


// UTILLITIES ---------------------------------------------------------------
// SwapBytes () - Will swap high byte, low byte of a number of words equal to 
// alNumberOfWords.

   long SwapBytes (char*   apcDestination, 
                   char*   apcSource, 
                   long    alNumberOfWords);

// End of UTILLITIES. -------------------------------------------------------


//protected:

   __int64     svmTimeOutLimit;        // milliseconds

// svmlInitialized ----------------------------------------------------------
// This value is initialized to FALSE by the constructor, and is then set to 
// TRUE by the Initialized () function. This value can be read by calling 
// IsInitialized () ---------------------------------------------------------
   long        svmlInitialized;

   DWORD       svmdwSendThreadId;

   DWORD       svmdwResponseThreadId;


   HANDLE      svmhSendThread;

   HANDLE      svmhResponseThread;

// Open () ------------------------------------------------------------------
// The base class implimentation should never be called directly from outside 
// the derived class. -------------------------------------------------------
   //(** base class implementation does nothing **)

   long        Open ();

// TerminateSendAndResponseThreads () ---------------------------------------
// Called in base class destructor.  Can be called in derived class 
// destructor if necessary (ex. SVS7PBFMSClass). ----------------------------

   long        TerminateSendAndResponseThreads ();

};



DWORD WINAPI PLCSendManager (LPVOID apCallingClass);
DWORD WINAPI PLCResponseManager (LPVOID apCallingClass);

#define SV_NBRCHARS(x)  sizeof (x) /  sizeof (TCHAR)

int	sv_strncpy (char		   *destString,
				     const char	*sourceString,
				     int			   nbrOfBytes);

// sv_tstrncpy () - This function will copy the number of TCHARs specified by
// nbrOfBytes, from sourceString to destString and will always NULL terminate
// the string.
//  
// NOTE: This value will usually be gotten from the "size" allocation of the 
// destination. If gotten from the actual string length, always remember to 
// add 1 for the NULL terminator.
int	SV_PLCAPI_DS sv_tstrncpy (TCHAR		   *destString,
			                       const TCHAR	*sourceString,
				                    int			   nbrOfBytes);

#pragma pack( pop )

#endif // ifndef SVPLCBaseClass_H_INCLUDED

// ******************************************************************************
// * LOG HISTORY:
// ******************************************************************************
/*
$Log:   N:\PVCSarch65\ProjectFiles\archives\SVObserver_src\SVP_SRC\PLCBaseClass.h_v  $
 * 
 *    Rev 1.0   25 Apr 2013 15:32:52   bWalter
 * Project:  SVObserver
 * Change Request (SCR) nbr:  814
 * SCR Title:  Upgrade SVObserver to Compile Using Visual Studio 2010
 * Checked in by:  bWalter;  Ben Walter
 * Change Description:  
 *   Initial check in to SVObserver_src.  (Merged with svo_src label SVO 6.10 Beta 008.)
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.1   07 Sep 2012 11:01:14   tbair
 * Project:  SVObserver
 * Change Request (SCR) nbr:  783
 * SCR Title:  Update SVObserver to Validate PLC Sub-system Before Entering a Running Mode
 * Checked in by:  tBair;  Tom Bair
 * Change Description:  
 *   Added Method to test communications before going online.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.0   14 Nov 2008 14:55:28   tbair
 * Project:  SVObserver
 * Change Request (SCR) nbr:  615
 * SCR Title:  Integrate PLC Classes into SVObserver Outputs
 * Checked in by:  tBair;  Tom Bair
 * Change Description:  
 *   Initial Check-in
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.5   13 Nov 2001 15:18:38   jBrown
 * Project:  SVFocusNT
 * Change Request (SCR) nbr:  142
 * SCR Title:  Remove thread spawning from within constructors
 * Checked in by:  JimAdmin;  James A. Brown
 * Change Description:  
 *   The functions
 * 
 * SVPLCBaseClass::Initialize (), and 
 * SVPLCBase:Class::IsInitialized ()
 * 
 * were added to separate class construction from class initialization.
 * 
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.4   01 Feb 2001 10:05:04   Jim
 * Project:  SVFocusNT
 * Change Request (SCR) nbr:  83
 * SCR Title:  Integrate string handling for Siemens PLCs.
 * Checked in by:  JimAdmin;  James A. Brown
 * Change Description:  
 *   SVPLCCommQueueStruct was modifed to include parent PLC class.
 * 
 * SVPLCCommQueueStructList was modified to include ResetParent ().
 * 
 * SVPLCBaseClass was modified to include static members for tracking the number of instances, and the TerminateSendAndResponseThreads () function.
 * 
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.3   18 Dec 2000 19:22:16   Jim
 * Project:  SVFocusNT
 * Change Request (SCR) nbr:  78
 * SCR Title:  Incorporate reading of strings from the PLC.
 * Checked in by:  JimAdmin;  James A. Brown
 * Change Description:  
 *   #define SV_MAX_SLC500_ST_LENGTH 84
 * was added to give a common definition of the Allen-Bradley PLC string data type size.
 * 
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.2   08 Dec 2000 16:33:54   Jim
 * Project:  SVFocusNT
 * Change Request (SCR) nbr:  78, 77
 * SCR Title:  Incorporate reading of strings from the PLC.
 * Checked in by:  JimAdmin;  James A. Brown
 * Change Description:  
 *   The following structures have been modfied in order to incorporate reading strings from the PLC:
 *   SVPLCBlockStruct
 *   SVPLCBaseClass
 * 
 * The following structures have been modfied in order to incorporate direct PLC bit addressing:
 *   SVPLCBaseClass
 * 
 *   
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.1   12 Sep 2000 13:52:36   Jim
 * Project:  SVFocusNT
 * Change Request (SCR) nbr:  14
 * SCR Title:  Stage 2: Expand PLC Support
 * Checked in by:  Jim;  James A. Brown
 * Change Description:  
 *   Extensive changes to support integration.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.0   Mar 22 2000 17:32:18   mike
 * Initial revision.
*/