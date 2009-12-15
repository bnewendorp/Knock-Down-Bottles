/*
 *  String.cpp
 *  String
 *
 *  Created by Christian Noon on 8/12/09.
 *  Copyright 2009 Amoniq LLC. All rights reserved.
 *
 *  Licensed to the Apache Software Foundation (ASF) under one
 *  or more contributor license agreements.  See the NOTICE file
 *  distributed with this work for additional information
 *  regarding copyright ownership.  The ASF licenses this file
 *  to you under the Apache License, Version 2.0 (the
 *  "License"); you may not use this file except in compliance
 *  with the License.  You may obtain a copy of the License at
 *  
 *  http://www.apache.org/licenses/LICENSE-2.0
 *  
 *  Unless required by applicable law or agreed to in writing,
 *  software distributed under the License is distributed on an
 *  "AS IS" BASIS, WITHOUT WARRANTIES OR CONDITIONS OF ANY
 *  KIND, either express or implied.  See the License for the
 *  specific language governing permissions and limitations
 *  under the License.
 */

#include "String"

#ifdef __APPLE__
	#pragma mark Constructors
#endif

using namespace aq;

String::String() : std::string() {}
String::String(const char* c_string) : std::string(c_string) {}
String::String(std::string std_string) : std::string(std_string) {}
String::String(float number) : std::string(_ftoa(number)) {}
String::String(int number) : std::string(_itoa(number)) {}
String::String(double number) : std::string(_dtoa(number)) {}
String::String(bool bool_value) : std::string(_btoa(bool_value)) {}


#ifdef __APPLE__
	#pragma mark Static Methods
#endif

String String::getCurrentWorkingDirectory()
{
	String directory;
	
	#ifdef _WIN32
		directory = String(_getcwd(NULL, 0));
	#else
		directory = String(getcwd(NULL, 0));
	#endif
	
	return directory;
}

String String::getEnvironmentVariable(String env_var)
{
	return String(getenv(env_var.c_str()));
}

String String::join(String path, String fname)
{
	String joined;
	#ifdef _WIN32
		if (path.size() == 0 || fname.size() == 0)
			joined = path + "\\" + fname;
		else
		{
			char end = path.at(path.length() - 1);
			char begin = fname.at(0);
			if (String(&end) == "/" || String(&begin) == "/")
				joined = path + fname;
			else
				joined = path + "\\" + fname;
		}
	#else
		if (path.size() == 0 || fname.size() == 0)
			joined = path + "/" + fname;
		else
		{
			char end = path.at(path.length() - 1);
			char begin = fname.at(0);
			if (String(&end) == "/" || String(&begin) == "/")
				joined = path + fname;
			else
				joined = path + "/" + fname;
		}
	#endif
	
	return joined;
}

String String::joinVector(std::vector<String> strings)
{
	if (strings.size() == 0)
		return String("");
	
	String joined = strings.at(0);
	for (unsigned int i = 1; i < strings.size(); i++)
	{
	#ifdef _WIN32
		joined += "\\" + strings.at(i);
	#else
		joined += "/" + strings.at(i);
	#endif
	}
	
	return joined;	
}

void String::print(String print_string)
{
	std::cout << print_string << std::endl;
}

void String::print(std::string print_string)
{
	std::cout << print_string << std::endl;
}

void String::print(const char* print_string)
{
	std::cout << print_string << std::endl;
}

void String::print(float print_string)
{
	std::cout << "float: " << print_string << std::endl;
}

void String::print(int print_string)
{
	std::cout << "int: " << print_string << std::endl;
}

void String::print(double print_string)
{
	std::cout << "double: " << print_string << std::endl;
}

void String::print(bool print_string)
{
	if (print_string == true)
		std::cout << "bool: true" << std::endl;
	else
		std::cout << "bool: false" << std::endl;
}


#ifdef __APPLE__
	#pragma mark Helper Methods
#endif

void String::append(const String append_string)
{
	*this += append_string;
}

