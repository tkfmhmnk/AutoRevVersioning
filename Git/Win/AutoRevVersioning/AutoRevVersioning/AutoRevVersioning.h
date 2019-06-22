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

/**
	@version 0.0.0.12664
*/

#ifndef _AUTO_REV_VERSIONING_H
#define _AUTO_REV_VERSIONING_H
#include<iostream>
#include<fstream>
#include<string>
#include"DeleteSpace.h"

#include"mtcLiteral.h"
#include"mtcNumericConv.h"
#include"mtcFileIO.h"

namespace mtc {
	MULTITYPE_CSTR(
		searchKeyword,
		_MULTITYPE_STR("FILEVERSION "),
		_MULTITYPE_STR("PRODUCTVERSION "),
		_MULTITYPE_STR("VALUE \"FileVersion\""),
		_MULTITYPE_STR("VALUE \"ProductVersion\"")
	);
	MULTITYPE_CSTR(
		headerKeyword,
		_MULTITYPE_STR("@version "),
	);
}

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
	mtc::FileIO::Manager<std::basic_istream<CharT>> rcStreamManager;
	mtc::FileIO::Manager<std::basic_ostream<CharT>> tempRcStreamManager;
	std::basic_string<CharT> line;
	std::basic_string<CharT> temp;
	size_t pos_FILEVERSION;

	ErrCode ret;

	Version<CharT> fileVer;
	Version<CharT> prodVer;

	mtc::FileIO::Ret rRet;
	mtc::FileIO::Ret wRet;

	static constexpr CharT cr = mtc::CR<CharT>();
	static constexpr const CharT* keyFILE = mtc::searchKeyword<CharT>(0);
	static constexpr const CharT* keyPROD = mtc::searchKeyword<CharT>(1);
	static constexpr const CharT* keyVFILE = mtc::searchKeyword<CharT>(2);
	static constexpr const CharT* keyVPROD = mtc::searchKeyword<CharT>(3);

	const CharT* endline;
	static constexpr const CharT endline_CRLF[] = { mtc::CR<CharT>(),mtc::LF<CharT>(),mtc::NullChar<CharT>() };
	static constexpr const CharT endline_LF[] = {mtc::LF<CharT>(),mtc::NullChar<CharT>() };
	static constexpr const CharT endline_none[] = { mtc::NullChar<CharT>() };

	rRet = rcStreamManager.OpenStream(rcFile);
	if (rRet != mtc::FileIO::Ret::OK) {
		std::cout << "Failed to read " << rcFile << std::endl;
		return ErrCode::FailedOpenRcFile;
	}

	std::string outputFileName = rcFile;
	outputFileName += ".temp";
	wRet = tempRcStreamManager.OpenStream(outputFileName.c_str(), rcStreamManager.endian);
	if (wRet != mtc::FileIO::Ret::OK) {
		std::cout << "Failed to write " << outputFileName << std::endl;
		rcStreamManager.CloseStream();
		return ErrCode::FailedOpenTempRcFile;
	}

	try {
		fileVer.revision =mtc::NumericConv::ToString<CharT>(rev);
		prodVer.revision = fileVer.revision;

		std::basic_istream<CharT>& rcStream = *(rcStreamManager.pStream);
		std::basic_ostream<CharT>& tempRcStream = *(tempRcStreamManager.pStream);

		while (rcStream.good()) {
			getline(rcStream, line);
			if (rcStream.eof()) {
				endline = endline_none;
			}
			else if ((!line.empty())&&(line.back() == cr)) {
				line.pop_back();
				endline = endline_CRLF;
			}
			else {
				endline = endline_LF;
			}

			temp = line;
			DeleteSpace<CharT>(temp);
			if ((pos_FILEVERSION = temp.find(keyFILE)) == 0) {
				ret = ProcFILEVERSION(line, fileVer);
				if (ret != ErrCode::OK) throw ret;
			}
			else if ((pos_FILEVERSION = temp.find(keyPROD)) == 0) {
				ret = ProcPRODUCTVERSION(line, prodVer);
				if (ret != ErrCode::OK) throw ret;

			}
			else if ((pos_FILEVERSION = temp.find(keyVFILE)) == 0) {
				ret = ProcVFILEVERSION(line, fileVer);
				if (ret != ErrCode::OK) throw ret;
			}
			else if ((pos_FILEVERSION = temp.find(keyVPROD)) == 0) {
				ret = ProcVPRODUCTVERSION(line, prodVer);
				if (ret != ErrCode::OK) throw ret;
			}
			tempRcStream << line << endline;
		}
	}
	catch (const ErrCode& e) {
		rcStreamManager.CloseStream();
		tempRcStreamManager.CloseStream();
		return e;
	}

	rcStreamManager.CloseStream();
	tempRcStreamManager.CloseStream();
	return ErrCode::OK;
}

