//
//  ScriptDocument.m
//  Sludge Dev Kit
//
//  Created by Rikard Peterson on 2009-08-23.
//  Copyright 2009-2011 SLUDGE Dev Team. All rights reserved.
//


#import "ScriptDocument.h"


@implementation ScriptDocument


- (NSString *)windowNibName {
    return @"ScriptDocument";
}

NSCharacterSet *whiteSpaceSet;

void addFunction (NSMutableDictionary *words, char *name) {
	[words setObject:[NSString stringWithUTF8String: name] forKey:[NSString stringWithUTF8String: name]];
}

- (void)awakeFromNib
{

	[text textStorage].delegate = self;
	
	// load our dictionary
	whiteSpaceSet = [NSCharacterSet whitespaceAndNewlineCharacterSet];
	
	// Built in functions
	builtinWords = [[NSMutableDictionary alloc] init];
#define FUNC(special,name) addFunction(builtinWords, #name);
#include "functionList.h"
#undef FUNC
	
	// Other keywords
	keywordsColour = [[NSDictionary alloc] initWithObjectsAndKeys: [NSColor colorWithCalibratedRed:0.5 
																							 green:0.0 
																							  blue:0.5
																							 alpha:1.0], NSForegroundColorAttributeName, nil];

	keyWords = [[NSMutableDictionary alloc] init];
	addFunction(keyWords, "sub");
	addFunction(keyWords, "return");
	addFunction(keyWords, "var");
	addFunction(keyWords, "unfreezable");
	addFunction(keyWords, "for");
	addFunction(keyWords, "loop");
	addFunction(keyWords, "while");
	addFunction(keyWords, "if");
	addFunction(keyWords, "else");
	addFunction(keyWords, "objectType");
	addFunction(keyWords, "event");
	addFunction(keyWords, "speechColour");
	addFunction(keyWords, "speechGap");
	addFunction(keyWords, "walkSpeed");
	addFunction(keyWords, "spinSpeed");
	addFunction(keyWords, "wrapSpeech");
	addFunction(keyWords, "flag");
	addFunction(keyWords, "flags");
	
}

-(void)	windowControllerDidLoadNib: (NSWindowController*)aController
{
    [super windowControllerDidLoadNib:aController];
		
	if( sourceCode != nil )
	{
		[text setString: sourceCode];
		[sourceCode release];
		sourceCode = nil;
	}
}	
	

-(NSData*)	dataRepresentationOfType: (NSString*)aType
{
    return [[text string] dataUsingEncoding: NSUTF8StringEncoding];
}

-(BOOL)	loadDataRepresentation: (NSData*)data ofType: (NSString*)aType
{
	if(sourceCode) {
		[sourceCode release];
		sourceCode = nil;
	}
	sourceCode = [[NSString alloc] initWithData:data encoding: NSUTF8StringEncoding];
	if (! sourceCode) {
		errorBox("Error opening file", "Is it an old file? I'll try opening it using the old Windows string encoding. If that works, it will be automatically converted to UTF-8. (Don't worry - that's what you want.)");
		sourceCode = [[NSString alloc] initWithData:data encoding: NSISOLatin1StringEncoding];
		[self updateChangeCount: NSChangeDone];	
	}
	
	return YES;
}


