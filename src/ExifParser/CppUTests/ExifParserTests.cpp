#define private public
#define protected public

#include "../ExifParser.hpp"

#include "CppUTest/TestHarness.h"

TEST_GROUP(ExifTests)
{
   cExifParser *pTestParser;

   void setup()
   {
      pTestParser = new cExifParser();
   }

   void teardown()
   {
      delete pTestParser;
   }
};

///////////////////////////////////////////////////////////////////////////////
TEST(ExifTests, ReadTwoBytes_LittleEndian)
{
   cApp0 TestApp;
   TestApp.mLittleEndian = true;
   std::vector<uint8_t> TestVector{0x8D, 0x00};
   CHECK_EQUAL(0x8D, TestApp.ReadTwoBytes(TestVector.begin()));
   TestVector.at(1) = 0x9A;
   CHECK_EQUAL(0x9A8D, TestApp.ReadTwoBytes(TestVector.begin()));
}

TEST(ExifTests, ReadTwoBytes_BigEndian)
{
   cApp0 TestApp;
   TestApp.mLittleEndian = false;
   std::vector<uint8_t> TestVector{0x8D, 0x00};
   CHECK_EQUAL(0x8D00, TestApp.ReadTwoBytes(TestVector.begin()));
   TestVector.at(1) = 0x9A;
   CHECK_EQUAL(0x8D9A, TestApp.ReadTwoBytes(TestVector.begin()));
}

///////////////////////////////////////////////////////////////////////////////
TEST(ExifTests, ReadFourBytes_LittleEndian)
{
   cApp0 TestApp;
   TestApp.mLittleEndian = true;
   std::vector<uint8_t> TestVector{0x8D, 0x9D, 0xAD, 0xBD};
   CHECK_EQUAL(0xBDAD9D8D, TestApp.ReadFourBytes(TestVector.begin()));
}

TEST(ExifTests, ReadFourBytes_BigEndian)
{
   cApp0 TestApp;
   TestApp.mLittleEndian = false;
   std::vector<uint8_t> TestVector{0x8D, 0x9D, 0xAD, 0xBD};
   CHECK_EQUAL(0x8D9DADBD, TestApp.ReadFourBytes(TestVector.begin()));
}

///////////////////////////////////////////////////////////////////////////////

TEST(ExifTests, StartOfImageFound)
{
   std::vector<uint8_t> TestVector{0xFF, 0xD8, 0xFD, 0xD7};
   CHECK_TRUE(pTestParser->DoesStartOfImageExist(TestVector.begin()));
}

TEST(ExifTests, StartOfImageNotFound)
{
   std::vector<uint8_t> TestVector{0xFF, 0xD7, 0xFD, 0xD7};
   CHECK_FALSE(pTestParser->DoesStartOfImageExist(TestVector.begin()));

   TestVector.at(0) = 0xF8;
   CHECK_FALSE(pTestParser->DoesStartOfImageExist(TestVector.begin()));

   TestVector.at(1) = 0xD6;
   CHECK_FALSE(pTestParser->DoesStartOfImageExist(TestVector.begin()));
}

///////////////////////////////////////////////////////////////////////////////

TEST(ExifTests, DoesAppMarkerExist_AppMarkerFound)
{
   std::vector<uint8_t> TestVector{0xFF, 0xD8, 0xFF, 0xE0};
   std::vector<uint8_t>::iterator TestIter = TestVector.begin();
   std::advance(TestIter, cExifParser::SOI_MARKER_LENGTH_BYTES);
   CHECK_TRUE(pTestParser->App0.DoesAppMarkerExist(TestIter, cApp0::MARKER_NUMBER));

   TestVector.at(3) = 0xE1;
   CHECK_TRUE(pTestParser->App1.DoesAppMarkerExist(TestIter, cApp1::MARKER_NUMBER));
}

TEST(ExifTests, DoesAppMarkerExist_AppMarkerNotFound)
{
   std::vector<uint8_t> TestVector{0xFF, 0xD8, 0xFF, 0xE1};
   std::vector<uint8_t>::iterator TestIter = TestVector.begin();
   std::advance(TestIter, cExifParser::SOI_MARKER_LENGTH_BYTES);
   CHECK_FALSE(pTestParser->App0.DoesAppMarkerExist(TestIter, cApp0::MARKER_NUMBER));
}

///////////////////////////////////////////////////////////////////////////////

TEST(ExifTests, App0_ParseApp)
{
   std::vector<uint8_t> TestVector{0x00, 0x10};
   cApp0 mApp0;
   uint32_t BytesRead = mApp0.ParseApp(TestVector.begin());
   CHECK_EQUAL(0x10, BytesRead);
}

///////////////////////////////////////////////////////////////////////////////

TEST(ExifTests, App1_GetEndianess_LittleEndian)
{
   std::vector<uint8_t> TestVector{0x49, 0x49};
   cApp1 TestApp;
   CHECK_TRUE(TestApp.GetEndianess(TestVector.begin()));
   CHECK_TRUE(TestApp.mLittleEndian);
}

TEST(ExifTests, App1_GetEndianess_BigEndian)
{
   std::vector<uint8_t> TestVector{0x4D, 0x4D};
   cApp1 TestApp;
   CHECK_TRUE(TestApp.GetEndianess(TestVector.begin()));
   CHECK_FALSE(TestApp.mLittleEndian);
}

