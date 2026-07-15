#include "TextUtils.h"

#include <gtest/gtest.h>

#include <string>

TEST(RemoveLineEndingTest, RemovesUnixNewline)
{
    std::string text = "Hello\n";

    text_utils::removeLineEnding(text);

    EXPECT_EQ(text, "Hello");
}

TEST(RemoveLineEndingTest, RemovesWindowsLineEnding)
{
    std::string text = "Hello\r\n";

    text_utils::removeLineEnding(text);

    EXPECT_EQ(text, "Hello");
}

TEST(RemoveLineEndingTest, LeavesTextWithoutLineEndingUnchanged)
{
    std::string text = "Hello";

    text_utils::removeLineEnding(text);

    EXPECT_EQ(text, "Hello");
}

TEST(RemoveLineEndingTest, HandlesEmptyString)
{
    std::string text;

    text_utils::removeLineEnding(text);

    EXPECT_TRUE(text.empty());
}

TEST(RemoveLineEndingTest, RemovesMultipleTrailingLineEndings)
{
    std::string text = "Hello\n\r\n";

    text_utils::removeLineEnding(text);

    EXPECT_EQ(text, "Hello");
}

TEST(RemoveLineEndingTest, PreservesInternalNewlines)
{
    std::string text = "Hello\nWorld\n";

    text_utils::removeLineEnding(text);

    EXPECT_EQ(text, "Hello\nWorld");
}