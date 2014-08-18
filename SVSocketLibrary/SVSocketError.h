//******************************************************************************
//* COPYRIGHT (c) 2011 by Seidenader Vision Inc., Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVSocketError
//* .File Name       : $Workfile:   SVSocketError.h  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.2  $
//* .Check In Date   : $Date:   15 Aug 2014 14:12:34  $
//******************************************************************************

#pragma once

namespace Seidenader
{
	namespace Socket
	{
		class SVSocketError
		{
		public:
			enum ErrorEnum
			{
				Success,
				Interrupted,
				PermissionDenied,
				InvalidPointerAddress,
				InvalidArgument,
				TooManyOpenSockets,
				WouldBlock,
				OperationInProgress,
				OperationAlreadyInProgress,
				NotASocket,
				DestinationAddressRequired,
				MessageTooLong,
				ProtocolWrong,
				BadProtocolOption,
				ProtocolNotSupported,
				SocketTypeNotSupported,
				OperationNotSupported,
				ProtocolFamilyNotSupported,
				AddressFamilyNotSupported,
				AddressInUse,
				AddressNotAvailable,
				NetworkDown,
				NetworkUnreachable,
				NetworkReset,
				ConnectionAborted,
				ConnectionReset,
				NoBufferSpaceAvailable,
				AlreadyConnected,
				NotConnected,
				Shutdown,
				ConnectionTimeout,
				ConnectionRefused,
				HostDown,
				HostUnreachable,
				ToManyProcesses,
				SystemNotReady,
				VersionNotSupported,
				NotInitialised,
				ShutdownInProgress,
				ClassTypeNotFound,
				HostNotFound,
				TryAgain,
				NoRecovery,
				NoData,
				InvalidHandle,
				InvalidParameter,
				IOIncomplete,
				IOPending,
				NotEnoughMemory,
				OperationAborted,
				InvalidProcTable,
				InvalidProvider,
				ProviderFailedInit,
				CallFailure,
				AlreadyCreated,
	 			Unknown
			};
			static SVSocketError::ErrorEnum GetLastSocketError();
			static SVSocketError::ErrorEnum TranslateError(int errorCode);
			static const char * GetErrorText(ErrorEnum err);
			static HRESULT HrFromSocketError(ErrorEnum err);

		private:
			SVSocketError();
			~SVSocketError();
		};
	}
}

typedef Seidenader::Socket::SVSocketError SVSocketError;

#if _MSC_VER < 1700
#define INET_E_INVALID_URL					_HRESULT_TYPEDEF_(0x800C0002L)
#define INET_E_NO_SESSION					_HRESULT_TYPEDEF_(0x800C0003L)
#define INET_E_CANNOT_CONNECT				_HRESULT_TYPEDEF_(0x800C0004L)
#define INET_E_RESOURCE_NOT_FOUND			_HRESULT_TYPEDEF_(0x800C0005L)
#define INET_E_OBJECT_NOT_FOUND				 _HRESULT_TYPEDEF_(0x800C0006L)
#define INET_E_DATA_NOT_AVAILABLE			_HRESULT_TYPEDEF_(0x800C0007L)
#define INET_E_DOWNLOAD_FAILURE				_HRESULT_TYPEDEF_(0x800C0008L)
#define INET_E_CONNECTION_TIMEOUT			_HRESULT_TYPEDEF_(0x800C000BL)
#define INET_E_INVALID_REQUEST				 _HRESULT_TYPEDEF_(0x800C000CL)
#define INET_E_UNKNOWN_PROTOCOL				_HRESULT_TYPEDEF_(0x800C000DL)
#define INET_E_CANNOT_LOAD_DATA				 _HRESULT_TYPEDEF_(0x800C000FL)
#define E_INVALID_PROTOCOL_OPERATION		_HRESULT_TYPEDEF_(0x83760001L)
#define E_INVALID_PROTOCOL_FORMAT			_HRESULT_TYPEDEF_(0x83760002L)
#define E_PROTOCOL_EXTENSIONS_NOT_SUPPORTED _HRESULT_TYPEDEF_(0x83760003L)
#define E_SUBPROTOCOL_NOT_SUPPORTED			_HRESULT_TYPEDEF_(0x83760004L)
#define E_PROTOCOL_VERSION_NOT_SUPPORTED	_HRESULT_TYPEDEF_(0x83760005L)

#endif

//******************************************************************************
//* LOG HISTORY:
//******************************************************************************
/*
$Log:   N:\PVCSarch65\ProjectFiles\archives\SVObserver_SRC\SVSocketLibrary\SVSocketError.h_v  $
 * 
 *    Rev 1.2   15 Aug 2014 14:12:34   jHanebach
 * Project:  SVObserver
 * Change Request (SCR) nbr:  886
 * SCR Title:  Add RunReject Server Support to SVObserver
 * Checked in by:  rYoho;  Rob Yoho
 * Change Description:  
 *   Syncing with svrc code base
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.1   24 Mar 2014 10:58:16   jHanebach
 * Project: SVRemoteControl
 * Change Request (SCR) nbr: 10
 * SCR Title: Add reject/run page server connection and commands
 * Checked in by: jHanebach; Jack Hanebach
 * Change Description: Changes for run/reject server.
 * \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\
 * 
 *    Rev 1.0   25 Apr 2013 17:16:44   bWalter
 * Project:  SVObserver
 * Change Request (SCR) nbr:  814
 * SCR Title:  Upgrade SVObserver to Compile Using Visual Studio 2010
 * Checked in by:  bWalter;  Ben Walter
 * Change Description:  
 *   Initial check in to SVObserver_src.  (Merged with svo_src label SVO 6.10 Beta 008.)
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.1   14 Feb 2013 11:02:56   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  812
 * SCR Title:  Add New Remote Command Functionality
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Update code to include error messages and changes thread name from standard string to basic string.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.0   19 Dec 2011 13:54:36   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  744
 * SCR Title:  Add Shared Memory and Socket Functionality for Run Page Web Server
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Initial Check-in.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
*/
