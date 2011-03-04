#ifndef CPP_QUERY_QUERY_H
#define CPP_QUERY_QUERY_H

namespace CppQuery{
	template<typename Str>
	class QueryImpl;
	
	template<typename Str>
	class Query{
		public:
		explicit Query(const Str &html);
		~Query();

		Str text() const;
		Str operator [] (const Str &attr) const;
		bool attr_exists(const Str &attr) const;
		Query operator [] (int) const;
		Query operator () (const Str &selector) const;
		int size() const;

		private:
		Query(QueryImpl<Str> *); //do not use it in client code
		QueryImpl<Str> *pimpl;
	};
}

#endif //CPP_QUERY_QUERY_H
