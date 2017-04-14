/* Author : Jerome ODIER
 * Email : jerome.odier@lpsc.in2p3.fr
 */

/*-------------------------------------------------------------------------*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <iomanip>

#include <iostream>

#include <fstream>
#include <sstream>

#include "internal.h"

/*-------------------------------------------------------------------------*/

using namespace std;

/*-------------------------------------------------------------------------*/

bool cami_create_context(cami_ctx_t *ctx, const string &host, const string &path, uint16_t port)
{
	/*-----------------------------------------------------------------*/

	ctx->host = host;
	ctx->path = path;

	/*-----------------------------------------------------------------*/

	struct ssl_ctx_st *ssl_ctx = cami_openssl_create_context();

	if(ssl_ctx == NULL)
	{
		return false;
	}

	/*-----------------------------------------------------------------*/

	if(cami_openssl_connect(&ctx->client_ctx, ssl_ctx, host.c_str(), port) < 0)
	{
		cami_openssl_destroy_context(ssl_ctx);

		return false;
	}

	/*-----------------------------------------------------------------*/

	return true;
}

/*-------------------------------------------------------------------------*/

bool cami_destroy_context(cami_ctx_t *ctx)
{
	/*-----------------------------------------------------------------*/

	if(cami_openssl_close(&ctx->client_ctx) < 0)
	{
		return false;
	}

	/*-----------------------------------------------------------------*/

	cami_openssl_destroy_context(ctx->client_ctx.ssl_ctx);

	/*-----------------------------------------------------------------*/

	return true;
}

/*-------------------------------------------------------------------------*/

static ssize_t _readline(cami_client_ctx_t *client_ctx, char *buffer, size_t size)
{
	register char *p = buffer;

	char c;

	for(;;)
	{
		if(size == 0 || cami_openssl_read(client_ctx, &c, 1) != 1)
		{
			return -1;
		}

		/**/

		if(c == '\r')
		{
			continue;
		}

		if(c == '\n')
		{
			*p = '\0';

			return (ssize_t) p - (ssize_t) buffer;
		}

		/**/

		*p++ = c;

		size--;
	}
}

/*-------------------------------------------------------------------------*/

static string _replaceAll(const string &str, const string &from, const string &to)
{
	string result = string(str);

	if(from.empty())
	{
		return result;
	}

	size_t start_pos = 0;

	while((start_pos = result.find(from, start_pos)) != string::npos)
	{
		result.replace(start_pos, from.length(), to);

		start_pos += to.length();
	}

	return result;
}

/*-------------------------------------------------------------------------*/

static string _urlEncode(const string &value)
{
	ostringstream escaped;
	escaped.fill('0');
	escaped << hex;

	for(string::const_iterator i = value.begin(), n = value.end(); i != n; i++)
	{
		string::value_type c = *i;

		if(isalnum(c) || c == '-' || c == '_' || c == '.' || c == '~')
		{
			escaped << c;

			continue;
        	}

        	escaped << uppercase;
        	escaped << '%' << setw(2) << int((unsigned char) c);
        	escaped << nouppercase;
	}

	return escaped.str();
}

/*-------------------------------------------------------------------------*/

#if defined(_WIN32) || defined(_WIN64)
#  define COOKIE_FILE_NAME "c:\\ami.cookie"
#else
#  define COOKIE_FILE_NAME "/tmp/ami.cookie"
#endif

/*-------------------------------------------------------------------------*/

static string _cookie(string result = "")
{
	if(result.empty())
	{
		ifstream f;

		f.open(COOKIE_FILE_NAME, ifstream::in);
		if(f.is_open()) getline(f, result);
		f.close();
	}
	else
	{
		ofstream f;

		f.open(COOKIE_FILE_NAME, ofstream::out);
		if(f.is_open()) f << result;
		f.close();
	}

	return result;
}

/*-------------------------------------------------------------------------*/

string cami_execute(cami_ctx_t *ctx, const string &command, const map<string, string> &arguments)
{
	ssize_t size;

	/*-----------------------------------------------------------------*/
	/* BUILD REQUEST BODY                                              */
	/*-----------------------------------------------------------------*/

	ostringstream tmp1;

	for(map<string, string>::const_iterator i = arguments.begin(), n = arguments.end(); i != n; i++)
	{
		ostringstream tmp2;

		tmp2 << " -"
		     << i->first
		     << "="
		     << "\""
		     << _replaceAll(i->second, "\"", "\\\"")
		     << "\""
		;

		tmp1 << _urlEncode(tmp2.str());
	}

	/*-----------------------------------------------------------------*/

	string body = "Command=" + command + tmp1.str() + "&Converter=AMIXmlToJson.xsl";

	/*-----------------------------------------------------------------*/
	/* BUILD REQUEST HEADER                                            */
	/*-----------------------------------------------------------------*/

	ostringstream tmp3;

	tmp3 << "POST " << ctx->path << " HTTP/1.0\r\n"
	     << "Host: " << ctx->host << "\r\n"
	     << "Content-Type: application/x-www-form-urlencoded; charset=UTF\r\n"
	     << "Content-Length: " << body.size() << "\r\n"
	     << "Cookie: " << _cookie() << "\r\n"
	     << "Connection: Close\r\n"
	     << "User-Agent: ami++\r\n"
	     << "\r\n"
	     << body
	;

	string header = tmp3.str();

	/*-----------------------------------------------------------------*/
	/* SEND REQUEST HEADER                                             */
	/*-----------------------------------------------------------------*/

	size = cami_openssl_write(&ctx->client_ctx, header.c_str(), header.size());

	if(size < 0)
	{
		cami_openssl_show_error();
		throw logic_error("could send request");
	}

	/*-----------------------------------------------------------------*/
	/* READ RESPONSE HEADER                                            */
	/*-----------------------------------------------------------------*/

	char buffer[8192];

	for(;;)
	{
		/*---------------------------------------------------------*/

		size = _readline(&ctx->client_ctx, buffer, 8192);

		if(size < 0)
		{
			cami_openssl_show_error();
			throw logic_error("could receive response");
		}

		if(size == 0)
		{
			break;
		}

		/*---------------------------------------------------------*/

		if(strncasecmp(buffer, "Set-Cookie: ", 12) == 0)
		{
			_cookie(buffer + 12);
		}

		/*---------------------------------------------------------*/
	}

	/*-----------------------------------------------------------------*/
	/* READ RESPONSE BODY                                              */
	/*-----------------------------------------------------------------*/

	ostringstream tmp4;

	ssize_t n;

	for(;;)
	{
		/*---------------------------------------------------------*/

		n = cami_openssl_read(&ctx->client_ctx, buffer, 4096);

		if(n < 0)
		{
			cami_openssl_show_error();
			throw logic_error("could receive response");
		}

		if(n == 0)
		{
			break;
		}

		buffer[n] = '\0';

		/*---------------------------------------------------------*/

		tmp4 << buffer;

		/*---------------------------------------------------------*/
	}

	/*-----------------------------------------------------------------*/

	return tmp4.str();
}

/*-------------------------------------------------------------------------*/
