//*****************************************************************************
/// \copyright (c) 2016,2016 by Seidenader Maschinenbau GmbH
/// All Rights Reserved 
//*****************************************************************************
/// SaxExtractPropertiesHandler
/// The class holds the SaxParser and Implements ISaxElementHandler (the callback function for the sax parser)
/// the class holds 3 trees RevisonTree, EncryptionTree, and m_DataTree for the data and a Propertiemap 
/// During Parsing the Propertie map m_Properties  is build. When all properties found parsing is stoped.
///naming convention from msxml6.h is used 
//******************************************************************************
#pragma once

#pragma region Includes
#include "ISaxElementHandler.h"
#include "SaxParser.h"
#include "SaxTreeElement.h"
#pragma endregion Includes

#pragma region Declarations
#pragma endregion Declarations

namespace SvXml
{
	struct SaxProperty
	{
		SaxProperty(): Type(VT_I4),
		IsFound(false) {};

	std::wstring NodeName;
	std::wstring DataName;
	std::wstring Content;
	bool IsFound;
	VARTYPE Type;
	};


	/// /brief Implement ISaxElementHandler 
	/// The class holds the SaxParser and Implements ISaxElementHandler (the callback function for the sax parser)
	/// the class holds 3 trees RevisonTree, EncryptionTree, and m_DataTree for the data and a Propertiemap 
	/// During Parsing the Propertie map m_Properties  is build. When all properties found parsing is stoped.
	///naming convention from msxml6.h is used 

	class SaxExtractPropertiesHandler  : public SvXml::ISaxElementHandler
	{
#pragma region Constructor
	public:
		SaxExtractPropertiesHandler();
		virtual ~SaxExtractPropertiesHandler();
#pragma endregion Constructor

#pragma region Public Methods
	public:
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
		//! Set the startnode for the data tree
		//! \param aName [in]
		//! \param atype [in]
		//! \returns void
		//************************************
		void SetStartNodeName(const wchar_t* aName , const wchar_t* atype = L"SV_BASENODE" );

		//! Extract the properties from the xmlfile stops parsing when the properties are found 
		HRESULT  ExtractProperties(LPCTSTR FileName, long& rNewDisableMethod, long& rEnableAuxExtents, unsigned long& rVersionNumber);

		HRESULT  ExtractProperties(LPCTSTR FileName, unsigned long& rVersionNumber);

#pragma endregion Public Methods

#pragma region Private Methods
	private:

		
		//************************************
		//! Parses the xml File to dataTre  and to m_EncryptionTree and m_RevisionTree only called by Extractproperties 
		//! \param pwstrPath [in
		//! \returns HRESULT
		//************************************
		HRESULT   BuildFromXMLFile(LPCTSTR pwstrPath);

		
		//************************************
		//! Returns the current element or null
		//! \returns SaxTreeElement*
		//************************************
		SaxTreeElement* GetCurrentSaxTreeElement(); 


#pragma endregion Private Methods
		//************************************
		//! clears internal variables 
		//! \returns void
		//************************************
		void Clear();

#pragma region Member variables
	private:
		int m_depth;		/// depth of the actual tree
		EPARSING m_status;  /// the actual parsing status 

		std::map<std::wstring, SaxProperty>  m_Properties;
		bool						m_BreakIfComplete;	
		bool						m_Complete;


		std::wstring m_StartNodeAttName;  ///Name of the Element which starts PARSING_DATATREE
		std::wstring m_StartNodeAttType;  ///Type  of the Element which starts PARSING_DATATREE	

		tcl::sequential_tree<SpTreeElement> m_DataTree;   /// When Parsing Revesion the data goes here 
		tcl::sequential_tree<SpTreeElement>* m_pCurrentNodeInDataTree; /// current branch in Revision tree

		tcl::sequential_tree<SpTreeElement> m_RevisionTree;   /// When Parsing Revesion the data goes here 
		tcl::sequential_tree<SpTreeElement>* m_pCurrentNodeInRevisionTree; /// current branch in Revision tree

		tcl::sequential_tree<SpTreeElement> m_EncryptionTree;
		tcl::sequential_tree<SpTreeElement>* m_pCurrentNodeInEncryptionTree;
		SvXml::SaxParser m_SaxParser;

		std::unique_ptr<std::vector<int>> m_spIntVector;   /// when Parsing VT_Array int goes here 
		std::unique_ptr<std::vector<UINT>> m_spUINTVector; /// when Parsing VT_Array uint goes here 
		std::unique_ptr<std::vector<std::wstring>> m_spWstringVector; /// when Parsing VT_Array wstring goes here 

#pragma endregion Member variables
	};
} //namespace SvXml
