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
#include "SVUtilityLibrary\SVSharedPtr.h"
#pragma endregion Includes

class SVOInspectionObj  
{
public:
	SVOInspectionObj();
	virtual ~SVOInspectionObj();

    CString GetInspectionLabelName() const;
    void SetInspectionLabelName(CString sLabel);

    CString GetInspectionName() const;
    void SetInspectionName(CString sInspection);

    CString GetToolsetImage() const;
    void SetToolsetImage(CString sImage);

	CString GetNewDisableMethod() const;
	void SetNewDisableMethod(CString sDisable);

	bool GetShowAuxExtent() const;
	void SetShowAuxExtent( bool ShowAuxExtent );

	long GetEnableAuxiliaryExtent() const;
	void SetEnableAuxiliaryExtent(long lEnable);

	bool HasInspectionNameChange() const;
    CString GetOrginalInspectionName() const;
    void RenameInspection(CString sNewName);
    
	bool IsNewInspection() const;
	void SetNewInspection();

	const CString& GetImportFilename() const;
	void SetImportFilename(const CString& importFilename);
	void ClearImportFilename();

	bool IsColor() const;
	void SetColor( bool Color );

private:
	CString m_sToolsetImage;
    CString m_sLabelName;
    CString m_sInspectionName;
    CString m_sOrginalName;
	CString m_sNewDisableMethod;
	CString m_ImportFilename;
	long m_lEnableAuxiliaryExtent;
	int m_iNumPanels;
	bool m_ShowAuxExtent;
    bool m_NameChanged;
    bool m_NewInspection;
	bool m_Color;
};

typedef SVSharedPtr< SVOInspectionObj > SVOInspectionObjPtr;

