#ifndef APICLIENT_HPP
#define APICLIENT_HPP

#include <string>
#include <curl/curl.h>
#include <iostream>
#include <sstream>

class APIClient {
private:
    std::string apiKey;
    std::string secretKey;
    CURL* hnd;
    static size_t WriteCallback(void* contents, size_t size, size_t nmemb, std::string* userp) {
        size_t totalSize = size * nmemb;
        userp->append((char*)contents, totalSize);
        return totalSize;
    }

public:
    APIClient(const std::string& apiKey, const std::string& secretKey) 
        : apiKey(apiKey), secretKey(secretKey) {
        std::cout<<apiKey<<std::endl;    
        hnd = curl_easy_init();
    }

    ~APIClient() {
        if (hnd) {
            curl_easy_cleanup(hnd);
        }
    }

    void setHeaders(struct curl_slist*& headers) {
        headers = curl_slist_append(headers, "accept: application/json");
        headers = curl_slist_append(headers, "content-type: application/json");
        headers = curl_slist_append(headers, ("APCA-API-KEY-ID: " + apiKey).c_str());
        headers = curl_slist_append(headers, ("APCA-API-SECRET-KEY: " + secretKey).c_str());
    }

    // Updated post method
    std::pair<CURLcode, std::string> post(const std::string& url, const std::string& jsonPayload) {
        struct curl_slist* headers = NULL;
        setHeaders(headers);
        
        std::string responseString;  // To store the response
        curl_easy_setopt(hnd, CURLOPT_CUSTOMREQUEST, "POST");
        curl_easy_setopt(hnd, CURLOPT_URL, url.c_str());
        curl_easy_setopt(hnd, CURLOPT_HTTPHEADER, headers);
        curl_easy_setopt(hnd, CURLOPT_POSTFIELDS, jsonPayload.c_str());
        curl_easy_setopt(hnd, CURLOPT_WRITEFUNCTION, WriteCallback);  
        curl_easy_setopt(hnd, CURLOPT_WRITEDATA, &responseString);  

        CURLcode ret = curl_easy_perform(hnd);
        curl_slist_free_all(headers);
        return {ret, responseString};  
    }

    // Updated get method
    std::pair<CURLcode, std::string> get(const std::string& url) {
        struct curl_slist* headers = NULL;
        setHeaders(headers);

        std::string responseString; 
        curl_easy_setopt(hnd, CURLOPT_CUSTOMREQUEST, "GET");
        curl_easy_setopt(hnd, CURLOPT_URL, url.c_str());
        curl_easy_setopt(hnd, CURLOPT_HTTPHEADER, headers);
        curl_easy_setopt(hnd, CURLOPT_WRITEFUNCTION, WriteCallback);  
        curl_easy_setopt(hnd, CURLOPT_WRITEDATA, &responseString); 

        CURLcode ret = curl_easy_perform(hnd);
        curl_slist_free_all(headers);
        return {ret, responseString}; 
    }

    // Updated del method
    std::pair<CURLcode, std::string> del(const std::string& url) {
        struct curl_slist* headers = NULL;
        setHeaders(headers);

        std::string responseString;  
        curl_easy_setopt(hnd, CURLOPT_CUSTOMREQUEST, "DELETE");
        curl_easy_setopt(hnd, CURLOPT_URL, url.c_str());
        curl_easy_setopt(hnd, CURLOPT_HTTPHEADER, headers);
        curl_easy_setopt(hnd, CURLOPT_WRITEFUNCTION, WriteCallback);  
        curl_easy_setopt(hnd, CURLOPT_WRITEDATA, &responseString);  

        CURLcode ret = curl_easy_perform(hnd);
        curl_slist_free_all(headers);
        return {ret, responseString};  
    }
};

#endif