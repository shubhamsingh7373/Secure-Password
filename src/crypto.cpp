/*
 * crypto.cpp – Self-contained AES-256-CBC, SHA-256, PBKDF2, Base64
 * No external library required; everything implemented from scratch.
 */

#include "crypto.h"
#include <cstring>
#include <stdexcept>
#include <sstream>
#include <iomanip>
#include <random>
#include <algorithm>

// ═══════════════════════════════════════════════════════════════════════════
// SHA-256
// ═══════════════════════════════════════════════════════════════════════════
namespace {

static const uint32_t K256[64] = {
    0x428a2f98,0x71374491,0xb5c0fbcf,0xe9b5dba5,0x3956c25b,0x59f111f1,
    0x923f82a4,0xab1c5ed5,0xd807aa98,0x12835b01,0x243185be,0x550c7dc3,
    0x72be5d74,0x80deb1fe,0x9bdc06a7,0xc19bf174,0xe49b69c1,0xefbe4786,
    0x0fc19dc6,0x240ca1cc,0x2de92c6f,0x4a7484aa,0x5cb0a9dc,0x76f988da,
    0x983e5152,0xa831c66d,0xb00327c8,0xbf597fc7,0xc6e00bf3,0xd5a79147,
    0x06ca6351,0x14292967,0x27b70a85,0x2e1b2138,0x4d2c6dfc,0x53380d13,
    0x650a7354,0x766a0abb,0x81c2c92e,0x92722c85,0xa2bfe8a1,0xa81a664b,
    0xc24b8b70,0xc76c51a3,0xd192e819,0xd6990624,0xf40e3585,0x106aa070,
    0x19a4c116,0x1e376c08,0x2748774c,0x34b0bcb5,0x391c0cb3,0x4ed8aa4a,
    0x5b9cca4f,0x682e6ff3,0x748f82ee,0x78a5636f,0x84c87814,0x8cc70208,
    0x90befffa,0xa4506ceb,0xbef9a3f7,0xc67178f2
};

inline uint32_t rotr32(uint32_t x,int n){ return (x>>n)|(x<<(32-n)); }
inline uint32_t ch(uint32_t e,uint32_t f,uint32_t g){ return (e&f)^(~e&g); }
inline uint32_t maj(uint32_t a,uint32_t b,uint32_t c){ return (a&b)^(a&c)^(b&c); }
inline uint32_t S0(uint32_t a){ return rotr32(a,2)^rotr32(a,13)^rotr32(a,22); }
inline uint32_t S1(uint32_t e){ return rotr32(e,6)^rotr32(e,11)^rotr32(e,25); }
inline uint32_t s0(uint32_t w){ return rotr32(w,7)^rotr32(w,18)^(w>>3); }
inline uint32_t s1(uint32_t w){ return rotr32(w,17)^rotr32(w,19)^(w>>10); }

std::vector<uint8_t> sha256Raw(const std::vector<uint8_t>& msg) {
    uint32_t h[8] = {
        0x6a09e667,0xbb67ae85,0x3c6ef372,0xa54ff53a,
        0x510e527f,0x9b05688c,0x1f83d9ab,0x5be0cd19
    };
    // padding
    std::vector<uint8_t> m(msg);
    uint64_t bitlen = (uint64_t)m.size() * 8;
    m.push_back(0x80);
    while (m.size() % 64 != 56) m.push_back(0);
    for (int i=7;i>=0;--i) m.push_back((uint8_t)(bitlen>>(i*8)));

    for (size_t i=0;i<m.size();i+=64) {
        uint32_t w[64];
        for (int j=0;j<16;++j)
            w[j]=(uint32_t)m[i+j*4]<<24|(uint32_t)m[i+j*4+1]<<16|
                 (uint32_t)m[i+j*4+2]<<8|(uint32_t)m[i+j*4+3];
        for (int j=16;j<64;++j)
            w[j]=s1(w[j-2])+w[j-7]+s0(w[j-15])+w[j-16];
        uint32_t a=h[0],b=h[1],c=h[2],d=h[3],
                 e=h[4],f=h[5],g=h[6],hh=h[7];
        for (int j=0;j<64;++j) {
            uint32_t t1=hh+S1(e)+ch(e,f,g)+K256[j]+w[j];
            uint32_t t2=S0(a)+maj(a,b,c);
            hh=g; g=f; f=e; e=d+t1;
            d=c; c=b; b=a; a=t1+t2;
        }
        h[0]+=a; h[1]+=b; h[2]+=c; h[3]+=d;
        h[4]+=e; h[5]+=f; h[6]+=g; h[7]+=hh;
    }
    std::vector<uint8_t> digest(32);
    for (int i=0;i<8;++i) {
        digest[i*4+0]=(h[i]>>24)&0xff;
        digest[i*4+1]=(h[i]>>16)&0xff;
        digest[i*4+2]=(h[i]>>8) &0xff;
        digest[i*4+3]=(h[i]   ) &0xff;
    }
    return digest;
}

// HMAC-SHA-256
std::vector<uint8_t> hmacSha256(const std::vector<uint8_t>& key,
                                  const std::vector<uint8_t>& msg) {
    std::vector<uint8_t> k = key;
    if (k.size() > 64) k = sha256Raw(k);
    k.resize(64, 0);
    std::vector<uint8_t> ipad(64), opad(64);
    for (int i=0;i<64;++i){ ipad[i]=k[i]^0x36; opad[i]=k[i]^0x5c; }
    std::vector<uint8_t> inner(ipad);
    inner.insert(inner.end(),msg.begin(),msg.end());
    auto h1 = sha256Raw(inner);
    std::vector<uint8_t> outer(opad);
    outer.insert(outer.end(),h1.begin(),h1.end());
    return sha256Raw(outer);
}

} // anon namespace

