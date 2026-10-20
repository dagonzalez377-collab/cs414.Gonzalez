#include "transaction.hpp"

Transaction::Transaction(Store& store) : store_(store), backup_(store) {
    // backup_(store) invokes Store's copy constructor, snapshotting the
    // state of the store at the moment the transaction begins.
}

void Transaction::commit() {
    committed_ = true;
}

Transaction::~Transaction() {
    if (!committed_) {
        // Rollback: restore the store to the state it had before the
        // transaction began.
        store_ = backup_;
    }
}
