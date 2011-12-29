#include "types.h"
#include "user.h"
#include "lib.h"
#include "unet.h"
#include "fcntl.h"
#include "stat.h"

#define VERSION "0.1"
#define HTTP_VERSION "1.0"
#define BUFSIZE 512

static void
error(int s, int code)
{
  static struct {
    int code;
    const char *msg;
  } errors[] = {
    { 400, "Bad Request" },
    { 404, "Page Not Found" },
  };
  
  char buf[512];
  int i;
  int r;

  for (i = 0; i < NELEM(errors); i++)
    if (errors[i].code == code)
      break;

  if (i == NELEM(errors))
    die("httpd error: unknown code %u", code);

  snprintf(buf, 512, "HTTP/" HTTP_VERSION" %d %s\r\n"
           "Server: xv6-httpd/" VERSION "\r\n"
           "Connection: close"
           "Content-type: text/html\r\n"
           "\r\n"
           "<html><body><p>%d - %s</p></body></html>\r\n",
           errors[i].code, errors[i].msg, 
           errors[i].code, errors[i].msg);
  r = strlen(buf);

  if (write(s, buf, r) != r)
    die("httpd error: incomplete write %d", r);
}

static int
header(int s)
{
  static const char *h = "HTTP/" HTTP_VERSION " 200 OK\r\n"
    "Server: xv6-httpd/" VERSION "\r\n";

  int len;
  int r;

  len = strlen(h);
  r = write(s, h, len);
  if (r != len)
    die("httpd header: incomplete write %d", r);

  return 0;
}

static int
header_fin(int s)
{
  static const char *f = "\r\n";
  
  int len;
  int r;

  len = strlen(f);
  r = write(s, f, len);
  if (r != len)
    die("httpd fin: incomplete write %d", r);

  return 0;
}

static int
content_length(int s, u64 size)
{
  char buf[128];
  int len;
  int r;

  snprintf(buf, 128, "Content-Length: %lu\r\n", size);
  len = strlen(buf);

  r = write(s, buf, len);
  if (r != len)
    die("httpd size: incomplete write %d", r);
  return 0;
}

static int
content_type(int s)
{
  static const char *t = "Content-Type: text/plain\r\n";

  int len;
  int r;

  len = strlen(t);
  r = write(s, t, len);
  if (r != len)
    die("httpd content_type: incomplete write %d", r);
  return 0;
}

static int
content(int s, int fd)
{
  char buf[256];
  int n;
  int r;

  for (;;) {
    n = read(fd, buf, sizeof(buf));
    if (n < 0) {
      printf(2, "send_data: read failed %d\n", n);
      return n;
    } else if (n == 0) {
      return 0;
    }
    
    r = write(s, buf, n);
    if (r != n)
      die("httpd content: read failed %d", r);
  }
}

static void
resp(int s, const char *url)
{
  struct stat stat;
  int fd;
  int r;

  fd = open(url, O_RDONLY);
  if (fd < 0) {
    error(s, 404);
    return;
  }

  r = fstat(fd, &stat);
  if (r < 0) {
    printf(2, "httpd resp: fstat %d\n", r);
    close(fd);
    return error(s, 404);
  }

  if (stat.type != T_FILE && stat.type != T_DEV) {
    close(fd);
    return error(s, 404);
  }

  r = header(s);
  if (r < 0)
    goto error;

  r = content_length(s, stat.size);
  if (r < 0)
    goto error;

  r = content_type(s);
  if (r < 0)
    goto error;

  r = header_fin(s);
  if (r < 0)
    goto error;

  r = content(s, fd);
  if (r < 0)
    goto error;
  
  close(fd);
  return;

error:
  close(fd);
  error(s, -r);
}

static int
parse(const char *b, char **rurl)
{
  const char *url;
  int len;
  char *r;

  if (strncmp(b, "GET ", 4) != 0)
    return -1;

  b += 4;
  url = b;
  while (*b && *b != ' ')
    b++;
  len = b - url;

  r = malloc(len+1);
  if (r == NULL)
    return -1;
  memmove(r, url, len);
  r[len] = 0;

  *rurl = r;
  return 0;
}

static void
client(int s)
{
  char b[BUFSIZE];
  char *url;
  int r;

  r = read(s, b, NELEM(b)-1);
  if (r < 0)
    printf(1, "httpd client: read %d\n", r);
  b[r] = 0;
  
  r = parse(b, &url);
  if (r < 0) {
    error(s, 400);
    return;
  }    

  printf(1, "httpd client: url %s\n", url);
  resp(s, url);
  free(url);
}

int
main(void)
{
  int s;
  int r;

  s = socket(AF_INET, SOCK_STREAM, 0);
  if (s < 0)
    die("httpd socket: %d\n", s);

  struct sockaddr_in sin;
  sin.sin_family = AF_INET;
  sin.sin_addr.s_addr = htonl(INADDR_ANY);
  sin.sin_port = htons(80);
  r = bind(s, (struct sockaddr *)&sin, sizeof(sin));
  if (r < 0)
    die("httpd bind: %d\n", r);
  
  r = listen(s, 5);
  if (r < 0)
    die("httpd listen: %d\n", r);

  printf(1, "httpd: port 80\n");

  for (;;) {
    socklen_t socklen;
    int ss;
    
    socklen = sizeof(sin);
    ss = accept(s, (struct sockaddr *)&sin, &socklen);
    if (ss < 0) {
      printf(2, "telnetd accept: %d\n", ss);
      continue;
    }
    printf(1, "httpd: connection\n");

    client(ss);
    close(ss);
  }
}
