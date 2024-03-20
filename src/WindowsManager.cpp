//
#include "WindowsManager.h"

WindowsManager::WindowsManager(CLogger* logger) :
   logger_(logger)
{
   // Create setups
   // Test
   windows_structures_[Test] = new WindowStructure[1];
   window_structures_size_[Test] = 1;

   // background
   test_buffer_ = new unsigned char [1080*1920*4];
   windows_structures_[Test][0] = {
      /*.format_ = */      HVS_PIXEL_FORMAT_RGBA8888,
      /*.order_ = */       HVS_PIXEL_ORDER_RGBA,
      /*.x_ = */           1920/4,
      /*.y_ = */           1080/4,
      /*.h_ = */           1080/2,
      /*.w_ = */           1920/2,
      /*.pitch_ = */       1080/2 * 4,
      /*.nb_buffers_ = */  1,
      /*.buffer_ = */      &test_buffer_,
      /*.animation_ = */   nullptr,
      /*.main_ = */        true

   };
   

   // Menu
   windows_structures_[Menu] = new WindowStructure[3];
   window_structures_size_[Menu] = 3;

   // background
   windows_structures_[Menu][0] = {
      /*.format_ = */      HVS_PIXEL_FORMAT_RGBA8888,
      /*.order_ = */       HVS_PIXEL_ORDER_RGBA,
      /*.x_ = */           0,
      /*.y_ = */           0,
      /*.h_ = */           1080,
      /*.w_ = */           1920,
      /*.pitch_ = */       1080,
      /*.nb_buffers_ = */  1,
      /*.buffer_ = */      nullptr,
      /*.animation_ = */   nullptr,
      /*.main_ = */        false

   };

   // Menu
   windows_structures_[Menu][1] = {
      /*.format_ = */      HVS_PIXEL_FORMAT_RGBA8888,
      /*.order_ = */       HVS_PIXEL_ORDER_RGBA,
      /*.x_ = */           0,
      /*.y_ = */           200,
      /*.h_ = */           1080,
      /*.w_ = */           1920,
      /*.pitch_ = */       1080,
      /*.nb_buffers_ = */  1,
      /*.buffer_ = */      nullptr,
      /*.animation_ = */   nullptr,
      /*.main_ = */        false
   };

   // Logo
   windows_structures_[Menu][2] = {
      /*.format_ = */      HVS_PIXEL_FORMAT_RGBA8888,
      /*.order_ = */       HVS_PIXEL_ORDER_RGBA,
      /*.x_ = */           0,
      /*.y_ = */           0,
      /*.h_ = */           200,
      /*.w_ = */           1920,
      /*.pitch_ = */       1080,
      /*.nb_buffers_ = */  1,
      /*.buffer_ = */      nullptr,
      /*.animation_ = */   nullptr,
      /*.main_ = */        false
   };

   // Main
   windows_structures_[Main] = new WindowStructure[1];
   window_structures_size_[Main] = 1;

   // background
   windows_structures_[Main][0] = {
      /*.format_ = */      HVS_PIXEL_FORMAT_RGBA8888,
      /*.order_ = */       HVS_PIXEL_ORDER_RGBA,
      /*.x_ = */           0,
      /*.y_ = */           0,
      /*.h_ = */           1080,
      /*.w_ = */           1920,
      /*.pitch_ = */       1080,
      /*.nb_buffers_ = */  1,
      /*.buffer_ = */      nullptr,
      /*.animation_ = */   nullptr,
      /*.main_ = */        true

   };

}

WindowsManager::~WindowsManager()
{
}
