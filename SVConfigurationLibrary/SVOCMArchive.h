// ******************************************************************************
// * COPYRIGHT (c) 2002 by SVResearch, Harrisburg
// * All Rights Reserved
// ******************************************************************************
// * .Module Name     : SVOCMArchive
// * .File Name       : $Workfile:   SVOCMArchive.h  $
// * ----------------------------------------------------------------------------
// * .Current Version : $Revision:   1.1  $
// * .Check In Date   : $Date:   03 Feb 2014 16:11:32  $
// ******************************************************************************

#ifndef SVOCMARCHIVE_H
#define SVOCMARCHIVE_H

#include <comdef.h>
#include "SVStatusLibrary/SVException.h"
#include "SVMessage/SVMessage.h"

class SVOCMArchive  
{
private:
	SVOCMArchive();
	virtual ~SVOCMArchive();

public:
	template< typename SVTreeType >
	static HRESULT ArchiveSVObserverApp(   unsigned long ulSVOVersion, 
		unsigned long& ulSECVersion, 
		CArchive& rArchive, 
		SVTreeType& p_rTree,
		CString& rIODocName,
		typename SVTreeType::SVBranchHandle p_rParent);

	template< typename SVTreeType >
	static HRESULT ArchiveSVSystemClass(   unsigned long ulSVOVersion, 
		unsigned long ulSECVersion, 
		bool bIsColorMode,
		CArchive& rArchive, 
		SVTreeType& p_rTree,
		typename typename SVTreeType::SVBranchHandle p_pParent,
		int& riIPDocSize );

	template< typename SVTreeType >
	static HRESULT ArchiveSVDigitizerArrayClass(   unsigned long ulSVOVersion, 
		unsigned long ulSECVersion, 
		bool bIsColorMode,
		CString& rcsBoardName,
		CArchive& rArchive, 
		SVTreeType& p_rTree,
		typename SVTreeType::SVBranchHandle p_pParent);

	template< typename SVTreeType >
	static HRESULT ArchiveSVDigitizerObjectClass(  unsigned long ulSVOVersion, 
		unsigned long ulSECVersion,
		bool bIsColorMode,
		CString& rcsBoardName, 
		long lIndex, 
		CArchive& rArchive, 
		SVTreeType& p_rTree,
		typename SVTreeType::SVBranchHandle p_pParent);

	template< typename SVTreeType >
	static HRESULT ArchiveSVDigitizerInfoStruct(   unsigned long ulSVOVersion, 
		unsigned long ulSECVersion,
		bool bIsColorMode,
		CString &rcsBoardName, 
		long lIndex, 
		typename SVTreeType::SVBranchHandle p_pPPQCameraBranch,
		CArchive& rArchive, 
		SVTreeType& p_rTree,
		typename SVTreeType::SVBranchHandle p_pParent,
		long& rlCount);

	template< typename SVTreeType >
	static HRESULT ArchiveSVAcquisitionSourceListClass(unsigned long ulSVOVersion, 
		unsigned long ulSECVersion,
		bool bIsColorMode,
		CString &rcsBoardName, 
		long lIndex, 
		long lSelected, 
		CArchive &rArchive, 
		SVTreeType& p_rTree,
		typename SVTreeType::SVBranchHandle p_pParent);

	template< typename SVTreeType >
	static HRESULT ArchiveSVAcquisitionSourceClass(unsigned long ulSVOVersion, 
		unsigned long ulSECVersion,
		bool bIsColorMode,
		CString &rcsBoardName, 
		long lIndex, 
		bool bIsSelected, 
		CArchive &rArchive, 
		SVTreeType& p_rTree,
		typename SVTreeType::SVBranchHandle p_pParent);

	template< typename SVTreeType >
	static HRESULT ArchiveSVImageInfoClass(unsigned long ulSVOVersion, 
		unsigned long ulSECVersion, 
		bool bIsColorMode,
		CString &rcsBoardName, 
		long lIndex, 
		CArchive &rArchive, 
		SVTreeType& p_rTree,
		typename SVTreeType::SVBranchHandle p_pParent);

