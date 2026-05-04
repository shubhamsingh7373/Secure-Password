// ═══════════════════════════════════════════════════════════════════════════════
//  GCS Client – Google Cloud Storage REST API
//  Uses WinHTTP for HTTPS and Windows CNG for RS256 JWT signing.
//  No libcurl, no OpenSSL, no external SDK required.
// ═══════════════════════════════════════════════════════════════════════════════

#include "gcs_client.h"
#include <nlohmann/json.hpp>
#include <windows.h>
#include <winhttp.h>
#include <wincrypt.h>
#include <bcrypt.h>
#include <fstream>
#include <sstream>
#include <stdexcept>
#include <chrono>
#include <iostream>
#include <vector>
#include <algorithm>

#pragma comment(lib, "winhttp.lib")
#pragma comment(lib, "bcrypt.lib")
#pragma comment(lib, "crypt32.lib")

using json = nlohmann::json;

// ─── Helpers ──────────────────────────────────────────────────────────────────

static std::string readFile(const std::string& path) {
    std::ifstream f(path, std::ios::binary);
    if (!f.is_open()) return "";
    return std::string((std::istreambuf_iterator<char>(f)),
                        std::istreambuf_iterator<char>());
}

static std::wstring toWide(const std::string& s) {
    if (s.empty()) return L"";
    int sz = MultiByteToWideChar(CP_UTF8, 0, s.c_str(), -1, nullptr, 0);
    std::vector<wchar_t> buf(sz);
    MultiByteToWideChar(CP_UTF8, 0, s.c_str(), -1, buf.data(), sz);
    return std::wstring(buf.data());
}

static long long unixNow() {
    return (long long)std::chrono::duration_cast<std::chrono::seconds>(
        std::chrono::system_clock::now().time_since_epoch()).count();
}

// ─── Base64 / Base64-URL ──────────────────────────────────────────────────────

static const char B64_TABLE[] =
    "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";

std::string GCSClient::base64urlEncode(const unsigned char* data, size_t len) {
    std::string out;
    out.reserve(((len + 2) / 3) * 4);
    for (size_t i = 0; i < len; i += 3) {
        unsigned char b0 = data[i];
        unsigned char b1 = (i + 1 < len) ? data[i + 1] : 0;
        unsigned char b2 = (i + 2 < len) ? data[i + 2] : 0;
        out += B64_TABLE[b0 >> 2];
        out += B64_TABLE[((b0 & 0x3) << 4) | (b1 >> 4)];
        out += (i + 1 < len) ? B64_TABLE[((b1 & 0xf) << 2) | (b2 >> 6)] : '=';
        out += (i + 2 < len) ? B64_TABLE[b2 & 0x3f] : '=';
    }
    // Convert to base64url (RFC 4648)
    for (auto& c : out) {
        if (c == '+') c = '-';
        else if (c == '/') c = '_';
    }
    // Remove padding
    while (!out.empty() && out.back() == '=') out.pop_back();
    return out;
}

std::string GCSClient::base64url(const std::string& input) {
    return base64urlEncode(
        reinterpret_cast<const unsigned char*>(input.data()), input.size());
}

// Standard base64 decode (for PEM key parsing)
static std::vector<unsigned char> base64Decode(const std::string& in) {
    static const int decode_table[256] = {
        -1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,
        -1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,
        -1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,62,-1,-1,-1,63,
        52,53,54,55,56,57,58,59,60,61,-1,-1,-1,-1,-1,-1,
        -1, 0, 1, 2, 3, 4, 5, 6, 7, 8, 9,10,11,12,13,14,
        15,16,17,18,19,20,21,22,23,24,25,-1,-1,-1,-1,-1,
        -1,26,27,28,29,30,31,32,33,34,35,36,37,38,39,40,
        41,42,43,44,45,46,47,48,49,50,51,-1,-1,-1,-1,-1,
        -1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,
        -1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,
        -1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,
        -1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,
        -1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,
        -1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,
        -1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,
        -1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1
    };
    std::vector<unsigned char> out;
    int val = 0, bits = -8;
    for (unsigned char c : in) {
        if (decode_table[c] == -1) continue;
        val = (val << 6) + decode_table[c];
        bits += 6;
        if (bits >= 0) {
            out.push_back((unsigned char)((val >> bits) & 0xff));
            bits -= 8;
        }
    }
    return out;
}

