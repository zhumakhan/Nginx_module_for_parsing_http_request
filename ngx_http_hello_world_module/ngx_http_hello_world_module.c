/*
	./configure --prefix=/home/juma/nginxmod --add-dynamic-module=/home/juma/nginx-handler/ngx_http_hello_world_module --with-http_ssl_module
  make
  make install

https://github.com/perusio/nginx-hello-world-module
https://www.nginx.com/resources/wiki/extending/examples/body_filter
http://nginx.org/en/docs/dev/development_guide.html#http_request_body
http://mailman.nginx.org/pipermail/nginx/2014-March/042905.html
https://www.geeksforgeeks.org/kmp-algorithm-for-pattern-searching/
*/
#include <ngx_config.h>
#include <ngx_core.h>
#include <ngx_http.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <stdlib.h>
#include <time.h>

static FILE *file_ptr;
char *file_name = (char*)"/home/juma/Desktop/ngx_log.txt";
u_char *ngx_hello_world;
size_t sz;
u_char  *text = NULL;
size_t  textLen = 0;
size_t strConcat(u_char **text, size_t textLen, u_char *src, size_t srcLen);
long KMP(u_char *Text, size_t TextLen, u_char *Pattern, size_t PatternLen, size_t StartSeachIndex);
static int hacker_counter = 0;

static char *ngx_http_hello_world(ngx_conf_t *cf, ngx_command_t *cmd, void *conf);

static ngx_command_t  ngx_http_hello_world_commands[] = {
  {
    ngx_string("print_hello_world"),
    NGX_HTTP_LOC_CONF|NGX_CONF_NOARGS,
    ngx_http_hello_world,
    0,
    0,
    NULL
  },
    ngx_null_command
};

static ngx_http_module_t  ngx_http_hello_world_module_ctx = {
  NULL,
  NULL,
  NULL,
  NULL,
  NULL,
  NULL,
  NULL,
  NULL
};

ngx_module_t ngx_http_hello_world_module = {
  NGX_MODULE_V1,
  &ngx_http_hello_world_module_ctx,
  ngx_http_hello_world_commands,
  NGX_HTTP_MODULE,
  NULL,
  NULL,
  NULL,
  NULL,
  NULL,
  NULL,
  NULL,
  NGX_MODULE_V1_PADDING
};


static void ngx_http_hello_world_handler(ngx_http_request_t *r){
  file_ptr = fopen(file_name,"a+");
  if(file_ptr == NULL){
        ngx_log_error(NGX_LOG_ERR, r->connection->log, 0, "Failed to create log file.%s", strerror(errno));
  }
int count_asked = 0, found = 0;
{
  u_char *temp;
  if(r->uri.len != 14){
    goto L1;
  }
  temp = (u_char*)"/hackers_count";
    for(int i = 0; i < 14; i++){
      if(r->uri.data[i] != temp[i])goto L1;
    }
    count_asked = 1;
    goto L2;
}
//Header
  L1:
  textLen = strConcat(&text, textLen, r->request_line.data, r->request_line.len);
  {
    u_char            ch;
    ngx_uint_t        i, n;
    ngx_table_elt_t  *header;
    ngx_list_part_t  *part;
    ngx_str_t temp_str;

    part = &r->headers_in.headers.part;
    header = part->elts;

    for (i = 0;; i++) {
        if (i >= part->nelts) {
            if (part->next == NULL) break;
            part = part->next;
            header = part->elts;
            i = 0;
        }
        for (n = 0; n < header[i].key.len; n++) {
            ch = header[i].key.data[n];
            temp_str.data = (u_char*)&ch;
            temp_str.len = 1;
            textLen = strConcat(&text, textLen, temp_str.data, temp_str.len);   
        }
        for (n = 0;n < header[i].value.len; n++) {
            ch = header[i].value.data[n];
            temp_str.data = (u_char*)&ch;
            temp_str.len = 1;
            textLen = strConcat(&text, textLen, temp_str.data, temp_str.len);      
        }
    }
  }
//Body
  if(r->request_body != NULL){{ 
      if(NULL == r->request_body->temp_file){//temp_file is empty, read from buffers
        u_char                      *p;
        ngx_chain_t                 *cl, *in;
        ngx_str_t                   temp_str;
        in = r->request_body->bufs;
        for (cl = in; cl; cl = cl->next) {
            p = cl->buf->pos;
          for (p = cl->buf->pos; p < cl->buf->last; p++) {
            temp_str.data = p;
            temp_str.len = 1;
            textLen = strConcat(&text, textLen, temp_str.data, temp_str.len);
          }
        }   
      }else{//temp file used, read from file.
        size_t ret;
        size_t offset = 0;
        u_char *data;
        data = (u_char*)calloc(4096, sizeof(u_char));
        while(  (ret = ngx_read_file(&r->request_body->temp_file->file, data, 4096, offset)) > 0)
        {
            textLen = strConcat(&text, textLen, data, ret);
            offset = offset + ret;
        }
      }
      
    }}
//string match;
{
long int ipos = KMP(text, textLen, (u_char*)" i ", 1,0);
long int ampos = -1;
while( ampos != -2){
  ampos = KMP(text, textLen, (u_char*)" am ", 2,ampos+1);
  if(ampos > ipos)break;
}
long int hackpos = -1;
while(hackpos != -2){
  hackpos= KMP(text, textLen, (u_char*)" hacker ", 6,hackpos+1);
  if(hackpos > ampos)break;
}

if(ipos != -2 && ampos != -2 && hackpos != -2){
  hacker_counter++;
  found = 1;
  if(count_asked == 0){
    ngx_table_elt_t *h;
    h = ngx_list_push(&r->headers_out.headers);
    h->hash = 1;
    ngx_str_set(&h->key, "Location");
    ngx_str_set(&h->value, "http://cybersec.kz/");
  }
  time_t t = time(NULL);
  struct tm tm = *localtime(&t);
  char temp[35];
  sprintf(temp, "[%d-%d-%d %d:%d:%d]\t ", tm.tm_year + 1900, tm.tm_mon + 1, tm.tm_mday, tm.tm_hour, tm.tm_min, tm.tm_sec);
  fputs(temp,file_ptr); 
  fputs("hacker detected\n",file_ptr);
  fclose(file_ptr);
}
}
L2:
  if(count_asked == 1){
    {int space = 0, temp = hacker_counter;
    if(temp == 0)space = 1;
    else{
        while(temp != 0){
        space++;
        temp /= 10;
      }
    }
    ngx_hello_world = (u_char*)calloc(space,sizeof(u_char));
    sprintf((char*)ngx_hello_world, "%d", hacker_counter);
    sz = space;
  }}
  else{
    ngx_hello_world = (u_char*)"OK";
    sz = 2;
  }
  if(text != NULL){free(text); text = NULL;}
  textLen = 0;

  r->headers_out.content_type.len = ngx_strlen("text/html") - 1;
  r->headers_out.content_type.data = (u_char *) "text/html";
  r->headers_out.status = NGX_HTTP_OK;
  if(count_asked == 0 && found == 1){
    r->headers_out.status = NGX_HTTP_MOVED_TEMPORARILY;
  }
  r->headers_out.content_length_n = sz;
  ngx_http_send_header(r);

  ngx_buf_t    *b;
  ngx_chain_t   *out;

  b = ngx_calloc_buf(r->pool);
  out = ngx_alloc_chain_link(r->pool);

  out->buf = b;
  out->next = NULL;
  	
  b->pos = ngx_hello_world;
  b->last = ngx_hello_world + sz;
  
  b->memory = 1;
  b->last_buf = 1;
  ngx_http_finalize_request(r, ngx_http_output_filter(r, out));
  return;
}

