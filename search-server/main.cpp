#include "read_input_functions.h"
#include "string_processing.h"
#include "document.h"
#include "search_server.h"
#include "paginator.h"
#include "request_queue.h"

#include <iostream>
#include <string>

//using namespace std; сугубо для того, чтобы решение прошло тесты в тренажере
using namespace std;

int main() {
    {
        SearchServer search_server(std::string("and in at"));
        RequestQueue request_queue(search_server);
        search_server.AddDocument(1, std::string("curly cat curly tail"), DocumentStatus::ACTUAL, {7, 2, 7});
        search_server.AddDocument(2, std::string("curly dog and fancy collar"), DocumentStatus::ACTUAL, {1, 2, 3});
        search_server.AddDocument(3, std::string("big cat fancy collar"), DocumentStatus::ACTUAL, {1, 2, 8});
        search_server.AddDocument(4, std::string("big dog sparrow Eugene"), DocumentStatus::ACTUAL, {1, 3, 2});
        search_server.AddDocument(5, std::string("big dog sparrow Vasiliy"), DocumentStatus::ACTUAL, {1, 1, 1});
        // 1439 запросов с нулевым результатом
        for (int i = 0; i < 1439; ++i) {
            request_queue.AddFindRequest(std::string("empty request"));
        }
        // все еще 1439 запросов с нулевым результатом
        request_queue.AddFindRequest(std::string("curly dog"));
        // новые сутки, первый запрос удален, 1438 запросов с нулевым результатом
        request_queue.AddFindRequest(std::string("big collar"));
        // первый запрос удален, 1437 запросов с нулевым результатом
        request_queue.AddFindRequest(std::string("sparrow"));
        std::cout << std::string("Total empty requests: ") << request_queue.GetNoResultRequests() << std::endl;
    }
    
    std::cout << std::endl;
    
    {
        SearchServer search_server(std::string("and with"));
        search_server.AddDocument(1, std::string("funny pet and nasty rat"), DocumentStatus::ACTUAL, {7, 2, 7});
        search_server.AddDocument(2, std::string("funny pet with curly hair"), DocumentStatus::ACTUAL, {1, 2, 3});
        search_server.AddDocument(3, std::string("big cat nasty hair"), DocumentStatus::ACTUAL, {1, 2, 8});
        search_server.AddDocument(4, std::string("big dog cat Vladislav"), DocumentStatus::ACTUAL, {1, 3, 2});
        search_server.AddDocument(5, std::string("big dog hamster Borya"), DocumentStatus::ACTUAL, {1, 1, 1});
   
        const auto search_results = search_server.FindTopDocuments(std::string("curly dog"));
        int page_size = 2;
        const auto pages = Paginate(search_results, page_size);
        // Выводим найденные документы по страницам
        for (auto page = pages.begin(); page != pages.end(); ++page) {
            std::cout << *page << std::endl;
            std::cout << std::string("Page break") << std::endl;
        }
    }
    
    return 0;
}
