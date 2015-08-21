//******************************************************************************
//* COPYRIGHT (c) 2008 by Seidenader Vision Inc., Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVXMLClass
//* .File Name       : $Workfile:   SVXMLClass.inl  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.0  $
//* .Check In Date   : $Date:   25 Apr 2013 19:50:20  $
//******************************************************************************

#include <assert.h>

#include "SVLibrary/SVBStr.h"		// SVBStr

#include "SVXMLLibraryGlobals.h"	// SVXMLLibraryGlobals
#include "SVXMLEncryptionClass.h"	// SVXMLEncryptionClass
#include "SVXMLSVRSchemaClass.h"	// SVXMLSVRSchemaClass
#include "SVVariantCustomTypes.h"
#include "SVXMLSafeArrayConverter.h"
#include "SVUtilityLibrary\SVUtilityGlobals.h"

template<typename SVT_TREE>
HRESULT SVXMLClass::CopyTreeToXMLFile (SVT_TREE& rTree, long alSVOCurrentVersion, BSTR abstrFormat, BSTR abstrRevisionHistory, BSTR abstrFileName)
{
	HRESULT	hr = 0;
	unsigned long ulCheckSum;
	SVXML::IXMLDOMElementPtr oDOMRootPtr;

	while (1)
	{
		if (svmlInitialized == 0)
		{
			hr = -1635;
			break;
		}

		hr = CopyTreeToDOM(rTree, alSVOCurrentVersion, abstrFormat, abstrRevisionHistory);
		if( SEV_SUCCESS != SV_SEVERITY( hr ) )
		{
			break;
		}

   	if (svmlUseCheckSums)
		{
			hr = GetRootNode (oDOMRootPtr);
			if( SEV_SUCCESS != SV_SEVERITY( hr ) )
			{
				break;
			}

			hr = CalculateNodeCheckSums (oDOMRootPtr, &ulCheckSum, g_lXMLCheckSumCreate, NULL);

			if( SEV_SUCCESS != SV_SEVERITY( hr ) )
			{
				break;
			}
		}

		hr = CopyDOMToXMLFile (abstrFileName);
		if( SEV_SUCCESS != SV_SEVERITY( hr ) )
		{
			break;
		}
		break;
	}
	return hr;
}

template<typename SVT_TREE>
HRESULT SVXMLClass::CopyXMLFileToTree (SVT_TREE& rTree, long alSVOCurrentVersion, BSTR abstrFilename, BSTR* abstrpRevisionHistory, BSTR* abstrpChangedNode)
{
	HRESULT hr = 0;

	SVXML::IXMLDOMElementPtr oDOMRootPtr;

	unsigned long ulCheckSum = 0;

	while (1)
	{
		hr = CopyXMLFileToDOM (abstrFilename, abstrpRevisionHistory);
		if( SEV_SUCCESS != SV_SEVERITY( hr ) )
		{
			break;
		}

	   	if (svmlUseCheckSums)
		{
			hr = GetRootNode (oDOMRootPtr);
			if( SEV_SUCCESS != SV_SEVERITY( hr ) )
			{
				break;
			}

			hr = CalculateNodeCheckSums (oDOMRootPtr, 
										&ulCheckSum, 
									    g_lXMLCheckSumCompare,// mode; compare
										abstrpChangedNode);			

			if (hr == 4)
			{
//-			Data was changed!
//-			We will leave hr set to 1.  If there are no other errors, this 
//-			value will be returned.
				// BRW - hr is not set to 1, it's 4. ???
			}
			else
			if( SEV_SUCCESS != SV_SEVERITY( hr ) )
			{
				break;
			}
		}

   		hr = CopyDOMToTree (rTree, alSVOCurrentVersion);
		if( SEV_SUCCESS != SV_SEVERITY( hr ) )
		{
			break;
		}
		break;
	} // while (1)
	return hr;
}