// ─── Service Account JSON loader ──────────────────────────────────────────────

bool GCSClient::loadServiceAccount(std::string& clientEmail,
                                    std::string& privateKey) const {
    std::string raw = readFile(cfg_.serviceAccountJson);
    if (raw.empty()) return false;
    try {
        auto j = json::parse(raw);
        clientEmail = j.at("client_email").get<std::string>();
        privateKey  = j.at("private_key").get<std::string>();
        return true;
    } catch (...) {
        return false;
    }
}

// ─── JWT RS256 signing via Windows CNG ───────────────────────────────────────

std::string GCSClient::makeJWT(const std::string& clientEmail,
                                const std::string& privateKeyPem) const {
    long long now = unixNow();
    long long exp = now + 3600;

    // Header
    json hdr = {{"alg", "RS256"}, {"typ", "JWT"}};
    std::string headerB64 = base64url(hdr.dump());

    // Payload
    json payload = {
        {"iss",   clientEmail},
        {"scope", "https://www.googleapis.com/auth/devstorage.read_write"},
        {"aud",   "https://oauth2.googleapis.com/token"},
        {"exp",   exp},
        {"iat",   now}
    };
    std::string payloadB64 = base64url(payload.dump());

    std::string sigInput = headerB64 + "." + payloadB64;

    // ── Strip PEM headers and decode DER ─────────────────────────────────────
    std::string pemBody = privateKeyPem;
    // Remove -----BEGIN/END PRIVATE KEY----- and newlines
    auto stripPem = [](std::string& pem) {
        std::string out;
        bool inHeader = false;
        for (auto& line : { std::string("\n") }) { (void)line; }
        std::istringstream ss(pem);
        std::string line;
        while (std::getline(ss, line)) {
            if (line.find("-----") != std::string::npos) continue;
            for (char c : line) if (c != '\r' && c != '\n') out += c;
        }
        pem = out;
    };
    stripPem(pemBody);
    std::vector<unsigned char> der = base64Decode(pemBody);

    // ── Import RSA private key via CNG ────────────────────────────────────────
    HCRYPTPROV hProv = 0;
    HCRYPTKEY  hKey  = 0;
    DWORD      cbPrivateKeyBlob = 0;
    BYTE*      pbPrivateKeyBlob = nullptr;

    // Convert PKCS#8 DER to PRIVATEKEYBLOB using CryptDecodeObjectEx
    if (!CryptDecodeObjectEx(
            X509_ASN_ENCODING | PKCS_7_ASN_ENCODING,
            PKCS_RSA_PRIVATE_KEY,
            der.data(), (DWORD)der.size(),
            CRYPT_DECODE_ALLOC_FLAG, nullptr,
            &pbPrivateKeyBlob, &cbPrivateKeyBlob)) {

        // Try PKCS#8 wrapper
        CRYPT_PRIVATE_KEY_INFO* pki = nullptr;
        DWORD                   pkiLen = 0;
        if (!CryptDecodeObjectEx(
                X509_ASN_ENCODING | PKCS_7_ASN_ENCODING,
                PKCS_PRIVATE_KEY_INFO,
                der.data(), (DWORD)der.size(),
                CRYPT_DECODE_ALLOC_FLAG, nullptr,
                &pki, &pkiLen)) {
            throw std::runtime_error("GCS: Cannot decode private key DER");
        }
        if (!CryptDecodeObjectEx(
                X509_ASN_ENCODING | PKCS_7_ASN_ENCODING,
                PKCS_RSA_PRIVATE_KEY,
                pki->PrivateKey.pbData, pki->PrivateKey.cbData,
                CRYPT_DECODE_ALLOC_FLAG, nullptr,
                &pbPrivateKeyBlob, &cbPrivateKeyBlob)) {
            LocalFree(pki);
            throw std::runtime_error("GCS: Cannot decode RSA key from PKCS#8");
        }
        LocalFree(pki);
    }

    if (!CryptAcquireContextA(&hProv, nullptr, nullptr, PROV_RSA_FULL,
                               CRYPT_VERIFYCONTEXT)) {
        LocalFree(pbPrivateKeyBlob);
        throw std::runtime_error("GCS: CryptAcquireContext failed");
    }
    if (!CryptImportKey(hProv, pbPrivateKeyBlob, cbPrivateKeyBlob, 0, 0, &hKey)) {
        LocalFree(pbPrivateKeyBlob);
        CryptReleaseContext(hProv, 0);
        throw std::runtime_error("GCS: CryptImportKey failed");
    }
    LocalFree(pbPrivateKeyBlob);

    // ── Hash the signing input (SHA-256) ──────────────────────────────────────
    HCRYPTHASH hHash = 0;
    if (!CryptCreateHash(hProv, CALG_SHA_256, 0, 0, &hHash)) {
        CryptDestroyKey(hKey); CryptReleaseContext(hProv, 0);
        throw std::runtime_error("GCS: CryptCreateHash failed");
    }
    if (!CryptHashData(hHash,
                       reinterpret_cast<const BYTE*>(sigInput.data()),
                       (DWORD)sigInput.size(), 0)) {
        CryptDestroyHash(hHash); CryptDestroyKey(hKey); CryptReleaseContext(hProv, 0);
        throw std::runtime_error("GCS: CryptHashData failed");
    }

    // ── Sign ──────────────────────────────────────────────────────────────────
    DWORD sigLen = 0;
    CryptSignHash(hHash, AT_KEYEXCHANGE, nullptr, 0, nullptr, &sigLen);
    std::vector<unsigned char> sig(sigLen);
    if (!CryptSignHash(hHash, AT_KEYEXCHANGE, nullptr, 0, sig.data(), &sigLen)) {
        CryptDestroyHash(hHash); CryptDestroyKey(hKey); CryptReleaseContext(hProv, 0);
        throw std::runtime_error("GCS: CryptSignHash failed");
    }
    // CryptoAPI returns signature in little-endian; RSA PKCS#1 expects big-endian
    std::reverse(sig.begin(), sig.end());

    CryptDestroyHash(hHash);
    CryptDestroyKey(hKey);
    CryptReleaseContext(hProv, 0);

    std::string sigB64 = base64urlEncode(sig.data(), sig.size());
    return sigInput + "." + sigB64;
}

