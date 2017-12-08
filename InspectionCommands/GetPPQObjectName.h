//*****************************************************************************
// \copyright COPYRIGHT (c) 2015 by Seidenader Maschinenbau GmbH
// All Rights Reserved
//*****************************************************************************

// This is the Command for getting the PPQ Object Name from the Inspection.
//******************************************************************************
#pragma once

#pragma region Includes
#include "ObjectInterfaces/IInspectionProcess.h"
#include "ObjectInterfaces/IObjectClass.h"
#include "ObjectInterfaces/IObjectManager.h"
#pragma endregion Includes

#pragma region Declarations
//! Declaration is in SVUtilityLibrary\SVGUID.h
class SVGUID;
#pragma endregion Declarations

namespace SvCmd
{
	struct GetPPQObjectName
	{
	public:
		GetPPQObjectName(const GetPPQObjectName&) = delete;
		GetPPQObjectName& operator=(const GetPPQObjectName&) = delete;

		GetPPQObjectName(const SVGUID& rInspectionID) : m_InspectionID(rInspectionID) {};

		virtual ~GetPPQObjectName() {};

		// This method is where the real separation would occur by using sockets/named pipes/shared memory
		// The logic contained within this method would be moved to the "Server" side of a Client/Server architecture
		// and replaced with the building and sending of the command
		HRESULT Execute()
		{
			HRESULT hr = E_POINTER;

			SvOi::IInspectionProcess* pInspection = dynamic_cast<SvOi::IInspectionProcess *>(SvOi::getObject(m_InspectionID));
			if (pInspection)
			{
				SvOi::IObjectClass* pPPQ = pInspection->GetPPQInterface();
				if (pPPQ)
				{
					m_Name = pPPQ->GetName();
					hr = S_OK;
				}
			}
			return hr;
		}
		bool empty() const { return false; }
		const std::string& GetName() const { return m_Name; }

	private:
		SVGUID m_InspectionID;
		std::string m_Name;
	};
} //namespace SvCmd
