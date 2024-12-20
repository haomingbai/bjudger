#include "problem.h"
#include <boost/json.hpp>
#include <memory>
#include <string>
#include <unordered_map>
#include <workflow/WFHttpServer.h>

extern std::unordered_map<std::string, std::unique_ptr<bjudger::Problem>> problems;

void exist(WFHttpTask *task)
{
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
                    if (problem->hasLang(l.as_string().c_str()))
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

void judge(WFHttpTask *task)
{
    // Unfinished
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

void hi(WFHttpTask *task)
{
    auto req = task->get_req();
    size_t size;
    void *body;
    const void **bodyPtr = (const void **)&body;
    req->get_parsed_body(bodyPtr, &size);
    std::string requestString((char *)body, size);
    boost::json::object request = boost::json::parse(requestString).as_object();
    auto *resp = task->get_resp();
    boost::json::object response;
    response["message"] = "Hello, World!";
    response["request"] = request.at("message").as_string().c_str();
    resp->append_output_body(boost::json::serialize(response).c_str());
    resp->set_status_code("200");
}

std::unordered_map<std::string, std::function<void(WFHttpTask *)>> routes = {
    {"/exist", exist}, {"/judge", judge}, {"/", hi}, {"/hi", hi}};

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

void initServer(int port, const std::string &cert, const std::string &key)
{
    auto server = WFHttpServer(route);
    if (cert.empty() || key.empty())
    {
        if (server.start(port) == 0)
        {
            getchar();
            // press "Enter" to end.
            server.stop();
        }
    }
    else if (server.start(port, cert.c_str(), key.c_str()) == 0)
    {
        getchar();
        // press "Enter" to end.
        server.stop();
    }
}
