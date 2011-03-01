#ifndef CPP_QUERY_IMPL_H
#define CPP_QUERY_IMPL_H


#include <string>
#include <stack>

#include <boost/spirit/include/qi.hpp>

#include <boost/fusion/container/vector.hpp>
#include <boost/fusion/sequence.hpp>
#include <boost/fusion/iterator.hpp>
#include <boost/fusion/algorithm.hpp>

#include "using.h"
//#include "Node.h"

namespace CppQuery{
	//that file won't be included by clients, so it's not harmful.
	//todo: check after build if it really does not any harm.
// 	namespace spirit = boost::spirit;
// 	namespace fusion = boost::fusion;
// 	using namespace CppQuery;
// 	using std::string;
// 	using std::vector;
// 	using std::stack;

	class QueryImpl{
		//typedef boost::shared_ptr<Node> NodePtr;

		typedef fusion::vector<string, vector<fusion::vector<string, string> > > HtmlStartTagAttr; //each start tag consists of tag name and some number of pairs <attribute-name, attribute-value>
		typedef spirit::qi::rule<string::const_iterator, fusion::vector<string, string>(), spirit::ascii::space_type> HtmlAttributeRule;
		typedef spirit::qi::rule<string::const_iterator, HtmlStartTagAttr(), spirit::ascii::space_type> HtmlStartTagRule;
		typedef spirit::qi::rule<string::const_iterator, string(), spirit::ascii::space_type> HtmlEndTagRule;
		typedef spirit::qi::rule<string::const_iterator, spirit::ascii::space_type> HtmlRule;
	
		public:
		QueryImpl(const string &html);
		void handle_start_tag(HtmlStartTagAttr &tag);
		void handle_start_end_tag(HtmlStartTagAttr &tag);
		void handle_end_tag(const string &tag);

		private:
	//	stack<NodePtr> open_tags;
	};
}

#endif // CPP_QUERY_IMPL_H
 
