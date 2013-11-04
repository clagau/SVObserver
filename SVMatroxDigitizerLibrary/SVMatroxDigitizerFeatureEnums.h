//******************************************************************************
//* COPYRIGHT (c) 2009 by Seidenader Vision Inc., Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVMatroxDigitizerFeatureEnums
//* .File Name       : $Workfile:   SVMatroxDigitizerFeatureEnums.h  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.0  $
//* .Check In Date   : $Date:   22 Apr 2013 14:27:32  $
//******************************************************************************

#ifndef SVMATROXDIGITIZERFEATUREENUMS_H
#define SVMATROXDIGITIZERFEATUREENUMS_H

#include "SVMatroxLibrary/SVMatroxTypedefs.h"
#include "SVMatroxLibrary/SVMatroxEnumConvertor.h"

/**
@SVObjectName Matrox Digitizer Feature Enum

@SVObjectOverview This enum is used to get/set the Gige "features" of the Digitizer.

@SVObjectOperations None

*/

struct SVMatroxDigitizerFeature
{
	// Feature Type
	enum SVFeatureTypeEnum
	{
		SVTypeCommand,				// M_TYPE_COMMAND
		SVTypeInt32,				// M_TYPE_MIL_INT32
		SVTypeDouble,				// M_TYPE_DOUBLE
		SVTypeString,				// M_TYPE_STRING
		SVTypeIntegerEnumeration,	// M_TYPE_INTEGER_ENUMERATION
		SVTypeStringEnumeration,	// M_TYPE_STRING_ENUMERATION
		SVTypeBool,					// M_TYPE_BOOLEAN
	};
	// define convertor for from/to SVEnum/Matrox types
	typedef SVMatroxEnumConvertor<SVFeatureTypeEnum, long> SVFeatureTypeEnumMap;
	static SVFeatureTypeEnumMap m_FeatureTypeEnumConvertor;
};

#endif

//******************************************************************************
//* LOG HISTORY:
//******************************************************************************
/*
$Log:   N:\PVCSarch65\ProjectFiles\archives\SVObserver_SRC\SVMatroxDigitizerLibrary\SVMatroxDigitizerFeatureEnums.h_v  $
 * 
 *    Rev 1.0   22 Apr 2013 14:27:32   bWalter
 * Project:  SVObserver
 * Change Request (SCR) nbr:  814
 * SCR Title:  Upgrade SVObserver to Compile Using Visual Studio 2010
 * Checked in by:  bWalter;  Ben Walter
 * Change Description:  
 *   Initial check in to SVObserver_src.  (Merged with svo_src label SVO 6.10 Beta 008.)
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.0   29 Jul 2009 16:12:28   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  665
 * SCR Title:  Fix unrecoverable failure when processing acquisitions at high speed
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Merged branced changes into current code branch with appropriate updates.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.0   02 Jun 2009 15:30:46   sjones
 * Project:  SVObserver
 * Change Request (SCR) nbr:  650
 * SCR Title:  Integrate Gigabit ethernet cameras
 * Checked in by:  sJones;  Steve Jones
 * Change Description:  
 *   Initial Checkin
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
*/