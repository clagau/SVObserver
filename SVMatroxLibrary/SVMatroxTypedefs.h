// ******************************************************************************
// * COPYRIGHT (c) 2007 by SVResearch, Harrisburg
// * All Rights Reserved
// ******************************************************************************
// * .Module Name     : SVMatroxTypedefs
// * .File Name       : $Workfile:   SVMatroxTypedefs.h  $
// * ----------------------------------------------------------------------------
// * .Current Version : $Revision:   1.2  $
// * .Check In Date   : $Date:   23 Oct 2013 08:50:46  $
// ******************************************************************************

#ifndef SV_MATROX_TYPEDEFS_H
#define SV_MATROX_TYPEDEFS_H

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include <string>
#include <vector>

/**
@SVObjectName Matrox Identifier

@SVObjectOverview

@SVObjectOperations None

*/
#ifdef _WIN64
   typedef long long SVMatroxIdentifier;
   typedef long long SVMatroxInt;
#else
   typedef long SVMatroxIdentifier;
   typedef long SVMatroxInt;
#endif


/**
@SVObjectName Matrox Status Code

@SVObjectOverview

@SVObjectOperations None

*/
//typedef long SVMatroxStatusCode;
typedef HRESULT SVMatroxStatusCode;

/**
@SVObjectName Matrox Status String

@SVObjectOverview

@SVObjectOperations None

*/
typedef std::string SVMatroxStatusString;

/**
@SVObjectName Matrox String

@SVObjectOverview

@SVObjectOperations None

*/
typedef std::string SVMatroxString;

/**
@SVObjectName Matrox Hook Function Pointer

@SVObjectOverview

@SVObjectOperations None

*/
typedef SVMatroxIdentifier ( _stdcall *SVMatroxHookFunctionPtr )( SVMatroxIdentifier, SVMatroxIdentifier, void* );

/**
@SVObjectName Matrox double array

@SVObjectOverview This is an array of doubles.

@SVObjectOperations None

*/
typedef std::vector<double> SVMatroxDoubleArray;


/**
@SVObjectName Matrox long array

@SVObjectOverview This is an array of doubles.

@SVObjectOperations None

*/
typedef std::vector<long> SVMatroxLongArray;

/**
@SVObjectName Matrox long array

@SVObjectOverview This is an array of doubles.

@SVObjectOperations None

*/
typedef std::vector<BYTE> SVMatroxByteArray;


#endif // SV_MATROX_TYPEDEFS_H

// ******************************************************************************
// * LOG HISTORY:
// ******************************************************************************
/*
$Log:   N:\PVCSarch65\ProjectFiles\archives\SVObserver_SRC\SVMatroxLibrary\SVMatroxTypedefs.h_v  $
 * 
 *    Rev 1.2   23 Oct 2013 08:50:46   tbair
 * Project:  SVObserver
 * Change Request (SCR) nbr:  852
 * SCR Title:  Add Multiple Platform Support to SVObserver's Visual Studio Solution
 * Checked in by:  tBair;  Tom Bair
 * Change Description:  
 *   Modified types to be compatible with 32 and 64bit librarys.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.1   01 Oct 2013 11:15:30   tbair
 * Project:  SVObserver
 * Change Request (SCR) nbr:  852
 * SCR Title:  Add Multiple Platform Support to SVObserver's Visual Studio Solution
 * Checked in by:  tBair;  Tom Bair
 * Change Description:  
 *   Add x64 platform.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.0   22 Apr 2013 15:28:58   bWalter
 * Project:  SVObserver
 * Change Request (SCR) nbr:  814
 * SCR Title:  Upgrade SVObserver to Compile Using Visual Studio 2010
 * Checked in by:  bWalter;  Ben Walter
 * Change Description:  
 *   Initial check in to SVObserver_src.  (Merged with svo_src label SVO 6.10 Beta 008.)
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.0   23 Jul 2007 15:22:34   tbair
 * Project:  SVObserver
 * Change Request (SCR) nbr:  597
 * SCR Title:  Upgrade Matrox Imaging Library to version 8.0
 * Checked in by:  tBair;  Tom Bair
 * Change Description:  
 *   Initial Check-in
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
*/
