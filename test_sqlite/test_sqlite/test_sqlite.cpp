// test_sqlite.cpp : This file contains the 'main' function. Program execution begins and ends there.
//
#include <iostream> 
#include "sqlite/sqlite3.h"
#pragma comment(lib, "sqlite/sqlite3.lib")

using namespace std;
static int callback(void* NotUsed, int argc, char** argv, char** azColName)
{
	int i;
	for (i = 0; i < argc; i++)
	{
		cout << azColName[i] << " = " << (argv[i] ? argv[i] : "NULL") << "\n";
	}
	cout << "\n";
	return 0;
}

int main()
{
	const int STATEMENTS = 8;
	sqlite3* db;
	char* zErrMsg = 0;
	const char* pSQL[STATEMENTS];
	int rc;

	rc = sqlite3_open("familyGuy.db", &db);

	if (rc)
	{
		cout << "Can't open database: " << sqlite3_errmsg(db) << "\n";
	}
	else
	{
		cout << "Open database successfully\n\n";
	}

	rc = sqlite3_key(db, "12345678", 8);

	pSQL[0] = "drop table myTable";

	pSQL[1] = "create table myTable (FirstName varchar(30), LastName varchar(30), Age smallint, Hometown varchar(30), Job varchar(30))";

	pSQL[2] = "insert into myTable (FirstName, LastName, Age, Hometown, Job) values ('Peter', 'Griffin', 41, 'Quahog', 'Brewery')";

	pSQL[3] = "insert into myTable (FirstName, LastName, Age, Hometown, Job) values ('Lois', 'Griffin', 40, 'Newport', 'Piano Teacher')";

	pSQL[4] = "insert into myTable (FirstName, LastName, Age, Hometown, Job) values ('Joseph', 'Swanson', 39, 'Quahog', 'Police Officer')";

	pSQL[5] = "insert into myTable (FirstName, LastName, Age, Hometown, Job) values ('Glenn', 'Quagmire', 41, 'Quahog', 'Pilot')";

	pSQL[6] = "select * from myTable";

	pSQL[7] = "select sum(Age) from myTable where Age >= 40";
	//pSQL[6] = "delete from myTable";

	//pSQL[7] = "drop table myTable";

	for (int i = 0; i < STATEMENTS; i++)
	{
		rc = sqlite3_exec(db, pSQL[i], callback, 0, &zErrMsg);
		if (rc != SQLITE_OK && i != 0)
		{
			cout << "SQL error: " << sqlite3_errmsg(db) << "\n";
			sqlite3_free(zErrMsg);
			break;
		}
	}

	sqlite3_close(db);

	return 0;
}
// Run program: Ctrl + F5 or Debug > Start Without Debugging menu
// Debug program: F5 or Debug > Start Debugging menu

// Tips for Getting Started: 
//   1. Use the Solution Explorer window to add/manage files
//   2. Use the Team Explorer window to connect to source control
//   3. Use the Output window to see build output and other messages
//   4. Use the Error List window to view errors
//   5. Go to Project > Add New Item to create new code files, or Project > Add Existing Item to add existing code files to the project
//   6. In the future, to open this project again, go to File > Open > Project and select the .sln file
