//*****************************************************************************
/// \copyright (c) 2015,2015 by Seidenader Maschinenbau GmbH
/// All Rights Reserved 
//*****************************************************************************
/// This is the data items to be used for the object selector
//******************************************************************************
#pragma once

#pragma region Includes
#include <vector>
#include "ObjectInterfaces/ISelectorItemVector.h"
#include "SelectorItem.h"
#pragma endregion Includes

namespace Seidenader { namespace ObjectSelectorLibrary
{
	class SelectorItemVector : public SvOi::ISelectorItemVector, public std::vector<SelectorItem>
	{
	#pragma region Constructor
	public:
		SelectorItemVector() {};

		virtual ~SelectorItemVector() {};
	#pragma endregion Constructor

	#pragma region Virtual Methods
	public:
		virtual int getSize() const override { return static_cast<int> (size()); };
		virtual SvOi::ISelectorItem* getAt(int Index) override
		{ 
			SvOi::ISelectorItem *pResult( nullptr );
			if( size() > Index ) 
			{
				pResult = static_cast<SvOi::ISelectorItem*> ( &at( Index ));
			}
			return pResult;
		};
	#pragma endregion Virtual Methods
	};

} /* namespace ObjectSelectorLibrary */ } /* namespace Seidenader */

namespace SvOsl = Seidenader::ObjectSelectorLibrary;
