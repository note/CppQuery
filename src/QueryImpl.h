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
	
	//the only purpose of struct CharsTypes is to define types from proper namespace.
	//types from namespace spirit:ascii for std::string
	//types from namespace spirit::standard_wide for std::wstring
	template <typename Str>
	struct CharsTypes{};

	template <>
	struct CharsTypes<std::string>{
		typedef spirit::ascii::space_type space_type;
		typedef spirit::ascii::alpha_type alpha_type;
		typedef spirit::ascii::alnum_type alnum_type;
		typedef spirit::ascii::char_type char_type;
	};

	template <>
	struct CharsTypes<std::wstring>{
		typedef spirit::standard_wide::space_type space_type;
		typedef spirit::standard_wide::alpha_type alpha_type;
		typedef spirit::standard_wide::alnum_type alnum_type;
		typedef spirit::standard_wide::char_type char_type;
	};
	
	template <typename Str>
	struct Chars{
		static typename CharsTypes<Str>::space_type space;
		static typename CharsTypes<Str>::alpha_type alpha;
		static typename CharsTypes<Str>::alnum_type alnum;
		static typename CharsTypes<Str>::char_type char_;
	};

	//static members initialization:
	template <typename Str> typename CharsTypes<Str>::space_type Chars<Str>::space = typename CharsTypes<Str>::space_type();
	template <typename Str> typename CharsTypes<Str>::alpha_type Chars<Str>::alpha = typename CharsTypes<Str>::alpha_type();
	template <typename Str> typename CharsTypes<Str>::alnum_type Chars<Str>::alnum = typename CharsTypes<Str>::alnum_type();
	template <typename Str> typename CharsTypes<Str>::char_type Chars<Str>::char_ = typename CharsTypes<Str>::char_type();
	//end of static members initialization

	template <typename Str>
	class QueryImpl{
		typedef boost::shared_ptr<Node<Str> > NodePtr;

		typedef fusion::vector<Str, std::vector<fusion::vector<Str, Str> > > HtmlStartTagAttr; //each start tag consists of tag name and some number of pairs <attribute-name, attribute-value>
		typedef spirit::qi::rule<typename Str::const_iterator, fusion::vector<Str, Str>(), typename CharsTypes<Str>::space_type> HtmlAttributeRule;
		typedef spirit::qi::rule<typename Str::const_iterator, HtmlStartTagAttr(), typename CharsTypes<Str>::space_type> HtmlStartTagRule;
		typedef spirit::qi::rule<typename Str::const_iterator, Str(), typename CharsTypes<Str>::space_type> HtmlEndTagRule;
		typedef spirit::qi::rule<typename Str::const_iterator, typename CharsTypes<Str>::space_type> HtmlRule;
		//typedef spirit::qi::rule<typename Str::const_iterator, spirit::staBasicSelector;

		public:
		explicit QueryImpl(const Str &html);
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
 