// ═══════════════════════════════════════════════════════════════════════════
// AES-256 (ECB blocks, used in CBC mode)
// ═══════════════════════════════════════════════════════════════════════════
namespace {

static const uint8_t SBOX[256] = {
    0x63,0x7c,0x77,0x7b,0xf2,0x6b,0x6f,0xc5,0x30,0x01,0x67,0x2b,0xfe,0xd7,0xab,0x76,
    0xca,0x82,0xc9,0x7d,0xfa,0x59,0x47,0xf0,0xad,0xd4,0xa2,0xaf,0x9c,0xa4,0x72,0xc0,
    0xb7,0xfd,0x93,0x26,0x36,0x3f,0xf7,0xcc,0x34,0xa5,0xe5,0xf1,0x71,0xd8,0x31,0x15,
    0x04,0xc7,0x23,0xc3,0x18,0x96,0x05,0x9a,0x07,0x12,0x80,0xe2,0xeb,0x27,0xb2,0x75,
    0x09,0x83,0x2c,0x1a,0x1b,0x6e,0x5a,0xa0,0x52,0x3b,0xd6,0xb3,0x29,0xe3,0x2f,0x84,
    0x53,0xd1,0x00,0xed,0x20,0xfc,0xb1,0x5b,0x6a,0xcb,0xbe,0x39,0x4a,0x4c,0x58,0xcf,
    0xd0,0xef,0xaa,0xfb,0x43,0x4d,0x33,0x85,0x45,0xf9,0x02,0x7f,0x50,0x3c,0x9f,0xa8,
    0x51,0xa3,0x40,0x8f,0x92,0x9d,0x38,0xf5,0xbc,0xb6,0xda,0x21,0x10,0xff,0xf3,0xd2,
    0xcd,0x0c,0x13,0xec,0x5f,0x97,0x44,0x17,0xc4,0xa7,0x7e,0x3d,0x64,0x5d,0x19,0x73,
    0x60,0x81,0x4f,0xdc,0x22,0x2a,0x90,0x88,0x46,0xee,0xb8,0x14,0xde,0x5e,0x0b,0xdb,
    0xe0,0x32,0x3a,0x0a,0x49,0x06,0x24,0x5c,0xc2,0xd3,0xac,0x62,0x91,0x95,0xe4,0x79,
    0xe7,0xc8,0x37,0x6d,0x8d,0xd5,0x4e,0xa9,0x6c,0x56,0xf4,0xea,0x65,0x7a,0xae,0x08,
    0xba,0x78,0x25,0x2e,0x1c,0xa6,0xb4,0xc6,0xe8,0xdd,0x74,0x1f,0x4b,0xbd,0x8b,0x8a,
    0x70,0x3e,0xb5,0x66,0x48,0x03,0xf6,0x0e,0x61,0x35,0x57,0xb9,0x86,0xc1,0x1d,0x9e,
    0xe1,0xf8,0x98,0x11,0x69,0xd9,0x8e,0x94,0x9b,0x1e,0x87,0xe9,0xce,0x55,0x28,0xdf,
    0x8c,0xa1,0x89,0x0d,0xbf,0xe6,0x42,0x68,0x41,0x99,0x2d,0x0f,0xb0,0x54,0xbb,0x16
};

static const uint8_t INV_SBOX[256] = {
    0x52,0x09,0x6a,0xd5,0x30,0x36,0xa5,0x38,0xbf,0x40,0xa3,0x9e,0x81,0xf3,0xd7,0xfb,
    0x7c,0xe3,0x39,0x82,0x9b,0x2f,0xff,0x87,0x34,0x8e,0x43,0x44,0xc4,0xde,0xe9,0xcb,
    0x54,0x7b,0x94,0x32,0xa6,0xc2,0x23,0x3d,0xee,0x4c,0x95,0x0b,0x42,0xfa,0xc3,0x4e,
    0x08,0x2e,0xa1,0x66,0x28,0xd9,0x24,0xb2,0x76,0x5b,0xa2,0x49,0x6d,0x8b,0xd1,0x25,
    0x72,0xf8,0xf6,0x64,0x86,0x68,0x98,0x16,0xd4,0xa4,0x5c,0xcc,0x5d,0x65,0xb6,0x92,
    0x6c,0x70,0x48,0x50,0xfd,0xed,0xb9,0xda,0x5e,0x15,0x46,0x57,0xa7,0x8d,0x9d,0x84,
    0x90,0xd8,0xab,0x00,0x8c,0xbc,0xd3,0x0a,0xf7,0xe4,0x58,0x05,0xb8,0xb3,0x45,0x06,
    0xd0,0x2c,0x1e,0x8f,0xca,0x3f,0x0f,0x02,0xc1,0xaf,0xbd,0x03,0x01,0x13,0x8a,0x6b,
    0x3a,0x91,0x11,0x41,0x4f,0x67,0xdc,0xea,0x97,0xf2,0xcf,0xce,0xf0,0xb4,0xe6,0x73,
    0x96,0xac,0x74,0x22,0xe7,0xad,0x35,0x85,0xe2,0xf9,0x37,0xe8,0x1c,0x75,0xdf,0x6e,
    0x47,0xf1,0x1a,0x71,0x1d,0x29,0xc5,0x89,0x6f,0xb7,0x62,0x0e,0xaa,0x18,0xbe,0x1b,
    0xfc,0x56,0x3e,0x4b,0xc6,0xd2,0x79,0x20,0x9a,0xdb,0xc0,0xfe,0x78,0xcd,0x5a,0xf4,
    0x1f,0xdd,0xa8,0x33,0x88,0x07,0xc7,0x31,0xb1,0x12,0x10,0x59,0x27,0x80,0xec,0x5f,
    0x60,0x51,0x7f,0xa9,0x19,0xb5,0x4a,0x0d,0x2d,0xe5,0x7a,0x9f,0x93,0xc9,0x9c,0xef,
    0xa0,0xe0,0x3b,0x4d,0xae,0x2a,0xf5,0xb0,0xc8,0xeb,0xbb,0x3c,0x83,0x53,0x99,0x61,
    0x17,0x2b,0x04,0x7e,0xba,0x77,0xd6,0x26,0xe1,0x69,0x14,0x63,0x55,0x21,0x0c,0x7d
};

static const uint8_t RCON[11] = {
    0x00,0x01,0x02,0x04,0x08,0x10,0x20,0x40,0x80,0x1b,0x36
};

static uint8_t gmul(uint8_t a, uint8_t b) {
    uint8_t p=0;
    for(int i=0;i<8;++i){
        if(b&1) p^=a;
        bool hi=a&0x80;
        a<<=1;
        if(hi) a^=0x1b;
        b>>=1;
    }
    return p;
}

struct AES256 {
    uint8_t rk[240]; // 15 round keys × 16 bytes

