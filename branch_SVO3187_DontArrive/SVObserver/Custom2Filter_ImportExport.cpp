//*****************************************************************************
/// COPYRIGHT (c) 2015,2015 by Körber Pharma Inspection GmbH. All Rights Reserved
/// All Rights Reserved 
/// \Author	Marc Ziegler
//*****************************************************************************
/// This file implemented the function for import and export custom2-Filter 
//*****************************************************************************
#pragma region Includes
#include "stdafx.h"
//Moved to precompiled header: #include <fstream>
#include "SVObserver.h"
#include "ObjectInterfaces/ICustom2Filter.h"
#include "ObjectInterfaces/IObjectWriter.h"
#include "SVXMLLibrary/SVXMLMaterialsTree.h"
#include "SVXMLLibrary/SaxXMLHandler.h"
#include "SVXMLLibrary/SVConfigurationTags.h"
#include "SVXMLLibrary/SVObjectXMLWriter.h"
#include "SVXMLLibrary/SVNavigateTree.h"
#include "SVUtilityLibrary/StringHelper.h"
#pragma endregion Includes

#pragma region local helper function
//************************************
// Description: This method writes an element into the XML file
// Parameter: rWriter <in> Reference to the XML writer
// Parameter: ResourceID <in> Resource ID containing the tag for the value
// Parameter: Value <in> Variant value to write into the file
//************************************
void writeElement(SvOi::IObjectWriter& rWriter, UINT ResourceID, variant_t Value )
{
	std::string Label = SvUl::LoadStdString( ResourceID );
	rWriter.WriteAttribute( Label.c_str(), Value);
}

//************************************
// Description: This method reads the element in the tree and branch of the given resource ID
// Parameter: rTree <in> Reference to the tree object
// Parameter: rBranch <in> Reference to the branch in which the value is
// Parameter: ResourceID <in> Resource ID containing the tag to the value in the branch
// Return: The value read from the XML entry as a variant
//************************************
variant_t readElement( SvXml::SVXMLMaterialsTree& rTree, const SvXml::SVXMLMaterialsTree::SVBranchHandle& rBranch, UINT ResourceID )
{
	variant_t Value;

	std::string Label = SvUl::LoadStdString( ResourceID );
	if( !SvXml::SVNavigateTree::GetItem( rTree, Label.c_str(), rBranch, Value ) )
	{
		Value.Clear();
	}
	return Value;
}

//************************************
//! This method reads the Custom2 Filter branch from the XML tree
//! \param rTree [in] The tree object
//! \param kernelWidth [out]
//! \param kernelHeight [out]
//! \param normalizationFactor [out]
//! \param absoluteValue [out]
//! \param clippingEnabled [out]
//! \param kernelArray [out]
//! \returns bool
//************************************
bool readCustom2FilterBranch( SvXml::SVXMLMaterialsTree& rTree,
	long &kernelWidth, 
	long &kernelHeight, 	
	long &normalizationFactor, 
	BOOL &absoluteValue, 
	BOOL &clippingEnabled,
	std::vector<long> &kernelArray)
{
	SvXml::SVXMLMaterialsTree::SVBranchHandle Branch;
	bool Result(false);

	std::string Label = SvUl::LoadStdString( IDS_CLASSNAME_CUSTOM2FILTER );
	if ( SvXml::SVNavigateTree::GetItemBranch( rTree, Label.c_str(), nullptr, Branch ) )
	{
		_variant_t Value;

		kernelWidth = readElement( rTree, Branch, IDS_OBJECTNAME_CUSTOMFILTER_KERNELWIDTH );
		kernelHeight = readElement( rTree, Branch, IDS_OBJECTNAME_CUSTOMFILTER_KERNELHEIGHT );
		normalizationFactor = readElement( rTree, Branch, IDS_OBJECTNAME_CUSTOMFILTER_TRANSFORM );
		absoluteValue = readElement( rTree, Branch, IDS_OBJECTNAME_CUSTOMFILTER_ABSOLUTE );
		clippingEnabled = readElement( rTree, Branch, IDS_OBJECTNAME_CUSTOMFILTER_CLIPPING );

		Label = SvUl::LoadStdString( IDS_OBJECTNAME_CUSTOMFILTER_KERNELCELL );
		SVTreeType::SVBranchHandle Elements = nullptr;
		if ( SvXml::SVNavigateTree::GetItemBranch( rTree, Label.c_str(), Branch, Elements ) )
		{
			SVTreeType::SVLeafHandle Leaf;

			Leaf = rTree.getFirstLeaf( Elements );

			kernelArray.clear();
			while ( rTree.isValidLeaf( Elements, Leaf ) )
			{
				rTree.getLeafData( Leaf, Value );
				kernelArray.push_back( Value );
				Leaf = rTree.getNextLeaf( Elements, Leaf );
			}
		}
		Result = true;
	}

	return Result;
}
#pragma endregion local helper function

