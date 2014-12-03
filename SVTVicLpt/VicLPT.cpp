// ******************************************************************************
// * COPYRIGHT (c) 2005 by SVResearch, Harrisburg
// * All Rights Reserved
// ******************************************************************************
// * .Module Name     : VicLPT
// * .File Name       : $Workfile:   VicLPT.cpp  $
// * ----------------------------------------------------------------------------
// * .Current Version : $Revision:   1.1  $
// * .Check In Date   : $Date:   01 Dec 2014 14:21:14  $
// ******************************************************************************

//====================================================================
//==============            TVicLpt 1.3               ================
//==         Implementation of the TVicLPT C++ class                ==
//==========   Copyright (c) 2001,2002 Victor Ishikeev   =============
//==============    mailto: tools@entechtaiwan.com    ================
//============   http://www.entechtaiwan.com/tools.htm   =============
//====================================================================

#define WINVER 0x0500		// WIN2K
#define _CRT_SECURE_NO_DEPRECATE 
#define _AFX_SECURE_NO_DEPRECATE
#define _ATL_SECURE_NO_DEPRECATE
#define _CRT_NON_CONFORMING_SWPRINTFS

#include <windows.h>
#include <winioctl.h>
#include <winsvc.h>
#include <stdio.h>

#include "VicLPT.h"
#include "tchar.h"
#include "SVSystemLibrary/SVThreadManager.h"

static ULONG InstanceOpened[16] = {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0};


#define nTimeOutCycles  300000


//-------------------------------------------------------------
ULONG CtlCode(ULONG Code) 
{
  if (!(GetVersion() & 0x80000000))	
	  return Code;
  else   		
	  return ((Code>>2)&0x000000ff);
}

//-------------------------------------------------------------

void __stdcall TheIsr(TVicLPT& HW32)
{
	
	ULONG	nByte = 0;
	USHORT    n;
	ULONG   nRetryCounts = 0;


    SetEvent(HW32.m_EventIRQSet);


    SetThreadPriority(GetCurrentThread(), THREAD_PRIORITY_TIME_CRITICAL);
    

    while (1)  
	{
    
      WaitForSingleObject(HW32.m_LocEvent, INFINITE);
		   
	  ResetEvent(HW32.m_LocEvent);

      if (!HW32.m_Terminated) 
	  {
		  if (!HW32.m_Masked) 
		  {
			HW32.m_LastDebugCode = HW32.m_pLockedMemory->L_Buff;

            while ((HW32.m_LastHandled) < (HW32.m_pLockedMemory->IrqCounter)) 
			{
			    HW32.m_LastDebugCode = HW32.m_pLockedMemory->L_Buff;

			    if (HW32.m_pLockedMemory->L_Buff > 0 ) 
				{
			      --(HW32.m_pLockedMemory->L_Buff);
			      n = HW32.m_pLockedMemory->N_Sel;

			      HW32.m_LastDebugCode = HW32.m_pLockedMemory->L_Buff;

                  HW32.FireLptInterrupt(
					  HW32.m_pLockedMemory->Buff[n].DataValue,
					  HW32.m_pLockedMemory->Buff[n].StatusValue,
					  HW32.m_pLockedMemory->Buff[n].LoTimeStamp,
					  HW32.m_pLockedMemory->Buff[n].HiTimeStamp);

			      n++;
			      if ( n > MAX_SHARED_BUFFER_LENGTH ) 
				  {
					  n=0;
				  }
			      HW32.m_pLockedMemory->N_Sel = n;
                  HW32.m_LastHandled++;
				}
			    else
				{
					nRetryCounts++;
					if( nRetryCounts > 100 )
					{
						HW32.m_LastHandled ++;
						nRetryCounts = 0;
					}
				}

			}
		  }
		  else 
		  {
		  }

      }
	  else 
		  break;
    }

}


BOOL TVicLPT::InstallDriver()
{
    SC_HANDLE	schService;
	char	    ServiceExe[255];
	BOOL        Result;

	GetSystemDirectory(ServiceExe, 128);
	strcat(ServiceExe, "\\DRIVERS\\");
	strcat(ServiceExe, "TVicLPT");
    strcat(ServiceExe, ".SYS");
	schService = CreateService( 
		                m_schSCManager,	    // SCManager database
					    "TVicLPT",  			// name of service
					    "TVicLPT",   			// name to display
					    SERVICE_ALL_ACCESS,		// desired access
					    SERVICE_KERNEL_DRIVER,	// service type
					    SERVICE_DEMAND_START,	// start type
					    SERVICE_ERROR_NORMAL,	// error control type
					    ServiceExe,				// service's binary
						NULL,                   // no load ordering group
						NULL,                   // no tag identifier
						NULL,                   // no dependencies
						NULL,                   // LocalSystem account
						NULL);                  // no password)

    Result = (schService != 0) || (GetLastError() == ERROR_SERVICE_EXISTS);


    CloseServiceHandle(schService);
	return Result;

}

BOOL TVicLPT::StartDriver() 
{
	
	SC_HANDLE	schService;
	BOOL		Result = FALSE;
    ULONG        Res;
    
	schService = OpenService (m_schSCManager, "TVicLPT", SERVICE_ALL_ACCESS);
	if (schService != 0) {
		
		Result = StartService(schService, 0, NULL);
		Res = GetLastError();

		Result |= (Res == ERROR_SERVICE_ALREADY_RUNNING);


		CloseServiceHandle (schService);

	}
	return Result;
}

BOOL TVicLPT::StopDriver() 
{
	
	SC_HANDLE schService;
	SERVICE_STATUS serviceStatus;
	BOOL Result = FALSE;

	schService = OpenService (m_schSCManager, "TVicLPT", SERVICE_ALL_ACCESS);
	if (schService != NULL) {
		Result = ControlService (schService, SERVICE_CONTROL_STOP, &serviceStatus);
		CloseServiceHandle (schService);
	}
	return Result;
}

BOOL TVicLPT::RemoveDriver() 
{

	SC_HANDLE	schService;
	BOOL		Result = FALSE;

	schService = OpenService (m_schSCManager, "TVicLPT", SERVICE_ALL_ACCESS);
	if (schService != 0) {
		Result = DeleteService (schService);
		CloseServiceHandle (schService);
	}
	return Result;
	
}

void TVicLPT::CloseStopUnloadDriver() 
{
	CloseHandle(m_hDrv); 
   if (m_Win95)return;

   if (m_Started) return;

	m_schSCManager = OpenSCManager(0, 0, SC_MANAGER_ALL_ACCESS);

	if  (m_schSCManager !=0) {

		CloseServiceHandle(m_schSCManager);

    };
}

void TVicLPT::InstallStartLoadDriver(USHORT DriverInstance) 
{
  char buffer[256];



	 memset(&buffer[0],0,256);

     if (m_Win95) { // Windows 95/98/ME

         strcpy(buffer,"\\\\.\\");
	     strcat(buffer,"TVICLPT");
	     strcat(buffer,".VXD");

	     m_hDrv   = CreateFile(buffer,
						 0,
						 0,
						 NULL,
						 0,
						 FILE_FLAG_DELETE_ON_CLOSE,
						 0
						 );
	     return;
	 }


  strcpy(buffer,"\\\\.\\");

  if (DriverInstance == 2) {
    strcat(buffer,"TVicLPTDevice1");
  }
  else
  {
    strcat(buffer,"TVicLPTDevice0");
  }
  

  m_Registry= TRUE; // assume driver already installed to th registry
  m_Started = TRUE; // assume driver already started


  m_hDrv     = CreateFile(
     	                buffer, 
	                  	GENERIC_READ | GENERIC_WRITE, 0, NULL,
                        OPEN_EXISTING, 
						FILE_ATTRIBUTE_NORMAL, 
						0
						);
 

  if (m_hDrv != INVALID_HANDLE_VALUE) return;


  m_Started = FALSE;

   m_schSCManager = OpenSCManager(0, 0, SC_MANAGER_ALL_ACCESS);


  if (m_schSCManager !=0) {

 
	  // Driver already installed but not started? Try to start...
	  if (StartDriver()) {

	     m_hDrv  = CreateFile(
			                buffer, 
		                  	GENERIC_READ | GENERIC_WRITE, 0, NULL,
                            OPEN_EXISTING, 
							FILE_ATTRIBUTE_NORMAL, 
							0
							);
		    m_LastDebugCode = GetLastError();
			CloseServiceHandle (m_schSCManager);

			return; // YES!

          }


		 m_Registry = FALSE; // not installed

		 
		 if (InstallDriver()) {

            if (StartDriver()) {

	           m_hDrv   = CreateFile(
			                buffer, 
		                  	GENERIC_READ | GENERIC_WRITE, 0, NULL,
                            OPEN_EXISTING, 
							FILE_ATTRIBUTE_NORMAL, 
							0
							);
			   m_LastDebugCode = GetLastError();
		
			};

		 };

	     CloseServiceHandle (m_schSCManager);

	};

	return;

}


