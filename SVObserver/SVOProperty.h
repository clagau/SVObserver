//******************************************************************************
//* COPYRIGHT (c) 2003 by SVResearch, Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVOProperty
//* .File Name       : $Workfile:   SVOProperty.h  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.0  $
//* .Check In Date   : $Date:   23 Apr 2013 13:11:20  $
//******************************************************************************

#if !defined(AFX_SVOPROPERTY_H__C50A4485_4D23_42E7_90B6_A71055789957__INCLUDED_)
#define AFX_SVOPROPERTY_H__C50A4485_4D23_42E7_90B6_A71055789957__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

class CSVOProperty  
{
public:
	CSVOProperty();
	virtual ~CSVOProperty();

    CSVOProperty& operator=(const CSVOProperty& source);
    CSVOProperty* operator=(const CSVOProperty* source);

public:
    CString m_sPropName;
    int m_iPropType;
    int m_iValueType;
    VARIANT m_vtPropValue;
};

#endif // !defined(AFX_SVOPROPERTY_H__C50A4485_4D23_42E7_90B6_A71055789957__INCLUDED_)