// ─── OAuth2 token refresh ──────────────────────────────────────────────────────

bool GCSClient::refreshToken() {
    std::string email, key;
    if (!loadServiceAccount(email, key)) {
        std::cerr << "[GCS] Cannot read service account JSON: "
                  << cfg_.serviceAccountJson << "\n";
        return false;
    }

    std::string jwt;
    try {
        jwt = makeJWT(email, key);
    } catch (const std::exception& ex) {
        std::cerr << "[GCS] JWT signing failed: " << ex.what() << "\n";
        return false;
    }

    // POST to Google token endpoint
    std::string body = "grant_type=urn%3Aietf%3Aparams%3Aoauth%3Agrant-type%3Ajwt-bearer&assertion=" + jwt;
    GCSResult res = httpPut(
        "https://oauth2.googleapis.com/token",
        "", body, "application/x-www-form-urlencoded");

    // Note: we abuse httpPut here as a generic POST
    if (!res.ok || res.data.empty()) {
        std::cerr << "[GCS] Token exchange failed: " << res.error << "\n";
        return false;
    }

    try {
        auto j = json::parse(res.data);
        if (!j.contains("access_token")) {
            std::cerr << "[GCS] No access_token in response: " << res.data << "\n";
            return false;
        }
        accessToken_ = j["access_token"].get<std::string>();
        long long expiresIn = j.value("expires_in", 3600LL);
        tokenExpiry_ = unixNow() + expiresIn - 60; // 60s buffer
        return true;
    } catch (...) {
        std::cerr << "[GCS] JSON parse error on token response\n";
        return false;
    }
}

// ─── WinHTTP helpers ─────────────────────────────────────────────────────────

