//******************************************************************************
/// \copyright (c) 2017,2020 by Seidenader Maschinenbau GmbH
/// \file SVAuthUtility.cpp
/// All Rights Reserved
//******************************************************************************
/// Not much to see here.
//******************************************************************************

#pragma region Includes
#include "stdafx.h"

#include "getopt.h"

#include "SVAuthLibrary/AuthManager.h"
#include "Settings.h"
#include "SettingsLoader.h"
#include "SVLogLibrary/Logging.h"
#pragma endregion Includes

void print_help(FILE* fp)
{
	fprintf(fp, "Usage:\n");
	fprintf(fp, "  ./SVAuthUtility <command> <arguments>\n");
	fprintf(fp, "\n");
	fprintf(fp, "The following commands are available:\n");
	fprintf(fp, "\n");
	fprintf(fp, "  * open-web-app\n");
	fprintf(fp, "      Generates a url with all required parameters that are\n");
	fprintf(fp, "      required for opening the WebApp without login.\n");
	fprintf(fp, "      Will check SVOGateway.ini for jwt configuration.\n");
	fprintf(fp, "      Then, Chrome is started with the generated url.\n");
	fprintf(fp, "\n");
	fprintf(fp, "      Parameters:\n");
	fprintf(fp, "        * -u <username>  username that should be used in the token\n");
	fprintf(fp, "        * -h <host>      host name used in the url (can include port)\n");
	fprintf(fp, "        * -p <protocol>  protocol used in the url (defaults to http)\n");
	fprintf(fp, "        * -t <bool>      hideTopNavigation query param (defaults to false)\n");
	fprintf(fp, "        * -b <bool>      hideBottomNavigation query param (defaults to false)\n");
	fprintf(fp, "        * -l <locale>    locale query param (defaults to de)\n");
	fprintf(fp, "\n");
}

static bool generate_url(const SvAuth::WebAppSettings& webAppSettings, const std::string& token, std::string& url)
{
	std::stringstream ss;
	ss << webAppSettings.protocol << "://" << webAppSettings.host << "/hmi-v1"
		<< "?token=" << token
		<< "&hideTopNavigation=" << (webAppSettings.hideTopNavigation ? "true" : "false")
		<< "&hideBottomNavigation=" << (webAppSettings.hideBottomNavigation ? "true" : "false")
		<< "&locale=" << webAppSettings.locale;
	url = ss.str();
	return true;
}

static bool open_chrome(const SvAuth::WebAppSettings& webAppSettings, const std::string& url)
{

	std::stringstream ss;
	ss << "--disable-translate --disable-features=TranslateUI " << url;
	std::string cmd = ss.str();

	STARTUPINFO si;
	ZeroMemory(&si, sizeof(si));
	si.cb = sizeof(STARTUPINFO);
	si.dwFlags = STARTF_USESHOWWINDOW;
	si.wShowWindow = SW_MAXIMIZE;

	PROCESS_INFORMATION pi;
	ZeroMemory(&pi, sizeof(pi));

	if (!CreateProcess(
		webAppSettings.chrome.c_str(),
		const_cast<char*>(cmd.c_str()),
		NULL,
		NULL,
		true,
		0,
		NULL,
		NULL,
		&si,
		&pi
	))
	{
		SV_LOG_GLOBAL(error) << "Error while creating chrome process: " << GetLastError();
		return false;
	}

	CloseHandle(pi.hThread);
	CloseHandle(pi.hProcess);

	return true;
}

static int open_web_app(int argc, char** argv)
{
	SvAuth::Settings settings;
	SvAuth::SettingsLoader loader;
	loader.loadFromIni(settings);

	int c;
	while ((c = getopt(argc, argv, "u:h:p:t:b:l:")) != -1)
	{
		switch (c)
		{
		case 'u':
			settings.webAppSettings.user = optarg;
			break;
		case 'h':
			settings.webAppSettings.host = optarg;
			break;
		case 'p':
			settings.webAppSettings.protocol = optarg;
			break;
		case 't':
			settings.webAppSettings.hideTopNavigation = (strcmp(optarg, "true") == 0);
			break;
		case 'b':
			settings.webAppSettings.hideBottomNavigation = (strcmp(optarg, "true") == 0);
			break;
		case 'l':
			settings.webAppSettings.locale = optarg;
			break;
		default:
			fprintf(stderr, "Unknown command line flag %c\n", c);
			abort();
		}
	}

	SvAuth::AuthManager authManager(settings.authSettings);

	SvAuth::LoginRequest req;
	req.set_username(settings.webAppSettings.user);
	SvAuth::LoginResponse res;
	if (!authManager.loginNoPassword(req, res))
	{
		SV_LOG_GLOBAL(error) << "Failed generating the auth token for user " << settings.webAppSettings.user;
		return 1;
	}

	std::string url;
	if (!generate_url(settings.webAppSettings, res.accesstoken(), url))
	{
		SV_LOG_GLOBAL(error) << "Error while generating WebApp url";
		return 1;
	}
	
	SV_LOG_GLOBAL(debug) << "Generated url: \"" << url << "\"";

	if (!open_chrome(settings.webAppSettings, url))
	{
		SV_LOG_GLOBAL(error) << "Error while opening chrome";
		return 1;
	}

	SV_LOG_GLOBAL(info) << "WebApp opened successfully. Exiting.";

	return 0;
}

int main(int argc, char** argv)
{
	if (argc < 2)
	{
		print_help(stderr);
		return 1;
	}

	const char* command = argv[1];
	if (strcmp(command, "help") == 0)
	{
		print_help(stdout);
		return 0;
	}
	else if (strcmp(command, "open-web-app") == 0)
	{
		return open_web_app(argc - 1, argv + 1);
	}
	else
	{
		fprintf(stderr, "Unknwon command \"%s\"", command);
		print_help(stderr);
		return 1;
	}
}
