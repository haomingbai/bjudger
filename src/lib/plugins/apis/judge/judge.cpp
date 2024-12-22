#include "context.h"
#include "problem.h"
#include <boost/json.hpp>
#include <memory>
#include <string>
#include <unordered_map>
#include <workflow/WFHttpServer.h>

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

    try
    {
        json::object request = json::parse(requestString).as_object();
        std::string id = request["id"].as_string().c_str();
        std::string code = request["code"].as_string().c_str();
        std::string token = request["token"].as_string().c_str();
        std::string lang = request["lang"].as_string().c_str();
        auto result = problems.at(id)->judge(std::move(code), std::move(lang));
        json::object response{{"id", id}};
        if (result.isSuccessful)
        {
            json::array cases;
            for (size_t i = 0; i < result.result.status.size(); i++)
            {
                json::object caseResult{{"status", result.result.status.at(i)},
                                        {"timeUsage", result.result.timeUsage.at(i)},
                                        {"memoryUsage", result.result.memoryUsage.at(i)}};
                cases.push_back(caseResult);
            }
            response["result"] = cases;
        }
        else
        {
            json::array cases;
            // Give all the cases a status of -1 with memory and time usage of 0
            for (size_t i = 0; i < result.result.status.size(); i++)
            {
                json::object caseResult{{"status", -1}, {"timeUsage", 0}, {"memoryUsage", 0}};
                cases.push_back(caseResult);
            }
            response["result"] = cases;
        }

        std::string responseString = json::serialize(response);
        resp->append_output_body(responseString.c_str(), responseString.length());
        resp->set_status_code("200");
        resp->set_header_pair("Content-Type", "application/json");
        resp->set_header_pair("Connection", "close");
    }
    catch (const std::exception &e)
    {
        json::object response = {{"message", e.what()}};
        std::string responseString = json::serialize(response);
        resp->append_output_body(responseString.c_str(), responseString.length());
        resp->set_status_code("400");
        resp->set_header_pair("Content-Type", "application/json");
        resp->set_header_pair("Connection", "close");
    }
}
