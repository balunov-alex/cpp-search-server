#pragma once

#include <iostream>
#include <iterator>
#include <vector>

template <typename Iterator>
class IteratorRange {
public:
    IteratorRange(Iterator range_begin, Iterator range_end);
    
    Iterator begin() const;
    
    Iterator end() const;    
    
private:
    Iterator range_begin_;
    Iterator range_end_;
};

template <typename Iterator>
IteratorRange<Iterator>::IteratorRange(Iterator range_begin, Iterator range_end)
    : range_begin_(range_begin), range_end_(range_end)
{
}

template <typename Iterator>
Iterator IteratorRange<Iterator>::begin() const {
    return range_begin_;
}

template <typename Iterator>
Iterator IteratorRange<Iterator>::end() const {
    return range_end_;
}


template <typename Iterator>
class Paginator {
public:
    Paginator(Iterator range_begin, Iterator range_end, std::size_t page_size);
    
    std::vector<IteratorRange<Iterator>>::const_iterator begin() const;
    
    std::vector<IteratorRange<Iterator>>::const_iterator end() const;
    
private:
    std::vector<IteratorRange<Iterator>> pages_;
    
    std::vector<IteratorRange<Iterator>> CreateVectorPages(Iterator range_begin, Iterator range_end, std::size_t page_size) const;
};

template <typename Iterator>
Paginator<Iterator>::Paginator(Iterator range_begin, Iterator range_end, std::size_t page_size)
    : pages_(CreateVectorPages(range_begin, range_end, page_size))
{
}

template <typename Iterator>
std::vector<IteratorRange<Iterator>>::const_iterator Paginator<Iterator>::begin() const {
    return pages_.begin();
}

template <typename Iterator>
std::vector<IteratorRange<Iterator>>::const_iterator Paginator<Iterator>::end() const {
    return pages_.end();
}

template <typename Iterator>
std::vector<IteratorRange<Iterator>> Paginator<Iterator>::CreateVectorPages(Iterator range_begin, Iterator range_end, std::size_t page_size) const {
    std::vector<IteratorRange<Iterator>> pages;
    const auto full_page_count = std::distance(range_begin, range_end) / page_size;
    for (std::size_t i = 0; i < full_page_count; ++i) {
        pages.push_back(IteratorRange(range_begin, range_begin + page_size));
        range_begin += page_size;
    }
    if (range_begin != range_end) {
        pages.push_back(IteratorRange(range_begin, range_end));
    }
    return pages;
}


template <typename Container>
Paginator<typename Container::const_iterator> Paginate(const Container& c, std::size_t page_size);

template <typename Container>
Paginator<typename Container::const_iterator> Paginate(const Container& c, std::size_t page_size) {
    return Paginator<typename Container::const_iterator>(std::begin(c), std::end(c), page_size);
}


template <typename Iterator>
std::ostream& operator<<(std::ostream& output, IteratorRange<Iterator> page);

template <typename Iterator>
std::ostream& operator<<(std::ostream& output, IteratorRange<Iterator> page) {
    for (auto it = page.begin(); it != page.end(); ++it) {
        output << *it;
    }
    return output;
}
