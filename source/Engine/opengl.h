//
//  opengl.h
//  Sludge Engine
//
//  Created by Matt Tuttle on 10/22/15.
//  Copyright © 2015 SLUDGE. All rights reserved.
//

#ifndef OPENGL_H
#define OPENGL_H

#if !defined(HAVE_GLES2)
#include <OpenGL/gl.h>
#else
#include <GLES2/gl2.h>
#include "eglport/eglport.h"
#endif

#endif /* opengl_h */
