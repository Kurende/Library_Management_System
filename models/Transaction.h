#ifndef TRANSACTION_H
#define TRANSACTION_H

#include <QString>
#include <QDate>
#include <QDateTime>

class Transaction {
public:
    // Enums
    enum class Status {
        Active,      // Book is currently borrowed
        Returned,    // Book was returned
        Lost,         // Book was marked as lost
        Paid
    };

    // Constructors
    Transaction();
    Transaction(int id, int learnerId, int bookId, 
                const QDate& borrowDate, const QDate& dueDate,
                const QDate& returnDate, Status status,
                const QDateTime& createdAt);

    // Getters
    int getId() const { return m_id; }
    int getLearnerId() const { return m_learnerId; }
    int getBookId() const { return m_bookId; }
    QDate getBorrowDate() const { return m_borrowDate; }
    QDate getDueDate() const { return m_dueDate; }
    QDate getReturnDate() const { return m_returnDate; }
    Status getStatus() const { return m_status; }
    QString getStatusString() const;
    QDateTime getCreatedAt() const { return m_createdAt; }

    // Setters
    void setId(int id) { m_id = id; }
    void setLearnerId(int learnerId) { m_learnerId = learnerId; }
    void setBookId(int bookId) { m_bookId = bookId; }
    void setBorrowDate(const QDate& date) { m_borrowDate = date; }
    void setDueDate(const QDate& date) { m_dueDate = date; }
    void setReturnDate(const QDate& date) { m_returnDate = date; }
    void setStatus(Status status) { m_status = status; }
    void setCreatedAt(const QDateTime& dateTime) { m_createdAt = dateTime; }

    // Business logic methods
    bool isOverdue() const;
    int getDaysOverdue() const;
    bool isActive() const { return m_status == Status::Active; }
    bool isReturned() const { return m_status == Status::Returned; }
    bool isLost() const { return m_status == Status::Lost; }

    // Static helper methods
    static Status stringToStatus(const QString& statusStr);
    static QString statusToString(Status status);
    static QDate calculateDueDate(const QDate& borrowDate);

private:
    int m_id;
    int m_learnerId;
    int m_bookId;
    QDate m_borrowDate;
    QDate m_dueDate;
    QDate m_returnDate;
    Status m_status;
    QDateTime m_createdAt;
};

#endif // TRANSACTION_H
