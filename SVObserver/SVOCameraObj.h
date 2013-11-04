//******************************************************************************
//* COPYRIGHT (c) 2003 by SVResearch, Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVOCameraObj
//* .File Name       : $Workfile:   SVOCameraObj.h  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.0  $
//* .Check In Date   : $Date:   23 Apr 2013 12:49:02  $
//******************************************************************************

#ifndef SVOCAMERAOBJ_H
#define SVOCAMERAOBJ_H

#include "SVOMFCLibrary/SVDeviceParamCollection.h"
#include "SVUtilityLibrary/SVString.h"

class CSVOCameraObj  
{
public:
	CSVOCameraObj();
	virtual ~CSVOCameraObj();
    CSVOCameraObj& operator =(const CSVOCameraObj &source);
    CSVOCameraObj* operator =(const CSVOCameraObj *source);

    CString GetCameraDisplayName() const;
    CString GetCameraFile() const;
    int GetDigNumber() const;
    int GetBandNumber() const;
	const SVDeviceParamCollection& GetCameraFileParams() const;
	SVDeviceParamCollection& GetCameraFileParamsNonConst();
	const SVDeviceParamCollection& GetCameraDeviceParams();
	SVDeviceParamCollection& GetCameraDeviceParamsNonConst();

    void SetCameraDisplayName(CString sCameraName);
    void SetDigNumber(int iDigNumber);
    void SetCameraFile(CString sFileName);
	void SetCameraFileChanged();
	BOOL GetCameraFileChanged();
    void SetBandNumber(int iBandNumber);
	void SetCameraFileParams(const SVDeviceParamCollection& rParams);
	void SetCameraDeviceParams(const SVDeviceParamCollection& rParams);
    BOOL IsCameraObjOk();

	bool IsFileAcquisition() const;
	void SetFileAcquisitionMode(bool bFileAcquisition);
	bool IsFileImageSizeEditModeFileBased() const;
	void SetFileImageSizeEditModeFileBased(bool bScanFile);
	long GetFileImageWidth() const;
	void SetFileImageWidth(long width);
	long GetFileImageHeight() const;
	void SetFileImageHeight(long height);
	const SIZE& GetFileImageSize() const;
	void SetFileImageSize(const SIZE& size);
	LPCTSTR GetImageFilename() const;
	void SetImageFilename(const SVString& filename);
	LPCTSTR GetImageDirectoryName() const;
	void SetImageDirectoryName(const SVString& directoryName);
	long GetFileLoadingMode() const;
	void SetFileLoadingMode(long mode);
	bool IsSoftwareTriggerSupported() const;
	bool IsCameraLineInputSupported() const;

private:  //data members
    CString m_sCameraDisplayName; 
    int m_iDigNumber;
    int m_iBandNumber;
    BOOL m_bIsColor;
    CString m_sCameraFile;
    BOOL m_bCameraFileSet;
	BOOL m_bCameraFileChanged;
    BOOL m_bDigSet;
    BOOL m_bCameraObjOk;
	SVDeviceParamCollection m_CameraDeviceParams;
	SVDeviceParamCollection m_CameraFileParams;

	bool m_bFileAcquisition;
	SVString m_imageFilename;
	SVString m_imageDirectory;
	bool m_imageSizeEditModeFileBased;
	SIZE m_fileImageSize;
	long m_fileMode;

    void CheckObject();
};

#endif