template<typename SVT_TREE>
HRESULT SVXMLClass::CopyTreeToDOM (SVT_TREE& rTree, long alSVOCurrentVersion, BSTR abstrFormat, BSTR abstrRevisionHistory)
{
	HRESULT hr = 0;

	typename SVT_TREE::SVBranchHandle l_TreeRootHandle;

	SVXML::IXMLDOMElementPtr oDOMRootPtr;
	SVXML::IXMLDOMElementPtr oDOMDestinationBranchPtr;

	SVBStr bstrNameSpace;

	oDOMRootPtr = NULL;

	while (1)
	{
		if (svmlInitialized == 0)
		{
			hr = -1636;
			break;
		}

//-   This clear will do DOM clear (which sets header) and set the schema.
		hr = Clear ();
		if( SEV_SUCCESS != SV_SEVERITY( hr ) )
		{
			break;
		}

		bstrNameSpace = (BSTR) ((_bstr_t) "x-schema:#" + (_bstr_t) g_csSVR00001SchemaName);

		svmopDOM->SetNameSpace (bstrNameSpace);

//-	The root, which should have been created in Clear (), must already 
//-	exist prior to calling SetRevisionHistory () or AddToRevisionHistory ().
		if (svmlUseRevisionHistoryInfo)
		{
			hr = SetRevisionHistory (abstrRevisionHistory);
			if( SEV_SUCCESS != SV_SEVERITY( hr ) )
			{
				break;
			}

			hr = AddToRevisionHistory (alSVOCurrentVersion, abstrFormat);
			if( SEV_SUCCESS != SV_SEVERITY( hr ) )
			{
				break;
			}
		}

		if (m_lUseEncryption)
		{
			hr = m_opEncryption->SetEncryption ();
			if( SEV_SUCCESS != SV_SEVERITY( hr ) )
			{
				break;
			}
			else
			if (hr == 3)
			{
//-			Encryption branch already existed.
				hr = -1897;
				break;
			}

//-		This is just to emphisize that this value in these two classes must
//-		be the same (shadowed data).
			hr = m_opEncryption->GetIsEncrypted (&m_lIsEncrypted);
			if( SEV_SUCCESS != SV_SEVERITY( hr ) )
			{
				break;
			}

//-		Here is where we set the encryption method to 1.
			hr = m_opEncryption->SetEncryptionMethod (1);
			if( SEV_SUCCESS != SV_SEVERITY( hr ) )
			{
				break;
			}
		}

		hr = rTree.GetRoot( l_TreeRootHandle );
		if( SEV_SUCCESS != SV_SEVERITY( hr ) )
		{
			break;
		}

		hr = GetRootNode( oDOMRootPtr );
		if( SEV_SUCCESS != SV_SEVERITY( hr ) )
		{
			break;
		}

//-	There should be a root. It should have been created in Clear ().
		if (oDOMRootPtr == NULL)
		{
			hr = -1695;
			break;
		}

 		hr = CopyTreeNodeToDOMNode( rTree, l_TreeRootHandle, oDOMDestinationBranchPtr );
		if( SEV_SUCCESS != SV_SEVERITY( hr ) )
		{
			break;
		}

		hr = AppendChildToDOMNode( oDOMRootPtr, oDOMDestinationBranchPtr );
		if( SEV_SUCCESS != SV_SEVERITY( hr ) )
		{
			break;
		}

		hr = svmopDOM->ClearNameSpace ();
		if( SEV_SUCCESS != SV_SEVERITY( hr ) )
		{
			break;
		}
		break;
	}
	return hr;
}

