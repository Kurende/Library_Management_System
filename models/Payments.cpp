#ifndef PAYMENTITEM_H
#define PAYMENTITEM_H

class PaymentItem {
private:
    int m_id;
    int m_paymentId;
    int m_transactionId;
    int m_bookId;
    double m_amount;

public:
    PaymentItem() : m_id(-1), m_paymentId(-1), m_transactionId(-1), m_bookId(-1), m_amount(0.0) {}

    // Getters
    int getId() const { return m_id; }
    int getPaymentId() const { return m_paymentId; }
    int getTransactionId() const { return m_transactionId; }
    int getBookId() const { return m_bookId; }
    double getAmount() const { return m_amount; }

    // Setters
    void setId(int id) { m_id = id; }
    void setPaymentId(int id) { m_paymentId = id; }
    void setTransactionId(int id) { m_transactionId = id; }
    void setBookId(int id) { m_bookId = id; }
    void setAmount(double amount) { m_amount = amount; }
};

#endif // PAYMENTITEM_H
