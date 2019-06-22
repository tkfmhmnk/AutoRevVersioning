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

#include<iostream>
#include<fstream>
#include<cstdlib>
#include<string>
#include"AutoRevVersioning.h"


using namespace std;

static const char* gitRevFile = "AutoRevVersioning_gitrev";

int GetGitRev(const char* exePath, const char* projDir, int& rev);

ErrCode UpdateRcFile(const char* rcfile) {
	string cmd;
	int cmd_ret;
	cmd = "move /Y " + string(rcfile) + ".temp " + string(rcfile) + " > nul";
	cmd_ret = system(cmd.c_str());

	if (cmd_ret != 0) {
		cout << "ErrorCode=" << cmd_ret << " : cmd=" << cmd << endl;
		return ErrCode::FailedUpdateRcFile;
	}
	return ErrCode::OK;
}

int DeleteFile(const char* file) {
	string cmd;
	int cmd_ret;
	cmd = "del /Q " + string(file) + " > nul";
	cmd_ret = system(cmd.c_str());

	if (cmd_ret != 0) {
		cout << "ErrorCode=" << cmd_ret << " : cmd=" << cmd << endl;
		return -1;
	}
	return 0;
}

int main(int argc, char* argv[], char* envp[]) {
	if (argc != 6) {
		cout << argv[0] << "Usage: AutoRevVersioning.exe <projectdir> <filepath> <file codepage> <filetype> <repositorytype>" << endl;
		return (int)ErrCode::InvalidArg;
	}

	string fileType(argv[4]);
	string repositoryType(argv[5]);
	int codePage;
	string codePageStr(argv[3]);
	int rev;										//git:SHA1の先頭も4文字の数値
	ErrCode replaseRet;
	ErrCode updateRet;

	try {
		size_t idx;
		codePage = stoi(codePageStr, &idx, 10);
		if (idx != codePageStr.length()) {
			cout << argv[0] << ": error: " << "Invalid arg. CodePage=" << codePageStr << endl;
			return (int)ErrCode::InvalidArg;
		}
	}
	catch (const invalid_argument& e) {
		cout << argv[0] << ": error: " << "Invalid arg. CodePage=" << codePageStr << endl;
		cout << argv[0] << ": error: " << e.what() << endl;
		return (int)ErrCode::InvalidArg;
	}
	catch (const out_of_range& e) {
		cout << argv[0] << ": error: " << "Invalid arg. CodePage=" << codePageStr << endl;
		cout << argv[0] << ": error: " << e.what() << endl;
		return (int)ErrCode::InvalidArg;
	}

	int ret;
	if ((repositoryType == "git") || (repositoryType == "Git")) {
		ret = GetGitRev(argv[0],argv[1],rev);
	}
	else if ((repositoryType == "svn") || (repositoryType == "SVN")) {

	}
	else {


	}

	if (ret == 0) {
		if (fileType == "rc") {
			switch (codePage) {
			case 1200:
				replaseRet = ReplaceRcVersion<char16_t>(rev, argv[2]);
				break;

			case 932:
				replaseRet = ReplaceRcVersion<char>(rev, argv[2]);
				break;

			default:
				return (int)ErrCode::UnknownCodePage;
				break;
			}
		}
		else if (fileType == "header") {
			switch (codePage) {
			case 1200:
				replaseRet = ReplaceHeaderVersion<char16_t>(rev, argv[2]);
				break;

			case 932:
				replaseRet = ReplaceHeaderVersion<char>(rev, argv[2]);
				break;

			default:
				return (int)ErrCode::UnknownCodePage;
				break;
			}
		}

		if (replaseRet == ErrCode::SameRevision) {
			cout << argv[0] << ": warning: Same revision. Skip revision versioning." << endl;
			return 0;
		}
		else if (replaseRet != ErrCode::OK) {
			cout << argv[0] << ": error: " << "Failed to replace revision of rc file. : " << argv[2] << endl;
			return (int)replaseRet;
		}
		updateRet = UpdateRcFile(argv[2]);
		if (updateRet != ErrCode::OK) return (int)updateRet;

		cout << argv[0] << ": note: Done revision versioning. rev=" << rev << endl;

		return 0;


	}

}

int GetGitRev(const char* exePath,const char* projDir,int& rev){
	string projectdir(projDir);
	string cmd;
	int cmd_ret;

	cmd = "git log -1 " + projectdir + " 1>" + gitRevFile;
	cmd_ret = system(cmd.c_str());

	if (cmd_ret != 0) {
		cout << exePath << ": error: " << "ErrorCode=" << cmd_ret << " : cmd=" << cmd << endl;
		return (int)ErrCode::FailedGetLog;
	}

	ifstream gitRevStream(gitRevFile);
	if (!gitRevStream.is_open()) {
		cout << exePath << ": error: " << "Failed to open " << gitRevFile << endl;
		return (int)ErrCode::FailedOpenRevFile;
	}

	try {
		string commitLine;								//commit SHA1の行の文字列
		string sha1;									//SHA1の文字列
		string sha1short;								//SHA1の先頭の4文字
		size_t pos;
		size_t idx;

		getline(gitRevStream, commitLine);
		pos = commitLine.find("commit ");
		if (pos == string::npos) {
			cout << exePath << ": error: " << "Not found commit" << endl;
			throw ErrCode::FailedReadCommit;
		}

		sha1 = commitLine.substr(pos + 7);
		if (sha1.length() < 4) {
			cout << exePath << ": error: " << "Invalid sha1. sha1=" << sha1 << endl;
			throw ErrCode::InvalidSHA1;
		}
		sha1short = sha1.substr(0, 4);
		try {
			rev = stoi(sha1short, &idx, 16);
			if (idx != sha1short.length()) {
				cout << exePath << ": error: " << "Invalid sha1. sha1=" << sha1 << endl;
				throw ErrCode::InvalidSHA1;
			}
			cout << exePath << ": note: " << "Get revision(" << projDir <<"): " << sha1 << endl;
		}
		catch (const invalid_argument& e) {
			cout << exePath << ": error: " << "Invalid sha1. sha1=" << sha1 << endl;
			cout << exePath << ": error: " << e.what() << endl;
			throw (int)ErrCode::InvalidSHA1;
		}
		catch (const out_of_range& e) {
			cout << exePath << ": error: " << "Invalid sha1. sha1=" << sha1 << endl;
			cout << exePath << ": error: " << e.what() << endl;
			throw ErrCode::OutRangeSHA1;
		}
	}
	catch (const ErrCode& e) {
		gitRevStream.close();
		DeleteFile(gitRevFile);
		return (int)e;
	}
	catch (...) {
		gitRevStream.close();
		DeleteFile(gitRevFile);
		cout << exePath << ": error: " << "Unknown" << endl;
		return (int)ErrCode::Unknown;
	}
	gitRevStream.close();
	if (DeleteFile(gitRevFile) != 0) {
		cout << exePath << ": error: " << "Failed to delete temporary file." << endl;
		return (int)ErrCode::FailedDeleteRevFile;
	}

	return 0;
}

int GetSVNRev(const char* exePath, const char* projDir, int& rev) {
	return 0;
}