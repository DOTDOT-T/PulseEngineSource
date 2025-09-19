#include "Account.h"
#include "PulseEngine/core/Network/Request/Request.h"

Account::Account(const std::string& mail, const std::string& password)
{
    Request host("localhost", "3000");
    nlohmann::json body;
    body["mail"] = mail;
    body["pass"] = password;
    nlohmann::json connectionResult = host.Post("/auth/ConnectUser", body);
    id = connectionResult.get<std::string>();

    nlohmann::json userData = host.Get("/Users/" + id);
    name = userData["name"];
    email = userData["email"];

}