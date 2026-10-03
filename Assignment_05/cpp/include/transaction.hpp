#pragma once

#include "store.hpp"

// RAII-based transaction. Construction preserves (backs up) the current
// state of the store. If commit() is called, changes made through the
// store during the transaction's lifetime remain. If the Transaction
// object is destroyed without commit() having been called, the store is
// restored to the state it had when the transaction began -- this
// restoration is the "meaningful resource/state-management operation"
// performed by the destructor.
class Transaction {
public:
    explicit Transaction(Store& store);
    ~Transaction();

    void commit();

    // Transactions are not copyable: copying would duplicate ownership
    // of the rollback responsibility.
    Transaction(const Transaction&) = delete;
    Transaction& operator=(const Transaction&) = delete;

private:
    Store& store_;
    Store backup_;
    bool committed_{false};
};
