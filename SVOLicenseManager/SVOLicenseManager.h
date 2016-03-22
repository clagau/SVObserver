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

//Moved to precompiled header: #include <set>
#include "SVUtilityLibrary/SVGUID.h"

typedef std::set< SVGUID > SVLicenseErrorListSet;

class SVOLicenseManager
{
public:
	SVOLicenseManager(void);
	~SVOLicenseManager(void);

	static SVOLicenseManager& Instance();
	
	HRESULT InitLicenseManager();
	bool HasMatroxLicense();
	bool HasMatroxGigELicense();
	bool HasMatroxIdentificationLicense();
	void AddLicenseErrorToList(const SVGUID& svGuid);
	void ShowLicenseManagerErrors();
	bool HasToolErrors();
	void ClearLicenseErrors();
	void RemoveLicenseErrorFromList(const SVGUID& svGuid);

	//FastOCR
	void SetFastOCRLicense(bool bPresent);
	bool HasFastOCRLicense();


//private member variables
private:
	bool m_bMatroxImageLicense;
	bool m_bMatroxIdentificationLicense;
	bool m_bMatroxGigELicense;

	bool m_bFastOCR;
	SVLicenseErrorListSet m_svErrorList;

protected:
	HANDLE m_hCheckEvent;

};

inline SVOLicenseManager& TheSVOLicenseManager() {return SVOLicenseManager::Instance();}

