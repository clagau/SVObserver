// ******************************************************************************
// * COPYRIGHT (c) 2007 by SVResearch, Harrisburg
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
#include "SVMatroxTypedefs.h"
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
	typedef SVMatroxStatusInformation::SVStatusCode SVStatusCode;

	static void Startup();
	static void Shutdown();

	static SVStatusCode GetLastStatus();
	static SVStatusCode GetLastStatus( SVMatroxStatusInformation& p_rStatus );

	static SVStatusCode GetFirstError();
	static SVStatusCode GetFirstError( SVMatroxStatusInformation& p_rStatus );

	static void Log( SVMatroxStatusInformation& p_rStatusInfo );
	static void LogMatroxException();
	static SVStatusCode GetSystemCount( long& p_lCount );
	static SVStatusCode GetSystemName( SVMatroxInt p_lSystemNumber, SVString& p_rSystemName );
	static HRESULT SVMatroxIntToHRESULT( SVMatroxIdentifier p_Int );

protected:
   static SVMatroxInt _stdcall SVMatroxHookHandler( SVMatroxInt HookType, SVMatroxIdentifier EventId, void* UserDataPtr );

private:
	static void LocalInitialize();
	static void LocalClear();

	SVMatroxApplicationInterface();
	SVMatroxApplicationInterface( const SVMatroxApplicationInterface& p_rObject );
	const SVMatroxApplicationInterface& operator=( const SVMatroxApplicationInterface& p_rObject );
};