    void setKey(const uint8_t* key) {
        memcpy(rk, key, 32);
        for(int i=8;i<60;++i){
            uint8_t t[4];
            memcpy(t, rk+(i-1)*4, 4);
            if(i%8==0){
                uint8_t tmp=t[0]; t[0]=SBOX[t[1]]^RCON[i/8];
                t[1]=SBOX[t[2]]; t[2]=SBOX[t[3]]; t[3]=SBOX[tmp];
            } else if(i%8==4){
                for(auto& x:t) x=SBOX[x];
            }
            for(int j=0;j<4;++j) rk[i*4+j]=rk[(i-8)*4+j]^t[j];
        }
    }

    void encryptBlock(const uint8_t* in, uint8_t* out) const {
        uint8_t s[16];
        for(int i=0;i<16;++i) s[i]=in[i]^rk[i];
        for(int r=1;r<=14;++r){
            // SubBytes
            for(auto& x:s) x=SBOX[x];
            // ShiftRows
            uint8_t t=s[1]; s[1]=s[5]; s[5]=s[9]; s[9]=s[13]; s[13]=t;
            t=s[2]; s[2]=s[10]; s[10]=t; t=s[6]; s[6]=s[14]; s[14]=t;
            t=s[15]; s[15]=s[11]; s[11]=s[7]; s[7]=s[3]; s[3]=t;
            // MixColumns (skip last round)
            if(r<14){
                for(int c=0;c<4;++c){
                    uint8_t* col=s+c*4;
                    uint8_t a=col[0],b=col[1],cc=col[2],d=col[3];
                    col[0]=gmul(2,a)^gmul(3,b)^cc^d;
                    col[1]=a^gmul(2,b)^gmul(3,cc)^d;
                    col[2]=a^b^gmul(2,cc)^gmul(3,d);
                    col[3]=gmul(3,a)^b^cc^gmul(2,d);
                }
            }
            // AddRoundKey
            for(int i=0;i<16;++i) s[i]^=rk[r*16+i];
        }
        memcpy(out,s,16);
    }

