//******************************************************************************
//* COPYRIGHT (c) 2004 by SVResearch, Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVExternalToolTask
//* .File Name       : $Workfile:   SVExternalToolTask.h  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.4  $
//* .Check In Date   : $Date:   30 Sep 2014 15:42:52  $
//******************************************************************************

#ifndef SVEXTERNALTOOLTASK_H
#define SVEXTERNALTOOLTASK_H

#pragma warning (push)
#pragma warning (disable : 4290)

#include "SVContainerLibrary/SVVector.h"
#include "SVHBitmapUtilitiesLibrary/SVHBitmapUtilities.h"
#include "SVStatusLibrary/SVErrorException.h"
#include "SVTaskObject.h"
#include "SVImageClass.h"
#include "SVValueObject.h"
#include "SVVariantResultClass.h"
#include "ISVCancel.h"
#include "SVDllToolLoadLibraryClass.h"
#include "PropertyTree/SVRPropTreeState.h"
#include "SVMatroxLibrary/SVMatroxTypedefs.h"

class SVToolClass;
class SVIPDoc;

struct SVExternalToolTaskData : public SVCancelData
{
	enum
	{
		NUM_TOOL_DEPENDENCIES = 50,
		NUM_INPUT_OBJECTS = 50,
		NUM_RESULT_OBJECTS = 50,
		NUM_INPUT_IMAGES = 4,
		NUM_RESULT_IMAGES = 4,
	};

	SVExternalToolTaskData();
	SVExternalToolTaskData(const SVExternalToolTaskData& src);
	
	const SVExternalToolTaskData& operator = (const SVExternalToolTaskData& rhs);


	SVFileNameValueObjectClass               m_voDllPath;
	std::vector<SVFileNameValueObjectClass>  m_aDllDependencies; //[NUM_TOOL_DEPENDENCIES];


	// DLL info
	SVStringValueObjectClass               m_voToolName;
	SVLongValueObjectClass                 m_voToolVersion;

	// Inputs
	std::vector<SVInObjectInfoStruct>      m_aInputImageInfo; //[NUM_INPUT_IMAGES]; // used to connect to other images
	std::vector<SVInObjectInfoStruct>      m_aInputObjectInfo; //[NUM_INPUT_OBJECTS];	// used to connect to other value objects
	std::vector<SVVariantValueObjectClass> m_aInputObjects; //[NUM_INPUT_OBJECTS]; // our own value objects
	std::vector<SVStringValueObjectClass>  m_aInputObjectNames; //[NUM_INPUT_OBJECTS]; // our value object names


	std::vector<SVVariantValueObjectClass> m_aResultObjects; //[NUM_RESULT_OBJECTS];
	std::vector<SVStringValueObjectClass>  m_aResultObjectNames; //[NUM_RESULT_OBJECTS];

	//std::vector<SVVariantResultClass>      m_aResultRangeObjects; //ResultClass ( Ranges )

	SVImageDefinitionStructArray             m_aResultImageDefinitions;
	ResultValueDefinitionStructArray       m_aResultValueDefinitions;
	InputValueDefinitionStructArray        m_aInputValueDefinitions;

	SVMultiCancelData                      m_RangeResultData;

	long m_lNumInputImages;
	long m_lNumInputValues;
	long m_lNumResultImages;
	long m_lNumResultValues;

	SVRPropTreeState                       m_PropTreeState;
};	// end struct SVExternalToolTaskData

class SVExternalToolTask : public SVTaskObjectListClass, public ISVCancel
{
	SV_DECLARE_CLASS( SVExternalToolTask )

public:
	typedef SVVector< CString > SVDependenyNames;

	SVExternalToolTask( SVObjectClass* POwner = NULL, int StringResourceID = IDS_CLASSNAME_SV_EXTERNAL_TOOL_TASK );
	virtual ~SVExternalToolTask();

	virtual BOOL CreateObject( SVObjectLevelCreateStruct* PCreateStructure );
	virtual BOOL CloseObject();
	virtual BOOL OnValidate();
	virtual BOOL ConnectAllInputs();

	
	HRESULT Initialize(SVDllLoadLibraryCallback fnNotify = SVDllLoadLibraryCallbackDefault() ) throw (SVErrorException);
	virtual HRESULT DisconnectInputsOutputs(SVObjectVector& rListOfObjects);
	virtual HRESULT HideInputsOutputs(SVObjectVector& rListOfObjects);

	HRESULT SetPathName(const CString& strPath);
	HRESULT SetDependencies( const SVDependenyNames astrPath );
	HRESULT GetResultImageDefinitions( SVImageDefinitionStructArray& raResultImageDefinitions );
	HRESULT GetResultValueDefinitions ( ResultValueDefinitionStructArray& raResultValueDefinitions );
	HRESULT GetInputValueDefinitions( InputValueDefinitionStructArray& raInputValueDefinitions );

	// ISVCancel interface
	virtual bool CanCancel();
	virtual HRESULT GetCancelData(SVCancelData*& rpData);
	virtual HRESULT SetCancelData(SVCancelData* pData);

