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

namespace Seidenader { namespace SVSocketLibrary
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
		static LPCTSTR GetErrorText(ErrorEnum err);
		static HRESULT HrFromSocketError(ErrorEnum err);

	private:
		SVSocketError();
		~SVSocketError();
	};
} /*namespace SVSocketLibrary*/ } /*namespace Seidenader*/

namespace SvSol = Seidenader::SVSocketLibrary;

#if _MSC_VER < 1700
#define INET_E_INVALID_URL					_HRESULT_TYPEDEF_(0x800C0002L)
#define INET_E_NO_SESSION					_HRESULT_TYPEDEF_(0x800C0003L)
#define INET_E_CANNOT_CONNECT				_HRESULT_TYPEDEF_(0x800C0004L)
#define INET_E_RESOURCE_NOT_FOUND			_HRESULT_TYPEDEF_(0x800C0005L)
#define INET_E_OBJECT_NOT_FOUND				_HRESULT_TYPEDEF_(0x800C0006L)
#define INET_E_DATA_NOT_AVAILABLE			_HRESULT_TYPEDEF_(0x800C0007L)
#define INET_E_DOWNLOAD_FAILURE				_HRESULT_TYPEDEF_(0x800C0008L)
#define INET_E_CONNECTION_TIMEOUT			_HRESULT_TYPEDEF_(0x800C000BL)
#define INET_E_INVALID_REQUEST				_HRESULT_TYPEDEF_(0x800C000CL)
#define INET_E_UNKNOWN_PROTOCOL				_HRESULT_TYPEDEF_(0x800C000DL)
#define INET_E_CANNOT_LOAD_DATA				_HRESULT_TYPEDEF_(0x800C000FL)
#define E_INVALID_PROTOCOL_OPERATION		_HRESULT_TYPEDEF_(0x83760001L)
#define E_INVALID_PROTOCOL_FORMAT			_HRESULT_TYPEDEF_(0x83760002L)
#define E_PROTOCOL_EXTENSIONS_NOT_SUPPORTED _HRESULT_TYPEDEF_(0x83760003L)
#define E_SUBPROTOCOL_NOT_SUPPORTED			_HRESULT_TYPEDEF_(0x83760004L)
#define E_PROTOCOL_VERSION_NOT_SUPPORTED	_HRESULT_TYPEDEF_(0x83760005L)
#endif

