#include "Request.h"

Request::Request(const std::string& ip, const std::string& port)
: asio::ip::tcp::iostream(ip, port), ip(ip), port(port) {
    if (!*this) {
        std::cout << "Could not connect to " + ip + ":" + port << std::endl;
    }
}

std::string Request::buildQuery(const std::map<std::string,std::string>& query) {
    if (query.empty()) return "";
    std::string q = "?";
    for (auto it = query.begin(); it != query.end(); ++it) {
        if (it != query.begin()) q += "&";
        q += it->first + "=" + it->second;
    }
    return q;
}

nlohmann::json Request::sendRequest(const std::string& method,
                                    const std::string& route,
                                    const std::string& body,
                                    const std::map<std::string,std::string>& headers) {
    // clear old buffer
    this->clear();

    // send request
    *this << method << " " << route << " HTTP/1.1\r\n";
    *this << "Host: " << ip << "\r\n";
    *this << "Connection: keep-alive\r\n";

    for (auto& kv : headers) {
        *this << kv.first << ": " << kv.second << "\r\n";
    }

    if (!body.empty()) {
        *this << "Content-Type: application/json\r\n";
        *this << "Content-Length: " << body.size() << "\r\n";
    }

    *this << "\r\n";

    if (!body.empty()) {
        *this << body;
    }

    // --- read status line ---
    std::string statusLine;
    std::getline(*this, statusLine);

    // --- read headers ---
    std::map<std::string, std::string> respHeaders;
    std::string line;
    while (std::getline(*this, line) && line != "\r") {
        auto pos = line.find(":");
        if (pos != std::string::npos) {
            std::string key = line.substr(0, pos);
            std::string value = line.substr(pos + 1);
            // trim
            key.erase(key.find_last_not_of(" \r\n") + 1);
            value.erase(0, value.find_first_not_of(" \t\r\n"));
            value.erase(value.find_last_not_of(" \r\n") + 1);
            respHeaders[key] = value;
        }
    }

    // --- read body ---
    std::string responseBody;

    if (respHeaders["Transfer-Encoding"] == "chunked") {
        // handle chunked
        while (true) {
            std::string chunkSizeStr;
            std::getline(*this, chunkSizeStr);
            int chunkSize = std::stoi(chunkSizeStr, nullptr, 16);
            if (chunkSize == 0) break;

            std::string chunk(chunkSize, '\0');
            this->read(&chunk[0], chunkSize);
            responseBody += chunk;

            // eat trailing CRLF
            std::getline(*this, line);
        }
    } else if (respHeaders.count("Content-Length")) {
        int contentLength = std::stoi(respHeaders["Content-Length"]);
        std::string buf(contentLength, '\0');
        this->read(&buf[0], contentLength);
        responseBody = buf;
    } else {
        // read until EOF
        std::ostringstream oss;
        oss << this->rdbuf();
        responseBody = oss.str();
    }

    // --- parse JSON ---
    if (!responseBody.empty()) {
        return nlohmann::json::parse(responseBody);
    }

    return nlohmann::json(); 
}


nlohmann::json Request::Get(const std::string& route,
                            const std::map<std::string,std::string>& query,
                            const std::map<std::string,std::string>& headers) {
    return sendRequest("GET", route + buildQuery(query), "", headers);
}

nlohmann::json Request::Post(const std::string& route,
                             const nlohmann::json& body,
                             const std::map<std::string,std::string>& headers) {
    return sendRequest("POST", route, body.dump(), headers);
}

nlohmann::json Request::Put(const std::string& route,
                            const nlohmann::json& body,
                            const std::map<std::string,std::string>& headers) {
    return sendRequest("PUT", route, body.dump(), headers);
}

nlohmann::json Request::Delete(const std::string& route,
                               const std::map<std::string,std::string>& headers) {
    return sendRequest("DELETE", route, "", headers);
}
