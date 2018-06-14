//*****************************************************************************
/// \copyright (c) 2018,2018 by Igor Pavlov
/// \file IPassword.h
/// All Rights Reserved 
/// Author           : Igor Pavlov
/// License          : Public Domain License
/// From             : http://www.7-zip.de/sdk.html
//*****************************************************************************
/// 7z password interface
//******************************************************************************

#pragma once
#include "7zTypes.h"
#include "IDecl.h"

#define PASSWORD_INTERFACE(i, x) DECL_INTERFACE(i, 5, x)

PASSWORD_INTERFACE(ICryptoGetTextPassword, 0x10)
{
  STDMETHOD(CryptoGetTextPassword)(BSTR *password) PURE;
};

PASSWORD_INTERFACE(ICryptoGetTextPassword2, 0x11)
{
  STDMETHOD(CryptoGetTextPassword2)(Int32 *passwordIsDefined, BSTR *password) PURE;
};
