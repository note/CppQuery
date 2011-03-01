#ifndef CPP_QUERY_IMPL_H
#define CPP_QUERY_IMPL_H


#include <string>
#include <stack>

#include <boost/fusion/container/vector.hpp>
#include <boost/fusion/sequence.hpp>
#include <boost/fusion/iterator.hpp>
#include <boost/fusion/algorithm.hpp>

#include "using.h"
#include "Node.h"

namespace CppQuery{
	class QueryImpl{
		typedef boost::shared_ptr<Node> NodePtr;

		typedef fusion::vector<std::string, std::vector<fusion::vector<std::string, std::string> > > HtmlStartTagAttr; //each start tag consists of tag name and some number of pairs <attribute-name, attribute-value>
		typedef spirit::qi::rule<std::string::const_iterator, fusion::vector<std::string, std::string>(), spirit::ascii::space_type> HtmlAttributeRule;
		typedef spirit::qi::rule<std::string::const_iterator, HtmlStartTagAttr(), spirit::ascii::space_type> HtmlStartTagRule;
		typedef spirit::qi::rule<std::string::const_iterator, std::string(), spirit::ascii::space_type> HtmlEndTagRule;
		typedef spirit::qi::rule<std::string::const_iterator, spirit::ascii::space_type> HtmlRule;
	
		public:
		QueryImpl(const std::string &html);

		private:
		NodePtr root;
		std::stack<NodePtr> open_tags;
		
		void handle_start_tag(HtmlStartTagAttr &tag);
		void handle_start_end_tag(HtmlStartTagAttr &tag);
		void handle_end_tag(const std::string &tag);
	};
}

#endif // CPP_QUERY_IMPL_H
 
