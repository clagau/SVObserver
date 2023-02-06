//******************************************************************************
//* COPYRIGHT (c) 2014 by Körber Pharma Inspection GmbH. All Rights Reserved
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : Tool clipboard
//* .File Name       : $Workfile:   ToolClipboard.h  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.1  $
//* .Check In Date   : $Date:   13 Jan 2015 10:38:48  $
//* ----------------------------------------------------------------------------
//* This class is used to write and read the selected tool to and from the clipboard
//******************************************************************************

#pragma once

#pragma region Includes
#include "Definitions/StringTypeDef.h"
#include "SVStatusLibrary/MessageManager.h"
#include "SVXMLLibrary/SVXMLMaterialsTree.h"
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

class ToolClipboard
{
public:
#pragma region Declarations
#pragma endregion Declarations

public:
#pragma region Public Methods

	ToolClipboard();

	//************************************
	// Writes zipped XML tool data to the clipboard
	// Parameter: rToolIds <in> Reference to a vector with the selected tool IDs to write to the clipboard
	// Return: S_OK on success
	//************************************
	HRESULT writeXmlToolData(const std::vector<uint32_t>& rToolIds) const;

	//************************************
	// reads all XML tool data from the clipboard
	// returns a string containing that information
	//************************************
	std::string readXmlToolData();

	std::vector<uint32_t> createToolsFromXmlData(const std::string& rXmlData, uint32_t ownerId, bool onlySameVersionValid = true, bool useExplorerStyle = true);

	const SvStl::MessageManager& getLastErrorMessage() const { return m_errorMessage; }
#pragma endregion Public Methods

protected:
#pragma region Protected Methods

	std::string createToolDefinitionString(const std::vector<uint32_t>& rToolIds) const;

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

	std::string xmlFilePath() const;


#pragma endregion Protected Methods

private:
#pragma region Member Variables
	mutable SvStl::MessageManager m_errorMessage{SvStl::MsgType::Log | SvStl::MsgType::Display};
	mutable std::vector<std::string> m_additionalNames;

	std::string m_baseFilePath;
	std::string m_zipFilePath;
#pragma endregion Member Variables
};

std::string clipboardDataToString(); ///< gets the clipboard data and converts it into a string
bool toolClipboardDataPresent(); ///<Checks to see if the clipboard data is valid

/// Stream tools and additional information to a XML-File and return all dependency files in a vector
/// \param rToolIds [in] List of toolIds which have to stream to the XML-File
/// \param rXmlFilePath [in] Path and name of the new XML-File.
/// \returns SvDef::StringVector List of all dependency files are needed by the tools.
SvDef::StringVector streamToolsToXmlFile(const std::vector<uint32_t>& rToolIds, const std::string& rXmlFilePath);

std::vector<uint32_t> parseTreeToTool(SvXml::SVXMLMaterialsTree& rTree, SVObjectClass& rOwner);