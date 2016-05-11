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

class SVUtilityIniClass  
{
public:
	SVUtilityIniClass();
	virtual ~SVUtilityIniClass();

	SVUtilityIniClass( const SVUtilityIniClass& p_rObject );

	const SVUtilityIniClass& operator=( const SVUtilityIniClass& p_rObject );

	CString m_csDisplayName;
	CString m_csCommand;
	CString m_csWorkingDirectory;
	CString m_csArguments;
	CString m_csPromptForArguments;
};
