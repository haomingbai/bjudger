#include "problem.h"
#include <boost/json.hpp>
#include <memory>
#include <string>
#include <unordered_map>
#include <workflow/WFHttpServer.h>
#include "context.h"

extern "C"
{
    void api(bjudger::Context *context, WFHttpTask *task);
}

void api(bjudger::Context *context, WFHttpTask *task)
{
    // Unfinished
    auto &problems = context->problems;
    namespace json = boost::json;

    // Get the request and response objects
    auto req = task->get_req();
    auto resp = task->get_resp();
    void *body;
    size_t size;
    req->get_parsed_body((const void **)&body, &size);
    std::string requestString((char *)body, size);

    // Parse the request
    json::object request;
}