    void decryptBlock(const uint8_t* in, uint8_t* out) const {
        uint8_t s[16];
        for(int i=0;i<16;++i) s[i]=in[i]^rk[14*16+i];
        for(int r=13;r>=0;--r){
            // Inv ShiftRows
            uint8_t t=s[13]; s[13]=s[9]; s[9]=s[5]; s[5]=s[1]; s[1]=t;
            t=s[2]; s[2]=s[10]; s[10]=t; t=s[6]; s[6]=s[14]; s[14]=t;
            t=s[3]; s[3]=s[7]; s[7]=s[11]; s[11]=s[15]; s[15]=t;
            // Inv SubBytes
            for(auto& x:s) x=INV_SBOX[x];
            // AddRoundKey
            for(int i=0;i<16;++i) s[i]^=rk[r*16+i];
            // Inv MixColumns (skip first round)
            if(r>0){
                for(int c=0;c<4;++c){
                    uint8_t* col=s+c*4;
                    uint8_t a=col[0],b=col[1],cc=col[2],d=col[3];
                    col[0]=gmul(0x0e,a)^gmul(0x0b,b)^gmul(0x0d,cc)^gmul(0x09,d);
                    col[1]=gmul(0x09,a)^gmul(0x0e,b)^gmul(0x0b,cc)^gmul(0x0d,d);
                    col[2]=gmul(0x0d,a)^gmul(0x09,b)^gmul(0x0e,cc)^gmul(0x0b,d);
                    col[3]=gmul(0x0b,a)^gmul(0x0d,b)^gmul(0x09,cc)^gmul(0x0e,d);
                }
            }
        }
        memcpy(out,s,16);
    }
};

} // anon namespace

// ═══════════════════════════════════════════════════════════════════════════
// Public Crypto API
// ═══════════════════════════════════════════════════════════════════════════

