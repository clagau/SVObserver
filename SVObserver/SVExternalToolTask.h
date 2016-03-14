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

	
	HRESULT Initialize(SVDllLoadLibraryCallback fnNotify = SVDllLoadLibraryCallbackDefault() );
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

	BOOL renameToolSetSymbol(const SVObjectClass* pObject, LPCTSTR OriginalName);

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
	HRESULT CollectInputImageNames( );

public:
	
	
	friend class SVExternalToolDlg;
	friend class SVExternalToolImageSelectPage;
	friend class SVExternalToolInputSelectPage;
	friend class SVExternalToolResultPage;
	friend class SVExternalToolDetailsSheet;

};
#pragma warning (pop)

#endif	// _SVIMAGESHIFTCLASS_H

