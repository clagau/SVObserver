//*****************************************************************************
/// \copyright (c) 2015,2015 by Seidenader Maschinenbau GmbH
/// All Rights Reserved 
//*****************************************************************************
///  class SaxTreeElement
///  class encapsulate info for an XML Element 
//*********

#pragma once
#pragma region Includes
#include <string>
#include <memory> 
#include <map>
#include <msxml2.h>
#pragma endregion Includes

namespace Seidenader
{
	namespace  SVXMLLibrary
	{

		///Describes  status of the Sax Parser 
		enum EPARSING 
		{
			PARSING_NONE,
			PARSING_REVISION,
			PARSING_ENCRYPTION,
			PARSING_DATATREE,

		};

		///\brief   class encapsulate info for an XML Element
		class SaxTreeElement
		{
		public:

			/// known XML Elements 
			enum EType 
			{
				DATA_Element =0,
				NODE_Element,
				Revision_Element,
				RevisionHistory_Element,
				Encryption_Element,
				Root_Element,
				Unknown_Element 
			};

			
			#pragma region Constructor
			SaxTreeElement(const wchar_t  *pwchName, int cchName);
			SaxTreeElement(std::wstring const &name);
			#pragma endregion Constructor

			#pragma region Public Methods
			//************************************
			//! Get the Element Type
			//! \returns Seidenader::SaxWrapper::SaxTreeElement::EType
			//************************************
			EType GetElementType() const;

			//************************************
			//! True if the Element is a Data Base Node Element 
			//! \param lpAttType [in] Base Node Type 
			//! \param lpoAttName [in]  Base Node Name 
			//! \returns bool
			//************************************
			bool IsDataBaseNode(LPCWSTR lpAttType, LPCWSTR lpoAttName ) const;
			//************************************
			//! true for RevisionHistory_Element
			//! \returns bool
			//************************************
			bool IsRevisionBaseNode() const;
			//************************************
			//! true for Encryption_Element
			//! \returns bool
			//************************************
			bool IsEncryptionBaseNode() const;
			//************************************
			//! true for node with arraytype
			//! \returns bool
			//************************************
			bool IsVtArrayNode() const;
			//************************************
			//! SetAttribut
			//! \param pAttributes [in]
			//! \returns int
			//************************************
			int SetAttributes(ISAXAttributes *pAttributes); 
			//************************************
			//! Get dept
			//! \returns int
			//************************************
			int GetDepth() const;
			//************************************
			//! set dephth
			//! \param depth [in]
			//! \returns void
			//************************************
			void SetDepth(int depth ); 
			//************************************
			//! Clear Content string  if it is only whitespace 
			//! \returns void
			//************************************
			void  ClearEmptyContent();
			//************************************
			//! get the Content string 
			//! \returns const wchar_t *
			//************************************
			const wchar_t *  GetContent() const;
			//************************************
			//! Append to content string 
			//! \param cont [in]
			//! \param len [in]
			//! \returns void
			//************************************
			void AppendToContent(const wchar_t * cont, int len);
			//************************************
			//! Get the Type attribute as Vartype 
			//! \returns VARTYPE
			//************************************
			VARTYPE GetVarTypeAtt();
			//************************************
			//! get the Type Attribute  as string 
			//! \returns const wchar_t*
			//************************************
			const wchar_t*  GetTypeAtt() const;
			//************************************
			//! Get the name Attribute  as string 
			//! \returns const wchar_t*
			//************************************
			const wchar_t*  GetNameAtt() const;

			//************************************
			//! convert namestring to Etype
			//! \param name [in]
			//! \returns Seidenader::SaxWrapper::SaxTreeElement::EType
			//************************************
			static EType GetElementType(LPCWSTR name);
		
			///Print functions only used by the testprogramm 
			void PrintElement(std::wstringstream& stream );
			void PrintElementStart(std::wstringstream& stream );
			void PrintElementEnd(std::wstringstream& stream );
		#pragma endregion Public Methods
		
		public:
			const static std::wstring ElementName[];
			const static std::wstring s_NameAttTag;
			const static std::wstring s_TypeAttTag;
		
		public:	
			//************************************
			//! Compare function necessary tree library
			//! \param lhs [in]
			//! \param rhs [in]
			//! \returns friend bool
			//************************************
			friend bool operator <(const SaxTreeElement& lhs, const SaxTreeElement& rhs)
			{
				return true;
			};


			

		protected:

			EType m_ElementType;    
			std::wstring m_Elementname; 
			
			bool m_bNameAtt;			///< True if a Name Attribute exist
			std::wstring m_NameAtt;    ///< value of the name Attribute 
			
			bool m_bTypeAtt;          ///< True if a Type Attribute exist 
			std::wstring m_TypeAtt;   ///<Value of the type attribute 
			VARTYPE m_VarTypeAtt;    ///< Value of the type attribute  as type
			
			std::wstring m_Content;   

			std::map<std::wstring,std::wstring> m_SpecialAtributes; //only used for special attributes 
			int m_depth;

			


		};

		typedef   std::shared_ptr<SaxTreeElement> SpTreeElement;  

	}
}
namespace SvXml = Seidenader::SVXMLLibrary;
