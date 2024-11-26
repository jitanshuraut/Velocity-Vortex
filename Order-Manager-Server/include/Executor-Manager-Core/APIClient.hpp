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

  /**
   * @brief Callback function for writing received data.
   * @param contents Pointer to the received data.
   * @param size Size of a single data element.
   * @param nmemb Number of data elements.
   * @param userp Pointer to the user data.
   * @return Total size of the data written.
   */
  static size_t WriteCallback(void* contents, size_t size, size_t nmemb, std::string* userp) {
    size_t totalSize = size * nmemb;
    userp->append((char*)contents, totalSize);
    return totalSize;
  }

  /**
   * @brief Initialize CURL handles.
   */
  void initHandles() {
    for (size_t i = 0; i < maxHandles; ++i) {
      CURL* hnd = curl_easy_init();
      if (hnd) {
        handles.push_back(hnd);
      }
    }
  }

  /**
   * @brief Cleanup CURL handles.
   */
  void cleanupHandles() {
    for (CURL* hnd : handles) {
      curl_easy_cleanup(hnd);
    }
    handles.clear();
  }

  /**
   * @brief Perform an HTTP request.
   * @param hnd CURL handle.
   * @param method HTTP method (GET, POST, DELETE).
   * @param url URL for the request.
   * @param jsonPayload JSON payload for POST requests.
   * @return Pair of CURLcode and response string.
   */
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
  /**
   * @brief Constructor for APIClient.
   * @param apiKey API key for authentication.
   * @param secretKey Secret key for authentication.
   * @param maxHandles Maximum number of CURL handles.
   */
  APIClient(const std::string& apiKey, const std::string& secretKey, size_t maxHandles = 4)
      : apiKey(apiKey), secretKey(secretKey), maxHandles(maxHandles) {
    initHandles();
  }

  /**
   * @brief Destructor for APIClient.
   */
  ~APIClient() {
    cleanupHandles();
  }

  /**
   * @brief Perform a POST request.
   * @param url URL for the request.
   * @param jsonPayload JSON payload for the request.
   * @return Pair of CURLcode and response string.
   */
  std::pair<CURLcode, std::string> post(const std::string& url, const std::string& jsonPayload) {
    if (handles.empty()) return {CURLE_FAILED_INIT, ""};
    CURL* hnd = handles.back();
    handles.pop_back();
    auto result = performRequest(hnd, "POST", url, jsonPayload);
    handles.push_back(hnd);
    return result;
  }

  /**
   * @brief Perform a GET request.
   * @param url URL for the request.
   * @return Pair of CURLcode and response string.
   */
  std::pair<CURLcode, std::string> get(const std::string& url) {
    if (handles.empty()) return {CURLE_FAILED_INIT, ""};
    CURL* hnd = handles.back();
    handles.pop_back();
    auto result = performRequest(hnd, "GET", url, "");
    handles.push_back(hnd);
    return result;
  }

  /**
   * @brief Perform a DELETE request.
   * @param url URL for the request.
   * @return Pair of CURLcode and response string.
   */
  std::pair<CURLcode, std::string> del(const std::string& url) {
    if (handles.empty()) return {CURLE_FAILED_INIT, ""};
    CURL* hnd = handles.back();
    handles.pop_back();
    auto result = performRequest(hnd, "DELETE", url, "");
    handles.push_back(hnd);
    return result;
  }
};

#endif  // APICLIENT_HPP
