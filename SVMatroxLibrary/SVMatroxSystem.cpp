//******************************************************************************
//* COPYRIGHT (c) 2009 by Körber Pharma Inspection GmbH. All Rights Reserved
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVMatroxSystem
//* .File Name       : $Workfile:   SVMatroxSystem.cpp  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.0  $
//* .Check In Date   : $Date:   22 Apr 2013 15:27:28  $
//******************************************************************************

#include "stdafx.h"
#include "SVMatroxSystem.h"
#include "SVMatroxSystemInterface.h"

SVMatroxSystem::SVMatroxSystem()
: m_SystemIdentifier(0)
{
}

SVMatroxSystem::~SVMatroxSystem()
{
	LocalClear();
}

bool SVMatroxSystem::operator<( const SVMatroxSystem& p_rSystem ) const 
{
	return m_SystemIdentifier < p_rSystem.m_SystemIdentifier; 
}

bool SVMatroxSystem::empty() const
{
	return ( m_SystemIdentifier == 0 );
}

void SVMatroxSystem::LocalClear()
{
	SVMatroxSystemInterface::Destroy(*this);
}

