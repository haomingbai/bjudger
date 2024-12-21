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