//******************************************************************************
//* COPYRIGHT (c) 2003 by SVResearch, Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVOInspectionObj
//* .File Name       : $Workfile:   SVOInspectionObj.cpp  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.0  $
//* .Check In Date   : $Date:   23 Apr 2013 13:04:30  $
//******************************************************************************

#include "stdafx.h"
#include "SVOInspectionObj.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

SVOInspectionObj::SVOInspectionObj() :
	m_NameChanged( false )
,	m_NewInspection( false )
,	m_sNewDisableMethod( _T( "Method 1" ) )
,	m_Color( false )
,	m_ShowAuxExtent( true )
{
}

SVOInspectionObj::~SVOInspectionObj()
{
}

CString SVOInspectionObj::GetInspectionLabelName() const
{
    return m_sLabelName;
}

void SVOInspectionObj::SetInspectionLabelName(CString sLabel)
{
    m_sLabelName = sLabel;
}

CString SVOInspectionObj::GetInspectionName() const
{
    return m_sInspectionName;
}

void SVOInspectionObj::SetInspectionName(CString sInspection)
{
    m_sInspectionName = sInspection;
}

CString SVOInspectionObj::GetToolsetImage() const
{
    return m_sToolsetImage; 
}

void SVOInspectionObj::SetToolsetImage(CString sImage)
{
    m_sToolsetImage = sImage;
} 

CString SVOInspectionObj::GetNewDisableMethod() const
{
    return m_sNewDisableMethod;
}

void SVOInspectionObj::SetNewDisableMethod(CString sDisable)
{
    m_sNewDisableMethod = sDisable;
} 

bool SVOInspectionObj::GetShowAuxExtent() const
{
	return m_ShowAuxExtent;
}

void SVOInspectionObj::SetShowAuxExtent( bool ShowAuxExtent )
{
	 m_ShowAuxExtent = ShowAuxExtent;
}

long SVOInspectionObj::GetEnableAuxiliaryExtent() const
{
    return m_lEnableAuxiliaryExtent; 
}

void SVOInspectionObj::SetEnableAuxiliaryExtent( long  lEnable)
{
    m_lEnableAuxiliaryExtent = lEnable;
} 

bool SVOInspectionObj::HasInspectionNameChange() const
{
    return m_NameChanged;
}

CString SVOInspectionObj::GetOrginalInspectionName() const
{
    return m_sOrginalName;
}

void SVOInspectionObj::RenameInspection(CString sNewName)
{
    if (!m_NameChanged)
    {
        //only change once...
        m_NameChanged = true;
        m_sOrginalName = m_sInspectionName;
    }
    m_sInspectionName = sNewName;
}

bool SVOInspectionObj::IsNewInspection() const
{
	return m_NewInspection;
}

void SVOInspectionObj::SetNewInspection()
{
    m_NewInspection = true;
}

const CString& SVOInspectionObj::GetImportFilename() const
{
	return m_ImportFilename;
}

void SVOInspectionObj::SetImportFilename(const CString& filename)
{
	m_ImportFilename = filename;
}

void SVOInspectionObj::ClearImportFilename()
{
	m_ImportFilename.Empty();
}

bool SVOInspectionObj::IsColor() const
{
	return m_Color;
}

void SVOInspectionObj::SetColor( bool Color )
{
	m_Color = Color;
}

