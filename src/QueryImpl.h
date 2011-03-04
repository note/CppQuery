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

	template <typename Str>
	class QueryImpl{
		typedef boost::shared_ptr<Node<Str> > NodePtr;

		typedef fusion::vector<Str, std::vector<fusion::vector<Str, Str> > > HtmlStartTagAttr; //each start tag consists of tag name and some number of pairs <attribute-name, attribute-value>
		typedef spirit::qi::rule<typename Str::const_iterator, fusion::vector<Str, Str>(), spirit::ascii::space_type> HtmlAttributeRule;
		typedef spirit::qi::rule<typename Str::const_iterator, HtmlStartTagAttr(), spirit::ascii::space_type> HtmlStartTagRule;
		typedef spirit::qi::rule<typename Str::const_iterator, Str(), spirit::ascii::space_type> HtmlEndTagRule;
		typedef spirit::qi::rule<typename Str::const_iterator, spirit::ascii::space_type> HtmlRule;
	
		public:
		QueryImpl(const Str &html);
		Str text();
		Str get_attribute(const Str &attribute);
		bool attr_exists(const Str &attribute);
		QueryImpl * get_ith(int index);
		QueryImpl * select(const Str &selector);
		int size(){
			return roots.size();
		}

		private:
		QueryImpl(){} // do not use it in client code. Useful when creating empty object.
		QueryImpl(std::vector<NodePtr> elements) : roots(elements){} //do not use it in client code. Useful when creating object in methods select and get_ith
		std::stack<NodePtr> open_tags;
		std::vector<NodePtr> roots;
		
		void handle_start_tag(HtmlStartTagAttr &tag);
		void handle_start_end_tag(HtmlStartTagAttr &tag);
		void handle_end_tag(const Str &tag);
		void handle_text(const Str &text);
	};
}

#endif // CPP_QUERY_IMPL_H
 
