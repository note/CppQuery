#ifndef CPP_QUERY_QUERY_H
#define CPP_QUERY_QUERY_H

#include <boost/shared_ptr.hpp>

namespace CppQuery{
	template<typename Str>
	class QueryImpl;
	
	template<typename Str>
	class Query{
		typedef boost::shared_ptr<QueryImpl<Str> > QueryImplPtr;
		public:
		explicit Query(const Str &html);
		Query(){}
		
		void load(const Str &html);
		Str text() const;
		Str operator [] (const Str &attr) const;
		bool attr_exists(const Str &attr) const;
		Query operator [] (int) const;
		Query operator () (const Str &selector) const;
		int size() const;

		private:
		Query(QueryImplPtr impl); //do not use it in client code
		QueryImplPtr pimpl;
	};
}

#endif //CPP_QUERY_QUERY_H
