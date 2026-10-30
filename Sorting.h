#ifndef SORTING_H
#define SORTING_H

#include <vector>
#include <algorithm>
#include <cctype>
#include "Contact.h"

// ------------------------------------------------------------
// Merge sort on contacts by name (case-insensitive), O(n log n).
// Used purely for producing a nice alphabetical listing --
// the hash table itself keeps no order, so we sort on demand.
// ------------------------------------------------------------
namespace SortUtil {

    inline std::string lower(const std::string& s) {
        std::string r = s;
        for (char& c : r) c = (char)tolower((unsigned char)c);
        return r;
    }

    inline void merge(std::vector<Contact>& arr, int left, int mid, int right) {
        std::vector<Contact> temp;
        temp.reserve(right - left + 1);
        int i = left, j = mid + 1;

        while (i <= mid && j <= right) {
            if (lower(arr[i].name) <= lower(arr[j].name)) temp.push_back(arr[i++]);
            else temp.push_back(arr[j++]);
        }
        while (i <= mid)  temp.push_back(arr[i++]);
        while (j <= right) temp.push_back(arr[j++]);

        for (int k = 0; k < (int)temp.size(); ++k) arr[left + k] = temp[k];
    }

    inline void mergeSort(std::vector<Contact>& arr, int left, int right) {
        if (left >= right) return;
        int mid = left + (right - left) / 2;
        mergeSort(arr, left, mid);
        mergeSort(arr, mid + 1, right);
        merge(arr, left, mid, right);
    }

    inline void sortByName(std::vector<Contact>& contacts) {
        if (!contacts.empty())
            mergeSort(contacts, 0, (int)contacts.size() - 1);
    }
}

#endif
