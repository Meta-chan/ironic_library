#define IR_IMPLEMENT
#include "ir_database_table.h"
#include <stdio.h>

ir::Database *database;

void test_insert(const char *key, const char *data, ir::Database::insertmode mode, ir::ec rightcode)
{
	printf("Adding key = '%s', data = '%s'\n", key, data);
	ir::Database::Data bkey(strlen(key) + 1, key);
	ir::Database::Data bdata(strlen(data) + 1, data);
	ir::ec code = database->insert(bkey, bdata, mode);
	printf("Errorcode : %u\n", code);
	printf("Test: %s\n\n", code == rightcode ? "ok" : "error");
};

void test_delete(const char *key, ir::Database::deletemode mode, ir::ec rightcode)
{
	printf("Deleting key = '%s'\n", key);
	ir::ec code = database->delet(ir::Database::Data(strlen(key) + 1, key), mode);
	printf("Result : %u\n", code);
	printf("Test: %s\n\n", code == rightcode ? "ok" : "error");
};

void test_read(const char *key, const char *rightdata, ir::ec rightcode)
{
	printf("Reading key = '%s'\n", key);
	ir::Database::Data result;
	ir::ec code = database->read(ir::Database::Data(strlen(key) + 1, key), &result);
	printf("Result : %u\n", code);
	bool isdataright = false;
	if (code == ir::ec::ec_ok) printf("Data : %s\n", (const char*)result.data);
	bool testok = (rightcode == ir::ec::ec_ok) ?
		(code == ir::ec::ec_ok && strcmp((const char*)result.data, rightdata) == 0) :
		(code == rightcode);
	printf("Test: %s\n\n", testok ? "ok" : "error");
};

int main()
{
	ir::TableDatabase::Settings settings;
	settings.offsetsize = sizeof(short int);
	settings.tablesize = 4;
	settings.mode = ir::TableDatabase::tablemode::chain;
	
	ir::ec code;
	database = new ir::TableDatabase(L"database.idt", &settings, true, &code);

	if (code == ir::ec::ec_ok)
	{
		printf("Database initialized\n");
		test_insert("Rarity", "Sweety Belle", ir::Database::insert_always, ir::ec::ec_ok);
		test_insert("Rainbow Dash", "Scootaloo", ir::Database::insert_always, ir::ec::ec_ok);
		test_insert("Celestia", "Luna", ir::Database::insert_not_existing, ir::ec::ec_ok);
		test_insert("Rainbow Dash", "Applebloom", ir::Database::insert_not_existing, ir::ec::ec_database_already_exists);
		test_delete("Celestia", ir::Database::delete_existing, ir::ec::ec_ok);
		test_read("Rarity", "Sweety Belle", ir::ec::ec_ok);
		test_read("Celestia", nullptr, ir::ec::ec_database_not_exists);
		test_insert("Celestia", "", ir::Database::insert_not_existing, ir::ec::ec_ok);
		test_read("Celestia", "", ir::ec::ec_ok);
		test_insert("Rarity", "Applejack", ir::Database::insert_not_existing, ir::ec::ec_database_already_exists);
		test_insert("Rarity", "Applejack", ir::Database::insert_existing, ir::ec::ec_ok);
		test_insert("Rarity", "Applejack", ir::Database::insert_always, ir::ec::ec_ok);
	}
	delete database;
};