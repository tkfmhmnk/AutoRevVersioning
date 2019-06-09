/*
Copyright[2019][tkfmhmnk]

Licensed under the Apache License, Version 2.0 (the "License");
you may not use this file except in compliance with the License.
You may obtain a copy of the License at

http ://www.apache.org/licenses/LICENSE-2.0

Unless required by applicable law or agreed to in writing, software
distributed under the License is distributed on an "AS IS" BASIS,
WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
See the License for the specific language governing permissions and
limitations under the License.
*/


#ifndef _AUTO_REV_VERSIONING_H
#define _AUTO_REV_VERSIONING_H
#include<iostream>
#include<fstream>
#include<locale>
#include<string>
#include <codecvt>
#include"DeleteSpace.h"

/**
	所望の型で特定の文字を取得できる関数の定義を展開する
*/
#define MULTITYPE_CHAR(name,val) template<class CharT> constexpr CharT Get##name() {\
		return val;\
	};\
	template<> constexpr char Get##name<char>(){\
		return val;\
	};\
	template<> constexpr wchar_t Get##name<wchar_t>(){\
		return L##val;\
	};\
	template<> constexpr char16_t Get##name<char16_t>(){\
		return u##val;\
	};\
	template<> constexpr char32_t Get##name<char32_t>(){\
		return U##val;\
	}

MULTITYPE_CHAR(Comma, ',');

/**
	複数の文字列型をまとめた構造体。_CSVSTREAM_MULTITYPE_STRと組み合わせて使うことを想定
*/
class MultiTypeCString {
public:
	constexpr MultiTypeCString(const char* pc, const wchar_t* pL, const char16_t* pu, const char32_t* pU):
	c(pc) , L(pL) , u(pu) , U(pU){
	};
	const char* c;
	const wchar_t* L;
	const char16_t* u;
	const char32_t* U;
};

/**
	文字列を複数の文字列リテラルにして展開する
*/
#define _MULTITYPE_STR(arg) {arg , L##arg , u##arg , U##arg} 

#define MULTITYPE_CSTR(name, ...) constexpr MultiTypeCString name[] = {__VA_ARGS__};\
constexpr std::size_t name##Size = sizeof(name)/sizeof(MultiTypeCString);\
template<class CharT> constexpr const CharT* Get##name(const int index) {\
	return (name+index)->c;\
};\
template<> constexpr const char* Get##name<char>(const int index) {\
	return (name+index)->c;\
};\
template<> constexpr const wchar_t* Get##name<wchar_t>(const int index) {\
	return (name+index)->L;\
};\
template<> constexpr const char16_t* Get##name<char16_t>(const int index) {\
	return (name+index)->u;\
};\
template<> constexpr const char32_t* Get##name<char32_t>(const int index) {\
	return (name+index)->U;\
}

MULTITYPE_CSTR(
	searchKeyword,
	_MULTITYPE_STR("FILEVERSION "),
	_MULTITYPE_STR("PRODUCTVERSION "),
	_MULTITYPE_STR("VALUE \"FileVersion\""),
	_MULTITYPE_STR("VALUE \"ProductVersion\"")
);

constexpr const char* tfunc() {
	return "aa";
}

enum class ErrCode :int {
	OK = 0,
	InvalidArg,
	UnknownCodePage,
	FailedGetLog,
	FailedOpenRevFile,
	FailedReadCommit,
	InvalidSHA1,
	OutRangeSHA1,
	FailedOpenTempRcFile,
	FailedOpenRcFile,
	FailedGetMajorVer,
	FailedGetMinorVer,
	FailedGetBuildVer,
	Unknown,
};

template<class CharT> class Version {
public:
	std::basic_string<CharT> major;
	std::basic_string<CharT> minor;
	std::basic_string<CharT> build;
	std::basic_string<CharT> revision;
};

template<class CharT> ErrCode ReplaceRcVersion(const int rev, const char* rcFile) {
	std::basic_ofstream<CharT> tempRcStream((std::string(rcFile) + ".temp").c_str());
	std::basic_ifstream<CharT> rcStream(rcFile);
	std::basic_string<CharT> line;
	std::basic_string<CharT> temp;
	size_t pos_FILEVERSION;

	ErrCode ret;

	Version<CharT> ver;

	if (!tempRcStream.is_open()) {
		std::cout << "Failed to open " << rcFile << ".temp" << std::endl;
		return ErrCode::FailedOpenTempRcFile;
	}
	if (!rcStream.is_open()) {
		std::cout << "Failed to open " << rcFile << std::endl;
		return ErrCode::FailedOpenRcFile;
	}
	//std::locale::global(std::locale("ja_JP.UTF-8"));
	//rcStream.imbue(std::locale("ja_JP.UTF-8"));

	static_assert(sizeof(wchar_t) == 2, "error.");//Linuxではつかうcvt違うから直してくれ
	rcStream.imbue(std::locale(std::locale(""), new std::codecvt_utf8_utf16<wchar_t, 0x10ffff, std::consume_header>()));


	while (rcStream.good()) {
		getline(rcStream, line);
		temp = line;
		DeleteSpace<CharT>(temp);
		if ((pos_FILEVERSION = temp.find(GetsearchKeyword<CharT>(0))) == 0) {
			ret = ProcFILEVERSION(line, rev, ver);
		}
		else if ((pos_FILEVERSION = temp.find(GetsearchKeyword<CharT>(1))) == 0) {

		}
		else if ((pos_FILEVERSION = temp.find(GetsearchKeyword<CharT>(2))) == 0) {

		}
		else if ((pos_FILEVERSION = temp.find(GetsearchKeyword<CharT>(3))) == 0) {

		}
		tempRcStream << line;
	}

	return ErrCode::OK;
}

template<class CharT> ErrCode ProcFILEVERSION(std::basic_string<CharT>& line, const int rev, Version<CharT>& ver) {
	size_t posHead, posComma_major, posComma_minor, posComma_build;
	std::basic_string<CharT> key(GetsearchKeyword<CharT>(0));
	if ((posHead = line.find(key)) == string::npos) return ErrCode::Unknown;

	if ((posComma_major = line.find(GetComma<CharT>(), posHead + key.length())) == string::npos) return ErrCode::FailedGetMajorVer;
	ver.major = line.substr(posHead + key.length(), posComma_major - 1 - (posHead + key.length()));

	if ((posComma_minor = line.find(GetComma<CharT>(), posComma_major + 1)) == string::npos) return ErrCode::FailedGetMinorVer;
	ver.minor = line.substr(posComma_major + 1, posComma_minor - 1 - (posComma_major + 1));

	if ((posComma_build = line.find(GetComma<CharT>(), posComma_minor + 1)) == string::npos) return ErrCode::FailedGetBuildVer;
	ver.build = line.substr(posComma_minor + 1, posComma_build - 1 - (posComma_minor + 1));


	return ErrCode::OK;
}

#endif