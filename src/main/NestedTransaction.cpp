/* The MIT License (MIT)
 * Copyright (c) 2014 Nicholas Wright
 * http://opensource.org/licenses/MIT
 */

/**
 * @file NestedTransaction.cpp
 *
 * @date 8 Nov 2014
 * @author Nicholas Wright
 * @brief an undocumented file
 * 
 * Some more details about this file.
 */

#include "NestedTransaction.hpp"

namespace imageHasher {
namespace db {

NestedTransaction::NestedTransaction(odb::transaction_impl* impl) {
	this->active_transaction = odb::transaction::has_current();

	if(this->active_transaction) {
		// use the existing transaction
		this->transaction = std::auto_ptr<odb::transaction>();
	}else{
		// create a new transaction
		this->transaction = std::auto_ptr<odb::transaction>(new odb::transaction(impl));
	}
}

void NestedTransaction::commit() {
	if(!this->active_transaction) {
		transaction->commit();
	}
}

void NestedTransaction::rollback() {
	if(!this->active_transaction) {
		transaction->rollback();
	}
}
} /* namespace db */
} /* namespace imageHasher */

