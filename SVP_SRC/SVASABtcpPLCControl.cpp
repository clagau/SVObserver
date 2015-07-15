//******************************************************************************
//* COPYRIGHT (c) 2008 by SVResearch, Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVASABtcpPLCControl
//* .File Name       : $Workfile:   SVASABtcpPLCControl.cpp  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.0  $
//* .Check In Date   : $Date:   25 Apr 2013 15:46:52  $
//******************************************************************************

#include "stdafx.h"
#include "objbase.h"

#include "SVASABtcpPLCControl.h"
#include "PLCEnglish\Resource.h"
#include "SVStatusLibrary/SVException.h"
#include "SVMessage\SVMessage.h"


#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

// Constructor
SVASABtcpPLCControl::SVASABtcpPLCControl()
: m_pCompleteEventHandler( NULL ), 
m_bControlCreated(false)
{
	// Get Com ready.
	//CoInitialize(NULL );
	CoInitializeEx( NULL, COINIT_MULTITHREADED );
	m_pCallback = NULL;
	m_pCurrentBlock = NULL;
}

// PLCControl destructor
SVASABtcpPLCControl::~SVASABtcpPLCControl()
{
	Destroy();
	m_bControlCreated = false;
   OutputDebugString( "Exiting ~SVASABtcpPLCControl\n" );
}

// Error String utility function.
CString ErrorString(DWORD err)
{
	CString Error;
	LPTSTR s;
	if(::FormatMessage(FORMAT_MESSAGE_ALLOCATE_BUFFER |
		FORMAT_MESSAGE_FROM_SYSTEM,
		NULL,
		err,
		0,
		(LPTSTR)&s,
		0,
		NULL) == 0)
	{ /* failed */
		// Unknown error code %08x (%d)
		CString fmt;
		CString t;
		fmt.LoadString(IDS_UNKNOWN_ERROR);
		t.Format(fmt, err, LOWORD(err));
		Error = t;
	} /* failed */
	else
	{ /* success */
		LPTSTR p = _tcschr(s, _T('\r'));
		if(p != NULL)
		{ /* lose CRLF */
			*p = _T('\0');
		} /* lose CRLF */
		Error = s;
		::LocalFree(s);
	} /* success */
	return Error;
} // ErrorString


// Hard Coded Runtime License key...
// Obtained from the LICREQST.exe program on a development system 
// or call function ClassFactory2:RequestLicKey from a development system.
/*
Copyright (c) 2001 Automated Solutions, Inc. All Rights Reserved. {309A0581-304B-11d5-8112-0050DA2CCA1A}
*/
WCHAR SVASABtcpPLCControl::pwchLicenseKey[] =
{
	0x0043,	0x006F,	0x0070,	0x0079,	0x0072,	0x0069,	
	0x0067,	0x0068,	0x0074,	0x0020,	0x0028,	0x0063,	
	0x0029,	0x0020,	0x0032,	0x0030,	0x0030,	0x0031,	
	0x0020,	0x0041,	0x0075,	0x0074,	0x006F,	0x006D,	
	0x0061,	0x0074,	0x0065,	0x0064,	0x0020,	0x0053,	
	0x006F,	0x006C,	0x0075,	0x0074,	0x0069,	0x006F,	
	0x006E,	0x0073,	0x002C,	0x0020,	0x0049,	0x006E,	
	0x0063,	0x002E,	0x0020,	0x0041,	0x006C,	0x006C,	
	0x0020,	0x0052,	0x0069,	0x0067,	0x0068,	0x0074,	
	0x0073,	0x0020,	0x0052,	0x0065,	0x0073,	0x0065,	
	0x0072,	0x0076,	0x0065,	0x0064,	0x002E,	0x0020,	
	0x007B,	0x0033,	0x0030,	0x0039,	0x0041,	0x0030,	
	0x0035,	0x0038,	0x0031,	0x002D,	0x0033,	0x0030,	
	0x0034,	0x0042,	0x002D,	0x0031,	0x0031,	0x0064,	
	0x0035,	0x002D,	0x0038,	0x0031,	0x0031,	0x0032,	
	0x002D,	0x0030,	0x0030,	0x0035,	0x0030,	0x0044,	
	0x0041,	0x0032,	0x0043,	0x0043,	0x0041,	0x0031,	
	0x0041,	0x007D
};

