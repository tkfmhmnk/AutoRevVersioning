/*
Copyright [2019] [tkfmhmnk]

   Licensed under the Apache License, Version 2.0 (the "License");
   you may not use this file except in compliance with the License.
   You may obtain a copy of the License at

	   http://www.apache.org/licenses/LICENSE-2.0

   Unless required by applicable law or agreed to in writing, software
   distributed under the License is distributed on an "AS IS" BASIS,
   WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
   See the License for the specific language governing permissions and
limitations under the License.
*/

#ifndef _UNICODE_FILE_INPUT_H
#define _UNICODE_FILE_INPUT_H

#ifdef LIBUNICODEFILEIO_EXPORTS
#define UNICODEFILEIO_ATTRBT __declspec(dllexport)
#else
#define UNICODEFILEIO_ATTRBT __declspec(dllimport)
#endif

#define UNICODEFILEIO_CALL __stdcall

#include <iostream>
#include <fstream>
#include<sstream>
#include <string>
namespace UnicodeFileIO {
	enum class Endian : int {
		BE,									//!<ビッグエンディアン
		LE									//!<リトルエンディアン
	};
	enum class Ret : int {
		Unknown = 0,
		OK,									//!<成功
		SrcFile_OpenErr,					//!<ソースファイルのオープンエラー
		SrcFile_UnknownBom,					//!<ソースファイルの不明なBOM
		SrcFileStream_BadStatus,			//!<ソースファイルのストリームのエラー
		DesFile_OpenErr,					//!<出力ファイルのオープンエラー
		SrcStringStream_BadStatus,			//!<入力StringStreamのストリームのエラー
		DesFileStream_BadStatus,			//!<出力ファイルのストリームのエラー

	};

	UNICODEFILEIO_ATTRBT Ret UNICODEFILEIO_CALL ReadString(const char* srcFileName, std::basic_stringstream<char16_t>& des, Endian &endian);
	UNICODEFILEIO_ATTRBT Ret UNICODEFILEIO_CALL WriteString(const char* desFileName, std::basic_stringstream<char16_t>& src, const Endian &endian);
}

#endif