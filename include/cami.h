/* Author : Jerome ODIER
 * Email : jerome.odier@lpsc.in2p3.fr
 */

/*-------------------------------------------------------------------------*/

#ifndef __CAMI_H_
#define __CAMI_H_

/*-------------------------------------------------------------------------*/

#include <stddef.h>
#include <stdint.h>

#include <sys/types.h>

/*-------------------------------------------------------------------------*/

#include <map>
#include <string>
#include <stdexcept>

/*-------------------------------------------------------------------------*/

#define buff_t /* */ void *
#define BUFF_t const void *

/*-------------------------------------------------------------------------*/
/* SSL CLIENT                                                              */
/*-------------------------------------------------------------------------*/

struct ssl_ctx_st;

struct ssl_st;

/*-------------------------------------------------------------------------*/
/* SSL CLIENT                                                              */
/*-------------------------------------------------------------------------*/

typedef struct cami_client_ctx_s
{
	struct ssl_ctx_st *ssl_ctx;

	struct ssl_st *ssl;

	int sock;

} cami_client_ctx_t;

/*-------------------------------------------------------------------------*/

#define cami_client_ctx_get_sock(client_ctx) \
	((client_ctx)->sock)

#define cami_client_ctx_get_ssl(client_ctx) \
	((client_ctx)->ssl)

/*-------------------------------------------------------------------------*/

struct ssl_ctx_st *cami_openssl_create_context(void);

void cami_openssl_destroy_context(
	struct ssl_ctx_st *ssl_ctx
);

int cami_openssl_connect(
	struct cami_client_ctx_s *client_ctx,
	struct ssl_ctx_st *ssl_ctx,
	const char *host,
	uint16_t port
);

int cami_openssl_close(
	struct cami_client_ctx_s *client_ctx
);

ssize_t cami_openssl_read(
	struct cami_client_ctx_s *client_ctx,
	buff_t buff,
	size_t size
);

ssize_t cami_openssl_write(
	struct cami_client_ctx_s *client_ctx,
	BUFF_t buff,
	size_t size
);

void cami_openssl_show_error(void);

/*-------------------------------------------------------------------------*/
/* AMI CLIENT                                                              */
/*-------------------------------------------------------------------------*/

typedef struct cami_ctx_s
{
	std::string host;
	std::string path;

	struct cami_client_ctx_s client_ctx;

} cami_ctx_t;

/*-------------------------------------------------------------------------*/

bool cami_create_context(
	struct cami_ctx_s *ctx,
	const std::string &host,
	const std::string &path,
	uint16_t port
);

bool cami_destroy_context(
	struct cami_ctx_s *ctx
);

std::string cami_execute(
	struct cami_ctx_s *ctx,
	const std::string &command,
	const std::map<std::string, std::string> &arguments
);

/*-------------------------------------------------------------------------*/

class AMIClient
{
    private:
	struct cami_ctx_s m_ctx;

    public:
	/*-----------------------------------------------------------------*/

	AMIClient(const std::string &host, const std::string &path, uint16_t port)
	{
		if(cami_create_context(&this->m_ctx, host, path, port) == false)
		{
			throw std::logic_error("could not connect");
		}
	}

	/*-----------------------------------------------------------------*/

	~AMIClient(void)
	{
		if(cami_destroy_context(&this->m_ctx) == false)
		{
			throw std::logic_error("could not disconnect");
		}
	}

	/*-----------------------------------------------------------------*/

	std::string execute(const std::string &command, const std::map<std::string, std::string> &arguments)
	{
		return ::cami_execute(&this->m_ctx, command, arguments);
	}

	/*-----------------------------------------------------------------*/
};

/*-------------------------------------------------------------------------*/

#endif /* __CAMI_H_ */

/*-------------------------------------------------------------------------*/