template<class CharT> ErrCode ProcFILEVERSION(std::basic_string<CharT>& line, Version<CharT>& ver) {
	size_t posHead, posComma_major, posComma_minor, posComma_build;
	std::basic_string<CharT> key(mtc::searchKeyword<CharT>(0));
	static constexpr CharT comma = mtc::Comma<CharT>();
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
	line = line + ver.major + comma + ver.minor + comma + ver.build + comma + ver.revision;

	return ErrCode::OK;
}

template<class CharT> ErrCode ProcPRODUCTVERSION(std::basic_string<CharT>& line, Version<CharT>& ver) {
	size_t posHead, posComma_major, posComma_minor, posComma_build;
	std::basic_string<CharT> key(mtc::searchKeyword<CharT>(1));
	static constexpr CharT comma = mtc::Comma<CharT>();
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
	line = line + ver.major + comma + ver.minor + comma + ver.build + comma + ver.revision;

	return ErrCode::OK;
}

template<class CharT> ErrCode ProcVFILEVERSION(std::basic_string<CharT>& line, const Version<CharT>& ver) {
	size_t posHead;
	std::basic_string<CharT> key(mtc::searchKeyword<CharT>(2));
	static constexpr CharT comma = mtc::Comma<CharT>();
	static constexpr CharT dot = mtc::Dot<CharT>();
	static constexpr CharT dquo = mtc::DQuo<CharT>();

	if ((posHead = line.find(key)) == string::npos) return ErrCode::Unknown;

	line.erase(posHead + key.length());
	line = line + comma + dquo + ver.major + dot + ver.minor + dot + ver.build + dot + ver.revision + dquo;

	return ErrCode::OK;
}

template<class CharT> ErrCode ProcVPRODUCTVERSION(std::basic_string<CharT>& line, const Version<CharT>& ver) {
	size_t posHead;
	std::basic_string<CharT> key(mtc::searchKeyword<CharT>(3));
	static constexpr CharT comma = mtc::Comma<CharT>();
	static constexpr CharT dot = mtc::Dot<CharT>();
	static constexpr CharT dquo = mtc::DQuo<CharT>();

	if ((posHead = line.find(key)) == string::npos) return ErrCode::Unknown;

	line.erase(posHead + key.length());
	line = line + comma + dquo + ver.major + dot + ver.minor + dot + ver.build + dot + ver.revision + dquo;

	return ErrCode::OK;
}



