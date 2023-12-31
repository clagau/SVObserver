//*****************************************************************************
/// \copyright (c) 2015,2015 by K�rber Pharma Inspection GmbH. All Rights Reserved
/// All Rights Reserved 
//*****************************************************************************
/// class  class SVSaxXMLHandler implements ISaxElementHandler
//******************************************************************************

#pragma once
#pragma region Includes
#include "ISaxElementHandler.h"
#include "SaxParser.h"
#include "SaxTreeElement.h"
#include "SaxEncryptionHandler.h"
#include "SVXMLMaterialsTree.h"
#pragma endregion Includes

namespace SvXml
{
#pragma region Declarations 
class SaxEncryptionHandler;
#pragma endregion Declarations 

/// /brief Implement ISaxElementHandler 
/// The class holds the SaxParser and Implements ISaxElementHandler (the callback function for the sax parser)
/// the class holds 3 trees RevisonTree, EncryptionTree, and m_pData_Tree for the data
///naming convention from msxml2.h is used 
class SaxXMLHandler : public SvXml::ISaxElementHandler
{
#pragma region Constructor
public:
	SaxXMLHandler(void);
	virtual ~SaxXMLHandler(void);
#pragma endregion Constructor
			
#pragma region Public Methods
public:
	//************************************
	//! Set an Encryption Handler
	//! \param pSaxEncryptionHandler [in]
	//! \returns void
	//************************************
	void SetSaxEncryptionHandler(SaxEncryptionHandler *pSaxEncryptionHandler);
			
	//************************************
	//! Callback function is called when an XML Elements start
	//! \param pwchNamespaceUri [in]
	//! \param cchNamespaceUri [in] len of pwchNamespaceUri
	//! \param pwchName [in]
	//! \param cchName [in]
	//! \param pAttributes [in]
	//! \param depth [in]
	//! \returns HRESULT
	//************************************
	virtual HRESULT  OnStartElement(const wchar_t  *pwchNamespaceUri, int cchNamespaceUri, const wchar_t *pwchName,  int cchName,ISAXAttributes *pAttributes,int depth ) override;

	//************************************
	//! Callback function is called for an XML data. One Line of data leads to several function calls 
	//! \param pwchData [in]
	//! \param cchData [in] len of pwchData
	//! \param depth [in]
	//! \returns HRESULT
	//************************************
	virtual HRESULT  OnElementData(const wchar_t *pwchData,  int cchData, int depth) override;

	//************************************
	//! Callback function is called when an XML Elements end
	//! \param pwchNamespaceUri [in] 
	//! \param cchNamespaceUri [in] len of pwchNamespaceUri
	//! \param pwchName [in]
	//! \param cchName [in]
	//! \param depth [in]
	//! \returns HRESULT
	//************************************
	virtual HRESULT  OnEndElement(const wchar_t  *pwchNamespaceUri, int cchNamespaceUri,const wchar_t *pwchName,  int cchName, int depth ) override;

	//************************************
	//! Callback Function when the saxparser locates  an error
	//! \param line [in,]
	//! \param column [in]
	//! \param pwchErrorText [in]
	//! \param errorCode [in]
	//! \param fatal [in] true if the error is fatal 
	//! returns HRESULT S_OK or E_FAIL if the parse operation should be aborted. 
	//************************************
	virtual HRESULT  OnXMLError(int line, int column, const wchar_t *pwchErrorText, unsigned long errorCode, bool fatal ) override;

			
	//************************************
	//! Writes to an XML file used by sax testprogram 
	//! \param path [in] Filename 
	//! \param what [in]
	//! \returns bool
	//************************************
	bool WriteToXML(const wchar_t* path, EPARSING what); 

	//************************************
	//! Parses the xml File to dataTre  and to m_EncryptionTree and m_RevisionTree
	//! \param dataTree [in]
	//! \param pwstrPath [in
	//! \returns HRESULT
	//************************************
	HRESULT BuildFromXMLFile(SVXMLMaterialsTree*  dataTree , LPCTSTR FileName);
			
