//******************************************************************************
//* COPYRIGHT (c) 2003 by Körber Pharma Inspection GmbH. All Rights Reserved
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
	SVOInspectionObj() = default;
	virtual ~SVOInspectionObj() = default;

    const std::string& GetOriginalName() const { return m_originalName; }
    void SetOriginalName(LPCTSTR pName) { m_originalName = pName; }

    const std::string& GetInspectionName() const { return m_InspectionName; }
    void SetInspectionName(LPCTSTR Inspection) { m_InspectionName = Inspection; }

    const std::string& GetToolsetImage() const { return m_ToolsetImage; }
    void SetToolsetImage(LPCTSTR Image) { m_ToolsetImage = Image; }

	const std::string& GetNewDisableMethodString() const { return m_NewDisableMethod; }
	void SetNewDisableMethod(LPCTSTR Disable) { m_NewDisableMethod = Disable; }

	bool GetShowAuxExtent() const { return m_ShowAuxExtent; }
	void SetShowAuxExtent( bool ShowAuxExtent ) { m_ShowAuxExtent = ShowAuxExtent; }

	long GetEnableAuxiliaryExtent() const { return m_lEnableAuxiliaryExtent; }
	void SetEnableAuxiliaryExtent(long lEnable) { m_lEnableAuxiliaryExtent = lEnable; }

    void RenameInspection(LPCTSTR NewName)
	{
		m_NameChanged = true;
		m_InspectionName = NewName;
	}

    
	bool IsNewInspection() const { return m_NewInspection; }
	void SetNewInspection() { m_NewInspection = true; }

	const std::string& GetImportFilename() const { return m_ImportFilename; }
	void SetImportFilename(LPCTSTR ImportFilename) { m_ImportFilename = ImportFilename; }
	void ClearImportFilename() { m_ImportFilename.clear(); }

	bool IsColor() const { return m_Color; }
	void SetColor(bool Color) { m_Color = Color; }

	DWORD GetObjectIdIndex() const { return m_ObjectIdIndex; }
	void SetObjectIdIndex(DWORD objectIdIndex) { m_ObjectIdIndex = objectIdIndex; }

private:
	std::string m_ToolsetImage {};
	std::string m_originalName {};
	std::string m_InspectionName {};
	std::string m_NewDisableMethod {_T("Method 1")};
	std::string m_ImportFilename {};
	long m_lEnableAuxiliaryExtent {0L};
	DWORD m_ObjectIdIndex {0};
	bool m_ShowAuxExtent {true};
	bool m_NameChanged {false};
	bool m_NewInspection {false};
	bool m_Color {false};
};

typedef std::shared_ptr< SVOInspectionObj > SVOInspectionObjPtr;

