//*****************************************************************************
// \copyright COPYRIGHT (c) 2015 by Seidenader Maschinenbau GmbH
// All Rights Reserved
//*****************************************************************************

// This is the Command for Executing a RunOnce.
//******************************************************************************

#pragma once

#pragma region Includes
//Moved to precompiled header: #include <boost/noncopyable.hpp>
#include "SVUtilityLibrary/SVGUID.h"
#include "SVUtilityLibrary/SVSharedPtr.h"
#include "ObjectInterfaces\IObjectManager.h"
#include "ObjectInterfaces\ITaskObject.h"
#include "ObjectInterfaces\IInspectionProcess.h"
#pragma endregion Includes

namespace SvCmd
{
	class InspectionRunOnce : public boost::noncopyable
	{
	public:
		InspectionRunOnce(const SVGUID& rInspectionID, const SVGUID& rToolID = SVGUID())
		: m_InspectionID(rInspectionID), m_ToolID(rToolID) {}

		virtual ~InspectionRunOnce() {}

		HRESULT Execute()
		{
			HRESULT hr = S_OK;

			SvOi::IInspectionProcess* pInspection = dynamic_cast<SvOi::IInspectionProcess *>(SvOi::getObject(m_InspectionID));
			if (nullptr != pInspection)
			{
				SvOi::ITaskObject* pTool(nullptr);
				if (GUID_NULL != m_ToolID)
				{
					pTool = dynamic_cast<SvOi::ITaskObject *>(SvOi::getObject(m_ToolID));
				}
				hr = pInspection->RunOnce(pTool);

				// Get additional Error Info if the run once was for the tool
				if (pTool && (SUCCEEDED(hr) || E_FAIL == hr))
				{
					HRESULT	hrTemp = pTool->getFirstTaskMessage().getMessage().m_MessageCode;
					if (!SUCCEEDED (hrTemp))
					{
						hr = hrTemp; // overwrite hr only if hrTemp contains error information.
					}
				}
			}
			else
			{
				hr = E_POINTER;
			}
			return hr;
		}

		bool empty() const
		{
			bool bEmpty = m_InspectionID.empty();
			//bEmpty = bEmpty && m_ToolID.empty(); //m_ToolID is optional
			return bEmpty;
		}

	private:
		SVGUID m_InspectionID;
		SVGUID m_ToolID;
	};
	typedef SVSharedPtr<InspectionRunOnce> InspectionRunOncePtr;

} //namespace SvCmd