	template< typename SVTreeType >
	static HRESULT ArchiveSVExtentClass(   unsigned long ulSVOVersion, 
		unsigned long ulSECVersion, 
		CArchive &rArchive, 
		SVTreeType& p_rTree,
		typename SVTreeType::SVBranchHandle p_pParent);

	template< typename SVTreeType >
	static HRESULT ArchiveSVLightReferenceArrayClass(  unsigned long ulSVOVersion, 
		unsigned long ulSECVersion,
		bool bIsColorMode,
		CArchive &rArchive, 
		SVTreeType& p_rTree,
		typename SVTreeType::SVBranchHandle p_pParent);

	template< typename SVTreeType >
	static HRESULT ArchiveSVLightReferenceClass(   unsigned long ulSVOVersion, 
		unsigned long ulSECVersion, 
		bool bIsColorMode,
		CArchive &rArchive, 
		SVTreeType& p_rTree,
		typename SVTreeType::SVBranchHandle p_pParent);

	template< typename SVTreeType >
	static HRESULT ArchiveSVPPQIndexHandleOwnerStruct( unsigned long ulSVOVersion, 
		unsigned long ulSECVersion, 
		typename SVTreeType::SVBranchHandle p_pPPQCameraBranch,
		CArchive &rArchive, 
		SVTreeType& p_rTree,
		typename SVTreeType::SVBranchHandle p_pParent);

	template< typename SVTreeType >
	static HRESULT ArchiveSVDDEEntryStruct(    unsigned long ulSVOVersion, 
		unsigned long ulSECVersion,
		CArchive &rArchive, 
		SVTreeType& p_rTree,
		typename SVTreeType::SVBranchHandle p_pParent,
		long& rlCount,
		LPCTSTR tszNameFormat);

	template< typename SVTreeType >
	static HRESULT ArchiveSVDDEInputEntryListClass(unsigned long ulSVOVersion, 
		unsigned long ulSECVersion,
		long lIndex, 
		CArchive &rArchive, 
		SVTreeType& p_rTree,
		typename SVTreeType::SVBranchHandle p_pParent,
		long& rlCount);

	template< typename SVTreeType >
	static HRESULT ArchiveSVPQDataLinkInfoStruct(  unsigned long ulSVOVersion, 
		unsigned long ulSECVersion,
		CArchive &rArchive, 
		SVTreeType& p_rTree,
		typename SVTreeType::SVBranchHandle p_pParent,
		typename SVTreeType::SVBranchHandle& p_rpBranch,
		long* plCount,
		LPCTSTR tszNameFormat);

	template< typename SVTreeType >
	static HRESULT ArchiveSVPQDataLinkInfoStruct(  unsigned long ulSVOVersion, 
		unsigned long ulSECVersion,
		CArchive &rArchive, 
		SVTreeType& p_rTree,
		typename SVTreeType::SVBranchHandle p_pParent);

	template< typename SVTreeType >
	static HRESULT ArchiveAddSVPQDataLinkInfoStruct( unsigned long ulSVOVersion, 
		unsigned long ulSECVersion,
		CArchive &rArchive, 
		SVTreeType& p_rTree,
		typename SVTreeType::SVBranchHandle p_pParent,
		typename SVTreeType::SVBranchHandle p_pBranch);

	template< typename SVTreeType >
	static HRESULT ArchiveSVByteArrayClass(unsigned long ulSVOVersion, 
		unsigned long ulSECVersion, 
		CArchive &rArchive, 
		SVTreeType& p_rTree,
		typename SVTreeType::SVBranchHandle p_pParent);

	template< typename SVTreeType >
	static HRESULT ArchiveSVPQDigitialInputInfoListClass(  unsigned long ulSVOVersion, 
		unsigned long ulSECVersion, 
		long lIndex,
		CArchive &rArchive, 
		SVTreeType& p_rTree,
		typename SVTreeType::SVBranchHandle p_pParent,
		long& rlCount);

