// ==================================================================
//  DIGITAL PHONE DIRECTORY
//  A stand-alone C++ console application demonstrating custom
//  data structure implementations:
//     - Hash Table (separate chaining)   -> name/phone lookup, O(1) avg
//     - Trie                             -> prefix / autocomplete search
//     - Stack (linked list)              -> undo-delete history
//     - Circular Queue (array based)     -> recently viewed contacts
//     - Merge Sort                       -> alphabetical listing
//  Contacts persist between runs via a flat text file (contacts.txt).
// ==================================================================

#include <iostream>
#include <iomanip>
#include <string>
#include <limits>
#include <algorithm>
#include <cctype>

#include "PhoneDirectory.h"

using namespace std;

// ---------------------- small input helpers ----------------------

void clearInputBuffer() {
    cin.clear();
    cin.ignore(numeric_limits<streamsize>::max(), '\n');
}

string readLine(const string& prompt) {
    cout << prompt;
    string line;
    if (!getline(cin, line) && cin.eof()) {
        cout << "\nInput stream closed. Exiting...\n";
        exit(0);
    }
    return line;
}

int readMenuChoice(int minVal, int maxVal) {
    int choice;
    while (true) {
        cout << "Enter choice: ";
        if (cin >> choice && choice >= minVal && choice <= maxVal) {
            clearInputBuffer();
            return choice;
        }
        if (cin.eof()) {
            // Input stream closed (e.g. redirected input ran out, or Ctrl+D).
            // Treat this the same as choosing Exit instead of looping forever.
            cout << "\nInput stream closed. Exiting...\n";
            exit(0);
        }
        cout << "Invalid choice, please try again.\n";
        clearInputBuffer();
    }
}

bool isValidPhone(const string& phone) {
    if (phone.size() < 7 || phone.size() > 15) return false;
    for (char c : phone) {
        if (!isdigit((unsigned char)c) && c != '+') return false;
    }
    return true;
}

bool isValidEmail(const string& email) {
    if (email.empty()) return true;   // email is optional
    size_t at = email.find('@');
    return at != string::npos && at > 0 && at < email.size() - 1
           && email.find('.', at) != string::npos;
}

string chooseCategory() {
    cout << "  1. Family\n  2. Friend\n  3. Work\n  4. Other\n";
    int c = readMenuChoice(1, 4);
    switch (c) {
        case 1: return "Family";
        case 2: return "Friend";
        case 3: return "Work";
        default: return "Other";
    }
}

void printDivider() {
    cout << string(58, '-') << "\n";
}

void printContact(const Contact& c) {
    printDivider();
    cout << left << setw(12) << "Name:"    << c.name    << "\n";
    cout << left << setw(12) << "Phone:"   << c.phone   << "\n";
    cout << left << setw(12) << "Email:"   << (c.email.empty() ? "-" : c.email) << "\n";
    cout << left << setw(12) << "Address:" << (c.address.empty() ? "-" : c.address) << "\n";
    cout << left << setw(12) << "Category:"<< c.category << "\n";
    printDivider();
}

void printContactRow(const Contact& c) {
    cout << left << setw(20) << c.name
         << setw(16) << c.phone
         << setw(10) << c.category
         << (c.email.empty() ? "-" : c.email) << "\n";
}

// ---------------------- menu actions ----------------------

void actionAddContact(PhoneDirectory& dir) {
    cout << "\n== Add New Contact ==\n";
    string name = readLine("Name: ");
    if (name.empty()) { cout << "Name cannot be empty. Cancelled.\n"; return; }

    string phone = readLine("Phone (digits only, 7-15 chars): ");
    if (!isValidPhone(phone)) { cout << "Invalid phone number. Cancelled.\n"; return; }

    string email = readLine("Email (optional): ");
    if (!isValidEmail(email)) { cout << "Invalid email format. Cancelled.\n"; return; }

    string address = readLine("Address (optional): ");
    cout << "Category:\n";
    string category = chooseCategory();

    Contact c(name, phone, email, address, category);
    auto result = dir.addContact(c);

    switch (result) {
        case PhoneDirectory::AddResult::SUCCESS:
            cout << "\nContact added successfully!\n"; break;
        case PhoneDirectory::AddResult::DUPLICATE_NAME:
            cout << "\nA contact with this name already exists. Try updating it instead.\n"; break;
        case PhoneDirectory::AddResult::DUPLICATE_PHONE:
            cout << "\nThis phone number is already assigned to another contact.\n"; break;
        default:
            cout << "\nInvalid input. Contact not added.\n"; break;
    }
}

void actionSearchByName(PhoneDirectory& dir) {
    cout << "\n== Search by Name ==\n";
    string name = readLine("Enter name: ");
    Contact c;
    if (dir.searchByName(name, c)) {
        cout << "\nContact found:\n";
        printContact(c);
        dir.logRecentView(c.name);
    } else {
        cout << "\nNo contact found with that name.\n";
        // offer autocomplete suggestions as a helpful fallback
        auto suggestions = dir.autocomplete(name);
        if (!suggestions.empty()) {
            cout << "Did you mean:\n";
            for (auto& s : suggestions) cout << "  - " << s << "\n";
        }
    }
}

void actionSearchByPhone(PhoneDirectory& dir) {
    cout << "\n== Search by Phone Number ==\n";
    string phone = readLine("Enter phone number: ");
    Contact c;
    if (dir.searchByPhone(phone, c)) {
        cout << "\nContact found:\n";
        printContact(c);
        dir.logRecentView(c.name);
    } else {
        cout << "\nNo contact found with that phone number.\n";
    }
}

