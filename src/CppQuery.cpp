#include "../CppQuery.h"
#include "QueryImpl.h"

using namespace CppQuery;

Query::Query(const std::string &html){
	pimpl = new QueryImpl(html);
}

Query::~Query(){
	delete pimpl;
}
