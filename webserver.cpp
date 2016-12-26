#include <iostream>

#include <assert.h>

#include <event2/event.h>
#include <event2/http.h>
#include <event2/buffer.h>
#include <event2/util.h>

#include <string>
#include <tuple>
#include <cstdint>

#define UNUSED(expr) do { (void)(expr); } while (0)


enum class URI_TO_PATH_STATUS : std::int8_t {
  SUCCESS = 0,
  FAILURE_URI_PARSE,
  FAILURE_URI_TO_PATH,
  FAILURE_PATH_DECODE,
};

std::tuple<std::string, URI_TO_PATH_STATUS> uri_to_path(const char *uri) {
  assert(uri != NULL);

  struct evhttp_uri *decodedUri = NULL;
  const char *path = NULL;
  const char *decodedPath = NULL;

  URI_TO_PATH_STATUS status = URI_TO_PATH_STATUS::SUCCESS;

  decodedUri = evhttp_uri_parse(uri);

  if (!decodedUri) {
    status = URI_TO_PATH_STATUS::FAILURE_URI_PARSE;
    goto end;
  }

  path = evhttp_uri_get_path(decodedUri);

  if (!path) {
    status = URI_TO_PATH_STATUS::FAILURE_URI_TO_PATH;
    goto end;
  }

  decodedPath = evhttp_uridecode(path, 0, NULL);

  if (!decodedPath) {
    status = URI_TO_PATH_STATUS::FAILURE_PATH_DECODE;
    goto end;
  }

end:
  if (decodedUri) {
    evhttp_uri_free(decodedUri);
  }

  auto result = std::make_tuple(
      (status == URI_TO_PATH_STATUS::SUCCESS ? std::string(decodedPath) : std::string("")),
      status
                         );

  free((void *)decodedPath);

  return result;
}

static void handle_file(struct evhttp_request *req, void *arg)
{
  UNUSED(arg);

  const char *uri = evhttp_request_get_uri(req);

  // only GET requests allowed
  if (evhttp_request_get_command(req) != EVHTTP_REQ_GET) {
    evhttp_send_error(req, HTTP_BADREQUEST, 0);
    return;
  }

  std::cout << "GET " << uri << std::endl;

  auto result = uri_to_path(uri);

  auto path = std::get<0>(result);
  auto status = std::get<1>(result);

  assert(status == URI_TO_PATH_STATUS::SUCCESS);

  std::cout << "Got decoded path " << path << std::endl;

  // here we could send back HTML with evbuffer_add_printf

  // return not found by default as long as we don't handle the URI
  evhttp_send_error(req, HTTP_NOTFOUND, 0);
}

int main(int argc, char **argv)
{
  UNUSED(argc);

  struct event_base *base;
  struct evhttp *http;
  struct evhttp_bound_socket *handle;

  const ev_uint16_t port = 8881;
  const char *host = "0.0.0.0";

  base = event_base_new();

  if (!base) {
    std::cerr << "FAILURE: Couldn't create an event_base." << std::endl;
    return 1;
  }

  http = evhttp_new(base);

  if (!http) {
    std::cerr << "FAILURE: Couldn't create an evhttp." << std::endl;
    return 1;
  }

  // register catchall handler
  evhttp_set_gencb(http, handle_file, argv[1]);

  handle = evhttp_bind_socket_with_handle(http, host, port);

  if (!handle) {
    std::cerr << "FAILURE: Couldn't bind to " << host << ":" << port << std::endl;
    return 1;
  }

  event_base_dispatch(base);

  return 0;
}
