/*
** Copyright (c) 2019 ZhangQuan
**
** This program is free software; you can redistribute it and/or
** modify it under the terms of the GNU General Public
** License as published by the Free Software Foundation; either
** version 3 of the License, or (at your option) any later version.
**
** Author contact information:
**   zhangqhn@foxmail.com
**   长沙巨松软件科技有限公司
**   http://www.mybase.vip
*/

#include "string_tool.h"
#include <cctype>
#include <memory>


/*
** This table maps from the first byte of a UTF-8 character to the number
** of trailing bytes expected. A value '255' indicates that the table key
** is not a legal first byte for a UTF-8 character.
*/
static const uint8_t xtra_utf8_bytes[256] = {
  /* 0xxxxxxx */
  0, 0, 0, 0, 0, 0, 0, 0,     0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0,     0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0,     0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0,     0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0,     0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0,     0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0,     0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0,     0, 0, 0, 0, 0, 0, 0, 0,

  /* 10wwwwww */
  255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255,
  255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255,
  255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255,
  255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255,

  /* 110yyyyy */
  1, 1, 1, 1, 1, 1, 1, 1,     1, 1, 1, 1, 1, 1, 1, 1,
  1, 1, 1, 1, 1, 1, 1, 1,     1, 1, 1, 1, 1, 1, 1, 1,

  /* 1110zzzz */
  2, 2, 2, 2, 2, 2, 2, 2,     2, 2, 2, 2, 2, 2, 2, 2,

  /* 11110yyy */
  3, 3, 3, 3, 3, 3, 3, 3,     255, 255, 255, 255, 255, 255, 255, 255,
};

#define SKIP_UTF8(zIn) { zIn += (xtra_utf8_bytes[*(uint8_t*)zIn] + 1); }

