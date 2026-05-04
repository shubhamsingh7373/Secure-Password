#include "auth.h"
#include "vault.h"
#include "gcs_client.h"
#include "password_gen.h"
#include "mongoose.h"
#include <nlohmann/json.hpp>
#include <iostream>
#include <string>
#include <vector>

extern "C" const struct mg_mem_file mg_packed_files[] = {{NULL, NULL, 0, 0}};

using json = nlohmann::json;

// Global vault, auth, and GCS objects
Auth*      g_auth   = nullptr;
Vault*     g_vault  = nullptr;
GCSClient* g_gcs    = nullptr;
std::string g_masterKey;

bool isAuthorized(struct mg_http_message *hm) {
    if (g_masterKey.empty()) return false;
    struct mg_str* auth_header = mg_http_get_header(hm, "Authorization");
    if (auth_header == NULL) return false;
    std::string token(auth_header->buf, auth_header->len);
    return token == g_masterKey;
}

void to_json(json& j, const Entry& e) {
    j = json{
        {"id", e.id},
        {"website", e.website},
        {"username", e.username},
        {"password", e.password},
        {"notes", e.notes},
        {"category", e.category},
        {"createdAt", e.createdAt},
        {"updatedAt", e.updatedAt}
    };
}

static void send_json(struct mg_connection *c, int status, const json& j) {
    std::string s = j.dump();
    mg_http_reply(c, status, "Content-Type: application/json\r\n", "%s", s.c_str());
}

static void send_error(struct mg_connection *c, int status, const char* err) {
    json j = {{"error", err}};
    send_json(c, status, j);
}

