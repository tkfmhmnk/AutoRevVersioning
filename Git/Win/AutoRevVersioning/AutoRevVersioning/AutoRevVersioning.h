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
#include"UnicodeFileIO.h"
#include"MultiTypeChar.h"
#include"UnicodeConvert.h"

MULTITYPE_CSTR(
	searchKeyword,
	_MULTITYPE_STR("FILEVERSION "),
	_MULTITYPE_STR("PRODUCTVERSION "),
	_MULTITYPE_STR("VALUE \"FileVersion\""),
	_MULTITYPE_STR("VALUE \"ProductVersion\"")
);

enum class ErrCode :int {
	OK = 0,
	SameRevision,
	InvalidArg,
	UnknownCodePage,
	FailedGetLog,
	FailedOpenRevFile,
	FailedDeleteRevFile,
	FailedReadCommit,
	InvalidSHA1,
	OutRangeSHA1,
	FailedOpenTempRcFile,
	FailedOpenRcFile,
	FailedGetMajorVer,
	FailedGetMinorVer,
	FailedGetBuildVer,
	FailedUpdateRcFile,
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
	std::basic_stringstream<CharT> tempRcStream;
	std::basic_stringstream<CharT> rcStream;
	std::basic_string<CharT> line;
	std::basic_string<CharT> temp;
	size_t pos_FILEVERSION;

	ErrCode ret;

	Version<CharT> fileVer;
	Version<CharT> prodVer;

	UnicodeFileIO::Ret rRet;
	UnicodeFileIO::Ret wRet;
	UnicodeFileIO::Endian endian;

	static constexpr CharT lf = MultiTypeChar::LF<CharT>();
	static constexpr const CharT* keyFILE = MultiTypeChar::GetsearchKeyword<CharT>(0);
	static constexpr const CharT* keyPROD = MultiTypeChar::GetsearchKeyword<CharT>(1);
	static constexpr const CharT* keyVFILE = MultiTypeChar::GetsearchKeyword<CharT>(2);
	static constexpr const CharT* keyVPROD = MultiTypeChar::GetsearchKeyword<CharT>(3);

	rRet = UnicodeFileIO::ReadString(rcFile, rcStream, endian);

	if (rRet != UnicodeFileIO::Ret::OK) {
		std::cout << "Failed to read " << rcFile << std::endl;
		return ErrCode::FailedOpenRcFile;
	}

	fileVer.revision = UnicodeConvert::ToString<CharT>(rev);
	prodVer.revision = fileVer.revision;

	while (rcStream.good()) {
		getline(rcStream, line);
		temp = line;
		DeleteSpace<CharT>(temp);
		if ((pos_FILEVERSION = temp.find(keyFILE)) == 0) {
			ret = ProcFILEVERSION(line, fileVer);
			if (ret != ErrCode::OK) return ret;
		}
		else if ((pos_FILEVERSION = temp.find(keyPROD)) == 0) {
			ret = ProcPRODUCTVERSION(line, prodVer);
			if (ret != ErrCode::OK) return ret;

		}
		else if ((pos_FILEVERSION = temp.find(keyVFILE)) == 0) {
			ret = ProcVFILEVERSION(line, fileVer);
			if (ret != ErrCode::OK) return ret;
		}
		else if ((pos_FILEVERSION = temp.find(keyVPROD)) == 0) {
			ret = ProcVPRODUCTVERSION(line, prodVer);
			if (ret != ErrCode::OK) return ret;
		}
		tempRcStream << line << lf;
	}

	std::string outputFileName = rcFile;
	outputFileName += ".temp";
	wRet = UnicodeFileIO::WriteString(outputFileName.c_str(), tempRcStream, endian);

	if (wRet != UnicodeFileIO::Ret::OK) {
		std::cout << "Failed to write " << outputFileName << std::endl;
		return ErrCode::FailedOpenTempRcFile;
	}

	return ErrCode::OK;
}