	//************************************
	//! //! Parses the xml string in var to to dataTre  and to m_EncryptionTree and m_RevisionTree
	//! \param dataTree [in]
	//! \param var [in]
	//! \returns HRESULT
	//************************************
	HRESULT  BuildFromXMLString(SVXMLMaterialsTree*  dataTree , const _variant_t &var);
			
	//************************************
	//! Time forparsing
	//! \returns DWORD
	//************************************
	DWORD GetParseTime(void ) const;

	//************************************
	//! Set the startnode for the data tree
	//! \param aName [in]
	//! \param atype [in]
	//! \returns void
	//************************************
	void SetStartNodeName(const wchar_t* aName , const wchar_t* atype = L"SV_BASENODE" );
			
			
	//************************************
	//! Time for writing xml file 
	//! \returns DWORD
	//************************************
	DWORD  GetWriteTime(void ) const;
			
#pragma endregion Public Methods

#pragma region Private Methods
private:
	void PrintConfigTree(std::wstringstream &myStream, const tcl::sequential_tree<SpTreeElement>& rtree);

	//************************************
	//! clears internal variables 
	//! \returns void
	//************************************
	void Clear();
			
	//************************************
	//! clears internal vector variables 
	//! \param newName [in,out]
	//! \returns void
	//************************************
	void VectorClear(const wchar_t* newName );

	//************************************
	//! Returns the current element or null
	//! \returns SaxTreeElement*
	//************************************
	SaxTreeElement* GetCurrentSaxTreeElement(); 
			
	//************************************
	//! return the current array as variant
	//! \returns VARIANT
	//************************************
	_variant_t GetVariantArray();

	//************************************
	//! return true if  an valid encryption part was read
	//! \returns bool
	//************************************
	bool IsEncrypted();
			
	//************************************
	//! Initialize the encryption Handler with values from  m_EncryptionTree
	//! \returns void
	//************************************
	void SetDecryption();
#pragma endregion Private Methods

#pragma region Member variables
private:
	int m_depth;		/// depth of the actual tree
	EPARSING m_status;  /// the actual parsing status 
	SpTreeElement m_spNewElement; /// this is the actual Element when parsing PARSING_DATATREE
	DWORD m_WriteTime;  /// time of the parsing
	bool m_ParseArray;  /// status when parsing arrays 

	std::wstring m_StartNodeAttName;  ///Name of the Element which starts PARSING_DATATREE
	std::wstring m_StartNodeAttType;  ///Type  of the Element which starts PARSING_DATATREE	

	SVXMLMaterialsTree*  m_pData_Tree;      /// when PARSING_DATATREE datas are insert here 
			
	SVXMLMaterialsTree::SVBranchHandle m_CurrentBranchHandle; /// current branch of m_pData_Tree

	tcl::sequential_tree<SpTreeElement> m_RevisionTree;   /// When Parsing Revesion the data goes here 
	tcl::sequential_tree<SpTreeElement>* m_pCurrentNodeInRevisionTree; /// current branch in Revision tree

	tcl::sequential_tree<SpTreeElement> m_EncryptionTree;
	tcl::sequential_tree<SpTreeElement>* m_pCurrentNodeInEncryptionTree;
	SvXml::SaxParser m_SaxParser;

	std::unique_ptr<std::vector<int>> m_spIntVector;   /// when Parsing VT_Array int goes here 
	std::unique_ptr<std::vector<UINT>> m_spUINTVector; /// when Parsing VT_Array uint goes here 
	std::unique_ptr<std::vector<double>> m_spDoubleVector; /// when Parsing VT_Array uint goes here 
	std::unique_ptr<std::vector<std::wstring>> m_spWstringVector; /// when Parsing VT_Array wstring goes here 
	std::wstring m_ArrayName;   /// when parsing VT_Array this is the name of the array 

	SaxEncryptionHandler *m_pSaxEncryptionHandler;
#pragma endregion Member variables
};

/// This function converts the XML to tree
/// \param rXmlData [in] Reference to the XML data to convert
/// \param rTree [out] Reference to the tree to store the XML conversion
/// Exception in error case.
void convertXmlToTree(const std::string& rXmlData, SvXml::SVXMLMaterialsTree& rTree);
} //namespace SvXml
