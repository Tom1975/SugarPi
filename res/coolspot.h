#pragma once

class CoolspotFont
{
public:
   CoolspotFont(unsigned int stride);
   virtual ~CoolspotFont();

   void CopyLetter(unsigned char c, int line, int* buffer);
   void CopyLetter(unsigned char c, int* buffer, int stride);

   int GetLetterLength(unsigned char c);
   int GetLetterHeight(unsigned char c);

   void Write(unsigned char c, int* buffer);

protected:
   void Write_21(int* buffer);
   void Write_22(int* buffer);
   void Write_23(int* buffer);
   void Write_24(int* buffer);
   void Write_25(int* buffer);
   void Write_26(int* buffer);
   void Write_27(int* buffer);
   void Write_28(int* buffer);
   void Write_29(int* buffer);
   void Write_2a(int* buffer);
   void Write_2b(int* buffer);
   void Write_2c(int* buffer);
   void Write_2d(int* buffer);
   void Write_2e(int* buffer);
   void Write_2f(int* buffer);
   void Write_30(int* buffer);
   void Write_31(int* buffer);
   void Write_32(int* buffer);
   void Write_33(int* buffer);
   void Write_34(int* buffer);
   void Write_35(int* buffer);
   void Write_36(int* buffer);
   void Write_37(int* buffer);
   void Write_38(int* buffer);
   void Write_39(int* buffer);
   void Write_3a(int* buffer);
   void Write_3b(int* buffer);
   void Write_3c(int* buffer);
   void Write_3d(int* buffer);
   void Write_3e(int* buffer);
   void Write_3f(int* buffer);
   void Write_40(int* buffer);
   void Write_41(int* buffer);
   void Write_42(int* buffer);
   void Write_43(int* buffer);
   void Write_44(int* buffer);
   void Write_45(int* buffer);
   void Write_46(int* buffer);
   void Write_47(int* buffer);
   void Write_48(int* buffer);
   void Write_49(int* buffer);
   void Write_4a(int* buffer);
   void Write_4b(int* buffer);
   void Write_4c(int* buffer);
   void Write_4d(int* buffer);
   void Write_4e(int* buffer);
   void Write_4f(int* buffer);
   void Write_50(int* buffer);
   void Write_51(int* buffer);
   void Write_52(int* buffer);
   void Write_53(int* buffer);
   void Write_54(int* buffer);
   void Write_55(int* buffer);
   void Write_56(int* buffer);
   void Write_57(int* buffer);
   void Write_58(int* buffer);
   void Write_59(int* buffer);
   void Write_5a(int* buffer);
   void Write_5b(int* buffer);
   void Write_5c(int* buffer);
   void Write_5d(int* buffer);
   void Write_5e(int* buffer);
   void Write_5f(int* buffer);
   void Write_60(int* buffer);
   void Write_61(int* buffer);
   void Write_62(int* buffer);
   void Write_63(int* buffer);
   void Write_64(int* buffer);
   void Write_65(int* buffer);
   void Write_66(int* buffer);
   void Write_67(int* buffer);
   void Write_68(int* buffer);
   void Write_69(int* buffer);
   void Write_6a(int* buffer);
   void Write_6b(int* buffer);
   void Write_6c(int* buffer);
   void Write_6d(int* buffer);
   void Write_6e(int* buffer);
   void Write_6f(int* buffer);
   void Write_70(int* buffer);
   void Write_71(int* buffer);
   void Write_72(int* buffer);
   void Write_73(int* buffer);
   void Write_74(int* buffer);
   void Write_75(int* buffer);
   void Write_76(int* buffer);
   void Write_77(int* buffer);
   void Write_78(int* buffer);
   void Write_79(int* buffer);
   void Write_7a(int* buffer);
   void Write_7b(int* buffer);
   void Write_7c(int* buffer);
   void Write_7d(int* buffer);
   void Write_7e(int* buffer);
   void Write_7f(int* buffer);

protected:
   unsigned int stride_;
   typedef void(CoolspotFont::*write_letter) (int*);
   write_letter write_[0x100];

   int char_position_[256];



};