	template< typename SVTreeType >
	static HRESULT ArchiveSVPQDigitialInputInfoStruct( unsigned long ulSVOVersion, 
		unsigned long ulSECVersion,
		long lIndex, 
		CArchive &rArchive, 
		SVTreeType& p_rTree,
		typename SVTreeType::SVBranchHandle p_pParent,
		long& rlCount);

	template< typename SVTreeType >
	static HRESULT ArchiveSVAcquisitionTriggerEventServerClass(unsigned long ulSVOVersion, 
		unsigned long ulSECVersion, 
		CArchive &rArchive, 
		SVTreeType& p_rTree,
		typename SVTreeType::SVBranchHandle p_pParent);

	template< typename SVTreeType >
	static HRESULT ArchiveSVEventServerClass(  unsigned long ulSVOVersion, 
		unsigned long ulSECVersion, 
		CArchive &rArchive, 
		SVTreeType& p_rTree,
		typename SVTreeType::SVBranchHandle p_pParent);

	template< typename SVTreeType >
	static HRESULT ArchiveSVDigitalOutputObjectClass(  unsigned long ulSVOVersion, 
		unsigned long ulSECVersion, 
		CArchive &rArchive, 
		SVTreeType& p_rTree,
		typename SVTreeType::SVBranchHandle p_pParent);

	template< typename SVTreeType >
	static HRESULT ArchiveSVPQObjectClass( unsigned long ulSVOVersion, 
		unsigned long ulSECVersion, 
		long lIndex,
		CArchive &rArchive, 
		SVTreeType& p_rTree,
		typename SVTreeType::SVBranchHandle p_pParent,
		long& rlCount);

	template< typename SVTreeType >
	static HRESULT ArchiveSVPQCheckerObjectClass(  unsigned long ulSVOVersion, 
		unsigned long ulSECVersion, 
		CArchive &rArchive, 
		SVTreeType& p_rTree,
		typename SVTreeType::SVBranchHandle p_pParent);

	template< typename SVTreeType >
	static HRESULT ArchiveSVPPQInfoStruct( unsigned long ulSVOVersion, 
		unsigned long ulSECVersion, 
		long lIndex,
		CArchive &rArchive, 
		SVTreeType& p_rTree,
		typename SVTreeType::SVBranchHandle p_pParent);

	template< typename SVTreeType >
	static HRESULT ArchiveSVDDEControlClass(   unsigned long ulSVOVersion, 
		unsigned long ulSECVersion, 
		long lIndex,
		CArchive &rArchive, 
		SVTreeType& p_rTree,
		typename SVTreeType::SVBranchHandle p_pParent);

	template< typename SVTreeType >
	static HRESULT ArchiveSVDDEOutputEntryListClass(   unsigned long ulSVOVersion, 
		unsigned long ulSECVersion, 
		long lIndex,
		CArchive &rArchive, 
		SVTreeType& p_rTree,
		typename SVTreeType::SVBranchHandle p_pParent);

	template< typename SVTreeType >
	static HRESULT ArchiveSVIPDocInfoClass(unsigned long ulSVOVersion, 
		unsigned long ulSECVersion, 
		CString &rcsBoardName,
		CString &rcsInspection, 
		CArchive &rArchive, 
		SVTreeType& p_rTree,
		typename SVTreeType::SVBranchHandle p_pParent);

	template< typename SVTreeType >
	static HRESULT ArchiveSVIPDocRequirementInfoClass( unsigned long ulSVOVersion, 
		unsigned long ulSECVersion, 
		typename SVTreeType::SVBranchHandle p_pInspection,
		CString &rcsBoardName,
		CString &rcsInspection, 
		CArchive &rArchive, 
		SVTreeType& p_rTree,
		typename SVTreeType::SVBranchHandle p_pParent);

