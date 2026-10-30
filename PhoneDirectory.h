#ifndef PHONEDIRECTORY_H
#define PHONEDIRECTORY_H

#include <string>
#include <vector>
#include <fstream>
#include <iostream>
#include <cctype>

#include "Contact.h"
#include "HashTable.h"
#include "Trie.h"
#include "UndoStack.h"
#include "RecentQueue.h"
#include "Sorting.h"

// ------------------------------------------------------------
// PhoneDirectory
// Ties together every data structure into one clean API:
//   - HashTable<Contact>   nameTable   : name  -> Contact   (O(1) avg)
//   - HashTable<string>    phoneIndex  : phone -> name      (O(1) avg reverse lookup)
//   - Trie                 nameTrie    : prefix -> names    (autocomplete)
//   - UndoStack             undo       : LIFO of deletions
//   - RecentQueue          recents     : last-viewed contacts
// Data is persisted to a flat text file between runs.
// ------------------------------------------------------------
class PhoneDirectory {
private:
    HashTable<Contact> nameTable;
    HashTable<std::string> phoneIndex;
    Trie nameTrie;
    UndoStack undo;
    RecentQueue recents;
    std::string filename;

    static std::string toLower(const std::string& s) {
        std::string r = s;
        for (char& c : r) c = (char)tolower((unsigned char)c);
        return r;
    }

public:
    PhoneDirectory(const std::string& file = "contacts.txt")
        : nameTable(101), phoneIndex(101), recents(5), filename(file) {
        loadFromFile();
    }

    // ---------------- Core operations ----------------

    enum class AddResult { SUCCESS, DUPLICATE_NAME, DUPLICATE_PHONE, INVALID_INPUT };

    AddResult addContact(const Contact& c) {
        if (c.name.empty() || c.phone.empty()) return AddResult::INVALID_INPUT;
        if (nameTable.contains(toLower(c.name))) return AddResult::DUPLICATE_NAME;
        if (phoneIndex.contains(c.phone))        return AddResult::DUPLICATE_PHONE;

        nameTable.insert(toLower(c.name), c);
        phoneIndex.insert(c.phone, c.name);
        nameTrie.insert(c.name);
        saveToFile();
        return AddResult::SUCCESS;
    }

    bool searchByName(const std::string& name, Contact& out) const {
        return nameTable.search(toLower(name), out);
    }

    bool searchByPhone(const std::string& phone, Contact& out) {
        std::string name;
        if (!phoneIndex.search(phone, name)) return false;
        return nameTable.search(toLower(name), out);
    }

    std::vector<std::string> autocomplete(const std::string& prefix) const {
        return nameTrie.searchByPrefix(prefix);
    }

    // Update an existing contact. newC.name may differ from oldName to rename.
    bool updateContact(const std::string& oldName, const Contact& newC) {
        Contact existing;
        if (!nameTable.search(toLower(oldName), existing)) return false;

        // if phone number changed, keep phone index consistent
        if (existing.phone != newC.phone) {
            phoneIndex.remove(existing.phone);
            if (phoneIndex.contains(newC.phone)) {
                // new phone number belongs to someone else - reject silently by caller check
            }
            phoneIndex.insert(newC.phone, newC.name);
        } else if (existing.name != newC.name) {
            // phone same but name changed -> update phone index's stored name
            phoneIndex.insert(newC.phone, newC.name);
        }

        // if name changed, move the record to a new key + update trie
        if (toLower(existing.name) != toLower(newC.name)) {
            nameTable.remove(toLower(existing.name));
            nameTrie.remove(existing.name);
            nameTable.insert(toLower(newC.name), newC);
            nameTrie.insert(newC.name);
        } else {
            nameTable.insert(toLower(existing.name), newC);
        }

        saveToFile();
        return true;
    }

    bool deleteContact(const std::string& name, Contact& deleted) {
        if (!nameTable.search(toLower(name), deleted)) return false;
        nameTable.remove(toLower(name));
        phoneIndex.remove(deleted.phone);
        nameTrie.remove(deleted.name);
        undo.push(deleted);       // remember for undo
        saveToFile();
        return true;
    }

    bool undoDelete(Contact& restored) {
        if (!undo.pop(restored)) return false;
        // Only restore if no naming/phone conflict has appeared since deletion
        if (nameTable.contains(toLower(restored.name)) || phoneIndex.contains(restored.phone))
            return false;
        nameTable.insert(toLower(restored.name), restored);
        phoneIndex.insert(restored.phone, restored.name);
        nameTrie.insert(restored.name);
        saveToFile();
        return true;
    }

    bool hasUndoAvailable() const { return !undo.isEmpty(); }

    void logRecentView(const std::string& name) {
        recents.enqueue(name);
    }

    std::vector<std::string> getRecent() const {
        return recents.getAll();
    }

    std::vector<Contact> getAllSortedByName() const {
        std::vector<Contact> all = nameTable.getAllValues();
        SortUtil::sortByName(all);
        return all;
    }

    int totalContacts() const { return nameTable.size(); }

    // ---------------- Persistence ----------------

    void saveToFile() const {
        std::ofstream out(filename);
        if (!out) {
            std::cerr << "Warning: could not write to " << filename << "\n";
            return;
        }
        std::vector<Contact> all = nameTable.getAllValues();
        for (const auto& c : all) out << c.serialize() << "\n";
        out.close();
    }

    void loadFromFile() {
        std::ifstream in(filename);
        if (!in) return;   // no existing file yet, start fresh

        std::string line;
        while (std::getline(in, line)) {
            if (line.empty()) continue;
            Contact c = Contact::deserialize(line);
            if (c.name.empty()) continue;
            nameTable.insert(toLower(c.name), c);
            phoneIndex.insert(c.phone, c.name);
            nameTrie.insert(c.name);
        }
        in.close();
    }
};

#endif