static void fn(struct mg_connection *c, int ev, void *ev_data) {
    if (ev == MG_EV_HTTP_MSG) {
        struct mg_http_message *hm = (struct mg_http_message *) ev_data;

        if (mg_match(hm->uri, mg_str("/api/status"), NULL)) {
            json j = {{"isFirstRun", g_auth->isFirstRun()}};
            send_json(c, 200, j);
        }
        else if (mg_match(hm->uri, mg_str("/api/setup"), NULL) && mg_match(hm->method, mg_str("POST"), NULL)) {
            if (!g_auth->isFirstRun()) {
                send_error(c, 400, "Already setup");
                return;
            }
            std::string body(hm->body.buf, hm->body.len);
            auto data = json::parse(body, nullptr, false);
            if (data.is_discarded() || !data.contains("password")) {
                send_error(c, 400, "Invalid JSON");
                return;
            }
            std::string pwd = data["password"];
            if (g_auth->setupMaster(pwd)) {
                g_masterKey = g_auth->derivedKey(pwd);
                g_vault->load(g_masterKey);
                json j = {{"token", g_masterKey}};
                send_json(c, 200, j);
            } else {
                send_error(c, 500, "Setup failed");
            }
        }
        else if (mg_match(hm->uri, mg_str("/api/login"), NULL) && mg_match(hm->method, mg_str("POST"), NULL)) {
            std::string body(hm->body.buf, hm->body.len);
            auto data = json::parse(body, nullptr, false);
            if (data.is_discarded() || !data.contains("password")) {
                send_error(c, 400, "Invalid JSON");
                return;
            }
            std::string pwd = data["password"];
            if (g_auth->verify(pwd)) {
                g_masterKey = g_auth->derivedKey(pwd);
                if (g_vault->load(g_masterKey)) {
                    json j = {{"token", g_masterKey}};
                    send_json(c, 200, j);
                } else {
                    send_error(c, 500, "Failed to load vault");
                }
            } else {
                send_error(c, 401, "Invalid password");
            }
        }
        else if (mg_match(hm->uri, mg_str("/api/entries"), NULL) && mg_match(hm->method, mg_str("GET"), NULL)) {
            if (!isAuthorized(hm)) { send_error(c, 401, "Unauthorized"); return; }
            auto entries = g_vault->allEntries();
            json j = json::array();
            for (const auto& e : entries) {
                json ej;
                to_json(ej, e);
                j.push_back(ej);
            }
            send_json(c, 200, j);
        }
        else if (mg_match(hm->uri, mg_str("/api/entries"), NULL) && mg_match(hm->method, mg_str("POST"), NULL)) {
            if (!isAuthorized(hm)) { send_error(c, 401, "Unauthorized"); return; }
            std::string body(hm->body.buf, hm->body.len);
            auto data = json::parse(body, nullptr, false);
            if (data.is_discarded()) { send_error(c, 400, "Invalid JSON"); return; }
            
            Entry e;
            if(data.contains("website")) e.website = data["website"];
            if(data.contains("username")) e.username = data["username"];
            if(data.contains("password")) e.password = data["password"];
            if(data.contains("category")) e.category = data["category"];
            if(data.contains("notes")) e.notes = data["notes"];

            int id = g_vault->addEntry(e);
            g_vault->save(g_masterKey);
            e.id = id;
            json j; to_json(j, e);
            send_json(c, 200, j);
        }
        else if (mg_match(hm->uri, mg_str("/api/entries/*"), NULL)) {
            std::string uri(hm->uri.buf, hm->uri.len);
            size_t last_slash = uri.find_last_of('/');
            if (last_slash == std::string::npos) { send_error(c, 400, "Invalid URI"); return; }
            int id = 0;
            try { id = std::stoi(uri.substr(last_slash + 1)); } 
            catch(...) { send_error(c, 400, "Invalid ID"); return; }

            if (mg_match(hm->method, mg_str("PUT"), NULL)) {
                if (!isAuthorized(hm)) { send_error(c, 401, "Unauthorized"); return; }
                auto optEntry = g_vault->getEntry(id);
                if (!optEntry) { send_error(c, 404, "Not found"); return; }

                std::string body(hm->body.buf, hm->body.len);
                auto data = json::parse(body, nullptr, false);
                if (data.is_discarded()) { send_error(c, 400, "Invalid JSON"); return; }

                Entry e = *optEntry;
                if(data.contains("website")) e.website = data["website"];
                if(data.contains("username")) e.username = data["username"];
                if(data.contains("password")) e.password = data["password"];
                if(data.contains("category")) e.category = data["category"];
                if(data.contains("notes")) e.notes = data["notes"];

                if (g_vault->updateEntry(e)) {
                    g_vault->save(g_masterKey);
                    json j; to_json(j, e);
                    send_json(c, 200, j);
                } else {
                    send_error(c, 500, "Update failed");
                }
            } else if (mg_match(hm->method, mg_str("DELETE"), NULL)) {
                if (!isAuthorized(hm)) { send_error(c, 401, "Unauthorized"); return; }
                if (g_vault->deleteEntry(id)) {
                    g_vault->save(g_masterKey);
                    json j = {{"status", "deleted"}};
                    send_json(c, 200, j);
                } else {
                    send_error(c, 404, "Not found");
                }
            } else if (mg_match(hm->method, mg_str("GET"), NULL)) {
                if (!isAuthorized(hm)) { send_error(c, 401, "Unauthorized"); return; }
                auto optEntry = g_vault->getEntry(id);
                if (optEntry) {
                    json j; to_json(j, *optEntry);
                    send_json(c, 200, j);
                } else {
                    send_error(c, 404, "Not found");
                }
            }
        }
        // ── GET /api/generate?length=16&upper=1&lower=1&numbers=1&symbols=0
        else if (mg_match(hm->uri, mg_str("/api/generate"), NULL) && mg_match(hm->method, mg_str("GET"), NULL)) {
            if (!isAuthorized(hm)) { send_error(c, 401, "Unauthorized"); return; }
            char buf[8];
            GenOptions opts;
            if (mg_http_get_var(&hm->query, "length",  buf, sizeof(buf)) > 0) opts.length    = std::max(8, std::min(128, std::stoi(buf)));
            if (mg_http_get_var(&hm->query, "upper",   buf, sizeof(buf)) > 0) opts.uppercase  = std::string(buf) != "0";
            if (mg_http_get_var(&hm->query, "lower",   buf, sizeof(buf)) > 0) opts.lowercase  = std::string(buf) != "0";
            if (mg_http_get_var(&hm->query, "numbers", buf, sizeof(buf)) > 0) opts.digits     = std::string(buf) != "0";
            if (mg_http_get_var(&hm->query, "symbols", buf, sizeof(buf)) > 0) opts.symbols    = std::string(buf) != "0";
            std::string pwd = PasswordGenerator::generate(opts);
            int score       = PasswordGenerator::strength(pwd);
            std::string lbl = PasswordGenerator::strengthLabel(score);
            json j = {{"password", pwd}, {"score", score}, {"label", lbl}};
            send_json(c, 200, j);
        }
        // ── GET /api/search?q=...
        else if (mg_match(hm->uri, mg_str("/api/search"), NULL) && mg_match(hm->method, mg_str("GET"), NULL)) {
            if (!isAuthorized(hm)) { send_error(c, 401, "Unauthorized"); return; }
            char qbuf[256] = {};
            mg_http_get_var(&hm->query, "q", qbuf, sizeof(qbuf));
            std::string query(qbuf);
            auto results = query.empty() ? g_vault->allEntries() : g_vault->search(query);
            json j = json::array();
            for (const auto& e : results) { json ej; to_json(ej, e); j.push_back(ej); }
            send_json(c, 200, j);
        }
        // GET /api/cloud/status
        else if (mg_match(hm->uri, mg_str("/api/cloud/status"), NULL) && mg_match(hm->method, mg_str("GET"), NULL)) {
            if (!isAuthorized(hm)) { send_error(c, 401, "Unauthorized"); return; }
            bool enabled = g_vault->isCloudEnabled();
            json j = {
                {"enabled", enabled},
                {"bucket", enabled ? g_gcs->config().bucketName : ""},
                {"object", enabled ? g_gcs->config().objectName : ""}
            };
            send_json(c, 200, j);
        }
        // POST /api/cloud/sync (manual push)
        else if (mg_match(hm->uri, mg_str("/api/cloud/sync"), NULL) && mg_match(hm->method, mg_str("POST"), NULL)) {
            if (!isAuthorized(hm)) { send_error(c, 401, "Unauthorized"); return; }
            if (!g_vault->isCloudEnabled()) {
                send_error(c, 503, "GCS not configured");
                return;
            }
            bool ok = g_vault->syncToCloud();
            json j = {{"synced", ok}, {"message", ok ? "Vault synced to GCS" : "Sync failed"}};
            send_json(c, ok ? 200 : 500, j);
        }
        else {
            struct mg_http_serve_opts opts = {0};
            opts.root_dir = "./public";
            mg_http_serve_dir(c, hm, &opts);
        }
    }
}