template<class CharT> ErrCode ReplaceHeaderVersion(const int rev, const char* headerFile) {
	mtc::FileIO::Manager<std::basic_istream<CharT>> headerStreamManager;
	mtc::FileIO::Manager<std::basic_ostream<CharT>> tempHeaderStreamManager;
	std::basic_string<CharT> line;
	std::basic_string<CharT> temp;
	size_t pos_version;

	ErrCode ret;

	Version<CharT> fileVer;
	Version<CharT> prodVer;

	mtc::FileIO::Ret rRet;
	mtc::FileIO::Ret wRet;

	static constexpr CharT cr = mtc::CR<CharT>();
	static constexpr const CharT* keyVersion = mtc::headerKeyword<CharT>(0);

	const CharT* endline;
	static constexpr const CharT endline_CRLF[] = { mtc::CR<CharT>(),mtc::LF<CharT>(),mtc::NullChar<CharT>() };
	static constexpr const CharT endline_LF[] = { mtc::LF<CharT>(),mtc::NullChar<CharT>() };
	static constexpr const CharT endline_none[] = { mtc::NullChar<CharT>() };

	rRet = headerStreamManager.OpenStream(headerFile);
	if (rRet != mtc::FileIO::Ret::OK) {
		std::cout << "Failed to read " << headerFile << std::endl;
		return ErrCode::FailedOpenRcFile;
	}

	std::string outputFileName = headerFile;
	outputFileName += ".temp";
	wRet = tempHeaderStreamManager.OpenStream(outputFileName.c_str(), headerStreamManager.endian);
	if (wRet != mtc::FileIO::Ret::OK) {
		std::cout << "Failed to write " << outputFileName << std::endl;
		headerStreamManager.CloseStream();
		return ErrCode::FailedOpenTempRcFile;
	}

	try {
		fileVer.revision = mtc::NumericConv::ToString<CharT>(rev);
		prodVer.revision = fileVer.revision;

		std::basic_istream<CharT>& headerStream = *(headerStreamManager.pStream);
		std::basic_ostream<CharT>& tempRcStream = *(tempHeaderStreamManager.pStream);

		while (headerStream.good()) {
			getline(headerStream, line);
			if (headerStream.eof()) {
				endline = endline_none;
			}
			else if ((!line.empty()) && (line.back() == cr)) {
				line.pop_back();
				endline = endline_CRLF;
			}
			else {
				endline = endline_LF;
			}

			temp = line;
			DeleteSpace<CharT>(temp);
			if ((pos_version = temp.find(keyVersion)) == 0) {
				ret = ProcHeaderVersion(line, fileVer);
				if (ret != ErrCode::OK) throw ret;
			}
			tempRcStream << line << endline;
		}
	}
	catch (const ErrCode& e) {
		headerStreamManager.CloseStream();
		tempHeaderStreamManager.CloseStream();
		return e;
	}

	headerStreamManager.CloseStream();
	tempHeaderStreamManager.CloseStream();
	return ErrCode::OK;
}

template<class CharT> ErrCode ProcHeaderVersion(std::basic_string<CharT>& line, Version<CharT>& ver) {
	size_t posHead, posDot_major, posDot_minor, posDot_build;
	std::basic_string<CharT> key(mtc::headerKeyword<CharT>(0));
	static constexpr CharT dot = mtc::Dot<CharT>();
	std::basic_string<CharT> oldRev;

	if ((posHead = line.find(key)) == string::npos) return ErrCode::Unknown;

	if ((posDot_major = line.find(dot, posHead + key.length())) == string::npos) return ErrCode::FailedGetMajorVer;
	ver.major = line.substr(posHead + key.length(), posDot_major - (posHead + key.length()));

	if ((posDot_minor = line.find(dot, posDot_major + 1)) == string::npos) return ErrCode::FailedGetMinorVer;
	ver.minor = line.substr(posDot_major + 1, posDot_minor - (posDot_major + 1));

	if ((posDot_build = line.find(dot, posDot_minor + 1)) == string::npos) return ErrCode::FailedGetBuildVer;
	ver.build = line.substr(posDot_minor + 1, posDot_build - (posDot_minor + 1));

	oldRev = line.substr(posDot_build + 1);
	DeleteSpace(oldRev);
	if (oldRev == ver.revision) return ErrCode::SameRevision;

	line.erase(posHead + key.length());
	line = line + ver.major + dot + ver.minor + dot + ver.build + dot + ver.revision;

	return ErrCode::OK;
}


#endif