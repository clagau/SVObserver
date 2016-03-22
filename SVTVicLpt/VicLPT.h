// ******************************************************************************
// * COPYRIGHT (c) 2005 by SVResearch, Harrisburg
// * All Rights Reserved
// ******************************************************************************
// * .Module Name     : VicLPT
// * .File Name       : $Workfile:   VicLPT.h  $
// * ----------------------------------------------------------------------------
// * .Current Version : $Revision:   1.0  $
// * .Check In Date   : $Date:   25 Apr 2013 19:10:00  $
// ******************************************************************************

//====================================================================
//==============            TVicLpt 1.3               ================
//==            Declaration of the TVicLPT Ñ++ class                ==
//==========   Copyright (c) 2001,2002 Victor Ishikeev   =============
//==============    mailto: tools@entechtaiwan.com    ================
//============   http://www.entechtaiwan.com/tools.htm   =============
//====================================================================

#pragma once

#define MAX_SHARED_BUFFER_LENGTH  255

#define  MaxMappedAreas            16
#define  MaxPins                   17

#define LPTACCESS_ERR_NONE             0
#define LPTACCESS_ERR_BAD_PORT         1
#define LPTACCESS_ERR_ACQUIRE_REFUSED  2

#define LPT_NOT_PRESENT          0
#define LPT_SPP_MODE             1
#define LPT_PS2_MODE             2
#define LPT_EPP_MODE             3
#define LPT_FAST_CENTRONIX       4
#define LPT_ECP_MODE             5

#define COMPATIBILTY_TRANSFER_MODE 0
#define NIBBLE_TRANSFER_MODE       1
#define BYTE_TRANSFER_MODE         2
#define EPP_TRANSFER_MODE          3
#define ECP_TRANSFER_MODE          4

typedef struct block {
	ULONG start12;
	ULONG checksum;
	ULONG len;

    char buff[65535];

} DATA_BLOCK_LPT, *PDATA_BLOCK_LPT;


#define EOL "\n"


#define F_D_P  0x00008000

#ifdef  DEMOVERSION
	#define TVicLPT_DEBUG
	#define W95_CTL_CODE_2( DeviceType, Function, Method, Access ) ((Function+2) & 0x000000ff)
	#define CTL_CODE_2( DeviceType, Function, Method, Access )         \
    (((DeviceType) << 16) | ((Access) << 14) | (((Function)+2) << 2) | (Method) )
#else
	#define W95_CTL_CODE_2( DeviceType, Function, Method, Access ) (Function & 0x000000ff)
	#define CTL_CODE_2 CTL_CODE
#endif

#define MAP_MEMORY        CTL_CODE_2(F_D_P, 0x802, METHOD_BUFFERED, FILE_ANY_ACCESS)
#define UNMAP_MEMORY      CTL_CODE_2(F_D_P, 0x803, METHOD_BUFFERED, FILE_ANY_ACCESS)
#define LOCK_MEMORY       CTL_CODE_2(F_D_P, 0x804, METHOD_BUFFERED, FILE_ANY_ACCESS)
#define UNLOCK_MEMORY     CTL_CODE_2(F_D_P, 0x805, METHOD_BUFFERED, FILE_ANY_ACCESS) 

#define GET_IRQ_COUNTER   CTL_CODE_2(F_D_P, 0x806, METHOD_BUFFERED, FILE_ANY_ACCESS)
#define MASK_INT_VEC      CTL_CODE_2(F_D_P, 0x807, METHOD_BUFFERED, FILE_ANY_ACCESS)
#define UNMASK_INT_VEC    CTL_CODE_2(F_D_P, 0x808, METHOD_BUFFERED, FILE_ANY_ACCESS)
#define PULSE_EVENT       CTL_CODE_2(F_D_P, 0x809, METHOD_BUFFERED, FILE_ANY_ACCESS)
#define ACQUIRE_LPT       CTL_CODE_2(F_D_P, 0x824, METHOD_BUFFERED, FILE_ANY_ACCESS)
#define RELEASE_LPT       CTL_CODE_2(F_D_P, 0x825, METHOD_BUFFERED, FILE_ANY_ACCESS)

