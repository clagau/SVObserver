// ******************************************************************************
// * COPYRIGHT (c) 2000 by SVResearch, Harrisburg
// * All Rights Reserved
// ******************************************************************************
// * .Module Name     : SecuredObject
// * .File Name       : $Workfile:   SecuredObject.cpp  $
// * ----------------------------------------------------------------------------
// * .Current Version : $Revision:   1.1  $
// * .Check In Date   : $Date:   01 Oct 2013 09:57:48  $
// ******************************************************************************
#pragma region Includes
#include "stdafx.h"
#include "SecuredObject.h"
#include "SVStatusLibrary/SVRegistry.h"
#pragma endregion Includes

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

static BOOL CALLBACK EnumChildKeys(LPCTSTR p_szKey, void* ptr)
{
	SVString value;

	SVSecuredObject* obj = (SVSecuredObject*)ptr;
	obj->m_key = obj->m_szParentKey + _T("\\") + p_szKey;
	obj->m_key.MakeLower();
	SVRegistryClass reg(obj->m_key);
	if (reg.GetRegistryValue(_T("groups"), value))
		obj->m_map.SetAt(obj->m_key, value.c_str());

	return TRUE;
}


static BOOL CALLBACK EnumParentKeys(LPCTSTR p_szKey, void* ptr)
{
	DWORD value = 0;

	SVSecuredObject* obj = (SVSecuredObject*)ptr;
	obj->m_szParentKey = obj->m_szAppKey + _T("\\") + p_szKey;
	SVRegistryClass reg(obj->m_szParentKey);

	reg.EnumKeys(EnumChildKeys, ptr);
	return TRUE;
}


static BOOL CALLBACK EnumApplicationKeys(LPCTSTR p_szKey, void* ptr)
{
	SVSecuredObject* obj = (SVSecuredObject*)ptr;
	obj->m_szAppKey.Format(_T("\\%s\\security"), p_szKey);
	SVRegistryClass reg(obj->m_szAppKey);
	if (reg.CreatedNewKey())
	{
		reg.DeleteKey();
		return TRUE;
	}
	
	reg.EnumKeys(EnumParentKeys, ptr);
	return TRUE;
}

IMPLEMENT_SERIAL(SVSecuredObject, CObject, 1)

SVSecuredObject::SVSecuredObject() : CObject()
{
}

SVSecuredObject::~SVSecuredObject()
{
}

CString SVSecuredObject::Lookup(LPCTSTR szApp, int nParent, int nChild, BOOL bCreate/*=FALSE*/, LPCTSTR groups/*=nullptr*/)
{
	CString key;
	CString	value = _T("Everybody");

	key.Format(_T("\\%s\\security\\%0.5u\\%0.5u"), szApp, nParent, nChild);
	key.MakeLower();
	if (!bCreate && m_map.IsEmpty())
	{
		SVRegistryClass reg(key);
		if (reg.CreatedNewKey())
		{
			reg.DeleteKey();
		}
		else
		{
			SVString l_Temp = _T("Everybody");

			reg.GetRegistryValue(_T("groups"), l_Temp);

			value = l_Temp.c_str();
		}
	}
	else
	{
		BOOL bFound = m_map.Lookup(key, value);
		if (!bFound && bCreate)
		{
			m_bModified = TRUE;
			m_map.SetAt(key, groups);
			value = groups;
		}
		if (bFound && bCreate)
		{
			m_bModified = TRUE;
			m_map.SetAt(key, groups);
			value = groups;
		}
	}

	return value;
}

void SVSecuredObject::Load(LPCTSTR szApp/*=nullptr*/, int nParent/*=0*/)
{
	if (!m_map.IsEmpty())
		m_map.RemoveAll();

	if (nullptr == szApp)
	{
		m_key = _T("\\");
		SVRegistryClass reg(m_key);
		reg.EnumKeys(EnumApplicationKeys, (void*)this);
	}
	else if (nParent == 0)
	{
		m_szAppKey.Format(_T("\\%s\\security"), szApp);
		SVRegistryClass reg(m_szAppKey);
		if (reg.CreatedNewKey())
		{
			reg.DeleteKey();
			return;
		}
		reg.EnumKeys(EnumParentKeys, (void*)this);
	}
	else
	{
		m_szParentKey.Format(_T("\\%s\\security\\%0.5u"), szApp, nParent);
		SVRegistryClass reg(m_szParentKey);
		if (reg.CreatedNewKey())
		{
			reg.DeleteKey();
			return;
		}
		reg.EnumKeys(EnumChildKeys, (void*)this);
	}
	m_bModified = FALSE;
}

void SVSecuredObject::Save()
{
	CString	key;
	CString	value;

	if (!m_bModified || m_map.IsEmpty())
		return;

	INT_PTR count = m_map.GetCount();
	POSITION pos = m_map.GetStartPosition();
	do
	{
		m_map.GetNextAssoc(pos, key, value);
		SVRegistryClass	reg(key);
		if (value == _T("Everybody"))
			reg.DeleteKey();
		else
			reg.SetRegistryValue(CString(_T("groups")), value);
	}
	while (--count);
	m_bModified = FALSE;
}

BOOL SVSecuredObject::IsSecured(DWORD nParent, DWORD nChild)
{
	return Lookup(AfxGetAppName(), nParent, nChild) != _T("Everybody") ? TRUE : FALSE;
}

BOOL SVSecuredObject::Validate(CString strAccount, CString strPassword, DWORD nParent, DWORD nChild)
{
	LPLOCALGROUP_USERS_INFO_0	pBuf = nullptr;
	LPLOCALGROUP_USERS_INFO_0	pTmpBuf = nullptr;
	NET_API_STATUS				nas = NERR_Success;
	BOOL						bResult = true;
	HANDLE						phToken = nullptr;
	DWORD						entriesread = 0;
	DWORD						totalentries = 0;

	USES_CONVERSION;

	CString groups = Lookup(AfxGetAppName(), nParent, nChild);
	if (!groups.IsEmpty() && groups != _T("Everybody"))
	{
		if (LogonUser((TCHAR *)(LPCTSTR)strAccount,
					  _T("."),
					  (TCHAR *)(LPCTSTR)strPassword,
					  LOGON32_LOGON_NETWORK,
					  LOGON32_PROVIDER_DEFAULT,
					  &phToken))
		{
			CloseHandle(phToken);
			nas = NetUserGetLocalGroups(nullptr,
										T2W((TCHAR *)(LPCTSTR) strAccount),
										0,
										LG_INCLUDE_INDIRECT,
										(LPBYTE*)&pBuf,
										-1,
										&entriesread,
										&totalentries);
			if (NERR_Success == nas && nullptr != (pTmpBuf = pBuf))
			{
				bResult = false;
				for (DWORD i=0; i<entriesread; i++)
				{
					CString group = pTmpBuf->lgrui0_name;
					int nStart = -1, nFinish;
					do
					{
						nFinish = groups.Find(_T(','), ++nStart);
						if (-1 == nFinish)
						{
							nFinish = groups.GetLength();
						}

						if (groups.Mid(nStart, (nFinish-nStart)) == group)
						{
							bResult = true;
							break;
						}
						nStart = nFinish;
					}
					while (nStart < groups.GetLength());
					if (bResult)
						break;
					pTmpBuf++;
				}
			}
		}
		else
		{
			bResult = false;
		}

		if (nullptr != pBuf)
		{
			NetApiBufferFree(pBuf);
		}
	}
	return bResult;
}

