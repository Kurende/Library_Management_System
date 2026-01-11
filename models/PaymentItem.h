#ifndef PAYMENTITEM_H
#define PAYMENTITEM_H

class PaymentItem
{
private:
    int m_id;
    int m_paymentId;
    int m_transactionId;
    int m_bookId;
    double m_amount;

public:
    PaymentItem();

    // Getters
    int getId() const;
    int getPaymentId() const;
    int getTransactionId() const;
    int getBookId() const;
    double getAmount() const;

    // Setters
    void setId(int id);
    void setPaymentId(int id);
    void setTransactionId(int id);
    void setBookId(int id);
    void setAmount(double amount);
};

#endif // PAYMENTITEM_H
