#include <stdio.h>
#define IR_IMPLEMENT
#include "ir_s2st_database.h"

ir::S2STDatabase *database;

void test_insert(const char *key, const char *data, ir::S2STDatabase::insertmode mode, ir::ec rightcode)
{
	printf("Adding key = '%s', data = '%s'\n", key, data);
	ir::ConstBlock bkey(strlen(key) + 1, key);
	ir::ConstBlock bdata(strlen(data) + 1, data);
	ir::ec code = database->insert(bkey, bdata, mode);
	printf("Errorcode : %u\n", code);
	printf("Test: %s\n\n", code == rightcode ? "ok" : "error");
};

void test_delete(const char *key, ir::S2STDatabase::deletemode mode, ir::ec rightcode)
{
	printf("Deleting key = '%s'\n", key);
	ir::ec code = database->delet(ir::ConstBlock(strlen(key) + 1, key), mode);
	printf("Result : %u\n", code);
	printf("Test: %s\n\n", code == rightcode ? "ok" : "error");
};

void test_read(const char *key, const char *rightdata, ir::ec rightcode)
{
	printf("Reading key = '%s'\n", key);
	ir::ConstBlock result;
	ir::ec code = database->read(ir::ConstBlock(strlen(key) + 1, key), &result);
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
	ir::ec code;
	database = new ir::S2STDatabase(L"database.idt", ir::S2STDatabase::create_new_always, &code);
	database->setrammode(true, true);
	if (code == ir::ec::ec_ok)
	{
		printf("S2STDatabase initialized\n");
		test_insert("Rarity", "Sweety Belle", ir::S2STDatabase::insert_always, ir::ec::ec_ok);
		test_insert("Rainbow Dash", "Scootaloo", ir::S2STDatabase::insert_always, ir::ec::ec_ok);
		test_insert("Celestia", "Luna", ir::S2STDatabase::insert_not_existing, ir::ec::ec_ok);
		test_insert("Rainbow Dash", "Applebloom", ir::S2STDatabase::insert_not_existing, ir::ec::ec_key_already_exists);
		test_delete("Celestia", ir::S2STDatabase::delete_existing, ir::ec::ec_ok);
		test_read("Rarity", "Sweety Belle", ir::ec::ec_ok);
		test_read("Celestia", nullptr, ir::ec::ec_key_not_exists);
		test_insert("Celestia", "", ir::S2STDatabase::insert_not_existing, ir::ec::ec_ok);
		test_read("Celestia", "", ir::ec::ec_ok);
		test_insert("Rarity", "Applejack", ir::S2STDatabase::insert_not_existing, ir::ec::ec_key_already_exists);
		test_insert("Rarity", "Applejack", ir::S2STDatabase::insert_existing, ir::ec::ec_ok);
		test_insert("Rarity", "Applejack", ir::S2STDatabase::insert_always, ir::ec::ec_ok);
	}
	delete database;
	getchar();
};