static const uint64_t crc64_tab[256] = {
  UINT64_C(0x0000000000000000), UINT64_C(0x7ad870c830358979),
  UINT64_C(0xf5b0e190606b12f2), UINT64_C(0x8f689158505e9b8b),
  UINT64_C(0xc038e5739841b68f), UINT64_C(0xbae095bba8743ff6),
  UINT64_C(0x358804e3f82aa47d), UINT64_C(0x4f50742bc81f2d04),
  UINT64_C(0xab28ecb46814fe75), UINT64_C(0xd1f09c7c5821770c),
  UINT64_C(0x5e980d24087fec87), UINT64_C(0x24407dec384a65fe),
  UINT64_C(0x6b1009c7f05548fa), UINT64_C(0x11c8790fc060c183),
  UINT64_C(0x9ea0e857903e5a08), UINT64_C(0xe478989fa00bd371),
  UINT64_C(0x7d08ff3b88be6f81), UINT64_C(0x07d08ff3b88be6f8),
  UINT64_C(0x88b81eabe8d57d73), UINT64_C(0xf2606e63d8e0f40a),
  UINT64_C(0xbd301a4810ffd90e), UINT64_C(0xc7e86a8020ca5077),
  UINT64_C(0x4880fbd87094cbfc), UINT64_C(0x32588b1040a14285),
  UINT64_C(0xd620138fe0aa91f4), UINT64_C(0xacf86347d09f188d),
  UINT64_C(0x2390f21f80c18306), UINT64_C(0x594882d7b0f40a7f),
  UINT64_C(0x1618f6fc78eb277b), UINT64_C(0x6cc0863448deae02),
  UINT64_C(0xe3a8176c18803589), UINT64_C(0x997067a428b5bcf0),
  UINT64_C(0xfa11fe77117cdf02), UINT64_C(0x80c98ebf2149567b),
  UINT64_C(0x0fa11fe77117cdf0), UINT64_C(0x75796f2f41224489),
  UINT64_C(0x3a291b04893d698d), UINT64_C(0x40f16bccb908e0f4),
  UINT64_C(0xcf99fa94e9567b7f), UINT64_C(0xb5418a5cd963f206),
  UINT64_C(0x513912c379682177), UINT64_C(0x2be1620b495da80e),
  UINT64_C(0xa489f35319033385), UINT64_C(0xde51839b2936bafc),
  UINT64_C(0x9101f7b0e12997f8), UINT64_C(0xebd98778d11c1e81),
  UINT64_C(0x64b116208142850a), UINT64_C(0x1e6966e8b1770c73),
  UINT64_C(0x8719014c99c2b083), UINT64_C(0xfdc17184a9f739fa),
  UINT64_C(0x72a9e0dcf9a9a271), UINT64_C(0x08719014c99c2b08),
  UINT64_C(0x4721e43f0183060c), UINT64_C(0x3df994f731b68f75),
  UINT64_C(0xb29105af61e814fe), UINT64_C(0xc849756751dd9d87),
  UINT64_C(0x2c31edf8f1d64ef6), UINT64_C(0x56e99d30c1e3c78f),
  UINT64_C(0xd9810c6891bd5c04), UINT64_C(0xa3597ca0a188d57d),
  UINT64_C(0xec09088b6997f879), UINT64_C(0x96d1784359a27100),
  UINT64_C(0x19b9e91b09fcea8b), UINT64_C(0x636199d339c963f2),
  UINT64_C(0xdf7adabd7a6e2d6f), UINT64_C(0xa5a2aa754a5ba416),
  UINT64_C(0x2aca3b2d1a053f9d), UINT64_C(0x50124be52a30b6e4),
  UINT64_C(0x1f423fcee22f9be0), UINT64_C(0x659a4f06d21a1299),
  UINT64_C(0xeaf2de5e82448912), UINT64_C(0x902aae96b271006b),
  UINT64_C(0x74523609127ad31a), UINT64_C(0x0e8a46c1224f5a63),
  UINT64_C(0x81e2d7997211c1e8), UINT64_C(0xfb3aa75142244891),
  UINT64_C(0xb46ad37a8a3b6595), UINT64_C(0xceb2a3b2ba0eecec),
  UINT64_C(0x41da32eaea507767), UINT64_C(0x3b024222da65fe1e),
  UINT64_C(0xa2722586f2d042ee), UINT64_C(0xd8aa554ec2e5cb97),
  UINT64_C(0x57c2c41692bb501c), UINT64_C(0x2d1ab4dea28ed965),
  UINT64_C(0x624ac0f56a91f461), UINT64_C(0x1892b03d5aa47d18),
  UINT64_C(0x97fa21650afae693), UINT64_C(0xed2251ad3acf6fea),
  UINT64_C(0x095ac9329ac4bc9b), UINT64_C(0x7382b9faaaf135e2),
  UINT64_C(0xfcea28a2faafae69), UINT64_C(0x8632586aca9a2710),
  UINT64_C(0xc9622c4102850a14), UINT64_C(0xb3ba5c8932b0836d),
  UINT64_C(0x3cd2cdd162ee18e6), UINT64_C(0x460abd1952db919f),
  UINT64_C(0x256b24ca6b12f26d), UINT64_C(0x5fb354025b277b14),
  UINT64_C(0xd0dbc55a0b79e09f), UINT64_C(0xaa03b5923b4c69e6),
  UINT64_C(0xe553c1b9f35344e2), UINT64_C(0x9f8bb171c366cd9b),
  UINT64_C(0x10e3202993385610), UINT64_C(0x6a3b50e1a30ddf69),
  UINT64_C(0x8e43c87e03060c18), UINT64_C(0xf49bb8b633338561),
  UINT64_C(0x7bf329ee636d1eea), UINT64_C(0x012b592653589793),
  UINT64_C(0x4e7b2d0d9b47ba97), UINT64_C(0x34a35dc5ab7233ee),
  UINT64_C(0xbbcbcc9dfb2ca865), UINT64_C(0xc113bc55cb19211c),
  UINT64_C(0x5863dbf1e3ac9dec), UINT64_C(0x22bbab39d3991495),
  UINT64_C(0xadd33a6183c78f1e), UINT64_C(0xd70b4aa9b3f20667),
  UINT64_C(0x985b3e827bed2b63), UINT64_C(0xe2834e4a4bd8a21a),
  UINT64_C(0x6debdf121b863991), UINT64_C(0x1733afda2bb3b0e8),
  UINT64_C(0xf34b37458bb86399), UINT64_C(0x8993478dbb8deae0),
  UINT64_C(0x06fbd6d5ebd3716b), UINT64_C(0x7c23a61ddbe6f812),
  UINT64_C(0x3373d23613f9d516), UINT64_C(0x49aba2fe23cc5c6f),
  UINT64_C(0xc6c333a67392c7e4), UINT64_C(0xbc1b436e43a74e9d),
  UINT64_C(0x95ac9329ac4bc9b5), UINT64_C(0xef74e3e19c7e40cc),
  UINT64_C(0x601c72b9cc20db47), UINT64_C(0x1ac40271fc15523e),
  UINT64_C(0x5594765a340a7f3a), UINT64_C(0x2f4c0692043ff643),
  UINT64_C(0xa02497ca54616dc8), UINT64_C(0xdafce7026454e4b1),
  UINT64_C(0x3e847f9dc45f37c0), UINT64_C(0x445c0f55f46abeb9),
  UINT64_C(0xcb349e0da4342532), UINT64_C(0xb1eceec59401ac4b),
  UINT64_C(0xfebc9aee5c1e814f), UINT64_C(0x8464ea266c2b0836),
  UINT64_C(0x0b0c7b7e3c7593bd), UINT64_C(0x71d40bb60c401ac4),
  UINT64_C(0xe8a46c1224f5a634), UINT64_C(0x927c1cda14c02f4d),
  UINT64_C(0x1d148d82449eb4c6), UINT64_C(0x67ccfd4a74ab3dbf),
  UINT64_C(0x289c8961bcb410bb), UINT64_C(0x5244f9a98c8199c2),
  UINT64_C(0xdd2c68f1dcdf0249), UINT64_C(0xa7f41839ecea8b30),
  UINT64_C(0x438c80a64ce15841), UINT64_C(0x3954f06e7cd4d138),
  UINT64_C(0xb63c61362c8a4ab3), UINT64_C(0xcce411fe1cbfc3ca),
  UINT64_C(0x83b465d5d4a0eece), UINT64_C(0xf96c151de49567b7),
  UINT64_C(0x76048445b4cbfc3c), UINT64_C(0x0cdcf48d84fe7545),
  UINT64_C(0x6fbd6d5ebd3716b7), UINT64_C(0x15651d968d029fce),
  UINT64_C(0x9a0d8ccedd5c0445), UINT64_C(0xe0d5fc06ed698d3c),
  UINT64_C(0xaf85882d2576a038), UINT64_C(0xd55df8e515432941),
  UINT64_C(0x5a3569bd451db2ca), UINT64_C(0x20ed197575283bb3),
  UINT64_C(0xc49581ead523e8c2), UINT64_C(0xbe4df122e51661bb),
  UINT64_C(0x3125607ab548fa30), UINT64_C(0x4bfd10b2857d7349),
  UINT64_C(0x04ad64994d625e4d), UINT64_C(0x7e7514517d57d734),
  UINT64_C(0xf11d85092d094cbf), UINT64_C(0x8bc5f5c11d3cc5c6),
  UINT64_C(0x12b5926535897936), UINT64_C(0x686de2ad05bcf04f),
  UINT64_C(0xe70573f555e26bc4), UINT64_C(0x9ddd033d65d7e2bd),
  UINT64_C(0xd28d7716adc8cfb9), UINT64_C(0xa85507de9dfd46c0),
  UINT64_C(0x273d9686cda3dd4b), UINT64_C(0x5de5e64efd965432),
  UINT64_C(0xb99d7ed15d9d8743), UINT64_C(0xc3450e196da80e3a),
  UINT64_C(0x4c2d9f413df695b1), UINT64_C(0x36f5ef890dc31cc8),
  UINT64_C(0x79a59ba2c5dc31cc), UINT64_C(0x037deb6af5e9b8b5),
  UINT64_C(0x8c157a32a5b7233e), UINT64_C(0xf6cd0afa9582aa47),
  UINT64_C(0x4ad64994d625e4da), UINT64_C(0x300e395ce6106da3),
  UINT64_C(0xbf66a804b64ef628), UINT64_C(0xc5bed8cc867b7f51),
  UINT64_C(0x8aeeace74e645255), UINT64_C(0xf036dc2f7e51db2c),
  UINT64_C(0x7f5e4d772e0f40a7), UINT64_C(0x05863dbf1e3ac9de),
  UINT64_C(0xe1fea520be311aaf), UINT64_C(0x9b26d5e88e0493d6),
  UINT64_C(0x144e44b0de5a085d), UINT64_C(0x6e963478ee6f8124),
  UINT64_C(0x21c640532670ac20), UINT64_C(0x5b1e309b16452559),
  UINT64_C(0xd476a1c3461bbed2), UINT64_C(0xaeaed10b762e37ab),
  UINT64_C(0x37deb6af5e9b8b5b), UINT64_C(0x4d06c6676eae0222),
  UINT64_C(0xc26e573f3ef099a9), UINT64_C(0xb8b627f70ec510d0),
  UINT64_C(0xf7e653dcc6da3dd4), UINT64_C(0x8d3e2314f6efb4ad),
  UINT64_C(0x0256b24ca6b12f26), UINT64_C(0x788ec2849684a65f),
  UINT64_C(0x9cf65a1b368f752e), UINT64_C(0xe62e2ad306bafc57),
  UINT64_C(0x6946bb8b56e467dc), UINT64_C(0x139ecb4366d1eea5),
  UINT64_C(0x5ccebf68aecec3a1), UINT64_C(0x2616cfa09efb4ad8),
  UINT64_C(0xa97e5ef8cea5d153), UINT64_C(0xd3a62e30fe90582a),
  UINT64_C(0xb0c7b7e3c7593bd8), UINT64_C(0xca1fc72bf76cb2a1),
  UINT64_C(0x45775673a732292a), UINT64_C(0x3faf26bb9707a053),
  UINT64_C(0x70ff52905f188d57), UINT64_C(0x0a2722586f2d042e),
  UINT64_C(0x854fb3003f739fa5), UINT64_C(0xff97c3c80f4616dc),
  UINT64_C(0x1bef5b57af4dc5ad), UINT64_C(0x61372b9f9f784cd4),
  UINT64_C(0xee5fbac7cf26d75f), UINT64_C(0x9487ca0fff135e26),
  UINT64_C(0xdbd7be24370c7322), UINT64_C(0xa10fceec0739fa5b),
  UINT64_C(0x2e675fb4576761d0), UINT64_C(0x54bf2f7c6752e8a9),
  UINT64_C(0xcdcf48d84fe75459), UINT64_C(0xb71738107fd2dd20),
  UINT64_C(0x387fa9482f8c46ab), UINT64_C(0x42a7d9801fb9cfd2),
  UINT64_C(0x0df7adabd7a6e2d6), UINT64_C(0x772fdd63e7936baf),
  UINT64_C(0xf8474c3bb7cdf024), UINT64_C(0x829f3cf387f8795d),
  UINT64_C(0x66e7a46c27f3aa2c), UINT64_C(0x1c3fd4a417c62355),
  UINT64_C(0x935745fc4798b8de), UINT64_C(0xe98f353477ad31a7),
  UINT64_C(0xa6df411fbfb21ca3), UINT64_C(0xdc0731d78f8795da),
  UINT64_C(0x536fa08fdfd90e51), UINT64_C(0x29b7d047efec8728),
};


