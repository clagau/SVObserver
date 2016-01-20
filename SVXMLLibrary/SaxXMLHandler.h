//*****************************************************************************
/// \copyright (c) 2015,2015 by Seidenader Maschinenbau GmbH
/// All Rights Reserved 
//*****************************************************************************
/// class  class SVSaxXMLHandler implements ISaxElementHandler
//******************************************************************************

#pragma once
#pragma region Includes
#include "ISaxElementHandler.h"
#include "SaxParser.h"
#include "SaxTreeElement.h"
#include <sequential_tree.h>
#include "SVXMLLibrary/SVXMLMaterialsTree.h"
#include "SVXMLLibrary/SVXMLTree.h"
#pragma endregion Includes




namespace Seidenader
{
	namespace  SVXMLLibrary
	{

		/// /brief Implement ISaxElementHandler 
		/// The class holds the SaxParser and Implements ISaxElementHandler (the callback function for the sax parser)
		/// the class holds 3 trees RevisonTree, EncryptionTree, and m_pData_Tree for the data
		///naming convention from msxml6.h is used 
		template<typename TreeType>
		class SaxXMLHandler : public SvXml::ISaxElementHandler
		{
		public:
			#pragma region Constructor
			SaxXMLHandler(void);
			~SaxXMLHandler(void);
			#pragma endregion Constructor
			
#pragma region Public Methods
			//************************************
			//! Callbackfunction is called when an XML Elements start
			//! \param pwchNamespaceUri [in]
			//! \param cchNamespaceUri [in] len of pwchNamespaceUri
			//! \param pwchName [in]
			//! \param cchName [in]
			//! \param pAttributes [in]
			//! \param depth [in]
			//! \returns HRESULT
			//************************************
			virtual HRESULT  OnStartElement(const wchar_t  *pwchNamespaceUri, int cchNamespaceUri, const wchar_t *pwchName,  int cchName,ISAXAttributes *pAttributes,int depth ) ;

			//************************************
			//! Callbackfunction is called for an XML data. One Line of data leads to several function calls 
			//! \param pwchData [in]
			//! \param cchData [in] len of pwchData
			//! \param depth [in]
			//! \returns HRESULT
			//************************************
			virtual HRESULT  OnElementData(const wchar_t *pwchData,  int cchData, int depth) ;

			//************************************
			//! Callbackfunction is called when an XML Elements end
			//! \param pwchNamespaceUri [in] 
			//! \param cchNamespaceUri [in] len of pwchNamespaceUri
			//! \param pwchName [in]
			//! \param cchName [in]
			//! \param depth [in]
			//! \returns HRESULT
			//************************************
			virtual HRESULT  OnEndElement(const wchar_t  *pwchNamespaceUri, int cchNamespaceUri,const wchar_t *pwchName,  int cchName, int depth );

			//************************************
			//! CallBackFunction when the saxparser locates  an error
			//! \param line [in,]
			//! \param column [in]
			//! \param pwchErrorText [in]
			//! \param errorCode [in]
			//! \param fatal [in] true if the error is fatal 
			//! returns HRESULT S_OK or E_FAIL if the parse operation should be aborted. 
			//************************************
			virtual HRESULT  OnXMLError(int line, int column, const wchar_t *pwchErrorText, unsigned long errorCode, bool fatal );

			
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
			HRESULT BuildFromXMLFile(TreeType*  dataTree , const wchar_t * pwstrPath);
			
			//************************************
			//! //! Parses the xml string in var to to dataTre  and to m_EncryptionTree and m_RevisionTree
			//! \param dataTree [in]
			//! \param var [in]
			//! \returns HRESULT
			//************************************
			HRESULT  BuildFromXMLString(TreeType*  dataTree , const _variant_t &var);
			
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
		protected:

			void PrintConfigTree(std::wstringstream &myStream, const tcl::sequential_tree<SpTreeElement>& rtree);

			//************************************
			//! PLEASE ENTER A DESCRIPTION
			//! \returns void
			//************************************
			void Clear();

			
			void VectorClear(const wchar_t* newName );

			SaxTreeElement* GetCurrentSaxTreeElement(); 
			
			VARIANT GetVariantArray();



		protected:
			int m_depth;		/// depth of the actual tree
			EPARSING m_status;  /// the actual parsing status 
			SpTreeElement m_spNewElement; /// this is the actual Element when parsing PARSING_DATATREE
			DWORD m_WriteTime;  /// time of the parsing
			bool m_ParseArray;  /// status when parsing arrays 

			std::wstring m_StartNodeAttName;  ///Name of the Element which starts PARSING_DATATREE
			std::wstring m_StartNodeAttType;  ///Type  of the Element which starts PARSING_DATATREE	
			



			TreeType*  m_pData_Tree;      /// when PARSING_DATATREE datas are insert here 
			
			
			typename TreeType::SVBranchHandle m_CurrentBranchHandle; /// current branch of m_pData_Tree

			tcl::sequential_tree<SpTreeElement> m_RevisionTree;   /// When Parsing Revesion the data goes here 
			tcl::sequential_tree<SpTreeElement>* m_pCurrentNodeInRevisionTree; /// current branch in Revision tree


			tcl::sequential_tree<SpTreeElement> m_EncryptionTree;
			tcl::sequential_tree<SpTreeElement>* m_pCurrentNodeInEncryptionTree;
			SvXml::SaxParser      *m_pSaxParser;

			std::unique_ptr<std::vector<int>> m_spIntVector;   /// when Parsing VT_Array int goes here 
			std::unique_ptr<std::vector<UINT>> m_spUINTVector; /// when Parsing VT_Array uint goes here 
			std::unique_ptr<std::vector<std::wstring>> m_spWstringVector; /// when Parsing VT_Array wstring goes here 
			std::wstring m_ArrayName;   /// when parsing VT_Array this is the name of the array 

		};

	}
}

namespace SvXml = Seidenader::SVXMLLibrary;

#pragma region Inline
#include "SaxXMLHandler.inl"
#pragma endregion Inline