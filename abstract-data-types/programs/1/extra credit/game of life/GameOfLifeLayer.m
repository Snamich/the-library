//
//  GameOfLifeLayer.m
//  game of life
//
//  Created by Scott Chatham on 1/20/13.
//  Copyright 2013 __MyCompanyName__. All rights reserved.
//

#import "GameOfLifeLayer.h"

#define CELLSIZE 16


@implementation GameOfLifeLayer

@synthesize isRunning;

+ (CCScene *) scene
{
	// 'scene' is an autorelease object.
	CCScene *scene = [CCScene node];
	
	// 'layer' is an autorelease object.
	GameOfLifeLayer *layer = [GameOfLifeLayer node];
	
	// add layer as a child to scene
	[scene addChild: layer];
	
	// return the scene
	return scene;
}

- (id) init
{
    if ( self == [super init] ) {
        CGSize winSize = [[CCDirector sharedDirector] winSize];
        // create menu
        [CCMenuItemFont setFontSize:30];
        
        CCMenuItemFont *resetButton = [CCMenuItemFont itemWithString:@"Reset" target:self selector:@selector(pressedReset)];
        CCMenuItemFont *runButton = [CCMenuItemFont itemWithString:@"Run" target:self selector:@selector(pressedRun)];
        CCMenuItemFont *stepButton = [CCMenuItemFont itemWithString:@"Step" target:self selector:@selector(pressedStep)];
        
        CCMenu *menu = [CCMenu menuWithItems:resetButton, runButton, stepButton, nil];
        menu.position = CGPointMake( winSize.width/2, 30 );
        [self addChild:menu];
        [menu alignItemsHorizontallyWithPadding:100];
        
        // set sprite cache
        [[CCSpriteFrameCache sharedSpriteFrameCache] addSpriteFramesWithFile:@"cells.plist"];

        // create cells
        CGPoint initialPosition = CGPointMake( 0, winSize.height - CELLSIZE );
        int numColumns = winSize.width / CELLSIZE;
        int numRows = (winSize.height - (menu.position.y + 40)) / CELLSIZE;

        NSMutableArray *tempCells = [[NSMutableArray alloc] init];
        CGPoint currentPosition = initialPosition;
        for ( int i = 0, ind = 0; i < numRows; ++i ) {
            for ( int j = 0; j < numColumns; ++j, ++ind ) {
                CCSprite *currentSprite = [[CCSprite alloc] initWithSpriteFrameName:@"deadCell.png"];
                currentSprite.anchorPoint = ccp( 0, 0 );
                currentSprite.position = currentPosition;
                [tempCells insertObject:currentSprite atIndex:ind];
                [self addChild:currentSprite];
                currentPosition.x += CELLSIZE;
            }
            currentPosition.y -= CELLSIZE;
            currentPosition.x = 0;
        }
        self.displayObjects = [[NSArray alloc] initWithArray:tempCells];
        
        // set up state
        marker = 0;
        self.isRunning = FALSE;
        self.isTouchEnabled = YES;
        [self schedule:@selector(update:) interval:0.1];
    }
    return self;
}

- (void)printWorld
{
    for( CCSprite* item in self.displayObjects ) {
        CGPoint coords = [self coordinatesOfCell:item];
        if ( CellExists( &world[(int)coords.x][(int)coords.y], liveCells, &marker ) ) {
            [item setDisplayFrame:[[CCSpriteFrameCache sharedSpriteFrameCache] spriteFrameByName:@"liveCell.png"]];
        } else {
            [item setDisplayFrame:[[CCSpriteFrameCache sharedSpriteFrameCache] spriteFrameByName:@"deadCell.png"]];
        }
	}
}

- (void)ccTouchesEnded:(NSSet *)touches withEvent:(UIEvent *)event
{
    UITouch *touch = [touches anyObject];

    CCSprite *touchedSprite = [self itemForTouch:touch];
    if ( touchedSprite != nil ) {
        CGPoint coords = [self coordinatesOfCell:touchedSprite];
        if ( CellExists( &world[(int)coords.x][(int)coords.y], liveCells, &marker ) ) {
            DeleteCell( world[(int)coords.x][(int)coords.y], liveCells, &marker );
            [touchedSprite setDisplayFrame:[[CCSpriteFrameCache sharedSpriteFrameCache] spriteFrameByName:@"deadCell.png"]];
        } else {
            AddCell( &world[(int)coords.x][(int)coords.y], liveCells, &marker, 0, 1 );
            [touchedSprite setDisplayFrame:[[CCSpriteFrameCache sharedSpriteFrameCache] spriteFrameByName:@"liveCell.png"]];
        }
    }
}

- (CGPoint)coordinatesOfCell:(CCSprite*)sprite
{
    int maxY = 1008;
    int spriteX = (int)sprite.position.x, spriteY = (int)sprite.position.y;
    int x = spriteX / CELLSIZE, y = (maxY - spriteY) / CELLSIZE;
    
    return CGPointMake( y, x );
}

- (CCSprite *)itemForTouch:(UITouch *)touch;
{
	CGPoint touchLocation = [touch locationInView: [touch view]];
	touchLocation = [[CCDirector sharedDirector] convertToGL: touchLocation];
	for( CCSprite* item in self.displayObjects ) {
		CGPoint local = [item convertToNodeSpace:touchLocation];
        CGRect absoluteBox = CGRectMake(item.position.x, item.position.y, [item boundingBox].size.width, [item boundingBox].size.height);
		absoluteBox.origin = CGPointZero;
		if( CGRectContainsPoint( absoluteBox, local ) )
			return item;
	}
	return nil;
}

- (void) onExit
{
    [super onExit];
    
    [self unscheduleAllSelectors];
}

- (void) update:(ccTime)delta
{
    if ( isRunning ) {
        Update( world, liveCells, &marker );
        [self printWorld];
    }
}

- (void) pressedReset
{
    isRunning = FALSE;
    
    // clear out all cells
    for ( int i = 0; i < marker; ++i ) {
        Cell_destroy( liveCells[i] );
    }
    marker = 0;
    [self printWorld];
}

- (void) pressedRun
{
    if ( isRunning ) {
        isRunning = FALSE;
    } else {
        isRunning = TRUE;
    }
}

- (void) pressedStep
{
    Update( world, liveCells, &marker );
    [self printWorld];
}

- (void) dealloc
{
	[super dealloc];
}

@end