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

namespace SvLib
{

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

SVUtilityIniClass::SVUtilityIniClass( const SVUtilityIniClass& rRhs )
{
	*this = rRhs;
}

const SVUtilityIniClass& SVUtilityIniClass::operator =( const SVUtilityIniClass& rRhs )
{
	if ( &rRhs != this ) 
	{
		m_DisplayName = rRhs.m_DisplayName;
		m_Command = rRhs.m_Command;
		m_WorkingDirectory = rRhs.m_WorkingDirectory;
		m_Arguments = rRhs.m_Arguments;
		m_PromptForArguments = rRhs.m_PromptForArguments;
	}

	return *this;
}

} //namespace SvLib
