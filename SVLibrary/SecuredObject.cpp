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

#include "stdafx.h"
#include "SecuredObject.h"
#include "SVStatusLibrary/SVRegistry.h"

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
		obj->m_map.SetAt(obj->m_key, value.ToString());

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

CString SVSecuredObject::Lookup(LPCTSTR szApp, int nParent, int nChild, BOOL bCreate/*=FALSE*/, LPCTSTR groups/*=NULL*/)
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

			value = l_Temp.ToString();
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

void SVSecuredObject::Load(LPCTSTR szApp/*=NULL*/, int nParent/*=0*/)
{
	if (!m_map.IsEmpty())
		m_map.RemoveAll();

	if (szApp == NULL)
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
	LPLOCALGROUP_USERS_INFO_0	pBuf = NULL;
	LPLOCALGROUP_USERS_INFO_0	pTmpBuf = NULL;
	NET_API_STATUS				nas = NERR_Success;
	BOOL						bResult = TRUE;
	HANDLE						phToken = NULL;
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
			nas = NetUserGetLocalGroups(NULL,
										T2W((TCHAR *)(LPCTSTR) strAccount),
										0,
										LG_INCLUDE_INDIRECT,
										(LPBYTE*)&pBuf,
										-1,
										&entriesread,
										&totalentries);
			if (nas == NERR_Success && (pTmpBuf = pBuf) != NULL)
			{
				bResult = FALSE;
				for (DWORD i=0; i<entriesread; i++)
				{
					CString group = pTmpBuf->lgrui0_name;
					int nStart = -1, nFinish;
					do
					{
						nFinish = groups.Find(_T(','), ++nStart);
						if (nFinish == -1)
							nFinish = groups.GetLength();

						if (groups.Mid(nStart, (nFinish-nStart)) == group)
						{
							bResult = TRUE;
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
			bResult = FALSE;

		if (pBuf != NULL)
			NetApiBufferFree(pBuf);
	}
	
	return bResult;
}

// ******************************************************************************
// * LOG HISTORY:
// ******************************************************************************
/*
$Log:   N:\PVCSarch65\ProjectFiles\archives\SVObserver_SRC\SVLibrary\SecuredObject.cpp_v  $
 * 
 *    Rev 1.1   01 Oct 2013 09:57:48   tbair
 * Project:  SVObserver
 * Change Request (SCR) nbr:  852
 * SCR Title:  Add Multiple Platform Support to SVObserver's Visual Studio Solution
 * Checked in by:  tBair;  Tom Bair
 * Change Description:  
 *   Add x64 platform.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.0   22 Apr 2013 13:30:32   bWalter
 * Project:  SVObserver
 * Change Request (SCR) nbr:  814
 * SCR Title:  Upgrade SVObserver to Compile Using Visual Studio 2010
 * Checked in by:  bWalter;  Ben Walter
 * Change Description:  
 *   Initial check in to SVObserver_src.  (Merged with svo_src label SVO 6.10 Beta 008.)
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.6   22 Oct 2010 08:10:16   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  704
 * SCR Title:  Upgrade SVObserver Version for 5.33 Release
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated source code to fix include problems and compilier efficiency.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.5   15 Dec 2009 10:32:48   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  677
 * SCR Title:  Fix problem in camera notify thread
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated include information and comments.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.4   03 Sep 2009 08:28:04   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  665
 * SCR Title:  Fix unrecoverable failure when processing acquisitions at high speed
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Fixed issues with moved header includes.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.3   22 Feb 2001 16:23:40   mike
 * Project:  SVFocusNT
 * Change Request (SCR) nbr:  113
 * SCR Title:  SVIMServer fails if path of destination file does not exist
 * Checked in by:  Mike;  Mike McCarl
 * Change Description:  
 *   Modified to create a non-unicode version to operate with svobserver
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.2   24 Jul 2000 17:17:22   mike
 * Project:  SVFocusNT
 * Change Request (SCR) nbr:  19
 * SCR Title:  Module dependency analysis
 * Checked in by:  Mike;  Mike McCarl
 * Change Description:  
 *   Updated Model Ids
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.1   19 Jul 2000 12:24:48   mike
 * Project:  SVFocusNT
 * Change Request (SCR) nbr:  11
 * SCR Title:  Stage 2: Security
 * Checked in by:  cSchmittinger;  Carl Schmittinger
 * Change Description:  
 *   Updated code for finding substrings within a string.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.0   18 Jul 2000 10:08:00   mike
 * Project:  SVFocusNT
 * Change Request (SCR) nbr:  11
 * SCR Title:  Stage 2: Security
 * Checked in by:  cSchmittinger;  Carl Schmittinger
 * Change Description:  
 *   Secured Object class using resource Id's and the registry for access to program functions
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
*/