const uint8_t upper_to_lower_char[256] = {
  0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08, 0x09, 0x0A, 0x0B, 0x0C, 0x0D, 0x0E, 0x0F,
  0x10, 0x11, 0x12, 0x13, 0x14, 0x15, 0x16, 0x17, 0x18, 0x19, 0x1A, 0x1B, 0x1C, 0x1D, 0x1E, 0x1F,
  0x20, 0x21, 0x22, 0x23, 0x24, 0x25, 0x26, 0x27, 0x28, 0x29, 0x2A, 0x2B, 0x2C, 0x2D, 0x2E, 0x2F,
  0x30, 0x31, 0x32, 0x33, 0x34, 0x35, 0x36, 0x37, 0x38, 0x39, 0x3A, 0x3B, 0x3C, 0x3D, 0x3E, 0x3F,
  0x40, 0x61, 0x62, 0x63, 0x64, 0x65, 0x66, 0x67, 0x68, 0x69, 0x6A, 0x6B, 0x6C, 0x6D, 0x6E, 0x6F,
  0x70, 0x71, 0x72, 0x73, 0x74, 0x75, 0x76, 0x77, 0x78, 0x79, 0x7A, 0x5B, 0x5C, 0x5D, 0x5E, 0x5F,
  0x60, 0x61, 0x62, 0x63, 0x64, 0x65, 0x66, 0x67, 0x68, 0x69, 0x6A, 0x6B, 0x6C, 0x6D, 0x6E, 0x6F,
  0x70, 0x71, 0x72, 0x73, 0x74, 0x75, 0x76, 0x77, 0x78, 0x79, 0x7A, 0x7B, 0x7C, 0x7D, 0x7E, 0x7F,
  0x80, 0x81, 0x82, 0x83, 0x84, 0x85, 0x86, 0x87, 0x88, 0x89, 0x8A, 0x8B, 0x8C, 0x8D, 0x8E, 0x8F,
  0x90, 0x91, 0x92, 0x93, 0x94, 0x95, 0x96, 0x97, 0x98, 0x99, 0x9A, 0x9B, 0x9C, 0x9D, 0x9E, 0x9F,
  0xA0, 0xA1, 0xA2, 0xA3, 0xA4, 0xA5, 0xA6, 0xA7, 0xA8, 0xA9, 0xAA, 0xAB, 0xAC, 0xAD, 0xAE, 0xAF,
  0xB0, 0xB1, 0xB2, 0xB3, 0xB4, 0xB5, 0xB6, 0xB7, 0xB8, 0xB9, 0xBA, 0xBB, 0xBC, 0xBD, 0xBE, 0xBF,
  0xC0, 0xC1, 0xC2, 0xC3, 0xC4, 0xC5, 0xC6, 0xC7, 0xC8, 0xC9, 0xCA, 0xCB, 0xCC, 0xCD, 0xCE, 0xCF,
  0xD0, 0xD1, 0xD2, 0xD3, 0xD4, 0xD5, 0xD6, 0xD7, 0xD8, 0xD9, 0xDA, 0xDB, 0xDC, 0xDD, 0xDE, 0xDF,
  0xE0, 0xE1, 0xE2, 0xE3, 0xE4, 0xE5, 0xE6, 0xE7, 0xE8, 0xE9, 0xEA, 0xEB, 0xEC, 0xED, 0xEE, 0xEF,
  0xF0, 0xF1, 0xF2, 0xF3, 0xF4, 0xF5, 0xF6, 0xF7, 0xF8, 0xF9, 0xFA, 0xFB, 0xFC, 0xFD, 0xFE, 0xFF,
};

