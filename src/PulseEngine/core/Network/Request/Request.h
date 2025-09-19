/**
 * @file Request.h
 * @author Dorian LEXTERIAQUE (dlexteriaque@gmail.com)
 * @brief Request class is a REST client design for web communication with a server.
 * @version 0.1
 * @date 2025-09-14
 * 
 * @copyright Copyright (c) 2025
 * 
 */
#ifndef REQUEST_H
#define REQUEST_H

#define ASIO_STANDALONE
#include "pch.h"
class Request : public asio::ip::tcp::iostream
{
    public:
        Request(const std::string& ip, const std::string& port);

        nlohmann::json Get(
            const std::string& route,
            const std::map<std::string, std::string>& queryParams = {},
            const std::map<std::string, std::string>& headers = {}
        );

        nlohmann::json Post(
            const std::string& route,
            const nlohmann::json& body,
            const std::map<std::string, std::string>& headers = {}
        );

        
        nlohmann::json Put(const std::string& route,
                           const nlohmann::json& body,
                           const std::map<std::string,std::string>& headers = {});

        nlohmann::json Delete(const std::string& route,
                              const std::map<std::string,std::string>& headers = {});
    private:
    std::string ip;
    std::string port;
    
    std::string buildQuery(const std::map<std::string,std::string>& query);
    nlohmann::json sendRequest(const std::string& method,
                               const std::string& route,
                               const std::string& body,
                               const std::map<std::string,std::string>& headers);
};

#endif 