#ifndef CPP_QUERY_QUERY_H
#define CPP_QUERY_QUERY_H

#include <string>

namespace CppQuery{
	class QueryImpl;
	class Query{
		public:
		Query(const std::string &html);
		~Query();

		std::string text() const;
		std::string operator [] (const std::string &attr) const;
		bool attr_exists(const std::string &attr) const;
		Query operator [] (int) const;
		Query operator () (const std::string &selector) const;
		int size() const;

		private:
		Query(QueryImpl *); //do not use it in client code
		QueryImpl *pimpl;
	};
}

#endif //CPP_QUERY_QUERY_H
