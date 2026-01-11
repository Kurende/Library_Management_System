#ifndef PAYMENT_H
#define PAYMENT_H

#include <QString>
#include <QDateTime>

class Payment {
private:
    int m_id;
    QString m_receiptNo;
    int m_learnerId;
    double m_amount;
    int m_processedBy;
    QDateTime m_paymentDate;
    QString m_notes;

public:
    Payment() : m_id(-1), m_learnerId(-1), m_amount(0.0), m_processedBy(-1) {}

    // Getters
    int getId() const { return m_id; }
    QString getReceiptNo() const { return m_receiptNo; }
    int getLearnerId() const { return m_learnerId; }
    double getAmount() const { return m_amount; }
    int getProcessedBy() const { return m_processedBy; }
    QDateTime getPaymentDate() const { return m_paymentDate; }
    QString getNotes() const { return m_notes; }

    // Setters
    void setId(int id) { m_id = id; }
    void setReceiptNo(const QString& no) { m_receiptNo = no; }
    void setLearnerId(int id) { m_learnerId = id; }
    void setAmount(double amount) { m_amount = amount; }
    void setProcessedBy(int userId) { m_processedBy = userId; }
    void setPaymentDate(const QDateTime& date) { m_paymentDate = date; }
    void setNotes(const QString& notes) { m_notes = notes; }

    // Helper - Generate unique receipt number
    QString generateReceiptNo() const {
        return QString("RCP-%1").arg(QDateTime::currentDateTime().toString("yyyyMMddhhmmss"));
    }
};

#endif // PAYMENT_H