/////////////////////////////////////////////////////////////////////////////
// TVicLPT::TVicLPT - Constructor

TVicLPT::TVicLPT()
{

	
    m_hDrv              = 0;
	m_LastDebugCode     = 0;
    m_LptAcquired       = 0;
	m_MappedLPTArea     = NULL;

	m_LPTNumber         = 0;
		
    m_HardAccess        = TRUE;
    m_MemoryMapped      = FALSE;
    m_OpenDrive         = FALSE;

    m_MaxLptMode  = LPT_NOT_PRESENT;
    m_CurrLptMode = LPT_NOT_PRESENT;

	m_LPTs              = 1;
	m_LPTBasePort       = 0x0000;
	m_EcpBasePort       = 0xFFFF;
	m_LPTNumber         = 1;
	m_LPTReadModes[0]   = 0;
    
	m_Masked            = TRUE;
	m_LastHandled       = 0;
	m_LastCounter       = 0;


	 //== Added with Lpt ==
  m_IrqNumber = 0;
  m_DMAChannel= 0;
  m_LPTPresent = FALSE;

	//== end  ==

}


/////////////////////////////////////////////////////////////////////////////
// TVicLPT::~TVicLPT - Destructor

TVicLPT::~TVicLPT()
{
    CloseTVicLPT();

}


/////////////////////////
// TVicLPT methods


		//== Added with FindLpt ==
BOOL TVicLPT::GetPresent()
{
	return m_OpenDrive && m_LPTPresent && (m_CurrLptMode!= LPT_NOT_PRESENT);
}
		//== end  ==

void TVicLPT::SetLptIoAccess( UCHAR LptNumber )
{
	struct {
		ULONG BeginPort;
        ULONG EndPort;
        ULONG Enable;
	} PortRange;

  ULONG nByte;
   
  if (m_OpenDrive && (! m_Win95)) {
  

    DeviceIoControl(m_hDrv,CtlCode(CLEAR_LIOPM),NULL, 0, NULL, 0, &nByte, NULL);

    PortRange.BeginPort = m_LPTAddresses[LptNumber-1];
    PortRange.Enable    = 1;

    if (PortRange.BeginPort == 0x03BC)
      PortRange.EndPort = PortRange.BeginPort + 2;
    else
      PortRange.EndPort = PortRange.BeginPort + 7;


    DeviceIoControl(m_hDrv,CtlCode(SET_RANGE),&PortRange,sizeof(PortRange), &PortRange,sizeof(PortRange), &nByte, NULL);

    if (m_EcpAddresses[LptNumber-1]!=0xFFFF) {
    
      PortRange.BeginPort = m_EcpAddresses[LptNumber-1];
      PortRange.EndPort = PortRange.BeginPort + 2;
      DeviceIoControl(m_hDrv,CtlCode(SET_RANGE),&PortRange,sizeof(PortRange), &PortRange,sizeof(PortRange), &nByte, NULL);
    }

  }
}


UCHAR TVicLPT::GetPort(USHORT PortAddress) 
{

	ULONG rez;

	if( !m_OpenDrive) return 0;

	_asm {
		mov  dx,PortAddress
		in   al,dx
		mov  rez,eax
	}
        
	return (UCHAR) (rez & 0x000000FF);

}

void TVicLPT::SetPort(USHORT PortAddress, UCHAR nNewValue) 
{

		if (!m_OpenDrive) return;

        _asm {
			mov  dx,PortAddress
			mov  al,nNewValue
			out dx,al
		}
}

USHORT TVicLPT::GetPortW(USHORT PortAddress) 
{
	
	ULONG rez;

	if( !m_OpenDrive) return 0;

        _asm {
			mov  dx,PortAddress
			in   ax,dx
			mov  rez,eax
		}
        
	return (USHORT) (rez && 0x0000FFFF);

}

void TVicLPT::SetPortW(USHORT PortAddress, USHORT nNewValue) 
{

		if (!m_OpenDrive) return;

		_asm {
			mov  dx,PortAddress
			mov  ax,nNewValue
			out dx,ax
		}

}

ULONG TVicLPT::GetPortL(USHORT PortAddress) 
{

	ULONG rez;

	if( !m_OpenDrive) return 0;

    _asm {
		mov  dx,PortAddress
		in   eax,dx
		mov  rez,eax
	}
        
	return rez;


}

void TVicLPT::SetPortL(USHORT PortAddress, ULONG nNewValue) 
{

		if (!m_OpenDrive) return;

       _asm {
			mov  dx,PortAddress
			mov  eax, nNewValue
			out dx,eax
		}

}

UCHAR TVicLPT::BitRead(UCHAR PortData, UCHAR nBit)
{
  return ( (PortData >> nBit) & 0x01);
}

UCHAR TVicLPT:: ReadEppTimeoutBit()
{
	UCHAR TimeOutBit;

    // Reads and clears the EPP timeout bit (status port bit 0).
    // Should be done after each EPP operation.
    // The method for clearing the bit varies, so try 3 ways:
    // 1. Write 1 to it.
    // 2. Write 0 to it.
    // 3. Read it again.

    TimeOutBit = BitRead( (BYTE) GetStatusPort(), 0 );
    SetStatusPort(1);
	SetStatusPort(0);
    TimeOutBit = BitRead( (BYTE) GetStatusPort(), 0 );

	return TimeOutBit;

}   

BOOL TVicLPT::TestForECP()
{
	// Test for the presence of an ECP.
    // If the ECP is idle and the FIFO empty,
    // in the ECP's Ecr (at Base Address+402h),
    // bit 1(Fifo full)=0, and bit 0(Fifo empty)=1.
    // The first test is to see if these bits differ from the
    // corresponding bits in the control port (at Base Address+2).
    // If so, a further test is to write 34h to the Ecr,
    // then read it back. Bit 1 is read/write, and bit 0 is read-only.
    // If the value read is 35h, the port is an ECP.

	UCHAR EcrBit0, EcrBit1, ControlBit1, EcrData, OriginalEcrData, ControlPortData;
	BOOL Rez = FALSE;

    
	Rez = FALSE;


    // Read ECR bits 0 & 1 and Control Port bit 1.

	EcrData = (BYTE)GetEcrPort();
    EcrBit0 = BitRead(EcrData,0);
	EcrBit1 = BitRead(EcrData,1);

	ControlPortData = (BYTE)GetControlPort();
	ControlBit1     = BitRead(ControlPortData,1);

	if ( (EcrBit0 == 1) && (EcrBit1 == 0) ) {

       // Compare control bit 1 to ECR bit 1.
       // Toggle the control bit if necessary,
       // to be sure the two registers are different.

		    if (ControlBit1 == 0) {
  
				SetControlPort(0x0F);
	
				ControlPortData = (BYTE) GetControlPort();
	
				ControlBit1 = BitRead(ControlPortData,1);
			}

		    if ( EcrBit1 != ControlBit1 ) {

			// Save the ECR's original value
			    OriginalEcrData = EcrData;
	
				SetEcrPort( 0x34 );
	
				EcrData = (BYTE) GetEcrPort();

			    if ( EcrData == 0x35 ) {

					Rez = TRUE;
							   
 		            SetEcrPort((BYTE)((OriginalEcrData & 0x1F)) | (BYTE)0x80);

                    m_EPPEnabled = TestForEPP();

				}

			// Restore the ECR to its original value
			    SetEcrPort( OriginalEcrData );


			}
	}

	return Rez;

}

USHORT TVicLPT::TestForEPP()
{
	// Write to an Epp register, then read it back.
    // If the reads match the writes, it's probably an Epp.

    UCHAR ByteRead;
	USHORT Rez = 0;

	// Use EppAddressPort for testing.
    // SPPs, ECPs, and PS/2 ports don't have this register.

	SetEPPAddressPort( 0x55 );

	// Clear the timeout bit after each EPP operation.

	ReadEppTimeoutBit();
	ByteRead = (BYTE) GetEPPAddressPort();
    ReadEppTimeoutBit();

	if ( ByteRead == 0x55 ) {

		SetEPPAddressPort( 0xAA );
        ReadEppTimeoutBit();
        ByteRead = (BYTE) GetEPPAddressPort();
        ReadEppTimeoutBit();

		Rez = (ByteRead == 0xAA );
	}

    m_EPPEnabled = Rez;

    return Rez;

}