-(void) colourString:(NSString *) string 
			   range: (NSRange) area
			 storage: (NSTextStorage *) textStorage
{
    if (area.length == 0) return; // bail early
  
    NSRange end;
    unsigned int areamax = NSMaxRange(area);
    NSRange found;
    NSString *word;

    NSCharacterSet *whiteSpaceSet;
	
    // extend our range along word boundaries.
    whiteSpaceSet = [[NSCharacterSet alphanumericCharacterSet]invertedSet];
	
	
    // remove the old colors
    [textStorage removeAttribute:NSForegroundColorAttributeName range:area];
	
//	[text setSpellingState:nil range:area];

    // add new colors
    while (area.length) {
        // find the next word
        end = [string rangeOfCharacterFromSet:whiteSpaceSet
                                      options:0
                                        range:area];
        if (end.location == NSNotFound) {
            end = found = area;
        } else {
            found.length = end.location - area.location;
            found.location = area.location;
        }
		
		if ([[string substringWithRange:NSMakeRange(found.location, 1)] isEqualToString:@"#"]) {
			end = [string lineRangeForRange:NSMakeRange(found.location, 0)];
			if (end.location < found.location) {
				end.length -= found.location - end.location;
				end.location = found.location;
			}
			[textStorage addAttribute:NSForegroundColorAttributeName
								value:[NSColor colorWithCalibratedRed:0.2 
																green:0.4 
																 blue:0.0
																alpha:1.0]
								range:end];
		} else if ([[string substringWithRange:NSMakeRange(found.location, 1)] isEqualToString:@"\""]) {
			end = [string lineRangeForRange:NSMakeRange(found.location, 0)];
			if (end.location < found.location) {
				end.length -= found.location - end.location;
				end.location = found.location;
			}
			found.location = end.location;
			found.length = end.length;
			
			int tryAgain = TRUE;
			while (tryAgain) {
				tryAgain = FALSE;
				if (found.location < end.location + end.length) {
					found.length = end.location + end.length - found.location;
					found = [string rangeOfString: @"\""
										  options: nil
											range: NSMakeRange(found.location + 1, found.length-1)];
					if (found.location != NSNotFound) {
						if ([[string substringWithRange:NSMakeRange(found.location-1, 1)] isEqualToString:@"\\"])
							tryAgain = TRUE;
						else
							end.length = found.location + found.length - end.location;
					}
				}
			}
			
			//[text setSpellingState:(1 << 0) range:end];

			[textStorage addAttribute:NSForegroundColorAttributeName
								value:[NSColor colorWithCalibratedRed:0.6 
																green:0.0 
																 blue:0.2
																alpha:1.0]
								range:end];
		} else if ([[string substringWithRange:NSMakeRange(found.location, 1)] isEqualToString:@"'"]) {
			end = [string lineRangeForRange:NSMakeRange(found.location, 0)];
			if (end.location < found.location) {
				end.length -= found.location - end.location;
				end.location = found.location;
			}
			found.location = end.location;
			found.length = end.length;
			
			if (found.location < end.location + end.length) {
				found.length = end.location + end.length - found.location;
				found = [string rangeOfString: @"'"
									  options: nil
										range: NSMakeRange(found.location + 1, found.length-1)];
				if (found.location != NSNotFound) {
					end.length = found.location + found.length - end.location;
				}
			}
			[textStorage addAttribute:NSForegroundColorAttributeName
								value:[NSColor colorWithCalibratedRed:0.0 
																green:0.4 
																 blue:0.4
																alpha:1.0]
								range:end];
		} else {
			
			word = [string substringWithRange:found];
			
			// color as necessary
			if ([keyWords objectForKey:word] != NULL) {
				[textStorage addAttribute:NSForegroundColorAttributeName
									value:[NSColor colorWithCalibratedRed:0.5 
																	green:0.0 
																	 blue:0.5
																	alpha:1.0]
									range:found];
			} else if ([builtinWords objectForKey:word] != NULL) {
				[textStorage addAttribute:NSForegroundColorAttributeName
									value:[NSColor colorWithCalibratedRed:0.1 
																	green:0.1 
																	 blue:0.6
																	alpha:1.0]
									range:found];
			}
		}
        
        // adjust our area
        areamax = NSMaxRange(end);
        area.length -= areamax - area.location;
        area.location = areamax;
    }
}

- (void)textStorageDidProcessEditing:(NSNotification *)notification
{
    NSTextStorage *textStorage = [notification object];
    NSString *string = [textStorage string];
    NSRange area = [textStorage editedRange];
    	
    // extend our range along line boundaries.
    area = [string lineRangeForRange:area];
	
	[self colourString: string range:area storage: textStorage];	
}

- (bool)commentMenu
{
	NSString *string = [text string];
	NSMutableString *newstring = [NSMutableString stringWithCapacity:10];
	NSRange area = [text selectedRange];
	NSRange area2;
	
	int c=0;

	// extend our range along line boundaries.
    area = [string lineRangeForRange:area];
	[newstring setString: [string substringWithRange:area]];

	area2 = [newstring lineRangeForRange:NSMakeRange(0, 0)];

	while (area2.length) {
		if (NSOrderedSame == [newstring compare:@"#~" options:nil range:NSMakeRange(area2.location, 2)]) {
			[newstring deleteCharactersInRange: NSMakeRange(area2.location, 2)];
			area2 = [newstring lineRangeForRange:NSMakeRange(area2.location+area2.length-2, 0)];
			c-=2;
		} else {
			[newstring insertString:@"#~" atIndex:area2.location];
			area2 = [newstring lineRangeForRange:NSMakeRange(area2.location+area2.length+2, 0)];
			c+=2;
		}
	}
	
	if ([text shouldChangeTextInRange: area replacementString: newstring]) {
		[text replaceCharactersInRange: area withString: newstring];
		[text didChangeText];
		area.length+=c;
		[text setSelectedRange: area];
	}

	return true;
}

- (void)selectLine:(int)lineNumber
{
	if (lineNumber) {
		lineNumber--;
		int i, line=0;
		NSString *string = [text string];
		int len = [string length];
		for (i = 0; i < len; i++)
		{
			if ([string characterAtIndex:i] == '\n')
			{
				line++;
				if (line >= lineNumber) {	
					if (i<len) i++;
					break;
				}
			}
		}
		
		NSRange l = NSMakeRange(i, 0);
		l = [string lineRangeForRange:l];
		[text setSelectedRange: l];
		[text scrollRangeToVisible:l];
	}
}

@end
