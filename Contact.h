#ifndef CONTACT_H
#define CONTACT_H

#include <string>
#include <sstream>

// ------------------------------------------------------------
// Contact: one entry in the phone directory
// ------------------------------------------------------------
struct Contact {
    std::string name;
    std::string phone;
    std::string email;
    std::string address;
    std::string category;   // Family / Friend / Work / Other

    Contact() {}
    Contact(std::string n, std::string p, std::string e,
            std::string a, std::string c)
        : name(n), phone(p), email(e), address(a), category(c) {}

    // Convert to a single pipe-delimited line for file storage
    std::string serialize() const {
        std::ostringstream oss;
        oss << name << '|' << phone << '|' << email << '|'
            << address << '|' << category;
        return oss.str();
    }

    // Rebuild a Contact from a stored line
    static Contact deserialize(const std::string& line) {
        Contact c;
        std::stringstream ss(line);
        std::getline(ss, c.name, '|');
        std::getline(ss, c.phone, '|');
        std::getline(ss, c.email, '|');
        std::getline(ss, c.address, '|');
        std::getline(ss, c.category, '|');
        return c;
    }
};

#endif
