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

#ifndef _UNICODE_CONVERT_H
#define _UNICODE_CONVERT_H

#include<string>
#include"MultiTypeChar.h"

namespace UnicodeConvert {
	template<class CharT> std::basic_string<CharT> ToString(int src,const int base=10) {


		static constexpr int bufSize = 16;
		static constexpr CharT Zero = MultiTypeChar::Zero<CharT>();
		static constexpr CharT UA = MultiTypeChar::UA<CharT>();

		CharT buf[bufSize];

		int count=0;
		int next;
		std::basic_string<CharT> tempStr;
		tempStr.reserve(16 - 1);

		switch (base) {
		case 10:
			if (src >= 0) {
				do {
					next = src / base;
					buf[count++] = Zero + (CharT)(src - next * base);
					src = next;
				} while (next != 0);
			}
			else {
				tempStr.push_back(MultiTypeChar::Minus<CharT>());
				do {
					next = src / base;
					buf[count++] = Zero + (CharT)(next * base - src);
					src = next;
				} while (next != 0);
			}
			count--;
			do {
				tempStr.push_back(buf[count--]);
			} while (count >= 0);


			break;

		case 16:
			//todo
			break;
		case 8:
			//todo
			break;
		case 2:
			//todo
			break;

		}

		return tempStr;
	}

}
#endif