template<typename SVT_TREE>
HRESULT SVXMLClass::CopyDOMToTree (SVT_TREE& rTree, long alSVOAppVersionFromConfig)
{
	HRESULT hr = 0;

	SVT_TREE::SVBranchHandle l_TreeRootHandle;

	SVXML::IXMLDOMElementPtr oDOMBasePtr;
	SVXML::IXMLDOMElementPtr oDOMDestinationBranchPtr;

	SVBStr bstrNameSpace;

	oDOMBasePtr = NULL;

	while (1)
	{
		if (svmlInitialized == 0)
		{
			hr = -1749;
			break;
		}

		hr = rTree.Clear();
		if( SEV_SUCCESS != SV_SEVERITY( hr ) )
		{
			break;
		}

		hr = GetBaseElement( oDOMBasePtr );
		if( SEV_SUCCESS != SV_SEVERITY( hr ) )
		{
			break;
		}

		if (oDOMBasePtr == NULL)
		{
			hr = -1753;
			break;
		}

		hr = rTree.GetRoot( l_TreeRootHandle );
		if( SEV_SUCCESS != SV_SEVERITY( hr ) )
		{
			break;
		}

//-	The second parameter would normally be the parent, but it is assumed 
//-	that the root node does not have a parent.
		hr = CreateTreeChildNodeFromDOMNode( oDOMBasePtr, rTree, l_TreeRootHandle );
		if( SEV_SUCCESS != SV_SEVERITY( hr ) )
		{
			break;
		}
		break;
	}
	return hr;
}

template<typename SVT_TREE>
HRESULT	SVXMLClass::CopyTreeNodeToDOMNode( SVT_TREE& rTree, const typename SVT_TREE::SVBranchHandle& alTreeNodeHandle, SVXML::IXMLDOMElementPtr& arDOMParentNodePtr )
{
	HRESULT	l_Status( S_OK );

	SVXML::IXMLDOMElementPtr oDOMNewParentElementPtr;

	if( svmlInitialized == 0 )
	{
		l_Status = -1637;
	}

	if( l_Status == S_OK )
	{
		_bstr_t bstrTreeNodeName;

		l_Status = rTree.GetBranchName( alTreeNodeHandle, bstrTreeNodeName.GetBSTR() );

		if( l_Status == S_OK )
		{
			l_Status = rTree.IsRoot( alTreeNodeHandle );

			if( SUCCEEDED( l_Status ) )
			{
				if( l_Status == S_OK )
				{
					l_Status = CreateDOMNode( oDOMNewParentElementPtr, bstrTreeNodeName, VT_SVBASENODE );
				}
				else
				{
					l_Status = CreateDOMNode( oDOMNewParentElementPtr, bstrTreeNodeName ); 
				}

				if( l_Status == S_OK )
				{
					l_Status = AppendCarriageReturnToDOMNode( oDOMNewParentElementPtr );
				}
			}
		}
	}

	if( l_Status == S_OK )
	{
		l_Status = rTree.DoesBranchHaveLeaves( alTreeNodeHandle );

		if( SUCCEEDED( l_Status ) )
		{
			if( l_Status == S_OK )
			{
				SVT_TREE::SVLeafHandle l_Leaf;

				l_Status = rTree.GetFirstLeaf( alTreeNodeHandle, l_Leaf );

				while( l_Status == S_OK )
				{
					SVXML::IXMLDOMElementPtr oDOMNewChildElementPtr;

					l_Status = CopyTreeDataToDOMData( rTree, l_Leaf, oDOMNewChildElementPtr );

					if( l_Status == S_OK )
					{
						l_Status = AppendChildToDOMNode( oDOMNewParentElementPtr, oDOMNewChildElementPtr );

						if( l_Status == S_OK )
						{
							l_Status = AppendCarriageReturnToDOMNode( oDOMNewParentElementPtr );
						}
					}

					if( l_Status == S_OK )
					{
						l_Status = rTree.GetNextLeaf( alTreeNodeHandle, l_Leaf );
					}
				}

				if( SUCCEEDED( l_Status ) )
				{
					l_Status = S_OK;
				}
			}
			else
			{
				l_Status = S_OK;
			}
		}
	}

	if( l_Status == S_OK )
	{
		l_Status = rTree.DoesBranchHaveBranches( alTreeNodeHandle );

		if( SUCCEEDED( l_Status ) )
		{
			if( l_Status == S_OK )
			{
				SVT_TREE::SVBranchHandle l_Child;
				SVT_TREE::SVBranchHandle l_Parent( alTreeNodeHandle );

				l_Status = rTree.GetFirstBranch( l_Parent, l_Child );

				while( l_Status == S_OK )
				{
					SVXML::IXMLDOMElementPtr oDOMNewChildElementPtr;

					l_Status = CopyTreeNodeToDOMNode( rTree, l_Child, oDOMNewChildElementPtr );

					if( l_Status == S_OK )
					{
						l_Status = AppendChildToDOMNode( oDOMNewParentElementPtr, oDOMNewChildElementPtr );

						if( l_Status == S_OK )
						{
							l_Status = AppendCarriageReturnToDOMNode( oDOMNewParentElementPtr );
						}
					}

					if( l_Status == S_OK )
					{
						l_Status = rTree.GetNextBranch( l_Parent, l_Child );
					}
				}

				if( SUCCEEDED( l_Status ) )
				{
					l_Status = S_OK;
				}
			}
			else
			{
				l_Status = S_OK;
			}
		}
	}

	if( l_Status == S_OK )
	{
		arDOMParentNodePtr = oDOMNewParentElementPtr;
	}

	return l_Status;
}

