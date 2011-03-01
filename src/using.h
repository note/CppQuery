#ifndef CPP_QUERY_USING_H
#define CPP_QUERY_USING_H

#include <boost/spirit/include/qi.hpp>

//that file won't be included by clients, so it's not harmful.
//todo: check after build if it really does not any harm.
namespace spirit = boost::spirit;
namespace fusion = boost::fusion;

#endif //CPP_QUERY_USING_H
