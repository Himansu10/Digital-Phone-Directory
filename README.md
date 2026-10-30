# Digital Phone Directory (C++)

A stand-alone, console-based desktop application — no browser, no server,
no internet required. Just compile and run.

## How to compile & run

You need any C++ compiler that supports C++17 (g++, MinGW, Code::Blocks,
Dev-C++, Visual Studio all work fine).

**Command line (g++):**
```
g++ -std=c++17 -O2 main.cpp -o PhoneDirectory
./PhoneDirectory          (Linux/Mac)
PhoneDirectory.exe        (Windows)
```

**Code::Blocks / Dev-C++ / Visual Studio:**
Create a new empty C++ console project, add all the files
(`main.cpp`, `Contact.h`, `HashTable.h`, `Trie.h`, `UndoStack.h`,
`RecentQueue.h`, `Sorting.h`, `PhoneDirectory.h`) to the project, and build.
Everything is header-only except `main.cpp`, so no extra linking is needed.

Contacts are saved automatically to `contacts.txt` in the same folder,
so your data is still there the next time you run the program.

## Features

1. **Add contact** — name, phone, email, address, category (Family / Friend / Work / Other)
2. **Search by name** — instant lookup, with "did you mean" suggestions if not found
3. **Search by phone number** — reverse lookup ("who is this number?")
4. **Autocomplete search** — type a few letters, see every matching name instantly
5. **Update contact** — edit any field, including renaming, without losing history
6. **Delete contact** — with a safety net (see Undo below)
7. **Undo last delete** — deleted a contact by mistake? Bring it right back
8. **Display all contacts** — neatly listed in alphabetical order
9. **Recently viewed** — the last 5 contacts you looked up, most recent first
10. **Duplicate protection** — won't let you add two contacts with the same name or phone number
11. **Input validation** — rejects malformed phone numbers and email addresses
12. **Persistent storage** — everything is saved to `contacts.txt` automatically

## Data structures used (and why)

| Data structure | Built from scratch? | Used for | Average time |
|---|---|---|---|
| **Hash Table** (separate chaining, djb2 hash, auto-rehashing) | Yes | Name → Contact lookup | O(1) |
| **Hash Table** (second instance) | Yes | Phone → Name reverse lookup | O(1) |
| **Trie** | Yes | Prefix-based autocomplete search | O(k), k = prefix length |
| **Stack** (singly linked list) | Yes | Undo-delete history (LIFO) | O(1) push/pop |
| **Circular Queue** (array based) | Yes | "Recently viewed" contacts (FIFO with fixed capacity) | O(1) |
| **Merge Sort** | Yes | Alphabetical display ordering | O(n log n) |

Everything above the STL basics (`std::vector`, `std::string`) is implemented
manually — the hash tables, the trie, the stack, and the queue do **not**
use `std::unordered_map`, `std::stack`, or `std::queue`. This is deliberate:
the point of the project is to demonstrate the mechanics of these structures,
not just call a library.

## Design notes

- **Why a hash table for the main store?** Linear search through a list of
  contacts is O(n) — fine for 10 contacts, painful for 10,000. A hash table
  gives average O(1) add/search/delete regardless of size.
- **Why a second hash table for phone numbers?** A phone directory needs to
  answer two questions: "what's Alice's number?" (name → phone) and
  "who's calling from this number?" (phone → name). One structure can't
  serve both efficiently, so we index both directions.
- **Why a Trie for autocomplete?** A hash table can tell you if an exact
  name exists, but it can't tell you "which names start with 'Ja'" without
  scanning everything. A Trie answers that in time proportional to the
  prefix length, not the number of contacts.
- **Why a Stack for undo?** Undo is inherently last-in-first-out — the most
  recent deletion should be the first thing restored. A stack is the
  natural fit.
- **Why a circular queue for recents?** We only ever want the last N items,
  and old ones should drop off automatically — a fixed-size circular buffer
  does this in O(1) without ever reallocating.

## Known limitations (good to mention if asked in a viva)

- Contact names cannot contain the `|` character (used internally as a
  field separator when saving to file).
- Names are assumed to be unique (used as the primary hash key). Two
  people with the exact same name need slightly different entries, e.g.
  "John Smith" and "John Smith (Work)".
- This is a single-user, local application — there's no concurrency or
  multi-user access control, which wouldn't make sense for a stand-alone
  desktop tool anyway.

## Possible extensions

- Group contacts by category and browse by group
- Import/export to CSV for use in Excel
- A simple Qt or Win32 GUI on top of the same `PhoneDirectory` class
  (the core logic is already fully decoupled from the console UI in `main.cpp`,
  so this would only mean writing a new front end)