#define CLEAR_LIOPM       CTL_CODE_2(F_D_P, 0x826, METHOD_BUFFERED, FILE_ANY_ACCESS )
#define SET_RANGE         CTL_CODE_2(F_D_P, 0x827, METHOD_BUFFERED, FILE_ANY_ACCESS )
#define GET_LIOPMA        CTL_CODE_2(F_D_P, 0x828, METHOD_BUFFERED, FILE_ANY_ACCESS )
#define ACTIVATE_KIOPM    CTL_CODE_2(F_D_P, 0x829, METHOD_BUFFERED, FILE_ANY_ACCESS )
#define DEACTIVATE_KIOPM  CTL_CODE_2(F_D_P, 0x82A, METHOD_BUFFERED, FILE_ANY_ACCESS )
#define QUERY_KIOPM       CTL_CODE_2(F_D_P, 0x82B, METHOD_BUFFERED, FILE_ANY_ACCESS )

#pragma pack(1)

//=============================

typedef struct _TPortRange {
  ULONG BeginPort;
  ULONG EndPort;
  ULONG Enable;
} TPortRange, * pPortRange;

typedef struct _TSRec {
	ULONG LoTimeStamp;
	ULONG HiTimeStamp;
	UCHAR DataValue;
	UCHAR StatusValue;
	UCHAR EppDataValue;
	UCHAR Rezerved;
} TSRec, *pTSRec;


typedef struct _SharedBuffer {
	ULONG IrqCounter;
	USHORT  Rezerved;
	USHORT  L_Buff;
	USHORT  N_Add;
	USHORT  N_Sel;
	TSRec Buff[MAX_SHARED_BUFFER_LENGTH+1];
} SharedBuffer, * pSharedBuffer;


typedef struct _LockedBuffer {
	pSharedBuffer pBuf;
	ULONG         BufferSize;
	ULONG         NumLockedPages;
	pSharedBuffer pInternalLockedAddress;
} LockedBuffer, * pLockedBuffer;


typedef struct _INT_VEC {

	ULONG  IRQNumber;
	HANDLE hUserEvent;   
	pSharedBuffer pKernelSharedBuffer;
	ULONG  IrqShared;

	UCHAR ClearIrq;        // 1 - Irq must be cleared, 0 - not
	UCHAR TypeOfRegister;  // 0 - memory-mapped register, 1 - port
	UCHAR WideOfRegister;  // Wide of register : 1 - Byte, 2 - Word, 4 - Double Word
	UCHAR ReadOrWrite;     // 0 - read register to clear Irq, 1 - write
	ULONG RegBaseAddress;  // Memory or port i/o register base address to clear
    ULONG RegOffset;       // Register offset
	ULONG ValueToWrite;    // Value to write (if ReadOrWrite=1)

} INT_VEC, * PINT_VEC;


typedef HANDLE (__stdcall * TOpenVxDHandle)(HANDLE); 

const USHORT MaskUp[]   = {0x0001,0x0002,0x0004,0x0008,0x0010,0x0020,0x0040,0x0080};
const USHORT MaskDown[] = {0x00FE,0x00FD,0x00FB,0x00F7,0x00EF,0x00DF,0x00BF,0x007F};

const BOOL PinsPort[] = {2,0,0,0,0,0,0,0,0,1,1,1,1,2,1,2,2};
const BOOL Negative[] = 

	{TRUE,                             // -STROBE, pin 1,     Base+2,
	 FALSE,FALSE,FALSE,FALSE,FALSE,FALSE,FALSE, FALSE,//  DATA,    pins 2..9, Base+0
     FALSE,                            //  ACKWL,  pin 10,    Base+1
     TRUE,                             //  BUSY,   pin 11,    Base+1
     FALSE,                            //  PE,     pin 12,    Base+1
     FALSE,                            //  SLCT,   pin 13,    Base+1
     TRUE,                             // -AUTOFD, pin 14,    Base+2
     FALSE,                            // -ERROR,  pin 15,    Base+1
     FALSE,                            //  INIT,   pin 16,    Base+2
     TRUE};                            // -SLCTIN  pin 17,    Base+2