//******************************************************************************
//* LOG HISTORY:
//******************************************************************************
/*
$Log:   N:\PVCSarch65\ProjectFiles\archives\SVObserver_src\SVObserver\SVOCameraObj.h_v  $
 * 
 *    Rev 1.0   23 Apr 2013 12:49:02   bWalter
 * Project:  SVObserver
 * Change Request (SCR) nbr:  814
 * SCR Title:  Upgrade SVObserver to Compile Using Visual Studio 2010
 * Checked in by:  bWalter;  Ben Walter
 * Change Description:  
 *   Initial check in to SVObserver_src.  (Merged with svo_src label SVO 6.10 Beta 008.)
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.11   21 Jan 2013 11:39:06   sjones
 * Project:  SVObserver
 * Change Request (SCR) nbr:  809
 * SCR Title:  Non I/O SVIM
 * Checked in by:  sJones;  Steve Jones
 * Change Description:  
 *   Added IsCameraLineInputSupported method
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.10   16 Jan 2013 16:14:26   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  802
 * SCR Title:  Add new product type GD1A
 * Checked in by:  bWalter;  Ben Walter
 * Change Description:  
 *   Migrating branch code into primary code base.  
 * 
 * Updated camera look-up functionality and const correctness of methods.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.9   16 Dec 2009 12:15:44   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  677
 * SCR Title:  Fix problem in camera notify thread
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Fix issues with includes and comments.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.8   30 Jul 2009 12:14:54   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  665
 * SCR Title:  Fix unrecoverable failure when processing acquisitions at high speed
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Merged branched changes into current code branch with appropriate updates.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.7   03 Jun 2009 09:54:58   sjones
 * Project:  SVObserver
 * Change Request (SCR) nbr:  650
 * SCR Title:  Integrate Gigabit ethernet cameras
 * Checked in by:  sJones;  Steve Jones
 * Change Description:  
 *   Added IsSoftwareTriggerSupported method.
 * Revised Include file definitions due to changes in SVDeviceParams
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.6   16 Sep 2008 14:37:26   sjones
 * Project:  SVObserver
 * Change Request (SCR) nbr:  634
 * SCR Title:  Implement a File Acquistion Device
 * Checked in by:  sJones;  Steve Jones
 * Change Description:  
 *   Added methods and member variables in support of FileAcquistion.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.5   10 Sep 2008 16:41:52   sjones
 * Project:  SVObserver
 * Change Request (SCR) nbr:  612
 * SCR Title:  Implement a timer trigger object
 * Checked in by:  sJones;  Steve Jones
 * Change Description:  
 *   Revised GetCameraDisplayName, GetCameraFile, and GetCameraFileParams to be const
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.4   23 Jul 2004 08:39:10   Joe
 * Project:  SVObserver
 * Change Request (SCR) nbr:  435
 * SCR Title:  Add Bulb Mode Operation to the Digital SVIM for Point Gray Dragonfly Cameras
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Added a new method to get the camera parameters as non-const so that they can get changed.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.3   15 Oct 2003 13:17:54   rschock
 * Project:  SVObserver
 * Change Request (SCR) nbr:  386
 * SCR Title:  Change Light Reference handling to match earlier versions of SVObserver
 * Checked in by:  eBeyeler;  Eric Beyeler
 * Change Description:  
 *   Made changes to edit configuration to track camera file changes so that the default light reference will be read from the camera file only initially or when changed.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.2   08 Jul 2003 10:38:26   ebeyeler
 * Project:  SVObserver
 * Change Request (SCR) nbr:  320
 * SCR Title:  Integrate Matrox Meteor II / 1394 Board and 1394 camera into SVObserver
 * Checked in by:  eBeyeler;  Eric Beyeler
 * Change Description:  
 *   added support for SVDeviceParamCollection
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.1   22 Apr 2003 12:29:46   rschock
 * Project:  SVObserver
 * Change Request (SCR) nbr:  346
 * SCR Title:  Update SVObserver to Version 4.21 Release
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Redid the #include defines and standardized the Tracker log headers and removed warning from release mode builds.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.0   15 Nov 2002 13:35:48   ryoho
 * Project:  SVObserver
 * Change Request (SCR) nbr:  226
 * SCR Title:  Monochrome SVIM configuration compatibility between ViperQUAD and ViperDUAL
 * Checked in by:  rYoho;  Rob Yoho
 * Change Description:  
 *   This is the first revision of this file.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
*/