	template< typename SVTreeType >
	static HRESULT ArchiveSVCameraInfoStruct(  unsigned long ulSVOVersion, 
		unsigned long ulSECVersion, 
		typename SVTreeType::SVBranchHandle p_pInspection,
		CString &rcsBoardName,
		CString &rcsInspection, 
		CArchive &rArchive, 
		SVTreeType& p_rTree,
		typename SVTreeType::SVBranchHandle p_pParent);

	template< typename SVTreeType >
	static HRESULT ArchiveIOD( unsigned long ulSVOVersion, 
		unsigned long &ulIODVersion, 
		CArchive &rArchive, 
		SVTreeType& p_rTree);

	template< typename SVTreeType >
	static HRESULT ArchiveSVIODoc( unsigned long ulSVOVersion, 
		unsigned long &ulIODVersion, 
		CArchive &rArchive, 
		SVTreeType& p_rTree,
		typename SVTreeType::SVBranchHandle p_pParent);

	template< typename SVTreeType >
	static HRESULT ArchiveSVTriggerEventServerClass(   unsigned long ulSVOVersion, 
		unsigned long ulIODVersion, 
		CArchive &rArchive, 
		SVTreeType& p_rTree,
		typename SVTreeType::SVBranchHandle p_pParent);

	template< typename SVTreeType >
	static HRESULT ArchiveSVTimerTriggerEventServerClass(  unsigned long ulSVOVersion, 
		unsigned long ulIODVersion, 
		CArchive &rArchive, 
		SVTreeType& p_rTree,
		typename SVTreeType::SVBranchHandle p_pParent);

	template< typename SVTreeType >
	static HRESULT ArchiveSVIOEntryClass(  unsigned long ulSVOVersion, 
		unsigned long ulIODVersion, 
		CArchive &rArchive, 
		SVTreeType& p_rTree,
		typename SVTreeType::SVBranchHandle p_pParent);

	template< typename SVTreeType >
	static HRESULT ArchiveSVDigitalInputObjectClass(   unsigned long ulSVOVersion, 
		unsigned long ulIODVersion, 
		CArchive &rArchive, 
		SVTreeType& p_rTree,
		typename SVTreeType::SVBranchHandle p_pParent);

	template< typename SVTreeType >
	static HRESULT ArchiveIPD( unsigned long ulSVOVersion, 
		unsigned long &ulIPDVersion, 
		CArchive &rArchive, 
		SVTreeType& p_rTree,
		typename SVTreeType::SVBranchHandle p_pParent);

	template< typename SVTreeType >
	static HRESULT ArchiveSVIPDoc( unsigned long ulSVOVersion, 
		unsigned long &ulIPDVersion, 
		CString &rcsInspection, 
		CArchive &rArchive, 
		SVTreeType& p_rTree,
		typename SVTreeType::SVBranchHandle p_pParent);

	template< typename SVTreeType >
	static HRESULT ArchiveSVIPDocViewData( unsigned long ulSVOVersion, 
		unsigned long &ulIPDVersion, 
		CArchive &rArchive, 
		SVTreeType& p_rTree,
		typename SVTreeType::SVBranchHandle p_pParent);

	template< typename SVTreeType >
	static HRESULT ArchiveSVImageViewScroll(   unsigned long ulSVOVersion, 
		unsigned long &ulIPDVersion, 
		CArchive &rArchive, 
		SVTreeType& p_rTree,
		typename SVTreeType::SVBranchHandle p_pParent);

	template< typename SVTreeType >
	static HRESULT ArchiveSVImageViewClass(unsigned long ulSVOVersion, 
		unsigned long &ulIPDVersion, 
		CArchive &rArchive, 
		SVTreeType& p_rTree,
		typename SVTreeType::SVBranchHandle p_pParent);

	template< typename SVTreeType >
	static HRESULT ArchiveSVImageViewClassEmbedded(unsigned long ulSVOVersion, 
		unsigned long &ulIPDVersion, 
		CArchive &rArchive, 
		SVTreeType& p_rTree,
		typename SVTreeType::SVBranchHandle p_pParent);