BOOL TVicLPT::TestForPS2()
{
    // Tests a parallel port's data port for bidirectional ability.
    // First, try to tri-state (disable) the data outputs by
    // setting bit 5 of the Control port.
    // Then write 2 values to the data port and read each back
    // If the values match, the data outputs are not disabled,
    // and the port is not bidirectional.
    // If the values don't match,
    // the data outputs are disabled and the port is bidirectional.

    UCHAR DataInput;
	BOOL TestForPS2 = FALSE;

    // Set Control port bit 5.

    SetControlPort( 0x2F );
    
    // Write the first byte and read it back:

    SetDataPort( 0x55 );
    DataInput = (BYTE) GetDataPort();

    // If it doesn't match, the port is bidirectional.
    // If Not DataInput = &H55 Then TestForPS2 = True
    // If it matches, write another and read it back.

	if (DataInput == 0x55 ) {

		SetDataPort ( 0xAA );
        DataInput = (BYTE) GetDataPort ();

       // If it doesn't match, the port is bidirectional
       if ( DataInput != 0xAA ) TestForPS2 = TRUE;
    }
	else
		TestForPS2 = TRUE;

    // Reset Control port bit 5
    SetControlPort(0x0F);

	return TestForPS2;

}

BOOL TVicLPT::TestForSPP()
{
	
    // Write two bytes and read them back.
    // If the reads match the writes, the port exists.

    BOOL TestForSPP = FALSE;
	UCHAR ByteRead;

    // Be sure that control port bit 5 = 0 (data outputs enabled).
    SetControlPort(0x0F);

    SetDataPort ( 0x55 );
	ByteRead = (BYTE) GetDataPort();

    if (ByteRead == 0x55) {
		SetDataPort ( 0xAA );
    	ByteRead = (BYTE) GetDataPort();
        if (ByteRead == 0xAA ) TestForSPP = TRUE;
    }

	return TestForSPP;

}

USHORT TVicLPT::OpenTVicLPT(USHORT DriverInstance) 
{

  OSVERSIONINFOA  WinVer;
  ULONG           nByte;

  int             i;
  LockedBuffer    Rec;

  DriverInstance &=0xFF;

  if (m_OpenDrive) return 1;

  if (InstanceOpened[DriverInstance]) return NULL;

  m_UseIrq =0;
 
  m_Win95  = (GetVersion() & 0x80000000)!=0;
  m_BiosMethod = 0;

  m_OpenDrive     = FALSE;

  m_Masked = TRUE;
 
  m_IrqShared = 1;


  m_LptAcquired = 0;

  
  m_MemoryMapped  = FALSE;
  m_HardAccess    = TRUE;
  m_hDrv          = INVALID_HANDLE_VALUE;

  m_LPTs         = 0;
  m_LPTBasePort  = 0x378;
  m_EcpBasePort  = m_LPTBasePort+0x400;
  m_LPTNumber    = 1;

  m_TransferDirection = 0;
	
  InstallStartLoadDriver(DriverInstance);
	                                 

  if (m_OpenDrive = (m_hDrv != INVALID_HANDLE_VALUE)) 
  {

	  m_DriverInstance = DriverInstance;
	  InstanceOpened[DriverInstance] = 1;
	  
      m_LocEvent = CreateEvent(NULL,TRUE,FALSE,NULL);


      if (!(m_Win95)) {
  
          WinVer.dwOSVersionInfoSize = sizeof(WinVer);
          GetVersionEx(&WinVer);
          m_NT5 = WinVer.dwMajorVersion>4;
	      
		  m_KernelEvent = m_LocEvent;
          
	  }
      else {

		  m_NT5               = 0;
          m_hDll               = GetModuleHandle("kernel32");
          m_OpenVxDHandle     = (TOpenVxDHandle)GetProcAddress(m_hDll,"OpenVxDHandle");
	      m_KernelEvent = m_OpenVxDHandle(m_LocEvent);

	  };


// Create thread for the interrupts hanling

	  m_EventIRQSet  = CreateEvent(NULL,TRUE,FALSE,NULL);
	  m_Terminated   = FALSE;

      ResetEvent(m_EventIRQSet);
    

      m_ThreadHandle = CreateThread(NULL,
	 			                 0,
					             (LPTHREAD_START_ROUTINE)&TheIsr,
					             this,
					             0,
					             (LPDWORD)&(m_ThreadId));

      SVThreadManager::Instance().Add( m_ThreadHandle, _T("SVTVic ISR Thread"));

      WaitForSingleObject(m_EventIRQSet,INFINITE);
    
      CloseHandle(m_EventIRQSet);
		
	  m_LPTs = 0;	
	  DeviceIoControl(m_hDrv,
		              CtlCode(MAP_MEMORY),
					  NULL,0,
					  &m_MappedLPTArea,4,
					  &nByte,NULL);

      m_hDll = (HINSTANCE)m_LPTAddresses;		

	  //===  Added with FindLpt ===
	  for (i=0; i<7; i++)
	  {
		  m_LPTIrq[i] = 0;
		  m_LPTDMA[i] = 0;
		  m_Present[i] = false;
		  m_LPTAddresses[i] = 0;
		  m_EcpAddresses[i] = 0xFFFF;
		  if ( m_FindLPT.SearchLpt(i+1))
		  {
			 m_Present[i] = true;
			 m_LPTAddresses[i] = m_FindLPT.GetPortBase(1);
			 m_EcpAddresses[i] = m_FindLPT.GetPortBase(2);
			 if (m_FindLPT.GetIRQCounter()>0)
				 m_LPTIrq[i] = m_FindLPT.GetIRQNumber();
			 if (m_FindLPT.GetDMACounter()>0)
				 m_LPTDMA[i] = m_FindLPT.GetDMAChannel();
			 m_LPTs++;
		  }
	  }

	  if (m_LPTs == 0)
	  {
		// == end ==
		  for ( i = 0; i<3; i++) {

			  __try {
				  if ((*(m_MappedLPTArea+i)) !=0) {

					  m_LPTs++;
				      m_LPTAddresses[i] = *(m_MappedLPTArea+i);
					  m_Present[i] = true;	
//				      m_LPTAddresses[m_LPTs++] = *(m_MappedLPTArea+i);		//????????????????
//				  m_LPTAddresses[i] = *(m_MappedLPTArea+i);					//????????????????

				}

			 }
			__finally {}

			  m_LPTReadModes[i] = 0;
		  } //for
	  }	//if

		//===  Added with FindLpt ===
 
	  m_LPTs = 7;
	  while ((m_LPTs>0) && (m_LPTAddresses[m_LPTs-1]==0) )
		  m_LPTs--;

	  if (m_LPTs == 0)
	  {
		  m_LPTs = 1;
	      m_LPTAddresses[m_LPTs] = 0x378;	//	????????????????????
	  }

	  for (i=0; i> m_LPTs; i++)
		  if (m_LPTAddresses[i] == 0)
			  if (i>0)
				  m_LPTAddresses[i] = m_LPTAddresses[i-1];
			  else
				  m_LPTAddresses[i] = m_LPTAddresses[i-1];


		// == end ==

      Rec.NumLockedPages   = 2;
	  Rec.BufferSize       = Rec.NumLockedPages * 4096;
      
      DeviceIoControl(m_hDrv,
                      CtlCode(LOCK_MEMORY),
                      &Rec,sizeof(Rec),&Rec,sizeof(Rec),
                      &nByte,NULL); 

	  m_pLockedMemory = Rec.pBuf;
      m_pInternalLockedMemory = Rec.pInternalLockedAddress;

	  SetCurrentLPT(1);

  }
  else {

      nByte = m_LastDebugCode;
  }

    
	return m_OpenDrive;
}

void TVicLPT::CloseTVicLPT() 
{

	ULONG nByte;

	
	if ( !m_OpenDrive) return;

	InstanceOpened[m_DriverInstance] = 0;
		

	if (m_OpenDrive) {


		if (m_LptAcquired) SetAcquired(0);


		m_LptAcquired = 0;
		
		m_HardAccess = TRUE;

	    if (!m_Masked) MaskIrq();

		m_IrqShared = 1;
		
		m_Terminated = 1;

	    SetEvent(m_LocEvent);
		
		SVThreadManager::Instance().Remove(m_ThreadHandle);

        WaitForSingleObject(m_ThreadHandle,INFINITE);
	    CloseHandle(m_ThreadHandle);

	    CloseHandle(m_LocEvent);
		m_LocEvent = 0;
		

		m_OpenDrive = FALSE;

		DeviceIoControl(m_hDrv,
			          CtlCode(UNLOCK_MEMORY),
					  &m_pLockedMemory,4,
					  &m_pLockedMemory,4,
					  &nByte,NULL);

		

        DeviceIoControl(m_hDrv,
		    			CtlCode(UNMAP_MEMORY),
			    		&m_MappedLPTArea, 4,
				    	NULL, 0, &nByte, NULL);
				  
		CloseStopUnloadDriver();

    }
//== Added with FindLPT ==
	m_LPTPresent = false;
	m_IrqNumber = 0;
	m_DMAChannel = 0;

//== end ==
	
}

