//*****************************************************************************
/// COPYRIGHT (c) 2015,2015 by Seidenader Maschinenbau GmbH
/// All Rights Reserved 
/// \Author	Marc Ziegler
//*****************************************************************************
/// This file implemented the function for import and export custom2-Filter 
//*****************************************************************************
#pragma region Includes
#include "stdafx.h"
//Moved to precompiled header: #include <fstream>
#include "SVObserver.h"
#include "SVXMLLibrary\SVObjectXMLWriter.h"
#include "SVXMLLibrary/SVConfigurationTags.h"
#include "SVXMLLibrary/SVNavigateTree.h"
#include "ObjectInterfaces/ICustom2Filter.h"
#include "SVXMLLibrary/SaxXMLHandler.h"
#include "SVUtilityLibrary/SVString.h"
#pragma endregion Includes

#pragma region local helper function
//************************************
// Description: This method writes an element into the XML file
// Parameter: rXmlWriter <in> Reference to the XML writer
// Parameter: ResourceID <in> Resource ID containing the tag for the value
// Parameter: Value <in> Variant value to write into the file
//************************************
void writeElement( SVObjectXMLWriter& rXmlWriter, UINT ResourceID, variant_t Value )
{
	SVString Label = SvUl_SF::LoadSVString( ResourceID );
	rXmlWriter.WriteAttribute( Label.c_str(), Value);
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

	SVString Label = SvUl_SF::LoadSVString( ResourceID );
	if( !SVNavigateTree::GetItem( rTree, Label.c_str(), rBranch, Value ) )
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
	SvOi::ICustom2Filter::LongArray &kernelArray)
{
	SvXml::SVXMLMaterialsTree::SVBranchHandle Branch;
	bool Result(false);

	SVString Label = SvUl_SF::LoadSVString( IDS_CLASSNAME_CUSTOM2FILTER );
	if ( SVNavigateTree::GetItemBranch( rTree, Label.c_str(), nullptr, Branch ) )
	{
		_variant_t Value;

		kernelWidth = readElement( rTree, Branch, IDS_OBJECTNAME_CUSTOMFILTER_KERNELWIDTH );
		kernelHeight = readElement( rTree, Branch, IDS_OBJECTNAME_CUSTOMFILTER_KERNELHEIGHT );
		normalizationFactor = readElement( rTree, Branch, IDS_OBJECTNAME_CUSTOMFILTER_TRANSFORM );
		absoluteValue = readElement( rTree, Branch, IDS_OBJECTNAME_CUSTOMFILTER_ABSOLUTE );
		clippingEnabled = readElement( rTree, Branch, IDS_OBJECTNAME_CUSTOMFILTER_CLIPPING );

		Label = SvUl_SF::LoadSVString( IDS_OBJECTNAME_CUSTOMFILTER_KERNELCELL );
		SVTreeType::SVBranchHandle Elements = nullptr;
		if ( SVNavigateTree::GetItemBranch( rTree, Label.c_str(), Branch, Elements ) )
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
void SvOi::exportCustom2Filter(const SVString &filePath, 
	long kernelWidth, 
	long kernelHeight, 
	long normalizationFactor, 
	BOOL absoluteValue, 
	BOOL clippingEnabled,
	const ICustom2Filter::LongArray::const_iterator kernelIteratorBegin,
	const ICustom2Filter::LongArray::const_iterator kernelIteratorEnd)
{
	std::ofstream FileStream;
	FileStream.open( filePath.c_str() );
	if( FileStream.is_open() )
	{
		SVObjectXMLWriter XmlWriter( FileStream );

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
		Value = TheSVObserverApp.getCurrentVersion();

		XmlWriter.StartElement( CTAG_ENVIRONMENT  );
		XmlWriter.WriteAttribute( CTAG_VERSION_NUMBER, Value );
		XmlWriter.EndElement();
		SVString Label = SvUl_SF::LoadSVString( IDS_CLASSNAME_CUSTOM2FILTER );
		XmlWriter.StartElement( Label.c_str() );

		writeElement( XmlWriter, IDS_OBJECTNAME_CUSTOMFILTER_KERNELWIDTH, kernelWidth );
		writeElement( XmlWriter, IDS_OBJECTNAME_CUSTOMFILTER_KERNELHEIGHT, kernelHeight );
		writeElement( XmlWriter, IDS_OBJECTNAME_CUSTOMFILTER_TRANSFORM, normalizationFactor );
		writeElement( XmlWriter, IDS_OBJECTNAME_CUSTOMFILTER_ABSOLUTE, absoluteValue );
		writeElement( XmlWriter, IDS_OBJECTNAME_CUSTOMFILTER_CLIPPING, clippingEnabled );

		Label = SvUl_SF::LoadSVString( IDS_OBJECTNAME_CUSTOMFILTER_KERNELCELL );
		XmlWriter.StartElement( Label.c_str() );
		SVVariantList KernelArray;
		ICustom2Filter::LongArray::const_iterator Iter( kernelIteratorBegin );
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

HRESULT SvOi::importCustom2Filter(const SVString& rFileName,
	long &kernelWidth, 
	long &kernelHeight, 
	long &normalizationFactor, 
	BOOL &absoluteValue, 
	BOOL &clippingEnabled,
	ICustom2Filter::LongArray &kernelArray)
{
	SvXml::SVXMLMaterialsTree Tree;
	SvXml::SaxXMLHandler<SVTreeType>  SaxHandler;

	HRESULT Result  = SaxHandler.BuildFromXMLFile(&Tree, rFileName.c_str() );	
	
	if(SUCCEEDED(Result))
	{
		SvXml::SVXMLMaterialsTree::SVBranchHandle Branch;
		if ( SVNavigateTree::GetItemBranch( Tree, CTAG_ENVIRONMENT, nullptr, Branch ) )
		{
			_variant_t Value;
			//At this moment in time we just check that the version number tag is in the file but do not worry which version saved the file
			if ( SVNavigateTree::GetItem( Tree, CTAG_VERSION_NUMBER, Branch, Value ) )
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