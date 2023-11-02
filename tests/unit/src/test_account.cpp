/* test_account.cpp
Copyright (c) 2022 by petervdmeer

Endless Sky is free software: you can redistribute it and/or modify it under the
terms of the GNU General Public License as published by the Free Software
Foundation, either version 3 of the License, or (at your option) any later version.

Endless Sky is distributed in the hope that it will be useful, but WITHOUT ANY
WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A
PARTICULAR PURPOSE. See the GNU General Public License for more details.

You should have received a copy of the GNU General Public License along with
this program. If not, see <https://www.gnu.org/licenses/>.
*/

#include "es-test.hpp"

// Include only the tested class's header.
#include "../../../source/Account.h"

// ... and any system includes needed for the test file.
#include <map>
#include <sstream>
#include <string>

using namespace std;

namespace { // test namespace

// #region mock data
// #endregion mock data



// #region unit tests
// run this test first so we don't have to retest the assumption later

SCENARIO( "Create an Account" , "[Account][Creation]" ) {
	GIVEN( "an account" ) {
		Account account;
		WHEN( "a fine is levied" ) {
			REQUIRE( account.TotalDebt() == 0 );
			account.AddFine(10000);
			THEN ( "debt is incurred" ) {
				REQUIRE( account.TotalDebt() == 10000 );
			}
		}
	}
}

SCENARIO( "Operations on credits", "[Account][credits]" ) {
	GIVEN( "An account" ) {
		Account account;
		THEN( "The account starts with 0 credits" ) {
			REQUIRE(account.Credits() == 0);
		}

		WHEN( "SetCredits is called with 5" ) {
			account.SetCredits(5);
			THEN( "The number of credits will be 5" ) {
				REQUIRE(account.Credits() == 5);
			}
		}

		WHEN( "AddCredits is called with 5" ) {
			account.AddCredits(5);
			THEN( "The number of credits will be 5" ) {
				REQUIRE(account.Credits() == 5);
			}
			AND_WHEN( "AddCredits is called with -5" ) {
				account.AddCredits(-5);
				THEN( "The number of credits will be 0" ) {
					REQUIRE(account.Credits() == 0);
				}
			}
		}
	}
}

TEST_CASE( "Set the credit score of the account", "[Acount][SetCreditScore]" ) {
	Account account;
	REQUIRE(account.CreditScore() == 400);

	account.SetCreditScore(500);
	REQUIRE(account.CreditScore() == 500);

	account.SetCreditScore(-100);
	REQUIRE(account.CreditScore() == 200);

	account.SetCreditScore(100);
	REQUIRE(account.CreditScore() == 200);

	account.SetCreditScore(900);
	REQUIRE(account.CreditScore() == 800);
}

SCENARIO( "Operations on history", "[Account][history]" ) {
	GIVEN( "An account" ) {
		Account account;
		THEN( "The account starts with an empty history" ) {
			REQUIRE(account.History().empty());
		}

		WHEN( "AddHistory is called with 1000" ) {
			account.AddHistory(1000);
			THEN( "History wil have a size of 1 and 1000 in the first index." ) {
				REQUIRE(account.History().size() == 1);
				REQUIRE(account.History().at(0) == 1000);
			}
			AND_WHEN( "AddHistory is called 100 more times" ) {
				for(int64_t i = 0; i < 100; i++)
				{
					account.AddHistory(i);
				}
				THEN( "Index 0 will contain 0 and the size will be 100" ) {
					REQUIRE(account.History().at(0) == 0);
					REQUIRE(account.History().size() == 100);
				}
				AND_WHEN( "ResetHistory is called" ) {
					account.ClearHistory();
					THEN( "The size of history will be size 0" ) {
						REQUIRE(account.History().empty());
					}
				}
			}
		}
	}
}

SCENARIO( "Working with mortgages on an account", "[Account][mortgages]" ) {
	GIVEN( "An account" ) {
		Account account;
		WHEN( "A mortgage is added to the account" ) {
			account.AddMortgage(480000);
			THEN( "The user has 1 mortgage and 480,000 credits" ) {
				REQUIRE(account.Credits() == 480000);
				REQUIRE(account.CreditScore() == 400);
				REQUIRE(account.Mortgages().size() == 1);
			}
			AND_WHEN( "A fine is added to the account" ) {
				account.AddFine(20000);
				THEN( "The user has 2 mortgages" ) {
					REQUIRE(account.Credits() == 480000);
					REQUIRE(account.CreditScore() == 400);
					REQUIRE(account.Mortgages().size() == 2);
				}
			}
		}
	}
}

SCENARIO( "Paying crew salaries during Step", "[Account][Step]" ) {
	GIVEN( "An account with no credits" ) {
		Account account;
		WHEN( "Crew salaries are paid during Step" ) {
			string message = account.Step(0, 100, 0);
			THEN( "The crew salaries cannot be paid, is stored as overdue, and the credit score drops" ) {
				REQUIRE(message == "You could not pay all your crew salaries. ");
				REQUIRE(account.OverdueCrewSalaries() == 100);
				REQUIRE(account.CreditScore() == 395);
			}
			AND_WHEN( "The player has enough credits to pay off all crew salaries" ) {
				account.SetCredits(200);
				string message = account.Step(0, 100, 0);
				string expectedMessage = "You paid 200 credits in crew salaries.";
				THEN( "All overdue crew salaries are paid off and the credit score rises" ) {
					REQUIRE(account.Credits() == 0);
					REQUIRE(message == expectedMessage);
					REQUIRE(account.OverdueCrewSalaries() == 0);
					REQUIRE(account.CreditScore() == 396);
				}
			}
		}
	}
}

SCENARIO( "Paying maintenance during Step", "[Account][Step]" ) {
	GIVEN( "An account with no credits" ) {
		Account account;
		WHEN( "Maintenance is paid during Step" ) {
			string message = account.Step(0, 0, 100);
			THEN( "The maintenance cannot be paid, is stored as overdue, and the credit score drops" ) {
				REQUIRE(message == "You could not pay all your maintenance costs. ");
				REQUIRE(account.OverdueMaintenance() == 100);
				REQUIRE(account.CreditScore() == 395);
			}
			AND_WHEN( "The player has enough credits to pay off all maintenance" ) {
				account.SetCredits(200);
				string message = account.Step(0, 0, 100);
				string expectedMessage = "You paid 200 credits in maintenance.";
				THEN( "All overdue maintenance is paid off and the credit score rises" ) {
					REQUIRE(account.Credits() == 0);
					REQUIRE(message == expectedMessage);
					REQUIRE(account.OverdueMaintenance() == 0);
					REQUIRE(account.CreditScore() == 396);
				}
			}
		}
	}
}

SCENARIO( "Paying Mortgages during Step", "[Account][Step]" ) {
	GIVEN( "An account with a mortgage" ) {
		Account account;
		int64_t principal = 20000;
		account.AddMortgage(principal);
		int64_t expectedPayment = account.Mortgages().at(0).Payment();
		ostringstream expectedMessage;
		expectedMessage << "You paid " << expectedPayment << " credits in mortgages.";
		WHEN( "A payment is made by an account that has enough credits" ) {
			string message = account.Step(0, 0, 0);
			THEN("The mortgage payment is made successfully and the credit score increases") {
				REQUIRE(account.Credits() == (principal - expectedPayment));
				REQUIRE(message == expectedMessage.str());
				REQUIRE(account.CreditScore() == 401);
			}
		}
		WHEN( "A payment is made by an account that does NOT enough credits" ) {
			account.SetCredits(5);
			string message = account.Step(0, 0, 0);
			THEN("The mortgage payment is not made, the principal increases, and the credit score decreases") {
				REQUIRE(account.Credits() == 5);
				REQUIRE(message == "You missed a mortgage payment. ");
				REQUIRE(account.Mortgages().at(0).Principal() > principal);
				REQUIRE(account.CreditScore() == 395);
			}
		}
		WHEN( "A final payment is made" ) {
			account.SetCredits(principal * 2);
			account.PayExtra(0, principal-1);
			string message = account.Step(0, 0, 0);
			THEN("The mortgage payment is made successfully, the credit score increases, and the mortgage is removed from the account") {
				REQUIRE(message == "You paid 1 credit in mortgages.");
				REQUIRE(account.Mortgages().size() == 0);
				REQUIRE(account.CreditScore() == 401);
			}
		}
	}
}

SCENARIO( "Paying Fines during Step", "[Account][Step]" ) {
	GIVEN( "An account with a fine" ) {
		Account account;
		int64_t principal = 1000;
		account.AddCredits(principal);
		account.AddFine(principal);
		int64_t expectedPayment = account.Mortgages().at(0).Payment();
		ostringstream expectedMessage;
		expectedMessage << "You paid " << expectedPayment << " credits in fines.";
		WHEN( "A payment is made by an account that has enough credits" ) {
			string message = account.Step(0, 0, 0);
			THEN("The fine payment is made successfully and the credit score increases") {
				REQUIRE(account.Credits() == (principal - expectedPayment));
				REQUIRE(message == expectedMessage.str());
				REQUIRE(account.CreditScore() == 401);
			}
		}
		WHEN( "A payment is made by an account that does NOT enough credits" ) {
			account.SetCredits(5);
			string message = account.Step(0, 0, 0);
			THEN("The fine payment is not made, the principal increases, and the credit score decreases") {
				REQUIRE(account.Credits() == 5);
				REQUIRE(message == "You missed a mortgage payment. ");
				REQUIRE(account.Mortgages().at(0).Principal() > principal);
				REQUIRE(account.CreditScore() == 395);
			}
		}
		WHEN( "A final payment is made" ) {
			account.SetCredits(principal * 2);
			account.PayExtra(0, principal-1);
			string message = account.Step(0, 0, 0);
			THEN("The fine payment is made successfully, the credit score increases, and the mortgage is removed from the account") {
				REQUIRE(message == "You paid 1 credit in fines.");
				REQUIRE(account.Mortgages().size() == 0);
				REQUIRE(account.CreditScore() == 401);
			}
		}
	}
}

SCENARIO( "Operations on overdueCrewSalaries", "[Account][overdueCrewSalaries]" ) {
	GIVEN( "An account" ) {
		Account account;
		WHEN( "overdueCrewSalaries is 0" ) {
			THEN( "OverdueCrewSalaries will return 0" ) {
				REQUIRE(account.OverdueCrewSalaries() == 0);
			}
			AND_WHEN( "SetOverdueCrewSalaries is used to set a value" ) {
				account.SetOverdueCrewSalaries(1000);
				THEN( "overdueCrewSalaries is set to that value" ) {
					REQUIRE(account.OverdueCrewSalaries() == 1000);
				}
				AND_WHEN( "PayOverdueCrewSalaries is used to pay off that value" ) {
					account.SetCredits(1000);
					account.PayOverdueCrewSalaries(1000);
					THEN( "overdueCrewSalaries is 0" ) {
						REQUIRE(account.OverdueCrewSalaries() == 0);
					}
				}
			}
		}
	}
}

SCENARIO( "Operations on overdueMaintenance", "[Account][overdueMaintenance]" ) {
	GIVEN( "An account" ) {
		Account account;
		WHEN( "overdueMaintenance is 0" ) {
			THEN( "overdueMaintenance will return 0" ) {
				REQUIRE(account.OverdueMaintenance() == 0);
			}
			AND_WHEN( "SetOverdueMaintenance is used to set a value" ) {
				account.SetOverdueMaintenance(1000);
				THEN( "overdueMaintenance is set to that value" ) {
					REQUIRE(account.OverdueMaintenance() == 1000);
				}
				AND_WHEN( "PayOverdueMaintenance is used to pay off that value" ) {
					account.SetCredits(1000);
					account.PayOverdueMaintenance(1000);
					THEN( "overdueMaintenance is 0" ) {
						REQUIRE(account.OverdueMaintenance() == 0);
					}
				}
			}
		}
	}
}

SCENARIO( "Operations on player salaries", "[Account][salariesIncome]" ) {
	GIVEN( "An account" ) {
		Account account;
		THEN( "The account starts with no player salaries" ) {
			REQUIRE(account.SalariesIncome().empty());
		}

		WHEN( "SetSalariesIncome is called" ) {
			account.SetSalariesIncome("test", 1000);
			THEN( "The data is stored properly" ) {
				REQUIRE(account.SalariesIncome().size() == 1);
				REQUIRE(account.SalariesIncome().find("test")->second == 1000);
			}
			AND_WHEN( "SalariesIncomeTotal is called after adding another couple salaries" ) {
				account.SetSalariesIncome("test2", 2000);
				account.SetSalariesIncome("test3", 3000);
				THEN("The correct total is returned") {
					REQUIRE(account.SalariesIncomeTotal() == 6000);
				}
				AND_WHEN("SetSalariesIncome is called to zero out the first salary") {
					account.SetSalariesIncome("test", 0);
					THEN( "Only the last two salaries remain" ) {
						REQUIRE(account.SalariesIncome().size() == 2);
						REQUIRE(account.SalariesIncome().find("test") ==
							account.SalariesIncome().end());
					}
				}
				AND_WHEN("RemoveSalariesIncome is called to zero out the first salary") {
					account.RemoveSalariesIncome("test");
					THEN( "Only the last two salaries remain" ) {
						REQUIRE(account.SalariesIncome().size() == 2);
						REQUIRE(account.SalariesIncome().find("test") ==
							account.SalariesIncome().end());
					}
				}
				AND_WHEN("ClearSalariesIncome is called") {
					account.ClearSalariesIncome();
					THEN( "The map of player salaries is empty" ) {
						REQUIRE(account.SalariesIncome().size() == 0);
					}
				}
			}
		}
	}
}

SCENARIO( "Step forward" , "[Account][Step]" ) {
	GIVEN( "An account with 1000 credits" ) {
		Account account;
		account.AddCredits(1000);
		THEN( "Account has 1000 credits" ) {
			REQUIRE( account.Credits() == 1000 );
		}

		WHEN( "Step() is called with no crew" ) {
			int64_t assets = 0;      // This is net worth of all ships
			int64_t salaries = 0;    // total owed in a single day's salaries
			int64_t maintenance = 0; // sum of maintenance and generated income

			string message = account.Step(assets, salaries, maintenance);
			INFO(message);
			string out = "";
			REQUIRE( message.compare(out) == 0 );
		}
	}
}

TEST_CASE( "Calculate the player's net worth", "[Account][CalculateNetWorth]" ) {
	Account account;
	account.AddMortgage(1000);
	account.AddFine(1000);
	account.SetOverdueCrewSalaries(1000);
	account.SetOverdueMaintenance(1000);
	REQUIRE(account.CalculateNetWorth(5000) == 1000);
}

SCENARIO( "Testing credit score updates", "[Account][UpdateCreditScore]" ) {
	GIVEN( "An account with a credit score of 400 and a bunch of receipts" ) {
		Account account;
		REQUIRE(account.CreditScore() == 400);

		Receipt salariesPaid, maintencancePaid, mortgagesPaid, finesPaid;
		std::vector<Receipt> receipts = {salariesPaid, maintencancePaid, mortgagesPaid, finesPaid};

		WHEN( "UpdateCreditScore is called with all receipts paid in full" ) {
			account.UpdateCreditScore(&receipts);
			THEN( "The account's credit score is 401" ) {
				REQUIRE(account.CreditScore() == 401);
			}
		}

		WHEN( "UpdateCreditScore is called with salariesPaid NOT paid in full" ) {
			receipts.at(0).paidInFull = false;
			account.UpdateCreditScore(&receipts);
			THEN( "The account's credit score is 395" ) {
				REQUIRE(account.CreditScore() == 395);
			}
		}

		WHEN( "UpdateCreditScore is called with maintencancePaid NOT paid in full" ) {
			receipts.at(1).paidInFull = false;
			account.UpdateCreditScore(&receipts);
			THEN( "The account's credit score is 395" ) {
				REQUIRE(account.CreditScore() == 395);
			}
		}

		WHEN( "UpdateCreditScore is called with mortgagesPaid NOT paid in full" ) {
			receipts.at(2).paidInFull = false;
			account.UpdateCreditScore(&receipts);
			THEN( "The account's credit score is 395" ) {
				REQUIRE(account.CreditScore() == 395);
			}
		}

		WHEN( "UpdateCreditScore is called with finesPaid NOT paid in full" ) {
			receipts.at(3).paidInFull = false;
			account.UpdateCreditScore(&receipts);
			THEN( "The account's credit score is 395" ) {
				REQUIRE(account.CreditScore() == 395);
			}
		}

		WHEN( "UpdateCreditScore is called with more than one bill not paid in full" ) {
			receipts.at(0).paidInFull = false;
			receipts.at(1).paidInFull = false;
			receipts.at(2).paidInFull = false;
			receipts.at(3).paidInFull = false;
			account.UpdateCreditScore(&receipts);
			THEN( "The account's credit score is 395" ) {
				REQUIRE(account.CreditScore() == 395);
			}
		}
	}
}

SCENARIO("Check if any bills were paid", "[Account][AnyPaymentsMade]") {
	GIVEN("A list of reciepts") {
		Receipt salariesPaid, maintencancePaid, mortgagesPaid, finesPaid;
		std::vector<Receipt> receipts = {salariesPaid, maintencancePaid, mortgagesPaid, finesPaid};

		WHEN("No bill had a payment made") {
			bool anyPaymentsMade = Account::AnyPaymentsMade(&receipts);
			THEN( "Return false" ) {
				REQUIRE(anyPaymentsMade == false);
			}
		}

		WHEN("A salary payment was made") {
			receipts.at(0).creditsPaid = 100;
			bool anyPaymentsMade = Account::AnyPaymentsMade(&receipts);
			THEN( "Return true" ) {
				REQUIRE(anyPaymentsMade == true);
			}
		}

		WHEN("A maintenance payment was made") {
			receipts.at(1).creditsPaid = 100;
			bool anyPaymentsMade = Account::AnyPaymentsMade(&receipts);
			THEN( "Return true" ) {
				REQUIRE(anyPaymentsMade == true);
			}
		}

		WHEN("A mortgage payment was made") {
			receipts.at(2).creditsPaid = 100;
			bool anyPaymentsMade = Account::AnyPaymentsMade(&receipts);
			THEN( "Return true" ) {
				REQUIRE(anyPaymentsMade == true);
			}
		}

		WHEN("A fine payment was made") {
			receipts.at(3).creditsPaid = 100;
			bool anyPaymentsMade = Account::AnyPaymentsMade(&receipts);
			THEN( "Return true" ) {
				REQUIRE(anyPaymentsMade == true);
			}
		}

		WHEN("A payment of every type was made") {
			receipts.at(0).creditsPaid = 100;
			receipts.at(1).creditsPaid = 100;
			receipts.at(2).creditsPaid = 100;
			receipts.at(3).creditsPaid = 100;
			bool anyPaymentsMade = Account::AnyPaymentsMade(&receipts);
			THEN( "Return true" ) {
				REQUIRE(anyPaymentsMade == true);
			}
		}
	}
}

SCENARIO( "Test GetTypesPaid", "[Account][GetTypesPaid]" ) {
	GIVEN("A list of reciepts") {
		Receipt salariesPaid, maintencancePaid, mortgagesPaid, finesPaid;
		std::vector<Receipt> receipts = {salariesPaid, maintencancePaid, mortgagesPaid, finesPaid};
		WHEN("No bill had a payment made") {
			map<string, int64_t> typesPaid = Account::GetTypesPaid(&receipts);
			THEN( "Return an empty map" ) {
				REQUIRE(typesPaid.empty());
			}
		}

		WHEN("A salary payment was made") {
			receipts.at(0).creditsPaid = 100;
			map<string, int64_t> typesPaid = Account::GetTypesPaid(&receipts);
			THEN( "Return a map with salaries data" ) {
				REQUIRE(typesPaid["crew salaries"] == 100);
			}
		}

		WHEN("A maintenance payment was made") {
			receipts.at(1).creditsPaid = 100;
			map<string, int64_t> typesPaid = Account::GetTypesPaid(&receipts);
			THEN( "Return a map with maintenance data" ) {
				REQUIRE(typesPaid["maintenance"] == 100);
			}
		}

		WHEN("A mortgage payment was made") {
			receipts.at(2).creditsPaid = 100;
			map<string, int64_t> typesPaid = Account::GetTypesPaid(&receipts);
			THEN( "Return a map with mortgage data" ) {
				REQUIRE(typesPaid["mortgages"] == 100);
			}
		}

		WHEN("A fine payment was made") {
			receipts.at(3).creditsPaid = 100;
			map<string, int64_t> typesPaid = Account::GetTypesPaid(&receipts);
			THEN( "Return a map with fine data" ) {
				REQUIRE(typesPaid["fines"] == 100);
			}
		}

		WHEN("A payment of every type was made") {
			receipts.at(0).creditsPaid = 100;
			receipts.at(1).creditsPaid = 100;
			receipts.at(2).creditsPaid = 100;
			receipts.at(3).creditsPaid = 100;
			map<string, int64_t> typesPaid = Account::GetTypesPaid(&receipts);
			THEN( "Return a map with all data types" ) {
				REQUIRE(typesPaid.size() == 4);
			}
		}
	}
}

SCENARIO( "Generating payment logs", "[Account][GeneratePaymentLogs]" ) {
	Receipt salariesPaid, maintencancePaid, mortgagesPaid, finesPaid;
	std::vector<Receipt> receipts = {salariesPaid, maintencancePaid, mortgagesPaid, finesPaid};

	GIVEN("A list of receipts") {
		WHEN("All receipts are fully paid") {
			receipts.at(0).creditsPaid = 100;
			receipts.at(1).creditsPaid = 100;
			receipts.at(2).creditsPaid = 100;
			receipts.at(3).creditsPaid = 100;
			ostringstream expectedLog;
			expectedLog << "You paid ";
			string logs = Account::GeneratePaymentLogs(&receipts);
			THEN( "The log includes every option" ) {
				expectedLog << "100 credits in crew salaries, ";
				expectedLog << "100 credits in fines, ";
				expectedLog << "100 credits in maintenance, ";
				expectedLog << "and 100 credits in mortgages.";
				REQUIRE(logs.compare(expectedLog.str()) == 0);
			}
		}
	}
}

// #endregion unit tests



} // test namespace
