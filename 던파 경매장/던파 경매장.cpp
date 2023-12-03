#define CPPHTTPLIB_OPENSSL_SUPPORT
#include "httplib.h"

#include <iostream>
#include <Windows.h>
#include <thread>
#include <nlohmann/json.hpp>
#include <vector>
#include <tuple>

const std::string domain = "https://api.neople.co.kr";
const std::string apiKey = "ZxAjZqxO9lUto8BhTeC9QE2MU8XxvWG3";
const int limit = 5;
std::vector<std::tuple<std::string, int, std::string>> items = 
{
    {"%ED%94%8C%EB%9E%98%ED%8B%B0%EB%84%98%20%EC%97%A0%EB%B8%94%EB%A0%98%5B%EA%B5%90%EA%B0%90%5D", 13000000, "교감"},
    {"%ED%94%8C%EB%9E%98%ED%8B%B0%EB%84%98%20%EC%97%A0%EB%B8%94%EB%A0%98%5B%EB%B9%88%ED%8B%B0%EC%A7%80%5D", 13000000, "빈티지"},
    {"%ED%94%8C%EB%9E%98%ED%8B%B0%EB%84%98%20%EC%97%A0%EB%B8%94%EB%A0%98%5B%ED%99%98%EC%88%98%20%ED%8F%AD%EC%A3%BC%5D", 3000000, "환수 폭주"}
};

const std::string queryBase = "/df/auction?"
                              "apikey=" + apiKey + "&"
                              "sort=unitPrice:asc&"
                              "limit=" + std::to_string(limit) + "&"
                              "itemName=";

const int beepCount = 20;

void printMessage(std::string message)
{
    std::time_t now = std::time(nullptr);
    std::tm* local_time = std::localtime(&now);

    // 시간 문자열로 변환
    char time_str[100];
    std::strftime(time_str, sizeof(time_str), "[%H:%M:%S]", local_time);

    // 출력
    std::cout << time_str << " " << message << std::endl;
}

int main()
{
    httplib::Client cli(domain);

    while (true)
    {
        for (const auto& [encodedItemName, itemPrice, itemName] : items)
        {
            std::string query = queryBase + encodedItemName;

            auto res = cli.Get(query);
            if (res && res->status == 200)
            {
                nlohmann::json body = nlohmann::json::parse(res->body);
                auto queryResult = body["rows"];
                for (int i = 0; i < queryResult.size(); i++)
                {
                    if (queryResult[i]["unitPrice"] < itemPrice)
                    {
                        printMessage(itemName + " " + std::to_string(queryResult[i]["unitPrice"].get<int>()));
                        for (int j = 0; j < beepCount; j++)
                        {
                            Beep(1000, 100);
                        }
                    }
                }
            }
            else {
                std::cerr << "Error: " << res.error() << std::endl;
            }
        }

        printMessage("Working...");

        std::this_thread::sleep_for(std::chrono::milliseconds{3000});
    }

    return 0;
}
