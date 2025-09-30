#pragma once

#include "document.h"
#include "search_server.h"

#include <deque>
#include <string>
#include <vector>

class RequestQueue {
public:
    explicit RequestQueue(const SearchServer& search_server);
    
    template <typename DocumentPredicate>
    std::vector<Document> AddFindRequest(const std::string& raw_query, DocumentPredicate document_predicate);
    
    std::vector<Document> AddFindRequest(const std::string& raw_query, DocumentStatus status);
    
    std::vector<Document> AddFindRequest(const std::string& raw_query);
    
    int GetNoResultRequests() const;
    
private:
    const SearchServer& search_server_;
    
    struct QueryResult {
        std::string query;
        std::size_t number_of_results;
    };
    std::deque<QueryResult> requests_;
    
    int unsuccessful_requests_ = 0;
    int current_time_ = 0;
    const static int min_in_day_ = 1440;
};

template <typename DocumentPredicate>
std::vector<Document> RequestQueue::AddFindRequest(const std::string& raw_query, DocumentPredicate document_predicate) {
    ++current_time_;
    if (current_time_ > min_in_day_) {
        if (requests_.back().number_of_results == 0) {
            --unsuccessful_requests_;
        }
        requests_.pop_back();      
    }
    const auto results = search_server_.FindTopDocuments(raw_query, document_predicate);
    requests_.push_front({raw_query, results.size()});
    if (results.size() == 0) {
        ++unsuccessful_requests_;
    }
    return results;
}
