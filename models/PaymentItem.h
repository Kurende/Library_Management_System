#ifndef PAYMENTITEM_H
#define PAYMENTITEM_H

class PaymentItem {
public:
    PaymentItem();
    PaymentItem(int id, int paymentId, int transactionId, int bookId, double amount);

    // Getters
    int getId() const { return m_id; }
    int getPaymentId() const { return m_paymentId; }
    int getTransactionId() const { return m_transactionId; }
    int getBookId() const { return m_bookId; }
    double getAmount() const { return m_amount; }

    // Setters
    void setId(int id) { m_id = id; }
    void setPaymentId(int paymentId) { m_paymentId = paymentId; }
    void setTransactionId(int transactionId) { m_transactionId = transactionId; }
    void setBookId(int bookId) { m_bookId = bookId; }
    void setAmount(double amount) { m_amount = amount; }

private:
    int m_id;
    int m_paymentId;
    int m_transactionId;
    int m_bookId;
    double m_amount;
};

#endif // PAYMENTITEM_H
