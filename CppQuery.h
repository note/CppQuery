#include <string>

namespace CppQuery{
	class QueryImpl;
	class Query{
		public:
		Query(const std::string &html);
		~Query();

		std::string text() const{}
		std::string operator [] (const std::string &attr) const {}
		Query operator [] (int) const {}
		Query operator () (const std::string &selector) const {}
		int size() const {}

		private:
		QueryImpl * pimpl;
	};
}
