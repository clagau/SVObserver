//******************************************************************************
//* COPYRIGHT (c) 2014 by Körber Pharma Inspection GmbH. All Rights Reserved
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : Tool export import
//* .File Name       : $Workfile:   ToolExportImport.h  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.1  $
//* .Check In Date   : $Date:   13 Jan 2015 10:38:48  $
//* ----------------------------------------------------------------------------
//* This class is used to write and read the selected tools to and from files
//*  or clipboard
//******************************************************************************

#pragma once

#pragma region Includes
#include "SVStatusLibrary/MessageManager.h"
#include "SVXMLLibrary/SVXMLMaterialsTree.h"
#include "SVProtoBuf/InspectionCommands.h"
#include "SVUtilityLibrary/SVGUID.h"
#pragma endregion Includes

//Forward declarations
namespace  SvOi
{
class IObjectWriter;
}

#pragma warning (push)
#pragma warning (disable : 4471)
namespace SvPb
{
	enum ClassIdEnum;
}
#pragma warning (pop)

namespace SvTo
{
class SVToolClass;
}

class SVObjectClass;
class SVIPDoc;

class ToolExportImport
{
public:
#pragma region Declarations
#pragma endregion Declarations

public:
#pragma region Public Methods

	ToolExportImport();

	//************************************
	// Writes zipped XML tool data to the clipboard
	// Parameter: rToolIds <in> Reference to a vector with the selected tool IDs to write to the clipboard
	// Return: S_OK on success
	//************************************
	HRESULT writeToolDataToClipboard(const std::vector<uint32_t>& rToolIds) const;

	static std::string createFileFromClipboard();

	HRESULT exportTools(const std::vector<uint32_t>& rToolIds, LPCTSTR exportFileName, LPCTSTR xmlFileName, bool includeModuleFiles) const;
	std::pair<std::string, google::protobuf::RepeatedField<SvPb::ImportModuleResponse>>  prepareImportTools(LPCTSTR fileName, const std::vector<SVGUID>& overwriteModules, const std::vector<SVGUID>& keepModules) const;

	std::vector<uint32_t> createToolsFromXmlData(const std::string& rXmlData, uint32_t ownerId, bool onlySameVersionValid = true, bool useExplorerStyle = true);

	const SvStl::MessageManager& getLastErrorMessage() const { return m_errorMessage; }

	static void SetReloadAfterCopyToolsToClipboard(bool flag) { m_isReloadedAfterCopyToolsToClipboard = flag; };
#pragma endregion Public Methods

protected:
#pragma region Protected Methods

	void createToolsExportFile(const std::vector<uint32_t>& rToolIds, LPCTSTR exportFileName, LPCTSTR xmlFileName, bool includeModuleFiles) const;

	//************************************
	// Description: This method checks the clipboard SVObserver version to the current SVObserver version
	// Parameter: rTree <in> Reference to the tree
	// Return: S_OK on success
	//************************************
	void checkVersion(SvXml::SVXMLMaterialsTree& rTree, bool onlySameVersionValid) const;

	/// This method replaces the tool name and the dotted name (e.g. in equations)
	/// \param rXmlData [in,out] Reference to the XML data to search and replace
	/// \param rTree [in] Reference to the tree generated from the clipboard
	/// \param pOwner [in] The owner of the new object.
	/// \returns HRESULT S_OK on success
	HRESULT updateAllToolNames(std::string& rXmlData, SVTreeType& rTree, const SVObjectClass& rOwner, const SVIPDoc& rDoc, bool useExplorerStyle) const;

	void updateToolName(std::string& rXmlData, const SVObjectClass& rOwner, const SVIPDoc& rDoc, const std::string& toolName, const std::string& rOldFullToolName, bool useExplorerStyle) const;
	std::string getUniqueToolName(const std::string& rToolName, const SVObjectClass& rOwner, const SVIPDoc& rDoc, bool useExplorerStyle) const;

	//************************************
	// Description: This method replaces all the unique ids
	// Parameter: rXmlData <in, out> Reference to the XML data to search and replace
	// Parameter: rTree <in> Reference to the tree generated from the clipboard
	// Return: S_OK on success
	//************************************
	HRESULT replaceUniqueIds( std::string& rXmlData, SvXml::SVXMLMaterialsTree& rTree ) const;
#pragma endregion Protected Methods

private:
#pragma region Member Variables
	mutable SvStl::MessageManager m_errorMessage{SvStl::MsgType::Log | SvStl::MsgType::Display};
	mutable std::vector<std::string> m_additionalNames;

	static bool m_isReloadedAfterCopyToolsToClipboard;
#pragma endregion Member Variables
};

bool toolClipboardDataPresent(); ///<Checks to see if the clipboard data is valid
void replaceToolName(std::string& rXmlData, const std::string& rOldToolName, const std::string& rNewToolName, const std::string& rOldFullToolName, const std::string& rNewFullToolName);
