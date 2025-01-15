#pragma once

class PiBitmap
{
public:
   virtual void DrawLogo(int line, int* buffer) = 0;
   virtual void GetSize(int& width, int& height) = 0;
};