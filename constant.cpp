#include"constant.h"
//===================================================================
//  �萔�֌W
//===================================================================
bool cn::isSea(int h){ return h <= SEA_HEIGHT; }
bool cn::isLand(int h){ return SEA_HEIGHT < h; }