	HRESULT GetImageInfo(const SVImageDefinitionStruct* pDefinitionStruct, SVImageInfoClass& rInfo);
	HRESULT GetImageDefinitionStruct( SVImageDefinitionStruct& rImageDef, const SVImageInfoClass& rInfo);

	void SetAllAttributes();
	virtual HRESULT ResetObject();
	enum FindEnum
	{
		FIND_IMAGES = 1,
		FIND_VALUES = 2,
		FIND_ALL_OBJECTS = FIND_IMAGES | FIND_VALUES
	};
	HRESULT FindInvalidatedObjects(SVObjectVector& rList, const SVCancelData* pOriginalData, FindEnum eWhich );

protected:
	HRESULT ConnectInputs();
	HRESULT ConnectInputImages();
	HRESULT Uninitialize();
	HRESULT ClearData();
	HRESULT SetDefaultValues();
	virtual BOOL onRun( SVRunStatusClass& RRunStatus );
	virtual DWORD_PTR processMessage( DWORD DwMessageID, DWORD_PTR DwMessageValue, DWORD_PTR DwMessageContext );
	HRESULT InspectionInputsToVariantArray();

	SVImageClass* GetInputImage(int iIndex);
	SVImageClass* GetResultImage(int iIndex);

	//************************************
	// Method:    GetInputValue
	// Description:  Get a variant value of a requested input.
	// Parameter: int iIndex Index of the requested input.
	// Returns:   _variant_t
	//************************************
	_variant_t GetInputValue(int iIndex);

	SVVariantValueObjectClass* GetResultValueObject(int iIndex);
	SVResultClass* GetResultRangeObject(int iIndex);
	std::vector<SVResultClass*> GetResultRangeObjects();

	BOOL RenameToolSetSymbol(SVObjectClass* pObject, LPCTSTR tstrOriginalName);

	HRESULT AllocateResult (int iIndex);

	SVDLLToolLoadLibraryClass m_dll;

	SVExternalToolTaskData m_Data;	// this is our cancelable data

	HRESULT                  m_hrInitialized;	// OnValidate checks this; set when CreateObject/Initialize is successful.
	
	std::vector<long>      m_aInspectionInputImages;
	std::vector<long>      m_aInspectionResultImages;
	std::vector<SVDIBITMAPINFO> m_aInspectionInputHBMImages;
	std::vector<HBITMAP> m_aInspectionResultHBMImages;

	std::vector<_variant_t> m_aInspectionInputValues;
	std::vector<_variant_t> m_aInspectionResultValues;

	std::vector<CRect> m_aPreviousInputImageRect;

	// Result Images
	SVImageClass             m_aResultImages[SVExternalToolTaskData::NUM_RESULT_IMAGES];

	// Temperary Images to run slower and safer..........
	SVImageClass             m_aResultImagesCopy[SVExternalToolTaskData::NUM_RESULT_IMAGES];
	SVImageClass             m_aInputImagesCopy[SVExternalToolTaskData::NUM_INPUT_IMAGES];
	bool                     m_bUseImageCopies;
	
private:
	HRESULT CollectInputImageNames( SVRunStatusClass& RRunStatus );

public:
	
	
	friend class SVExternalToolDlg;
	friend class SVExternalToolImageSelectPage;
	friend class SVExternalToolInputSelectPage;
	friend class SVExternalToolResultPage;
	friend class SVExternalToolDetailsSheet;

};
#pragma warning (pop)

#endif	// _SVIMAGESHIFTCLASS_H

