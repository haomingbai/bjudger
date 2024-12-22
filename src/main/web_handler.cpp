#include "context.h"
#include "problem.h"
#include <boost/json.hpp>
#include <chrono>
#include <iostream> // Debug
#include <memory>
#include <string>
#include <thread>
#include <unordered_map>
#include <workflow/WFHttpServer.h>
#include <workflow/WFTaskFactory.h>

extern std::unordered_map<std::string, std::unique_ptr<bjudger::Problem>> &problems;

extern bjudger::Context ctx;

void hi(WFHttpTask *task)
{
    auto req = task->get_req();
    std::this_thread::sleep_for(std::chrono::seconds(2));
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

std::unordered_map<std::string, std::function<void(WFHttpTask *)>> routes{{"/hi", hi}};

void route(WFHttpTask *task)
{
    auto req = task->get_req();
    auto path = req->get_request_uri();
#ifdef DEBUG
    std::cout << path << std::endl;
#endif
    if (routes.find(path) != routes.end())
    {
        WFGoTask *goTask = WFTaskFactory::create_go_task("route", routes[path], task);

        goTask->set_callback([task](WFGoTask *go_task) {
            SeriesWork *series = series_of(task);
            auto resp = task->get_resp();

            if (go_task->get_state() != WFT_STATE_SUCCESS)
            {
                resp->set_status_code("500");
                boost::json::object response;
                response["message"] = "Internal Server Error";
                std::string responseString = boost::json::serialize(response);
                resp->append_output_body(responseString.c_str(), responseString.length());
            }
        });

        auto series = series_of(task);
        *series << goTask;
        series->set_callback([](const SeriesWork *series) {});
    }
    else
    {
        auto *resp = task->get_resp();
        resp->set_status_code("404");
        resp->append_output_body("404 Not Found");
    }
}