HRESULT SVASABtcpPLCControl::Create()
{
	// ***** Create an instance of an object which implements Complete Event. 
	// ***** SmartPointer...

	GUID clsid = __uuidof( Asabtcp ); 

	// Get Class Factory
	IClassFactory* pCF = NULL;
	HRESULT l_hr = CoGetClassObject( clsid, CLSCTX_ALL, 
				 NULL, IID_IClassFactory, (void**)&pCF);

	IClassFactory2* pCF2 = NULL;
	if( l_hr == S_OK )
	{
		// Query for ClassFactory2
		l_hr = pCF->QueryInterface(IID_IClassFactory2, (void **)&pCF2);
		pCF->Release();
	}

	IUnknown * pObj = NULL;
	if( l_hr == S_OK )
	{
		// Use ClassFactory2 to CreateInstance with license.
		BSTR l_bstLic = ::SysAllocStringLen( pwchLicenseKey, sizeof(pwchLicenseKey)/sizeof(WCHAR) );
		l_hr = pCF2->CreateInstanceLic( NULL, NULL, IID_IUnknown , 
									 l_bstLic, (void **)&pObj); 
		pCF2->Release();
		::SysFreeString( l_bstLic );
	}


	_DAsabtcp * l_pASAB=NULL;
	if( l_hr == S_OK )
	{
		// Query for the IDispatch which is the interface to the asabtcp control.
		l_hr = pObj->QueryInterface( IID_IDispatch, (void **) &m_spASABtcp);
		// Release the temperary pointer.
		pObj->Release();
	}



	if( l_hr == S_OK )
	{
		// Initialize Control..... This seems to be needed on controls that are not on a window.
		// *************************************
		IPersistStreamInit* l_pStreamInit=NULL;
		m_spASABtcp->QueryInterface( IID_IPersistStreamInit, (void**)&l_pStreamInit );
		l_hr = l_pStreamInit->InitNew();
		l_pStreamInit->Release();
		// *************************************

		if( l_hr == S_OK)
		{
			// ***** Instantiate an IEventFiringObjectEventHandler object. *****
			m_pCompleteEventHandler = new ICompleteEventHandler(*this, m_spASABtcp, 
											&SVASABtcpPLCControl::OnEventFiringObjectInvoke);
			m_bControlCreated = true;
		}
		else
		{
			SVException svE;
			SETEXCEPTION1(svE, SVMSG_PLC_FAILED, ErrorString( l_hr ) );
			svE.LogException( );
		}
	}
	else
	{
		SVException svE;
		SETEXCEPTION1(svE, SVMSG_PLC_FAILED, ErrorString( l_hr ) );
		svE.LogException( );
	}
	return l_hr;
}

HRESULT SVASABtcpPLCControl::Destroy()
{
	// ***** When the program is terminating, make sure that we instruct our *****
	// ***** Event Handler to disconnect from the connection point of the *****
	// ***** object which implemented the IEventFiringObject interface. *****
	// ***** We also needs to Release() it (instead of deleting it). *****
	HRESULT hr = S_OK;
	if (m_pCompleteEventHandler)
	{
	  m_pCompleteEventHandler-> ShutdownConnectionPoint();
	  m_pCompleteEventHandler-> Release();
	  m_pCompleteEventHandler = NULL;
	}
	if( m_bControlCreated )
	{
		hr = m_spASABtcp->Disconnect();
		// Remove reference to resource.
		m_bControlCreated = false;
		OutputDebugString("Released ASABtcp Control\n");
	}
	return hr;
}

