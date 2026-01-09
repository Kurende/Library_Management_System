#include "Encryption.h"
#include <QByteArray>

// Simple key for basic encryption - in production, use more secure method
const QString Encryption::ENCRYPTION_KEY = "LibraryMgmtKey2024";

Encryption::Encryption() {
}

QString Encryption::hashPassword(const QString& password) {
    // Use SHA-256 for password hashing
    QByteArray passwordData = password.toUtf8();
    QByteArray hash = QCryptographicHash::hash(passwordData, QCryptographicHash::Sha256);
    return QString(hash.toHex());
}

bool Encryption::verifyPassword(const QString& password, const QString& hash) {
    QString hashedInput = hashPassword(password);
    return hashedInput == hash;
}

QString Encryption::generateSalt(int length) {
    const QString possibleCharacters = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789";
    QString salt;
    
    for (int i = 0; i < length; ++i) {
        int index = QRandomGenerator::global()->bounded(possibleCharacters.length());
        salt.append(possibleCharacters.at(index));
    }
    
    return salt;
}

QString Encryption::hashWithSalt(const QString& password, const QString& salt) {
    QString saltedPassword = salt + password + salt;
    return hashPassword(saltedPassword);
}

QString Encryption::encryptText(const QString& text) {
    // Simple XOR encryption for security answers
    // Note: This is not cryptographically secure, just basic obfuscation
    QByteArray textData = text.toUtf8();
    QByteArray keyData = ENCRYPTION_KEY.toUtf8();
    QByteArray encrypted;
    
    for (int i = 0; i < textData.size(); ++i) {
        encrypted.append(textData[i] ^ keyData[i % keyData.size()]);
    }
    
    return QString(encrypted.toBase64());
}

QString Encryption::decryptText(const QString& encryptedText) {
    // Decrypt XOR encrypted text
    QByteArray encrypted = QByteArray::fromBase64(encryptedText.toUtf8());
    QByteArray keyData = ENCRYPTION_KEY.toUtf8();
    QByteArray decrypted;
    
    for (int i = 0; i < encrypted.size(); ++i) {
        decrypted.append(encrypted[i] ^ keyData[i % keyData.size()]);
    }
    
    return QString::fromUtf8(decrypted);
}