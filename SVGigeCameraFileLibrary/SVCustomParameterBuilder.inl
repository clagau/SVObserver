//******************************************************************************
//* COPYRIGHT (c) 2013 by Seidenader Vision, Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVCustomParameterBuilder
//* .File Name       : $Workfile:   SVCustomParameterBuilder.inl  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.1  $
//* .Check In Date   : $Date:   13 May 2013 11:04:18  $
//******************************************************************************

#pragma region Includes
#include "CameraLibrary/SVCustomDeviceParam.h"
#include "CameraLibrary/SVDeviceParam.h"
#pragma endregion Includes

template<typename Insertor>
void SVCustomParameterBuilder::BuildCustomDeviceParams(const SVMaterialsTree::SVTreeContainer& rTree, Insertor insertor)
{
	for (SVMaterialsTree::const_iterator it = rTree.begin();it!= rTree.end();++it)
	{
		std::shared_ptr<SVCustomDeviceParam> pParam(SVCustomParameterBuilder::BuildCustomDeviceParam( *it.node() ));
		if (pParam.get())
		{
			// Add to list (makes a copy)
			insertor = std::make_pair(pParam->Type(), SVDeviceParamWrapper(*pParam));
		}
	}
}

