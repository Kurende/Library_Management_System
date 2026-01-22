void AuthManager::login(const QString& username, const QString& password) {
    if (username.isEmpty() || password.isEmpty()) {
        qDebug() << "Username and password cannot be empty.";
        return;
    }

    QSqlQuery updateQuery;
    updateQuery.prepare("UPDATE users SET last_login = CURRENT_TIMESTAMP WHERE username = :username AND password = :password");
    updateQuery.bindValue(":username", username);
    updateQuery.bindValue(":password", password);

    if (!updateQuery.exec()) {
        qDebug() << "Failed to update last login for user:" << username << "Error:" << updateQuery.lastError().text();
        return;
    }

    // Proceed with login operations if necessary
    qDebug() << "Login successful for user:" << username;
}