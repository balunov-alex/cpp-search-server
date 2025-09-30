#include "search_server.h"

#include <cmath>
#include <numeric>

SearchServer::SearchServer(const std::string& stop_words_text)
    : SearchServer(SplitIntoWords(stop_words_text))
{
}
  
void SearchServer::AddDocument(int document_id, const std::string& document, DocumentStatus status, const std::vector<int>& ratings) {
    if (document_id < 0) {
        throw std::invalid_argument(std::string("Attempt to add a document with a negative id"));
    }
        
    if (documents_.count(document_id)) {
        throw std::invalid_argument(std::string("Attempt to add a document with the id of a previously added document"));
    }
        
    if (CheckPresenceControlCharacter(document)) {
        throw std::invalid_argument(std::string("Invalid characters in the document text"));
    }
        
    ids_in_the_order_of_adding_documents_.push_back(document_id);
        
    const std::vector<std::string> words = SplitIntoWordsNoStop(document);
    const double inv_word_count = 1.0 / words.size();
    for (const std::string& word : words) {
        word_to_document_freqs_[word][document_id] += inv_word_count;
    }
    documents_.emplace(document_id, DocumentData{ComputeAverageRating(ratings), status});
}

std::vector<Document> SearchServer::FindTopDocuments(const std::string& raw_query, DocumentStatus status) const {
    return FindTopDocuments(raw_query, [status](int document_id, DocumentStatus document_status, int rating) {
                                           return document_status == status;
                                       });
}

std::vector<Document> SearchServer::FindTopDocuments(const std::string& raw_query) const {
    return FindTopDocuments(raw_query, DocumentStatus::ACTUAL);
}

int SearchServer::GetDocumentCount() const {
    return documents_.size();
}
    
int SearchServer::GetDocumentId(int index) const {
    return ids_in_the_order_of_adding_documents_.at(index);
}    

std::tuple<std::vector<std::string>, DocumentStatus> SearchServer::MatchDocument(const std::string& raw_query, int document_id) const {
    if (CheckPresenceControlCharacter(raw_query)) {
        throw std::invalid_argument(std::string("Invalid characters in the search query words"));
    }
                
    const Query query = ParseQuery(raw_query);
              
    std::vector<std::string> matched_words;
    for (const std::string& word : query.plus_words) {
        if (word_to_document_freqs_.count(word) == 0) {
            continue;
        }
        if (word_to_document_freqs_.at(word).count(document_id)) {
            matched_words.push_back(word);
        }
    }
    for (const std::string& word : query.minus_words) {
        if (word_to_document_freqs_.count(word) == 0) {
            continue;
        }
        if (word_to_document_freqs_.at(word).count(document_id)) {
            matched_words.clear();
            break;
        }
    }
    return {matched_words, documents_.at(document_id).status};
}    

bool SearchServer::IsStopWord(const std::string& word) const {
    return stop_words_.count(word) > 0;
}

std::vector<std::string> SearchServer::SplitIntoWordsNoStop(const std::string& text) const {
    std::vector<std::string> words;
    for (const std::string& word : SplitIntoWords(text)) {
        if (!IsStopWord(word)) {
            words.push_back(word);
        }
    }
    return words;
}

int SearchServer::ComputeAverageRating(const std::vector<int>& ratings) {
    if (ratings.empty()) {
        return 0;
    }
    return std::accumulate(ratings.begin(), ratings.end(), 0) / static_cast<int>(ratings.size());
}

SearchServer::QueryWord SearchServer::ParseQueryWord(std::string text) const {
    if (text == std::string("-")) {
        throw std::invalid_argument(std::string("No word after the \"minus\" symbol in the search query"));
    }
    bool is_minus = false;
    if (text[0] == '-') {
        text = text.substr(1);
        if (text[0] == '-') {
            throw std::invalid_argument(std::string("More than one minus sign before words that must not be in the required documents")); 
        }
        is_minus = true;
    } 
    return {text, is_minus, IsStopWord(text)};
}

SearchServer::Query SearchServer::ParseQuery(const std::string& text) const {
    Query query;
    for (const std::string& word : SplitIntoWords(text)) {
        const QueryWord query_word = ParseQueryWord(word);
        if (!query_word.is_stop) {
            if (query_word.is_minus) {
                query.minus_words.insert(query_word.data);
            } else {
                query.plus_words.insert(query_word.data);
            }
        }
    }
    return query;
}
    
bool SearchServer::CheckPresenceControlCharacter(const std::string& text) {
    for (const char c : text) {
        if (c >= '\0' && c < ' ') {
            return true;
        }
    }
    return false;
}
    
double SearchServer::ComputeWordInverseDocumentFreq(const std::string& word) const {
    return std::log(GetDocumentCount() * 1.0 / word_to_document_freqs_.at(word).size());
}