template<typename SVT_TREE>
HRESULT SVXMLClass::CopyTreeDataToDOMData (SVT_TREE& rTree, const typename SVT_TREE::SVLeafHandle& alNodeHandle, SVXML::IXMLDOMElementPtr& arDOMDataPtr)
{
	HRESULT hr = 0;

	VARIANT vTreeNodeData;

	BSTR bstrTreeNodeName = NULL;

	SVXML::IXMLDOMElementPtr oDOMDataPtr;

	::VariantInit (&vTreeNodeData);

	while (1)
	{
		hr = rTree.GetLeafName( alNodeHandle, bstrTreeNodeName );
		if( SEV_SUCCESS != SV_SEVERITY( hr ) )
		{
			break;
		}

		hr = rTree.GetLeafData( alNodeHandle, vTreeNodeData );
		if( SEV_SUCCESS != SV_SEVERITY( hr ) )
		{
			break;
		}

//-	If the data type is a safe array type, then the safe array will be 
//-   treated as a "node" element with each of the individual elements being 
//-	considered "data" elements.
		if (vTreeNodeData.vt & VT_ARRAY)
		{
//-		oDOMDataPtr will come back as a node element; not a data 
//-		element.
			hr = SVXMLSafeArrayConverter::CopySafeArrayToDOMNode( *this, bstrTreeNodeName, vTreeNodeData.parray, oDOMDataPtr );
		}
		else
		{
			hr = CreateDOMData( oDOMDataPtr, bstrTreeNodeName, &vTreeNodeData );
		}

		if( SEV_SUCCESS != SV_SEVERITY( hr ) )
		{
			break;
		}
		break;
	}

	SysFreeString( bstrTreeNodeName );
	VariantClear( &vTreeNodeData );

	if( SEV_SUCCESS != SV_SEVERITY( hr ) )
	{
	}
	else
	{
		arDOMDataPtr = oDOMDataPtr;
	}
	return hr;
}

