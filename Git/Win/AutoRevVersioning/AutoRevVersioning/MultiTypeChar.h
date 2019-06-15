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

#ifndef _MULTITYPECHAR_H
#define _MULTITYPECHAR_H

/**
	所望の型で特定の文字を取得できる関数の定義を展開する
*/
#define MULTITYPE_CHAR(name,val)namespace MultiTypeChar{ template<class CharT> constexpr CharT Get##name() {\
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
	};\
}

namespace MultiTypeChar {
	/**
		複数の文字列型をまとめた構造体。_MULTITYPE_STRと組み合わせて使うことを想定
	*/
	class CStrings {
	public:
		constexpr CStrings(const char* pc, const wchar_t* pL, const char16_t* pu, const char32_t* pU) :
			c(pc), L(pL), u(pu), U(pU) {
		};
		const char* c;
		const wchar_t* L;
		const char16_t* u;
		const char32_t* U;
	};
}

/**
	文字列を複数の文字列リテラルにして展開する
*/
#define _MULTITYPE_STR(arg) {arg , L##arg , u##arg , U##arg} 

#define MULTITYPE_CSTR(name, ...) namespace MultiTypeChar{\
	constexpr CStrings name[] = {__VA_ARGS__};\
	constexpr std::size_t name##Size = sizeof(name)/sizeof(CStrings);\
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
	};\
}


MULTITYPE_CHAR(Comma, ',');
MULTITYPE_CHAR(Dot, '.');
MULTITYPE_CHAR(CR, '\r');
MULTITYPE_CHAR(LF, '\n');
MULTITYPE_CHAR(Zero, '0');
MULTITYPE_CHAR(UA, 'A');
MULTITYPE_CHAR(Minus, '-');
MULTITYPE_CHAR(DQuo, '\"');

#endif