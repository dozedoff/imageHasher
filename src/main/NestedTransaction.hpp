/* The MIT License (MIT)
 * Copyright (c) 2014 Nicholas Wright
 * http://opensource.org/licenses/MIT
 */

/**
 * @file NestedTransaction.hpp
 *
 * @date 8 Nov 2014
 * @author Nicholas Wright
 * @brief an undocumented file
 * 
 * Some more details about this file.
 */

#ifndef NESTEDTRANSACTION_HPP_
#define NESTEDTRANSACTION_HPP_

#include <odb/transaction.hxx>
#include <memory>

namespace imageHasher {
namespace db {

class NestedTransaction {
	public:
	NestedTransaction(odb::transaction_impl* impl);

	void commit();
	void rollback();

	private:

	bool active_transaction;
	std::unique_ptr<odb::transaction> transaction;
};

} /* namespace db */
} /* namespace imageHasher */
#endif /* NESTEDTRANSACTION_HPP_ */
