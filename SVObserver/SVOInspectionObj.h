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
//Moved to precompiled header: #include <memory>
#pragma endregion Includes

class SVOInspectionObj  
{
public:
	SVOInspectionObj();
	virtual ~SVOInspectionObj();

    const std::string& GetInspectionLabelName() const;
    void SetInspectionLabelName(LPCTSTR Label);

    const std::string& GetInspectionName() const;
    void SetInspectionName(LPCTSTR Inspection);

    const std::string& GetToolsetImage() const;
    void SetToolsetImage(LPCTSTR Image);

	const std::string& GetNewDisableMethodString() const;
	void SetNewDisableMethod(LPCTSTR Disable);

	bool GetShowAuxExtent() const;
	void SetShowAuxExtent( bool ShowAuxExtent );

	long GetEnableAuxiliaryExtent() const;
	void SetEnableAuxiliaryExtent(long lEnable);

    void RenameInspection(LPCTSTR NewName);
    
	bool IsNewInspection() const;
	void SetNewInspection();

	const std::string& GetImportFilename() const;
	void SetImportFilename(LPCTSTR ImportFilename);
	void ClearImportFilename();

	bool IsColor() const;
	void SetColor( bool Color );

private:
	std::string m_ToolsetImage;
    std::string m_LabelName;
    std::string m_InspectionName;
	std::string m_NewDisableMethod;
	std::string m_ImportFilename;
	long m_lEnableAuxiliaryExtent;
	int m_iNumPanels;
	bool m_ShowAuxExtent;
    bool m_NameChanged;
    bool m_NewInspection;
	bool m_Color;
};

typedef std::shared_ptr< SVOInspectionObj > SVOInspectionObjPtr;

