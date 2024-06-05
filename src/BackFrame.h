#pragma once

#include "BasicFrame.h"

#ifdef  __circle__
#include <circle/logger.h>
#else
#include "CLogger.h"
#endif

class BackFrame : public BasicFrame
{
public:
   BackFrame();
   virtual ~BackFrame();

   void Init(int width, int height, int nb_buffers = 1);

   void Draw ();
   void Refresh ();

private:
   float value_;   
};