uint64_t StringTool::Crc64(const char* pStr)
{
  const uint8_t* pTmp = (const uint8_t*)pStr;
  uint64_t crc = 0;

  while (*pTmp != '\0')
  {
    crc = crc64_tab[(uint8_t)(crc ^ (*pTmp))] ^ (crc >> 8);

    pTmp++;
  }

  return crc;
}

uint64_t StringTool::Crc64(const char* pStr, size_t len)
{
  const uint8_t* pTmp = (const uint8_t*)pStr;
  uint64_t crc = 0;

  while (len > 0)
  {
    crc = crc64_tab[(uint8_t)(crc ^ (*pTmp))] ^ (crc >> 8);

    len--;
    pTmp++;
  }

  return crc;
}

uint64_t StringTool::Crc64NoCase(const char* pStr)
{
  const uint8_t* pTmp = (const uint8_t*)pStr;
  uint64_t crc = 0;
  uint8_t byteCode = 0;

  while (*pTmp != '\0')
  {
    byteCode = upper_to_lower_char[(uint8_t)(*pTmp)];
    crc = crc64_tab[(uint8_t)(crc ^ byteCode)] ^ (crc >> 8);

    pTmp++;
  }

  return crc;
}

uint64_t StringTool::Crc64NoCase(const unsigned char* pStr, size_t len)
{
  const uint8_t* pTmp = (const uint8_t*)pStr;
  uint64_t crc = 0;
  uint8_t byteCode = 0;

  while (len > 0)
  {
    byteCode = upper_to_lower_char[(uint8_t)(*pTmp)];
    crc = crc64_tab[(uint8_t)(crc ^ byteCode)] ^ (crc >> 8);

    len--;
    pTmp++;
  }

  return crc;
}