const UCHAR MaskPins[] =
	{0x01,                             // -STROBE,  pin 1,     Base+2,
     0x01,							   //  DATA0,   pin 2,     Base+0
	 0x02,							   //  DATA1,   pin 3,     Base+0
	 0x04,							   //  DATA2,   pin 4,     Base+0
	 0x08,						       //  DATA3,   pin 5,     Base+0
	 0x10,							   //  DATA4,   pin 6,     Base+0
	 0x20,							   //  DATA5,   pin 7,     Base+0
	 0x40,							   //  DATA6,   pin 8,     Base+0
	 0x80,							   //  DATA7,   pin 9,     Base+0
     0x40,                             // -ACKWL,   pin 10,    Base+1
     0x80,                             //  BUSY,    pin 11,    Base+1
     0x20,                             //  PE,      pin 12,    Base+1
     0x10,                             //  SLCT,    pin 13,    Base+1
     0x02,                             // -AUTOFD,  pin 14,    Base+2
     0x08,                             // -ERROR,   pin 15,    Base+1
     0x04,                             //  INIT,    pin 16,    Base+2
     0x08};                            // -SLCTIN,  pin 17,    Base+2





typedef struct _SaveLptState {
  
    bool             sSaved;
    UCHAR            sMaxLptMode;
    UCHAR            sCurrLptMode;
    UCHAR            sLptReadMode;
    UCHAR            sEPPEnabled;
    UCHAR            sLptAcquired;
    UCHAR            sDataReadPorts[11];
    UCHAR            sDataWritePorts[11];

} TSaveLptState, *pSaveLptState;


//===========Find LPT ===========================================================
const unsigned long	ResType_IO    = 0x00000002;      // Physical IO address resource.
const unsigned long	ResType_DMA   = 0x00000003;      // DMA channels 0-7 resource.
const unsigned long	ResType_IRQ   = 0x00000004;      // IRQ 0-15 resource.

struct  TPort95Config
{
    ULONG LengthRecord;
    ULONG TypeRecord;   // ResType_IO   = $00000002;
    USHORT IOD_Count;
    USHORT IOD_Type;
    USHORT IOD_Alloc_Base;
    USHORT IOD_Alloc_End;
    USHORT IOD_DesFlags;
    UCHAR IOD_Alloc_Alias;
    UCHAR IOD_Alloc_Decode;
    USHORT IOR_Align;		// Mask for base alignment
    USHORT IOR_nPorts;	// Number of ports
    USHORT IOR_Min;		// Min port address
    USHORT IOR_Max;		// Max port address
    USHORT IOR_RangeFlags;// Flags
    UCHAR IOR_Alias;		// Alias offset
    UCHAR IOR_Decode;	// Address specified
};

struct TDMA95Config 
{
	ULONG LengthRecord;
	ULONG TypeRecord;   // ResType_DMA   = $00000003;
	UCHAR DD_Flags;
	UCHAR DD_Alloc_Chan;	// Channel number allocated
	UCHAR DD_Req_Mask;
	UCHAR DD_Reserved;
};

struct TIRQ95Config
{
	ULONG LengthRecord;
	ULONG TypeRecord;	// ResType_IRQ   = $00000004;
	USHORT IRQD_Flags;
	USHORT IRQD_Alloc_Num;// Allocated IRQ number
	USHORT IRQD_Req_Mask;
	USHORT IRQD_Reserved;
};

//========================================================================
//======================    Windows NT/2000/XP   =========================
//========================================================================

