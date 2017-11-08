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
#pragma endregion Includes

//Forward declarations
class SVGUID;
class SVInspectionProcess;
class SVToolClass;
namespace SvXml
{
	class SVObjectXMLWriter;
}

class ToolClipboard
{
public:
#pragma region Declarations
	typedef SvXml::SVXMLMaterialsTree SVTreeType;
#pragma endregion Declarations

public:
#pragma region Constructor
	//************************************
	// Description: The class constructor
	//************************************
	ToolClipboard( SVInspectionProcess& rInspection );

	//************************************
	// Description: The class destructor
	//************************************
	virtual ~ToolClipboard();
#pragma endregion Constructor

public:
#pragma region Public Methods
	//************************************
	// Description: Write tool to the clipboard
	// Parameter: rToolGuid <in> Reference to the selected tool GUID to write to the clipboard
	// Return: S_OK on success
	//************************************
	HRESULT writeToClipboard( const SVGUID& rToolGuid ) const;

	//************************************
	// Description: Read tool from the clipboard
	// Parameter: ToolListindex <in> The tool list index where the tool will be inserted
	// Parameter: rToolGuid <out> Reference to the tool GUID generated from reading the clipboard
	// Return: S_OK on success
	//************************************
	HRESULT readFromClipboard( int ToolListindex, SVGUID& rToolGuid );

	//************************************
	// Description: Checks to see if the clipboard data is valid
	// Return: True when valid
	//************************************
	static bool isClipboardDataValid();
#pragma endregion Public Methods

protected:
#pragma region Protected Methods
	//************************************
	// Description: This method writes the tool to a zip file
	// Parameter: rFileName <in> Reference to the file name for the zip file
	// Parameter: rToolGuid <in> Reference to the selected tool GUID to write to the clipboard
	// Return: S_OK on success
	//************************************
	HRESULT streamToolToZip( const std::string rFileName, const SVGUID& rToolGuid ) const;

	//************************************
	// Description: This method writes the Base and Environment nodes
	// Parameter: rXmlWriter <in> Reference to the XML writer
	//************************************
	void writeBaseAndEnvironmentNodes(SvXml::SVObjectXMLWriter& rXmlWriter ) const;

	//************************************
	// Description: This method writes the Guids of sources like Inspection and Image
	// Parameter: rXmlWriter <in> Reference to the XML writer
	// Parameter: rTool <in> Reference to the tool to save
	//************************************
	void writeSourceGuids(SvXml::SVObjectXMLWriter& rXmlWriter, SVToolClass& rTool ) const;

	//************************************
	// Description: This method finds the dependency files in the tool Xml string
	// Parameter: rToolXmlString <in> Reference to the tool XML string to search
	// Parameter: rDependencyFiles <out> Reference to the set of dependency files
	//************************************
	void findDependencyFiles( const std::string& rToolXmlString, SvDef::StringSet& rDependencyFiles ) const;

	//************************************
	// Description: This method updates dependency files if required
	// Parameter: rDependencyFiles <in> Reference to the set of dependency files
	//************************************
	void updateDependencyFiles( const SvDef::StringSet& rDependencyFiles ) const;

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

	//************************************
	// Description: This method validates Guids and replaces the image if required
	// Parameter: rXmlData <in, out> Reference to the XML data to search and replace
	// Parameter: rTree <in> Reference to the tree generated from the clipboard
	// Return: S_OK on success
	//************************************
	HRESULT validateGuids( std::string& rXmlData, SVTreeType& rTree, int ToolListindex ) const;

	//************************************
	// Description: This method replaces the tool name
	// Parameter: rXmlData <in, out> Reference to the XML data to search and replace
	// Parameter: rTree <in> Reference to the tree generated from the clipboard
	// Return: S_OK on success
	//************************************
	HRESULT replaceToolName( std::string& rXmlData, SVTreeType& rTree ) const;

	//************************************
	// Description: This method replaces all the unique Guids
	// Parameter: rXmlData <in, out> Reference to the XML data to search and replace
	// Parameter: rTree <in> Reference to the tree generated from the clipboard
	// Return: S_OK on success
	//************************************
	HRESULT replaceUniqueGuids( std::string& rXmlData, SVTreeType& rTree ) const;

	//************************************
	// Description: This method parses the tree and generates the tool
	// Parameter: rTree <in> Reference to the tree to parse
	// Parameter: rToolGuid <out> Reference to the tool GUID generated
	// Return: S_OK on success
	//************************************
	HRESULT parseTreeToTool( SVTreeType& rTree, SVGUID& rToolGuid );

#pragma endregion Protected Methods

private:
#pragma region Member Variables
	SVInspectionProcess&	 m_rInspection;							//Reference to the corresponding inspection
#pragma endregion Member Variables
};

