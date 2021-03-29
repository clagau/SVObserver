//******************************************************************************
//* COPYRIGHT (c) 2014 by Seidenader
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
#include "SVXMLLibrary/SVXMLMaterialsTree.h"
#include "Definitions/StringTypeDef.h"
#include "SVStatusLibrary/MessageManager.h"
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
class SVInspectionProcess;

class ToolClipboard
{
public:
#pragma region Declarations
	typedef SvXml::SVXMLMaterialsTree SVTreeType;
#pragma endregion Declarations

public:
#pragma region Public Methods
	//************************************
	// Description: Write tool to the clipboard
	// Parameter: toolId <in> Reference to the selected tool Id to write to the clipboard
	// Return: S_OK on success
	//************************************
	HRESULT writeToClipboard(uint32_t toolId) const;

	//************************************
	// Description: Read tool from the clipboard
	// Parameter: postId <in> Reference to the tool ID selected where the tool is to be inserted
	// Parameter: ownerId <in> Reference to the owner ID 
	// Parameter: rToolId <out> Reference to the tool ID generated from reading the clipboard
	// Return: S_OK on success
	//************************************
	HRESULT readFromClipboard(uint32_t postId, uint32_t ownerId, uint32_t& rToolId);

	//************************************
	// Description: Checks to see if the clipboard data is valid
	// Return: True when valid
	//************************************
	static bool isClipboardDataValid();

	const SvStl::MessageManager& getLastErrorMessage() const { return m_errorMessage; }
#pragma endregion Public Methods

protected:
#pragma region Protected Methods
	//************************************
	// Description: This method writes the tool to a zip file
	// Parameter: rFileName <in> Reference to the file name for the zip file
	// Parameter: toolId <in> Reference to the selected tool ID to write to the clipboard
	// Return: S_OK on success
	//************************************
	HRESULT streamToolToZip(const std::string& rFileName, uint32_t toolId) const;

	//************************************
	// Description: This method writes the Base and Environment nodes
	// Parameter: rXmlWriter <in> Reference to the XML writer
	//************************************
	void writeBaseAndEnvironmentNodes(SvOi::IObjectWriter& rWriter) const;

	//************************************
	// Description: This method writes the ids of sources like Inspection and Image
	// Parameter: rXmlWriter <in> Reference to the XML writer
	// Parameter: rTool <in> Reference to the tool to save
	//************************************
	void writeSourceIds(SvOi::IObjectWriter& rWriter, SvTo::SVToolClass& rTool ) const;

	//************************************
	// Description: This method finds the dependency files in the tool Xml string
	// Parameter: rToolXmlString <in> Reference to the tool XML string to search
	// Parameter: rDependencyFiles <out> Reference to the set of dependency files
	//************************************
	void findDependencyFiles( const std::string& rToolXmlString, SvDef::StringVector& rDependencyFiles ) const;

	//************************************
	// Description: This method updates dependency files if required
	// Parameter: rDependencyFiles <in> Reference to the set of dependency files
	//************************************
	void updateDependencyFiles( const SvDef::StringVector& rDependencyFiles ) const;

	//************************************
	// Description: This method gets the clipboard data and converts it into a string
	// Parameter: rClipboardData <out> Reference to the string to hold the clipboard data
	// Return: S_OK on success
	//************************************
	static HRESULT convertClipboardDataToString( std::string& rClipboardData );

	//************************************
	// Description: This method writes the string to a file
	// Parameter: rFileName <in> Reference to the file name
	// Parameter: rFileData <in> Reference to where the file data is
	// Parameter: Text <in> data is text format
	//************************************
	void writeStringToFile( const std::string& rFileName, const std::string& rFileData, bool Text ) const;

	//************************************
	// Description: This method reads the file to a string
	// Parameter: rFileName <in> Reference to the file name
	// Parameter: rFileData <out> Reference to where the data is read in to
	//************************************
	void readFileToString( const std::string& rFileName, std::string& rFileData ) const;

	//************************************
	// Description: This method converts the XML to tree
	// Parameter: rXmlData <in> Reference to the XML data to convert
	// Parameter: rTree <out> Reference to the tree to store the XML conversion
	// Return: S_OK on success
	//************************************
	HRESULT convertXmlToTree( const std::string& rXmlData, SVTreeType& rTree ) const;

	//************************************
	// Description: This method checks the clipboard SVObserver version to the current SVObserver version
	// Parameter: rTree <in> Reference to the tree
	// Return: S_OK on success
	//************************************
	HRESULT checkVersion( SVTreeType& rTree ) const;

	/// This method reads the tools data
	/// \param rXmlData [in,out] Reference to the XML data to search and replace
	/// \param rTree [in] Reference to the tree generated from the clipboard
	/// \param postId [in] The id of the object currently selected where the new object should be inserted
	/// \param ownerId [in] The id of the owner of the new object.
	/// \returns HRESULT S_OK on success
	HRESULT readTool(std::string& rXmlData, SVTreeType& rTree, uint32_t postId, uint32_t ownerId) const;

	/// This method validates the tools IDs
	/// \param rXmlData [in,out] Reference to the XML data to search and replace
	/// \param postId [in] The id of the object currently selected where the new object should be inserted
	/// \param ownerId [in] The id of the owner of the new object.
	/// \param inspectionId [in] The id of the inspection
	/// \param toolClassId [in] The class Id of the tool
	/// \param rInputImages [in] The input images of the tool
	/// \returns HRESULT S_OK on success
	HRESULT validateIds(std::string& rXmlData, uint32_t postId, uint32_t ownerId, uint32_t inspectionId, SvPb::ClassIdEnum toolClassId, std::set<uint32_t> rInputImages) const;

	//************************************
	/// This method replaces the tool name and the dotted name (e.g. in Equation)
	/// \param rXmlData [in,out] Reference to the XML data to search and replace
	/// \param rTree [in] Reference to the tree generated from the clipboard
	/// \param pOwner [in] The owner of the new object.
	/// \returns HRESULT S_OK on success
	HRESULT replaceToolName( std::string& rXmlData, SVTreeType& rTree, const SVObjectClass* pOwner ) const;

	//************************************
	// Description: This method replaces all the unique ids
	// Parameter: rXmlData <in, out> Reference to the XML data to search and replace
	// Parameter: rTree <in> Reference to the tree generated from the clipboard
	// Return: S_OK on success
	//************************************
	HRESULT replaceUniqueIds( std::string& rXmlData, SVTreeType& rTree ) const;

	//************************************
	// Description: This method parses the tree and generates the tool
	// Parameter: rTree <in> Reference to the tree to parse
	// Parameter: rToolId <out> Reference to the tool ID generated
	// Return: S_OK on success
	//************************************
	HRESULT parseTreeToTool(SVTreeType& rTree, SVObjectClass* pOwner, uint32_t& rToolId);
#pragma endregion Protected Methods

private:
#pragma region Member Variables
	mutable SVInspectionProcess* m_pInspection{nullptr};
	mutable SvStl::MessageManager m_errorMessage{SvStl::MsgType::Log | SvStl::MsgType::Display};
#pragma endregion Member Variables
};

