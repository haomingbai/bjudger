#include "problem.h"
#include <boost/json.hpp>
#include <memory>
#include <string>
#include <unordered_map>
#include <workflow/WFHttpServer.h>
#include "context.h"

extern std::unordered_map<std::string, std::unique_ptr<bjudger::Problem>> &problems;

void hi(WFHttpTask *task)
{
    auto req = task->get_req();
    size_t size;
    void *body;
    const void **bodyPtr = (const void **)&body;
    req->get_parsed_body(bodyPtr, &size);
    std::string requestString((char *)body, size);
    try
    {
        boost::json::object request = boost::json::parse(requestString).as_object();
        auto *resp = task->get_resp();
        boost::json::object response;
        response["message"] = "Hello, World!";
        response["request"] = request.at("message").as_string().c_str();
        resp->append_output_body(boost::json::serialize(response).c_str());
        resp->set_status_code("200");
    }
    catch (const std::exception &e)
    {
        boost::json::object response;
        response["message"] = "Error";
        response["error"] = e.what();
        auto *resp = task->get_resp();
        resp->append_output_body(boost::json::serialize(response).c_str());
    }
}

std::unordered_map<std::string, std::function<void(WFHttpTask *)>> routes {{"/hi", hi}};

void route(WFHttpTask *task)
{
    auto req = task->get_req();
    auto path = req->get_request_uri();
    if (routes.find(path) != routes.end())
    {
        routes[path](task);
    }
    else
    {
        auto *resp = task->get_resp();
        resp->set_status_code("404");
        resp->append_output_body("404 Not Found");
    }
}