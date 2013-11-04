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

#ifndef SVO_LICENSE_MANAGER
#define SVO_LICENSE_MANAGER

#include "SVUtilityLibrary/SVGUID.h"
#include <set>

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

#endif

//******************************************************************************
//* LOG HISTORY:
//******************************************************************************
/*
$Log:   N:\PVCSarch65\ProjectFiles\archives\SVObserver_src\SVOLicenseManager\SVOLicenseManager.h_v  $
 * 
 *    Rev 1.1   06 May 2013 14:15:48   bWalter
 * Project:  SVObserver
 * Change Request (SCR) nbr:  814
 * SCR Title:  Upgrade SVObserver to Compile Using Visual Studio 2010
 * Checked in by:  bWalter;  Ben Walter
 * Change Description:  
 *   Merged with svo_src label SVO 6.10 Beta 009.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.3   10 Apr 2013 14:24:46   ryoho
 * Project:  SVObserver
 * Change Request (SCR) nbr:  796
 * SCR Title:  Add a License Manager that will handle the Matrox Licenses
 * Checked in by:  rYoho;  Rob Yoho
 * Change Description:  
 *   - added the methods for the Fast OCR License
 * - new method RemoveLicenseErrorFromList
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.0   25 Apr 2013 12:40:46   bWalter
 * Project:  SVObserver
 * Change Request (SCR) nbr:  814
 * SCR Title:  Upgrade SVObserver to Compile Using Visual Studio 2010
 * Checked in by:  bWalter;  Ben Walter
 * Change Description:  
 *   Initial check in to SVObserver_src.  (Merged with svo_src label SVO 6.10 Beta 008.)
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.2   27 Mar 2013 15:01:56   ryoho
 * Project:  SVObserver
 * Change Request (SCR) nbr:  810
 * SCR Title:  Remove support for Legacy Tools
 * Checked in by:  rYoho;  Rob Yoho
 * Change Description:  
 *   made changes to the includes
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.1   25 Mar 2013 12:37:38   ryoho
 * Project:  SVObserver
 * Change Request (SCR) nbr:  796
 * SCR Title:  Add a License Manager that will handle the Matrox Licenses
 * Checked in by:  rYoho;  Rob Yoho
 * Change Description:  
 *   added the methods - AddLicenseErrorToList, HasToolErros and ClearLicenseErrors, added the use of a modeless dialog to display the license errors
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.0   27 Feb 2013 09:56:06   ryoho
 * Project:  SVObserver
 * Change Request (SCR) nbr:  796
 * SCR Title:  Add a License Manager that will handle the Matrox Licenses
 * Checked in by:  rYoho;  Rob Yoho
 * Change Description:  
 *   first iteration of file
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
*/
 