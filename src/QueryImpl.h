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
#include "QueryImpl.h"

namespace CppQuery{
	
	//the only purpose of struct CharsTypes is to define types from proper namespace (spirit::ascii or spirit::standard_wide)
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

	
	template<typename Str>
	struct NotMatchingAttr{
		typedef boost::shared_ptr<Node<Str> > NodePtr;
		
		NotMatchingAttr(const Str &attr_name, const Str &attr_val) : attr_name(attr_name), attr_val(attr_val){}

		bool operator() (NodePtr &node){
			return !node->has_attribute_value(attr_name, attr_val);
		}

		private:
		const Str &attr_name, attr_val;
	};
	
	template<typename Str>
	struct NotMatchingElement{
		typedef boost::shared_ptr<Node<Str> > NodePtr;
		
		NotMatchingElement(const Str &el_name) : el_name(el_name){}

		bool operator() (NodePtr &node){
			return !node->has_tag_name(el_name);
		}

		private:
		const Str &el_name;
	};

	template<typename Str>
	struct NotMatchingText{
		typedef boost::shared_ptr<Node<Str> > NodePtr;
		
		NotMatchingText(const Str &text) : text(text){}

		bool operator() (NodePtr &node){
			return !node->has_text(text);
		}

		private:
		const Str &text;
	};

	template<typename Str>
	struct HaveParentAmong{
		typedef boost::shared_ptr<Node<Str> > NodePtr;

		HaveParentAmong(const std::vector<NodePtr> &v) : v(v){}

		bool operator() (NodePtr &node){
			for(int i=0; i<v.size(); ++i)
				if(v[i] != node && node->is_descendant_of(v[i]))
					return true;
			return false;
		}
		
		private:
		const std::vector<NodePtr> &v;
	};
	
	template<typename Str>
	struct HaveNotChildrenAmong{
		typedef boost::shared_ptr<Node<Str> > NodePtr;

		HaveNotChildrenAmong(const std::vector<NodePtr> &v) : descendants_nodes(v){}

		bool operator() (NodePtr &node){
			for(int i=0; i<descendants_nodes.size(); ++i)
				if(descendants_nodes[i] != node && descendants_nodes[i]->is_descendant_of(node))
					return false;
			return true;
		}
		
		private:
		const std::vector<NodePtr> &descendants_nodes;
	};
	
	template<typename Str>
	struct IsPresent{
		typedef boost::shared_ptr<Node<Str> > NodePtr;

		IsPresent(const std::vector<NodePtr> &v) : v(v){}

		bool operator() (NodePtr &node){
			for(int i=0; i<v.size(); ++i)
				if(find(v.begin(), v.end(), node) != v.end())
					return true;
			return false;
		}
		
		private:
		const std::vector<NodePtr> &v;
	};

	template <typename Str>
	class QueryImpl{
		typedef boost::shared_ptr<Node<Str> > NodePtr;
		typedef boost::shared_ptr<QueryImpl> QueryImplPtr;

		typedef fusion::vector<Str, std::vector<fusion::vector<Str, Str> > > HtmlStartTagAttr; //each start tag consists of tag name and a number of pairs <attribute-name, attribute-value>
		typedef spirit::qi::rule<typename Str::const_iterator, fusion::vector<Str, Str>(), typename CharsTypes<Str>::space_type> HtmlAttributeRule;
		typedef spirit::qi::rule<typename Str::const_iterator, HtmlStartTagAttr(), typename CharsTypes<Str>::space_type> HtmlStartTagRule;
		typedef spirit::qi::rule<typename Str::const_iterator, Str(), typename CharsTypes<Str>::space_type> HtmlEndTagRule;
		typedef spirit::qi::rule<typename Str::const_iterator, typename CharsTypes<Str>::space_type> HtmlRule;
		typedef spirit::qi::rule<typename Str::const_iterator, Str(), typename CharsTypes<Str>::space_type> BasicSelector;

		public:
		explicit QueryImpl(const Str &html);
		Str text();
		Str get_attribute(const Str &attribute);
		bool attr_exists(const Str &attribute);
		QueryImplPtr get_ith(int index);
		QueryImplPtr select(const Str &selector);
		//works analogically to select besides of the fact that ignore roots elements when searching
		QueryImpl * search_inside(const Str &selector);
		int size(){
			return roots.top().size();
		}

		private:
		QueryImpl(){ // do not use it in client code. Useful when creating an empty object.
			std::vector<NodePtr> v;
			roots.push(v);
		} 
		QueryImpl(std::vector<NodePtr> elements){
			roots.push(elements);
		} //do not use it in client code. Useful when creating object in methods select and get_ith
		std::stack<NodePtr> open_tags;
		
		//stack of vectors because when parsing nested :not or :has we do something like function call
		//so this methods push new vector when begin and pop when return
		std::stack<std::vector<NodePtr> > roots;
		
		//the same idea as with roots
		std::stack<std::vector<NodePtr> > tmp_res;
		
		std::stack<bool> flags;

		//if set to false then some selector has been already applied in current search.
		//eg. handling selector "divspecial p": before applying "div" first_selector will be set to true to indicate that no selector has been applied so far, before applying ".special" and " p" will be set to false
		bool first_selector;
		bool descendant;
		bool child;
		
		/**
		 * Restores default values of all flags used during selector parsing
		 */
		void reset();
		
		void push_flags(){
			flags.push(child);
			flags.push(descendant);
			flags.push(first_selector);
		}
		
		void pop_flags(){
			first_selector = flags.top();
			flags.pop();
			descendant = flags.top();
			flags.pop();
			child = flags.top();
			flags.pop();
		}
		
		void get_all(std::vector<NodePtr> & v, const int flags){
			for(int i=0; i<tmp_res.top().size(); ++i)
				tmp_res.top()[i]->get_all(v, flags);
		}
		
		void get_ancestors(std::vector<NodePtr> & v){
			for(int i=0; i<tmp_res.top().size(); ++i)
				tmp_res.top()[i]->get_ancestors(v, true);
		}
		
		void get_ancestors_inside(std::vector<NodePtr> & v, std::vector<NodePtr> & res){
			for(int i=0; i<v.size(); ++i)
				v[i]->get_ancestors_inside(v, tmp_res.top(), res);
		}
		
		/**
		 * Remove nodes from vector vec that have ancestor among items of vector vec
		 */
		void remove_children(std::vector<NodePtr> &vec);

		//begin of html parsing handlers
		void handle_start_tag(HtmlStartTagAttr &tag);
		void handle_start_end_tag(HtmlStartTagAttr &tag);
		void handle_end_tag(const Str &tag);
		void handle_text(const Str &text);
		void handle_comment();
		//end of html parsing handlers
		
		//begin of selectors parsing handlers
		void handle_class(const Str &str);
		void handle_id(const Str &str);
		void handle_contains(const Str &txt);
		void handle_attr(fusion::vector<Str, Str> attr_v);
		void handle_element(const Str &el_name);
		void handle_start_not();
		void handle_end_not();
		void handle_start_has();
		void handle_end_has();
		void handle_descendant();
		void handle_child();
		//end of selectors parsing handlers
		
		void diff(const QueryImpl<Str> * other, std::vector<NodePtr> & res);
	};
}

#endif // CPP_QUERY_IMPL_H
 