//******************************************************************************
//* LOG HISTORY:
//******************************************************************************
/*
$Log:   N:\PVCSarch65\ProjectFiles\archives\SVObserver_SRC\SVObserver\SVExternalToolTask.h_v  $
 * 
 *    Rev 1.4   30 Sep 2014 15:42:52   bwalter
 * Project:  SVObserver
 * Change Request (SCR) nbr:  925
 * SCR Title:  Add PPQ Items and SVObserver Modes to Equation Editor Object Selector
 * Checked in by:  mZiegler;  Marc Ziegler
 * Change Description:  
 *   Changed the method GetInputValueObject to GetInputValue; also, 
 * 
 * changed to returned _variant_t instead of SVValueObjectClass*.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.3   15 May 2014 11:21:48   sjones
 * Project:  SVObserver
 * Change Request (SCR) nbr:  852
 * SCR Title:  Add Multiple Platform Support to SVObserver's Visual Studio Solution
 * Checked in by:  tBair;  Tom Bair
 * Change Description:  
 *   Revised processMessage to use DWORD_PTR
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.2   01 Feb 2014 10:42:28   tbair
 * Project:  SVObserver
 * Change Request (SCR) nbr:  852
 * SCR Title:  Add Multiple Platform Support to SVObserver's Visual Studio Solution
 * Checked in by:  tBair;  Tom Bair
 * Change Description:  
 *   Changed sendmessage to use LONG_PTR instead of DWORD.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.1   01 Oct 2013 14:12:22   tbair
 * Project:  SVObserver
 * Change Request (SCR) nbr:  852
 * SCR Title:  Add Multiple Platform Support to SVObserver's Visual Studio Solution
 * Checked in by:  tBair;  Tom Bair
 * Change Description:  
 *   Add x64 platform.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.0   23 Apr 2013 10:33:12   bWalter
 * Project:  SVObserver
 * Change Request (SCR) nbr:  814
 * SCR Title:  Upgrade SVObserver to Compile Using Visual Studio 2010
 * Checked in by:  bWalter;  Ben Walter
 * Change Description:  
 *   Initial check in to SVObserver_src.  (Merged with svo_src label SVO 6.10 Beta 008.)
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.13   09 Nov 2010 16:09:28   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  704
 * SCR Title:  Upgrade SVObserver Version for 5.33 Release
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated source code to remove duplicate container objects.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.12   05 Nov 2010 11:02:22   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  703
 * SCR Title:  Allow SVObserver to Change Remote Input Values
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated source code to remove redundent data objects.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.11   16 Dec 2009 10:14:32   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  677
 * SCR Title:  Fix problem in camera notify thread
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Fix issues with includes and comments.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.10   03 Sep 2009 10:07:18   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  665
 * SCR Title:  Fix unrecoverable failure when processing acquisitions at high speed
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated files to fix moved file includes.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.9   07 Mar 2008 11:55:00   tbair
 * Project:  SVObserver
 * Change Request (SCR) nbr:  616
 * SCR Title:  Add Server-side Configuration Data Def Lists to the SVObserver program
 * Checked in by:  tBair;  Tom Bair
 * Change Description:  
 *   Added support for Transfer Image Definition List Source Image Names.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.8   24 Jul 2007 13:06:26   tbair
 * Project:  SVObserver
 * Change Request (SCR) nbr:  597
 * SCR Title:  Upgrade Matrox Imaging Library to version 8.0
 * Checked in by:  tBair;  Tom Bair
 * Change Description:  
 *   Upgrade Matrox Imaging Library to 8.0
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.7   21 Jun 2007 12:46:36   Joe
 * Project:  SVObserver
 * Change Request (SCR) nbr:  598
 * SCR Title:  Upgrade SVObserver to compile using vc++ in VS2005
 * Checked in by:  jSpila;  Joseph Spila
 * Change Description:  
 *   These changes include modification based on fixing compiler-based and project-based differences between VC6 and VC8.  These changes mainly include casting issues, but some include type conversion and assignment of new compiler controlling defines.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.6   04 Aug 2006 10:16:04   ebeyeler
 * Project:  SVObserver
 * Change Request (SCR) nbr:  556
 * SCR Title:  Fix external tool problems
 * Checked in by:  JimAdmin;  James A. Brown
 * Change Description:  
 *   removed ResetObject option on Initialize
 * changed code to properly handle 0 input/result images and values
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.5   09 Dec 2005 09:58:04   ebeyeler
 * Project:  SVObserver
 * Change Request (SCR) nbr:  528
 * SCR Title:  Fix External Tool problem with resizing input images
 * Checked in by:  eBeyeler;  Eric Beyeler
 * Change Description:  
 *   fixed logic for detecting input image size change
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.4   02 Mar 2005 15:19:30   ebeyeler
 * Project:  SVObserver
 * Change Request (SCR) nbr:  476
 * SCR Title:  Optimize performance for External Tool DLL HBITMAP operations
 * Checked in by:  eBeyeler;  Eric Beyeler
 * Change Description:  
 *   optimized HBITMAP <-> MIL conversion
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.3   01 Mar 2005 15:21:04   ebeyeler
 * Project:  SVObserver
 * Change Request (SCR) nbr:  476
 * SCR Title:  Optimize performance for External Tool DLL HBITMAP operations
 * Checked in by:  eBeyeler;  Eric Beyeler
 * Change Description:  
 *   fixed initialization bug
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.2   17 Feb 2005 14:07:24   Joe
 * Project:  SVObserver
 * Change Request (SCR) nbr:  456
 * SCR Title:  Update Image and Tool Objects to use the new Extent Classes
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Update class with new methodologies based on new base object, extents and reset structure.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.1   12 Oct 2004 12:52:46   ebeyeler
 * Project:  SVObserver
 * Change Request (SCR) nbr:  448
 * SCR Title:  Fix External Tool Change DLL functionality
 * Checked in by:  eBeyeler;  Eric Beyeler
 * Change Description:  
 *   modified to allow correct functioning when choosing a different DLL
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.0   08 Jul 2004 12:22:16   ebeyeler
 * Project:  SVObserver
 * Change Request (SCR) nbr:  406
 * SCR Title:  Implement External Tool
 * Checked in by:  eBeyeler;  Eric Beyeler
 * Change Description:  
 *   first iteration
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
*/