// Setup Some PLC stuff...
HRESULT SVASABtcpPLCControl::SetupPLC(LPCTSTR p_szNodeAddress, long p_lTransactionTimeout)
{
	HRESULT l_hr = -4108;
	if( m_bControlCreated )
	{
		// Add Validation of node address
		// Add validation of timeout.
		m_spASABtcp->PutTimeoutConnect( 1000 );
		m_spASABtcp->PutNodeAddress( p_szNodeAddress );
		m_spASABtcp->PutTimeoutTrans( static_cast<short>(p_lTransactionTimeout) );
		l_hr = S_OK;
	}
	return l_hr;
}

// Returns the number of Bytes for the given data type.
long SVASABtcpPLCControl::DataTypeSize (SVPLCDataTypeEnum aDataType)
{
   long  lRetValue;

   switch (aDataType)
   {
	   case SV_PLC_BIT:
      {
         lRetValue = 2;
         break;
      }
	   case SV_PLC_INT16:
      {
         lRetValue = 2;
         break;
      }

	   case SV_PLC_INT32:
      {
         lRetValue = 4;
         break;
      }

	   case SV_PLC_STRING:
      {
         lRetValue = 82;
         break;
      }
	   case SV_PLC_SPFLOAT:
      {
         lRetValue = 4;
         break;
      }
	   default:
      {
         lRetValue = 2;	// Default Data Size. Least common denominator
         break;
      }
   }
   return lRetValue;
}


HRESULT SVASABtcpPLCControl::WriteBlock(SVPLCBlockStruct* p_pBlock, long p_lTimeout, void* p_pCallback)
{
	HRESULT l_hr = -3104;	// Control is Busy...  We will have to queue up the transactions 
							// if this starts happening.
	if( m_spASABtcp->GetBusy() == 0 )
	{
		short l_lLength=1;
		ASABTCPLib::enumFunction l_eFunc;
		switch (p_pBlock->svmPLCDataType)
		   {
				case SV_PLC_BIT:
				{
					l_eFunc = FUNC_AB_SLC500_WRITE_WORD;
					l_lLength = 1;
					break;
				}
				case SV_PLC_INT16:
				{
					l_eFunc = FUNC_AB_SLC500_WRITE_WORD;
					l_lLength = 1;
					break;
				}
				case SV_PLC_INT32:
				{
					l_eFunc = FUNC_AB_SLC500_WRITE_WORD;
					l_lLength = 2;
					break;
				}
				case SV_PLC_STRING:
				{
					l_eFunc = FUNC_AB_SLC500_WRITE_WORD;
					l_lLength = 41;	// Words or 82 characters
					break;
				}
				case SV_PLC_SPFLOAT:
				{
					l_eFunc = FUNC_AB_SLC500_WRITE_WORD;
					l_lLength = 2;
					break;
				}
				default:
				{
					break;
				}
		   }
		   // Set the appropriate function.
		   m_spASABtcp->PutFunction( l_eFunc );

		   // Set the starting PLC memory address.
		   m_spASABtcp->PutMemStart( p_pBlock->svmRebuiltDTAddress );

		   // Set the transfer length.
		   m_spASABtcp->PutMemQty( static_cast<short>( p_pBlock->svmlNbrElements * l_lLength ) );

		   while( p_pBlock->svmlNbrElements * l_lLength > 128 )
		   {
			   p_pBlock->svmlNbrElements --;
			   ASSERT(0);
		   }
		   // Copy the data
		   if( p_pBlock->svmPLCDataType == SV_PLC_STRING )
		   {
			   // Temperary Hack to see whats happening.
			   if( strlen( p_pBlock->svmcpTxRxBuffer ) > (size_t)DataTypeSize(SV_PLC_STRING ) )
			   {
				   TRACE("********** String Length Error *********");
			   }
			   p_pBlock->svmcpTxRxBuffer[81]=0;
			   _bstr_t l_bstTmp( p_pBlock->svmcpTxRxBuffer );
			   m_spASABtcp->PutDataString( l_bstTmp );
		   }
		   else
		   {
			   for( int i = 0 ; i < p_pBlock->svmlNbrElements * l_lLength ; i++ )
			   {
				   short l_Temp = *(reinterpret_cast<short*>(p_pBlock->svmcpTxRxBuffer) + i );
				   m_spASABtcp->PutDataWord( i, l_Temp );
			   }
		   }

		   // set the current block pointer
		   m_pCurrentBlock = p_pBlock;

		   // Set Operation
		   m_bReadOperation = false;

		   m_pCallback = reinterpret_cast<ASAB_IO_CALLBACK_PROC>(p_pCallback);
		   // Initiate the transaction... a callback will occur when finished.
		   l_hr = m_spASABtcp->AsyncRefresh();
		   long l_lCountOut = 0;
		   while( p_pBlock->svmulRawStatus == 0xffff )
		   {
			   l_lCountOut++;
			   Sleep(15);
			   if( l_lCountOut > 100 )
			   {
				   break;
			   }
		   }
	}
	return l_hr;
}

