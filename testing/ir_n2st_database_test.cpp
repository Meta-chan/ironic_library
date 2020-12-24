#define IR_IMPLEMENT
#include <ir_database/ir_n2st_database.h>
#include <stdio.h>
#include <string.h>

ir::N2STDatabase *database;

void test_insert(unsigned int key, const char *data, ir::Database::insert_mode mode, ir::ec rightcode)
{
	printf("Adding key = '%i', data = '%s'\n", key, data);
	ir::ConstBlock bdata(data, strlen(data) + 1);
	ir::ec code = database->insert(key, bdata, mode);
	printf("Errorcode : %u\n", code);
	printf("Test: %s\n\n", code == rightcode ? "ok" : "error");
}

void test_delete(unsigned int key, ir::Database::delete_mode mode, ir::ec rightcode)
{
	printf("Deleting key = '%i'\n", key);
	ir::ec code = database->delet(key, mode);
	printf("Result : %u\n", code);
	printf("Test: %s\n\n", code == rightcode ? "ok" : "error");
}

void test_read(unsigned int key, const char *rightdata, ir::ec rightcode)
{
	printf("Reading key = '%i'\n", key);
	ir::ConstBlock result;
	ir::ec code = database->read(key, &result);
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
	database = new ir::N2STDatabase(SS("database.idt"), ir::Database::create_mode::neww, &code);
	database->set_ram_mode(true, true);

	if (code == ir::ec::ok)
	{
		printf("N2STDatabase initialized\n");
		test_insert(1, "Sweety Belle", ir::Database::insert_mode::always, ir::ec::ok);
		test_insert(2, "Luna", ir::Database::insert_mode::not_existing, ir::ec::ok);
		char longstr[300]; for (unsigned int i = 0; i < 299; i++) longstr[i] = '0'; longstr[299] = '\0';
		test_insert(2, longstr, ir::Database::insert_mode::always, ir::ec::ok);
		test_insert(8, "Applebloom", ir::Database::insert_mode::not_existing, ir::ec::ok);
		test_read(2, longstr, ir::ec::ok);
		test_read(3, nullptr, ir::ec::key_not_exists);
		test_insert(1, "", ir::Database::insert_mode::not_existing, ir::ec::key_already_exists);
		test_read(4, "", ir::ec::key_not_exists);
		test_insert(7, "Applejack", ir::Database::insert_mode::existing, ir::ec::key_not_exists);
		test_insert(7, "Applejack", ir::Database::insert_mode::not_existing, ir::ec::ok);
		test_insert(7, "Applejack", ir::Database::insert_mode::not_existing, ir::ec::key_already_exists);
	}
	delete database;
	getchar();
}