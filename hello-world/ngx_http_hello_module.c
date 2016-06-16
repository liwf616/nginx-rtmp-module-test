#include <ngx_config.h>
#include <ngx_core.h>
#include <ngx_http.h>

static int ngx_hello_visited_times = 0;

static char* ngx_http_hello_string(ngx_conf_t *cf, ngx_command_t* cmd, void* conf);
static char *ngx_http_hello_counter(ngx_conf_t *cf, ngx_command_t *cmd,
                                    void *conf);

static ngx_int_t ngx_http_hello_init(ngx_conf_t *cf);
static void *ngx_http_hello_create_loc_conf(ngx_conf_t *cf);
static ngx_int_t handler(ngx_http_request_t *);

typedef struct
{
    ngx_str_t hello_string;
    ngx_int_t hello_count;
}ngx_http_hello_loc_conf_t;

static ngx_command_t test_commands[] = {
    {
        ngx_string("test"),
        NGX_HTTP_LOC_CONF | NGX_CONF_NOARGS |NGX_CONF_TAKE1,
        ngx_http_hello_string,
        NGX_HTTP_LOC_CONF_OFFSET,
        offsetof(ngx_http_hello_loc_conf_t,hello_string),
        NULL
    },
    {
        ngx_string("hello_count"),
        NGX_HTTP_LOC_CONF | NGX_CONF_FLAG,
        ngx_http_hello_counter,
        NGX_HTTP_LOC_CONF_OFFSET,
        offsetof(ngx_http_hello_loc_conf_t,hello_count),
        NULL
    },
    ngx_null_command
};


static ngx_http_module_t test_ctx = {
    NULL,
    ngx_http_hello_init,
    NULL,
    NULL,
    NULL,
    NULL,
    ngx_http_hello_create_loc_conf,
    NULL
};

ngx_module_t ngx_http_hello_module = {
    NGX_MODULE_V1,
    &test_ctx,
    test_commands,
    NGX_HTTP_MODULE,
    NULL,NULL,NULL,NULL,NULL,NULL,NULL,
    NGX_MODULE_V1_PADDING
};

//配置解析
static char* ngx_http_hello_string(ngx_conf_t *cf, ngx_command_t *cmd, void *conf) {
    ngx_http_hello_loc_conf_t* local_conf;
    local_conf = conf;
    char* rv = ngx_conf_set_str_slot(cf, cmd, conf);

    ngx_conf_log_error(NGX_LOG_EMERG, cf, 0, "test:%s", local_conf->hello_string.data);

    return rv;
}

static char *ngx_http_hello_counter(ngx_conf_t *cf, ngx_command_t *cmd,
                                    void *conf)
{
    ngx_http_hello_loc_conf_t * local_conf;
    local_conf = conf;

    char *rv = NULL;
    rv = ngx_conf_set_flag_slot(cf, cmd, conf);
    ngx_conf_log_error(NGX_LOG_EMERG, cf, 0, "hello_count:%d", local_conf->hello_count);
    return rv;
}

//handler 注册
static ngx_int_t
ngx_http_hello_init(ngx_conf_t *cf)
{
    ngx_http_handler_pt *h;
    ngx_http_core_main_conf_t *cmcf;

    cmcf = ngx_http_conf_get_module_main_conf(cf,ngx_http_core_module);

    h = ngx_array_push(&cmcf->phases[NGX_HTTP_CONTENT_PHASE].handlers);
    if(h == NULL) {
        return NGX_ERROR;
    }

    *h = handler;

    return NGX_OK;
}

static void *ngx_http_hello_create_loc_conf(ngx_conf_t *cf)
{
    ngx_http_hello_loc_conf_t* local_conf = NULL;
    local_conf = ngx_pcalloc(cf->pool, sizeof(ngx_http_hello_loc_conf_t));
    if (local_conf == NULL)
    {
        return NULL;
    }

    // init param
    ngx_str_null(&local_conf->hello_string);
    local_conf->hello_count = NGX_CONF_UNSET;

    return local_conf;
}

//handler to process /test
static ngx_int_t handler(ngx_http_request_t *req) {
    ngx_chain_t  out;
    ngx_int_t    rc;
    ngx_http_hello_loc_conf_t *local_conf;

    u_char html[1024] = "<h1>This is Test Page!</h1>";
    u_char ngx_hello_string[1024] = {0};

    ngx_log_error(NGX_LOG_EMERG,req->connection->log,0,"handler is called!");

    local_conf = ngx_http_get_module_loc_conf(req,ngx_http_hello_module);
    if(local_conf->hello_string.len == 0) {
        ngx_log_error(NGX_LOG_EMERG, req->connection->log, 0, "hello_string is empty!");
        return NGX_DECLINED;
    }

    if(local_conf->hello_count == NGX_CONF_UNSET
       || local_conf->hello_count == 0) {
        ngx_sprintf(ngx_hello_string, "%s",local_conf->hello_string.data);
    } else {
        ngx_sprintf(ngx_hello_string,"%s visited time:%d",html,++ ngx_hello_visited_times );
    }

    int len = ngx_strlen(ngx_hello_string);
    req->headers_out.status = NGX_HTTP_OK;
    req->headers_out.content_length_n = len;
    ngx_str_set(&req->headers_out.content_type, "text/html");

    rc = ngx_http_discard_request_body(req);
    if(rc != NGX_OK) {
        return rc;
    }

    /* send the header only, if the request type is http 'HEAD' */
    if(req->method == NGX_HTTP_HEAD) {
        return ngx_http_send_header(req);
    }

    /* allocate a buffer for your response body */
    ngx_buf_t *b;
    b = ngx_pcalloc(req->pool, sizeof(ngx_buf_t));
    if(b == NULL) {
        return NGX_HTTP_INTERNAL_SERVER_ERROR;
    }

    /* attach this buffer to the buffer chain */
    out.buf = b;
    out.next = NULL;

    /* adjust the pointers of the buffer */
    b->pos = ngx_hello_string;
    b->last = ngx_hello_string + len;
    b->memory = 1;     /* this buffer is in memory */
    b->last_buf = 1;   /* this is the last buffer in the buffer chain */
    /* send the headers of your response */
    rc = ngx_http_send_header(req);

    if(rc == NGX_ERROR || rc > NGX_OK || req->header_only) {
        return rc;
    }

    return ngx_http_output_filter(req, &out);
}