USHORT TVicLPT::GetIrqNumber() 
{
	return m_IrqNumber;
}

void TVicLPT::SetIrqNumber(USHORT nNewValue) 
{
    m_IrqNumber =  nNewValue;
	m_LPTIrq[m_LPTNumber] = (BYTE) nNewValue;
}


HRESULT TVicLPT::MaskIrq() 
{
	HRESULT l_Status = S_OK;

	if( m_OpenDrive )
	{
		if( ! m_Masked )
		{
			ULONG nByte = 0;
			ULONG Counter = 0;

			SetControlPort(GetControlPort() & 0xEF);	        

			if( DeviceIoControl(m_hDrv,
				CtlCode(MASK_INT_VEC),
				NULL,0, &Counter, 4,
				&nByte, NULL) )
			{
				m_Masked = true;
			}
			else
			{
				l_Status = E_FAIL;
			}

			m_IrqShared = 1;
		}
	}
	else
	{
		l_Status = E_FAIL;
	}

	return l_Status;
}

HRESULT TVicLPT::UnmaskIrq() 
{
	HRESULT l_Status = S_OK;

	if( m_OpenDrive )
	{
		if( m_Masked )
		{
			ULONG   nByte = 0;
			INT_VEC Rec;

			SetControlPort(GetControlPort() & 0xEF);	        

			Rec.ClearIrq = 0;

			m_LastHandled  = 0;

			m_IrqShared = 1;

			Rec.IRQNumber  = m_IrqNumber;
			Rec.hUserEvent = m_KernelEvent;
			Rec.pKernelSharedBuffer = m_pInternalLockedMemory;

			Rec.IrqShared = 1;

			memset(m_pLockedMemory,0,100);

			Rec.RegBaseAddress = m_LPTBasePort;

			if( DeviceIoControl(m_hDrv,
				CtlCode(UNMASK_INT_VEC),
				&Rec,sizeof(Rec),NULL,0,
				&nByte,NULL) )
			{
				m_Masked = false;
			}
			else
			{
				l_Status = E_FAIL;
			}

			SetControlPort(GetControlPort() | 0x10);

		}
	}
	else
	{
		l_Status = E_FAIL;
	}

	return l_Status;
}

ULONG TVicLPT::GetIrqCounter() 
{

	if (m_OpenDrive) {
		
		if (!m_Masked) {

			return m_pLockedMemory->IrqCounter;
      	        
		}

		else

			return m_LastHandled;
	
	}

    else return 0;	
}


USHORT TVicLPT::GetNumLPTs() 
{
	return m_LPTs;
}

USHORT TVicLPT::GetCurrentLPT() 
{
	
    return m_LPTNumber;
	
}

void TVicLPT::SaveLptState( UCHAR nLpt )
{

	int i;
    pSaveLptState p;

    if (nLpt == 0) return;

    p = &m_SaveArea[nLpt-1];

    p->sSaved = true;
    p->sMaxLptMode   = m_MaxLptMode;
    p->sCurrLptMode  = m_CurrLptMode;
    p->sLptReadMode  = (UCHAR)m_LPTReadModes[nLpt-1];
    p->sEPPEnabled   = (UCHAR)m_EPPEnabled;
    p->sLptAcquired  = (UCHAR)m_LptAcquired;

    for (i=0; i<11; i++) {
      p->sDataReadPorts[i]   = m_DataReadPorts[i];
      p->sDataWritePorts[i]  = m_DataWritePorts[i];
    }

}
  
bool TVicLPT::RestoreLptState ( UCHAR nLpt)
{
	int i;

	pSaveLptState p;

	if (nLpt==0) return false;

    m_LPTNumber    =  nLpt;
    m_LPTBasePort  =  m_LPTAddresses[nLpt-1];
    m_EcpBasePort  =  m_EcpAddresses[nLpt-1];
    
    p = &m_SaveArea[nLpt-1];
    if ( !(p->sSaved) ) return false;

    m_MaxLptMode   =  p->sMaxLptMode;
    m_CurrLptMode  =  p->sCurrLptMode;
    m_LPTReadModes[nLpt-1] =  p->sLptReadMode;
    m_EPPEnabled   =  p->sEPPEnabled;
    m_LptAcquired  =  p->sLptAcquired;
    
    for (i=0; i<11; i++) {
      m_DataReadPorts[i]  =  p->sDataReadPorts[i];
      m_DataWritePorts[i] =  p->sDataWritePorts[i];
    }

	return true;

}

void TVicLPT::SetCurrentLPT(USHORT nNewValue) 
{
		//===  Added with FindLpt ===
	m_IrqNumber = m_LPTIrq[nNewValue-1];
	m_DMAChannel = m_LPTDMA[nNewValue-1];
	m_LPTPresent = m_Present[nNewValue-1];
		//== end ==

	   SetLptIoAccess( (UCHAR)nNewValue );
   
	    if (nNewValue != m_LPTNumber) {
            SaveLptState(m_LPTNumber);
            if (RestoreLptState((UCHAR)nNewValue)) return;
		}

		SetLptIoAccess(m_LPTNumber);
		
		m_LPTBasePort = m_LPTAddresses[nNewValue-1];
		m_EcpBasePort = m_EcpAddresses[nNewValue-1];
		m_LPTNumber   = (BYTE)nNewValue;
		

        m_MaxLptMode  = LPT_NOT_PRESENT;
		m_CurrLptMode = LPT_NOT_PRESENT;

		m_EPPEnabled = 0;

        if ((m_LPTBasePort!=0x3BC) && (m_EcpBasePort!=0xFFFF) && TestForECP()) {

			m_MaxLptMode = LPT_ECP_MODE;

			switch ( (GetEcrPort() & 0xE0) >> 5) {

			case 0x00 : m_CurrLptMode = LPT_SPP_MODE; break;
            case 0x01 : m_CurrLptMode = LPT_PS2_MODE; break;
			case 0x02 : m_CurrLptMode = LPT_FAST_CENTRONIX; break;
			case 0x03 : m_CurrLptMode = LPT_ECP_MODE; break;
			case 0x04 : if (m_EPPEnabled) m_CurrLptMode = LPT_EPP_MODE; break;
			default   : m_CurrLptMode = LPT_ECP_MODE;
			}

		}

		else
			if ((m_LPTBasePort != 0x3BC) && TestForEPP()) {

				m_MaxLptMode = LPT_EPP_MODE;
				m_CurrLptMode = LPT_EPP_MODE;
				m_EPPEnabled = 1;

			}
			else
				if (TestForSPP()) {

					if (TestForPS2()) {

						m_MaxLptMode  = LPT_PS2_MODE;
						m_CurrLptMode = LPT_PS2_MODE;
					}
					else {
						
					    m_MaxLptMode  = LPT_SPP_MODE;
					    m_CurrLptMode = LPT_SPP_MODE;
					}

				}


		//Changed with TVicLpt ===
//???????????????????????????????????????????????????
		//== end ==



		SaveLptState((UCHAR)m_LPTNumber);

		SetControlPort( 0xFFFF );
		SetControlPort( 0x0000 );

		UnmaskIrq();
		MaskIrq();
		
}


USHORT TVicLPT::GetPin(USHORT nPin) 
{
    UCHAR data;
	USHORT ofs,n;
	BOOL Result = FALSE;

    if ( !m_OpenDrive || (nPin>17) || (nPin<=0)) return 0;
	n = nPin - 1;
	ofs = PinsPort[n];
	data = (BYTE)GetPort((USHORT)(m_LPTBasePort+ofs));
    Result = (data & MaskPins[n]) != 0;
	if (Negative[n]) Result = !Result;
	
	if (Result) return 1;
	else        return 0;
}



void TVicLPT::SetPin(USHORT nPin, USHORT nNewValue) 
{
   UCHAR   data;
   USHORT ofs,n;
   BOOL bNewValue;

   if (!m_OpenDrive || (nPin>17) || (nPin<=0)) return;

   bNewValue = (nNewValue!=0);

   n = nPin-1;
   ofs    = PinsPort[n];
   data   = m_DataWritePorts[ofs];
   if (bNewValue != Negative[n]) data = data | MaskPins[n];
   else data = data & (~MaskPins[n]);
   SetPort((short)(m_LPTBasePort+ofs),data);
   m_DataWritePorts[ofs] = data;

}

USHORT TVicLPT::GetReadMode() 
{
	if (m_OpenDrive & (m_LPTNumber>0) ) {

		if (GetControlPort() & 0x20)

	        return m_LPTReadModes[m_LPTNumber-1] = 1;

		else
			return m_LPTReadModes[m_LPTNumber-1] = 0;
	}
    else
	  return 0;
}

