#include "../CppQuery.h"
#include "QueryImpl.h"

using namespace CppQuery;
using namespace std;

extern template class QueryImpl<string>;
extern template class QueryImpl<wstring>;

template <typename Str>
Query<Str>::Query(const Str &html){
	pimpl = new QueryImpl<Str>(html);
}

template <typename Str>
Query<Str>::Query(QueryImpl<Str> * impl) : pimpl(impl){}

template <typename Str>
Query<Str>::~Query(){
	delete pimpl;
}

template <typename Str>
Str Query<Str>::text() const {
	return pimpl->text();
}

template <typename Str>
Str Query<Str>::operator [](const Str &attr) const {
	return pimpl->get_attribute(attr);
}

template <typename Str>
bool Query<Str>::attr_exists(const Str &attr) const {
	return pimpl->attr_exists(attr);
}

template <typename Str>
Query<Str> Query<Str>::operator [](int index) const {
	return Query(pimpl->get_ith(index));
}
		
template <typename Str>
Query<Str> Query<Str>::operator () (const Str &selector) const {
	return Query(pimpl->select(selector));
}

template <typename Str>
int Query<Str>::size() const {
	return pimpl->size();
}

template class CppQuery::Query<string>;
template class CppQuery::Query<wstring>;
