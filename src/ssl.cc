/* Author : Jerome ODIER
 * Email : jerome.odier@lpsc.in2p3.fr
 */

/*-------------------------------------------------------------------------*/

#if !defined(_WIN32) && !defined(_WIN64)
  #include <netdb.h>
  #include <unistd.h>

  #include <arpa/inet.h>
#else
  #include <winsock2.h>
#endif

#include <openssl/ssl.h>
#include <openssl/err.h>

#include "internal.h"

/*-------------------------------------------------------------------------*/

static void _openssl_init(void)
{
	SSL_library_init();

	SSL_load_error_strings();
}

/*-------------------------------------------------------------------------*/

static int _create_socket(const char *host, uint16_t port)
{
	int ret;

	/*-----------------------------------------------------------------*/
	/* RESOLVE HOST NAME                                               */
	/*-----------------------------------------------------------------*/

	IN_ADDR in_addr;

	struct hostent *_host = gethostbyname(host);

	if(_host != NULL)
	{
		in_addr = *(IN_ADDR *) _host->h_addr;
	}
	else
	{
		in_addr.s_addr = inet_addr(host);

		if(in_addr.s_addr == INADDR_NONE)
		{
			return -1;
		}
	}

	/*-----------------------------------------------------------------*/
	/* CREATE SOCKET                                                   */
	/*-----------------------------------------------------------------*/

	int sock = socket(AF_INET, SOCK_STREAM, 0);

	if(sock < 0)
	{
		return sock;
	}

	/*-----------------------------------------------------------------*/

	SOCKADDR_IN sockaddr = {0};

	sockaddr.sin_family = AF_INET;
	sockaddr.sin_port = htons(port);
	sockaddr.sin_addr = in_addr;

	/*-----------------------------------------------------------------*/
	/* CONNECT SOCKET                                                  */
	/*-----------------------------------------------------------------*/

	ret = connect(sock, (SOCKADDR *) &sockaddr, sizeof(sockaddr));

	if(ret < 0)
	{
		closesocket(sock);

		return ret;
	}

	/*-----------------------------------------------------------------*/

	return sock;
}

/*-------------------------------------------------------------------------*/

static int _always_true_callback(X509_STORE_CTX *ctx, void *arg)
{
	return 1;
}

/*-------------------------------------------------------------------------*/

SSL_CTX *cami_openssl_create_context(void)
{
	/*-----------------------------------------------------------------*/

	const SSL_METHOD *method = SSLv23_client_method();

	if(method == NULL)
	{
		return NULL;
	}

	/*-----------------------------------------------------------------*/

	SSL_CTX *result = SSL_CTX_new(method);

	if(result == NULL)
	{
		return NULL;
	}

	/*-----------------------------------------------------------------*/

	SSL_CTX_set_cert_verify_callback(result, _always_true_callback, NULL);

	/*-----------------------------------------------------------------*/

	return result;
}

/*-------------------------------------------------------------------------*/

void cami_openssl_destroy_context(SSL_CTX *ssl_ctx)
{
	SSL_CTX_free(ssl_ctx);
}

/*-------------------------------------------------------------------------*/

int cami_openssl_connect(cami_client_ctx_t *client_ctx, SSL_CTX *ssl_ctx, const char *host, uint16_t port)
{
	/*-----------------------------------------------------------------*/

	int sock = _create_socket(host, port);

	if(sock < 0)
	{
		return sock;
	}

	/*-----------------------------------------------------------------*/

	SSL *ssl = SSL_new(ssl_ctx);

	if(ssl == NULL)
	{
		return -1;
	}

	/*-----------------------------------------------------------------*/

	if(SSL_set_fd(ssl, sock) == 0)
	{
		SSL_free(ssl);

		closesocket(sock);

		return -1;
	}

	/*-----------------------------------------------------------------*/

	if(SSL_connect(ssl) < 0)
	{
		SSL_free(ssl);

		closesocket(sock);

		return -1;
	}

	/*-----------------------------------------------------------------*/

	client_ctx->ssl_ctx = ssl_ctx;
	client_ctx->ssl     = ssl    ;
	client_ctx->sock    = sock   ;

	/*-----------------------------------------------------------------*/

	return 0;
}

/*-------------------------------------------------------------------------*/

int cami_openssl_close(cami_client_ctx_t *client_ctx)
{
	/*-----------------------------------------------------------------*/

	SSL_shutdown(client_ctx->ssl);
 
	SSL_free(client_ctx->ssl);

	/*-----------------------------------------------------------------*/

	return closesocket(client_ctx->sock);

	/*-----------------------------------------------------------------*/
}

/*-------------------------------------------------------------------------*/

ssize_t cami_openssl_read(cami_client_ctx_t *client_ctx, buff_t buff, size_t size)
{
	return SSL_read(client_ctx->ssl, buff, size);
}

/*-------------------------------------------------------------------------*/

ssize_t cami_openssl_write(cami_client_ctx_t *client_ctx, BUFF_t buff, size_t size)
{
	return SSL_write(client_ctx->ssl, buff, size);
}

/*-------------------------------------------------------------------------*/

void cami_openssl_show_error(void)
{
	ERR_print_errors_fp(stderr);
}

/*-------------------------------------------------------------------------*/
/*-------------------------------------------------------------------------*/

__attribute__((constructor))
static void C(void)
{
#if defined(_WIN32) || defined(_WIN64)
	WSADATA wsadata;

	int err = WSAStartup(MAKEWORD(2, 2), &wsadata);

	if(err < 0)
	{
		/* TODO */
	}
#endif
	_openssl_init();
}
/*-------------------------------------------------------------------------*/
/*-------------------------------------------------------------------------*/

__attribute__((destructor))
static void D(void)
{
#if defined(_WIN32) || defined(_WIN64)
	WSACleanup();
#endif
}

/*-------------------------------------------------------------------------*/
/*-------------------------------------------------------------------------*/
