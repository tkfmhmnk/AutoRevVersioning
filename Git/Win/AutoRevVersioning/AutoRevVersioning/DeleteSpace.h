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
#ifndef _DELETE_SPACE_H
#define _DELETE_SPACE_H
#include<string>
#include<locale>

/**
	文字列の先頭のスペース(std::isspaceで空白とみなされる文字)を削除する
	@param str スペースを削除する文字列への参照
	@return strへの参照
*/
template<class CharT> std::basic_string<CharT>& DeleteFirstSpace(std::basic_string<CharT> &str) {
	if (str.empty()) {											//空の文字列の場合、終了
		return str;
	}
	typename std::basic_string<CharT>::const_iterator strIt;
	std::locale loc = std::locale();

	strIt = str.cbegin();
	if (std::isspace<CharT>(*strIt, loc) == false) {			//最初の文字がスペースがチェック					
		return str;												//最初の文字がスペースでないなら、終了
	}
	++strIt;

	for (; (strIt != str.cend()) && (std::isspace<CharT>(*strIt, loc)); ++strIt) {}	//文字列の最後になるか、空白でなくなるまでループ

	str.erase(str.cbegin(), strIt);							//最初の文字から空白でない文字の一つまえまでを削除
	return str;
}

/**
	文字列の末尾のスペース(std::isspaceで空白とみなされる文字)を削除する
	@param str スペースを削除する文字列への参照
	@return strへの参照
*/
template<class CharT> std::basic_string<CharT>& DeleteLastSpace(std::basic_string<CharT> &str) {
	if (str.empty()) {											//空の文字列の場合、終了
		return str;
	}

	std::locale loc = std::locale();
	typename std::basic_string<CharT>::size_type i;
	typename std::basic_string<CharT>::size_type size = str.length();
	i = 0;
	if (std::isspace<CharT>(str[size - i - 1], loc) == false) {
		return str;
	}
	i++;

	for (; (i < size) && (std::isspace<CharT>(str[size - i - 1], loc)); i++) {}	//文字列の最後になるか、空白でなくなるまでループ

	str.erase(size - i);										//空白でない文字より後の文字を削除
	return str;
}
/**
	文字列の先頭と末尾のスペース(std::isspaceで空白とみなされる文字)を削除する
	@param str スペースを削除する文字列への参照
	@return strへの参照
*/
template<class CharT> std::basic_string<CharT>& DeleteSpace(std::basic_string<CharT> &str) {
	return DeleteLastSpace<CharT>(DeleteFirstSpace<CharT>(str));
}

#endif