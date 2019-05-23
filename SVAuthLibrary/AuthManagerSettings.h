//******************************************************************************
/// \copyright (c) 2017,2018 by Seidenader Maschinenbau GmbH
/// \file AuthManagerSettings.h
/// All Rights Reserved
//******************************************************************************
/// Global settings for the AuthManager
//******************************************************************************

#pragma once


namespace SvAuth
{

struct AuthManagerSettings
{
	/// Full path to the xml user database file. This is based on the format
	/// of the HMI user database with one little exception: we use a different
	/// function hashing the user password (because we don't know the original
	/// algorithm).
	std::string UserSettingsFile {"C:\\SVObserver\\UserSettings.xml"};

	/// This is the secret that is used to generate HMAC signed JSON Web Token.
	/// It it is a shared secret between all parties that are allowed to issue
	/// tokens for a particular use case. In our case SVOGateway and HMI share
	/// this secret so both can issue access tokens.
	std::string JwtHmacSecret;

	/// Public key that can be used to verify the signature of JSON Web Token
	/// using RSA algorithm. Only parties verifying the token was really
	/// generated by the expected parties need this public key so they can
	/// verify the signature. The server needs it for the sole purpose of
	/// providing it via the /.well-known/jwks.json endpoint.
	/// The format of the public key must be in the standard RSA format including
	/// header `-----BEGIN PUBLIC KEY-----` and according footer.
	std::string JwtRsaPublicKeyFile;

	/// Private key that is used by the SVOGateway to sign RSA JSON Web Token.
	/// This key must be kept secret and must not be shared with anybody to
	/// avoid other parties issuing valid tokens.
	/// The format of the public key must be in the standard RSA format including
	/// header `-----BEGIN RSA PRIVATE KEY-----` and according footer.
	std::string JwtRsaPrivateKeyFile;

	/// Configure whether clients are allowed to connect using an empty auth
	/// token. This setting must be handled with care, because if it is easy
	/// for the user to change it, he can gain access to all the data without
	/// being authorized.
	bool AllowUnauthorizedRpcClients {false};

	/// Defines how long a generated access token is valid, by defining its
	/// exp field (a.k.a expiration) as now plus configured validity.
	/// The access token should not be valid for too long as it is used during
	/// login phase only. The user will receive an auth token for on successful
	/// login which should be valid for longer.
	/// Default value is 1 hour which should be enough to finish the login.
	int JwtAccessTokenValidityInSeconds {60 * 60};

	/// Defines how long a generated auth token is valid, by defining its
	/// exp field (a.k.a expiration) as now plus configured validity.
	/// As long as we do not have a refresh token, the auth token should be
	/// valid for as long as you expect the user to use the website before
	/// having to login again.
	/// Default value is 10 years which is common practice for refresh token.
	int JwtAuthTokenValidityInSeconds {60 * 60 * 24 * 30 * 12 * 10};
};

} // namespace SvAuth
