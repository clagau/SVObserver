//******************************************************************************
//* COPYRIGHT (c) 2005 by SVResearch, Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVOLicenseManager
//* .File Name       : $Workfile:   SVOLicenseManager.h  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.1  $
//* .Check In Date   : $Date:   06 May 2013 14:15:48  $
//******************************************************************************

#pragma once

#pragma region Includes
//Moved to precompiled header: #include <set>
#pragma endregion Includes

class SVOLicenseManager
{
public:
	SVOLicenseManager();
	~SVOLicenseManager();

	static SVOLicenseManager& Instance();
	
	HRESULT InitLicenseManager();
	bool HasMatroxLicense()  const;
	bool HasMatroxGigELicense()  const;
	bool HasMatroxIdentificationLicense()  const;
	void AddLicenseErrorToList(uint32_t objectId);
	void ShowLicenseManagerErrors();
	bool HasToolErrors()  const;
	void ClearLicenseErrors();
	void RemoveLicenseErrorFromList(uint32_t objectId);
	
//private member variables
private:
	bool m_bMatroxImageLicense;
	bool m_bMatroxIdentificationLicense;
	bool m_bMatroxGigELicense;

	std::set<uint32_t> m_svErrorList;

protected:
	HANDLE m_hCheckEvent;
};