namespace Crypto {

std::string sha256(const std::string& input) {
    std::vector<uint8_t> data(input.begin(), input.end());
    auto digest = sha256Raw(data);
    std::ostringstream oss;
    for(auto b : digest) oss << std::hex << std::setw(2) << std::setfill('0') << (int)b;
    return oss.str();
}

std::vector<uint8_t> randomBytes(size_t n) {
    std::vector<uint8_t> buf(n);
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<int> dist(0,255);
    for(auto& b : buf) b = (uint8_t)dist(gen);
    return buf;
}

static const char B64CHARS[] =
    "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";

std::string base64Encode(const std::vector<uint8_t>& data) {
    std::string out;
    int val=0, valb=-6;
    for(uint8_t c : data){
        val=(val<<8)+c; valb+=8;
        while(valb>=0){ out.push_back(B64CHARS[(val>>valb)&0x3f]); valb-=6; }
    }
    if(valb>-6) out.push_back(B64CHARS[((val<<8)>>(valb+8))&0x3f]);
    while(out.size()%4) out.push_back('=');
    return out;
}

std::vector<uint8_t> base64Decode(const std::string& encoded) {
    std::vector<int> T(256,-1);
    for(int i=0;i<64;++i) T[(uint8_t)B64CHARS[i]]=i;
    std::vector<uint8_t> out;
    int val=0, valb=-8;
    for(char c : encoded){
        if(T[(uint8_t)c]==-1) break;
        val=(val<<6)+T[(uint8_t)c]; valb+=6;
        if(valb>=0){ out.push_back((val>>valb)&0xff); valb-=8; }
    }
    return out;
}

std::vector<uint8_t> pbkdf2(const std::string& password,
                              const std::vector<uint8_t>& salt,
                              int iterations, int keyLen) {
    std::vector<uint8_t> pkey(password.begin(), password.end());
    std::vector<uint8_t> result;
    int blockCount = (keyLen+31)/32;
    for(int block=1; block<=blockCount; ++block){
        std::vector<uint8_t> input(salt);
        input.push_back((block>>24)&0xff);
        input.push_back((block>>16)&0xff);
        input.push_back((block>> 8)&0xff);
        input.push_back((block    )&0xff);
        auto u = hmacSha256(pkey, input);
        auto f = u;
        for(int i=1; i<iterations; ++i){
            u = hmacSha256(pkey, u);
            for(size_t j=0; j<32; ++j) f[j]^=u[j];
        }
        result.insert(result.end(), f.begin(), f.end());
    }
    result.resize(keyLen);
    return result;
}

std::string aes256Encrypt(const std::string& plaintext, const std::string& key) {
    // Derive 32-byte key from the string key
    auto salt = randomBytes(16);
    auto dk = pbkdf2(key, salt, 1000, 32);
    auto iv = randomBytes(16);

    // PKCS#7 padding
    std::vector<uint8_t> pt(plaintext.begin(), plaintext.end());
    int pad = 16 - (pt.size() % 16);
    for(int i=0;i<pad;++i) pt.push_back((uint8_t)pad);

    AES256 aes; aes.setKey(dk.data());

    std::vector<uint8_t> ct(pt.size());
    std::vector<uint8_t> prev(iv);
    for(size_t i=0; i<pt.size(); i+=16){
        uint8_t block[16];
        for(int j=0;j<16;++j) block[j]=pt[i+j]^prev[j];
        aes.encryptBlock(block, ct.data()+i);
        memcpy(prev.data(), ct.data()+i, 16);
    }

    // Layout: salt(16) | iv(16) | ciphertext
    std::vector<uint8_t> combined;
    combined.insert(combined.end(), salt.begin(), salt.end());
    combined.insert(combined.end(), iv.begin(), iv.end());
    combined.insert(combined.end(), ct.begin(), ct.end());
    return base64Encode(combined);
}

std::string aes256Decrypt(const std::string& ciphertext_b64, const std::string& key) {
    auto combined = base64Decode(ciphertext_b64);
    if(combined.size() < 33) throw std::runtime_error("Invalid ciphertext");

    std::vector<uint8_t> salt(combined.begin(), combined.begin()+16);
    std::vector<uint8_t> iv(combined.begin()+16, combined.begin()+32);
    std::vector<uint8_t> ct(combined.begin()+32, combined.end());

    auto dk = pbkdf2(key, salt, 1000, 32);
    AES256 aes; aes.setKey(dk.data());

    std::vector<uint8_t> pt(ct.size());
    std::vector<uint8_t> prev(iv);
    for(size_t i=0; i<ct.size(); i+=16){
        uint8_t block[16];
        aes.decryptBlock(ct.data()+i, block);
        for(int j=0;j<16;++j) pt[i+j]=block[j]^prev[j];
        memcpy(prev.data(), ct.data()+i, 16);
    }

    // Remove PKCS#7 padding
    if(pt.empty()) throw std::runtime_error("Empty plaintext");
    int pad = pt.back();
    if(pad<1||pad>16) throw std::runtime_error("Bad padding");
    pt.resize(pt.size()-pad);
    return std::string(pt.begin(), pt.end());
}

} // namespace Crypto