void String::append(const char* append_char)
{
	*this += append_char;
}

char& String::at(int position)
{
	return this->std::string::at(position);
}

const char* String::c_str()
{
	return this->std::string::c_str();
}

void String::capitalize()
{
	at(0) = toupper(at(0));
}

void String::clear()
{
	this->std::string::clear();
}

bool String::compare(const String str) const
{
	if (this->std::string::compare(str) == 0)
		return true;
	return false;
}

bool String::compare(const char* char_star) const
{
	if (this->std::string::compare(char_star) == 0)
		return true;
	return false;
}

bool String::contains(String contain_string, String::CaseSensitivity cs)
{
	size_t found;
	String sub = *this;
	
	if (cs == String::NotCaseSensitive)
	{
		sub.toLowerCase();
		contain_string.toLowerCase();	
	}
	
	found = sub.find(contain_string);
	if (found != std::string::npos)
		return true;
	else
		return false;
}

bool String::contains(const char* contain_char, String::CaseSensitivity cs)
{
	return this->contains(String(contain_char), cs);
}

int String::count(String contain_string, String::CaseSensitivity cs)
{
	size_t found;
	std::string temp_str;
	int contains_count = 0;
	String sub = *this;
	
	if (cs == String::NotCaseSensitive)
	{
		sub.toLowerCase();
		contain_string.toLowerCase();
	}
	
	for (int i = 0; i < sub.length(); i++)
	{
		temp_str = sub.substr(i, contain_string.length());
		found = temp_str.find(contain_string);
		if (found != std::string::npos)
			contains_count++;
	}
	
	return contains_count;
}

int String::count(const char* contain_char, String::CaseSensitivity cs)
{
	return this->count(String(contain_char), cs);
}

bool String::empty()
{
	return this->std::string::empty();
}

bool String::endsWith(String end_string, String::CaseSensitivity cs)
{
	String sub = *this;
	if (cs == String::NotCaseSensitive)
	{
		sub.toLowerCase();
		end_string.toLowerCase();
	}
	
	int index = sub.length() - end_string.length();
	String last_part(sub.substr(index, sub.length()));
	
	if (last_part == end_string)
		return true;
	return false;
}

bool String::endsWith(const char* end_char, String::CaseSensitivity cs)
{
	return this->endsWith(String(end_char), cs);
}

void String::erase(int position, int width)
{
	this->std::string::erase(position, width);
}

void String::erase(iterator position)
{
	this->std::string::erase(position);
}

void String::erase(iterator first, iterator last)
{
	this->std::string::erase(first, last);
}

void String::fill(const char* ch, int size)
{
	if (String(ch).length() > 1)
		assert(false);

	if (size != -1)
		this->resize(size);
	
	for (int i = 0; i < this->size(); i++)
		at(i) = *ch;
}

int String::find(const String str, int position)
{
	if (position > this->size() - 1)
		return -1;

	return int(this->std::string::find(str, position));
}

int String::find(const char* char_star, int position)
{
	if (position > this->size() - 1)
		return -1;

	return int(this->std::string::find(char_star, position));
}

int String::indexOf(String index_string, int from, String::CaseSensitivity cs)
{
	if (from > this->size() - 1)
		assert(false);

	size_t found;
	String sub = this->substr(from, this->length());
	
	if (cs == String::NotCaseSensitive)
	{
		sub.toLowerCase();
		index_string.toLowerCase();
	}
	
	found = sub.find(index_string);
	if (found != std::string::npos)
		return int(found);
		
	return -1;
}

int String::indexOf(const char* index_char, int from, String::CaseSensitivity cs)
{
	return this->indexOf(String(index_char), from, cs);
}

void String::insert(int position, String insert_string)
{
	if (position > this->size())
		assert(false);

	String first = this->substr(0, position);
	String end = this->substr(position, this->length());
	
	*this = first + insert_string + end;
}

