//******************************************************************************
//* COPYRIGHT (c) 2014 by Seidenader Vision Inc., Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVLptIO
//******************************************************************************

#pragma once

#pragma region Includes
//Moved to precompiled header: #include <functional>
#pragma endregion Includes

typedef std::function<void()> IrqHandler;

class SVLptIO
{
#pragma region public
public:
	SVLptIO();
	virtual ~SVLptIO();

	 HRESULT GetDataPort(UCHAR& value) const;
	 HRESULT SetDataPort(UCHAR value);

	 HRESULT GetStatusPort(UCHAR& value) const;

	 HRESULT GetControlPort(UCHAR& value) const;
	 HRESULT SetControlPort(UCHAR value);

	 HRESULT IsBidirectionalSupported(bool& biDirectional) const;

	 HRESULT ActivateReadMode();
	 HRESULT ActivateWriteMode();

	 bool IsActive() const;
	 HRESULT SetActive(bool bActivate);

	 HRESULT DisableAckInterrupt();
	 HRESULT EnableAckInterrupt(const IrqHandler& func);

	 bool isIrqHandlerEnabled() const;

#pragma endregion public

#pragma region private
private:
	void Destroy();
	static void OnLPTInterrupt();
#pragma endregion private
 };

