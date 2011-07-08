// vim: fileencoding=utf-8 :
#include <gtest/gtest.h>
#include "../CppQuery.h"
#include <boost/locale.hpp>
#include <sstream>
#include <fstream>
#include <string>

using namespace std;

const string real_file = "pyquery.html";
const string simple_file = "simple.html";
const string open_tags_file = "open_tags.html";

class CppQueryTest : public ::testing::Test{
	public:
	CppQueryTest(const string &fileName){
		boost::locale::generator gen;
		locale loc = gen("pl.UTF-8");
		locale::global(loc);
		
		wcout.imbue(loc);
		ios_base::sync_with_stdio(false);
	    
		wifstream s(fileName.c_str());
		wstringstream ss;
		ss << s.rdbuf();
		html = ss.str();
		cq.load(html);
	}

	protected:
	wstring html;
	CppQuery::Query<wstring> cq;
};

class SimpleCase : public CppQueryTest{
	public:
	SimpleCase() : CppQueryTest(simple_file) {}
};

class RealCase : public CppQueryTest{
	public:
	RealCase() : CppQueryTest(real_file){}
};

class OpenTagsCase : public CppQueryTest{
	public:
	OpenTagsCase() : CppQueryTest(open_tags_file){}
};

TEST_F(SimpleCase, Selectors){
	//should return 5 because one paragraph is inside comment
	EXPECT_EQ(5, cq(L"p").size());
	EXPECT_EQ(5, cq(L"P").size());
	
	//:contains
	EXPECT_STREQ(L"Lorem ipsum dolor ...", cq(L"p:contains(dolor)").text().c_str());
	//EXPECT_EQ(2, cq(L":contains(\"androids)").size()); //todo: jquery :contains works differently than I assumed
	EXPECT_EQ(1, cq(L"p:contains(\"androids)").size());
	EXPECT_STREQ(L"Do \"androids dream ...", cq(L"p:contains(\"androids").text().c_str());
 
	//:has()
	EXPECT_STREQ(L"second", cq(L"div:has(h1)")[L"id"].c_str());
 
	// descendant selector
	EXPECT_EQ(3, cq(L"#second p").size());
	
	/* there is html like that:
	<div id="second">
	<div id="third"></div></div>
 
	cq("#second div")["id"] <-- This code should not return "second", should return "third"
	*/
	EXPECT_STREQ(L"third", cq(L"#second div")[L"id"].c_str());
	
	// ">" selector
	EXPECT_EQ(2, cq(L"body > div").size());
	EXPECT_EQ(3, cq(L"body >").size());
 
	//the same problem as with descendant selector
	EXPECT_STREQ(L"third", cq(L"#second > div")[L"id"].c_str());
 
	//attribute equals selector [name=value]
	EXPECT_STREQ(L"chbox", cq(L"[name=team]")[L"id"].c_str());
 
	//:not() selector
	EXPECT_EQ(3, cq(L"p:not(.some)").size());
	EXPECT_STREQ(L"Lorem ipsum dolor ...", cq(L"p:not(.some)")[0].text().c_str());
 
	//more complex examples:
	EXPECT_EQ(1, cq(L"div:has(p:has(b))").size());
	EXPECT_STREQ(L"first", cq(L"div:has(p:has(b))")[L"id"].c_str());
	EXPECT_EQ(0, cq(L"div:has(p:has(u))").size());


	//utf-8
	//CppQuery::Query<std::wstring> cq2(std::wstring(html));
	EXPECT_STREQ(L"zażółć", cq(L"#find-me").text().c_str()); //checks if deals with: <h3   id="find-me"  >zażółć/h3   >
	EXPECT_EQ(6, cq(L"#find-me").text().size());
	EXPECT_EQ(1, cq(L":contains(zażółć").size());
	EXPECT_STREQ(L"find-me", cq(L":contains(zażółć)")[L"id"].c_str());
 
}