static GCSResult winhttp(
    const std::wstring& method,
    const std::wstring& host,
    const std::wstring& path,
    const std::wstring& token,
    const std::string&  requestBody,
    const std::wstring& contentType)
{
    GCSResult res;

    HINTERNET hSession = WinHttpOpen(
        L"HealixVault/1.0",
        WINHTTP_ACCESS_TYPE_DEFAULT_PROXY,
        WINHTTP_NO_PROXY_NAME, WINHTTP_NO_PROXY_BYPASS, 0);
    if (!hSession) { res.error = "WinHttpOpen failed"; return res; }

    HINTERNET hConnect = WinHttpConnect(hSession, host.c_str(),
                                         INTERNET_DEFAULT_HTTPS_PORT, 0);
    if (!hConnect) {
        WinHttpCloseHandle(hSession);
        res.error = "WinHttpConnect failed";
        return res;
    }

    HINTERNET hReq = WinHttpOpenRequest(
        hConnect, method.c_str(), path.c_str(),
        nullptr, WINHTTP_NO_REFERER, WINHTTP_DEFAULT_ACCEPT_TYPES,
        WINHTTP_FLAG_SECURE);
    if (!hReq) {
        WinHttpCloseHandle(hConnect); WinHttpCloseHandle(hSession);
        res.error = "WinHttpOpenRequest failed";
        return res;
    }

    // Authorization header
    if (!token.empty()) {
        std::wstring authHeader = L"Authorization: Bearer " + token;
        WinHttpAddRequestHeaders(hReq, authHeader.c_str(), (DWORD)-1,
                                  WINHTTP_ADDREQ_FLAG_ADD);
    }

    // Content-Type
    if (!contentType.empty()) {
        std::wstring ctHeader = L"Content-Type: " + contentType;
        WinHttpAddRequestHeaders(hReq, ctHeader.c_str(), (DWORD)-1,
                                  WINHTTP_ADDREQ_FLAG_ADD);
    }

    BOOL sent = WinHttpSendRequest(
        hReq, WINHTTP_NO_ADDITIONAL_HEADERS, 0,
        requestBody.empty() ? nullptr : (LPVOID)requestBody.data(),
        (DWORD)requestBody.size(),
        (DWORD)requestBody.size(), 0);

    if (!sent || !WinHttpReceiveResponse(hReq, nullptr)) {
        WinHttpCloseHandle(hReq);
        WinHttpCloseHandle(hConnect);
        WinHttpCloseHandle(hSession);
        res.error = "HTTP send/receive failed";
        return res;
    }

    // Check HTTP status
    DWORD statusCode = 0;
    DWORD statusLen  = sizeof(statusCode);
    WinHttpQueryHeaders(hReq,
        WINHTTP_QUERY_STATUS_CODE | WINHTTP_QUERY_FLAG_NUMBER,
        WINHTTP_HEADER_NAME_BY_INDEX, &statusCode, &statusLen,
        WINHTTP_NO_HEADER_INDEX);

    // Read body
    std::string body;
    DWORD bytesAvail = 0;
    while (WinHttpQueryDataAvailable(hReq, &bytesAvail) && bytesAvail > 0) {
        std::vector<char> buf(bytesAvail + 1, 0);
        DWORD bytesRead = 0;
        WinHttpReadData(hReq, buf.data(), bytesAvail, &bytesRead);
        body.append(buf.data(), bytesRead);
    }

    WinHttpCloseHandle(hReq);
    WinHttpCloseHandle(hConnect);
    WinHttpCloseHandle(hSession);

    res.data = body;
    res.ok   = (statusCode >= 200 && statusCode < 300);
    if (!res.ok) {
        res.error = "HTTP " + std::to_string(statusCode) + ": " + body.substr(0, 256);
    }
    return res;
}

GCSResult GCSClient::httpGet(const std::string& url, const std::string& token) {
    // Parse https://host/path
    std::string noScheme = url.substr(8); // strip "https://"
    auto slash = noScheme.find('/');
    std::wstring host = toWide(noScheme.substr(0, slash));
    std::wstring path = toWide(noScheme.substr(slash));
    return winhttp(L"GET", host, path, toWide(token), "", L"");
}

