//******************************************************************************
//* COPYRIGHT (c) 2009 by Seidenader Vision Inc., Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVMatroxDigitizerFeatureEnums
//* .File Name       : $Workfile:   SVMatroxDigitizerFeatureEnums.cpp  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.0  $
//* .Check In Date   : $Date:   22 Apr 2013 14:27:26  $
//******************************************************************************
#include "stdafx.h"
#include "SVMatroxDigitizerFeatureEnums.h"
#include <boost/config.hpp>
#include <boost/assign/list_of.hpp>
#include <mil.h>

// Assign mappings for Feature Types
SVMatroxDigitizerFeature::SVFeatureTypeEnumMap SVMatroxDigitizerFeature::m_FeatureTypeEnumConvertor = boost::assign::map_list_of<>
(SVMatroxDigitizerFeature::SVTypeCommand,				M_TYPE_COMMAND)
(SVMatroxDigitizerFeature::SVTypeInt32,					M_TYPE_MIL_INT32)
(SVMatroxDigitizerFeature::SVTypeDouble,				M_TYPE_DOUBLE)
(SVMatroxDigitizerFeature::SVTypeString,				M_TYPE_STRING)
(SVMatroxDigitizerFeature::SVTypeIntegerEnumeration,	M_TYPE_INTEGER_ENUMERATION)
(SVMatroxDigitizerFeature::SVTypeStringEnumeration,		M_TYPE_STRING_ENUMERATION)
(SVMatroxDigitizerFeature::SVTypeBool,					M_TYPE_BOOLEAN);

//******************************************************************************
//* LOG HISTORY:
//******************************************************************************
/*
$Log:   N:\PVCSarch65\ProjectFiles\archives\SVObserver_SRC\SVMatroxDigitizerLibrary\SVMatroxDigitizerFeatureEnums.cpp_v  $
 * 
 *    Rev 1.0   22 Apr 2013 14:27:26   bWalter
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
