#ifndef __REF_LINE_LAYER_H__
#define __REF_LINE_LAYER_H__

#include "cocos2d.h"
USING_NS_CC;

class RefLineLayer : public Layer {
public:
    virtual bool init() override;

    CREATE_FUNC(RefLineLayer);
};

#endif