#include "AuthManager.h"
#include "DatabaseManager.h"
#include "Encryption.h"
#include <QRegularExpression>

AuthManager& AuthManager::instance() {
    static AuthManager instance;
    return instance;
}

AuthManager::AuthManager() : m_isLoggedIn(false) {
}

AuthManager::~AuthManager() {
}

bool AuthManager::login(const QString& username, const QString& password) {
    if (username.isEmpty() || password.isEmpty()) {
        setLastError("Username and password cannot be empty");
        return false;
    }
    
    // Get user from database
    User user = DatabaseManager::instance().getUserByUsername(username);
    
    if (user.getId() == -1) {
        setLastError("Invalid username or password");
        return false;
    }
    
    // Verify password
    if (!Encryption::verifyPassword(password, user.getPasswordHash())) {
        setLastError("Invalid username or password");
        return false;
    }
    
    // Login successful
    m_currentUser = user;
    m_isLoggedIn = true;
    m_lastError.clear();

    QSqlQuery updateQuery(DatabaseManager::instance().getDatabase());
    updateQuery.prepare("UPDATE users SET last_login = :last_login WHERE id = :id");
    updateQuery.bindValue(":last_login", QDateTime::currentDateTime());
    updateQuery.bindValue(":id", m_currentUser.getId());
    updateQuery.exec();

    // Log the login activity
    DatabaseManager::instance().logUserActivity(
        m_currentUser.getId(),
        "Login",
        "User logged in successfully"
        );
    
    return true;
}

void AuthManager::logout() {
    m_isLoggedIn = false;
    m_currentUser = User();
    m_lastError.clear();
}

bool AuthManager::hasAdminPermission() const {
    return m_isLoggedIn && m_currentUser.getRole() == User::Role::Admin;
}

bool AuthManager::hasLibrarianPermission() const {
    if (!m_isLoggedIn) return false;
    User::Role role = m_currentUser.getRole();
    return role == User::Role::Admin || role == User::Role::Librarian;
}

bool AuthManager::hasFinancePermission() const {
    if (!m_isLoggedIn) return false;
    User::Role role = m_currentUser.getRole();
    return role == User::Role::Admin || role == User::Role::Finance;
}

bool AuthManager::canManageUsers() const {
    return hasAdminPermission();
}

bool AuthManager::canManageLearners() const {
    return hasLibrarianPermission();
}

bool AuthManager::canManageBooks() const {
    return hasLibrarianPermission();
}

bool AuthManager::canManageTransactions() const {
    return hasLibrarianPermission();
}

bool AuthManager::registerUser(const User& user, const QString& password) {
    // Validate username
    auto usernameResult = validateUsername(user.getUsername());
    if (!usernameResult.isValid) {
        setLastError(usernameResult.message);
        return false;
    }
    
    // Validate password
    auto passwordResult = validatePassword(password);
    if (!passwordResult.isValid) {
        setLastError(passwordResult.message);
        return false;
    }
    
    // Validate email
    auto emailResult = validateEmail(user.getEmail());
    if (!emailResult.isValid) {
        setLastError(emailResult.message);
        return false;
    }
    
    // Check if username already exists
    if (DatabaseManager::instance().userExists(user.getUsername())) {
        setLastError("Username already exists");
        return false;
    }
    
    // Check if email already exists
    User existingUser = DatabaseManager::instance().getUserByEmail(user.getEmail());
    if (existingUser.getId() != -1) {
        setLastError("Email already registered");
        return false;
    }
    
    // Hash password
    QString hashedPassword = Encryption::hashPassword(password);
    
    // Create user with hashed password
    User newUser = user;
    newUser.setPasswordHash(hashedPassword);
    
    // Encrypt security answer
    QString encryptedAnswer = Encryption::encryptText(user.getSecurityAnswer());
    newUser.setSecurityAnswer(encryptedAnswer);
    
    // Add to database
    if (!DatabaseManager::instance().addUser(newUser)) {
        setLastError(DatabaseManager::instance().getLastError());
        return false;
    }
    
    m_lastError.clear();
    return true;
}

bool AuthManager::verifyEmail(const QString& email) {
    User user = DatabaseManager::instance().getUserByEmail(email);
    if (user.getId() == -1) {
        setLastError("Email not found");
        return false;
    }
    
    m_lastError.clear();
    return true;
}

QString AuthManager::getSecurityQuestion(const QString& email) {
    User user = DatabaseManager::instance().getUserByEmail(email);
    if (user.getId() == -1) {
        setLastError("Email not found");
        return "";
    }
    
    return user.getSecurityQuestion();
}

bool AuthManager::verifySecurityAnswer(const QString& email, const QString& answer) {
    User user = DatabaseManager::instance().getUserByEmail(email);
    if (user.getId() == -1) {
        setLastError("Email not found");
        return false;
    }
    
    // Decrypt stored answer and compare
    QString decryptedAnswer = Encryption::decryptText(user.getSecurityAnswer());
    
    if (decryptedAnswer.toLower() != answer.toLower()) {
        setLastError("Incorrect security answer");
        return false;
    }
    
    m_lastError.clear();
    return true;
}