const unsigned char CmResourceTypePort             =  1;
const unsigned char CmResourceTypeInterrupt        =  2;
const unsigned char CmResourceTypeMemory           =  3;
const unsigned char CmResourceTypeDma              =  4;

typedef long PHYSICAL_ADDRESS[2];

struct TPortResource_NT
{
    PHYSICAL_ADDRESS Start;
    ULONG Length;
};

struct TInterruptResource_NT
{
    ULONG Level;
    ULONG Vector;
    ULONG Affinity;
};

struct TDMAResource_NT
{
    ULONG Channel;
    ULONG PortAddr;
    ULONG Reserved1;
};

struct CM_PARTIAL_RESOURCE_DESCRIPTOR
{
    UCHAR TypeResource;		// Specifies type of resource
    UCHAR ShareDisposition;  // Specifies how this resourse can be shared
    USHORT Flags;				// Specifies type-dependent information

	union
	{
		TPortResource_NT PortResource;
		TInterruptResource_NT InterruptResource;
		TDMAResource_NT DMAResource;
	};
};

struct CM_PARTIAL_RESOURCE_LIST
{
    USHORT Version;           // The version number of this structure.
    USHORT Revision;          // The revision of this structure.
    ULONG  Count;			// The number of partial resource descriptors
    CM_PARTIAL_RESOURCE_DESCRIPTOR PartialDescriptors[8];
};

struct CM_FULL_RESOURCE_DESCRIPTOR
{
    ULONG InterfaceType;	// Specifies the bus interface type
    ULONG BusNumber;		// The zero-based number for this bus
    CM_PARTIAL_RESOURCE_LIST PartialResourceList;
};

struct CM_RESOURCE_LIST
{
    ULONG Count;         // The number of full resource descriptors
    CM_FULL_RESOURCE_DESCRIPTOR List[8];
};


//===========================================================================
//============    TFindLpt Class Declarations      ==========================
//===========================================================================

class TFindLpt

{
private: 

	BOOL m_Win95;
	BOOL m_NT5;
	char m_ClassN[256];
    USHORT m_Devices;
    UCHAR m_IRQCounter;
	UCHAR m_IRQNumber;
    UCHAR m_DMACounter;
    UCHAR m_DMAChannel; 
    UCHAR m_PortCounter;
    USHORT m_PortBase[2];      
    USHORT m_PortEnd[2];

    BOOL SearchLpt_9X();
    BOOL  SearchLpt_NT();

public:
	TFindLpt();
    BOOL SearchLpt(UCHAR Port_Number);
	UCHAR GetIRQCounter(); 
    UCHAR GetIRQNumber();
	UCHAR GetDMACounter();
    UCHAR GetDMAChannel(); 
	UCHAR GetPortCounter();
    USHORT GetPortBase(UCHAR Index); 
	USHORT GetPortEnd(UCHAR Index); 
};





//////////////////////////////////////////////////
// TVicLPT : See VicLPT.cpp for implementation.

class TVicLPT

{
	

public:

	TVicLPT();
	~TVicLPT();


protected:

	
		
    HANDLE     m_hDrv;
	SC_HANDLE  m_schSCManager;
	HMODULE    m_hDll;
	ULONG      m_LastDebugCode;
    
	BOOL       m_Win95;
    BOOL       m_NT5;

	TSaveLptState m_SaveArea[8];

	UCHAR       m_LPTNumber;
    UCHAR       m_LPTs;
	USHORT       m_LPTBasePort;
	USHORT       m_EcpBasePort;
	int        m_MaxLptMode;
	int        m_CurrLptMode;

	USHORT       m_LPTAddresses[8];
	USHORT       m_EcpAddresses[8];
	
		//===  Added with FindLpt ===
    UCHAR m_LPTIrq[8];
    UCHAR m_LPTDMA[8];
    BOOL m_Present[8];
    BOOL m_LPTPresent;
    TFindLpt m_FindLPT;
    UCHAR m_DMAChannel;
		//===  end ===

	USHORT       m_LPTReadModes[8];
	
