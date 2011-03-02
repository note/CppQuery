#include "../CppQuery.h"
#include "QueryImpl.h"

using namespace CppQuery;
using namespace std;

Query::Query(const string &html){
	pimpl = new QueryImpl(html);
}

Query::Query(QueryImpl * impl) : pimpl(impl){}

Query::~Query(){
	delete pimpl;
}

string Query::text() const {
	return pimpl->text();
}

string Query::operator [](const string &attr) const {
	return pimpl->get_attribute(attr);
}

bool Query::attr_exists(const string &attr) const {
	return pimpl->attr_exists(attr);
}

Query Query::operator [](int index) const {
	return Query(pimpl->get_ith(index));
}
		
Query Query::operator () (const string &selector) const {
	return Query(pimpl->select(selector));
}

int Query::size() const {
	return pimpl->size();
}