	template< typename SVTreeType >
	static HRESULT ArchiveSVToolSetTabViewClass(   unsigned long ulSVOVersion, 
		unsigned long &ulIPDVersion, 
		CArchive &rArchive, 
		SVTreeType& p_rTree,
		typename SVTreeType::SVBranchHandle p_pParent);

	template< typename SVTreeType >
	static HRESULT ArchiveSVResultViewClass(   unsigned long ulSVOVersion, 
		unsigned long &ulIPDVersion, 
		CArchive &rArchive, 
		SVTreeType& p_rTree,
		typename SVTreeType::SVBranchHandle p_pParent);

	template< typename SVTreeType >
	static HRESULT ArchiveWindowPlacementData( unsigned long ulSVOVersion, 
		unsigned long &ulIPDVersion, 
		CArchive &rArchive, 
		SVTreeType& p_rTree,
		typename SVTreeType::SVBranchHandle p_pParent);

	template< typename SVTreeType >
	static HRESULT ArchivePointData(   unsigned long ulSVOVersion, 
		unsigned long &ulIPDVersion, 
		CArchive &rArchive, 
		SVTreeType& p_rTree,
		typename SVTreeType::SVBranchHandle p_pParent);

	template< typename SVTreeType >
	static HRESULT ArchiveRectData(unsigned long ulSVOVersion, 
		unsigned long &ulIPDVersion, 
		CArchive &rArchive, 
		SVTreeType& p_rTree,
		typename SVTreeType::SVBranchHandle p_pParent);

	template< typename SVTreeType >
	static HRESULT ArchiveSizeData(unsigned long ulSVOVersion, 
		unsigned long &ulIPDVersion, 
		CArchive &rArchive, 
		SVTreeType& p_rTree,
		typename SVTreeType::SVBranchHandle p_pParent);

	template< typename SVTreeType >
	static HRESULT CreateTreeFromConfigurationFile (unsigned long ulSVOVersion, 
		unsigned long ulSECVersion, 
		BSTR pstrFilename,
		SVTreeType& p_rTree);

	// CreateConfigurationFileFromTree () ---------------------------------------
	//
	// ulSVOVersion - input -
	//	  It is intended that this parameter contain the current 
	//   SVObserver version number.
	//
	// bstrRevisionHistory - input - 
	//   It is intended that this parameter contain the XML string which 
	//   represents the revision history as it was last pulled from the 
	//   configuration.  This revision history excludes the SVObserver instance.
	//	  The current SVObserver instance should not be added to the list until
	//   the actual save.  This should be done automatically by SVXMLClass.
	//
	//	  This parameter can be NULL if there is no revision history.
	//
	//	ulSECVersion - input - 
	//   It is intended that this parameter contain the SVObserver version file 
	//   format that is desired in the saved file.  This functionallity is not 
	//   currently implemented, so this parameter is not used.  Potential future 
	//   implementation.
	//##ModelId=3F2FA26101AD
	template< typename SVTreeType >
	static HRESULT CreateConfigurationFileFromTree(
		unsigned long	ulSVOVersion, 
		BSTR				bstrRevisionHistory,
		unsigned long	ulSECVersion, 
		LPCTSTR			pstrFilename,
		SVTreeType& p_rTree);


	// GetSVOSavedVersionNumberFromConfigurationFile () -------------------------
	// This function currently only works on SVX files.
	static HRESULT GetSVOSavedVersionNumberFromConfigurationFile (
		BSTR				abstrFileName, 
		unsigned long*	apulSVOConfigVersion);



protected:


	//	FindVersion401_430 () ----------------------------------------------------
	// If a version number is not found, this function will set 
	// aplConfigurationVersion to zero.
	static HRESULT FindVersion401_430 (char* apczSourceString, 
		unsigned long* aplConfigurationVersion);
	//	FindVersion440_ () -------------------------------------------------------
	// If a version number is not found, this function will set 
	// aplConfigurationVersion to zero.
	static HRESULT FindVersion440_ (char* apczSourceString, 
		unsigned long* aplConfigurationVersion);