void TVicLPT::SetReadMode(USHORT nNewValue) 
{

    if ( m_OpenDrive && (m_LPTNumber>0) ) {
	
       if (nNewValue!=0) nNewValue = 1;	  

	   if (nNewValue) 
	    	
            SetControlPort(GetControlPort() | 0x20);
	   
 	   else 

		    SetControlPort(GetControlPort() & 0xDF);
	   
	   m_LPTReadModes[m_LPTNumber-1] = nNewValue;

	}
}

ULONG TVicLPT::GetDebugCode() 
{
	return m_MainDebugCode;
}

USHORT TVicLPT::GetBasePortAddress() 
{
   return  m_LPTBasePort;
}

USHORT TVicLPT::GetEcpPortAddress() 
{
   return  m_EcpBasePort;
}

USHORT TVicLPT::GetDataPort() 
{
	return m_DataReadPorts[0] = GetPort(m_LPTBasePort);
}

void TVicLPT::SetDataPort(USHORT nNewValue) 
{
	SetPort(m_LPTBasePort, m_DataWritePorts[0] = (UCHAR)nNewValue);	
}

USHORT TVicLPT::GetStatusPort() 
{
	return m_DataReadPorts[1] = GetPort(m_LPTBasePort+1);
}

void TVicLPT::SetStatusPort(USHORT nNewValue) 
{
	SetPort(m_LPTBasePort+1, m_DataWritePorts[1] = (UCHAR)nNewValue);
}

USHORT TVicLPT::GetControlPort() 
{
	return m_DataReadPorts[2] = GetPort(m_LPTBasePort+2);
	
}

void TVicLPT::SetControlPort(USHORT nNewValue) 
{
	SetPort(m_LPTBasePort+2, m_DataWritePorts[2] = (UCHAR)nNewValue);
}

USHORT TVicLPT::GetEPPAddressPort() 
{
	if (m_LPTBasePort==0x3BC) return 0x3BC;		//???????????????????????????
	
	return m_DataReadPorts[3] = GetPort(m_LPTBasePort+3);
}

void TVicLPT::SetEPPAddressPort(USHORT nNewValue) 
{
	if (m_LPTBasePort==0x3BC) return;
	SetPort(m_LPTBasePort+3, m_DataWritePorts[3] = (UCHAR)nNewValue);
}

USHORT TVicLPT::GetEPPDataPort() 
{
	if (m_LPTBasePort==0x3BC) return 0;
	return m_DataReadPorts[4] = GetPort(m_LPTBasePort+4);
}

void TVicLPT::SetEPPDataPort(USHORT nNewValue) 
{
	if (m_LPTBasePort==0x3BC) return;
	SetPort(m_LPTBasePort+4, m_DataWritePorts[4] = (UCHAR)nNewValue);
}

USHORT TVicLPT::GetAcquired() 
{   
	return m_LptAcquired;
}

void TVicLPT::SetAcquired(USHORT nNewValue) 
{
	char   LPTName[5];

    
	if (nNewValue) { // Acquire LPT


       if (m_LptAcquired) return;

    
       switch (m_LPTNumber) {
		   
	       case 1: strcpy(LPTName,"LPT1"); break;
		   case 2: strcpy(LPTName,"LPT2"); break;
		   case 3: strcpy(LPTName,"LPT3"); break;

	   }

	   m_hLptAcquired = CreateFile(
		                    LPTName, 
		                    GENERIC_READ | GENERIC_WRITE,
                            FILE_SHARE_DELETE,                    // exclusive access
                            NULL,                 // no security attrs
                            OPEN_EXISTING,
                            FILE_ATTRIBUTE_NORMAL |
                            FILE_FLAG_OVERLAPPED, // overlapped I/O
                            NULL );

       
       if (m_hLptAcquired != INVALID_HANDLE_VALUE) {

		   m_LptAcquired = 1;
	   }
	   else {
           m_LptAcquired = 0;
	   }
	}
	else { 
		if (m_LptAcquired) 	CloseHandle(m_hLptAcquired);
        m_LptAcquired = 0;
	}
}

USHORT TVicLPT::GetCurrentLptMode() 
{
	return m_CurrLptMode;
}

void TVicLPT::SetCurrentLptMode(USHORT nNewValue) 
{
	UCHAR mode, DataRead;

    if ((nNewValue <= m_MaxLptMode) && (m_MaxLptMode == LPT_ECP_MODE) 
		&& (nNewValue>0) && m_OpenDrive) {

		m_CurrLptMode = (BYTE)nNewValue;

		switch (nNewValue) {
            case LPT_SPP_MODE      : mode = 0x00; break;
			case LPT_PS2_MODE      : mode = 0x01; break;
			case LPT_FAST_CENTRONIX: mode = 0x02; break;
			case LPT_EPP_MODE      : mode = 0x04; break;
			case LPT_ECP_MODE      : mode = 0x03; break;
            default                : mode = 0x00; break;
		}

		DataRead = (BYTE)GetEcrPort();
		DataRead = (DataRead & 0x1F) | (mode << 5);
		SetEcrPort(DataRead);

	}
	
}

USHORT TVicLPT::GetMaxLptMode() 
{
	return m_MaxLptMode;
}

USHORT TVicLPT::AddCustomPort(USHORT CustomBaseAddress) 
{
  if (!m_OpenDrive ) return 0;
  m_LPTAddresses[m_LPTs] = CustomBaseAddress;
  m_LPTReadModes[m_LPTs] = 0;
  return ++m_LPTs;
}


USHORT TVicLPT::GetEPPEnabled() 
{

	return m_EPPEnabled;
}

void TVicLPT::SetEPPEnabled(USHORT nNewValue) 
{
	
	if (m_OpenDrive) {

		if (m_MaxLptMode == LPT_EPP_MODE) {
			m_EPPEnabled = 1;
			return;
		}

		if ((m_MaxLptMode == LPT_ECP_MODE) &&
			(m_EPPEnabled) ) {

			SetEcrPort((GetEcrPort() & 0x1F) | 0x80);

           
		}
	}
}

USHORT TVicLPT::GetTransferMode() 
{
	return m_TransferMode;
}

void TVicLPT::SetTransferMode(USHORT nNewValue) 
{
	m_TransferMode = nNewValue;
	return;

}

void TVicLPT::PulseStrobe(ULONG DelayCycles) 
{
	USHORT  nPort;

	if (!m_OpenDrive) return;

	if (DelayCycles <=0) DelayCycles = 1;

	nPort = m_LPTBasePort+2;


    __asm {

       mov  ecx,DelayCycles
       mov  dx,nPort
       in   al,dx // save control reg value
       and  al,0feh
      cyc:
	    out  dx,al
	    loop cyc
       or   al,1
      out dx,al
	}


}

USHORT TVicLPT::PulseStrobeAndWaitBusy(ULONG DelayCycles) 
{
        USHORT nPort;

		if (!m_OpenDrive) return 0;

		if (DelayCycles <=0) DelayCycles = 1;

		nPort = m_LPTBasePort+2;

		// Put low, wait for BUSY, put high
        __asm {
			mov  ecx,DelayCycles  // how long is time out
    		mov  dx,nPort
    		in   al,dx
    		mov  ah,al
    		and  al,0feh
    		out  dx,al
    		inc  dx
    	s1:
        		in   al,dx
			test al,1
			jz   endl
			mov  nPort,1   // success
			jmp  estr
    	endl:
			loop s1
			mov  nPort,0  // time out
    	estr:
			dec  dx
        	mov  al,ah
			or   al,1
			out dx,al
		}
        
		return (nPort != 0);
}

USHORT TVicLPT::GetIsPrinterReady() 
{

	UCHAR StatusData;
	if (!m_OpenDrive) return 0;

	StatusData = (BYTE)GetStatusPort();
	if ((BitRead(StatusData,7) == 0) &&
		(BitRead(StatusData,3) == 1) ) 
		return 1;

	return 0;

}

USHORT TVicLPT::GetDataPortBit(USHORT BitNumber) 
{
    return (GetDataPort() >> BitNumber) & 0x0001 ;
}

void TVicLPT::SetDataPortBit(USHORT BitNumber, USHORT nNewValue) 
{
	if (nNewValue) 
	    SetDataPort(m_DataWritePorts[0] |= MaskUp[BitNumber]);
	else 
        SetDataPort(m_DataWritePorts[0] &= MaskDown[BitNumber]);

}

USHORT TVicLPT::GetStatusPortBit(USHORT BitNumber) 
{
	
	return (GetStatusPort() >> BitNumber) & 0x0001 ;
}

void TVicLPT::SetStatusPortBit(USHORT BitNumber, USHORT nNewValue) 
{
	if (nNewValue) 
	    SetStatusPort(GetStatusPort() | MaskUp[BitNumber]);
	else 
        SetStatusPort(GetStatusPort() & MaskDown[BitNumber]);
}

