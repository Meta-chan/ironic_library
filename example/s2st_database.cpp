#define IR_INCLUDE 'a'
#include "../include/ir/s2st_database.h"
#include <stdio.h>

ir::S2STDatabase *database;

void test_insert(const char *key, const char *data, ir::Database::insert_mode mode, ir::ec rightcode)
{
	printf("Adding key = '%s', data = '%s'\n", key, data);
	ir::Block bkey(key, strlen(key) + 1);
	ir::Block bdata(data, strlen(data) + 1);
	ir::ec code = database->insert(bkey, bdata, mode);
	printf("Errorcode : %u\n", code);
	printf("Test: %s\n\n", code == rightcode ? "ok" : "error");
}

void test_delete(const char *key, ir::Database::delete_mode mode, ir::ec rightcode)
{
	printf("Deleting key = '%s'\n", key);
	ir::ec code = database->delet(ir::Block(key, strlen(key) + 1), mode);
	printf("Result : %u\n", code);
	printf("Test: %s\n\n", code == rightcode ? "ok" : "error");
}

void test_read(const char *key, const char *rightdata, ir::ec rightcode)
{
	printf("Reading key = '%s'\n", key);
	ir::Block result;
	ir::ec code = database->read(ir::Block(key, strlen(key) + 1), &result);
	printf("Result : %u\n", code);
	if (code == ir::ec::ok) printf("Data : %s\n", (const char*)result.data());
	bool testok = (rightcode == ir::ec::ok) ?
		(code == ir::ec::ok && strcmp((const char*)result.data(), rightdata) == 0) :
		(code == rightcode);
	printf("Test: %s\n\n", testok ? "ok" : "error");
}

int main()
{
	ir::ec code = ir::ec::ok;
	database = new ir::S2STDatabase(SS("database"), ir::S2STDatabase::create_mode::neww, &code);
	//database->set_ram_mode(true, true);
	if (code == ir::ec::ok)
	{
		printf("S2STDatabase initialized\n");
		test_insert("Rarity", "Sweety Belle", ir::Database::insert_mode::always, ir::ec::ok);
		test_insert("Rainbow Dash", "Scootaloo", ir::Database::insert_mode::always, ir::ec::ok);
		test_insert("Celestia", "Luna", ir::Database::insert_mode::not_existing, ir::ec::ok);
		test_insert("Rainbow Dash", "Applebloom", ir::Database::insert_mode::not_existing, ir::ec::key_already_exists);
		test_delete("Celestia", ir::Database::delete_mode::existing, ir::ec::ok);
		test_read("Rarity", "Sweety Belle", ir::ec::ok);
		test_read("Celestia", nullptr, ir::ec::key_not_exists);
		test_insert("Celestia", "", ir::Database::insert_mode::not_existing, ir::ec::ok);
		test_read("Celestia", "", ir::ec::ok);
		test_insert("Rarity", "Applejack", ir::Database::insert_mode::not_existing, ir::ec::key_already_exists);
		test_insert("Rarity", "Applejack", ir::Database::insert_mode::existing, ir::ec::ok);
		test_insert("Rarity", "Applejack", ir::Database::insert_mode::always, ir::ec::ok);
	}
	delete database;
	getchar();
}