	// FindVersion440__Standard () ----------------------------------------------
	// Only called from FindVersion440_ ().  
	static HRESULT FindVersion440__Standard (char* apczSourceString, 
		unsigned long* aplConfigurationVersion);

	// FindVersion440__RevisionHistory () ---------------------------------------
	// Only called from FindVersion440_ ().  
	// If the version number can not be found within the configuration document
	// (FindVersion440__Standard), then we will search the XML document.  The 
	// only (legitimate) reason (at this time) that the version would be found
	// in the XML document but not the configuration document would be if the 
	// configuration document were encrypted.
	static HRESULT FindVersion440__RevisionHistory (char*				p_czpSourceString, 
		unsigned long*	p_ulpConfigurationVersion);


	template< typename SVTreeType >
	static HRESULT GetEnvironmentBranch( SVTreeType& p_rTree, 
		typename SVTreeType::SVBranchHandle& p_rpBranch );

	template< typename SVTreeType >
	static HRESULT GetAcquisitionDeviceBranch( LPCTSTR lpszBoardName,
		long lDigNumber,
		SVTreeType& p_rTree, 
		typename SVTreeType::SVBranchHandle& p_rpBranch );

	template< typename SVTreeType >
	static HRESULT GetCameraBranch( long lNumber,
		SVTreeType& p_rTree, 
		typename SVTreeType::SVBranchHandle& p_rpBranch );

	template< typename SVTreeType >
	static HRESULT GetTriggerBranch( long lNumber,
		SVTreeType& p_rTree, 
		typename SVTreeType::SVBranchHandle& p_rpBranch );

	template< typename SVTreeType >
	static HRESULT GetInspectionBranch( long lNumber,
		SVTreeType& p_rTree, 
		typename SVTreeType::SVBranchHandle& p_rpBranch );

	template< typename SVTreeType >
	static HRESULT GetPPQBranch( long lNumber,
		SVTreeType& p_rTree, 
		typename SVTreeType::SVBranchHandle& p_rpBranch );

	template< typename SVTreeType >
	static HRESULT GetIOBranch( SVTreeType& p_rTree, 
		typename SVTreeType::SVBranchHandle& p_rpBranch );

	template< typename SVTreeType >
	static HRESULT GetBranch( SVTreeType& p_rTree, 
		LPCTSTR lpszName, 
		typename SVTreeType::SVBranchHandle p_pParent, 
		typename SVTreeType::SVBranchHandle& p_rpItem );

	template< typename SVTreeType >
	static HRESULT GetItem( SVTreeType& p_rTree, 
		LPCTSTR lpszName, 
		typename SVTreeType::SVBranchHandle p_pParent, 
		typename SVTreeType::SVLeafHandle& p_rLeaf );

	template< typename SVTreeType >
	static HRESULT AddItem( LPCTSTR lpszName, 
		typename SVTreeType::SVBranchHandle& p_rParent,
		SVTreeType& rTree, 
		_variant_t &rVariant );

	template< typename SVTreeType >
	static HRESULT AddItem( LPCTSTR lpszName, 
		typename SVTreeType::SVBranchHandle& p_rParent,
		int iType,
		SVTreeType& rTree, 
		CArchive &rArchive,
		void *pvValue = NULL );

	template< typename SVTreeType >
	static HRESULT AddArray( LPCTSTR lpszName, 
		typename SVTreeType::SVBranchHandle& p_rParent,
		int iType,
		unsigned long ulSize,
		SVTreeType& rTree, 
		CArchive &rArchive );

	template< typename SVTreeType >
	static HRESULT AddBranch( SVTreeType& p_rTree, 
		LPCTSTR lpszName, 
		typename SVTreeType::SVBranchHandle p_pParent, 
		typename SVTreeType::SVBranchHandle& p_rpBranch );