#pragma region function-implementation of ICustom2Filter
void SvOi::exportCustom2Filter(const std::string &filePath, 
	long kernelWidth, 
	long kernelHeight, 
	long normalizationFactor, 
	BOOL absoluteValue, 
	BOOL clippingEnabled,
	const std::vector<long>::const_iterator kernelIteratorBegin,
	const std::vector<long>::const_iterator kernelIteratorEnd)
{
	std::ofstream FileStream;
	FileStream.open( filePath.c_str() );
	if( FileStream.is_open() )
	{
		SvXml::SVObjectXMLWriter XmlWriter( FileStream );

		XmlWriter.WriteRootElement( _T( "Custom2_Filter_Export") );
		XmlWriter.WriteSchema();

		_variant_t xmlnsValue;
		_variant_t Value;

		xmlnsValue.SetString( "x-schema:#SVR00001" );

		Value.SetString( "SV_BASENODE" );

		XmlWriter.StartElement( _T("Base") );
		XmlWriter.ElementAttribute( _T("xmlns"), xmlnsValue );
		XmlWriter.ElementAttribute( _T("Type"), Value );

		Value.Clear();
		Value = TheSVObserverApp().getCurrentVersion();

		XmlWriter.StartElement( SvXml::CTAG_ENVIRONMENT  );
		XmlWriter.WriteAttribute( SvXml::CTAG_VERSION_NUMBER, Value );
		XmlWriter.EndElement();
		std::string Label = SvUl::LoadStdString( IDS_CLASSNAME_CUSTOM2FILTER );
		XmlWriter.StartElement( Label.c_str() );

		writeElement( XmlWriter, IDS_OBJECTNAME_CUSTOMFILTER_KERNELWIDTH, kernelWidth );
		writeElement( XmlWriter, IDS_OBJECTNAME_CUSTOMFILTER_KERNELHEIGHT, kernelHeight );
		writeElement( XmlWriter, IDS_OBJECTNAME_CUSTOMFILTER_TRANSFORM, normalizationFactor );
		writeElement( XmlWriter, IDS_OBJECTNAME_CUSTOMFILTER_ABSOLUTE, absoluteValue );
		writeElement( XmlWriter, IDS_OBJECTNAME_CUSTOMFILTER_CLIPPING, clippingEnabled );

		Label = SvUl::LoadStdString( IDS_OBJECTNAME_CUSTOMFILTER_KERNELCELL );
		XmlWriter.StartElement( Label.c_str() );
		std::vector<_variant_t> KernelArray;
		std::vector<long>::const_iterator Iter( kernelIteratorBegin );
		while( kernelIteratorEnd != Iter )
		{
			KernelArray.push_back( *Iter );
			++Iter;
		}
		XmlWriter.WriteAttribute( _T("Element"), KernelArray );

		XmlWriter.EndElement();// end of Kernel
		XmlWriter.EndElement();// end of Custom2 Filter

		XmlWriter.EndElement(); // end of BaseNode
		XmlWriter.EndElement(); // end of Root Element

		FileStream.close();
	}
}	

HRESULT SvOi::importCustom2Filter(const std::string& rFileName,
	long &kernelWidth, 
	long &kernelHeight, 
	long &normalizationFactor, 
	BOOL &absoluteValue, 
	BOOL &clippingEnabled,
	std::vector<long> &kernelArray)
{
	SvXml::SVXMLMaterialsTree Tree;
	SvXml::SaxXMLHandler<SVTreeType>  SaxHandler;

	HRESULT Result  = SaxHandler.BuildFromXMLFile(&Tree, rFileName.c_str() );	
	
	if(SUCCEEDED(Result))
	{
		SvXml::SVXMLMaterialsTree::SVBranchHandle Branch;
		if ( SvXml::SVNavigateTree::GetItemBranch( Tree, SvXml::CTAG_ENVIRONMENT, nullptr, Branch ) )
		{
			_variant_t Value;
			//At this moment in time we just check that the version number tag is in the file but do not worry which version saved the file
			if ( SvXml::SVNavigateTree::GetItem( Tree, SvXml::CTAG_VERSION_NUMBER, Branch, Value ) )
			{
				if( !readCustom2FilterBranch( Tree, kernelWidth, kernelHeight, normalizationFactor, absoluteValue, clippingEnabled, kernelArray ) )
				{
					Result = E_CUSTOM_IMPORT_FORMAT_INVALID;
				}
			}
			else
			{
				Result = E_CUSTOM_IMPORT_VERSION_MISMATCH;
			}
		}
		else
		{
			Result = E_CUSTOM_IMPORT_FORMAT_INVALID;
		}

	}
	return Result;
}
#pragma endregion function-implementation of ICustom2Filter