template<class CharT> ErrCode ProcFILEVERSION(std::basic_string<CharT>& line, Version<CharT>& ver) {
	size_t posHead, posComma_major, posComma_minor, posComma_build;
	std::basic_string<CharT> key(MultiTypeChar::GetsearchKeyword<CharT>(0));
	static constexpr CharT comma = MultiTypeChar::Comma<CharT>();
	std::basic_string<CharT> oldRev;

	if ((posHead = line.find(key)) == string::npos) return ErrCode::Unknown;

	if ((posComma_major = line.find(comma, posHead + key.length())) == string::npos) return ErrCode::FailedGetMajorVer;
	ver.major = line.substr(posHead + key.length(), posComma_major - (posHead + key.length()));

	if ((posComma_minor = line.find(comma, posComma_major + 1)) == string::npos) return ErrCode::FailedGetMinorVer;
	ver.minor = line.substr(posComma_major + 1, posComma_minor - (posComma_major + 1));

	if ((posComma_build = line.find(comma, posComma_minor + 1)) == string::npos) return ErrCode::FailedGetBuildVer;
	ver.build = line.substr(posComma_minor + 1, posComma_build - (posComma_minor + 1));

	oldRev = line.substr(posComma_build + 1);
	DeleteSpace(oldRev);
	if (oldRev == ver.revision) return ErrCode::SameRevision;

	line.erase(posHead + key.length());
	line = line + ver.major + comma + ver.minor + comma + ver.build + comma + ver.revision + MultiTypeChar::CR<CharT>();

	return ErrCode::OK;
}

template<class CharT> ErrCode ProcPRODUCTVERSION(std::basic_string<CharT>& line, Version<CharT>& ver) {
	size_t posHead, posComma_major, posComma_minor, posComma_build;
	std::basic_string<CharT> key(MultiTypeChar::GetsearchKeyword<CharT>(1));
	static constexpr CharT comma = MultiTypeChar::Comma<CharT>();
	std::basic_string<CharT> oldRev;

	if ((posHead = line.find(key)) == string::npos) return ErrCode::Unknown;

	if ((posComma_major = line.find(comma, posHead + key.length())) == string::npos) return ErrCode::FailedGetMajorVer;
	ver.major = line.substr(posHead + key.length(), posComma_major - (posHead + key.length()));

	if ((posComma_minor = line.find(comma, posComma_major + 1)) == string::npos) return ErrCode::FailedGetMinorVer;
	ver.minor = line.substr(posComma_major + 1, posComma_minor - (posComma_major + 1));

	if ((posComma_build = line.find(comma, posComma_minor + 1)) == string::npos) return ErrCode::FailedGetBuildVer;
	ver.build = line.substr(posComma_minor + 1, posComma_build - (posComma_minor + 1));

	oldRev = line.substr(posComma_build + 1);
	DeleteSpace(oldRev);
	if (oldRev == ver.revision) return ErrCode::SameRevision;

	line.erase(posHead + key.length());
	line = line + ver.major + comma + ver.minor + comma + ver.build + comma + ver.revision + MultiTypeChar::CR<CharT>();

	return ErrCode::OK;
}

template<class CharT> ErrCode ProcVFILEVERSION(std::basic_string<CharT>& line, const Version<CharT>& ver) {
	size_t posHead;
	std::basic_string<CharT> key(MultiTypeChar::GetsearchKeyword<CharT>(2));
	static constexpr CharT comma = MultiTypeChar::Comma<CharT>();
	static constexpr CharT dot = MultiTypeChar::Dot<CharT>();
	static constexpr CharT dquo = MultiTypeChar::DQuo<CharT>();

	if ((posHead = line.find(key)) == string::npos) return ErrCode::Unknown;

	line.erase(posHead + key.length());
	line = line + comma + dquo + ver.major + dot + ver.minor + dot + ver.build + dot + ver.revision + dquo + MultiTypeChar::CR<CharT>();

	return ErrCode::OK;
}

template<class CharT> ErrCode ProcVPRODUCTVERSION(std::basic_string<CharT>& line, const Version<CharT>& ver) {
	size_t posHead;
	std::basic_string<CharT> key(MultiTypeChar::GetsearchKeyword<CharT>(3));
	static constexpr CharT comma = MultiTypeChar::Comma<CharT>();
	static constexpr CharT dot = MultiTypeChar::Dot<CharT>();
	static constexpr CharT dquo = MultiTypeChar::DQuo<CharT>();

	if ((posHead = line.find(key)) == string::npos) return ErrCode::Unknown;

	line.erase(posHead + key.length());
	line = line + comma + dquo + ver.major + dot + ver.minor + dot + ver.build + dot + ver.revision + dquo + MultiTypeChar::CR<CharT>();

	return ErrCode::OK;
}


#endif