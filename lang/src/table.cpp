#include "value.hpp"
#include <cassert>
#include <table.hpp>
#include <upvalue.hpp>

namespace snap {

using VT = ValueType;
using OT = ObjType;

#define TABLE_GET_SLOT(k, h)	   search_entry<Table, Entry>(this, k, h)
#define TABLE_GET_SLOT_CONST(k, h) search_entry<const Table, const Entry>(this, k, h)
#define TABLE_HASH(k)			   (hash_value(k))

// check if an entry is unoccupied.
#define IS_ENTRY_FREE(e) (SNAP_IS_NIL(e.key))

Table::~Table() {
	delete[] m_entries;
}

void Table::ensure_capacity() {
	if (m_num_entries < m_cap * LoadFactor) return;
	std::size_t old_cap = m_cap;
	m_cap *= GrowthFactor;
	Entry* old_entries = m_entries;
	m_entries = new Entry[m_cap];

	for (std::size_t i = 0; i < old_cap; ++i) {
		Entry& entry = old_entries[i];
		if (IS_ENTRY_FREE(entry)) continue;
		Entry& new_entry = TABLE_GET_SLOT(entry.key, entry.hash);
		new_entry = std::move(entry);
	}

	delete[] old_entries;
}

Value Table::get(Value key) const {
	u64 mask = m_cap - 1;
	u64 hash = TABLE_HASH(key);
	u64 index = hash & mask;

	while (true) {
		Entry& entry = m_entries[index];
		if ((entry.hash == hash and entry.key == key) or IS_ENTRY_FREE(entry)) return entry.value;
		index = (index + 1) & mask;
	}

	return SNAP_NIL_VAL;
}

bool Table::set(Value key, Value value) {
	assert(SNAP_GET_TT(key) != VT::Nil);
	ensure_capacity();
	u64 hash = TABLE_HASH(key);
	u64 mask = m_cap - 1;

	u64 index = hash & mask;
	// The probe distance that we have covered so far.
	// Initially we are at our "desired" slot, where
	// our entry would ideally sit. So the probe distance is
	// 0.
	u32 dist = 0;

	while (true) {
		Entry& entry = m_entries[index];

		// If we found an unitialized spot or a
		// tombstone in the entries buffer, use that
		// slot for insertion.
		const bool is_free = IS_ENTRY_FREE(entry);
		const bool is_tombstone = SNAP_IS_EMPTY(entry.key);

		if (is_free or is_tombstone) {
			entry.key = std::move(key);
			entry.value = std::move(value);
			entry.probe_distance = dist;
			entry.hash = hash;
			// Only increment number of entries
			// if the slot was free.
			if (is_free) ++m_num_entries;
			return true;
		}

		// if we found an intialized list that has the
		// same key as the current key we're trying to set,
		// then change the value in that entry.
		if (entry.key == key) {
			entry.value = std::move(value);
			entry.probe_distance = dist;
			return true;
		}

		// if we found an entry that isn't what
		// we're looking for, but has a lower
		// probe count, then we swap the entries
		// and keep going.
		if (entry.probe_distance < dist) {
			std::swap(hash, entry.hash);
			std::swap(key, entry.key);
			std::swap(value, entry.value);
			dist = entry.probe_distance;
		}

		index = (index + 1) & mask;
		++dist;
	}

	return true;
}

String* Table::find_string(const char* chars, u64 length) {
	assert(chars != nullptr);
	u64 hash = hash_cstring(chars, length);
	u64 mask = m_cap - 1;
	u64 index = hash & mask;

	while (true) {
		Entry& entry = m_entries[index];

		if (entry.hash == hash) {
			Value& k = entry.key;
			if (SNAP_IS_STRING(k)) {
				String* s = SNAP_AS_STRING(k);
				if (s->m_length == length and std::memcmp(s->c_str(), chars, length) == 0) return s;
			}
		}

		// we have hit an empty slot, meaning there
		// is no such string in the hashtable.
		if (entry.hash == 0) return nullptr;

		index = (index + 1) & mask;
	}

	return nullptr;
}

u64 Table::hash_value(Value key) const {
	assert(SNAP_GET_TT(key) != VT::Nil);
	switch (SNAP_GET_TT(key)) {
	case VT::Bool: return SNAP_AS_BOOL(key) ? 7 : 15;
	case VT::Number: return std::size_t(SNAP_AS_NUM(key)); // TODO: use a proper numeric hash
	case VT::Object: return hash_object(SNAP_AS_OBJECT(key));
	default: return -1; // impossible.
	}
}

u64 Table::hash_object(Obj* object) const {
	switch (object->tag) {
	case OT::string: return static_cast<String*>(object)->m_hash;
	case OT::upvalue: return hash_value(*static_cast<Upvalue*>(object)->value);
	default:
		if (object->m_hash == -1) return object->hash();
		return object->m_hash;
	}
}

} // namespace snap