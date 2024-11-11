#pragma once

#include <iostream>
#include <string>
#include "glad/gl.h"

//Set up our debug printing tools
#ifdef _DEBUG
#define DEBUG_PRINT(x) std::cout << x;
#define DEBUG_PRINTLN(x) std::cout << x << std::endl;
#define DEBUG_PRINT_GL_ERRORS(x) printGLErrors(x)
#define DEBUG_WAIT_FOR_CONSOLE_INPUT() char debugWaitForConsoleInputChar = getchar()
#else
#define DEBUG_PRINT(x)
#define DEBUG_PRINTLN(x)
#define DEBUG_PRINT_GL_ERRORS(x)
#define DEBUG_WAIT_FOR_CONSOLE_INPUT()
#endif

inline void printGLErrors(std::string location)
{
#ifdef _DEBUG
	GLenum flag = glGetError();
	if (flag != GL_NO_ERROR)
	{
		DEBUG_PRINT("GL error detected at: " + location + " | Error code: ");
		switch (flag)
		{
		case GL_INVALID_ENUM:
			DEBUG_PRINTLN("GL_INVALID_ENUM");
			break;
		case GL_INVALID_VALUE:
			DEBUG_PRINTLN("GL_INVALID_VALUE");
			break;
		case GL_INVALID_OPERATION:
			DEBUG_PRINTLN("GL_INVALID_OPERATION");
			break;
		case GL_INVALID_FRAMEBUFFER_OPERATION:
			DEBUG_PRINTLN("GL_INVALID_FRAMEBUFFER_OPERATION");
			break;
		case GL_OUT_OF_MEMORY:
			DEBUG_PRINTLN("GL_OUT_OF_MEMORY");
			break;
		case GL_STACK_UNDERFLOW:
			DEBUG_PRINTLN("GL_STACK_UNDERFLOW");
			break;
		case GL_STACK_OVERFLOW:
			DEBUG_PRINTLN("GL_STACK_OVERFLOW");
			break;
		default:
			DEBUG_PRINTLN("UNKNOWN ERROR CODE");
			break;
		}
	}
#endif
}