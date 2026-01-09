#include "Transaction.h"

Transaction::Transaction()
    : m_id(-1), m_learnerId(-1), m_bookId(-1), m_status(Status::Active) {
}

Transaction::Transaction(int id, int learnerId, int bookId,
                         const QDate& borrowDate, const QDate& dueDate,
                         const QDate& returnDate, Status status,
                         const QDateTime& createdAt)
    : m_id(id), m_learnerId(learnerId), m_bookId(bookId),
      m_borrowDate(borrowDate), m_dueDate(dueDate), m_returnDate(returnDate),
      m_status(status), m_createdAt(createdAt) {
}

QString Transaction::getStatusString() const {
    return statusToString(m_status);
}

bool Transaction::isOverdue() const {
    if (m_status != Status::Active) return false;
    return QDate::currentDate() > m_dueDate;
}

int Transaction::getDaysOverdue() const {
    if (!isOverdue()) return 0;
    return m_dueDate.daysTo(QDate::currentDate());
}

Transaction::Status Transaction::stringToStatus(const QString& statusStr) {
    QString lower = statusStr.toLower();
    if (lower == "returned") return Status::Returned;
    if (lower == "lost") return Status::Lost;
    return Status::Active; // Default
}

QString Transaction::statusToString(Status status) {
    switch (status) {
        case Status::Active: return "Active";
        case Status::Returned: return "Returned";
        case Status::Lost: return "Lost";
        default: return "Active";
    }
}

QDate Transaction::calculateDueDate(const QDate& borrowDate) {
    // Books must be returned by November 28 of the borrow year
    int year = borrowDate.year();
    
    // If borrowed after November 28, due date is November 28 of next year
    if (borrowDate.month() > 11 || 
        (borrowDate.month() == 11 && borrowDate.day() > 28)) {
        year++;
    }
    
    return QDate(year, 11, 28);
}