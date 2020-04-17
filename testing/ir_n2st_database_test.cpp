#define IR_IMPLEMENT
#include "ir_n2st_database.h"
#include <stdio.h>
#include <string.h>

ir::N2STDatabase *database;

void test_insert(unsigned int key, const char *data, ir::N2STDatabase::insertmode mode, ir::ec rightcode)
{
	printf("Adding key = '%i', data = '%s'\n", key, data);
	ir::ConstBlock bdata(strlen(data) + 1, data);
	ir::ec code = database->insert(key, bdata, mode);
	printf("Errorcode : %u\n", code);
	printf("Test: %s\n\n", code == rightcode ? "ok" : "error");
};

void test_delete(unsigned int key, ir::N2STDatabase::deletemode mode, ir::ec rightcode)
{
	printf("Deleting key = '%i'\n", key);
	ir::ec code = database->delet(key, mode);
	printf("Result : %u\n", code);
	printf("Test: %s\n\n", code == rightcode ? "ok" : "error");
};

void test_read(unsigned int key, const char *rightdata, ir::ec rightcode)
{
	printf("Reading key = '%i'\n", key);
	ir::ConstBlock result;
	ir::ec code = database->read(key, &result);
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
	database = new ir::N2STDatabase(SS("C:\\Users\\Dell E7240\\Desktop\\Dialog"),
		ir::N2STDatabase::accessmode::access_new,
		ir::N2STDatabase::rammode::rammode_no,
		&code);

	if (code == ir::ec::ec_ok)
	{
		printf("N2STDatabase initialized\n");
		test_insert(1, "Sweety Belle", ir::N2STDatabase::insert_always, ir::ec::ec_ok);
		test_insert(2, "Luna", ir::N2STDatabase::insert_not_existing, ir::ec::ec_ok);
		test_insert(2, "01234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789", ir::N2STDatabase::insert_always, ir::ec::ec_ok);
		test_insert(8, "Applebloom", ir::N2STDatabase::insert_not_existing, ir::ec::ec_key_already_exists);
		test_read(2, "Sweety Belle", ir::ec::ec_ok);
		test_read(3, nullptr, ir::ec::ec_key_not_exists);
		test_insert(1, "", ir::N2STDatabase::insert_not_existing, ir::ec::ec_ok);
		test_read(4, "", ir::ec::ec_ok);
		test_insert(7, "Applejack", ir::N2STDatabase::insert_not_existing, ir::ec::ec_key_already_exists);
		test_insert(7, "Applejack", ir::N2STDatabase::insert_existing, ir::ec::ec_ok);
		test_insert(7, "Applejack", ir::N2STDatabase::insert_always, ir::ec::ec_ok);
	}
	delete database;
};