int main() {
    const std::string configPath    = "vault_config.txt";
    const std::string vaultPath     = "vault.dat";
    const std::string gcsConfigPath = "gcs_config.json";

    Auth  auth(configPath);
    Vault vault(vaultPath);
    g_auth  = &auth;
    g_vault = &vault;

    // ── GCS Setup ──────────────────────────────────────────────────────────
    GCSConfig gcsCfg = loadGCSConfig(gcsConfigPath);
    GCSClient* gcsClient = nullptr;
    if (gcsCfg.enabled) {
        gcsClient = new GCSClient(gcsCfg);
        vault.setGCSClient(gcsClient);
        g_gcs = gcsClient;
    } else {
        std::cout << "[GCS] Cloud sync disabled. Using local vault.dat\n";
        std::cout << "[GCS] To enable, fill in gcs_config.json\n";
    }
    // ───────────────────────────────────────────────────────────────────────

    struct mg_mgr mgr;
    mg_mgr_init(&mgr);
    mg_http_listen(&mgr, "http://0.0.0.0:8080", fn, NULL);

    std::cout << "Starting Healix Vault Web Server on http://localhost:8080\n";
    std::cout << "Open your browser and navigate to http://localhost:8080\n";
    std::cout << "Press Ctrl+C to stop.\n";

    for (;;) {
        mg_mgr_poll(&mgr, 1000);
    }
    
    mg_mgr_free(&mgr);
    return 0;
}