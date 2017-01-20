//******************************************************************************
//* COPYRIGHT (c) 2003 by SVResearch, Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVUtilityIniClass
//* .File Name       : $Workfile:   SVUtilityIniClass.h  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.0  $
//* .Check In Date   : $Date:   23 Apr 2013 15:58:30  $
//******************************************************************************

#pragma once

#include "SVUtilityLibrary/SVString.h"

class SVUtilityIniClass  
{
public:
	SVUtilityIniClass();
	virtual ~SVUtilityIniClass();

	SVUtilityIniClass( const SVUtilityIniClass& rRhs );

	const SVUtilityIniClass& operator=( const SVUtilityIniClass& rRhs);

	SVString m_DisplayName;
	SVString m_Command;
	SVString m_WorkingDirectory;
	SVString m_Arguments;
	SVString m_PromptForArguments;
};
