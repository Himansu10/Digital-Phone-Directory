#ifndef TRIE_H
#define TRIE_H

#include <string>
#include <vector>
#include <cctype>

// ------------------------------------------------------------
// Trie
// Used for fast prefix search / autocomplete on contact names.
// e.g. typing "jo" instantly lists "John", "Joseph", "Joanna"...
// Each node holds up to 26 children (lowercase a-z).
// ------------------------------------------------------------
class TrieNode {
public:
    TrieNode* children[26];
    bool isEndOfWord;
    std::string originalName;   // preserves original casing at the leaf

    TrieNode() : isEndOfWord(false) {
        for (int i = 0; i < 26; ++i) children[i] = nullptr;
    }
};

class Trie {
private:
    TrieNode* root;

    static std::string toLower(const std::string& s) {
        std::string r = s;
        for (char& c : r) c = (char)tolower((unsigned char)c);
        return r;
    }

    void collectWords(TrieNode* node, std::vector<std::string>& results) const {
        if (!node) return;
        if (node->isEndOfWord) results.push_back(node->originalName);
        for (int i = 0; i < 26; ++i)
            if (node->children[i]) collectWords(node->children[i], results);
    }

    void deleteSubtree(TrieNode* node) {
        if (!node) return;
        for (int i = 0; i < 26; ++i) deleteSubtree(node->children[i]);
        delete node;
    }

public:
    Trie() { root = new TrieNode(); }
    ~Trie() { deleteSubtree(root); }

    void insert(const std::string& name) {
        std::string key = toLower(name);
        TrieNode* cur = root;
        for (char ch : key) {
            if (ch < 'a' || ch > 'z') continue;   // skip spaces/punctuation
            int idx = ch - 'a';
            if (!cur->children[idx]) cur->children[idx] = new TrieNode();
            cur = cur->children[idx];
        }
        cur->isEndOfWord = true;
        cur->originalName = name;
    }

    // Soft delete: unmark the end-of-word flag (nodes are cheap to keep)
    void remove(const std::string& name) {
        std::string key = toLower(name);
        TrieNode* cur = root;
        for (char ch : key) {
            if (ch < 'a' || ch > 'z') continue;
            int idx = ch - 'a';
            if (!cur->children[idx]) return;
            cur = cur->children[idx];
        }
        cur->isEndOfWord = false;
    }

    std::vector<std::string> searchByPrefix(const std::string& prefix) const {
        std::vector<std::string> results;
        std::string key = toLower(prefix);
        TrieNode* cur = root;
        for (char ch : key) {
            if (ch < 'a' || ch > 'z') continue;
            int idx = ch - 'a';
            if (!cur->children[idx]) return results;   // no matches
            cur = cur->children[idx];
        }
        collectWords(cur, results);
        return results;
    }
};

#endif
