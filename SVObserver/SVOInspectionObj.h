//******************************************************************************
//* COPYRIGHT (c) 2003 by SVResearch, Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVOInspectionObj
//* .File Name       : $Workfile:   SVOInspectionObj.h  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.0  $
//* .Check In Date   : $Date:   23 Apr 2013 13:04:40  $
//******************************************************************************

#pragma once

#pragma region Includes
#include "SVUtilityLibrary\SVString.h"
#include "SVUtilityLibrary\SVSharedPtr.h"
#pragma endregion Includes

class SVOInspectionObj  
{
public:
	SVOInspectionObj();
	virtual ~SVOInspectionObj();

    const SVString& GetInspectionLabelName() const;
    void SetInspectionLabelName(LPCTSTR Label);

    const SVString& GetInspectionName() const;
    void SetInspectionName(LPCTSTR Inspection);

    const SVString& GetToolsetImage() const;
    void SetToolsetImage(LPCTSTR Image);

	const SVString& GetNewDisableMethod() const;
	void SetNewDisableMethod(LPCTSTR Disable);

	bool GetShowAuxExtent() const;
	void SetShowAuxExtent( bool ShowAuxExtent );

	long GetEnableAuxiliaryExtent() const;
	void SetEnableAuxiliaryExtent(long lEnable);

	bool HasInspectionNameChange() const;
    const SVString& GetOrginalInspectionName() const;
    void RenameInspection(LPCTSTR NewName);
    
	bool IsNewInspection() const;
	void SetNewInspection();

	const SVString& GetImportFilename() const;
	void SetImportFilename(LPCTSTR ImportFilename);
	void ClearImportFilename();

	bool IsColor() const;
	void SetColor( bool Color );

private:
	SVString m_ToolsetImage;
    SVString m_LabelName;
    SVString m_InspectionName;
    SVString m_OrginalName;
	SVString m_NewDisableMethod;
	SVString m_ImportFilename;
	long m_lEnableAuxiliaryExtent;
	int m_iNumPanels;
	bool m_ShowAuxExtent;
    bool m_NameChanged;
    bool m_NewInspection;
	bool m_Color;
};

typedef SVSharedPtr< SVOInspectionObj > SVOInspectionObjPtr;

