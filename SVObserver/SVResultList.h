//******************************************************************************
//* COPYRIGHT (c) 2003 by SVResearch, Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVREsultList.h
//* .File Name       : $Workfile:   SVResultList.h  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.2  $
//* .Check In Date   : $Date:   07 Jan 2015 17:49:30  $
//******************************************************************************

#pragma once

#pragma region Includes
#include "SVObjectLibrary/SVObjectClass.h"
#include "SVObjectLibrary/SVOutputInfoListClass.h"
#include "SVInfoStructs.h"
#include "ResultViewReferences.h"
#include "SVObserver.h"
#pragma endregion Includes


#pragma region Declarations
class SVResultClass;
class SVResultViewClass;
class SVTaskObjectClass;
class SVToolSetClass;


enum SVInspectionStateEnum;
#pragma endregion Declarations

class SVResultListClass
{
public:
#pragma region Constructor
	SVResultListClass();
	virtual ~SVResultListClass();	// no heirarchy; but so we can have RTTI
#pragma endregion Constructor

#pragma region Public Methods
	void Destroy();
	void Refresh(SVTaskObjectClass* pRootObject);

	void SetToolSet(SVToolSetClass* pToolSet);

	SVProductInspectedState GetInspectionState();

	//************************************
	// Description:  Build SVIPResultData from the reference vector.  (Get the values of the variables.)
	// Parameter:  rResultData <out>:  object containing data from the class's variables.
	// Parameter:  getColor <in>:  true (default) if the color for the item in the result view should be fetched.
	// Returns: S_OK if no error occurs   
	//************************************
	HRESULT  GetResultData( SVIPResultData& p_rResultData) const;

	//************************************
	//! Description:  Rebuild the reference vector from the list of GUIDs.
	//! \param pInspection [in] if not null the References will be checked if the are  a Disabled PPQVariables
	//! \returns void
	//************************************
	void RebuildReferenceVector(SVInspectionProcess* pInspection = nullptr);
	
	void Clear();

	
	//************************************
	// Description:  Build SVResultDefinitionDeque from the reference vector.  (Defines the list of viewed variables.)
	// Parameter:  rDefinitions <out>:  object containing data from the class's variables.
	// Returns:  S_OK if no error occurs   
	//************************************
	HRESULT GetResultDefinitions( ResultViewReferences::SVResultDefinitionDeque& rDefinitions ) const; 

	
	//************************************
	// Description:  Build SVResultsWrapperClass from reference vector.  (Get values of variables for remote interface.)
	// Parameter:  [out] pSVRWC:  
	// Parameter:  [in] lptitle:  
	// Returns:  int:  number of entries which are added to pSVRWC
	//************************************
	int AddResults( SVResultsWrapperClass* pSVRWC, LPCTSTR lptitle );
	
	
	bool LoadViewedVariables(SVTreeType& rTree, SVTreeType::SVBranchHandle htiParent);
	
	void GetNameSet(SVStringSet& SelectedNamesRaw);
	bool Insert(const SVString& param1);
	SVClock::SVTimeStamp getUpdateTimeStamp();
	void Save(SVObjectWriter& rWriter);

#pragma endregion Public Methods

#pragma region Member Variables
protected:
	ResultViewReferences   m_ResultViewReferences;
	SVToolSetClass* m_pToolSet;
	SVVector< SVResultClass* > m_results;
#pragma endregion Member Variables
};

