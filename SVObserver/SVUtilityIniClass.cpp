//******************************************************************************
//* COPYRIGHT (c) 2003 by SVResearch, Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVUtilityIniClass
//* .File Name       : $Workfile:   SVUtilityIniClass.cpp  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.0  $
//* .Check In Date   : $Date:   23 Apr 2013 15:58:16  $
//******************************************************************************

// SVUtilityIniClass.cpp: implementation of the SVUtilityIniClass class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "SVUtilityIniClass.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

SVUtilityIniClass::SVUtilityIniClass()
{
	m_csDisplayName = "";
	m_csCommand = "";
	m_csWorkingDirectory = "";
	m_csArguments = "";
	m_csPromptForArguments = "";
}

SVUtilityIniClass::~SVUtilityIniClass()
{

}
SVUtilityIniClass::SVUtilityIniClass( const SVUtilityIniClass& p_rObject )
{
	*this = p_rObject;
}

const SVUtilityIniClass& SVUtilityIniClass::operator =( const SVUtilityIniClass& p_rObject )
{
	if ( &p_rObject != this ) 
	{
		m_csDisplayName = p_rObject.m_csDisplayName;
		m_csCommand = p_rObject.m_csCommand;
		m_csWorkingDirectory = p_rObject.m_csWorkingDirectory;
		m_csArguments = p_rObject.m_csArguments;
		m_csPromptForArguments = p_rObject.m_csPromptForArguments;
	}

	return *this;
}