template<typename SVT_TREE>
HRESULT SVXMLClass::CreateTreeChildNodeFromDOMNode( SVXML::IXMLDOMElementPtr aDOMElementPtr, SVT_TREE& rTree, const typename SVT_TREE::SVBranchHandle& alTreeParentNodeHandle )
{
	HRESULT	l_Status( S_OK );

	if( aDOMElementPtr == NULL )
	{
		//-		No node.  Leave peacefully.
		return 2;
	}

	if( svmlInitialized == 0 )
	{
		l_Status = -1765;
	}

	if( l_Status == S_OK )
	{
		VARIANT vDOMElementData;

		BSTR bstrDOMElementName( NULL );

		::VariantInit( &vDOMElementData );

		//-   GetElementName () is executed first for debugging, so that the name of
		//-   the items being manipulated can be seen.
		l_Status = GetElementName( aDOMElementPtr, &bstrDOMElementName );

		if( SUCCEEDED( l_Status ) )
		{
			//-   For NODE elements with type (no data), vDOMElementData will return with
			//-	the type set and the value set to zero (0). 
			//-   For Node elements without type, vDOMElementData will return with type
			//-	set to VT_NULL, and the value set to zero (0).
			l_Status = GetElementData( aDOMElementPtr, &vDOMElementData );

			assert( SUCCEEDED( l_Status ) );
		}

		if( SUCCEEDED( l_Status ) )
		{
			BSTR bstrDOMElementTag( NULL );

			l_Status = IsElementData( aDOMElementPtr, &bstrDOMElementTag );

			if( SUCCEEDED( l_Status ) )
			{
				if( l_Status == S_OK )
				{
					l_Status = rTree.CreateLeaf( alTreeParentNodeHandle, bstrDOMElementName, vDOMElementData );
				}
				else
				{
					if( ( vDOMElementData.vt & VT_ARRAY ) == VT_ARRAY && vDOMElementData.vt != VT_ARRAY )
					{
						l_Status = rTree.CreateLeaf( alTreeParentNodeHandle, bstrDOMElementName, vDOMElementData );
					}
					else
					{
						SVXML::IXMLDOMElementPtr oDOMChildElementPtr;

						SVT_TREE::SVBranchHandle l_Branch;

						if( vDOMElementData.vt == VT_SVBASENODE )
						{
							l_Status = rTree.GetRoot( l_Branch );
						}
						else
						{
							l_Status = rTree.CreateBranch( alTreeParentNodeHandle, bstrDOMElementName, &l_Branch );
						}

						oDOMChildElementPtr = GetFirstElementChild(aDOMElementPtr);

						while( SUCCEEDED( l_Status ) && oDOMChildElementPtr != NULL )
						{
							l_Status = CreateTreeChildNodeFromDOMNode( oDOMChildElementPtr, rTree, l_Branch );

							if( SUCCEEDED( l_Status ) )
							{
								oDOMChildElementPtr = GetNextElementSibling(oDOMChildElementPtr);
							}
						} // while (oDOMChildElementPtr != NULL)
					}
				}
			}

			::SysFreeString( bstrDOMElementTag );
			bstrDOMElementTag = NULL;
		}

		::VariantClear( &vDOMElementData );

		::SysFreeString( bstrDOMElementName );
		bstrDOMElementName = NULL;
	}
	return l_Status;
}

//******************************************************************************
//* LOG HISTORY:
//******************************************************************************
/*
$Log:   N:\PVCSarch65\ProjectFiles\archives\SVObserver_src\SVXMLLibrary\SVXMLClass.inl_v  $
 * 
 *    Rev 1.0   25 Apr 2013 19:50:20   bWalter
 * Project:  SVObserver
 * Change Request (SCR) nbr:  814
 * SCR Title:  Upgrade SVObserver to Compile Using Visual Studio 2010
 * Checked in by:  bWalter;  Ben Walter
 * Change Description:  
 *   Initial check in to SVObserver_src.  (Merged with svo_src label SVO 6.10 Beta 008.)
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.2   11 Oct 2012 10:40:08   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  602
 * SCR Title:  Revise the Toolset Parsing and Object Creation Methodology
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated code and projects to fix whitespace issues in the XML file and consolidate functionality for load and saving to this library.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.1   28 Sep 2012 14:22:40   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  602
 * SCR Title:  Revise the Toolset Parsing and Object Creation Methodology
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated class to manage the perserve whitespace functionality and removed check for attached tree in copy method that does not require the tree.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.0   18 Sep 2012 18:12:44   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  602
 * SCR Title:  Revise the Toolset Parsing and Object Creation Methodology
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated code to use the templated XML objects that moved from the SVLibrary to the new XML Library.
 * 
 * This code is the refacting of the items from the SVLibrary.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
*/