void actionAutocomplete(PhoneDirectory& dir) {
    cout << "\n== Autocomplete Search (Trie) ==\n";
    string prefix = readLine("Start typing a name: ");
    auto results = dir.autocomplete(prefix);
    if (results.empty()) {
        cout << "No matching contacts.\n";
        return;
    }
    cout << "\nMatches:\n";
    for (auto& name : results) {
        Contact c;
        if (dir.searchByName(name, c)) printContactRow(c);
    }
}

void actionUpdateContact(PhoneDirectory& dir) {
    cout << "\n== Update Contact ==\n";
    string name = readLine("Enter the name of the contact to update: ");
    Contact existing;
    if (!dir.searchByName(name, existing)) {
        cout << "No contact found with that name.\n";
        return;
    }

    cout << "Leave a field blank to keep its current value.\n";
    printContact(existing);

    string newName = readLine("New name [" + existing.name + "]: ");
    string newPhone = readLine("New phone [" + existing.phone + "]: ");
    string newEmail = readLine("New email [" + (existing.email.empty() ? "-" : existing.email) + "]: ");
    string newAddress = readLine("New address [" + (existing.address.empty() ? "-" : existing.address) + "]: ");

    Contact updated = existing;
    if (!newName.empty()) updated.name = newName;
    if (!newPhone.empty()) {
        if (!isValidPhone(newPhone)) { cout << "Invalid phone number. Update cancelled.\n"; return; }
        updated.phone = newPhone;
    }
    if (!newEmail.empty()) {
        if (!isValidEmail(newEmail)) { cout << "Invalid email format. Update cancelled.\n"; return; }
        updated.email = newEmail;
    }
    if (!newAddress.empty()) updated.address = newAddress;

    cout << "Update category? (y/n): ";
    string yn; getline(cin, yn);
    if (!yn.empty() && tolower(yn[0]) == 'y') {
        cout << "New category:\n";
        updated.category = chooseCategory();
    }

    if (dir.updateContact(name, updated)) {
        cout << "\nContact updated successfully!\n";
        printContact(updated);
    } else {
        cout << "\nUpdate failed.\n";
    }
}

void actionDeleteContact(PhoneDirectory& dir) {
    cout << "\n== Delete Contact ==\n";
    string name = readLine("Enter the name of the contact to delete: ");
    Contact deleted;
    if (dir.deleteContact(name, deleted)) {
        cout << "\nDeleted:\n";
        printContact(deleted);
        cout << "(You can undo this from the main menu.)\n";
    } else {
        cout << "\nNo contact found with that name.\n";
    }
}

void actionUndo(PhoneDirectory& dir) {
    cout << "\n== Undo Last Delete ==\n";
    if (!dir.hasUndoAvailable()) {
        cout << "Nothing to undo.\n";
        return;
    }
    Contact restored;
    if (dir.undoDelete(restored)) {
        cout << "Restored contact:\n";
        printContact(restored);
    } else {
        cout << "Could not restore: a conflicting name/phone now exists.\n";
    }
}

void actionDisplayAll(PhoneDirectory& dir) {
    cout << "\n== All Contacts (sorted alphabetically) ==\n";
    auto all = dir.getAllSortedByName();
    if (all.empty()) {
        cout << "The directory is empty.\n";
        return;
    }
    cout << left << setw(20) << "Name" << setw(16) << "Phone"
         << setw(10) << "Category" << "Email" << "\n";
    printDivider();
    for (auto& c : all) printContactRow(c);
    cout << "\nTotal contacts: " << all.size() << "\n";
}

void actionRecent(PhoneDirectory& dir) {
    cout << "\n== Recently Viewed Contacts ==\n";
    auto recent = dir.getRecent();
    if (recent.empty()) {
        cout << "No recent activity yet.\n";
        return;
    }
    int i = 1;
    for (auto& name : recent) {
        Contact c;
        if (dir.searchByName(name, c)) {
            cout << i++ << ". ";
            printContactRow(c);
        }
    }
}

void printMenu(int totalContacts) {
    cout << "\n";
    printDivider();
    cout << "   DIGITAL PHONE DIRECTORY   (" << totalContacts << " contacts)\n";
    printDivider();
    cout << " 1. Add Contact\n"
         << " 2. Search by Name\n"
         << " 3. Search by Phone Number\n"
         << " 4. Autocomplete Search\n"
         << " 5. Update Contact\n"
         << " 6. Delete Contact\n"
         << " 7. Undo Last Delete\n"
         << " 8. Display All Contacts (A-Z)\n"
         << " 9. Recently Viewed\n"
         << " 0. Exit\n";
    printDivider();
}

int main() {
    PhoneDirectory dir("contacts.txt");

    cout << "Welcome to your Digital Phone Directory.\n";
    cout << "Loaded " << dir.totalContacts() << " existing contact(s) from contacts.txt.\n";

    bool running = true;
    while (running) {
        printMenu(dir.totalContacts());
        int choice = readMenuChoice(0, 9);

        switch (choice) {
            case 1: actionAddContact(dir); break;
            case 2: actionSearchByName(dir); break;
            case 3: actionSearchByPhone(dir); break;
            case 4: actionAutocomplete(dir); break;
            case 5: actionUpdateContact(dir); break;
            case 6: actionDeleteContact(dir); break;
            case 7: actionUndo(dir); break;
            case 8: actionDisplayAll(dir); break;
            case 9: actionRecent(dir); break;
            case 0:
                dir.saveToFile();
                cout << "\nAll changes saved to contacts.txt. Goodbye!\n";
                running = false;
                break;
        }
    }
    return 0;
}