bool AuthManager::resetPassword(const QString& email, const QString& newPassword) {
    // Validate new password
    auto passwordResult = validatePassword(newPassword);
    if (!passwordResult.isValid) {
        setLastError(passwordResult.message);
        return false;
    }
    
    // Get user
    User user = DatabaseManager::instance().getUserByEmail(email);
    if (user.getId() == -1) {
        setLastError("Email not found");
        return false;
    }
    
    // Hash new password
    QString hashedPassword = Encryption::hashPassword(newPassword);
    user.setPasswordHash(hashedPassword);
    
    // Update in database
    if (!DatabaseManager::instance().updateUser(user)) {
        setLastError(DatabaseManager::instance().getLastError());
        return false;
    }
    
    m_lastError.clear();
    return true;
}

bool AuthManager::changePassword(const QString& oldPassword, const QString& newPassword) {
    if (!m_isLoggedIn) {
        setLastError("No user logged in");
        return false;
    }
    
    // Verify old password
    if (!Encryption::verifyPassword(oldPassword, m_currentUser.getPasswordHash())) {
        setLastError("Current password is incorrect");
        return false;
    }
    
    // Validate new password
    auto passwordResult = validatePassword(newPassword);
    if (!passwordResult.isValid) {
        setLastError(passwordResult.message);
        return false;
    }
    
    // Hash new password
    QString hashedPassword = Encryption::hashPassword(newPassword);
    m_currentUser.setPasswordHash(hashedPassword);
    
    // Update in database
    if (!DatabaseManager::instance().updateUser(m_currentUser)) {
        setLastError(DatabaseManager::instance().getLastError());
        return false;
    }
    
    m_lastError.clear();
    return true;
}

AuthManager::ValidationResult AuthManager::validateUsername(const QString& username) {
    ValidationResult result;
    
    if (username.isEmpty()) {
        result.isValid = false;
        result.message = "Username cannot be empty";
        return result;
    }
    
    if (username.length() < 3) {
        result.isValid = false;
        result.message = "Username must be at least 3 characters long";
        return result;
    }
    
    if (username.length() > 100) {
        result.isValid = false;
        result.message = "Username must be less than 100 characters";
        return result;
    }
    
    // Allow email format (since we use email as username) or alphanumeric with underscores
    QRegularExpression emailRegex("^[a-zA-Z0-9._%+-]+@[a-zA-Z0-9.-]+\\.[a-zA-Z]{2,}$");
    QRegularExpression alphanumericRegex("^[a-zA-Z0-9_]+$");
    
    if (!emailRegex.match(username).hasMatch() && !alphanumericRegex.match(username).hasMatch()) {
        result.isValid = false;
        result.message = "Username must be a valid email address or contain only letters, numbers, and underscores";
        return result;
    }
    
    result.isValid = true;
    result.message = "Username is valid";
    return result;
}

AuthManager::ValidationResult AuthManager::validatePassword(const QString& password) {
    ValidationResult result;
    
    if (password.isEmpty()) {
        result.isValid = false;
        result.message = "Password cannot be empty";
        return result;
    }
    
    if (password.length() < 6) {
        result.isValid = false;
        result.message = "Password must be at least 6 characters long";
        return result;
    }
    
    if (password.length() > 50) {
        result.isValid = false;
        result.message = "Password must be less than 50 characters";
        return result;
    }
    
    result.isValid = true;
    result.message = "Password is valid";
    return result;
}

AuthManager::ValidationResult AuthManager::validateEmail(const QString& email) {
    ValidationResult result;
    
    if (email.isEmpty()) {
        result.isValid = false;
        result.message = "Email cannot be empty";
        return result;
    }
    
    // Basic email validation regex
    QRegularExpression regex("^[a-zA-Z0-9._%+-]+@[a-zA-Z0-9.-]+\\.[a-zA-Z]{2,}$");
    if (!regex.match(email).hasMatch()) {
        result.isValid = false;
        result.message = "Invalid email format";
        return result;
    }
    
    result.isValid = true;
    result.message = "Email is valid";
    return result;
}

void AuthManager::setLastError(const QString& error) {
    m_lastError = error;
}

bool AuthManager::verifyPassword(const QString& username, const QString& password) {
    QSqlQuery query(DatabaseManager::instance().getDatabase());

    query.prepare("SELECT password_hash FROM users WHERE username = :username");
    query.bindValue(":username", username);

    if (!query.exec() || !query.next()) {
        return false;
    }

    QString storedHash = query.value(0).toString();
    QString inputHash = Encryption::hashPassword(password);

    return (storedHash == inputHash);
}

