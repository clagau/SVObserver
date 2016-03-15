// ******************************************************************************
// * COPYRIGHT (c) 2009 by SVResearch, Harrisburg
// * All Rights Reserved
// ******************************************************************************
// * .Module Name     : SVCIServerStruct
// * .File Name       : $Workfile:   SVCIServerStruct.cpp  $
// * ----------------------------------------------------------------------------
// * .Current Version : $Revision:   1.0  $
// * .Check In Date   : $Date:   18 Apr 2013 17:59:06  $
// ******************************************************************************

#include "stdafx.h"
#include "SVCIServerStruct.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

/*
This constructor initializes all attribute data to desired defaults.
*/
SVCIServerStruct::SVCIServerStruct()
{
	m_hCorServer = CORHANDLE_NULL;
}

/*
This copy constructor uses the operator= method to copy the attributes from the provided object into the current one.
*/
SVCIServerStruct::SVCIServerStruct( const SVCIServerStruct &p_rValue )
{
	*this = p_rValue;
}

/*
This destructor is a placeholder and performs no actions at this time.
*/
SVCIServerStruct::~SVCIServerStruct()
{
}

/*
This operator copies all of the internal attributes from the provided object into the current one.
*/
const SVCIServerStruct &SVCIServerStruct::operator=( const SVCIServerStruct &p_rValue )
{
	m_csName = (LPCTSTR)p_rValue.m_csName;

	m_hCorServer = p_rValue.m_hCorServer;

	return *this;
}

