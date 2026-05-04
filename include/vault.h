#pragma once
#ifndef VAULT_H
#define VAULT_H

#include <string>
#include <vector>
// #include <optional> // Not available in MinGW 6.3.0

// Forward-declare to avoid circular include
class GCSClient;

// Simple optional implementation
template<typename T>
class Optional {
public:
    Optional() : hasValue(false) {}
    Optional(const T& val) : hasValue(true), value(val) {}
    bool has_value() const { return hasValue; }
    operator bool() const { return hasValue; }
    const T& operator*() const { return value; }
    const T* operator->() const { return &value; }
private:
    bool hasValue;
    T value;
};

#include <functional>

// ─── Data model ──────────────────────────────────────────────────────────────
struct Entry {
    int         id;
    std::string website;
    std::string username;
    std::string password;
    std::string notes;
    std::string category;
    std::string createdAt;
    std::string updatedAt;
};

// ─── Vault ───────────────────────────────────────────────────────────────────
class Vault {
public:
    explicit Vault(const std::string& filePath);

    // Attach a GCS client for cloud sync (optional)
    void setGCSClient(GCSClient* gcs) { gcs_ = gcs; }

    // Call once after master-password verification
    // With GCS attached: downloads vault.dat from cloud before decrypting
    bool load(const std::string& masterKey);
    // With GCS attached: uploads encrypted vault.dat to cloud after saving
    bool save(const std::string& masterKey);

    // Explicit cloud sync helpers
    bool syncFromCloud(const std::string& masterKey);
    bool syncToCloud();
    bool isCloudEnabled() const;

    // CRUD
    int         addEntry(Entry e);                    // returns assigned id
    bool        updateEntry(const Entry& e);
    bool        deleteEntry(int id);
    Optional<Entry> getEntry(int id) const;

    // Query
    std::vector<Entry> allEntries()                                const;
    std::vector<Entry> search(const std::string& query)            const;
    std::vector<Entry> byCategory(const std::string& cat)          const;

    // Iteration
    void forEach(std::function<void(const Entry&)> fn)             const;

    bool isDirty() const { return dirty_; }

private:
    std::string         filePath_;
    std::vector<Entry>  entries_;
    int                 nextId_  = 1;
    bool                dirty_   = false;
    GCSClient*          gcs_     = nullptr;  // non-owning ptr

    std::string serialize()                                        const;
    bool        deserialize(const std::string& plain);
};

#endif // VAULT_H
