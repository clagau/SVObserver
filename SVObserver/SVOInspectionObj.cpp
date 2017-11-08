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
,	m_NewDisableMethod( _T( "Method 1" ) )
,	m_Color( false )
,	m_ShowAuxExtent( true )
{
}

SVOInspectionObj::~SVOInspectionObj()
{
}

const std::string& SVOInspectionObj::GetInspectionLabelName() const
{
    return m_LabelName;
}

void SVOInspectionObj::SetInspectionLabelName(LPCTSTR Label)
{
    m_LabelName = Label;
}

const std::string& SVOInspectionObj::GetInspectionName() const
{
    return m_InspectionName;
}

void SVOInspectionObj::SetInspectionName(LPCTSTR Inspection)
{
    m_InspectionName = Inspection;
}

const std::string& SVOInspectionObj::GetToolsetImage() const
{
    return m_ToolsetImage; 
}

void SVOInspectionObj::SetToolsetImage(LPCTSTR Image)
{
    m_ToolsetImage = Image;
} 

const std::string& SVOInspectionObj::GetNewDisableMethodString() const
{
    return m_NewDisableMethod;
}

void SVOInspectionObj::SetNewDisableMethod(LPCTSTR Disable)
{
    m_NewDisableMethod = Disable;
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

const std::string& SVOInspectionObj::GetOrginalInspectionName() const
{
    return m_OrginalName;
}

void SVOInspectionObj::RenameInspection(LPCTSTR NewName)
{
    if (!m_NameChanged)
    {
        //only change once...
        m_NameChanged = true;
        m_OrginalName = m_InspectionName;
    }
    m_InspectionName = NewName;
}

bool SVOInspectionObj::IsNewInspection() const
{
	return m_NewInspection;
}

void SVOInspectionObj::SetNewInspection()
{
    m_NewInspection = true;
}

const std::string& SVOInspectionObj::GetImportFilename() const
{
	return m_ImportFilename;
}

void SVOInspectionObj::SetImportFilename(LPCTSTR Filename)
{
	m_ImportFilename = Filename;
}

void SVOInspectionObj::ClearImportFilename()
{
	m_ImportFilename.clear();
}

bool SVOInspectionObj::IsColor() const
{
	return m_Color;
}

void SVOInspectionObj::SetColor( bool Color )
{
	m_Color = Color;
}

