#ifndef ENCRYPTION_H
#define ENCRYPTION_H

#include <QString>
#include <QCryptographicHash>
#include <QRandomGenerator>

class Encryption {
public:
    // Password hashing
    static QString hashPassword(const QString& password);
    static bool verifyPassword(const QString& password, const QString& hash);
    
    // Generate salt for additional security
    static QString generateSalt(int length = 16);
    
    // Hash with salt
    static QString hashWithSalt(const QString& password, const QString& salt);
    
    // Simple encryption for security answers (less secure, reversible)
    static QString encryptText(const QString& text);
    static QString decryptText(const QString& encryptedText);

private:
    Encryption(); // Private constructor - utility class
    static const QString ENCRYPTION_KEY;
};

#endif // ENCRYPTION_H