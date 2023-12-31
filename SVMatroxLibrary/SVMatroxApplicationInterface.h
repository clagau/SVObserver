// ******************************************************************************
// * COPYRIGHT (c) 2007 by K�rber Pharma Inspection GmbH. All Rights Reserved
// * All Rights Reserved
// ******************************************************************************
// * .Module Name     : SVMatroxApplicationInterface
// * .File Name       : $Workfile:   SVMatroxApplicationInterface.h  $
// * ----------------------------------------------------------------------------
// * .Current Version : $Revision:   1.1  $
// * .Check In Date   : $Date:   01 Oct 2013 11:08:08  $
// ******************************************************************************

#pragma once

#pragma region Includes
#include "SVMatroxStatusInformation.h"
#pragma endregion Includes

/**
@SVObjectName Matrox Application Interface

@SVObjectOverview This class provides an interface for the Matrox Application operations.

@SVObjectOperations The GetLastStatus function returns a SVMatroxStatusInformation with the most recent status.
The GetFirstError returns a SVMatroxStatusInformation which is filled with information of the oldest error information since this function was last read.

*/
class SVMatroxApplicationInterface
{
public:

	static void Startup();
	static void Shutdown();

	static HRESULT GetLastStatus();
	static HRESULT GetLastStatus( SVMatroxStatusInformation& p_rStatus );

	static HRESULT GetFirstError();
	static HRESULT GetFirstError( SVMatroxStatusInformation& p_rStatus );

	static void Log( SVMatroxStatusInformation& p_rStatusInfo );
	static void LogMatroxException();
	static HRESULT GetSystemCount( long& p_lCount );
	static HRESULT GetSystemName( long long p_lSystemNumber, std::string& p_rSystemName );
	static HRESULT SVMatroxIntToHRESULT( __int64 p_Int );

protected:
   static long long _stdcall SVMatroxHookHandler( long long HookType, __int64 EventId, void* UserDataPtr );

private:
	static void LocalInitialize();
	static void LocalClear();

	SVMatroxApplicationInterface();
	SVMatroxApplicationInterface( const SVMatroxApplicationInterface& p_rObject );
	const SVMatroxApplicationInterface& operator=( const SVMatroxApplicationInterface& p_rObject );
};