GCSResult GCSClient::httpPut(const std::string& url, const std::string& token,
                               const std::string& body,
                               const std::string& contentType) {
    std::string noScheme = url.substr(8);
    auto slash = noScheme.find('/');
    std::wstring host = toWide(noScheme.substr(0, slash));
    std::wstring path = toWide(noScheme.substr(slash));
    std::wstring method = L"POST"; // token exchange uses POST
    // Use PUT for GCS uploads (object insert)
    if (url.find("googleapis.com/upload") != std::string::npos ||
        url.find("storage.googleapis.com") != std::string::npos) {
        method = L"POST"; // multipart media upload uses POST
    }
    if (url.find("oauth2.googleapis.com") == std::string::npos) {
        method = L"POST";
    }
    return winhttp(method, host, path, toWide(token), body, toWide(contentType));
}

// ─── GCS upload/download ──────────────────────────────────────────────────────

GCSResult GCSClient::upload(const std::string& data) {
    if (unixNow() >= tokenExpiry_) {
        if (!refreshToken()) {
            GCSResult r;
            r.error = "Failed to obtain GCS access token";
            return r;
        }
    }

    // GCS JSON API – simple upload (≤5 MB objects)
    // POST https://storage.googleapis.com/upload/storage/v1/b/{bucket}/o?uploadType=media&name={object}
    std::string url =
        "https://storage.googleapis.com/upload/storage/v1/b/" +
        cfg_.bucketName + "/o?uploadType=media&name=" + cfg_.objectName;

    // Use WinHTTP directly for binary-safe PUT/POST
    std::string noScheme = url.substr(8);
    auto slash = noScheme.find('/');
    std::wstring host = toWide(noScheme.substr(0, slash));
    std::wstring path = toWide(noScheme.substr(slash));

    return winhttp(L"POST", host, path, toWide(accessToken_),
                   data, L"application/octet-stream");
}

GCSResult GCSClient::download() {
    if (unixNow() >= tokenExpiry_) {
        if (!refreshToken()) {
            GCSResult r;
            r.error = "Failed to obtain GCS access token";
            return r;
        }
    }

    // GET https://storage.googleapis.com/storage/v1/b/{bucket}/o/{object}?alt=media
    std::string url =
        "https://storage.googleapis.com/storage/v1/b/" +
        cfg_.bucketName + "/o/" + cfg_.objectName + "?alt=media";

    return httpGet(url, accessToken_);
}

bool GCSClient::ping() {
    if (unixNow() >= tokenExpiry_) {
        return refreshToken();
    }
    return !accessToken_.empty();
}

// ─── GCSClient constructor ────────────────────────────────────────────────────

GCSClient::GCSClient(const GCSConfig& cfg)
    : cfg_(cfg), tokenExpiry_(0) {
    if (cfg_.enabled) {
        std::cout << "[GCS] Connecting to bucket: " << cfg_.bucketName << "\n";
        if (!refreshToken()) {
            std::cerr << "[GCS] WARNING: Could not authenticate with Google Cloud.\n"
                      << "[GCS] Falling back to local-only storage.\n";
            cfg_.enabled = false;
        } else {
            std::cout << "[GCS] ✓ Connected to Google Cloud Storage\n";
        }
    }
}

// ─── Config loader ────────────────────────────────────────────────────────────

GCSConfig loadGCSConfig(const std::string& configPath) {
    GCSConfig cfg;
    std::string raw = readFile(configPath);
    if (raw.empty()) return cfg;
    try {
        auto j = json::parse(raw);
        cfg.enabled            = j.value("enabled", false);
        cfg.bucketName         = j.value("bucket_name", "");
        cfg.objectName         = j.value("object_name", "vault.dat");
        cfg.serviceAccountJson = j.value("service_account_key", "");
        if (cfg.bucketName.empty()) cfg.enabled = false;
    } catch (const std::exception& ex) {
        std::cerr << "[GCS] Config parse error: " << ex.what() << "\n";
    }
    return cfg;
}