TEST(ExifTests, App1_GetEndianess_UnknownEndian)
{
   std::vector<uint8_t> TestVector{0x4F, 0x4D};
   cApp1 TestApp;
   CHECK_FALSE(TestApp.GetEndianess(TestVector.begin()));
   CHECK_FALSE(TestApp.mLittleEndian);
}

///////////////////////////////////////////////////////////////////////////////

TEST(ExifTests, App1_VerifyExifHeader_ValidHeader)
{
   std::string TestExifHeader = "Exif";
   std::vector<uint8_t> TestExifVector;
   for (char ch : TestExifHeader)
   {
      TestExifVector.push_back(static_cast<uint8_t>(ch));
   }
   TestExifVector.push_back(0x0);
   TestExifVector.push_back(0x0);

   cApp1 TestApp;
   std::vector<uint8_t>::iterator TestAppIter = TestExifVector.begin();
   CHECK_TRUE(TestApp.VerifyExifHeader(TestAppIter));
}

TEST(ExifTests, App1_VerifyExifHeader_ExifNotInHeader)
{
   std::string TestExifHeader = "EXIF"; // Only the E should be capitalized
   std::vector<uint8_t> TestExifVector;
   for (char ch : TestExifHeader)
   {
      TestExifVector.push_back(static_cast<uint8_t>(ch));
   }
   TestExifVector.push_back(0x0);
   TestExifVector.push_back(0x0);

   cApp1 TestApp;
   std::vector<uint8_t>::iterator TestAppIter = TestExifVector.begin();
   CHECK_FALSE(TestApp.VerifyExifHeader(TestAppIter));
}

TEST(ExifTests, App1_VerifyExifHeader_BlankBytesNotInHeader)
{
   std::string TestExifHeader = "EXIF";
   std::vector<uint8_t> TestExifVector;
   for (char ch : TestExifHeader)
   {
      TestExifVector.push_back(static_cast<uint8_t>(ch));
   }
   TestExifVector.push_back(0x0);

   cApp1 TestApp;
   std::vector<uint8_t>::iterator TestAppIter = TestExifVector.begin();
   CHECK_FALSE(TestApp.VerifyExifHeader(TestAppIter));
}

///////////////////////////////////////////////////////////////////////////////

TEST(ExifTests, App1_ParseDateTime_ExpectedResult)
{
   std::string TestDateString = "2023:04:29 19:38:33";
   std::vector<uint8_t> TestDateVector;
   for (char ch : TestDateString)
   {
      TestDateVector.push_back(static_cast<uint8_t>(ch));
   }
   TestDateVector.push_back(0x0);

   cApp1 TestApp;
   std::vector<uint8_t>::iterator TestAppIter = TestDateVector.begin();
   TestApp.ParseDateTime(TestAppIter, TestDateVector.size());
   CHECK_EQUAL(123, TestApp.mDateTime.tm_year); // Expecting 2023 - 1900
   CHECK_EQUAL(3, TestApp.mDateTime.tm_mon);
   CHECK_EQUAL(29, TestApp.mDateTime.tm_mday);
}

TEST(ExifTests, App1_ParseDateTime_CountLessThanExpected)
{
   std::string TestDateString = "2023:04:29 19:38:33";
   std::vector<uint8_t> TestDateVector;
   for (char ch : TestDateString)
   {
      TestDateVector.push_back(static_cast<uint8_t>(ch));
   }

   cApp1 TestApp;
   std::vector<uint8_t>::iterator TestAppIter = TestDateVector.begin();
   TestApp.ParseDateTime(TestAppIter, TestDateString.length());
   CHECK_EQUAL(0, TestApp.mDateTime.tm_year);
   CHECK_EQUAL(0, TestApp.mDateTime.tm_mon);
   CHECK_EQUAL(0, TestApp.mDateTime.tm_mday);
}

TEST(ExifTests, App1_ParseDateTime_CountGreaterThanExpected)
{
   std::string TestDateString = "2023:04:29 19:38:33:22 ";
   std::vector<uint8_t> TestDateVector;
   for (char ch : TestDateString)
   {
      TestDateVector.push_back(static_cast<uint8_t>(ch));
   }

   cApp1 TestApp;
   std::vector<uint8_t>::iterator TestAppIter = TestDateVector.begin();
   TestApp.ParseDateTime(TestAppIter, TestDateString.length());
   CHECK_EQUAL(0, TestApp.mDateTime.tm_year);
   CHECK_EQUAL(0, TestApp.mDateTime.tm_mon);
   CHECK_EQUAL(0, TestApp.mDateTime.tm_mday);
}

TEST(ExifTests, App1_ParseDateTime_FinalByteIsNotBlank)
{
   std::string TestDateString = "2023:04:29 19:38:33:";
   std::vector<uint8_t> TestDateVector;
   for (char ch : TestDateString)
   {
      TestDateVector.push_back(static_cast<uint8_t>(ch));
   }

   cApp1 TestApp;
   std::vector<uint8_t>::iterator TestAppIter = TestDateVector.begin();
   TestApp.ParseDateTime(TestAppIter, TestDateString.length());
   CHECK_EQUAL(0, TestApp.mDateTime.tm_year);
   CHECK_EQUAL(0, TestApp.mDateTime.tm_mon);
   CHECK_EQUAL(0, TestApp.mDateTime.tm_mday);
}