USHORT TVicLPT::GetControlPortBit(USHORT BitNumber) 
{
	return (GetControlPort() >> BitNumber) & 0x0001 ;
}

void TVicLPT::SetControlPortBit(USHORT BitNumber, USHORT nNewValue) 
{
	if (nNewValue) 
	    SetControlPort(GetControlPort() | MaskUp[BitNumber]);
	else 
        SetControlPort(GetControlPort() & MaskDown[BitNumber]);
}


USHORT TVicLPT::GetActive() 
{
  return m_OpenDrive;   
}

void TVicLPT::SetActive(USHORT nNewValue) 
{
	if (nNewValue)
		OpenTVicLPT(nNewValue);
	else
		CloseTVicLPT();
}


USHORT TVicLPT::GetTransferDirection() 
{
	return m_TransferDirection; // 0 - none
	                            // 1 - Output
	                            // 2 - Input
}

void TVicLPT::SetTransferDirection(USHORT nNewValue) 
{

    m_TransferDirection = nNewValue;

    switch (m_TransferDirection) {

	case 1:
		SetControlPortBit(0,1);
		SetControlPortBit(3,1);
		SetControlPortBit(5,0);
        break;
	case 2:
		SetControlPortBit(0,0);
		SetControlPortBit(3,0);
		SetControlPortBit(5,1);
        break;
	default:
		m_TransferDirection = 0;
	}
     	

}

USHORT TVicLPT::GetCfgaPortBit(USHORT BitNumber) 
{
	if (m_LPTBasePort==0x3BC) return 0;
	return (GetCfgaPort() >> BitNumber) & 0x0001;
}

void TVicLPT::SetCfgaPortBit(USHORT BitNumber, USHORT nNewValue) 
{
	if (m_LPTBasePort==0x3BC) return;
	if (nNewValue) 
	    SetCfgaPort(GetCfgaPort() | MaskUp[BitNumber]);
	else 
        SetCfgaPort(GetCfgaPort() & MaskDown[BitNumber]);
}


USHORT TVicLPT::GetCfgbPort() 
{
	if (m_LPTBasePort==0x3BC) return 0;
	return m_DataReadPorts[9] = GetPort(m_EcpBasePort+1);
}

void TVicLPT::SetCfgbPort(USHORT nNewValue) 
{
	if (m_LPTBasePort==0x3BC) return;
	SetPort(m_EcpBasePort+1, m_DataWritePorts[9] = (UCHAR)nNewValue);
}

USHORT TVicLPT::GetCfgbPortBit(USHORT BitNumber) 
{
	if (m_LPTBasePort==0x3BC) return 0;
	return (GetCfgbPort() >> BitNumber) & 0x0001 ;
}

void TVicLPT::SetCfgbPortBit(USHORT BitNumber, USHORT nNewValue) 
{
	if (m_LPTBasePort==0x3BC) return;
	if (nNewValue) 
	    SetCfgbPort(GetCfgbPort() | MaskUp[BitNumber]);
	else 
        SetCfgbPort(GetCfgbPort() & MaskDown[BitNumber]);
	
}



USHORT TVicLPT::GetEcrPort() 
{
	if ((m_LPTBasePort==0x3BC) || (m_EcpBasePort==0xFFFF)) return 0;
	return m_DataReadPorts[10] = GetPort(m_EcpBasePort+2);
}

void TVicLPT::SetEcrPort(USHORT nNewValue) 
{
	if ((m_LPTBasePort==0x3BC)  || (m_EcpBasePort==0xFFFF)) return;
	SetPort(m_EcpBasePort+2, m_DataWritePorts[10] = (UCHAR)nNewValue);
}

USHORT TVicLPT::GetEcrPortBit(USHORT BitNumber) 
{
	if ((m_LPTBasePort==0x3BC)  || (m_EcpBasePort==0xFFFF)) return 0;
	return (GetEcrPort() >> BitNumber) & 0x0001 ;
}

void TVicLPT::SetEcrPortBit(USHORT BitNumber, USHORT nNewValue) 
{
	if ((m_LPTBasePort==0x3BC)  || (m_EcpBasePort==0xFFFF)) return;
	if (nNewValue) 
	    SetEcrPort(GetEcrPort() | MaskUp[BitNumber]);
	else 
        SetEcrPort(GetEcrPort() & MaskDown[BitNumber]);
}

USHORT TVicLPT::GetFifoPort() 
{
	if (m_LPTBasePort==0x3BC) return 0;
	return m_DataReadPorts[8] = GetPort(m_EcpBasePort);
}

void TVicLPT::SetFifoPort(USHORT nNewValue) 
{
	if (m_LPTBasePort==0x3BC) return;
	SetPort(m_EcpBasePort, m_DataWritePorts[8] = (UCHAR)nNewValue);
}

USHORT TVicLPT::GetCfgaPort() 
{
	if (m_LPTBasePort==0x3BC) return 0;
	return m_DataReadPorts[8] = GetPort(m_EcpBasePort);
}

void TVicLPT::SetCfgaPort(USHORT nNewValue) 
{
	if (m_LPTBasePort==0x3BC) return;	
	SetPort(m_EcpBasePort, m_DataWritePorts[8] = (UCHAR)nNewValue);
	
}

USHORT TVicLPT::GetEPPDataPortWord() 
{
    if (m_LPTBasePort==0x3BC) return 0;
	return GetPortW(m_LPTBasePort+4);
	
}

void TVicLPT::SetEPPDataPortWord(USHORT nNewValue) 
{
	if (m_LPTBasePort==0x3BC) return;
	SetPortW(m_LPTBasePort+4, nNewValue);
}

ULONG TVicLPT::GetEPPDataPortLong() 
{
	if (m_LPTBasePort==0x3BC) return 0;
	return GetPortL(m_LPTBasePort+4);
}

void TVicLPT::SetEPPDataPortLong(ULONG nNewValue) 
{
    if (m_LPTBasePort==0x3BC) return;
	SetPortL(m_LPTBasePort+4, nNewValue);
}

void TVicLPT::ReadEppByteArray(ULONG NumOfBytes, const VARIANT FAR& pByteBuffer) 
{
	ULONG   NumOfUnitsEPP;
    ULONG   PortAddrEPP;
    PVOID   pBufferEPP;
	
	if( !m_OpenDrive || m_LPTBasePort==0x3BC) return;

	NumOfUnitsEPP = NumOfBytes;
    pBufferEPP    = pByteBuffer.parray->pvData;
	PortAddrEPP   = m_LPTBasePort+4;

    _asm {
			mov ecx,NumOfUnitsEPP
	        mov edx,PortAddrEPP
	        mov edi,pBufferEPP
	        rep insb		
	}


}

void TVicLPT::ReadEppWordArray(ULONG NumOfWords, const VARIANT FAR& pWordBuffer) 
{
	ULONG   NumOfUnitsEPP;
    ULONG   PortAddrEPP;
    PVOID   pBufferEPP;

	
	if( !m_OpenDrive || m_LPTBasePort==0x3BC) return;

	
	NumOfUnitsEPP = NumOfWords;
    pBufferEPP    = pWordBuffer.parray->pvData;
	PortAddrEPP   = m_LPTBasePort+4;

	_asm {
			mov ecx,NumOfUnitsEPP
	        mov edx,PortAddrEPP
	        mov edi,pBufferEPP
	        rep insw
	}


}

void TVicLPT::ReadEppLongArray(ULONG NumOfDwords, const VARIANT FAR& pLongBuffer) 
{

	ULONG   NumOfUnitsEPP;
    ULONG   PortAddrEPP;
    PVOID   pBufferEPP;

	
	if( !m_OpenDrive || m_LPTBasePort==0x3BC) return;


	NumOfUnitsEPP = NumOfDwords;
    pBufferEPP    = pLongBuffer.parray->pvData;
	PortAddrEPP   = m_LPTBasePort+4;

	_asm {
			mov ecx,NumOfUnitsEPP
	        mov edx,PortAddrEPP
	        mov edi,pBufferEPP
	        rep insd
	}
}

void TVicLPT::WriteEppByteArray(ULONG NumOfBytes, const VARIANT FAR& pByteArray) 
{
	
	ULONG   NumOfUnitsEPP;
    ULONG   PortAddrEPP;
    PVOID   pBufferEPP;

	
	if( !m_OpenDrive || m_LPTBasePort==0x3BC) return;

	
	NumOfUnitsEPP = NumOfBytes;
    pBufferEPP    = pByteArray.parray->pvData;
	PortAddrEPP   = m_LPTBasePort+4;

	_asm {
			mov ecx,NumOfUnitsEPP
	        mov edx,PortAddrEPP
	        mov esi,pBufferEPP
	        rep outsb
	}

}

