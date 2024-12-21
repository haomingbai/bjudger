#include "context.h"
#include "problem.h"
#include <boost/json.hpp>
#include <iostream> // Debug
#include <memory>
#include <string>
#include <unordered_map>
#include <workflow/WFHttpServer.h>

extern std::unordered_map<std::string, std::unique_ptr<bjudger::Problem>> &problems;

extern bjudger::Context ctx;

void hi(WFHttpTask *task)
{
    auto req = task->get_req();
    size_t size;
    void *body;
    const void **bodyPtr = (const void **)&body;
    req->get_parsed_body(bodyPtr, &size);
    std::cout << "debug" << std::endl;
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

void api(bjudger::Context *context , WFHttpTask *task)
{
    auto &problems = context->problems;
    auto *req = task->get_req();
    auto *resp = task->get_resp();
    void *body;
    const void **bodyPtr = (const void **)&body;
    size_t size;
    req->get_parsed_body(bodyPtr, &size);

    std::string requestString((char *)body, size);
    try
    {
        namespace json = boost::json;
        auto request = json::parse(requestString).as_array();
        json::array response;
        response.reserve(request.size());
        for (auto problem : request)
        {
            // Create a response object for each problem
            auto problemObj = problem.as_object();
            std::string id = problemObj.at("id").as_string().c_str();
            json::object responseProblemObj;

            // The problem id is always returned
            responseProblemObj["id"] = id;
            json::array langSupportArray;
            auto lang = problemObj.at("lang").as_array();
            langSupportArray.reserve(lang.size());

            // Check if the problem exists
            if (problems.find(id) != problems.end())
            {
                auto problem = problems.at(id).get();

                // Check if the problem supports the given languages
                for (auto l : lang)
                {
                    json::object langSupport{{"lang", l.as_string().c_str()}, {"support", false}};
                    std::string langStr = l.as_string().c_str();
                    if (problem->hasLang(std::move(langStr)))
                    {
                        langSupport["support"] = true;
                    }
                    langSupportArray.push_back(langSupport);
                }
            }
            else // If the problem does not exist, return false for all languages
            {
                for (auto l : lang)
                {
                    langSupportArray.push_back(json::object{{"lang", l.as_string().c_str()}, {"support", false}});
                }
            }

            // Add the language support array to the response object
            responseProblemObj["lang"] = langSupportArray;
            response.push_back(responseProblemObj);
        }
        resp->add_header_pair("Content-Type", "application/json");
        auto responseString = json::serialize(response);
        resp->append_output_body(responseString.c_str(), responseString.size());
        resp->set_status_code("200");
    }
    catch (const std::exception &e)
    {
        resp->append_output_body("Error: " + std::string(e.what()));
        resp->set_status_code("500");
        return;
    }
}

std::unordered_map<std::string, std::function<void(WFHttpTask *)>> routes{{"/hi", hi}, {"/api", std::bind(api, &ctx, std::placeholders::_1)}};

void route(WFHttpTask *task)
{
    auto req = task->get_req();
    auto path = req->get_request_uri();
    std::cout << path << std::endl;
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