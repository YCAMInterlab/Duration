
//sourced from here:
// http://stackoverflow.com/questions/6690743/glut-window-menu-bar-in-mac-osx
//
#include "ofxRemoveCocoaMenu.h"
#import <Cocoa/Cocoa.h>

void removeCocoaMenuFromGlut(){
#ifdef TARGET_OSX
	if (NSApp){
		NSMenu      *menu;
		NSMenuItem  *menuItem;
		NSAutoreleasePool* p = [[NSAutoreleasePool alloc] init];
//		NSLog(@"%@",[[NSApp mainMenu] description]);
//
		[NSApp setMainMenu:[[NSMenu alloc] init]];
		NSMenu* existingMenu = [NSApp mainMenu];
		
		menu = [[NSMenu alloc] initWithTitle:@""];
		[menu addItemWithTitle:@"About..." action:@selector(orderFrontStandardAboutPanel:) keyEquivalent:@""];
		[menu addItemWithTitle:@"Quit Duration" action:@selector(terminate:) keyEquivalent:@"q"];
		
		menuItem = [[NSMenuItem alloc] initWithTitle:@"Apple" action:nil keyEquivalent:@""];
		[menuItem setSubmenu:menu];
		[[NSApp mainMenu] addItem:menuItem];
		[NSApp performSelector:@selector(setAppleMenu:) withObject:menu];
		
		[p release];
	}
#endif
}