void TVicLPT::WriteEppWordArray(ULONG NumOfWords, const VARIANT FAR& pWordArray) 
{
	ULONG   NumOfUnitsEPP;
    ULONG   PortAddrEPP;
    PVOID   pBufferEPP;

	
	if( !m_OpenDrive || m_LPTBasePort==0x3BC) return;

	
	NumOfUnitsEPP = NumOfWords;
    pBufferEPP    = pWordArray.parray->pvData;
	PortAddrEPP   = m_LPTBasePort+4;

	_asm {
			mov ecx,NumOfUnitsEPP
	        mov edx,PortAddrEPP
	        mov esi,pBufferEPP
	        rep outsw
	}

}

void TVicLPT::WriteEppLongArray(ULONG NumOfDwords, const VARIANT FAR& pLongArray) 
{
	ULONG   NumOfUnitsEPP;
    ULONG   PortAddrEPP;
    PVOID   pBufferEPP;

	
	if( !m_OpenDrive || m_LPTBasePort==0x3BC) return;

	
	NumOfUnitsEPP = NumOfDwords;
    pBufferEPP    = pLongArray.parray->pvData;
	PortAddrEPP   = m_LPTBasePort+4;

	_asm {
			mov ecx,NumOfUnitsEPP
	        mov edx,PortAddrEPP
	        mov esi,pBufferEPP
	        rep outsd
	}
}

void TVicLPT::FireLptInterrupt(USHORT DataValue, USHORT StatusValue, ULONG LoTimeStamp, ULONG HiTimeStamp){};


//////////////////////////////////////////////////////////////////////
// FindLpt.cpp: implementation of the FindLpt class.
//
//////////////////////////////////////////////////////////////////////

void OpenSubKey(HKEY MainKey,
				const char *SubKey,
                HKEY* hSubKey,
                ULONG* cSubKeys,
                ULONG* NumKeys);

BOOL SearchKeyValue(HKEY hSubKey,
                    ULONG NumKeys,		
                    const char *KeyName,
                    UCHAR *OutBuffer);

void OpenSubKey(HKEY MainKey,
					 const char * SubKey,
					 HKEY* hSubKey,
					 ULONG* cSubKeys,
					 ULONG *NumKeys)
{

	char achClass[256];
    ULONG cchClassName,cbMaxSubKey;
    ULONG cchMaxClass,cbMaxValueData,cbSecurityDescriptor;
    ULONG cchMaxValue, Reserv;
    _FILETIME ftLastWriteTime;

	memset(achClass,0,sizeof(achClass));
	cchClassName = 255;
	*cSubKeys = 0;
	*NumKeys  = 0;

    Reserv = RegOpenKeyEx(
		MainKey,
		SubKey,
		0,
		KEY_READ,
		hSubKey);
	if (Reserv == ERROR_SUCCESS)
	{
		RegQueryInfoKey (*hSubKey,
			achClass,
            &cchClassName,
            NULL,
            cSubKeys,
            &cbMaxSubKey,
            &cchMaxClass,
            NumKeys,
            &cchMaxValue,
            &cbMaxValueData,
            &cbSecurityDescriptor,
            &ftLastWriteTime);
	}
	else *hSubKey = 0;
}

