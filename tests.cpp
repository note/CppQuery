#include <gtest/gtest.h>
#include "CQuery.h"

using namespace std;

const string realFile = "pyquery.html";
const string simpleFile = "simple.htlm";

class SimpleCase : public ::testing::Test{
	public:
	SimpleCase(){
		ifstream s(simpleFile);
		stringstream ss;
		ss << s.rdbuf();
		html = ss.str();
	}

	private string html;
};

class RealCase : public ::testing::Test{
	public:
	RealCase(){
		ifstream s(realFile);
		stringstream ss;
		ss << s.rdbuf();
		html = ss.str();
	}
	
	private string html;
};


