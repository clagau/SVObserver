//******************************************************************************
/// COPYRIGHT (c) 2015 by Seidenader Maschinenbau GmbH
/// All Rights Reserved
//******************************************************************************
/// Interface class for tool set class
//******************************************************************************

#pragma once
#include "ITaskObjectListClass.h"

namespace Seidenader
{
	namespace ObjectInterfaces
	{	
		//this class is a interface. It should only have pure virtual public method and new member parameter
		class IToolSet : virtual public ITaskObjectListClass
		{
		public:
			/**********
			 The method gets ?.
			 \param rToolID <out>: tool id
			***********/
			virtual bool IsToolPreviousToSelected( const SVGUID& rToolID ) const = 0;
		};
	}
}

namespace SvOi = Seidenader::ObjectInterfaces;