HRESULT SVASABtcpPLCControl::ReadBlock(SVPLCBlockStruct* p_pBlock, long p_lTimeout, void* p_pCallback)
{
	HRESULT l_hr = -4109;	// Control is Busy...  We will have to queue up the transactions 
							// if this starts happening.
	if( m_spASABtcp->GetBusy() == 0 )
	{
		short l_lLength=1;
		ASABTCPLib::enumFunction l_eFunc;
		   switch (p_pBlock->svmPLCDataType)
		   {
				case SV_PLC_BIT:
				{
					l_eFunc = FUNC_AB_SLC500_READ_WORD;
					l_lLength = 1;
					break;
				}
				case SV_PLC_INT16:
				{
					l_eFunc = FUNC_AB_SLC500_READ_WORD;
					l_lLength = 1;
					break;
				}
				case SV_PLC_INT32:
				{
					l_eFunc = FUNC_AB_SLC500_READ_WORD;
					l_lLength = 2;
					break;
				}
				case SV_PLC_STRING:
				{
					l_eFunc = FUNC_AB_SLC500_READ_WORD;
					l_lLength = 41;
					break;
				}
				case SV_PLC_SPFLOAT:
				{
					l_eFunc = FUNC_AB_SLC500_READ_WORD;
					l_lLength = 2;
					break;
				}
				default:
				{
					break;
				}
		   }
		   // Set the appropriate function.
		   m_spASABtcp->PutFunction( l_eFunc );

		   // Set the starting PLC memory address.
		   m_spASABtcp->PutMemStart( p_pBlock->svmRebuiltDTAddress );

		   // Set the transfer length.
		   m_spASABtcp->PutMemQty( l_lLength );

		   // Set the callback pointer.
		   m_pCallback = reinterpret_cast<ASAB_IO_CALLBACK_PROC>(p_pCallback);

		   // set the current block pointer
		   m_pCurrentBlock = p_pBlock;

		   // Set Operation
		   m_bReadOperation = true;

		   // Initiate the transaction... a callback will occur when finished.
		   l_hr = m_spASABtcp->AsyncRefresh();
	}
	return l_hr;
}


