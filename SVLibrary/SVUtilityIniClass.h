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


namespace SvLib
{

class SVUtilityIniClass  
{
public:
	SVUtilityIniClass() = default;
	virtual ~SVUtilityIniClass() = default;

	SVUtilityIniClass( const SVUtilityIniClass& rRhs ) = default;

	SVUtilityIniClass& operator=( const SVUtilityIniClass& rRhs) = default;

	std::string m_DisplayName;
	std::string m_Command;
	std::string m_WorkingDirectory;
	std::string m_Arguments;
	std::string m_PromptForArguments;
};

} //namespace SvLib
