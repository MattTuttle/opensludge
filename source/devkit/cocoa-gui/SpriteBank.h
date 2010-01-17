//
//  SpriteBank.h
//  Sludge Dev Kit
//
//  Created by Rikard Peterson on 2009-07-18.
//  Copyright 2009 __MyCompanyName__. All rights reserved.
//

#import <Cocoa/Cocoa.h>

#ifdef __linux__
#include <GL/gl.h>
#else
#include <OpenGL/gl.h>
#endif

#include "sprites.h"

@interface SpriteOpenGLView : NSOpenGLView
{
	id doc;
	struct spriteBank *sprites;
	int spriteIndex;
	bool showBox;
	int x, y, w, h;
	float z;
}
- (void) connectToDoc: (id) myDoc;
- (void) drawRect: (NSRect) bounds ;
- (int) spriteIndex;
- (void) setSpriteIndex: (int)i;
@end


@interface SpriteBank : NSDocument {
	
	IBOutlet SpriteOpenGLView *spriteView;
	IBOutlet NSSlider *spriteIndexSlider;
	
	IBOutlet NSMatrix *palMode;
	
	IBOutlet NSButton *insertButton;
	IBOutlet NSButton *deleteButton;
	IBOutlet NSButton *exportButton;
	IBOutlet NSButton *replaceButton;
				
	struct spriteBank sprites;
	
	int hotSpotX, hotSpotY;
}
- (struct spriteBank *) getSprites;

- (IBAction)hotSpotCentre:(id)sender;
- (IBAction)hotSpotBase:(id)sender;

- (IBAction)setModePalOpen:(id)sender;
- (IBAction)setModePalClosed:(id)sender;
- (IBAction)setModePalNone:(id)sender;
- (IBAction)insertSprite:(id)sender;
- (IBAction)replaceSprite:(id)sender;
- (IBAction)deleteSprite:(id)sender;
- (IBAction)exportSprite:(id)sender;

@end