void String::insert(int position, const char* insert_char)
{
	insert(position, String(insert_char));
}

bool String::isEmpty()
{
	return this->empty();
}

int String::lastIndexOf(String index_string, int from, String::CaseSensitivity cs)
{
	if (from > this->size() - 1)
		assert(false);
	
	size_t found;
	String sub = this->substr(from, this->length());
	
	if (cs == String::NotCaseSensitive)
	{
		sub.toLowerCase();
		index_string.toLowerCase();
	}
	
	found = sub.rfind(index_string);
	if (found != std::string::npos)
		return int(found);
	
	return -1;
}

int String::lastIndexOf(const char* index_char, int from, String::CaseSensitivity cs)
{
	return this->lastIndexOf(String(index_char), from, cs);
}

String String::left(int num)
{
	if (num > this->size() - 1)
		assert(false);
		
	return this->substr(0, num);
}

int String::length()
{
	return this->std::string::length();
}

void String::prepend(const String prepend_string)
{
	this->std::string::insert(0, prepend_string);
}

void String::prepend(const char* prepend_char)
{
	this->std::string::insert(0, prepend_char);
}

void String::print()
{
	std::cout << *this << std::endl;
}

void String::remove(int position, int width)
{
	erase(position, width);
}

void String::remove(String remove_string, String::CaseSensitivity cs)
{
	size_t found = 0;
	String sub = *this;
	
	if (cs == String::NotCaseSensitive)
	{
		sub.toLowerCase();
		remove_string.toLowerCase();
	}
	
	while (found != std::string::npos) 
	{
		found = sub.rfind(remove_string);
		if (found != std::string::npos)
		{
			this->erase(int(found), remove_string.length());
			sub.erase(int(found), remove_string.length());
		}
	}
}

void String::remove(const char* remove_char, String::CaseSensitivity cs)
{
	remove(String(remove_char), cs);
}

void String::replace(int position, int width, const String replace_string)
{
	erase(position, width);
	insert(position, replace_string);
}

void String::replace(int position, int width, const char* replace_char)
{
	replace(position, width, String(replace_char));
}

void String::replace(String before_string, String after_string, String::CaseSensitivity cs)
{
	size_t found = 0;
	String sub = *this;
	
	if (before_string == after_string)
		assert(false);
	
	if (cs == String::NotCaseSensitive)
	{
		sub.toLowerCase();
		before_string.toLowerCase();
	}
	
	while (found != std::string::npos) 
	{
		found = sub.find(before_string);
		if (found != std::string::npos)
		{
			erase(int(found), before_string.length());
			sub.erase(int(found), before_string.length());
			insert(found, after_string);
			sub.insert(found, after_string);
		}
	}
}

void String::replace(const char* before_char, const char* after_char, 
					   String::CaseSensitivity cs)
{
	replace(String(before_char), String(after_char), cs);
}

void String::resize(int n)
{
	this->std::string::resize(n);
}
	  
void String::resize(int n, const char* c)
{
	this->std::string::resize(n, *c);
}

int String::rfind(const String str)
{
	return int(this->std::string::rfind(str));
}

int String::rfind(const char* char_star)
{	
	return int(this->std::string::rfind(char_star));
}

String String::right(int num)
{
	if (num > this->size() - 1)
		assert(false);
	
	return this->substr(this->length() - num, this->length());
}

String String::section(int start, int end)
{
	if (end == -1)
		end = this->length();
	return subString(start, end);
}

int String::size()
{
	return this->std::string::size();
}

std::vector<String> String::split(const char* sep)
{
	size_t found = 0;
	String search_str = *this;
	std::vector<String> split_strings;

	while (found != std::string::npos) 
	{
		found = search_str.find(sep);
		if (found != std::string::npos)
		{
			split_strings.push_back(search_str.substr(0, found));
			search_str.erase(0, int(found) + 1);
		}
	}
	split_strings.push_back(search_str);
	
	return split_strings;
}

