// Copyright (c) 2014-2017 The Dash Core developers
// Distributed under the MIT/X11 software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.

#include <hash.h>
#include <key_io.h>
#include <messagesigner.h>
#include <tinyformat.h>
#include <util/strencodings.h>
#include <validation.h>

bool CMessageSigner::GetKeysFromSecret(const std::string strSecret, CKey& keyRet, CPubKey& pubkeyRet)
{
    keyRet = DecodeSecret(strSecret);
    if (!keyRet.IsValid()) return false;
    pubkeyRet = keyRet.GetPubKey();
    return true;
}

bool CMessageSigner::SignMessage(const std::string strMessage, std::vector<unsigned char>& vchSigRet, const CKey key)
{
    CHashWriter ss(SER_GETHASH, 0);
    ss << strMessageMagic;
    ss << strMessage;
    return CHashSigner::SignHash(ss.GetHash(), key, vchSigRet);
}

bool CMessageSigner::VerifyMessage(const CPubKey pubkey, const std::vector<unsigned char>& vchSig, const std::string strMessage, std::string& strErrorRet)
{
    CHashWriter ss(SER_GETHASH, 0);
    ss << strMessageMagic;
    ss << strMessage;
    return CHashSigner::VerifyHash(ss.GetHash(), pubkey, vchSig, strErrorRet);
}

bool CHashSigner::SignHash(const uint256& hash, const CKey key, std::vector<unsigned char>& vchSigRet)
{
    return key.SignCompact(hash, vchSigRet);
}

bool CHashSigner::VerifyHash(const uint256& hash, const CPubKey pubkey, const std::vector<unsigned char>& vchSig, std::string& strErrorRet)
{
    CPubKey pubkeyFromSig;
    CPubKey::InputScriptType inputScriptType;
    if(!pubkeyFromSig.RecoverCompact(hash, vchSig, inputScriptType)) {
        strErrorRet = "Error recovering public key.";
        return false;
    }
    if(pubkeyFromSig.GetID() != pubkey.GetID()) {
        strErrorRet = strprintf("Keys don't match: pubkey=%s, pubkeyFromSig=%s, hash=%s, vchSig=%s",
                    pubkey.GetID().ToString(), pubkeyFromSig.GetID().ToString(), hash.ToString(),
                    EncodeBase64(&vchSig[0], vchSig.size()));
        return false;
    }
    return true;
}