	static HRESULT SerializeGUID( CArchive &rArchive, GUID &rGuid);

	// EB 2002 08 08 : add exception handling
#define CONFIG_MGR_USE_ERROR_HANDLING

#ifndef CONFIG_MGR_USE_ERROR_HANDLING
#define HandleErrorCondition(h) ()
#else
	// do some tricky macro expansion to automagically get the source file, line, and timestamps
	//#define HandleErrorCondition(h,s) (HECSourceInformation((h),CString((s)),__,__,__,__,__))
	//#define HECSourceInformation(h,s,f,l,ts,d,t) (implHandleErrorCondition(h,s,f##FILE__,l##LINE__,t##TIMESTAMP__,d##DATE__,ts##TIME__))

	// I guess it doesn't have to be quite so tricky... the __FILE__ etc. are substituted AFTER regular macro expansion
#define HandleErrorCondition(e, h) HandleErrorConditionWarning(e, h)    // default severity
#define HandleErrorConditionInfo(e, h) (implHandleErrorCondition(1, (e), (h),__FILE__,__LINE__,__TIMESTAMP__,__DATE__,__TIME__))
#define HandleErrorConditionWarning(e, h) (implHandleErrorCondition(2, (e), (h),__FILE__,__LINE__,__TIMESTAMP__,__DATE__,__TIME__))
#define HandleErrorConditionSevere(e, h) (implHandleErrorCondition(3, (e), (h),__FILE__,__LINE__,__TIMESTAMP__,__DATE__,__TIME__))

	inline static void implHandleErrorCondition (  BYTE bySeverity, long lErr, HRESULT hr, 
		TCHAR* tszSourceFile,
		int iSourceLine,
		TCHAR* tszSourceModificationTimestamp,
		TCHAR* tszSourceCompileDate,
		TCHAR* tszSourceCompileTime)
	{
		if (hr != S_OK)
		{

			if (bySeverity == 2)    // if (bySeverity >= minloglevel && bySeverity < 3)
			{
				// we don't want to throw, just create an exception object and use it's reporting facilities
				SVException e(lErr, tszSourceCompileDate, tszSourceCompileTime, _T(""), tszSourceFile, iSourceLine, tszSourceModificationTimestamp, SVMSG_SVO_15_LOAD_CONFIG_WARNING, hr);

				e.LogException();
			}
			else if (bySeverity == 3)
			{
				// go directly to jail, do not pass go, do not collect $200.
				// (stop processing and jump to the catch block)
				throw SVException(lErr, tszSourceCompileDate, tszSourceCompileTime, _T(""), tszSourceFile, iSourceLine, tszSourceModificationTimestamp, SVMSG_SVO_16_LOAD_CONFIG_ERROR, hr);
			}

		}
	}
#endif
};

#include "SVOCMArchive.inl"

#endif

