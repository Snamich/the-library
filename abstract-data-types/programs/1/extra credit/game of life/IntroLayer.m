//
//  IntroLayer.m
//  game of life
//
//  Created by Scott Chatham on 1/20/13.
//  Copyright __MyCompanyName__ 2013. All rights reserved.
//


// Import the interfaces
#import "IntroLayer.h"
#import "GameOfLifeLayer.h"


#pragma mark - IntroLayer

// HelloWorldLayer implementation
@implementation IntroLayer

+ (CCScene *) scene
{
	// 'scene' is an autorelease object.
	CCScene *scene = [CCScene node];
	
	// 'layer' is an autorelease object.
	IntroLayer *layer = [IntroLayer node];
	
	// add layer as a child to scene
	[scene addChild: layer];
	
	// return the scene
	return scene;
}

-(void) onEnter
{
	[super onEnter];

	// ask director for the window size
	CGSize size = [[CCDirector sharedDirector] winSize];

	CCSprite *background;
	
	if( UI_USER_INTERFACE_IDIOM() == UIUserInterfaceIdiomPhone ) {
		background = [CCSprite spriteWithFile:@"Default.png"];
		background.rotation = 90;
	} else {
		background = [CCSprite spriteWithFile:@"gol.png"];
	}
	background.position = ccp(size.width/2, size.height/2);

	// add the label as a child to this Layer
	[self addChild: background];
	
	// In one second transition to the new scene
	[self scheduleOnce:@selector(makeTransition:) delay:0.5];
}

-(void) makeTransition:(ccTime)dt
{
	[[CCDirector sharedDirector] replaceScene:[GameOfLifeLayer scene]];
}
@end
