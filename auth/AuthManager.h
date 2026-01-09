#ifndef AUTHMANAGER_H
#define AUTHMANAGER_H

#include <QString>
#include "User.h"

class AuthManager {
public:
    static AuthManager& instance();
    
    // Authentication
    bool login(const QString& username, const QString& password);
    void logout();
    bool isLoggedIn() const { return m_isLoggedIn; }
    
    // Current user information
    User getCurrentUser() const { return m_currentUser; }
    int getCurrentUserId() const { return m_currentUser.getId(); }
    QString getCurrentUsername() const { return m_currentUser.getUsername(); }
    User::Role getCurrentUserRole() const { return m_currentUser.getRole(); }
    
    // Permission checks
    bool hasAdminPermission() const;
    bool hasLibrarianPermission() const;
    bool hasFinancePermission() const;
    bool canManageUsers() const;
    bool canManageLearners() const;
    bool canManageBooks() const;
    bool canManageTransactions() const;
    
    // Registration
    bool registerUser(const User& user, const QString& password);
    
    // Password recovery
    bool verifyEmail(const QString& email);
    QString getSecurityQuestion(const QString& email);
    bool verifySecurityAnswer(const QString& email, const QString& answer);
    bool resetPassword(const QString& email, const QString& newPassword);
    
    // Password change
    bool changePassword(const QString& oldPassword, const QString& newPassword);
    
    // Validation
    struct ValidationResult {
        bool isValid;
        QString message;
    };
    
    ValidationResult validateUsername(const QString& username);
    ValidationResult validatePassword(const QString& password);
    ValidationResult validateEmail(const QString& email);
    
    // Error handling
    QString getLastError() const { return m_lastError; }

private:
    AuthManager();
    ~AuthManager();
    AuthManager(const AuthManager&) = delete;
    AuthManager& operator=(const AuthManager&) = delete;
    
    bool m_isLoggedIn;
    User m_currentUser;
    QString m_lastError;
    
    void setLastError(const QString& error);
};

#endif // AUTHMANAGER_H