// ******************************************************************************
// * LOG HISTORY:
// ******************************************************************************
/*
$Log:   N:\PVCSarch65\ProjectFiles\archives\SVObserver_SRC\SVConfigurationLibrary\SVOCMArchive.h_v  $
 * 
 *    Rev 1.1   03 Feb 2014 16:11:32   bwalter
 * Project:  SVObserver
 * Change Request (SCR) nbr:  880
 * SCR Title:  Remove .SEC
 * Checked in by:  mZiegler;  Marc Ziegler
 * Change Description:  
 *   Removed method ArchiveSEC.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.0   18 Apr 2013 18:39:40   bWalter
 * Project:  SVObserver
 * Change Request (SCR) nbr:  814
 * SCR Title:  Upgrade SVObserver to Compile Using Visual Studio 2010
 * Checked in by:  bWalter;  Ben Walter
 * Change Description:  
 *   Initial check in to SVObserver_src.  (Merged with svo_src label SVO 6.10 Beta 008.)
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.0   18 Sep 2012 17:54:24   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  602
 * SCR Title:  Revise the Toolset Parsing and Object Creation Methodology
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated code to use the templated XML objects that moved from the SVLibrary to the new XML Library.
 * 
 * This code was moved from the SVOConfigMgr Library.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.7   05 Nov 2010 10:16:16   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  703
 * SCR Title:  Allow SVObserver to Change Remote Input Values
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated source code to remove redundent data objects.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.6   15 Dec 2009 12:58:14   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  677
 * SCR Title:  Fix problem in camera notify thread
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated include information and comments.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.5   03 Sep 2009 09:16:20   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  665
 * SCR Title:  Fix unrecoverable failure when processing acquisitions at high speed
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated files to fix moved file includes.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.4   16 Sep 2005 16:16:34   jBrown
 * Project:  SVObserver
 * Change Request (SCR) nbr:  505
 * SCR Title:  Encrypt security file
 * Checked in by:  JimAdmin;  James A. Brown
 * Change Description:  
 *   Encryption was generically implemented through the SVXMLClass.  Because the configurations use this class, their interface to the SVXMLClass needed modified.  
 * 
 * Also, the configuration handling was modified to handle actual excrypted configurations because this was believed the best way to test the XML enryption.  Getting the version number needed enhanced since the part getting the version number has no encryption knowledge.
 * 
 * The following functions were modified:
 *  SVOMArchive::CreateConfigurationFileFromTree ()
 *  SVOMArchive::CreateTreeFromConfigurationFile ()
 *  SVOMArchive::GetSVOSavedVersionNumberFromConfigurationFile (), and
 *  SVOMArchive::FindVersion440_ ()
 * 
 * The following functions were added:
 *  SVOMArchive::FindVersion440_Standard (), and
 *  SVOMArchive::FindVersion440_RevisionHistory () 
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.3   22 Oct 2004 07:12:02   jBrown
 * Project:  SVObserver
 * Change Request (SCR) nbr:  375
 * SCR Title:  Remodel XML usage for configuration storage and SIAC transfers.
 * Checked in by:  JimAdmin;  James A. Brown
 * Change Description:  
 *   Added class declairation for external SVDOMClass.
 * 
 * Renamed function from 
 * BuildTreeFromXMLDoc () 
 * to
 * CreateTreeFromConfigurationFile ()
 * along with modifying associated parameters.
 * 
 * Renamed function from 
 * CreateXMLDocFromTree ()
 * to
 * CreateConfigurationFileFromTree()
 * along with modifying associated parameters.
 * 
 * Added member functions:
 *   GetSVOSavedVersionNumberFromConfigurationFile ()
 *   FindVersion401_430 (), and
 *   FindVersion440_ ().
 * 
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.2   22 Oct 2003 07:53:54   ebeyeler
 * Project:  SVObserver
 * Change Request (SCR) nbr:  320
 * SCR Title:  Integrate Matrox Meteor II / 1394 Board and 1394 camera into SVObserver
 * Checked in by:  eBeyeler;  Eric Beyeler
 * Change Description:  
 *   changed SVOMessage usage to SVMessage usage
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.1   09 Jan 2003 16:05:56   ebeyeler
 * Project:  SVObserver
 * Change Request (SCR) nbr:  317
 * SCR Title:  Load SEC improperly imports Light Reference data for RGB Color
 * Checked in by:  eBeyeler;  Eric Beyeler
 * Change Description:  
 *   Modified the ArchiveSVLightReferenceArrayClass and ArchiveSVLightReferenceClass functions to take as a parameter whether the system is a RGB Color or not (bIsColorMode).
 * In ArchiveSVLightReferenceClass, if setting a default value for RGB contrast, check this flag before assigning the value. If color mode, default value=10000 otherwise default is 8100.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.0   18 Nov 2002 09:57:12   ryoho
 * Project:  SVObserver
 * Change Request (SCR) nbr:  226
 * SCR Title:  Monochrome SVIM configuration compatibility between ViperQUAD and ViperDUAL
 * Checked in by:  rYoho;  Rob Yoho
 * Change Description:  
 *   first version of file
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
*/
