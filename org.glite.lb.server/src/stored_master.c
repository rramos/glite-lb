#ident "$Header$"

#include <sys/time.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <string.h>
#include <fcntl.h>
#include <errno.h>
#include <assert.h>

#include "glite/lb/il_msg.h"
#include "glite/lb/lb_gss.h"
#include "glite/lb/lb_plain_io.h"
#include "glite/lb/context-int.h"

#include "store.h"

static edg_wll_Context tmp_ctx;

static
int
gss_reader(char *buffer, int max_len)
{
  int ret, len;
  edg_wll_GssStatus gss_code;

  ret = edg_wll_gss_read_full(&tmp_ctx->connPool[tmp_ctx->connToUse].gss,
			      buffer, max_len,
			      &tmp_ctx->p_tmp_timeout,
			      &len, &gss_code);
  if(ret < 0) switch(ret) {

  case EDG_WLL_GSS_ERROR_TIMEOUT: 
    ret = edg_wll_SetError(tmp_ctx, ETIMEDOUT, "read message");
    break;

  case EDG_WLL_GSS_ERROR_EOF:
    ret = edg_wll_SetError(tmp_ctx, ENOTCONN, NULL);
    break;

  default:
    ret = edg_wll_SetError(tmp_ctx, EDG_WLL_ERROR_GSS, "read message");
    break;
  }

  return(ret);
}


int edg_wll_StoreProto(edg_wll_Context ctx)
{
	char	*buf;
	int	len,ret;
	size_t	total;
	edg_wll_GssStatus	gss_code;

	edg_wll_ResetError(ctx);
	tmp_ctx = ctx;
	ret = read_il_data(&buf, gss_reader);
	if(ret < 0) 
	  return(ret);

	handle_request(ctx,buf);
	free(buf);

	if ((len = create_reply(ctx,&buf)) > 0) {
		if ((ret = edg_wll_gss_write_full(&ctx->connPool[ctx->connToUse].gss,buf,len,&ctx->p_tmp_timeout,&total,&gss_code)) < 0)
			edg_wll_SetError(ctx,
				ret == EDG_WLL_GSS_ERROR_TIMEOUT ? 
					ETIMEDOUT : EDG_WLL_ERROR_GSS,
				"write reply");
		free(buf);
	}
	else edg_wll_SetError(ctx,E2BIG,"create_reply()");

	return edg_wll_Error(ctx,NULL,NULL);
}


static
int
gss_plain_reader(char *buffer, int max_len)
{
  int ret;

  ret = edg_wll_plain_read_full(&tmp_ctx->connProxy->conn, buffer, max_len,
				&tmp_ctx->p_tmp_timeout);
  if(ret < 0) return(edg_wll_SetError(tmp_ctx, errno, "StoreProtoProxy() - reading data"));

  return(ret);
}


int edg_wll_StoreProtoProxy(edg_wll_Context ctx)
{
	char	*buf;
	int		len, ret;


	edg_wll_ResetError(ctx);
	tmp_ctx = ctx;
	ret = read_il_data(&buf, gss_plain_reader);
	if(ret < 0)
	  return(ret);

	handle_request(ctx, buf);
	free(buf);

	if ( (len = create_reply(ctx, &buf)) > 0 ) {
		if ( edg_wll_plain_write_full(&ctx->connProxy->conn, buf, len, &ctx->p_tmp_timeout) < 0 )
			return edg_wll_SetError(ctx, errno, "StoreProtoProxy() - sending reply");
		free(buf);
	}
	else edg_wll_SetError(ctx, E2BIG, "create_reply()");

	return edg_wll_Error(ctx, NULL, NULL);
}
