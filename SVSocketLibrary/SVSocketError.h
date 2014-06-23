//******************************************************************************
//* COPYRIGHT (c) 2011 by Seidenader Vision Inc., Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVSocketError
//* .File Name       : $Workfile:   SVSocketError.h  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.1  $
//* .Check In Date   : $Date:   19 Jun 2014 15:48:06  $
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

		private:
			SVSocketError();
			~SVSocketError();
		};
	}
}

typedef Seidenader::Socket::SVSocketError SVSocketError;

//******************************************************************************
//* LOG HISTORY:
//******************************************************************************
/*
$Log:   N:\PVCSarch65\ProjectFiles\archives\SVObserver_SRC\SVSocketLibrary\SVSocketError.h_v  $
 * 
 *    Rev 1.1   19 Jun 2014 15:48:06   bwalter
 * Project:  SVObserver
 * Change Request (SCR) nbr:  886
 * SCR Title:  Add RunReject Server Support to SVObserver
 * Checked in by:  rYoho;  Rob Yoho
 * Change Description:  
 *   Merged changes from SVRemoteControl project.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
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
