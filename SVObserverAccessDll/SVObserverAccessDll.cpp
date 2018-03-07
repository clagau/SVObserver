//*****************************************************************************
/// \copyright (c) 2018,2018 by Seidenader Maschinenbau GmbH
/// \file SVObserverAccessDll.cpp
/// All Rights Reserved 
//*****************************************************************************
///Defines the exported functions for the DLL application.
//******************************************************************************

#include "stdafx.h"
#include "SVObserverAccessDll.h"
#include <SVProtoBuf\envelope.pb.h>
#include "SVSharedMemoryLibrary\ShareControl.h"


namespace SVObserverAccessDll
{

std::unique_ptr<SvSml::ShareControl> ShareControlInstance;

void Init()
{
	ShareControlInstance = std::make_unique<SvSml::ShareControl>();
}

void Finish()
{
	ShareControlInstance.release();
}




bool  QueryListItem(const RRWS::QueryListItemRequest& req, RRWS::QueryListItemResponse& resp, SVRPC::Error& err)
{
	if (ShareControlInstance.get())
	{
		return ShareControlInstance->QueryListItem(req, resp, err);
	}
	else
	{
		return false;
	}
}
bool GetProductOrReject(const RRWS::GetProductRequest& request, RRWS::GetProductResponse& resp, SVRPC::Error& err)
{
	if (ShareControlInstance.get())
	{
		return ShareControlInstance->GetProductOrReject(request, resp, err);
	}
	else
	{
		return false;
	}
}
bool GetFailstatus(const RRWS::GetFailStatusRequest& request, RRWS::GetFailStatusResponse& resp, SVRPC::Error& err)
{

	if (ShareControlInstance.get())
	{
		return ShareControlInstance->GetFailstatus(request, resp, err);
	}
	else
	{
		return false;
	}

}
bool  GetImageFromCurId(const  RRWS::GetImageFromCurIdRequest& req, RRWS::GetImageFromCurIdResponse& resp, SVRPC::Error& err)
{

	if (ShareControlInstance.get())
	{
		return ShareControlInstance->GetImageFromCurId(req, resp, err);
	}
	else
	{
		return false;
	}

}

bool QueryListName(const RRWS::QueryListNameRequest& req, RRWS::QueryListNameResponse& resp, SVRPC::Error& err)
{
	if (ShareControlInstance.get())
	{
		return ShareControlInstance->QueryListName(req, resp, err);
	}
	else
	{
		return false;
	}

}

}