static ngx_int_t ngx_http_hello_world_body_handler(ngx_http_request_t *r){
    ngx_int_t     rc;
    rc = ngx_http_read_client_request_body(r, ngx_http_hello_world_handler);

    if (rc >= NGX_HTTP_SPECIAL_RESPONSE){
            return rc;
    }
   return NGX_DONE;
}
static char *ngx_http_hello_world(ngx_conf_t *cf, ngx_command_t *cmd, void *conf){
  ngx_http_core_loc_conf_t  *clcf;
  clcf = ngx_http_conf_get_module_loc_conf(cf, ngx_http_core_module);
  clcf->handler = ngx_http_hello_world_body_handler;
  return NGX_CONF_OK;
}
size_t strConcat(u_char **text, size_t textLen, u_char *src, size_t srcLen){
    u_char * temp = NULL;
    size_t i, size = textLen + srcLen;
    temp = (u_char*)calloc(size, sizeof(u_char));
    for(i = 0; i < textLen; i++){
      temp[i] = (*text)[i];
    }
    for(i = textLen; i < size; i++){
      temp[i] = src[i-textLen];
    }

    if(*text != NULL){free(*text); *text = NULL;}
    *text = temp;
    return size;
}
void Prefix(long *Pi, u_char *Pattern, size_t PatternLen){
  size_t len = 0, i = 1;
  Pi[0] = 0;
  while(i < PatternLen){
    if(Pattern[len] == Pattern[i]){
      len++;
      Pi[i] = len;
      i++;
    }else{
      if(len != 0)len = Pi[len-1];
      else{
        Pi[i] = 0;
        i++;
      }
    }
  }
}
long KMP(u_char *Text, size_t TextLen, u_char *Pattern, size_t PatternLen, size_t StartSeachIndex){
  long *Pi;
  Pi = (long*)calloc(PatternLen, sizeof(long));
  size_t i = StartSeachIndex, j = 0;
  while(i < TextLen){
    if(Pattern[j] == Text[i]){
        i++;
        j++;
      }
      if(j == PatternLen){
	free(Pi);
        return i-PatternLen;
      }
      else if(i < TextLen && Pattern[j] != Text[i]){
        if(j != 0){
          j = Pi[j-1];
        }else{
          i++;
        }
      }
    }
    free(Pi);
    return -2;
}
