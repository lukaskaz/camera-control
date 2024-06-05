#include "gtest/gtest.h"

class TestCommon : public testing::Test
{
  public:
    uint32_t getNormal() const
    {
        return value;
    }

    uint32_t getInverted() const
    {
        return ~value;
    }

    const uint32_t value = 5;
};

TEST_F(TestCommon, IsNormalValueCorrect)
{
    EXPECT_EQ(getNormal(), value);
}

TEST_F(TestCommon, IsInvertedValueCorrect)
{
    ASSERT_NE(getInverted(), value);
    EXPECT_EQ(getInverted(), ~value);
}
