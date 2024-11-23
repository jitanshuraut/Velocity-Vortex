#ifndef APICLIENT_HPP
#define APICLIENT_HPP

#include <string>
#include <curl/curl.h>
#include <iostream>
#include <sstream>
#include <vector>

class APIClient {
private:
    std::string apiKey;
    std::string secretKey;
    std::vector<CURL*> handles;
    size_t maxHandles;

    static size_t WriteCallback(void* contents, size_t size, size_t nmemb, std::string* userp) {
        size_t totalSize = size * nmemb;
        userp->append((char*)contents, totalSize);
        return totalSize;
    }

    void initHandles() {
        for (size_t i = 0; i < maxHandles; ++i) {
            CURL* hnd = curl_easy_init();
            if (hnd) {
                handles.push_back(hnd);
            }
        }
    }

    void cleanupHandles() {
        for (CURL* hnd : handles) {
            curl_easy_cleanup(hnd);
        }
        handles.clear();
    }

    std::pair<CURLcode, std::string> performRequest(CURL* hnd, const std::string& method, const std::string& url, const std::string& jsonPayload) {
        struct curl_slist* headers = NULL;
        headers = curl_slist_append(headers, "accept: application/json");
        headers = curl_slist_append(headers, "content-type: application/json");
        headers = curl_slist_append(headers, ("APCA-API-KEY-ID: " + apiKey).c_str());
        headers = curl_slist_append(headers, ("APCA-API-SECRET-KEY: " + secretKey).c_str());

        std::string responseString;
        curl_easy_setopt(hnd, CURLOPT_HTTPHEADER, headers);
        curl_easy_setopt(hnd, CURLOPT_WRITEFUNCTION, WriteCallback);
        curl_easy_setopt(hnd, CURLOPT_WRITEDATA, &responseString);

        if (method == "POST") {
            curl_easy_setopt(hnd, CURLOPT_CUSTOMREQUEST, "POST");
            curl_easy_setopt(hnd, CURLOPT_URL, url.c_str());
            curl_easy_setopt(hnd, CURLOPT_POSTFIELDS, jsonPayload.c_str());
        } else if (method == "GET") {
            curl_easy_setopt(hnd, CURLOPT_CUSTOMREQUEST, "GET");
            curl_easy_setopt(hnd, CURLOPT_URL, url.c_str());
        } else if (method == "DELETE") {
            curl_easy_setopt(hnd, CURLOPT_CUSTOMREQUEST, "DELETE");
            curl_easy_setopt(hnd, CURLOPT_URL, url.c_str());
        }

        CURLcode ret = curl_easy_perform(hnd);
        curl_slist_free_all(headers);
        return {ret, responseString};
    }

public:
    APIClient(const std::string& apiKey, const std::string& secretKey, size_t maxHandles = 4) 
        : apiKey(apiKey), secretKey(secretKey), maxHandles(maxHandles) {
        initHandles();
    }

    ~APIClient() {
        cleanupHandles();
    }

    std::pair<CURLcode, std::string> post(const std::string& url, const std::string& jsonPayload) {
        if (handles.empty()) return {CURLE_FAILED_INIT, ""};
        CURL* hnd = handles.back();
        handles.pop_back();
        auto result = performRequest(hnd, "POST", url, jsonPayload);
        handles.push_back(hnd);
        return result;  
    }

    std::pair<CURLcode, std::string> get(const std::string& url) {
        if (handles.empty()) return {CURLE_FAILED_INIT, ""};
        CURL* hnd = handles.back();
        handles.pop_back();
        auto result = performRequest(hnd, "GET", url, "");
        handles.push_back(hnd);
        return result; 
    }

    std::pair<CURLcode, std::string> del(const std::string& url) {
        if (handles.empty()) return {CURLE_FAILED_INIT, ""};
        CURL* hnd = handles.back();
        handles.pop_back();
        auto result = performRequest(hnd, "DELETE", url, "");
        handles.push_back(hnd);
        return result;  
    }
};

#endif