// Here is the complete Event...
HRESULT SVASABtcpPLCControl::OnEventFiringObjectInvoke
(
  ICompleteEventHandler* pEventHandler,
  DISPID dispidMember, 
  REFIID riid,
  LCID lcid, 
  WORD wFlags, 
  DISPPARAMS* pdispparams, 
  VARIANT* pvarResult,
  EXCEPINFO* pexcepinfo, 
  UINT* puArgErr
)
{
	if (dispidMember == 0x01)  // Event1 event.
	{
		// 1st param : [in] long lValue.
		VARIANT	varlValue;
		unsigned long	lValue = 0;

		VariantInit(&varlValue);
		VariantClear(&varlValue);

		varlValue = (pdispparams -> rgvarg)[0];

		lValue = V_I4(&varlValue);

		if( lValue == 0 )
		{
			if( m_bReadOperation )
			{
				TRACE(_T("Successfull Read\n"));
				long l_Words =  DataTypeSize( m_pCurrentBlock->svmPLCDataType )/2;
				if( m_spASABtcp->GetMemQty() != l_Words )
				{
					TRACE("Warning ... Sizes do not Match");
				}
				for( int x = 0 ; x < l_Words ; x++ )
				{
					short l_sWord;
					l_sWord = m_spASABtcp->GetDataWordM( x );
					short* l_pWord = reinterpret_cast<short*>(&m_pCurrentBlock->svmcpTxRxBuffer[x*2]);
					*l_pWord = l_sWord;
				}
			}
		}
		else
		{
			// Error code from ASABtcp control.
			short l_sError = m_spASABtcp->GetResult();
			TCHAR buf[512];
			LPCTSTR l_szTmp = GetControlErrorString( &buf[0], 512, l_sError );
			SVException svE;
			SETEXCEPTION1(svE, SVMSG_PLC_FAILED, l_szTmp );
			svE.LogException( );
		}

		m_pCallback( reinterpret_cast<unsigned long>(m_pCurrentBlock), lValue );
	}

	return S_OK;
}

bool SVASABtcpPLCControl::IsValid()
{
	return m_bControlCreated;
}


LPCTSTR SVASABtcpPLCControl::GetControlErrorString( TCHAR* buf, long l_lBufSize, short l_sError )
{

	_bstr_t l_bstTmp = m_spASABtcp->GetResultString();

	long l_length = l_bstTmp.length();

	if( l_length < l_lBufSize )
	{
		_tcscpy_s(buf, l_lBufSize, (LPCTSTR)l_bstTmp );
	}
	else
	{
		ASSERT(0);
	}
	return buf;
}

HRESULT SVASABtcpPLCControl::TestCommunication()
{
	WORD l_tmpData = 0;
	WORD l_tmpData2 = 0;
	HRESULT l_hr = S_OK;
	VARIANT_BOOL l_vbRet = m_spASABtcp->GetBusy();
	if( l_vbRet == VARIANT_FALSE )
	{
		// Do two reads of the free running clock(S:4) to make sure we get a different result.  
		// If we get the same data for both reads then there is a problem.
		m_spASABtcp->Function = FUNC_AB_SLC500_READ_WORD;
		m_spASABtcp->MemQty = 1;
		m_spASABtcp->MemStart = "S:4"; 
		l_hr = m_spASABtcp->SyncRefresh();
		l_tmpData = m_spASABtcp->GetDataWordM(0);
		l_hr = m_spASABtcp->SyncRefresh();
		l_tmpData2 = m_spASABtcp->GetDataWordM(0);
		if( l_tmpData == l_tmpData2)
		{
			l_hr = S_FALSE;
		}
	}
	else
	{
		l_hr = S_FALSE;
	}
	return l_hr;
}

// ******************************************************************************
// * LOG HISTORY:
// ******************************************************************************
/*
$Log:   N:\PVCSarch65\ProjectFiles\archives\SVObserver_src\SVP_SRC\SVASABtcpPLCControl.cpp_v  $
 * 
 *    Rev 1.0   25 Apr 2013 15:46:52   bWalter
 * Project:  SVObserver
 * Change Request (SCR) nbr:  814
 * SCR Title:  Upgrade SVObserver to Compile Using Visual Studio 2010
 * Checked in by:  bWalter;  Ben Walter
 * Change Description:  
 *   Initial check in to SVObserver_src.  (Merged with svo_src label SVO 6.10 Beta 008.)
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
*/