bool String::startsWith(String start_string, String::CaseSensitivity cs)
{
	String sub = *this;
	if (cs == String::NotCaseSensitive)
	{
		sub.toLowerCase();
		start_string.toLowerCase();
	}
	
	String first_part(sub.substr(0, start_string.length()));
	
	if (first_part == start_string)
		return true;
	return false;
}

bool String::startsWith(const char* start_char, String::CaseSensitivity cs)
{
	return this->startsWith(String(start_char), cs);
}

String String::substr(int from, int to)
{
	return String(this->std::string::substr(from, to));
}

String String::subString(int from, int to)
{
	return substr(from, to);
}

double String::toDouble()
{
	return strtod(this->c_str(), NULL);
}

float String::toFloat()
{
	return atof(this->c_str());
}

int String::toInt()
{
	return atoi(this->c_str());
}

void String::toLowerCase()
{	
	for (int i = 0; i < this->length(); i++)
		at(i) = tolower(at(i));
}

std::string String::toStdString()
{
	return (std::string)*this;
}

void String::toUpperCase()
{
	for (int i = 0; i < this->length(); i++)
		at(i) = toupper(at(i));
}


#ifdef __APPLE__
	#pragma mark File I/O Methods
#endif

bool String::isFile()
{
	FILE* pFile;

	pFile = fopen(this->c_str(), "r");
	if (pFile != NULL)
		return true;
	return false;
}

bool String::isDirectory()
{
	String fname = join(*this, "test.txt");
	bool isDir = false;

	FILE* pFile;
	pFile = fopen(fname.c_str(), "wt");
	
	if (pFile != NULL)
		isDir = true;
	
	fclose(pFile);
	std::remove(fname.c_str());
	
	return isDir;
}

void String::convertToUnixPath()
{
	replace("\\", "/");
}

void String::convertToWindowsPath()
{
	replace("/", "\\");
}

String String::getExtension()
{
	String extension = this->substr(this->rfind(".") + 1, this->length());
	extension.toLowerCase();
	return extension;
}

String String::getFileName()
{
	String fname;
	size_t returnValue = this->rfind("/");
	if (returnValue != std::string::npos)
		fname = this->substr(this->rfind("/") + 1, this->length());
	else
	{
		returnValue = this->rfind("\\");
		if (returnValue != std::string::npos)
			fname = this->substr(this->rfind("\\") + 1, this->length());
		else
			fname = *this;
	}
	
	return fname;
}

String String::getFileNameWithoutExtension()
{
	String fname;
	size_t returnValue = this->rfind("/");
	if (returnValue != std::string::npos)
		fname = this->substr(this->rfind("/") + 1, this->length());
	else
	{
		returnValue = this->rfind("\\");
		if (returnValue != std::string::npos)
			fname = this->substr(this->rfind("\\") + 1, this->length());
		else
			fname = *this;
	}

	String withoutExt = fname.substr(0, fname.rfind("."));
	return withoutExt;
}

String String::getFilePath()
{
	String path = "";
	size_t returnValue = this->rfind("/");
	if (returnValue != std::string::npos)
		path = this->substr(0, this->rfind("/"));
	else 
	{
		returnValue = this->rfind("\\");
		if (returnValue != std::string::npos)
			path = this->substr(0, this->rfind("\\"));
	}
	
	return path;
}


#ifdef __APPLE__
	#pragma mark Private Methods
#endif

String String::_itoa(int input)
{
	std::ostringstream output;
	if(!(output << input))
	{
		return "ERROR - Conversion did not work";
	}
	
	return String(output.str());
}

String String::_ftoa(float input)
{
	char floatChars[32];
	sprintf(floatChars, "%f", input);
	return String(floatChars);
}

String String::_dtoa(double input)
{
	char floatChars[32];
	sprintf(floatChars, "%f", input);
	return String(floatChars);
}

String String::_btoa(bool input)
{
	if (input == true)
		return String("true");
	else
		return String("false");
}