	UCHAR       m_DataWritePorts[11];
	UCHAR       m_DataReadPorts[11];
    
    USHORT     m_LptAcquired; // 0 - free; 1 - by us; 2 - by other
	HANDLE     m_hLptAcquired;
	USHORT     * m_MappedLPTArea;

	BOOL       m_BiosMethod;
	BOOL       m_Registry;
	BOOL       m_Started;

	USHORT      m_TransferDirection; // 0 - none
	                                // 1 - Output
	                                // 2 - Input

    USHORT      m_UseIrq;
	TOpenVxDHandle m_OpenVxDHandle;

    BOOL	 m_Terminated;
		
    pSharedBuffer m_pLockedMemory;
	pSharedBuffer m_pInternalLockedMemory; 

	PVOID    m_OnHwInterrupt;
	ULONG    m_IrqShared;
	USHORT   m_IrqNumber;
    HANDLE	 m_LocEvent;
	HANDLE	 m_KernelEvent;
	HANDLE   m_EventIRQSet;
		
    BOOL	 m_HardAccess;
    BOOL     m_MemoryMapped;
    USHORT    m_OpenDrive;
	USHORT    m_DriverInstance;


	ULONG    m_MainDebugCode;

    
	BOOL     m_Masked;
	HANDLE	 m_ThreadId;
    HANDLE	 m_ThreadHandle;
    
	ULONG    m_LastHandled;
	ULONG    m_LastCounter;

	USHORT    m_TransferMode;
    
	//====================================
	//===     LPT Registers table      ===
	//====================================

    // 0  - DataReg
	// 1  - StatusReg
	// 2  - ControlReg
	// 3  - EPP_Address
	// 4  - EPP_Data
	// 5  - EPP_Data1
	// 6  - EPP_Data2
	// 7  - EPP_Data3
	// 8  - ECP_FIFO (ECP_CfgA) (Base+400)
	// 9  - ECP_CfgB            (Base+401)
    // 10 - ECP_ECR             (Base+402)

	USHORT   m_EPPEnabled;
   
	BOOL InstallDriver();
    BOOL StartDriver() ;
    BOOL StopDriver() ;
    BOOL RemoveDriver() ;
    void CloseStopUnloadDriver() ;
    void InstallStartLoadDriver(USHORT DriverInstance) ;
	USHORT OpenTVicLPT(USHORT DriverInstance);
	void  CloseTVicLPT();

	void  SetLptIoAccess( UCHAR LptNumber );


	UCHAR  GetPort  (USHORT PortAddress);
    void   SetPort  (USHORT PortAddress, UCHAR nNewValue);
    USHORT GetPortW (USHORT PortAddress);
    void   SetPortW (USHORT PortAddress, USHORT nNewValue);
    ULONG  GetPortL (USHORT PortAddress);
    void   SetPortL (USHORT PortAddress, ULONG nNewValue);

    UCHAR   BitRead   (UCHAR PortData, UCHAR nBit);

	UCHAR   ReadEppTimeoutBit();
	BOOL   TestForECP();
	BOOL   TestForPS2();
	BOOL   TestForSPP();
	USHORT  TestForEPP();

	void   SaveLptState( UCHAR nLpt );
    bool   RestoreLptState ( UCHAR nLpt);



// methods

public:


	    // added with FindLpt ==
	 BOOL GetPresent();
	 inline USHORT GetDMAChannel(){ return m_DMAChannel;} //???????????
		//===  end ===