TEST_F(SimpleCase, Problematic){
	//checks if deals with: <p class="some">Second <b>paragraph</b> ...</p>
	EXPECT_STREQ(L"Second paragraph ...", cq(L"p")[1].text().c_str());
	
	EXPECT_STREQ(L"Lorem ipsum dolor ...Second paragraph ...", cq(L"#first p").text().c_str()); //check how it deals when text() is called on object containing a few nodes (should concatenate)
	
	EXPECT_STREQ(L"class-name", cq(L"#attr")[L"class"].c_str()); //checks if deals with: <h3 id="attr" class='class-name'>Another</h3>
	
	//check if deals with: <input type="checkbox" id="chbox" name="team" value="team" checked>Spurs
	//to distinguish between situations when "checked" is not on attributes list and is on it but empty - use attr_exists()
	EXPECT_TRUE(cq(L"#chbox")[L"checked"].empty());
	EXPECT_TRUE(cq(L"#chbox").attr_exists(L"checked"));

	//checks if deals with: <h3 id='no-quote'  class=no-quote>Just another</h3>
	EXPECT_STREQ(L"no_quote", cq(L"#no-quote")[L"class"].c_str());

	//if cq("selector")==1 then: cq("selector")[0] returns cq("selector"):
	EXPECT_STREQ(L"special", cq(L"#first")[L"class"].c_str());
	EXPECT_STREQ(L"special", cq(L"#first")[0][L"class"].c_str());

	//copy constructor
	cq = cq(L"#first");
	EXPECT_EQ(2, cq(L"p").size());
	CppQuery::Query<std::wstring> cq2(cq);
	EXPECT_EQ(2, cq2(L"p").size());
	cq = cq(L"p")[0];
	EXPECT_EQ(1, cq(L"p").size());
	EXPECT_EQ(2, cq2(L"p").size());

	CppQuery::Query<std::wstring> cq3;
	//operator = ()
	cq3 = cq2;
	EXPECT_EQ(2, cq3(L"p").size());

}

/*TEST_F(RealCase, Selectors){
	//simple selector and ["attribute"] notation
	EXPECT_STREQ("homepage", cq("logolink")["alt"].c_str());

	//nested selector and text()
	EXPECT_STREQ("Quickstart", cq("#quickstart h1").text().c_str());

	//alternative version of nested selector
	EXPECT_STREQ("Quickstart", cq("quickstart")("h1").text().c_str());

	//get n-th element with [n] notation
	EXPECT_STREQ("quickstart", cq(".section")[2]["id"].c_str());

	//get number of elements with size()
	EXPECT_EQ(6, cq(".section").size());
}

TEST_F(RealCase, Problematic){
	//more problematic situations:
	//out of range, there is only 6 nodes with class "section"
	string tmp = cq(".section")[6].text();
	EXPECT_EQ(NULL, &tmp);
	
	tmp =  cq("#logolink")["nonexisting"];
	EXPECT_TRUE(tmp.empty());
	EXPECT_FALSE(cq("#logolink").attr_exists("nonexisting"));
	EXPECT_EQ(0, cq(".nonexisting").size());
	EXPECT_EQ(0, cq(".nonexisting")("nonexisting2").size());
	tmp =  cq(".nonexisting").text();
	EXPECT_TRUE(tmp.empty());
}

TEST_F(OpenTagsCase, OpenTags){
	//checks if deals with: <img id="logo" src="some.gif" />
	EXPECT_STREQ("", cq("#logo").text().c_str());
	EXPECT_STREQ("some.gif", cq("#logo")["src"].c_str());
	
	//checks if deals with: <p>some paragraph
	EXPECT_STREQ("some paragraph", cq("p")[0].text().c_str());

	//checks if deals with: <li>First
	EXPECT_STREQ("first", cq("li")[0].text().c_str());
}*/

int main(int argc, char **argv) {
  ::testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}