bool StringTool::Utf8LikeCompare(const char* zPattern, const char* zString, size_t len)
{
  uint32_t c = 0;
  int32_t c2;

  const char* pEnd = zString + len;

  while ((zString < pEnd) && ((c = upper_to_lower_char[(uint8_t)*zPattern]) != 0))
  {
    switch (c)
    {
    case '%':
    {
      while ((c = zPattern[1]) == '%' || c == '_')
      {
        if (c == '_')
        {
          if (*zString == 0 || zString >= pEnd)
            return false;
          SKIP_UTF8(zString);
        }
        zPattern++;
      }

      if (c == 0)
        return true;

      c = upper_to_lower_char[(uint8_t)c];
      while (zString < pEnd && ((c2 = upper_to_lower_char[(uint8_t)*zString]) != 0))
      {
        while (c2 != 0 && c2 != c)
        {
          zString++;
          c2 = upper_to_lower_char[(uint8_t)*zString];
        }
        if (c2 == 0)
          return false;
        if (StringTool::Utf8LikeCompare(&zPattern[1], zString, (int32_t)(pEnd - zString)))
          return true;
        SKIP_UTF8(zString);
      }
      return false;
    }
    case '_':
    {
      if (zString >= pEnd || *zString == 0)
        return false;
      SKIP_UTF8(zString);
      zPattern++;
      break;
    }
    default:
    {
      if (c != upper_to_lower_char[(uint8_t)*zString])
        return false;
      zPattern++;
      zString++;
      break;
    }
    }
  }

  return (*zString == 0 || zString == pEnd) && (*zPattern == 0);
}

