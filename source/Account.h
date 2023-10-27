/* Account.h
Copyright (c) 2014 by Michael Zahniser

Endless Sky is free software: you can redistribute it and/or modify it under the
terms of the GNU General Public License as published by the Free Software
Foundation, either version 3 of the License, or (at your option) any later version.

Endless Sky is distributed in the hope that it will be useful, but WITHOUT ANY
WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A
PARTICULAR PURPOSE. See the GNU General Public License for more details.

You should have received a copy of the GNU General Public License along with
this program. If not, see <https://www.gnu.org/licenses/>.
*/

#ifndef ACCOUNT_H_
#define ACCOUNT_H_

#include "Mortgage.h"

#include <cstdint>
#include <map>
#include <string>
#include <tuple>
#include <vector>

class DataNode;
class DataWriter;

// A utility stucture to return multpile values needed during processing
// inside the Step function and its subprocesses.
struct Receipt {
	bool paidInFull = true;
	int64_t creditsPaid = 0;
};


// Class representing all your assets and liabilities and tracking their change
// over time.
class Account {
public:
	// -- Load or save account data.

	// Load account information from a data file (saved game or starting conditions).
	void Load(const DataNode &node, bool clearFirst);

	// Write account information to a saved game file.
	void Save(DataWriter &out) const;

	// -- Functions operating on the player's credits.

	// Get much the player currently has in the bank.
	int64_t Credits() const;

	// Set the number of credits in the players account.
	void SetCredits(int64_t value);

	// Give the player credits (or pass negative number to subtract). If subtracting,
	// the calling function needs to check that this will not result in negative credits.
	void AddCredits(int64_t value);

	// Remove the player's credits (or pass negative number to add). If subtracting,
	// the calling function needs to check that this will not result in negative credits.
	void SubtractCredits(int64_t value);

	// -- Functions operating on the player's credit score.

	// Get the player's credit rating.
	int CreditScore() const;

	// Specify the player's credit score (cannot be set lower than 200, or greater than 800).
	void SetCreditScore(int64_t value);

	// -- Functions operating on the player's crew salaries.

	// Access the overdue crew salaries.
	int64_t OverdueCrewSalaries() const;

	// Pay off the current overdue crew salaries by a given amount.
	void PayOverdueCrewSalaries(int64_t amount);

	// -- Functions operating on the player's history.

	// Access the history of the player's net worth.
	const std::vector<int64_t> &History() const;

	// -- Functions operating on the player's maintenance.

	// Access the overdue maintenance costs.
	int64_t OverdueMaintenance() const;

	// Pay off the current overdue maintenance by a given amount.
	void PayMaintenance(int64_t amount); // TODO: RENAME THIS TO PayOverdueMaintenance

	// -- Functions operating on the player's mortgages and fines.

	// Access the list of mortgages.
	const std::vector<Mortgage> &Mortgages() const;

	// Add a new mortgage for the given amount, with an interest rate determined by your credit score.
	void AddMortgage(int64_t principal);

	// Add a "fine" as a mortgage.
	void AddFine(int64_t amount);

	// Pay down extra principal on a mortgage.
	void PayExtra(int mortgage, int64_t amount);

	// Check how big a mortgage the player can afford to pay at their current income.
	int64_t Prequalify() const;

	// -- Functions operating on the player's salaries.

	// Access the map of player salaries.
	const std::map<std::string, int64_t> &SalariesIncome() const;

	// Return the sum of all player salaries.
	int64_t SalariesIncomeTotal() const;

	// Set the number of credits in a specified salary.
	void SetSalaryIncome(std::string name, int64_t amount);

	// -- Miscellaneous Functions

	// Step forward one day, and return a string summarizing payments made.
	std::string Step(int64_t assets, int64_t salaries, int64_t maintenance);

	// Attempt to pay all bills, returning the results.
	std::vector<Receipt> PayBills(int64_t salaries, int64_t maintenance);

	// Attempt to pay crew salaries, returning the results.
	Receipt PayCrewSalaries(int64_t salaries);

	// Attempt to pay mainenance costs, returning the results.
	Receipt PayShipMaintenance(int64_t maintenance); // TODO: RENAME THIS TO PayMaintenance

	// Attempt to mortgages, returning the results.
	Receipt PayMortgages();

	// Attempt to pay fines, returning the results.
	Receipt PayFines();

	// Generate a log string from the given receipts.
	const std::string GenerateMissedPaymentLogs(std::vector<Receipt> *receipts) const;

	// Update the lists of mortgages the player holds.
	// This will remove any mortgages with a principal of 0.
	void UpdateMortgages();

	// Update the history of the player's net worth using the given assets.
	// If the history has grown too long, the oldest instance is removed as well.
	void UpdateHistory(int64_t assets);

	// Calculate the player's net worth based on the given assets.
	int64_t CalculateNetWorth(int64_t assets) const;

	// Update the players credit score based on the given receipts.
	// The credit score will always be set to a number between 200
	// and 800, inclusive.
	void UpdateCreditScore(std::vector<Receipt> *receipts);

	// Check the receipts to see if any contained a non-zero payment.
	static bool AnyPaymentsMade(std::vector<Receipt> *receipts);

	// Check the receipts to see what types fo payments were made.
	// If none were made, the map returned will be empty.
	static std::map<std::string, int64_t> GetTypesPaid(std::vector<Receipt> *receipts);

	// Generate a log string from the given receipts.
	static std::string GeneratePaymentLogs(std::vector<Receipt> *receipts);

	// Get the player's total net worth (counting all ships and all debts).
	int64_t NetWorth() const;

	// Get the total amount owed for "Mortgage", "Fine", or both.
	int64_t TotalDebt(const std::string &type = "") const;


private:
	// Extrapolate from the player's current net worth history to determine how much
	// their net worth is expected to change over the course of the next year.
	int64_t YearlyRevenue() const;


private:
	// The liquid cash a player has in their account.
	int64_t credits = 0;

	// Your credit score determines the interest rate on your mortgages.
	int creditScore = 400;

	// If back salaries cannot be paid, it piles up rather than being ignored.
	int64_t overdueCrewSalaries = 0;

	// History of the player's net worth. This is used to calculate your average
	// daily income, which is used to calculate how big a mortgage you can afford.
	std::vector<int64_t> history;

	// If back maintenance cannot be paid, it piles up rather than being ignored.
	int64_t overdueMaintenance = 0;

	// A list containing mortgages taken out and fines assigned to the player.
	std::vector<Mortgage> mortgages;

	// Regular income from salaries paid to the player.
	std::map<std::string, int64_t> salariesIncome;
};



#endif
