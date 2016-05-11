// ******************************************************************************
// * COPYRIGHT (c) 2000 by SVResearch, Harrisburg
// * All Rights Reserved
// ******************************************************************************
// * .Module Name     : SecuredObject
// * .File Name       : $Workfile:   SecuredObject.h  $
// * ----------------------------------------------------------------------------
// * .Current Version : $Revision:   1.0  $
// * .Check In Date   : $Date:   22 Apr 2013 13:30:36  $
// ******************************************************************************

#pragma once

class SVSecuredObject : public CObject
{
	DECLARE_SERIAL(SVSecuredObject)

public:
	SVSecuredObject();
	virtual ~SVSecuredObject();

public:
	CString Lookup(LPCTSTR szApp, int nParent, int nChild, BOOL bCreate=FALSE, LPCTSTR groups=nullptr);
	void  Load(LPCTSTR szApp=nullptr, int nParent=0);
	void  Save();
	BOOL  IsSecured(DWORD nParent, DWORD nChild=0);
	BOOL  Validate(CString strAccount, CString strPassword, DWORD nParent, DWORD nChild=0);

private:
	friend BOOL CALLBACK EnumChildKeys(LPCTSTR p_szKey, void* ptr);
	friend BOOL CALLBACK EnumParentKeys(LPCTSTR p_szKey, void* ptr);
	friend BOOL CALLBACK EnumApplicationKeys(LPCTSTR p_szKey, void* ptr);

private:
	CMapStringToString	m_map;
	CString				m_key;
	BOOL				m_bModified;
	CString				m_szAppKey;
	CString				m_szParentKey;

};

