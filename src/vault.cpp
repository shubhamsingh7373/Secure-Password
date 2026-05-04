#include "vault.h"
#include "crypto.h"
#include <fstream>
#include <sstream>
#include <algorithm>
#include <stdexcept>
#include <chrono>
#include <ctime>

// ─── Timestamp helper ────────────────────────────────────────────────────────
static std::string nowTimestamp() {
    auto now = std::chrono::system_clock::now();
    std::time_t t = std::chrono::system_clock::to_time_t(now);
    char buf[32];
    std::strftime(buf, sizeof(buf), "%Y-%m-%d %H:%M:%S", std::localtime(&t));
    return buf;
}

// ─── CSV-style escaping (simple | delimiter) ─────────────────────────────────
static std::string esc(const std::string& s) {
    std::string out;
    for(char c : s) {
        if(c=='\\') out += "\\\\";
        else if(c=='|') out += "\\|";
        else if(c=='\n') out += "\\n";
        else out += c;
    }
    return out;
}
static std::string unesc(const std::string& s) {
    std::string out;
    for(size_t i=0; i<s.size(); ++i) {
        if(s[i]=='\\' && i+1<s.size()) {
            char nx=s[i+1];
            if(nx=='\\') { out+='\\'; ++i; }
            else if(nx=='|') { out+='|'; ++i; }
            else if(nx=='n') { out+='\n'; ++i; }
            else out+=s[i];
        } else out+=s[i];
    }
    return out;
}

// ─── Vault ────────────────────────────────────────────────────────────────────
Vault::Vault(const std::string& filePath) : filePath_(filePath) {}

bool Vault::load(const std::string& masterKey) {
    std::ifstream f(filePath_);
    if(!f.is_open()) return true; // fresh vault is fine

    std::string encrypted((std::istreambuf_iterator<char>(f)),
                           std::istreambuf_iterator<char>());
    if(encrypted.empty()) return true;

    try {
        std::string plain = Crypto::aes256Decrypt(encrypted, masterKey);
        return deserialize(plain);
    } catch(...) {
        return false; // wrong key or corrupt
    }
}

bool Vault::save(const std::string& masterKey) {
    std::string plain = serialize();
    std::string encrypted = Crypto::aes256Encrypt(plain, masterKey);
    std::ofstream f(filePath_);
    if(!f.is_open()) return false;
    f << encrypted;
    dirty_ = false;
    return true;
}

// ─── CRUD ─────────────────────────────────────────────────────────────────────
int Vault::addEntry(Entry e) {
    e.id        = nextId_++;
    e.createdAt = nowTimestamp();
    e.updatedAt = e.createdAt;
    entries_.push_back(e);
    dirty_ = true;
    return e.id;
}

bool Vault::updateEntry(const Entry& e) {
    for(auto& en : entries_) {
        if(en.id == e.id) {
            en = e;
            en.updatedAt = nowTimestamp();
            dirty_ = true;
            return true;
        }
    }
    return false;
}

bool Vault::deleteEntry(int id) {
    auto it = std::find_if(entries_.begin(), entries_.end(),
                           [id](const Entry& e){ return e.id == id; });
    if(it == entries_.end()) return false;
    entries_.erase(it);
    dirty_ = true;
    return true;
}

Optional<Entry> Vault::getEntry(int id) const {
    for(auto& e : entries_)
        if(e.id == id) return Optional<Entry>(e);
    return Optional<Entry>();
}

// ─── Query ───────────────────────────────────────────────────────────────────
std::vector<Entry> Vault::allEntries() const { return entries_; }

std::vector<Entry> Vault::search(const std::string& query) const {
    std::string q = query;
    std::transform(q.begin(), q.end(), q.begin(), ::tolower);
    std::vector<Entry> res;
    for(auto& e : entries_) {
        auto lower = [](std::string s){
            std::transform(s.begin(),s.end(),s.begin(),::tolower);
            return s;
        };
        if(lower(e.website).find(q)!=std::string::npos ||
           lower(e.username).find(q)!=std::string::npos ||
           lower(e.category).find(q)!=std::string::npos ||
           lower(e.notes).find(q)!=std::string::npos)
            res.push_back(e);
    }
    return res;
}

std::vector<Entry> Vault::byCategory(const std::string& cat) const {
    std::vector<Entry> res;
    for(auto& e : entries_)
        if(e.category == cat) res.push_back(e);
    return res;
}

void Vault::forEach(std::function<void(const Entry&)> fn) const {
    for(auto& e : entries_) fn(e);
}

// ─── Serialization (pipe-delimited plaintext inside encryption) ──────────────
std::string Vault::serialize() const {
    std::ostringstream oss;
    oss << "NEXTID:" << nextId_ << "\n";
    for(auto& e : entries_) {
        oss << e.id        << "|"
            << esc(e.website)  << "|"
            << esc(e.username) << "|"
            << esc(e.password) << "|"
            << esc(e.notes)    << "|"
            << esc(e.category) << "|"
            << esc(e.createdAt)<< "|"
            << esc(e.updatedAt)<< "\n";
    }
    return oss.str();
}

bool Vault::deserialize(const std::string& plain) {
    entries_.clear();
    nextId_ = 1;
    std::istringstream iss(plain);
    std::string line;
    while(std::getline(iss, line)) {
        if(line.empty()) continue;
        if(line.rfind("NEXTID:", 0) == 0) {
            nextId_ = std::stoi(line.substr(7));
            continue;
        }
        // Split by unescaped '|'
        std::vector<std::string> fields;
        std::string cur;
        for(size_t i=0; i<line.size(); ++i) {
            if(line[i]=='\\' && i+1<line.size()) {
                cur += line[i]; cur += line[i+1]; ++i;
            } else if(line[i]=='|') {
                fields.push_back(unesc(cur)); cur.clear();
            } else {
                cur += line[i];
            }
        }
        fields.push_back(unesc(cur));
        if(fields.size() < 8) continue;
        Entry e;
        e.id        = std::stoi(fields[0]);
        e.website   = fields[1];
        e.username  = fields[2];
        e.password  = fields[3];
        e.notes     = fields[4];
        e.category  = fields[5];
        e.createdAt = fields[6];
        e.updatedAt = fields[7];
        entries_.push_back(e);
    }
    return true;
}
