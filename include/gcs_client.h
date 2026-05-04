#pragma once
#ifndef GCS_CLIENT_H
#define GCS_CLIENT_H

// ═══════════════════════════════════════════════════════════════════════════════
//  GCS Client – Google Cloud Storage via JSON REST API (WinHTTP)
//  Uploads / downloads the encrypted vault.dat blob using a Service Account
//  access token obtained via OAuth2 JWT flow.
// ═══════════════════════════════════════════════════════════════════════════════

#include <string>

// ─── GCS Configuration ────────────────────────────────────────────────────────
struct GCSConfig {
    std::string bucketName;        // e.g. "my-vault-bucket"
    std::string objectName;        // e.g. "vault.dat"
    std::string serviceAccountJson; // path to service_account.json key file
    bool        enabled = false;
};

// ─── GCS Result ───────────────────────────────────────────────────────────────
struct GCSResult {
    bool        ok      = false;
    std::string data;             // downloaded bytes or upload response body
    std::string error;
};

// ─── GCS Client ───────────────────────────────────────────────────────────────
class GCSClient {
public:
    explicit GCSClient(const GCSConfig& cfg);

    // Upload raw bytes to GCS (replaces/creates the object)
    GCSResult upload(const std::string& data);

    // Download raw bytes from GCS
    GCSResult download();

    // Check connectivity – returns true if bucket is reachable
    bool ping();

    const GCSConfig& config() const { return cfg_; }

private:
    GCSConfig   cfg_;
    std::string accessToken_;
    long long   tokenExpiry_ = 0; // Unix timestamp

    // OAuth2 JWT → access token
    bool refreshToken();

    // Low-level HTTP helpers (WinHTTP)
    GCSResult httpGet (const std::string& url, const std::string& token);
    GCSResult httpPut (const std::string& url, const std::string& token,
                       const std::string& body, const std::string& contentType);

    // Read + parse service account JSON
    bool loadServiceAccount(std::string& clientEmail,
                            std::string& privateKey) const;

    // Minimal JWT RS256 signing (using Windows CNG)
    std::string makeJWT(const std::string& clientEmail,
                        const std::string& privateKeyPem) const;

    // Base64-URL encode (no padding)
    static std::string base64url(const std::string& input);
    static std::string base64urlEncode(const unsigned char* data, size_t len);
};

// ─── Config loader ────────────────────────────────────────────────────────────
// Reads gcs_config.json from configPath directory
GCSConfig loadGCSConfig(const std::string& configPath);

#endif // GCS_CLIENT_H