	 USHORT GetIrqNumber();
	 void SetIrqNumber(USHORT nNewValue);
	 ULONG GetIrqCounter();
	 USHORT GetNumLPTs();
	 USHORT GetCurrentLPT();
	 void SetCurrentLPT(USHORT nNewValue);
	 USHORT GetReadMode();
	 void SetReadMode(USHORT nNewValue);
	 ULONG GetDebugCode();
	 USHORT GetBasePortAddress();
	 USHORT GetEcpPortAddress();
	 USHORT GetDataPort();
	 void SetDataPort(USHORT nNewValue);
	 USHORT GetStatusPort();
	 void SetStatusPort(USHORT nNewValue);
	 USHORT GetControlPort();
	 void SetControlPort(USHORT nNewValue);
	 USHORT GetEPPAddressPort();
	 void SetEPPAddressPort(USHORT nNewValue);
	 USHORT GetEPPDataPort();
	 void SetEPPDataPort(USHORT nNewValue);
	 USHORT GetAcquired();
	 void SetAcquired(USHORT nNewValue);
	 USHORT GetCurrentLptMode();
	 void SetCurrentLptMode(USHORT nNewValue);
	 USHORT GetMaxLptMode();
	 USHORT GetEPPEnabled();
	 void SetEPPEnabled(USHORT nNewValue);
	 USHORT GetTransferMode();
	 void SetTransferMode(USHORT nNewValue);
	 USHORT GetIsPrinterReady();
	 USHORT GetActive();
	 void SetActive(USHORT nNewValue);
	 USHORT GetTransferDirection();
	 void SetTransferDirection(USHORT nNewValue);
	 USHORT GetCfgbPort();
	 void SetCfgbPort(USHORT nNewValue);
	 USHORT GetEcrPort();
	 void SetEcrPort(USHORT nNewValue);
	 USHORT GetFifoPort();
	 void SetFifoPort(USHORT nNewValue);
	 USHORT GetCfgaPort();
	 void SetCfgaPort(USHORT nNewValue);
	 USHORT GetEPPDataPortWord();
	 void SetEPPDataPortWord(USHORT nNewValue);
	 ULONG GetEPPDataPortLong();
	 void SetEPPDataPortLong(ULONG nNewValue);
	 HRESULT MaskIrq();
	 HRESULT UnmaskIrq();
	 USHORT AddCustomPort(USHORT CustomBaseAddress);
	 void PulseStrobe(ULONG DelayCycles);
	 USHORT PulseStrobeAndWaitBusy(ULONG DelayCycles);
	 void ReadEppByteArray(ULONG NumOfBytes, const VARIANT FAR& pByteBuffer);
	 void ReadEppWordArray(ULONG NumOfWords, const VARIANT FAR& pWordBuffer);
	 void ReadEppLongArray(ULONG NumOfDwords, const VARIANT FAR& pLongBuffer);
	 void WriteEppByteArray(ULONG NumOfBytes, const VARIANT FAR& pByteArray);
	 void WriteEppLongArray(ULONG NumOfDwords, const VARIANT FAR& pLongArray);
	 void WriteEppWordArray(ULONG NumOfWords, const VARIANT FAR& pWordArray);
	 USHORT GetPin(USHORT nPin);
	 void SetPin(USHORT nPin, USHORT nNewValue);
	 USHORT GetDataPortBit(USHORT BitNumber);
	 void SetDataPortBit(USHORT BitNumber, USHORT nNewValue);
	 USHORT GetStatusPortBit(USHORT BitNumber);
	 void SetStatusPortBit(USHORT BitNumber, USHORT nNewValue);
	 USHORT GetControlPortBit(USHORT BitNumber);
	 void SetControlPortBit(USHORT BitNumber, USHORT nNewValue);
	 USHORT GetCfgaPortBit(USHORT BitNumber);
	 void SetCfgaPortBit(USHORT BitNumber, USHORT nNewValue);
	 USHORT GetCfgbPortBit(USHORT BiyNumber);
	 void SetCfgbPortBit(USHORT BiyNumber, USHORT nNewValue);
	 USHORT GetEcrPortBit(USHORT BitNumber);
	 void SetEcrPortBit(USHORT BitNumber, USHORT nNewValue);

	 friend 
	  void __stdcall TheIsr(TVicLPT& HW32);

	 virtual void FireLptInterrupt(USHORT DataValue, USHORT StatusValue, ULONG LoTimeStamp, ULONG HiTimeStamp);
	
};

