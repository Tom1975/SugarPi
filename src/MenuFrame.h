#pragma once

#include "BasicFrame.h"

#ifdef  __circle__
#include <circle/logger.h>
#else
#include "CLogger.h"
#endif

class MenuFrame : public BasicFrame
{
public:
   MenuFrame();
   virtual ~MenuFrame();

   virtual unsigned int AttributesHasChanged();
   virtual void Init(int width, int height, int nb_buffers = 1);

   void Draw ();
   void Refresh ();

private:

};