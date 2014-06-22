//
//  GameOfLifeLayer.h
//  game of life
//
//  Created by Scott Chatham on 1/20/13.
//  Copyright 2013 __MyCompanyName__. All rights reserved.
//

#import <Foundation/Foundation.h>
#import "cocos2d.h"
#import "gol.h"

@interface GameOfLifeLayer : CCLayer {
    struct Cell *liveCells[MAX_CELLS];
    int world[NUM_ROWS][NUM_COLUMNS];
    int marker;
}

@property (nonatomic,strong) NSArray *displayObjects;
@property (nonatomic, strong) CCTexture2D *liveCell;
@property (nonatomic, strong) CCTexture2D *deadCell;
@property (nonatomic) bool isRunning;

+ (CCScene*) scene;

@end
