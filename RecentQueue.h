#ifndef RECENTQUEUE_H
#define RECENTQUEUE_H

#include <string>
#include <vector>

// ------------------------------------------------------------
// RecentQueue
// A fixed-size circular queue (built on a plain array) that
// remembers the last N contacts the user looked up -- like the
// "recent calls" list on a real phone. Newest entries push out
// the oldest once the buffer is full.
// ------------------------------------------------------------
class RecentQueue {
private:
    std::vector<std::string> buffer;
    int frontIdx, rearIdx, currentSize, maxSize;

public:
    RecentQueue(int size = 5) : buffer(size), frontIdx(0), rearIdx(-1),
                                  currentSize(0), maxSize(size) {}

    void enqueue(const std::string& name) {
        // avoid duplicate back-to-back entries of the same contact
        if (currentSize > 0 && buffer[rearIdx] == name) return;

        if (currentSize == maxSize) {
            // drop oldest (advance front) to make room -- circular behavior
            frontIdx = (frontIdx + 1) % maxSize;
            currentSize--;
        }
        rearIdx = (rearIdx + 1) % maxSize;
        buffer[rearIdx] = name;
        currentSize++;
    }

    // Returns most-recent-first
    std::vector<std::string> getAll() const {
        std::vector<std::string> result;
        for (int i = 0; i < currentSize; ++i) {
            int idx = (rearIdx - i + maxSize) % maxSize;
            result.push_back(buffer[idx]);
        }
        return result;
    }
};

#endif