bool StringTool::StrToInt64(const char* pStr, size_t len, int64_t* pVal)
{
  if (pStr == nullptr || pVal == nullptr)
    return false;

  int64_t val = 0;

  bool symbolFlg = true;
  if (*pStr == '-')
  {
    symbolFlg = false;
    len--;
    pStr++;
  }
  else if (*pStr == '+')
  {
    len--;
    pStr++;
  }

  size_t i = 0;
  int c = pStr[0];
  for (i = 0; i < len && c >= '0' && c <= '9'; i++)
  {
    val = val * 10 + c - '0';
    c = pStr[i + 1];
  }
  if (i != len)
    return false;

  *pVal = symbolFlg ? val : -val;

  if (i > 0 &&
    (i < 19 || (i == 19 && memcmp(pStr, "9223372036854775807", 19) <= 0)))
  {
    return true;
  }

  return false;
}

bool StringTool::StrToDouble(const char* pStr, size_t len, double* pVal)
{
  bool symbolFlg = true;
  double v1 = 0.0;
  const char* pTmp = pStr;

  if (pVal == nullptr)
  {
    return false;
  }

  if (*pTmp == '-')
  {
    symbolFlg = false;
    pTmp++;
  }
  else if (*pTmp == '+')
  {
    symbolFlg = true;
    pTmp++;
  }

  while (isdigit(*pTmp)) {
    v1 = v1*10.0 + (*pTmp - '0');
    pTmp++;
  }
  if (*pTmp == '.')
  {
    double divisor = 1.0;
    pTmp++;
    while (isdigit(*pTmp))
    {
      v1 = v1 * 10.0 + (*pTmp - '0');
      divisor *= 10.0;
      pTmp++;
    }
    v1 /= divisor;
  }
  if (*pTmp == 'e' || *pTmp == 'E')
  {
    int esign = 1;
    int eval = 0;
    double scale = 1.0;
    pTmp++;
    if (*pTmp == '-')
    {
      esign = -1;
      pTmp++;
    }
    else if (*pTmp == '+')
    {
      pTmp++;
    }

    while (isdigit(*pTmp))
    {
      eval = eval * 10 + *pTmp - '0';
      pTmp++;
    }

    while (eval >= 64) { scale *= 1.0e+64; eval -= 64; }
    while (eval >= 16) { scale *= 1.0e+16; eval -= 16; }
    while (eval >= 4) { scale *= 1.0e+4; eval -= 16; }
    while (eval >= 1) { scale *= 1.0e+1; eval -= 1; }
    if (esign < 0)
    {
      v1 /= scale;
    }
    else
    {
      v1 *= scale;
    }
  }

  if (pTmp - pStr != len)
  {
    return false;
  }

  if (symbolFlg)
  {
    *pVal = v1;
  }
  else
  {
    *pVal = -v1;
  }

  return true;
}
