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

#if !defined(AFX_SVUTILITYINICLASS_H__ABB010B7_F3C1_424E_8E4D_647A85C14D82__INCLUDED_)
#define AFX_SVUTILITYINICLASS_H__ABB010B7_F3C1_424E_8E4D_647A85C14D82__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

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

#endif // !defined(AFX_SVUTILITYINICLASS_H__ABB010B7_F3C1_424E_8E4D_647A85C14D82__INCLUDED_)

