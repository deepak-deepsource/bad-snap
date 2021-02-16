#include <algorithm>
#include <cstring>
#include <string.hpp>

namespace snap {

// this hash function is from: https://craftinginterpreters.com/hash-tables.html
std::size_t hash_cstring(const char* key, int len) {
	// hash upto 32 characters max.
	len = std::min(len, 32);

	uint32_t hash = 2166136261u;

	for (int i = 0; i < len; i++) {
		hash ^= key[i];
		hash *= 16777619;
	}

	return hash;
}

using OT = ObjType;

String::String(const char* chrs, std::size_t len) : Obj(ObjType::string), m_length{len} {
	char* buf = new char[len + 1];
	std::memcpy(buf, chrs, len);
	buf[len] = '\0';
	m_hash = hash_cstring(buf, m_length);
	m_chars = buf;
}

String::String(const String* left, const String* right)
	: Obj(ObjType::string), m_length(left->m_length + right->m_length) {
	char* buf = new char[m_length + 1];
	buf[m_length] = '\0';
	std::memcpy(buf, left->m_chars, left->m_length);
	std::memcpy(buf + left->m_length, right->m_chars, right->m_length);
	m_hash = hash_cstring(buf, m_length);
	m_chars = buf;
}

String::String(char* chrs) : Obj(ObjType::string), m_chars{chrs}, m_length{strlen(chrs)} {
	m_hash = hash_cstring(chrs, m_length);
}

String::~String() {
	delete[] m_chars;
}

s32 String::hash() {
	m_hash = hash_cstring(m_chars, m_length);
	return m_hash;
}

String* String::concatenate(const String* left, const String* right) {
	std::size_t length = left->m_length + right->m_length;

	char* buf = new char[length + 1];
	buf[length] = '\0';
	std::memcpy(buf, left->m_chars, left->m_length);
	std::memcpy(buf + left->m_length, right->m_chars, right->m_length);
	return new String(buf, length);
}

bool operator==(const String& a, const String& b) {
	if (a.m_length != b.m_length or a.m_hash != b.m_hash) return false;
	return std::memcmp(a.c_str(), b.c_str(), a.m_length) == 0;
}

const char* String::c_str() const {
	return m_chars;
}

char String::at(number index) const {
	if (index < 0 or index > m_length) return '\0';
	return m_chars[std::size_t(index)];
}

} // namespace snap