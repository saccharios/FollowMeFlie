#include "gtest/gtest.h"
#include "control/double_buffer.h"
class DoubleBufferTest : public ::testing::Test
{
public:
private:
};

TEST_F(DoubleBufferTest, CheckInitializationReal)
{

    using DBR_float = Double_Buffer <float> ;

    // use inplace ctor, so we can set the memory beforehand
    DBR_float dbr;
    void * f1 = &dbr;
    memset(f1, 0xABABABAB, sizeof(DBR_float));
    DBR_float *test_dbr = new (f1) DBR_float();

    EXPECT_FLOAT_EQ (0.0f, test_dbr->read());
    EXPECT_FLOAT_EQ (0.0f, test_dbr->write());
}

TEST_F(DoubleBufferTest, Swap)
{
    Double_Buffer <int> db;
    EXPECT_EQ (0, db.read());

    db.write() = 10;
    db.swap();
    EXPECT_EQ (10, db.read());
    db.swap();
    EXPECT_EQ (0, db.read());
}

