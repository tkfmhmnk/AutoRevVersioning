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
	������̐擪�̃X�y�[�X(std::isspace�ŋ󔒂Ƃ݂Ȃ���镶��)���폜����
	@param str �X�y�[�X���폜���镶����ւ̎Q��
	@return str�ւ̎Q��
*/
template<class CharT> std::basic_string<CharT>& DeleteFirstSpace(std::basic_string<CharT> &str) {
	if (str.empty()) {											//��̕�����̏ꍇ�A�I��
		return str;
	}
	typename std::basic_string<CharT>::const_iterator strIt;
	std::locale loc = std::locale();

	strIt = str.cbegin();
	if (std::isspace<CharT>(*strIt, loc) == false) {			//�ŏ��̕������X�y�[�X���`�F�b�N					
		return str;												//�ŏ��̕������X�y�[�X�łȂ��Ȃ�A�I��
	}
	++strIt;

	for (; (strIt != str.cend()) && (std::isspace<CharT>(*strIt, loc)); ++strIt) {}	//������̍Ō�ɂȂ邩�A�󔒂łȂ��Ȃ�܂Ń��[�v

	str.erase(str.cbegin(), strIt);							//�ŏ��̕�������󔒂łȂ������̈�܂��܂ł��폜
	return str;
}

/**
	������̖����̃X�y�[�X(std::isspace�ŋ󔒂Ƃ݂Ȃ���镶��)���폜����
	@param str �X�y�[�X���폜���镶����ւ̎Q��
	@return str�ւ̎Q��
*/
template<class CharT> std::basic_string<CharT>& DeleteLastSpace(std::basic_string<CharT> &str) {
	if (str.empty()) {											//��̕�����̏ꍇ�A�I��
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

	for (; (i < size) && (std::isspace<CharT>(str[size - i - 1], loc)); i++) {}	//������̍Ō�ɂȂ邩�A�󔒂łȂ��Ȃ�܂Ń��[�v

	str.erase(size - i);										//�󔒂łȂ���������̕������폜
	return str;
}
/**
	������̐擪�Ɩ����̃X�y�[�X(std::isspace�ŋ󔒂Ƃ݂Ȃ���镶��)���폜����
	@param str �X�y�[�X���폜���镶����ւ̎Q��
	@return str�ւ̎Q��
*/
template<class CharT> std::basic_string<CharT>& DeleteSpace(std::basic_string<CharT> &str) {
	return DeleteLastSpace<CharT>(DeleteFirstSpace<CharT>(str));
}

#endif