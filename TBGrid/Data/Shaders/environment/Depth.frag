#version 330

//Shader to generate a depth map

precision highp float;

void main()
{
	
//	if (gl_FragCoord.x >= 512)
//	{
//		gl_FragDepth = 0.5;
//	}
//	else
//	{
//		gl_FragDepth = gl_FragCoord.z;
//	}
//In theory, this should give me a depth map of uniform 0.5, but I seem to be getting uniform 1.0
	//gl_FragDepth = 0.5;
	//Empty shader automatically sets depth buffer values
}