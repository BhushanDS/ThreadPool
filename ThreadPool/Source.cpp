#include <iostream>
#include <string>
#include "threadpool.h"
using namespace std;

int Execute(int a)
{
	std::this_thread::sleep_for(std::chrono::seconds(2));
	cout << "Executing Thread : " << std::this_thread::get_id() << "\n";
	return a * a;
}

std::string ExecuteString(string s)
{
	std::this_thread::sleep_for(std::chrono::seconds(2));
	cout << "Executing Thread : " << std::this_thread::get_id() << "\n";
	return s+s;
}


int main()
{

	ThreadPool pool(16);

	std::future<int>result = pool.ExecuteTask(Execute,5); //Calling int function

	std::future<std::string>result_string = pool.ExecuteTask(ExecuteString, "ABC"); // calling string functions 

	cout << "Result : " << result.get() << "\n";
	cout << "Result : " << result_string.get() << "\n";
	
	return EXIT_SUCCESS;
}