//-------------------------------------------------------
BOOL SearchKeyValue(HKEY hSubKey,
					ULONG NumKeys,
					const char *KeyName,
					UCHAR *OutBuffer)
{
	ULONG retValue,type_key,LBuf,cchMaxValue;
    char achValue[512];
    USHORT n;
	BOOL result;

    result = FALSE;
	if ( NumKeys > 0 ) 
	{
		for (n=0;n<= NumKeys-1 ; n++)
		{
			cchMaxValue = 512; LBuf = 512;  result =  FALSE;
			retValue = RegEnumValue(hSubKey, 
									n, 
									achValue, 
									&cchMaxValue, 
									NULL,
									&type_key, 
									OutBuffer, 
									&LBuf);
			if (retValue == ERROR_SUCCESS)
				if ( strcmp(achValue, KeyName) == 0)
					return TRUE;
		}
	}
	return result;
}

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
TFindLpt::TFindLpt()
{
  OSVERSIONINFO WinVer;

  m_NT5 = false;
  m_Win95 = (GetVersion() & 0x80000000)!=0;
  if (!m_Win95)
  {
    WinVer.dwOSVersionInfoSize = sizeof(WinVer);
    GetVersionEx(&WinVer);
    if (WinVer.dwMajorVersion>4) m_NT5=1;
  }
	
}
//-------------------------------------------------------
UCHAR TFindLpt::GetIRQCounter()
{
	return m_IRQCounter;
}
//-------------------------------------------------------
UCHAR TFindLpt::GetIRQNumber()
{
	return (BYTE)m_IRQNumber;
}
//-------------------------------------------------------
UCHAR TFindLpt::GetDMACounter()
{
	return m_DMACounter;
}
//-------------------------------------------------------
UCHAR TFindLpt::GetDMAChannel()
{
	return (BYTE) m_DMAChannel;
}
//-------------------------------------------------------
UCHAR TFindLpt::GetPortCounter()
{
	return m_PortCounter;
}
//-------------------------------------------------------
USHORT TFindLpt::GetPortBase(UCHAR nom)
{
  if ((nom==0) || (nom>m_PortCounter))
     return 0xFFFF;
  else
    return (USHORT)m_PortBase[nom];
}
//-------------------------------------------------------
USHORT TFindLpt::GetPortEnd(UCHAR nom)
{
  if ((nom==0) || (nom>m_PortCounter))
    return 0xFFFF;
  else
    return m_PortEnd[nom];
}
//-------------------------------------------------------
BOOL TFindLpt::SearchLpt(UCHAR Port_Number)
{
	sprintf(m_ClassN, "LPT%d", Port_Number);
	if (m_Win95)
		return SearchLpt_9X();
	else
		return SearchLpt_NT();
}
//-------------------------------------------------------
BOOL  TFindLpt::SearchLpt_9X()
{
	char  s[256],HWKey[256],DD[256];
    TIRQ95Config IRQConfig;
    TPort95Config PortConfig;
    TDMA95Config DMAConfig;
    char achKey[1024];
	UCHAR	Buffer[1024], LogBuffer[1024];
    ULONG KeysInDynKey,KeysInLM,i,SubKeys,nn;
    HKEY hLMKey,hDynKey,hLocDynKey;
    char KeyLMValue[256];
    ULONG BegBuffer;
    ULONG PStatus;
    TPort95Config TmpPortConfig;

	m_Devices = 0;
	
	m_IRQCounter = 0;
	m_DMACounter = 0;
	m_PortCounter = 0;

	
	memset(&IRQConfig,0,sizeof(TIRQ95Config));
	memset(&DMAConfig,0,sizeof(TDMA95Config));
	memset(&PortConfig,0, sizeof(TPort95Config));

	OpenSubKey(HKEY_DYN_DATA,"Config Manager\\Enum",&hDynKey,&SubKeys,&KeysInDynKey);

	i=0;

	while (i<SubKeys)
	{
		RegEnumKey(hDynKey,i,achKey,1024);

		strcpy(s, "Config Manager\\Enum\\");
		strcat(s, (char*)achKey);
		
		OpenSubKey(HKEY_DYN_DATA,s,&hLocDynKey,&nn,&KeysInDynKey);
		memset(Buffer,0,sizeof(Buffer));
		SearchKeyValue(hLocDynKey,KeysInDynKey,"Problem", Buffer);
		memcpy(&PStatus, Buffer, sizeof(PStatus));
		
		memset(Buffer,0,sizeof(Buffer));
		SearchKeyValue(hLocDynKey,KeysInDynKey,"HardWareKey",Buffer);
		strcpy(HWKey,(const char *)Buffer);

		memset(Buffer,0,sizeof(LogBuffer));
		SearchKeyValue(hLocDynKey,KeysInDynKey,"Allocation",LogBuffer);
		RegCloseKey(hLocDynKey);

		if (HWKey)
		{
			char tmp[256];
			strcpy(tmp,"Enum\\");
			strcat(tmp, HWKey);
			OpenSubKey(HKEY_LOCAL_MACHINE, tmp,&hLMKey,&nn,&KeysInLM);
			if (hLMKey!=0)
			{
				SearchKeyValue(hLMKey,KeysInLM,"Class",Buffer);
				strcpy(KeyLMValue,(const char *) Buffer);
				SearchKeyValue(hLMKey,KeysInLM,"DeviceDesc",Buffer);
				strcpy(DD,(const char *) Buffer);
				RegCloseKey(hLMKey);
			}
			else  memset(KeyLMValue,0,sizeof(KeyLMValue));

			
			if ((  !_strnicmp(KeyLMValue,"Port",4)    ||
				   !_strnicmp(KeyLMValue,"Ports", 5) )
					&& (PStatus==0)   && strstr(DD, m_ClassN)>0)
			{
				BegBuffer = 8;

				m_Devices++;

				while (LogBuffer[BegBuffer] > 0)
				{

					memcpy(&TmpPortConfig, &LogBuffer[BegBuffer],8);

					switch (TmpPortConfig.TypeRecord)
					{
						case ResType_IO:
							memcpy(&PortConfig, &LogBuffer[BegBuffer], sizeof(TPort95Config));
							m_PortCounter++;
							m_PortBase[m_PortCounter] = PortConfig.IOD_Alloc_Base;
							m_PortEnd[m_PortCounter]  = PortConfig.IOD_Alloc_End;
						break;
						case ResType_IRQ:
							memcpy(&IRQConfig, &LogBuffer[BegBuffer], sizeof(TIRQ95Config));
							m_IRQCounter++;
							m_IRQNumber = (BYTE)IRQConfig.IRQD_Alloc_Num;
						break;
						case ResType_DMA:
							memcpy(&DMAConfig, &LogBuffer[BegBuffer],sizeof(TDMA95Config));
							m_DMACounter++;
							m_DMAChannel = DMAConfig.DD_Alloc_Chan;
							break;
					}// case

					BegBuffer = BegBuffer+TmpPortConfig.LengthRecord;
				} // while
				
				RegCloseKey(hDynKey);
				return TRUE;
			} //if
		} //if
		i++;
	} //while
  RegCloseKey(hDynKey);
  return  m_Devices > 0;
}
//-------------------------------------------------------
BOOL TFindLpt::SearchLpt_NT()
{
	char sLevel1[256],sLevel2[256],sLevel3[256],sLevel4[256];
	char ClassString[256],DD[256];
    CM_RESOURCE_LIST ResList;
	UCHAR  i,j;
    char achKey[1024];
    ULONG KeysInKey0,KeysInKey1,KeysInKey2,KeysInKey3,KeysInKey4;
    ULONG SubKeys0,  SubKeys1,  SubKeys2,  SubKeys3,  SubKeys4;
    ULONG Level0,    Level1,    Level2;
    HKEY Key0,Key1,Key2,Key3,Key4;
    UCHAR Buffer[1024];
    BOOL Qres;

	const char sLevel0[30] = "SYSTEM\\CurrentControlSet\\Enum";  

	m_Devices   = 0;

	m_IRQCounter  = 0;
	m_DMACounter  = 0;
	m_PortCounter = 0;

	OpenSubKey(HKEY_LOCAL_MACHINE,sLevel0,&Key0,&SubKeys0,&KeysInKey0);

	if (SubKeys0 > 0)
	{
		for (Level0=0; Level0<SubKeys0 ; Level0++)
		{
			
			RegEnumKey(Key0,Level0,achKey,1024);

			sprintf(sLevel1,"%s\\%s",sLevel0, achKey);

			OpenSubKey(HKEY_LOCAL_MACHINE,sLevel1,&Key1,&SubKeys1,&KeysInKey1);

			if (SubKeys1 > 0)
			{
				for (Level1=0; Level1<SubKeys1; Level1++)
				{
					RegEnumKey(Key1,Level1,achKey,1024);
					sprintf(sLevel2,"%s\\%s",sLevel1, achKey);
					
					OpenSubKey(HKEY_LOCAL_MACHINE,sLevel2,&Key2,&SubKeys2,&KeysInKey2);

					if (SubKeys2 > 0)
					{
						for (Level2=0; Level2 < SubKeys2;Level2++ )
						{
							RegEnumKey(Key2,Level2,achKey,1024);
							sprintf(sLevel3,"%s\\%s",sLevel2, achKey);

							OpenSubKey(HKEY_LOCAL_MACHINE,sLevel3,&Key3,&SubKeys3,&KeysInKey3);
							
							memset(Buffer,0,sizeof(Buffer));
							SearchKeyValue(Key3,KeysInKey3,"Class",Buffer);

							strcpy(ClassString, (const char *)Buffer);

							if ( !SearchKeyValue(Key3,KeysInKey3,"FriendlyName",Buffer))
											SearchKeyValue(Key3,KeysInKey3,"DeviceDesc", Buffer);
							strcpy(DD,(const char *)Buffer);

							if (!_strnicmp(ClassString,"Ports",5) && strstr(DD, m_ClassN)>0)
							{
								if (!m_NT5)
									sprintf(sLevel4,"%s\\LogConf",sLevel3);
								else 
									sprintf(sLevel4,"%s\\Control",sLevel3);

								OpenSubKey(HKEY_LOCAL_MACHINE,sLevel4, &Key4,&SubKeys4,&KeysInKey4);
									
								if (!m_NT5) 
									Qres = SearchKeyValue(Key4, KeysInKey4, "BootConfig", (UCHAR*)&ResList);
								else 
									Qres = SearchKeyValue(Key4,KeysInKey4,"AllocConfig",(UCHAR*)&ResList);

								if (Qres) 
								{
									m_Devices++;
									for (i=0; i<ResList.Count; i++)	
									{
										for (j=0; j < ResList.List[i].PartialResourceList.Count;j++ )	
										{
											switch (ResList.List[i].PartialResourceList.PartialDescriptors[j].TypeResource)
											{
												case CmResourceTypePort:      // port i/o area
													m_PortCounter++;
													m_PortBase[m_PortCounter] = 
														(USHORT) ResList.List[i].PartialResourceList.PartialDescriptors[j].PortResource.Start[0];
                                                    m_PortBase[m_PortCounter] &= 0xFFFE;
													m_PortEnd[m_PortCounter]  = (USHORT)(
														ResList.List[i].PartialResourceList.PartialDescriptors[j].PortResource.Start[0]+
														ResList.List[i].PartialResourceList.PartialDescriptors[j].PortResource.Length-1);
													m_PortEnd[m_PortCounter] &= 0xFFFE;
												break;
												case CmResourceTypeInterrupt: // irq#
													m_IRQCounter++;
													m_IRQNumber = 
														(BYTE)ResList.List[i].PartialResourceList.PartialDescriptors[j].InterruptResource.Vector;
												break;
												case CmResourceTypeDma:       // DMA channel
													m_DMACounter++;
													m_DMAChannel = 
														(BYTE)ResList.List[i].PartialResourceList.PartialDescriptors[j].DMAResource.Channel;
												break;
											} //switch
										}//	for 
									}	//for
									RegCloseKey(Key3);
									RegCloseKey(Key2);
									RegCloseKey(Key1);
									RegCloseKey(Key0);
									return TRUE;
								}	//if
							} //if 
							RegCloseKey(Key3); 
						} //for 
					} //if 
					RegCloseKey(Key2);
				} //for 
			} // if 
			RegCloseKey(Key1);
		}	// for			
	}	//	if 
	RegCloseKey(Key0);
	return m_Devices > 0;
}

// ******************************************************************************
// * LOG HISTORY:
// ******************************************************************************
/*
$Log:   N:\PVCSarch65\ProjectFiles\archives\SVObserver_SRC\SVTVicLpt\VicLPT.cpp_v  $
 * 
 *    Rev 1.1   01 Dec 2014 14:21:14   tbair
 * Project:  SVObserver
 * Change Request (SCR) nbr:  960
 * SCR Title:  Pipe/core management
 * Checked in by:  tBair;  Tom Bair
 * Change Description:  
 *   Added thread attribute and label.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.0   25 Apr 2013 19:09:46   bWalter
 * Project:  SVObserver
 * Change Request (SCR) nbr:  814
 * SCR Title:  Upgrade SVObserver to Compile Using Visual Studio 2010
 * Checked in by:  bWalter;  Ben Walter
 * Change Description:  
 *   Initial check in to SVObserver_src.  (Merged with svo_src label SVO 6.10 Beta 008.)
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.2   19 Jan 2010 15:51:32   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  681
 * SCR Title:  Update PPQ to automatically threshold inspections when continuously NAKing
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Added a parameter to the Thread process method to control the wait functionality.  Updated logical flow to handle better startup functionality and initialization.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.1   20 Jun 2007 15:15:24   Joe
 * Project:  SVObserver
 * Change Request (SCR) nbr:  598
 * SCR Title:  Upgrade SVObserver to compile using vc++ in VS2005
 * Checked in by:  jSpila;  Joseph Spila
 * Change Description:  
 *   These changes include modification based on fixing compiler-based and project-based differences between VC6 and VC8.  These changes mainly include casting issues, but some include type conversion and assignment of new compiler controlling defines.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.0   03 Nov 2005 13:23:14   tbair
 * Project:  SVObserver
 * Change Request (SCR) nbr:  513
 * SCR Title:  Add Parallel Port I/O to SVObserver
 * Checked in by:  tBair;  Tom Bair
 * Change Description:  
 *   Initial Check-in
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
*/
