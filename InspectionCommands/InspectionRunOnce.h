//*****************************************************************************
// \copyright COPYRIGHT (c) 2015 by Seidenader Maschinenbau GmbH
// All Rights Reserved
//*****************************************************************************

// This is the Command for Executing a RunOnce.
//******************************************************************************

#pragma once

#pragma region Includes
#include "SVUtilityLibrary/SVSharedPtr.h"
#include "ObjectInterfaces\IObjectManager.h"
#include "ObjectInterfaces\ITaskObject.h"
#include "ObjectInterfaces\IInspectionProcess.h"
#pragma endregion Includes

#pragma region Declarations
//! Declaration is in SVUtilityLibrary\SVGUID.h
class SVGUID;
#pragma endregion Declarations

namespace SvCmd
{
	class InspectionRunOnce
	{
	public:
		InspectionRunOnce(const InspectionRunOnce&) = delete;
		InspectionRunOnce& operator=(const InspectionRunOnce&) = delete;

		InspectionRunOnce(const SVGUID& rInspectionID, const SVGUID& rToolID = SVGUID())
			: m_InspectionID(rInspectionID), m_ToolID(rToolID) {};

		virtual ~InspectionRunOnce() {};

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
