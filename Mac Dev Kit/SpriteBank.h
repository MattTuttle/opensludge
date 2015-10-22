//
//  SpriteBank.h
//  Sludge Dev Kit
//
//  Created by Rikard Peterson on 2009-07-18.
//  Copyright 2009 __MyCompanyName__. All rights reserved.
//


#include <OpenGL/gl.h>
#import <Cocoa/Cocoa.h>
#import "SLUDGE Document.h"
#include "sprites.h"

#define SPRITEBANK_FILE_TYPE @"com.hungrysoftware.sludge-spritebank"

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


@interface SpriteBank : SLUDGE_Document {
	
	IBOutlet SpriteOpenGLView *spriteView;
	IBOutlet NSSlider *spriteIndexSlider;
	
	IBOutlet NSMatrix *palMode;
	
	IBOutlet NSButton *insertButton;
	IBOutlet NSButton *deleteButton;
	IBOutlet NSButton *exportButton;
	IBOutlet NSButton *replaceButton;
				
	struct spriteBank sprites;
	
	int hotSpotX, hotSpotY;

	IBOutlet NSPanel *fontifyWindow;
	IBOutlet NSButton *fontifyOK;
	int fontifySpaceWidth;
	
	// For setting multiple hotspots
	IBOutlet NSPanel *rangeWindow;
	IBOutlet NSTextField *indexStart;
	IBOutlet NSTextField *indexEnd;	
}
- (struct spriteBank *) getSprites;

- (IBAction)hotSpotCentre:(id)sender;
- (IBAction)hotSpotBase:(id)sender;
- (IBAction)hotSpotRange:(id)sender;
- (IBAction)hotSpotRangeOk:(id)sender;
- (IBAction)hotSpotRangeCancel:(id)sender;

- (IBAction)setModePalOpen:(id)sender;
- (IBAction)setModePalClosed:(id)sender;
- (IBAction)setModePalNone:(id)sender;
- (IBAction)insertSprite:(id)sender;
- (IBAction)replaceSprite:(id)sender;
- (IBAction)deleteSprite:(id)sender;
- (IBAction)exportSprite:(id)sender;

-(void) fontifyMe;
- (IBAction)doFontify:(id)sender;

@end

