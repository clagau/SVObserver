//******************************************************************************
//* COPYRIGHT (c) 2003 by SVResearch, Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVOProperty
//* .File Name       : $Workfile:   SVOProperty.cpp  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.0  $
//* .Check In Date   : $Date:   23 Apr 2013 13:11:08  $
//******************************************************************************

#include "stdafx.h"
#include "SVOProperty.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CSVOProperty::CSVOProperty()
{
}

CSVOProperty::~CSVOProperty()
{
}

CSVOProperty& CSVOProperty::operator=(const CSVOProperty &source)
{
    m_sPropName = source.m_sPropName;
    m_iPropType = source.m_iPropType;
    m_iValueType = source.m_iValueType;
    m_vtPropValue = source.m_vtPropValue;
    return *this;
}

CSVOProperty* CSVOProperty::operator=(const CSVOProperty *source)
{
    m_sPropName = source->m_sPropName;
    m_iPropType = source->m_iPropType;
    m_iValueType = source->m_iValueType;
    m_vtPropValue = source->m_vtPropValue;
    return this;
}

