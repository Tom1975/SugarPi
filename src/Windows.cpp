//
#include "Windows.h"


Windows::Windows(Windows* parent) : x_(0), y_(0), width_(0), height_(0), parent_(parent), windows_children_(nullptr)
{
}

Windows::~Windows()
{
}

