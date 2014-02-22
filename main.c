#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include "uv.h"

#define CHECK(r, msg) if (r) {                                                          \
  fprintf(stderr, "%s: [%s(%d): %s]\n", msg, uv_err_name((r)), (int)r, uv_strerror((r)));    \
  exit(1);                                                                              \
}

#define CHECK_CODE(r, msg) if (r < 0) {                                                     \
  fprintf(stderr, "%s: [%s(%d): %s]\n", msg, uv_err_name((r)), (int)r, uv_strerror((r)));    \
  exit(1);                                                                              \
}

#define UVERR(r, msg) fprintf(stderr, "%s: [%s(%d): %s]\n", msg, uv_err_name((r)), r, uv_strerror((r)));

#define STDOUT 1

void read_cb(uv_fs_t *req);
void write_cb(uv_fs_t *req);
void close_cb(uv_fs_t *req);

uv_fs_t open_req;
uv_fs_t read_req;
uv_fs_t write_req;
uv_fs_t close_req;

char buffer[1024];

void open_cb(uv_fs_t* req) {
  CHECK_CODE(req->result, "async open");

  fprintf(stderr, "opened %s\n", req->path);

  uv_file fd = req->result;

  fprintf(stderr, "file: %d\n", fd);
  uv_fs_read(uv_default_loop(), &read_req, fd, buffer, sizeof(buffer), -1 /*offset*/, read_cb);

  uv_fs_req_cleanup(req);
  assert(req->path == NULL);
}

void read_cb(uv_fs_t *req) {
  CHECK_CODE(req->result, "async read");

  uv_fs_req_cleanup(req);

  if (req->result == 0) {
    uv_fs_close(uv_default_loop(), &close_req, open_req.result, close_cb);
  } else {
    uv_fs_write(uv_default_loop(), &write_req, STDOUT, buffer, req->result, -1 /*offset*/, write_cb);
  }

  fprintf(stderr, "%ld bytes read\n", req->result);
}

void write_cb(uv_fs_t *req) {
  CHECK_CODE(req->result, "async write");

  uv_fs_req_cleanup(req);

  fprintf(stderr, "%ld bytes written\n", req->result);
  uv_fs_read(uv_default_loop(), &read_req, open_req.result, buffer, sizeof(buffer), -1, read_cb);
}

void close_cb(uv_fs_t *req) {
  uv_fs_req_cleanup(req);

  CHECK_CODE(req->result, "async close");
  fprintf(stderr, "closed\n");
}

int main() {
  const char *filename = "main.c";

  uv_fs_open(uv_default_loop(), &open_req, filename, O_RDONLY, S_IRUSR, open_cb);

  uv_run(uv_default_loop(), UV_RUN_DEFAULT);
  return 0;
}
